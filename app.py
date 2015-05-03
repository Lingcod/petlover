"""
    app.py

    This file runs the full application. 
    It takes the image input using its image filename. Then, it goes through the search engine powered by
    CamFind API and retrieves the terms there. Using those terms, the code comapres the information about
    the pet breeds with the terms, and the one that contains the most matched terms gets the highest score.
    Given the top three selected pets, the user chooses one of them, and the C++ query search code will be
    used to return the search result.

"""

import image_recognition
import compare
import search_pets
import sys
import pandas as pd

# call the search result from CamFind API
search_result = image_recognition.get_image_result(sys.argv[1]) # sys.argv[1] == the name of image input file

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

which_pet_to_look_at = raw_input('which pet do you want to look at? ' + str(ipresult) + " ")

while (which_pet_to_look_at not in ipresult):
  which_pet_to_look_at = raw_input('Not in the list. Try it again. ' + str(ipresult) + " ")


# evaluate the query using the search query terms


# estimate the similarity score.
sim2 = compare.similarity(breeds_json, list = ipresult)
simlist = compare.findsims(breeds_json, sim = sim2)

# using the list of similar breeds, 





"""
import os
from flask import Flask, render_template, request, jsonify

import unirest
from time import sleep


app = Flask(__name__)

@app.route('/')
def main():
    return render_template('form_submit.html')

@app.route('/search', methods=['POST'])
def search():
 
    if request.method == "POST":
 
        a_file = request.files['image_file']
        
        try:

            response = unirest.post("https://camfind.p.mashape.com/image_requests",
                 headers={
                "X-Mashape-Key": "rkv1zmf6dgmshNzA9gAFDc13pB6Qp139uVejsnKlCuSjdse91c"
                },
                  params={
                "image_request[image]": a_file,
                "image_request[language]": "en",
                "image_request[locale]": "en_US"
                }
              )
              

            api_key = 'JAPQbhGVyMmshUB7QBAbnO0Xsxqcp1T10Fbjsn0LR4gjoptFqH'
            token = response.body['token']

            sleep(25)

            response_2 = unirest.get("https://camfind.p.mashape.com/image_responses/" + response.body['token'],
                  headers={
                      "X-Mashape-Key": "rkv1zmf6dgmshNzA9gAFDc13pB6Qp139uVejsnKlCuSjdse91c",
                      "Accept": "application/json"
                  }
            )

            a = open("image_recognition_result.json", 'w')
            pickle.dump(response_2.body, a)
            a.close()

        except:
 
            # return error
            jsonify({"sorry": "Sorry, no results! Please try again."}), 500

"""


if __name__ == '__main__':
    app.run()