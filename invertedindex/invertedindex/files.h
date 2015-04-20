//
// docid: word type pos,word type pos,...			map<key=docid, vector<struct{word,type,pos}>>
//
// word:  docid freq pos.type pos.type ..., ...		map<key=word, vector<struct{docid,freq,vector<struct{pos,type}>}>>
#include <vector>
#include <map>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <cctype>
#define INDEX_CHUNK 1048576 // 1MB
#define DATA_CHUNK 62914560 // 50MB

extern "C" {
#define ZLIB_WINAPI
#include "zlib.h"
#include "parser.h"
}
using namespace std;

struct indextuple
{
	char* URL;
	long int size;
};

struct postype
{
	long int pos;
	char type;
};

struct invertedindextuple
{
	size_t freq;
	vector<postype> postypes;
};


//operations for one specific fileNum
class files
{
public:
	int fileNum;
	int il_size;
	int realloc_times;
	long int tt_len;
	char* indexBuffer = "";
	char* dataBuffer = "";
	char* page_orgn = "";
	char* pool_orgn = "";
	char* PAGEDIR = "";

	vector<indextuple> indexlist;
	map<string, map<long int, invertedindextuple>> fileii;

	//open index file for indicated fileNum, and initialize indexbuffer
	int openindex();

	//open data file, initialize databuffer
	int opendata();

	//parse indexbuffer into indexlist
	int parseindex();

	//parse databuffer into fileii. st_docid is the start docid. return the next start docid. Write docid to url file.
	long int parsedata(long int st_docid, ofstream &d2ufile);

	//write inverted index to files, initiate freq for each word each docid
	int write2file();

};