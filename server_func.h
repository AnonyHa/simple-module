#ifndef __SERVER_FUNC_H__
#define __SERVER_FUNC_H__

#include "packet_interface.h"
#include "packet_process.h"
#include "encrypt.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

class SimpleServerFunc:public PacketInterface{
	private:
		clsPacketEncrypt* _EncryptObj;
		clsPacketDecrypt* _DecryptObj;

	public:
		SimpleServerFunc()
		{
			SimpleXOR* encrypt = new SimpleXOR();
			SimpleXOR* decrypt = new SimpleXOR();
			_EncryptObj = new clsPacketEncrypt(encrypt);
			_DecryptObj = new clsPacketDecrypt(decrypt);
		}

		bool PacketOnRead(int Vfd, char * Buf, int BufLen)
		{
			_DecryptObj->DecryptPacket(Vfd, Buf, BufLen);
			return true;
		};

		bool PacketOnError(int Vfd)
		{
			cout<<"Close Vfd="<<Vfd<<endl;
			return true;
		};
		bool PacketOnConnect(int Vfd)
		{
			char* Buf="Hello World";
			WriteData(Vfd, Buf, strlen(Buf));

			char* Buf1="another Msg!";
			WriteData(Vfd, Buf1, strlen(Buf1));
			return true;
		};

		int WriteData(int Vfd, char* Buf,int BufLen)
		{
			int OutLen;
			char* Output;
			Output = _EncryptObj->EncryptPacket(Vfd, Buf, BufLen, OutLen);	
			if (OutLen>0) PacketOnWrite(Vfd, Output, OutLen);
			return OutLen;
		};
};

#endif
