#pragma once
#ifndef HUFFMAN_COMPRESS
#define HUFFMAN_COMPRESS

#include "huffman_tree.h"

//字符缓冲器
struct BufferType
{
	char ch;
	unsigned int bits;
};

//哈夫曼压缩类
class HuffmanCompress
{
protected:
	HuffmanTree<char, unsigned long>*pHuffmanTree;
    FILE*infp, *outfp;
    BufferType buf;

    void Write(unsigned int bit);
    void WriteToOutfp();
public:
	HuffmanCompress() { pHuffmanTree = NULL; };
	~HuffmanCompress() { if (pHuffmanTree != NULL)delete[]pHuffmanTree; };
	void Compress();
	void DeCompress();
};

void HuffmanCompress::Write(unsigned int bit)
//操作结果：向目标文件写入一个bit
{
	buf.bits++;                    //缓存bit数自增1
	buf.ch = (buf.ch << 1) | bit;  //将bit加入到缓存字符中
	if (buf.bits == 8)
	{
		fputc(buf.ch, outfp);
		buf.bits = 0;
		buf.ch = 0;
	}
}
void HuffmanCompress::WriteToOutfp()
//操作结果：强行将字符缓存写入目标文件
{
	unsigned int len = buf.bits;
	if (len>0)
	{
		for (unsigned int i = 0; i<8 - len; i++)
		{   //缓存非空，将缓存的bit数增加到8，自动写入目标文件
			Write(0);
		}
	}
}

void HuffmanCompress::Compress()
//操作结果：用哈夫曼编码压缩文件
{
	char infName[256], outfName[256];

	cout << "Please input source file name(size less than 4GB):";
	cin >> infName;
	if ((infp = fopen(infName, "rb")) == NULL)
	{
		throw Error("Failed to open source file！");
	}

	fgetc(infp);
	if (feof(infp))
	{
		throw Error("Source file is empty");
	}

	cout << "Please input code file name:";
	cin >> outfName;
	if ((outfp = fopen(outfName, "wb")) == NULL)
	{
		throw Error("Failed to open code file");
	}

	cout << "Pocessing..." << endl;

	const unsigned long n = 256;
	char ch[n];
	unsigned long w[n];
	unsigned long i, size=0;
	char cha;

	for (i=0; i<n; i++)
	{   //初始化ch[]和w[]
		ch[i] = (char)i;
		w[i] = 0;
	}

	rewind(infp);
	cha = fgetc(infp);
	while (!feof(infp))
	{
		w[(unsigned char)cha]++;     //字符cha出现频度自加1
		size++;                      //文件大小自加1
		cha = fgetc(infp);
	}

	if (pHuffmanTree != NULL)delete[]pHuffmanTree;
	pHuffmanTree = new HuffmanTree<char, unsigned long>(ch, w, n);

	rewind(outfp);
	fwrite(&size, sizeof(unsigned long), 1, outfp);    //向目标文件写入源文件大小
	for (i=0; i<n; i++)
	{
		fwrite(&w[i], sizeof(unsigned long), 1, outfp);//向目标文件写入字符出现频度
	}

	buf.bits = 0;
	buf.ch = 0;
	rewind(infp);
	cha = fgetc(infp);
	while (!feof(infp))
	{
		String strTmp = pHuffmanTree->Encode(cha);//字符编码
		for (i = 0; i <strTmp.Length(); i++)
		{
			if (strTmp[i] == '0')
			{
				Write(0);
			}
			else
			{
				Write(1);
			}
		}
		cha = fgetc(infp);
	}
	WriteToOutfp();

	fclose(infp);
	fclose(outfp);
	cout << "Process end." << endl;
}

void HuffmanCompress::DeCompress()
//操作结果：解压缩用哈夫曼编码压缩的文件
{
	char infName[256], outfName[256];

	cout << "Please input code file name:";
	cin >> infName;
	if ((infp = fopen(infName, "rb")) == NULL)
	{
		throw Error("Failed to open code file");
	}

	fgetc(infp);
	if (feof(infp))
	{
		throw Error("Code file is empty！");
	}

	cout << "Please input target file name:";
	cin >> outfName;
	if ((outfp = fopen(outfName, "wb")) == NULL)
	{
		throw Error("Failed to open target file");
	}

	cout << "Pocessing..." << endl;

	const unsigned long n = 256;
	char ch[n];
	unsigned long w[n];
	unsigned long i, size=0;
	char cha;

	rewind(infp);
	fread(&size, sizeof(unsigned long), 1, infp);    //读取目标文件的大小
	for (i = 0; i<n; i++)
	{
		ch[i] = (char)i;
		fread(&w[i], sizeof(unsigned long), 1, infp);//读取字符频度
	}
	if (pHuffmanTree != NULL)delete[]pHuffmanTree;
	pHuffmanTree = new HuffmanTree<char, unsigned long>(ch, w, n);

	unsigned long len = 0;   //解压的字符数
	String strTmp = "";

	cha = fgetc(infp);
	while (!feof(infp))
	{	
		unsigned char c = (unsigned char)cha;   //将cha转换为 unsigned char型
		
		for (i = 0; i < 8; i++)
		{   //将c转化为二进制串
			if (c < 128) Concat(strTmp, "0");
			else Concat(strTmp, "1");
			c = c << 1;
		}
		cha = fgetc(infp);
	}
	String strTemp(pHuffmanTree->Decode(strTmp)); //译码
	for (i = 0; i<strTemp.Length(); i++)
	{
		len++;
		fputc(strTemp[i], outfp);
		if (len == size)break;
	}
	

	fclose(infp);
	fclose(outfp);
	cout << "Process end." << endl;
}

#endif