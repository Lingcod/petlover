"""
    app.py

    This file actually runs the application. It takes the image input and gets the final document result from




    
"""

import image_recognition
import compare
import search_pets
import sys
import pandas as pd


# call the search result from CamFind API
search_result = image_recognition.get_image_result(sys.argv[1]) # sys.argv[1] == the name of image input file

# call the json file 
breeds_json = comapre.read_breed_json('breeds.json')
breeds_df = pd.DataFrame.from_dict(breeds_json, orient='index')

# the terms extracted from the search_result variables should be contained in the list type (name_of_pet_ls)
terms_ls = search_result['name'].split(" ")

# initialize the dictionary that contains the scores 
breeds_score_dict = pd.Series([0]*len(breeds_df.index), index = breeds_df.index).to_dict()

# score the pets that contain the most matched terms of the CamFind API
search_pets.find_pets_by_actual_name(breeds_score_dict, terms_ls, breeds_df.index)
search_pets.find_pets_by_category(breeds_score_dict, terms_ls, breeds_df['category'])
search_pets.find_pets_by_color(breeds_score_dict, termsls, breeds_df['color'])

# extract the top three
top_three_pets = sorted(breeds_score_dict.iteritems(), key=lambda (k, v): (-v, k))[:3]
ipresult = []
for a_tuple in top_three_pets:
    ipresult.append(a_tuple[0])




# estimate the similarity score.
sim2 = similarity(breeds_json, list = ipresult)
simlist = findsims(breeds_json, sim = sim2)





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