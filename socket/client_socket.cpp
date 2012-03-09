#include <iostream>
#include <string>

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;

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

		//内部的函数接口
		void SetVfd(int Vfd) {_Vfd = Vfd;};
		void SetStatus(int Status) {_Status = Status;};
	public:
		clsClient(string Ip, int Port):_Status(0),_Vfd(-1) {_Ip = Ip; _Port = Port;};
		~clsClient() {if(_Status) close(_Vfd);};
		void Connect();
		void Close();
		string GetIp() {return _Ip;};
		int GetPort() {return _Port;};
		int GetVfd() {return _Vfd;};

		string GetAimInfo() {return _Ip;};
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
}

void clsClient::Close()
{
	if(!_Status) return;
	close(_Vfd);
}


int main(void)
{
	clsClient* Test = new clsClient(string("192.168.10.48"), 7735);
	try{
		Test->Connect();
	}
	catch(SocketError& e)
	{
		cout << "Vfd:" << e.GetVfd() << "\tErrMsg:" << e.GetMsg() << endl;
	}
	
	return 0;
}
