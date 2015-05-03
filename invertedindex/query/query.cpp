#define _CRT_SECURE_NO_WARNINGS
#include "term.h"
#include <time.h>

struct urlsize
{
	string url;
	long int size;
};

struct forBM25
{
	long double occurNum;
	long double freq;
};

long double bm25(vector<forBM25> stat, long int idocid, long int idSize, long double avgSize, long int iN)
{
	long double K;
	long double k1 = 1.2L;
	long double b = 0.75L;
	long double result = 0.0L;
	long double N = (long double)iN;
	long double docid = (long double)idocid;
	long double dSize = (long double)idSize;

	K = k1*((1.0L - b) + b*(dSize / avgSize));

	for (forBM25 f:stat)
	{
		long double ft = f.occurNum;
		long double fdt = f.freq;
		result += (log10((N - ft + 0.5L) / (ft + 0.5L)) * ((k1 + 1.0L)*fdt) / (K + fdt));
	}
	return result;
}

int initLexicon(char* lexDIR, map<string, lexicontuple>* lexData)
{
	ifstream lexicon;
	lexicon.open(lexDIR);

	//cout << "2";
	//find position of inverted index file, from lexicon file
	bool isFind = false;
	unsigned long lineLength = 256;
	char* line = new char[256];
	unsigned long lastPos = 0;

	while (lexicon.good())
	{
		//read line from lexicon file
		lexicon.getline(line, lineLength);
		while (lexicon.fail() && !lexicon.eof())
		{
			lineLength *= 2;
			line = new char[lineLength];
			lexicon.clear();
			lexicon.seekg(lastPos);
			lexicon.getline(line, lineLength);
		}
		lastPos = lexicon.tellg();
		if (strlen(line) < 5)
		{
			break;
		}

		//parse line
		char* token = strtok(line, " ");
		if (token != NULL)
		{
			string tstr(token);
			lexData->operator[] (tstr).fileNum = atoi(strtok(NULL, " "));
			lexData->operator[] (tstr).offset = atoi(strtok(NULL, " "));
			lexData->operator[] (tstr).occurNum = atoi(strtok(NULL, " "));
		}
	}
	delete[] line;
	lexicon.close();

	return 0;
}

int initD2U(char* d2uDIR, map<long int, urlsize>* d2uData)
{
	long temp = 0;
	long avg = 0;
	ifstream d2uStream;
	char filename[512];
	char* line = new char[2100];
	int fn;

		fn = 1;
		sprintf_s(filename, "%s\\d2u.txt", d2uDIR);

	for (int i = 0; i < fn; i++)
	{
		d2uStream.open(filename);
		while (d2uStream.good())
		{
			d2uStream.getline(line, 2100);
			char* tk = strtok(line, " ");
			if (tk != NULL)
			{
				long d = atoi(tk);

				d2uData->operator[](d).url = string(strtok(NULL, " "));
				d2uData->operator[](d).size = atoi(strtok(NULL, " "));

				temp = avg + d2uData->operator[](d).size;
				if (temp == 0) continue;
				avg = temp / 2;
			}
		}
		sprintf_s(filename, "%s\\d2u_%d00.txt", d2uDIR, i + 1);
		d2uStream.close();
	}
	return avg;
}


void get_result(char* query)
{
	map<string, lexicontuple>* lexData = new map<string, lexicontuple>;
	map<long int, urlsize>* d2uData = new map<long int, urlsize>;
	char* lexDIR;
	char* d2uDIR;
	long double avgSize;
	clock_t start, finish;

	start = clock();

		lexDIR = "E:\\Downloads\\pets\\Pages\\lexicon.txt";
		d2uDIR = "E:\\Downloads\\pets\\Pages";

	initLexicon(lexDIR, lexData);


	avgSize = initD2U(d2uDIR, d2uData);
	finish = clock();
	double initduration = (double)(finish - start) / CLOCKS_PER_SEC;
	cout << "Initialization: " << initduration << " seconds\n";
	while (true)
	{
	restart:
		double duration[6] = { 0 }; //openlist, daat, push d2u, calculate score, init lex and url table, delete objects

		shared_ptr<vector<char*>> queries = make_shared<vector<char*>>();
		shared_ptr<vector<term>> terms = make_shared<vector<term>>();
		shared_ptr<vector<long int>> didCollection = make_shared<vector<long int>>(); //all docids meet requirement	
		shared_ptr<map<long double, string>> results = make_shared<map<long double, string>>(); //final results


		char* token;
		token = strtok(query, " ");
		while (token != NULL)
		{
			for (size_t i = 0; i < strlen(token); i++)
			{
				if (!isalnum(token[i]))
				{
					cout << "Please input alphabetical and numerical characters.\n";
					goto restart;
				}
			}
			queries->push_back(token);
			token = strtok(NULL, " ");
		}

		//run openlist() for each term, then push to tempterms
		//cout << "1 ";
		start = clock();
		vector<term> tempTerms;
		for (char* temp : *queries)
		{
			string str(temp);
			term oneTerm;
			oneTerm.word = str;

				oneTerm.indexDIR = "E:\\Downloads\\pets\\Pages\\index";


			int r = oneTerm.openList(lexData);
			if (r == -1)
			{
				goto restart;
			}
			tempTerms.push_back(oneTerm);
		}

		//sort terms by list size
		//cout << "2 ";
		while (tempTerms.size() > 0)
		{
			unsigned long minSize = maxDoc;
			int minIndex;
			for (size_t i = 0; i < tempTerms.size(); i++)
			{
				if (tempTerms[i].postings->size() < minSize)
				{
					minIndex = i;
					minSize = tempTerms[i].postings->size();
				}
			}
			terms->push_back(tempTerms[minIndex]);
			tempTerms.erase(tempTerms.begin() + minIndex);
		}
		finish = clock();
		duration[0] += (double)(finish - start) / CLOCKS_PER_SEC;


		//DAAT query process
		//cout << "3 ";
		start = clock();
		long int did = 0;
		long int tempDid = 0;

		while (did < maxDoc)
		{
			did = terms->operator[](0).nextGEQ(did);
			for (unsigned i = 1; (i < terms->size()) && ((tempDid = terms->operator [](i).nextGEQ(did)) == did); i++);
			if (tempDid > did)
			{
				did = tempDid;
			}
			else
			{
				if (did != maxDoc) didCollection->push_back(did);
				did++;
			}
		}
		finish = clock();
		duration[1] += (double)(finish - start) / CLOCKS_PER_SEC;


		//Calculate score, push into results
		//cout << "4 ";
		start = clock();
		double score;

		for (long int d : *didCollection)
		{
			vector<forBM25> stat;
			for (term t : *terms)
			{
				forBM25 temp;
				temp.freq = t.postings->operator[](d).freq;
				temp.occurNum = t.occurNum;
				stat.push_back(temp);
			}

			score = bm25(stat, d, d2uData->operator[](d).size, avgSize, maxDoc);
			while (results->find(score) != results->end())
			{
				score += 0.000000000001L;
			}
			results->operator[](score) = d2uData->operator[](d).url;

		}
		finish = clock();
		duration[2] += (double)(finish - start) / CLOCKS_PER_SEC;

		//print results
		int i = 0;
		cout << "\n";
		for (map<long double, string>::reverse_iterator it = results->rbegin(); i < 10 && it != results->rend(); it++, i++)
		{
			cout << it->first << " " << it->second.c_str() << "\n";
		}


		cout << "\nOpen List: " << duration[0]
			<< " seconds\nQuery Process: " << duration[1]
			<< " seconds\nFind top10 scores: " << duration[2] << " seconds\n";
	}

}