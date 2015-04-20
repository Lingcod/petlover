#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <time.h>
#include <iostream>

#define iiChunk 20971520 //20MB for 1 inverted index file

#define tt_fileNum 13
#define folderNum 1

unsigned long int HEAPCHUNK = 838860800; //800MB
unsigned long int HEAPLIMIT = 629145600; //600MB

using namespace std;

char* buf_add_1[tt_fileNum] = {};
char* buf_add_2[folderNum] = {};
map<string, int> wordfreq;

struct bufferStuct
{
	ifstream *strm;
	map<string, char*> cache;
	int fileNum;
	int isExceed = 0;
};

struct heapStruct
{
	char* allpost[tt_fileNum];
	vector<int> contrib;
	heapStruct() :allpost() {}
};

//push records to an empty bufferStruct* Return Null if fstream failure. Return pointer to the new position
char* pushrecords(bufferStuct* emptybuf, char* buf_st_space, int records, unsigned times, int phase)
{
	unsigned long BUFFCHUNK;
	switch (phase)
	{
	case 1:
		BUFFCHUNK = 2097152;
		break;
	case 2:
		BUFFCHUNK = 5242880;
	default:
		break;
	}
	char* old_st = buf_st_space;
	if (emptybuf == NULL || !emptybuf->strm->good())
	{
		return NULL;
	}
	long pos = emptybuf->strm->tellg();
	for (int j = 0; j < records; j++)
	{
		string line;
		if (emptybuf->strm->good())
		{
			getline(*emptybuf->strm, line);

			if (buf_st_space + line.length() > old_st + BUFFCHUNK * times)
			{
				cout << emptybuf->fileNum << "EXCD" << times << " ";
				emptybuf->strm->seekg(pos);
				return buf_st_space + line.length();
			}
			char *cline = buf_st_space;
			strcpy(cline, line.c_str());
			unsigned clinelen = strlen(cline);
			char* word = strtok(cline, ":");
			if (word != NULL)
			{
				string wordstr(word);
				char* index = strtok(NULL, "\n");
				buf_st_space += (clinelen + 2);
				emptybuf->cache[wordstr] = index;
			}
			else
			{
				emptybuf->strm->close();
				if (j == 0)
				{
					return NULL;
				}
				break;
			}
		}
		else
		{
			emptybuf->strm->close();
			if (j == 0)
			{
				return NULL;
			}
			break;
		}
	}

	return buf_st_space;
}

//push cache map of buf1's first records into heap. 
char* push2heap(bufferStuct* buf1, map<string, heapStruct> &heap, char* heap_st_space, int f_num)
{
	string beginword(buf1->cache.begin()->first);
	int fileNum = buf1->fileNum - f_num * 100;

	heap[beginword].allpost[fileNum] = heap_st_space;

	strcpy(heap[beginword].allpost[fileNum], buf1->cache.begin()->second);
	heap_st_space += (strlen(heap[beginword].allpost[fileNum]) + 1);
	buf1->cache.erase(beginword);
	heap[beginword].contrib.push_back(buf1->fileNum);
	return heap_st_space;
}

//first phase merge and sort
int mergesort_1st(char* dir, int f_num)
{
	unsigned long BUFFCHUNK = 2097152;
	bufferStuct* buffers[tt_fileNum] = {};
	map<string, heapStruct> heap;
	ofstream merged;
	char* h_space = (char*)malloc(HEAPCHUNK);//heap
	char* heap_st_space = h_space;

	char mgdfilename[256];
	sprintf_s(mgdfilename, "%s\\merged.txt", dir);//Directory
	merged.open(mgdfilename);
	if (!merged.good())	return -1;

	//initialize buffers for each file
	unsigned int times = 1;
	for (int i = 0; i < tt_fileNum; i++)
	{
		if (f_num == 0 && times == 1) //init memory to each buffer, store pointers in buf_add_1
		{
			char* bufadd = (char*)malloc(BUFFCHUNK);
			buf_add_1[i] = bufadd;
		}
		if (f_num == 41 && i == 80)
		{
			break;
		}
		int k = f_num * 100 + i;
		bufferStuct* newbuf = new bufferStuct;
		ifstream* newif = new ifstream;
		char fn[256];
		sprintf_s(fn, 255, "%s\\ii%d.txt", dir, k);//Directory
		newif->open(fn);
		newbuf->strm = newif;
		newbuf->fileNum = k;
		buffers[i] = newbuf;
		char* temp = pushrecords(buffers[i], buf_add_1[i], 50, times, 1);
		if (temp == NULL)
		{
			continue;
		}
		if (temp >= buf_add_1[i]+BUFFCHUNK * times)  //if 50 records exceeded buffer size, realloc and try again
		{
			times = times * 2;
			char* bufadd = (char*)realloc(buf_add_1[i], BUFFCHUNK * times);
			buffers[i]->cache.clear();
			buffers[i]->isExceed = 1;
			buf_add_1[i] = bufadd;
			i--;
			continue;

		}
		else
		{
			if (times != 1)
			{
				times = 1;
			}
		}
	}

	for (int m = 0; m < tt_fileNum; m++) //initialize heap
	{
		if (f_num == 41 && m == 80)
		{
			break;
		}
		if (buffers[m]->cache.size() == 0)
		{
			continue;
		}
		char* temp = push2heap(buffers[m], heap, heap_st_space, f_num);
		heap_st_space = temp;
	}

	while (heap.size() != 0)
	{
		if (heap_st_space > h_space + 419430400)
		{
			heap_st_space = h_space;
		}
		map<string, heapStruct>::iterator it = heap.begin();
		merged << it->first << ":";

		for (int n = 0; n < tt_fileNum; n++)
		{
			if (f_num == 41 && n == 80)
			{
				break;
			}
			if (it->second.allpost[n] != NULL)
			{
				merged << it->second.allpost[n];
			}
		}
		merged << "\n";
		vector<int> contrib = it->second.contrib;
		times = 1;
		for (vector<int>::iterator it3 = contrib.begin(); it3 != contrib.end(); ++it3)
		{
			int filen = *it3 - f_num * 100; // last 2 digit of fileNum in contrib
			if (buffers[filen]->cache.size() == 0)
			{
				if (times == 1 && buffers[filen]->isExceed == 1) //shrink when it is not in realloc procedure but exceeded last time
				{
					char* bufadd = (char*)realloc(buf_add_1[filen], BUFFCHUNK);
					buf_add_1[filen] = bufadd;
					buffers[filen]->isExceed = 0;
				}
				char* temp = pushrecords(buffers[filen], buf_add_1[filen], 50, times, 1);
				if (temp == NULL)
				{
					continue;
				}
				if (temp >= buf_add_1[filen] + BUFFCHUNK * times)
				{
					times = times * 2;
					char* bufadd = (char*)realloc(buf_add_1[filen], BUFFCHUNK * times);
					buffers[filen]->isExceed = 1;
					buffers[filen]->cache.clear();
					buf_add_1[filen] = bufadd;
					it3--;
					continue;
				}
				else // when buffers[filen] has proper buffer size, re-init times = 1
				{
					if (times != 1)
					{
						times = 1;
					}
				}
			}
			char* temp = push2heap(buffers[filen], heap, heap_st_space, f_num);
			heap_st_space = temp;

		}
		heap.erase(it->first);
	}
	merged.close();
	free(h_space);

	return 0;
}

//second phase merge and sort. Write lexicon into file
int mergesort_2nd()
{
	bufferStuct* buffers[folderNum] = {};
	map<string, heapStruct> heap;
	unsigned long int BUFFCHUNK = 5242880;   //5MB

	char* h_space = (char*)malloc(HEAPCHUNK);//heap
	char* heap_st_space = h_space;

	ofstream lexicon;
	char lexfilename[512];

		sprintf_s(lexfilename, "E:\\Downloads\\pets\\Pages\\lexicon.txt");//Directory


	lexicon.open(lexfilename);

	unsigned int times = 1;
	for (int i = 0; i < folderNum; i++) //initialize buffers for each file
	{
		bufferStuct* newbuf = new bufferStuct;
		ifstream* newif = new ifstream;
		char* temp;

		char fn[512];

			sprintf_s(fn, "E:\\Downloads\\pets\\Pages\\index\\merged.txt");


		if (times == 1)
		{
			char* bufadd = (char*)malloc(BUFFCHUNK);
			buf_add_2[i] = bufadd;
		}

		newif->open(fn);
		newbuf->strm = newif;
		newbuf->fileNum = i;
		buffers[i] = newbuf;

		temp = pushrecords(buffers[i], buf_add_2[i], 5, times, 2);
		if (temp >= buf_add_2[i]+ BUFFCHUNK * times)
		{
			times = times * 2;
			char* bufadd = (char*)realloc(buf_add_2[i], BUFFCHUNK * times);
			buffers[i]->isExceed = 1;
			buffers[i]->cache.clear();
			buf_add_2[i] = bufadd;
			i--;
			continue;

		}
		else
		{
			if (times != 1)
			{
				times = 1;
			}
		}
	}

	for (int m = 0; m < folderNum; m++) //initialize heap
	{
		char* temp = push2heap(buffers[m], heap, heap_st_space, 0);
		heap_st_space = temp;
	}

	ofstream merged;
	char mgdfilename[512];

		sprintf_s(mgdfilename, "E:\\Downloads\\pets\\Pages\\index\\merged2nd_0.txt"); //Directory

	merged.open(mgdfilename);
	long posit = 0;
	long endpos = 0;
	int mfileNum = 0;

	while (heap.size() != 0)
	{
		if (endpos > iiChunk)
		{
			cout << mfileNum << " ";
			mfileNum++;
			merged.close();
			merged.clear();

				sprintf_s(mgdfilename, "E:\\Downloads\\pets\\Pages\\index\\merged2nd_%d.txt", mfileNum);//Directory

			merged.open(mgdfilename);

		}

		if (heap_st_space > h_space + HEAPLIMIT)
		{
			heap_st_space = h_space;
		}
		map<string, heapStruct>::iterator it = heap.begin();
		size_t dNum = 0;
		posit = merged.tellp();
		merged << it->first << ":";

		lexicon << it->first << " " << mfileNum << " ";

		long int curdid;
		long int lastdid = 0;
		for (int n = 0; n < folderNum; n++)
		{
			if (it->second.allpost[n] != NULL)
			{
				char* posting = strtok(it->second.allpost[n], ";");
				char* restofp;
				while (posting != NULL)
				{
					char* nextposting = posting + strlen(posting) + 1;
					dNum++;
					curdid = atoi(strtok(posting, " "));
					restofp = strtok(NULL, ";");
					merged << curdid - lastdid << " " << restofp << ";";
					lastdid = curdid;
					if(nextposting != NULL) posting = strtok(nextposting, ";");
				}
			}
		}
		lexicon << posit << " " << dNum << " \n";
		merged << "\n";
		vector<int> contrib = it->second.contrib;
		times = 1;
		for (size_t i = 0; i < contrib.size(); i++)
		{
			int id = contrib[i];
			if (buffers[id]->cache.size() == 0)
			{

				if (times == 1 && buffers[id]->isExceed == 1)
				{
					char* bufadd = (char*)realloc(buf_add_2[id], BUFFCHUNK);
					buf_add_2[id] = bufadd;
					buffers[id]->isExceed = 0;
				}
				char* temp = pushrecords(buffers[id], buf_add_2[id], 5, times, 2);
				if (temp == NULL)
				{
					continue;
				}
				if (temp >= buf_add_2[id] + BUFFCHUNK * times)
				{
					cout << id << "Realloc" << times << " ";
					times = times * 2;
					char* bufadd = (char*)realloc(buf_add_2[id], BUFFCHUNK * times);
					buffers[id]->isExceed = 1;
					buffers[id]->cache.clear();
					buf_add_2[id] = bufadd;
					i--;
					continue;

				}
				else
				{
					if (times != 1)
					{
						times = 1;
					}
				}

			}

			char* temp = push2heap(buffers[id], heap, heap_st_space, 0);

			heap_st_space = temp;
		}
		heap.erase(it->first);
		endpos = merged.tellp();
	}
	merged.close();
	free(h_space);
	return 0;
}

int main()
{
	cout << "Merge Sort\n";
	clock_t start, finish, allstart, allfinish;
	allstart = clock();
	for (int k = 0; k < folderNum; k++) //todo
	{
		cout << k << " ";
		char dir[512];

		sprintf_s(dir, "E:\\Downloads\\pets\\Pages\\index");
		mergesort_1st(dir, k);
	}
	for (int l = 0; l < tt_fileNum; l++)
	{
		free(buf_add_1[l]);
	}

	finish = clock();
	cout << "\nFirst Phase: " << (double)(finish - allstart) / CLOCKS_PER_SEC << " seconds.\n";

	start = clock();
	mergesort_2nd();
	for (int l = 0; l < folderNum; l++)
	{
		free(buf_add_2[l]);
	}

	allfinish = clock();
	cout << "\nSecond Phase: " << (double)(allfinish - start) / CLOCKS_PER_SEC << " seconds.\n";
	double duration = (double)(allfinish - allstart) / CLOCKS_PER_SEC;
	cout << "Total Time: " << duration << " seconds.";

	cin.get();
	return 0;
}