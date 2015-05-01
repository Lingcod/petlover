import os
from flask import Flask, render_template, request, jsonify

from pyimagesearch.colordescriptor import ColorDescriptor
from pyimagesearch.searcher import Searcher

import unirest
from time import sleep

@app.route('/search', methods=['POST'])
def search():
 
    if request.method == "POST":
 
        RESULTS_ARRAY = []
 
        # get url
        image_url = request.form.get('img')
 
        try:

            response = unirest.post("https://camfind.p.mashape.com/image_requests",
                 headers={
                "X-Mashape-Key": "rkv1zmf6dgmshNzA9gAFDc13pB6Qp139uVejsnKlCuSjdse91c"
                },
                  params={
                "image_request[image]": open("samoyed_60.jpg", mode="r"),
                "image_request[language]": "en",
                "image_request[locale]": "en_US"
                }
              )
              

            api_key = 'JAPQbhGVyMmshUB7QBAbnO0Xsxqcp1T10Fbjsn0LR4gjoptFqH'
            token = response.body['token']

            sleep(10)

            response_2 = unirest.get("https://camfind.p.mashape.com/image_responses/" + response.body['token'],
                  headers={
                      "X-Mashape-Key": "rkv1zmf6dgmshNzA9gAFDc13pB6Qp139uVejsnKlCuSjdse91c",
                      "Accept": "application/json"
                  }
            )
 
            # initialize the image descriptor
            cd = ColorDescriptor((8, 12, 3))
 
            # load the query image and describe it
            from skimage import io
            import cv2
            query = io.imread(image_url)
            query = (query * 255).astype("uint8")
            (r, g, b) = cv2.split(query)
            query = cv2.merge([b, g, r])
            features = cd.describe(query)
 
            # perform the search
            searcher = Searcher(INDEX)
            results = searcher.search(features)
 
            # loop over the results, displaying the score and image name
            for (score, resultID) in results:
                RESULTS_ARRAY.append(
                    {"image": str(resultID), "score": str(score)})
 
            # return success
            return jsonify(results=(RESULTS_ARRAY[:3]))
 
        except:
 
            # return error
            jsonify({"sorry": "Sorry, no results! Please try again."}), 500


app = Flask(__name__)

if __name__ == '__main__':
    app.run()