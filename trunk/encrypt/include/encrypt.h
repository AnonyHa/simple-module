#include <cstdlib>
#include <cstring>

class clsEncrypt {
	public:
		virtual char* Encrypt(char* Input,int InputLen,int& OutputLen)=0;
		virtual char* Decrypt(char* Input,int InputLen,int& OutputLen)=0;
};

#define DIC_SIZE 256
class SimpleXOR:public clsEncrypt{
	private:
		unsigned char dic_i;
		unsigned char* dic;
		bool SeedFlag;
		int _Seed;

	    unsigned char xmake(unsigned char c) {
			++dic_i;
			return dic[dic_i]^c;
		}
	public:
		SimpleXOR() {dic = new unsigned char[DIC_SIZE];SeedFlag=false;memset(dic, 0, DIC_SIZE);};
		~SimpleXOR() {delete dic;};

		void SetSeed(int Seed)
		{
			_Seed = Seed;
			dic_i = 0;
			for (int i=0; i<256; ++i) {
				dic[i] = (unsigned char)(Seed%256);
				Seed = (Seed<<3)+(Seed>>3)+Seed;
			} 

			SeedFlag = true;
		}

		int GetSeed()
		{
			return _Seed;
		}

	
		char* Encrypt(char* Input, int InputLen, int& OutputLen)
		{
			if (!SeedFlag) {
				OutputLen = 0;
				return 0;
			}

			char* Output = new char[InputLen];
			OutputLen = InputLen;
			for(int i=0; i<InputLen; i++)
			{
				Output[i] = (char)xmake((unsigned char)Input[i]);
			}
			return Output;
		}

		char* Decrypt(char* Input,int InputLen,int& OutputLen)
		{
			int StartLen = 0;
			if (!SeedFlag)
			{
				if(InputLen < 4)
				{
					//³ö´í
					OutputLen = 0;
					return 0;
				}
				else
				{
					int Seed = *((int *)Input);
					SetSeed(Seed);
					StartLen = sizeof(int);
				}
			}

			OutputLen = InputLen - StartLen;
			char* Output = new char[OutputLen];

			for (int i=StartLen;i<InputLen;i++)
			{
				Output[i-StartLen] = (char)xmake((unsigned char)Input[i]);	
			}

			return Output;
		}

		unsigned char* GetDic()
		{
			return dic;
		}
};

