"""
    app.py

    This file runs the full application. 
    It takes the image input using its image filename. Then, it goes through the search engine powered by
    CamFind API and retrieves the terms there. Using those terms, the code comapres the information about
    the pet breeds with the terms, and the one that contains the most matched terms gets the highest score.
    Given the top three selected pets, the user chooses one of them, and the python query search code will be
    used to return the search result.

"""

import image_recognition
import compare
import search_pets
#import query_search
import sys
import pandas as pd
import time

# call the search result from CamFind API
search_result = image_recognition.get_image_result(sys.argv[1]) # sys.argv[1] == the name of image input file

print "image search result " + str(search_result)

# call the json file 
breeds_json = compare.read_breed_json('breeds.json')
breeds_df = pd.DataFrame.from_dict(breeds_json, orient='index')

# the terms extracted from the search_result variables should be contained in the list type (name_of_pet_ls)
terms_ls = search_result['name'].split(" ")

# initialize the dictionary that contains the scores 
breeds_score_dict = pd.Series([0]*len(breeds_df.index), index = breeds_df.index).to_dict()

# score the pets that contain the most matched terms of the CamFind API
search_pets.find_pets_by_actual_name(breeds_score_dict, terms_ls, breeds_df.index)
search_pets.find_pets_by_category(breeds_score_dict, terms_ls, breeds_df['category'])
search_pets.find_pets_by_color(breeds_score_dict, terms_ls, breeds_df['color'])

# extract the top three
top_three_pets = sorted(breeds_score_dict.iteritems(), key=lambda (k, v): (-v, k))[:3]
ipresult = []
for a_tuple in top_three_pets:
    ipresult.append(a_tuple[0])

print ipresult
# estimate the similarity score.
sim2 = compare.similarity(breeds_json, list = ipresult)
simlist = compare.findsims(breeds_json, sim = sim2)
print simlist

# using the list of similar breeds, use the result from similarity list to find the documents
# which_pet_to_look_at = raw_input('which pet do you want to look at? ' + str(simlist) + " ")

# while (which_pet_to_look_at not in ipresult):
#   which_pet_to_look_at = raw_input('Not in the list. Try it again. ' + str(simlist) + " ")


# evaluate the query using the search query terms
'''
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
LOOK AT THIS PART!!!!!!!!!!!!!!!!!!

YOU NEED TO PROVIDE THE FILENAME OF THE DOCUMENT HASHTABLE AND THE DIRECTORY TO THE FOLDER WHERE YOU HAVE
THE INVERTED INDEX FILES. 
e.g. document hashtable : doc_hashtable.txt
  inverted index file list : ./inverted_index_foldername/  (you must put / at both ends)

I know because we don't have time we have to use this, but just run it, and if it works, that's fine.

I'm not sure about how you are going to use the similarity 

""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
'''
how_many = 10
k = 20
start_time = time.clock()
# document_hashtable_filename = ""
# directory_to_inverted_index_files = ""
# query_search.search(which_pet_to_look_at, k, how_many, document_hashtable_filename, directory_to_inverted_index_files)
import PyQuery
PyQuery.petsearch(ipresult, simlist)
end_time = time.clock()
print "run-time : " + str((end_time - start_time)) + " seconds."  