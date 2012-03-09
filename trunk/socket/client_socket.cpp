#include "event.h"
#include "packet.h"

#include <iostream>
#include <string>

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define RECV_BLOCK_SIZE 1024

using namespace std;

void ClientOnRead(struct bufferevent * buf_ev, void * arg);
void ClientOnError(struct bufferevent * buf_ev, short error_no, void * arg);

bool SetNoNBlock(int fd)
{
	int flags;
	flags = fcntl(fd,F_GETFL);
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)	return false;
	return true;
}


class SocketError {
	private:
		string _ErrMsg;
		int _ErrNo;
		int _Vfd;
	public:
		SocketError(int Vfd,int ErrNo,string ErrMsg)
		{
			_Vfd = Vfd;
			_ErrNo = ErrNo;
			_ErrMsg = ErrMsg;
		};

		int GetErrNo() {return _ErrNo;};
		string GetErrMsg() {return _ErrMsg;};
		string GetMsg() {return _ErrMsg;};
		int GetVfd() {return _Vfd;};
		~SocketError() {};
};

class clsClient{
	private:
		//基本的连接信息
		string _Ip;
		int _Port;
		//内部的变量和状态
		int _Status; //0表示没有连接,1表示已经进行了连接
		int _Vfd;
		bufferevent* _BufEv;	
		PacketInterface* PacketObj;

		//内部的函数接口
		void SetVfd(int Vfd) {_Vfd = Vfd;};
		void SetStatus(int Status) {_Status = Status;};
	public:
		clsClient(string Ip, int Port):_Status(0),_Vfd(-1),PacketObj(NULL) {_Ip = Ip; _Port = Port;};
		~clsClient() {if(_Status) close(_Vfd);};
		void Connect();
		void Close();
		string GetIp() {return _Ip;};
		int GetPort() {return _Port;};
		int GetVfd() {return _Vfd;};
		void OnRead(struct bufferevent * buf_ev, void * arg);
		void OnError(struct bufferevent * buf_ev, short error_no, void * arg);
		string GetAimInfo() {return _Ip;};
		void RegistForPacket(PacketInterface* p){PacketObj = p;};
};


void clsClient::Connect()
{
	if(_Status) {throw SocketError(GetVfd(), 1, GetAimInfo() + "Has Client Connect");}	
	
	int ClientFd = socket(AF_INET, SOCK_STREAM, 0);
	if(ClientFd < 0) {throw SocketError(-1, 2, GetAimInfo() + "Create File Describtion error!");};

	struct sockaddr_in ClientAddr;
	memset(&ClientAddr, 0, sizeof(ClientAddr));
	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_addr.s_addr = inet_addr(GetIp().c_str());
	ClientAddr.sin_port = htons(GetPort());
	cout << "try to Connect Ip:" << GetIp() << " Port:"<< GetPort() <<endl;

	if(connect(ClientFd, (struct sockaddr *)&ClientAddr, sizeof(ClientAddr))<0) {
		throw SocketError(ClientFd, 2, GetAimInfo() + " Connect Failed");
	}

	if (!SetNoNBlock(ClientFd))
	{
		close(ClientFd);
		throw SocketError(ClientFd, 3, GetAimInfo() + "Set NonBlock Failed");
	}

	//成功创建连接
	SetVfd(ClientFd);
	SetStatus(1);

	_BufEv = bufferevent_new(_Vfd, ClientOnRead, NULL, ClientOnError, this);
	bufferevent_enable(_BufEv, EV_READ);
}

void clsClient::Close()
{
	if(!_Status) return;
	close(_Vfd);
}

void clsClient::OnRead(struct bufferevent * buf_ev, void * arg)
{
	int ReadLen = 0;
	char buf[RECV_BLOCK_SIZE];	

	while(1) {
		ReadLen = bufferevent_read(buf_ev, buf, sizeof(buf));
		if (ReadLen == 0) return;
		
		if (PacketObj) {PacketObj->PacketOnRead(buf, ReadLen);};
	}
}

void clsClient::OnError(struct bufferevent * buf_ev, short error_no, void * arg)
{
}

////////////////////////////////////////////////////////////////////////////////////////

void ClientOnRead(struct bufferevent * buf_ev, void * arg)
{
	clsClient* p = static_cast<clsClient *>(arg);
	p->OnRead(buf_ev, arg);
}

void ClientOnError(struct bufferevent * buf_ev, short error_no, void * arg)
{
	clsClient* p = static_cast<clsClient *>(arg);
	p->OnError(buf_ev, error_no, arg);
}


/************  测试用 *****************/
int main(void)
{
	event_init();

	SimpleOutPacket* PacketTest = new SimpleOutPacket();
	clsClient* Test = new clsClient(string("127.0.0.1"), 6666);
	Test->RegistForPacket(PacketTest);
	try{
		Test->Connect();
	}
	catch(SocketError& e)
	{
		cout << "Vfd:" << e.GetVfd() << "\tErrMsg:" << e.GetMsg() << endl;
	}

	while(1)
	{
		event_dispatch();
	}

	return 0;
}
