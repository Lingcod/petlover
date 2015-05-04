#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#define isTest 0

#define maxDoc 10950

using namespace std;

struct postype
{
	long int pos;
	char type;
};

struct invertedindextuple
{
	long int freq;
	vector<postype> postypes;
};

struct lexicontuple
{
	int fileNum;
	unsigned long offset;
	unsigned long occurNum;
};

class term
{
public:
	string word;
	long int occurNum;
	shared_ptr<map<long int, invertedindextuple>> postings = make_shared<map<long int, invertedindextuple>>();
	char* indexDIR = new char[512];

	//read index for current word into memory, then close file and delete trivial
	int openList(map<string, lexicontuple>* lexData);
	long int nextGEQ(long int minDID);
};