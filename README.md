# petlover

Crawler.py
----------------
keywords according to petlist.txt. For ambiguous name, I add dog/cat as suffix. 
10950 pages in totall, stored in 0_data/index - 36_data/index

invertedindex
---------------------
 - Gernerated by Microsoft Visual Studio
 - Build up by 3 projects: invertedindex, mergesort and query
 - [All inputs and outputs file in google drive](https://drive.google.com/file/d/0BxQ3OgSlSbhCYXBQTzZxYUNGaTA/view?usp=sharing)
 - To revise file directories. Search "sprintf" in code. Substitute path before "\\Pages\\..." with your path
 - To run invertedindex(which is not necessary if you downloaded all inputs and outputs files from google drive), copy zlib-1.2.5 and zlib125dll folders into \invertedindex\invertedindex\
 - To search a term, only use query.cpp and outputs from last stage. It will return 10 results
 - To compile all projects. Run invertedindex.cpp first, then mergesort.cpp. query.cpp will be the last one.
