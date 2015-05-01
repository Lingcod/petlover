import unirest
from time import sleep
import pickle

"""
 image_recognition.py
 
 the script uses the camfind API to recognize the image of a pet
 
 
"""
 
response = unirest.post("https://camfind.p.mashape.com/image_requests",
     headers={
    "X-Mashape-Key": "rkv1zmf6dgmshNzA9gAFDc13pB6Qp139uVejsnKlCuSjdse91c"
    },
      params={
    "image_request[image]": open("basset_hound_155.jpg", mode="r"),
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


a = open("image_recognition_result.json", 'w')
pickle.dump(response_2.body, a)
a.close()

  
  



