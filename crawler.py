import urllib2
import gzip
from google import search

def g_search(keywords, dataf, indexf):
    i = 0
    for url in search(keywords, num = 50, stop = 1000, only_standard=True):
        try:
            page = urllib2.urlopen(url)
        except Exception, e:
            #print 'error', i
            continue
        print i,
        html = page.read()     
        dataf.write(html+'\n')
        indexf.write(url+' '+str(len(html)+1)+ '\n')
        i = i+1
        if i>1000:
            return

def main():
    # with open('petlist.txt','r') as petlist:
    #     for line in petlist:
    #         line = line.rstrip()
    #         result = line.partition(' ')
    #         keywords = result[2]
    #         print '\n'+keywords
    #         filename = 'E:/Downloads/pets/Pages/'+result[0]
    #         dataf = gzip.open(filename+'_data', 'a')
    #         indexf = gzip.open(filename+'_index','a')
    #         g_search(keywords, dataf, indexf)
    #         dataf.close()
    #         indexf.close()
    # petlist.close()

    # i = 4
    while True:
        i = int(raw_input('\nNum:'))
        keywords = raw_input('Breed:')
        filename = 'E:/Downloads/pets/Pages/'+str(i)
        dataf = gzip.open(filename+'_data', 'w')
        indexf = gzip.open(filename+'_index','w')
        g_search(keywords, dataf, indexf)
        dataf.close()
        indexf.close()
        # i = i+1

if __name__== '__main__':
    main()  