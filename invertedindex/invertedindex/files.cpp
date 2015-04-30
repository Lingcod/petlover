#include "files.h"

int files::openindex(){
	char indexfilename[256];

	long int size;
	long int oldSize;
	long int count;

	sprintf(indexfilename, "%s\\%d_index", PAGEDIR, fileNum);//Directory

	gzFile cIndex;
	cIndex = gzopen(indexfilename, "r");

	size = INDEX_CHUNK;
	oldSize = size;
	count = 0;             //The number of bytes that already read
	realloc_times = 0;
	while (!gzeof(cIndex))
	{
		count += gzread(cIndex, indexBuffer + count, oldSize);
		if (count == size)                    // Reallocate when buffer is full
		{
			realloc_times++;
			oldSize = size;
			size *= 2;
		}
		if (realloc_times == 0)
		{
			break;
		}
	}

	gzclose(cIndex);
	return 0;
}

int files::opendata()
{
	long int size;
	long int oldSize;
	long int count;

	char datafilename[256];
	sprintf(datafilename, "%s\\%d_data", PAGEDIR, fileNum);//Directory
	gzFile cData;
	cData = gzopen(datafilename, "r");

	size = DATA_CHUNK;
	oldSize = size;
	count = 0;
	realloc_times = 0;
	while (!gzeof(cData))
	{
		count += gzread(cData, dataBuffer + count, oldSize);
		if (count == size)
		{
			realloc_times++;
			oldSize = size;
			size *= 2;
		}
		if (realloc_times == 0)
		{
			break;
		}
	}
	gzclose(cData);
	return 0;

}

int files::parseindex()
{
	char* token = strtok(indexBuffer, " ");
	while (token != NULL)
	{
		indextuple newit;
		newit.URL = token;
		token = strtok(NULL, "\n");
		if (token == NULL) break;
		newit.size = atoi(token);
		indexlist.push_back(newit);
		token = strtok(NULL, " ");
	}

	tt_len = 0;
	il_size = static_cast<int>(indexlist.size());
	for (vector<indextuple>::iterator it = indexlist.begin(); it != indexlist.end(); ++it)
	{
		tt_len += it->size;
	}

	return 0;
}

long int files::parsedata(long int st_docid, ofstream &d2ufile){
	char *pool;
	char *page;
	long int pos_b = 0;
	int i = 0;

	page = page_orgn;
	pool = pool_orgn;

	while (i < il_size)//<il_size
	{
		long int len = indexlist[i].size;
		char* url = indexlist[i].URL;
		d2ufile << st_docid + i << " " << url << " " << indexlist[i].size << "\n";

		copy(dataBuffer + pos_b, dataBuffer + pos_b + len, page);
		page[len] = '\0';

		int ret = parser(url, page, pool, 2 * len + 1, len + 1);
		long int docid = st_docid + i;

		//stores into structure
		long int k = 0;
		char* token = strtok(pool, " ");
		while (token != NULL)
		{
			if (!isalnum(token[0])) break;
			postype newpt;
			char word[128];

			string str(token);
			size_t strl = str.copy(word, 127, 0);
			word[strl] = '\0';

			token = strtok(NULL, "\n");
			if (token == NULL) break;
			if (!isalpha(token[0])) break;

			newpt.type = token[0];
			newpt.pos = k;
			fileii[word][docid].postypes.push_back(newpt);
			k++;

			token = strtok(NULL, " ");
		}

		pos_b = pos_b + len;
		i++;

		page += (len + 1);
		pool += (2 * len + 1);

	}
	return st_docid + i;
}

int files::write2file(){

	char filename[256];
	sprintf(filename, "%s\\index\\ii%d.txt", PAGEDIR, fileNum);//Directory
	ofstream myfile(filename);
	if (myfile.is_open())
	{

		for (map<string, map<long int, invertedindextuple>>::iterator it = fileii.begin(); it != fileii.end(); ++it)
		{
			myfile << it->first << ":";
			int lastdocid = 0;
			for (map<long int, invertedindextuple>::iterator it2 = fileii[it->first].begin(); it2 != fileii[it->first].end(); ++it2)
			{
				it2->second.freq = it2->second.postypes.size();
				myfile << it2->first << " " << it2->second.freq;
				lastdocid = it2->first;
				int lastpos = 0;
				for (vector<postype>::iterator it3 = it2->second.postypes.begin(); it3 != it2->second.postypes.end(); ++it3)
				{
					myfile << " " << it3->pos - lastpos << "." << it3->type; //only write down the difference
					lastpos = it3->pos;
				}
				myfile << ";";
			}
			myfile << "\n";
		}
		myfile.close();
	}

	return 0;
}