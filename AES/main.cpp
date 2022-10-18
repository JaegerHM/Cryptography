#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <tgmath.h>
#include "AES.h"

using namespace std;

typedef struct 
{
	short int bfType;
	int bfSize;
	short int bfReserved1;
	short int bfReserved2;
	int bfOffBits;
} BMPHeader;

typedef struct 
{
	int biSize;
	int biWidth;
	int biHeight;
	short int biPlanes;
	short int biBitCount;
	int biCompression;
	int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	int biClrUsed;
	int biClrImportant;
} BMPInfo;

typedef struct
{
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
}PIXEL;

PIXEL** read_bmp(FILE* f, BMPHeader* bfh, BMPInfo* bih)
{
	int k = 0;
	k = fread(bfh, sizeof(*bfh) - 2, 1, f); //читаем файловый заголовок
	if (k == 0)
	{
		std::cout << "reading error";
		return 0;
	}
	
	k = fread(bih, sizeof(*bih), 1, f); //Читаем заголовок изображения
	if (f == NULL)
	{
		std::cout << "reading error";
		return 0;
	}

	int a = bih->biHeight;
	int b = bih->biWidth;

	PIXEL** rgb = new PIXEL * [a];

	for (int i = 0; i < a; i++)
	{
		rgb[i] = new PIXEL[b];
	}
	int pad = 4 - (b * 3) % 4;
	for (int i = 0; i < a; i++)
	{
		fread(rgb[i], sizeof(PIXEL), b, f);   //Читаем байты пикселей
		if (pad != 4)
		{
			fseek(f, pad, SEEK_CUR);
		}
	}
	return rgb;
}

void write_bmp(FILE* f, PIXEL** rgbb, BMPHeader* bfh, BMPInfo* bih, int nlen, int nwid, int b)
{
	bih->biHeight = nlen;
	bih->biWidth = nwid;
	fwrite(bfh, sizeof(*bfh) - 2, 1, f);
	fwrite(bih, sizeof(*bih), 1, f);
	int pad = 4 - ((b / 2) * 3) % 4;
	char buf = 0;
	for (int i = 0; i < nlen; i++)
	{
		fwrite((rgbb[i]), sizeof(PIXEL), nwid, f);
		if (pad != 4)
		{
			fwrite(&buf, 1, pad, f);
		}
	}
}

void WriteBitsIntoFile(unsigned char in[], unsigned int len, string inpath)
{
	int n = inpath.length();
    char char_array[n + 1];
    strcpy(char_array, inpath.c_str());
	ofstream out(char_array);
	
	for(int i = 0; i < len; i++)
	{
		unsigned char x = in[i];
		for(int j = 0; j < 8; j++)
		{
			out << (bool((1 << j)  &  x));
		}
	}		
}

void OriginalFreaquancyTest(string original, unsigned int len, int blockLen)
{
	int n = original.length();
    char char_array[n + 1];
    strcpy(char_array, original.c_str());
	ifstream orig(char_array);
	ofstream out("Original_FT.txt");

	//Freaquancy test
	int bits = blockLen * 8;
	int size = 0;
	while(size < len)
	{
    	int sum1 = 0;
    	int sum0 = 0;
    	for(int i = 0; i < bits; i++)
    	{
    		char c;
        	orig.get(c);
			if(c == '1')
				sum1++;
			if(c == '0')
				sum0++;			
		}
		size += bits;
		double p = (double) sum1/sum0;
		out << "For block " << size / bits << ": " << endl; 
		out << "ones = " << sum1 << endl;
		out << "zeroes = " << sum0 << endl;	
		out << "p = " << p << endl;
	}	
}

void OriginalSeriesTest(string original, unsigned int len, int blockLen)
{
	int n = original.length();
    char char_array[n + 1];
    strcpy(char_array, original.c_str());
	ifstream orig(char_array);
	ofstream out("Original_ST.txt");
		
	//Series test
	int bits = blockLen * 8;
	int size = 0;
	while (size < len)
    {
    	int sum = 0;
    	
    	char block[bits];
    	
    	for(int i = 0; i < bits; i++)
    	{
    		char c;
        	orig.get(c);
			if(c == '1')
				sum++;
			
			block[i] = c;	
		}
		double n = (double) sum/bits;                //////////////////////////
		bool check = (abs(n - 0.5) <= 2/sqrt(bits));
		size += bits;
		
		if(!check)
			out << "For block " << size / bits << " series test is passed on the start" << endl;
		else
		{
			int series = 0;
			for(int i = 0; i < bits - 1; i++)
			{
				if(block[i] == block[i+1])
					series++;
			}
			
			double p = erfc(abs(series - 2 * n * bits * (1 - n)) / (2*sqrt(2*bits) * n * (1 - n)));
			
			out << "For block " << size / bits << " p values is " << p << endl;
		}
	}	
}

int mod(int a, int b)
{
	return a % b;
}

void OriginalAutocorrelationTest(string original, unsigned int len, int blockLen)
{
	int n = original.length();
    char char_array[n + 1];
    strcpy(char_array, original.c_str());
	ifstream orig(char_array);
	ofstream out("Original_AcT.txt");
	
	int bits = blockLen * 8;
	
	//Autocorrelation test
	int size = 0;
	while (size < len)
    {
    	
    	int sum = 0;
    	int sum1 = 0;
    	char block[bits];
    	
    	for(int i = 0; i < bits; i++)
    	{
    		char c;
        	orig.get(c);
			block[i] = c;
		}

		int t = 1;              ////////////////////
		for(int i = 0; i < bits; i++)
    	{
    		int j = mod(i+t, bits);
    		
    		if(block[i] != block[j])
    			sum1++;
		}
		size+=bits;
		int sum0 = bits - sum1;
		double p = (double) sum1/sum0;
		out << "For block " << size / bits << ": " << endl; 
		out << "ones = " << sum1 << endl;
		out << "zeroes = " << sum0 << endl;	
		out << "p = " << p << endl;
	}		
}

void CipherFreaquancyTest(string original, unsigned int len, int blockLen)
{
	int n = original.length();
    char char_array[n + 1];
    strcpy(char_array, original.c_str());
	ifstream orig(char_array);
	ofstream out("Cipher_FT.txt");
	
	//Freaquancy test
	int bits = blockLen * 8;
	int size = 0;
	while (size < len)
    {
    	int sum1 = 0;
    	int sum0 = 0;
    	for(int i = 0; i < bits; i++)
    	{
    		char c;
        	orig.get(c);
			if(c == '1')
				sum1++;
			if(c == '0')
				sum0++;			
		}
		double p = erfc(abs(sum1) / sqrt(2*bits));
		//double p = (double) sum1/sum0;
		size += bits;
		out << "For block " << size / bits << ": " << endl; 
		out << "ones = " << sum1 << endl;
		out << "zeroes = " << sum0 << endl;	
		out << "p = " << p << endl;
	}	
}

void CipherSeriesTest(string original, unsigned int len, int blockLen)
{
	int n = original.length();
    char char_array[n + 1];
    strcpy(char_array, original.c_str());
	ifstream orig(char_array);
	ofstream out("Cipher_ST.txt");
	int bits = blockLen * 8;
	
	//Series test
	int size = 0;
	while (size < len)
    {
    	int sum = 0;
    	char block[bits];
    	
    	for(int i = 0; i < bits; i++)
    	{
    		char c;
        	orig.get(c);
			if(c == '1')
				sum++;
			
			block[i] = c;	
		}
		double n = (double) sum/bits;                         
		bool check = (abs(n - 0.5) <= 2/sqrt(bits));
		size += bits;
		
		if(!check)
			out << "For block " << size / bits << " series test is not passed on the start" << endl;
		else
		{
			int series = 0;
			for(int i = 0; i < bits - 1; i++)
			{
				if(block[i] == block[i+1])
					series++;
			}
			
			double p = erfc(abs(series - 2 * n * bits * (1 - n)) / (2*sqrt(2*bits) * n * (1 - n)));
			out << "For block " << size / bits << " p values is " << p << endl;
		}
	}		
}

void CipherAutocorrelationTest(string original, unsigned int len, int blockLen)
{
	int n = original.length();
    char char_array[n + 1];
    strcpy(char_array, original.c_str());
	ifstream orig(char_array);
	ofstream out("Cipher_AcT.txt");
	
	int bits = blockLen * 8;
	
	//Autocorrelation test
	int size = 0;
	while (size < len)
    {
    	
    	int sum = 0;
    	int sum1 = 0;
    	char block[bits];
    	
    	for(int i = 0; i < bits; i++)
    	{
    		char c;
        	orig.get(c);
			block[i] = c;
		}

		int t = 3;              ////////////////////
		for(int i = 0; i < bits; i++)
    	{
    		int j = mod(i+t, bits);
    		
    		if(block[i] != block[j])
    			sum1++;
		}
		size+=bits;
		int sum0 = bits - sum1;
		double p = (double) sum1/sum0;
		out << "For block " << size / bits << ": " << endl; 
		out << "ones = " << sum1 << endl;
		out << "zeroes = " << sum0 << endl;	
		out << "p = " << p << endl;
	}		
}

int main()
{
	BMPHeader bfh;
	BMPInfo bih;
	FILE *f1;
	FILE *f2;
	f1 = fopen("airplane.bmp", "rb");

	if (f1 == NULL)
	{
		cout << "reading error";
		return 0;
	}
	int cipher_p = 0;
	PIXEL** rgb = read_bmp(f1, &bfh, &bih);
	int a = bih.biHeight;
	int b = bih.biWidth;

	unsigned char plain[a*b*3];
	
	int plain_p = 0;
	
	for(int i = 0; i < a; i++){
		for(int j = 0; j < b; j++){
			plain[plain_p] = rgb[i][j].rgbBlue;
			plain[plain_p + 1] = rgb[i][j].rgbGreen; 	
			plain[plain_p + 2] = rgb[i][j].rgbRed;
			plain_p += 3;		
		}
	}
	fclose(f1);
	cout << "Finished reading\n";
	
	unsigned char key[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned int plainLen = a * b * 3 * sizeof(unsigned char);
	int blockLen = 16;
	AES aes(256);
	unsigned char *ecb = aes.EncryptECB(plain, plainLen, key);
	cout << "Encrypted ECB\n";
	f2 = fopen("Encrypted_ECB.bmp", "wb");	
	cipher_p = 0;
	for(int i = 0; i < a; i++){
		for(int j = 0; j < b; j++){
		    rgb[i][j].rgbBlue =	ecb[cipher_p];
			rgb[i][j].rgbGreen = ecb[cipher_p + 1]; 	
			rgb[i][j].rgbRed = ecb[cipher_p + 2];
			cipher_p += 3;		
		}
	}
		
	write_bmp(f2, rgb, &bfh, &bih, a, b, b);
	cout << "ECB writed into file\n";
	fclose(f2);
	
	WriteBitsIntoFile(plain, plainLen, "PlainText.txt");
	WriteBitsIntoFile(ecb, plainLen, "CipherText.txt");
	
	OriginalFreaquancyTest("PlainText.txt", plainLen, blockLen);
	OriginalSeriesTest("PlainText.txt", plainLen, blockLen);
	OriginalAutocorrelationTest("PlainText.txt", plainLen, blockLen);
	
	CipherFreaquancyTest("CipherText.txt", plainLen, blockLen);
	CipherSeriesTest("CipherText.txt", plainLen, blockLen);
	CipherAutocorrelationTest("CipherText.txt", plainLen, blockLen);
	
	for (int i = 0; i < a; i++){
		delete(rgb[i]);
	}
	delete(rgb);
	
	return 0;
}

