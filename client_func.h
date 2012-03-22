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
			_DecryptObj->DecryptPacket(Vfd, Buf, BufLen);
			return true;
		};

		bool PacketOnError(int Vfd)
		{
			cout << "Vfd="<<Vfd<<" is Closed!"<<endl;
			return true;
		};
		bool PacketOnConnect(int Vfd)
		{
			cout << Vfd << "Connect OK!"<<endl;
			char* TmpBuf="conn ok!";
			WriteData(Vfd, TmpBuf, strlen(TmpBuf));

			char* TmpBuf1="Try to Send two msg!";
			WriteData(Vfd, TmpBuf1, strlen(TmpBuf1));
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
