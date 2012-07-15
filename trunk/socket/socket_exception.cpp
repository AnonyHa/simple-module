#include "socket_exception.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

const char* ErrNo2Msg[] = {
	"Create Server Socket Err",
	"Create Client Socket Err",
	"Connect to Server Err",
	"Read Data Err",
	"Write Data Err",
	"Clear Data Err",
	"Server Create Peer Conn Err",
};

SocketError::SocketError(int Vfd,int ErrNo,char* ErrMsg)
{
	_Vfd = Vfd;
	_ErrNo = ErrNo;
	_ErrMsg = ErrMsg;
};

SocketError::SocketError(int Vfd,int ErrNo,string ErrMsg)
{
	_Vfd = Vfd;
	_ErrNo = ErrNo;
	_ErrMsg = (char*)(ErrMsg.c_str());
};
	
char* SocketError::GetMsg() {
	char* RetMsg = (char *)malloc(MAX_ERR_MSG);
	sprintf(RetMsg, "Vfd=%d,%s:%s", _Vfd, ErrNo2Msg[_ErrNo], _ErrMsg);
	return RetMsg;
};
