import unirest
from time import sleep
import pickle

"""
  image_recognition.py

  The script uses the camfind API to recognize the image of a pet. The function, get_image_result
  takes the filename of the image as an input and returns the string containing the search term
  that CamFind API predicts.
 
"""
def get_image_result(image_filename):
    # the function that uses the filname of the image, processes the image analyiss using CamFind API,
    # extracts the terms that are relevant to the image

    # Paramter
    # image_filename : string contaning the name of image file.
    # Returns
    # response_2.body : the dictionary containing the name of the search term as well as the status
    # of the response
    
    response = unirest.post("https://camfind.p.mashape.com/image_requests",
         headers={
        "X-Mashape-Key": "rkv1zmf6dgmshNzA9gAFDc13pB6Qp139uVejsnKlCuSjdse91c"
        },
          params={
        "image_request[image]": open(image_filename, mode="r"),
        "image_request[language]": "en",
        "image_request[locale]": "en_US"
        }
      )
      

    api_key = 'JAPQbhGVyMmshUB7QBAbnO0Xsxqcp1T10Fbjsn0LR4gjoptFqH'
    token = response.body['token']

    sleep(30)

    response_2 = unirest.get("https://camfind.p.mashape.com/image_responses/" + response.body['token'],
          headers={
              "X-Mashape-Key": "rkv1zmf6dgmshNzA9gAFDc13pB6Qp139uVejsnKlCuSjdse91c",
              "Accept": "application/json"
          }
      )
    return response_2.body