#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

#include <string>

#define RECV_BLOCK_SIZE 1024

using namespace std;

class clsClientSocket{
	private:
		//基本的连接信息
		string _Ip;
		int _Port;
		int _Vfd;
		PacketInterface* _PacketClass;

		//内部的函数接口
		void SetVfd(int Vfd) {_Vfd = Vfd;};
	public:
		clsClientSocket(string Ip, int Port, PacketInterface* PacketClass=0):_Vfd(-1) {_Ip = Ip; _Port = Port;_PacketClass=PacketClass;};
		~clsClientSocket() {close(_Vfd);};
		void Connect();
		void Close();
		string GetIp() {return _Ip;};
		int GetPort() {return _Port;};
		int GetVfd() {return _Vfd;};
		string GetAimInfo() {return _Ip;};
		PacketInterface* GetPacketInterface() {return _PacketClass;};
};

#endif
