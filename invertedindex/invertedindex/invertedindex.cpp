// invertedindex.cpp : Defines the entry point for the console application.

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <time.h>
#include "files.h"

#define tt_fileNum 37

unsigned long int MEM_CHUNK = 838060800; //800MB
unsigned long int MEM_LIMIT = 629145600; //600MB
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Inverted Index\n";
	long int st_docid = 0; //start docid for each file
	long t = 10000000L;
	clock_t start, finish, allstart, allfinish;
	double duration[5] = { 0, 0, 0, 0, 0 }; //openfile, parseindex, parsedata, writefile, alloc&free
	allstart = clock();

	char* m_space = (char *)malloc(MEM_CHUNK);
	char* st_space = m_space;

	char PAGEDIR[512];
	char D2UDIR[512];

	sprintf_s(PAGEDIR, "E:\\Downloads\\pets\\Pages");//Directory
	sprintf_s(D2UDIR, "E:\\Downloads\\pets\\Pages\\d2u.txt");//Directory



	ofstream d2ufile(D2UDIR);

	for (int i = 0; i < tt_fileNum; i++)
	{
		start = clock();
		if (st_space - m_space > MEM_LIMIT)
		{
			//cout << "realloc";
			free(m_space);
			m_space = (char *)malloc(MEM_CHUNK);
			st_space = m_space;
		}
		finish = clock();
		duration[4] += (double)(finish - start) / CLOCKS_PER_SEC;

		std::cout << i << " ";
		files* newfile = new files();
		newfile->fileNum = i;
		newfile->PAGEDIR = PAGEDIR;

		start = clock();
		newfile->indexBuffer = st_space;
		newfile->openindex();
		st_space += INDEX_CHUNK;
		if (newfile->realloc_times > 0)
		{
			unsigned long int chunkNum = (unsigned long int)pow(2, newfile->realloc_times);
			while (chunkNum - 1 > 0)
			{
				st_space += INDEX_CHUNK;
				chunkNum--;
			}
		}
		newfile->dataBuffer = st_space;
		newfile->opendata();
		st_space += DATA_CHUNK;
		if (newfile->realloc_times > 0)
		{
			unsigned long int chunkNum = (unsigned long int)pow(2, newfile->realloc_times);
			while (chunkNum - 1 > 0)
			{
				st_space += DATA_CHUNK;
				chunkNum--;
			}
		}
		finish = clock();
		duration[0] += (double)(finish - start) / CLOCKS_PER_SEC;

		start = clock();
		newfile->parseindex();
		finish = clock();
		duration[1] += (double)(finish - start) / CLOCKS_PER_SEC;

		start = clock();
		int il_size = newfile->il_size;
		long int tt_len = newfile->tt_len;
		newfile->page_orgn = st_space;
		st_space += (tt_len + il_size);
		newfile->pool_orgn = st_space;
		st_space += (2 * tt_len + il_size);
		finish = clock();
		duration[4] += (double)(finish - start) / CLOCKS_PER_SEC;

		start = clock();
		long int temp;

		temp = newfile->parsedata(st_docid, d2ufile);
		st_docid = temp;
		finish = clock();
		duration[2] += (double)(finish - start) / CLOCKS_PER_SEC;

		start = clock();
		newfile->write2file();
		finish = clock();
		duration[3] += (double)(finish - start) / CLOCKS_PER_SEC;

		start = clock();
		newfile->fileii.clear();
		newfile->indexlist.clear();
		delete newfile;
		finish = clock();
		duration[4] += (double)(finish - start) / CLOCKS_PER_SEC;

	}
	d2ufile.close();

	std::cout << "\nFinish\n";

	allfinish = clock();
	double tt_time = (double)(allfinish - allstart) / CLOCKS_PER_SEC;

	printf("Open Files: %f seconds\nParse Index: %f seconds\nParse Data: %f seconds\nWrite Files: %f seconds\nAlloc and Free Space: %f seconds\nTotal: %f seconds",
		duration[0], duration[1], duration[2], duration[3], duration[4], tt_time);

	std::cin.get();
	return 0;
}

