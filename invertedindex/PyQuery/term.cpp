#define _CRT_SECURE_NO_WARNINGS
#include "term.h"

int term::openList(map<string, lexicontuple>* lexData)
{
	int iiFileNum = lexData->operator[](word).fileNum;
	unsigned long offset = lexData->operator[](word).offset;
	occurNum = lexData->operator[](word).occurNum;

	//find inverted index, then store into map
	char iiFilename[512];
	sprintf_s(iiFilename, "%s\\merged2nd_%d.txt", indexDIR, iiFileNum);
	ifstream indexStream;
	indexStream.open(iiFilename);
	indexStream.seekg(offset);

	bool isFind = false;
	long lineLength = 51200;
	char* line = new char[51200];
	if (indexStream.good())
	{
		//read line from indexStream file
		indexStream.getline(line, lineLength);
		while (indexStream.fail())
		{
			lineLength *= 2;
			delete[] line;
			line = new char[lineLength];
			indexStream.clear();
			indexStream.seekg(offset);
			indexStream.getline(line, lineLength);
		}

		//parse line to map postings
		char* token = strtok(line, ":");
		long int lastdid = 0;
		string tstr(token);
		if (tstr == word)
		{
			isFind = true;
			token = strtok(NULL, ";");
			while (token != NULL)
			{
				char* nextPosting = token + strlen(token) + 1;
				long int docid;
				char* cdocid;
				long int freq;
				char* cfreq;
				char* pt;
				long int lastPos = 0;

				//for docid, freq
				cdocid = strtok(token, " ");
				docid = atoi(cdocid) + lastdid;
				lastdid = docid;
				cfreq = strtok(NULL, " ");
				freq = atoi(cfreq);

				postings->operator[](docid).freq = freq;
				//cout << docid << " ";
				char* temp = cfreq + strlen(cfreq) + 1;

				//for postypes
				pt = strtok(NULL, " ");
				while (pt != NULL)
				{
					postype newpt;
					temp = strtok(pt, ".");
					newpt.pos = atoi(temp) + lastPos;
					lastPos = newpt.pos;
					temp += (strlen(temp) + 1);
					newpt.type = temp[0];
					postings->operator[](docid).postypes.push_back(newpt);
					temp += 2;
					if (temp >= nextPosting)
					{
						break;
					}
					pt = strtok(temp, " ");

				}
				token = strtok(temp, ";");
			}

		}
		indexStream.close();
		delete[] line;
	}
	else
	{
		indexStream.close();
		delete[] line;
		cout << "index stream no good\n";
		return -1;
	}

	if (!isFind)
	{
		cout << "Can't find word: " << word.c_str() << " in index\n";
		return -1;
	}
	return 0;
}

long int term::nextGEQ(long int minDID)
{
	if (postings->find(minDID) != postings->end())
	{
		return minDID;
	}
	else
	{
		map<long int, invertedindextuple>::iterator it = postings->upper_bound(minDID);
		if (it != postings->end())
		{
			return it->first;
		}
		else
		{
			return maxDoc;
		}
	}
}
