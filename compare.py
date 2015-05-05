"""
	compare.py

"""
import json
import pickle
import pandas as pd

def read_breed_json(filename):
    # the function that reads the json file that contains the information about the 37 different
    # pets and returns the json object 
    # Paramter
    # filename : the json filename
    # Returns
    # j : the dictionary that contains the json structure of the pet breeds

    f = open(filename)
    j = json.loads(f.read())
    return j

#return -1 if 1.len(list)!=3, 2.one or more petname can't be find in json, 3.a mix of dog and cat in list
def similarity(j, list):
	sim1 = { "origin":[], "category":[], "weight":{"male":{"amin":0, "amax":0}, "female":{"amin":0, "amax":0}}, "height":{"male":{"amin":0, "amax":0}, "female":{"amin":0, "amax":0}}, "coat":{"length":None, "description":None}, "color":{"feature":[], "colors":[], "description":None}, "littersize":{"amin":0, "amax":0}, "lifespan":{"amin":0, "amax":0} }
	sim2 = { "origin":[], "category":[], "weight":{"male":{"amin":0, "amax":0}, "female":{"amin":0, "amax":0}}, "height":{"male":{"amin":0, "amax":0}, "female":{"amin":0, "amax":0}}, "coat":{"length":None, "description":None}, "color":{"feature":[], "colors":[], "description":None}, "littersize":{"amin":0, "amax":0}, "lifespan":{"amin":0, "amax":0} }

	if len(list) != 3:
		print "list size must be 3"
		return -1
	if set(list).issubset(set(j.keys()))==False :
		print list, "one or more petname cannot be find in json"
		return -1

	score1 = simscore(petA = j[list[0]], petB = j[list[1]], sim = sim1, is2nd = False)
	score2 = simscore(petA = j[list[2]], petB = sim1, sim = sim2, is2nd = True)
	if score1 == -1 or score2 == -1:
		print "Wrong input. There is a mix of cat and dog"
		return -1
	return sim2

# Calculate a score for given petA and pet B. Store shared features in sim. Return score. Dog and cat: -1
def simscore(petA, petB, sim, is2nd):
	score = 0

	#cat or dog. If different species, score = -1
	if petA["category"] and petB["category"]:
		if (petA["category"][0] == "cat" and petB["category"][0] != "cat") or (petA["category"][0] != "cat" and petB["category"][0] == "cat"):
			return -1
	elif (petA["category"] and petA["category"][0] == "cat") or (petB["category"] and petB["category"][0] == "cat"):
		return -1;

	#origin country. +number*1
	sim["origin"] = [val for val in petA["origin"] if val in petB["origin"]]
	score += len(sim["origin"])

	#category. +number*2
	sim["category"] = [val for val in petA["category"] if val in petB["category"]]
	score += len(sim["category"])*2

	#weight
	if petA["weight"] and petB["weight"] :
		mmin = max(petA["weight"]["male"]["amin"], petB["weight"]["male"]["amin"])
		mmax = min(petA["weight"]["male"]["amax"], petB["weight"]["male"]["amax"])
		fmin = max(petA["weight"]["female"]["amin"], petB["weight"]["female"]["amin"])
		fmax = min(petA["weight"]["female"]["amax"], petB["weight"]["female"]["amax"])
		if mmin > mmax or fmin > fmax :
			sim["weight"] = None
		else:
			sim["weight"]["male"]["amin"] = mmin
			sim["weight"]["male"]["amax"] = mmax
			sim["weight"]["female"]["amin"] = fmin
			sim["weight"]["female"]["amax"] = fmax
			score += ((mmax - mmin)/mmax + (fmax - fmin)/fmax)*10
	else:
		sim["weight"] = None

	#height (cats does not have "height" property)
	if petA["height"] and petB["height"]:
		mmin = max(petA["height"]["male"]["amin"], petB["height"]["male"]["amin"])
		mmax = min(petA["height"]["male"]["amax"], petB["height"]["male"]["amax"])
		fmin = max(petA["height"]["female"]["amin"], petB["height"]["female"]["amin"])
		fmax = min(petA["height"]["female"]["amax"], petB["height"]["female"]["amax"])
		if mmin > mmax or fmin > fmax :
			sim["height"] = None
		else:
			sim["height"]["male"]["amin"] = mmin
			sim["height"]["male"]["amax"] = mmax
			sim["height"]["female"]["amin"] = fmin
			sim["height"]["female"]["amax"] = fmax
			score += ((mmax - mmin)/mmax + (fmax - fmin)/fmax)*10
	else:
		sim["height"] = None

	#coat samelength:+5
	if petA["coat"] and petB["coat"] and petA["coat"]["length"] == petB["coat"]["length"]:
		score += 5

	#color +numberofsamecolor*2+numberofsamefeature*2
	#when feature/colors = None, it means it could have any features/colors
	if petA["color"] and petB["color"]:
		if petA["color"]["feature"] and petB["color"]["feature"]:
			sim["color"]["feature"] = [val for val in petA["color"]["feature"] if val in petB["color"]["feature"]]
			score += len(sim["color"]["feature"]) * 2
		elif is2nd==False or petA["color"]["feature"]==None: 
				score += 5

		if petA["color"]["colors"] and petB["color"]["colors"]:
			sim["color"]["colors"] = [val for val in petA["color"]["colors"] if val in petB["color"]["colors"]]
			score += len(sim["color"]["colors"])*2
		elif is2nd==False or petA["color"]["colors"]==None:
			score += 5
	elif is2nd==False or petA["color"]==None:
		score += 10

	#littersize
	if petA["littersize"] and petB["littersize"]:
		amin = max(petA["littersize"]["amin"], petB["littersize"]["amin"])
		amax = min(petA["littersize"]["amax"], petB["littersize"]["amax"])
		if amin > amax or amin == 0:
			sim["littersize"]["amin"] = 0
			sim["littersize"]["amax"] = 0
		else:
			sim["littersize"]["amin"] = amin
			sim["littersize"]["amax"] = amax
			score += ((amax - amin)/amax)*5

	#lifespan
	if petA["lifespan"] and petB["lifespan"]:
		amin = max(petA["lifespan"]["amin"], petB["lifespan"]["amin"])
		amax = min(petA["lifespan"]["amax"], petB["lifespan"]["amax"])
		if amin > amax or amin == 0:
			sim["lifespan"]["amin"] = 0
			sim["lifespan"]["amax"] = 0
		else:
			sim["lifespan"]["amin"] = amin
			sim["lifespan"]["amax"] = amax
			score += ((amax - amin)/amax)*5

	return score

#find 3 other similar pets for given features. return a dictionary sorted by key(i.e score value, ascending), only contains 3 results.
def findsims(j, sim):
	simlist = {}
	temp = { "origin":[], "category":[], "weight":{"male":{"amin":0, "amax":0}, "female":{"amin":0, "amax":0}}, "height":{"male":{"amin":0, "amax":0}, "female":{"amin":0, "amax":0}}, "coat":{"length":None, "description":None}, "color":{"feature":[], "colors":[], "description":None}, "littersize":{"amin":0, "amax":0}, "lifespan":{"amin":0, "amax":0} }
	for petname in j.keys():
		score = simscore(petA = j[petname], petB = sim, sim = temp, is2nd = False)
		while score in simlist:
			score += 0.01
		simlist[score] = petname

	sortedkey = sorted(simlist, reverse = True)
	sorteddic = {}
	for i in range(3):
		sorteddic[sortedkey[i]] = simlist[sortedkey[i]]

	return sorteddic