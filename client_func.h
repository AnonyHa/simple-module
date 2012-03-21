#ifndef __CLIENT_FUNC_H__
#define __CLIENT_FUNC_H__

#include "packet_interface.h"
#include "packet_process.h"
#include "encrypt.h"

#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

class SimplePacketFunc:public PacketInterface{
	private:
		clsPacketEncrypt* _EncryptObj;
		clsPacketDecrypt* _DecryptObj;
	public:
		SimplePacketFunc()
		{
			SimpleXOR* encrypt = new SimpleXOR();
			SimpleXOR* decrypt = new SimpleXOR();
			_EncryptObj = new clsPacketEncrypt(encrypt);
			_DecryptObj = new clsPacketDecrypt(decrypt);
		}

		bool PacketOnRead(int Vfd, char* Buf, int BufLen)
		{
			cout << "Vfd="<<Vfd<<"Get Msg:";
			_DecryptObj->DecryptPacket(Buf, BufLen);
			return true;
		};

		bool PacketOnError(int Vfd)
		{
			cout << "Vfd="<<Vfd<<" is Closed!"<<endl;
			return true;
		};
		bool PacketOnConnect(int Vfd)
		{
			cout << "Connect OK!"<<Vfd<<endl;
			char* TmpBuf="conn ok!";
			WriteData(Vfd, TmpBuf, strlen(TmpBuf));
			return true;
		};
		int WriteData(int Vfd, char* Buf,int BufLen)
		{
			int OutLen;
			char* Output;
			Output = _EncryptObj->EncryptPacket(Buf, BufLen, OutLen);	
			cout << "Send Data Len="<<OutLen<<endl;
			if (OutLen>0) PacketOnWrite(Vfd, Output, OutLen);
			return OutLen;
		};
};

#endif
