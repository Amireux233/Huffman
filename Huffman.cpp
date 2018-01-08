// Huffman.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "HuffmanCompress.h"

int main()
{
	HuffmanCompress compress;

	try
	{
		char answer;
		do
		{
			cout<<endl
				<< "1.Huffman compress." << endl     
				<< "2.Huffman decompress." << endl      
				<< "3.Exit." << endl 
				<< "Please select:";
				cin >> answer;
				if (answer == '1')
				{
					compress.Compress();
				}
				else if (answer == '2')
				{
					compress.DeCompress();
				}
		} while (answer != '3');
	}
	catch (Error err)
	{
		err.Show();
	}

	system("pause");
	return 0;
}