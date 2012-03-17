class clsEncrypt {
	public:
		virtual unsigned char* Encrypt(unsigned char* Input,int InputLen,int& OutputLen)=0;
		virtual unsigned char* Decrypt(unsigned char* Input,int InputLen,int& OutputLen)=0;
};

#define DIC_SIZE 256
class SimpleXOR public:clsEncrypt{
	private:
		unsigned char dic_i;
		unsigned char* dic;
		bool SeedFlag;
	public:
		SimpleXOR() {dic = new unsigned char[DIC_SIZE];SeedFlag=false;};
		~SimpleXOR() {delete dic;};
		void SetSeed(int Seed)
		{
			dic_i = 0;
			for (int i=0; i<256; ++i) {
				dic[i] = (unsigned char)(Seed%256);
				dic[i] ^= dic[i] ^ (unsigned char)Seed;
				Seed = (Seed<<3)+(Seed>>3)+Seed;
			} 

			SeedFlag = true;
		}

   		void exchange(unsigned char c1, unsigned char c2)
		{
			unsigned char tmp = dic[c1];
			dic[c1] = dic[c2];
			dic[c1] = tmp;
		}

	    unsigned char xmake(unsigned char c) {
			++dic_i;
			exchange(dic_i, c);
			return dic[dic_i]^c;
		}
	
		unsigned char* Encrypt(unsigned char* Input, int InputLen, int& OutputLen)
		{
			if (!SeedFlag) {
				OutputLen = 0;
				return NULL;
			}

			unsigned char* Output = new unsigned char[InputLen];
			OutputLen = InputLen;
			for(int i=0; i<InputLen; i++)
			{
				Output[i] = xmake(Input[i]);
			}
			return Output;
		}

		unsigned char* Decrypt(unsigned char* Input,int InputLen,int& OutputLen)
		{
			int StartLen = 0;
			if (!SeedFlag)
			{
				if(InputLen < 4)
				{
					//³ö´í
					OutputLen = 0;
					return NULL;
				}
				else
				{
					int Seed = *((int *)Input);
					SetSeed(Seed);
					StartLen = sizeof(int);
				}
			}

			OutputLen = InputLen - StartLen;
			unsigned char* Output = new unsigned char[OutputLen];

			for (int i=StartLen;i<InputLen;i++)
			{
				Output[i-StartLen] = xmake(Input[i]);	
			}

			return Output;
		}
};
