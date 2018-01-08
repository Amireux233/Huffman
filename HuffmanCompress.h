#pragma once
#ifndef HUFFMAN_COMPRESS
#define HUFFMAN_COMPRESS

#include "huffman_tree.h"

//�ַ�������
struct BufferType
{
	char ch;
	unsigned int bits;
};

//������ѹ����
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
//�����������Ŀ���ļ�д��һ��bit
{
	buf.bits++;                    //����bit������1
	buf.ch = (buf.ch << 1) | bit;  //��bit���뵽�����ַ���
	if (buf.bits == 8)
	{
		fputc(buf.ch, outfp);
		buf.bits = 0;
		buf.ch = 0;
	}
}
void HuffmanCompress::WriteToOutfp()
//���������ǿ�н��ַ�����д��Ŀ���ļ�
{
	unsigned int len = buf.bits;
	if (len>0)
	{
		for (unsigned int i = 0; i<8 - len; i++)
		{   //����ǿգ��������bit�����ӵ�8���Զ�д��Ŀ���ļ�
			Write(0);
		}
	}
}

void HuffmanCompress::Compress()
//����������ù���������ѹ���ļ�
{
	char infName[256], outfName[256];

	cout << "Please input source file name(size less than 4GB):";
	cin >> infName;
	if ((infp = fopen(infName, "rb")) == NULL)
	{
		throw Error("Failed to open source file��");
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
	{   //��ʼ��ch[]��w[]
		ch[i] = (char)i;
		w[i] = 0;
	}

	rewind(infp);
	cha = fgetc(infp);
	while (!feof(infp))
	{
		w[(unsigned char)cha]++;     //�ַ�cha����Ƶ���Լ�1
		size++;                      //�ļ���С�Լ�1
		cha = fgetc(infp);
	}

	if (pHuffmanTree != NULL)delete[]pHuffmanTree;
	pHuffmanTree = new HuffmanTree<char, unsigned long>(ch, w, n);

	rewind(outfp);
	fwrite(&size, sizeof(unsigned long), 1, outfp);    //��Ŀ���ļ�д��Դ�ļ���С
	for (i=0; i<n; i++)
	{
		fwrite(&w[i], sizeof(unsigned long), 1, outfp);//��Ŀ���ļ�д���ַ�����Ƶ��
	}

	buf.bits = 0;
	buf.ch = 0;
	rewind(infp);
	cha = fgetc(infp);
	while (!feof(infp))
	{
		String strTmp = pHuffmanTree->Encode(cha);//�ַ�����
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
//�����������ѹ���ù���������ѹ�����ļ�
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
		throw Error("Code file is empty��");
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
	fread(&size, sizeof(unsigned long), 1, infp);    //��ȡĿ���ļ��Ĵ�С
	for (i = 0; i<n; i++)
	{
		ch[i] = (char)i;
		fread(&w[i], sizeof(unsigned long), 1, infp);//��ȡ�ַ�Ƶ��
	}
	if (pHuffmanTree != NULL)delete[]pHuffmanTree;
	pHuffmanTree = new HuffmanTree<char, unsigned long>(ch, w, n);

	unsigned long len = 0;   //��ѹ���ַ���
	String strTmp = "";

	cha = fgetc(infp);
	while (!feof(infp))
	{	
		unsigned char c = (unsigned char)cha;   //��chaת��Ϊ unsigned char��
		
		for (i = 0; i < 8; i++)
		{   //��cת��Ϊ�����ƴ�
			if (c < 128) Concat(strTmp, "0");
			else Concat(strTmp, "1");
			c = c << 1;
		}
		cha = fgetc(infp);
	}
	String strTemp(pHuffmanTree->Decode(strTmp)); //����
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