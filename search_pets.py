"""
search_pets.py

	The script contains the functions that evaluates the search terms extracted from the CamFind API.
	Based on the terms, which may contain the actual name of the pet, the color, or its category,
	the fucntions gives the scores to the pet breeeds whose information  (read from breeds.json file)
	contains the most matched terms
	
"""

def find_pets_by_actual_name(breeds_score_dict, name_of_pet_ls, breeds_df_index):
	# the function that finds the pet names by the actual names
	# in this case, the extracted terms are scanned, and if the terms are matched with anything
	# inside the list of names of pet breeds, it gives the score of 2 to the dictionary that contains
	# the names of the pets and the associated scores

	# Pararmter
	# breeds_score_dict : a dictionary whose key is the name of the pet and whose value is the score
	# name_of_pet_ls : the list containing the terms extracted from the CamFind API reponses
	# breeds_df_index : the Series frame in pandas cotaning the list of pet names
    
    for i in range(0, len(breeds_df_index)):
        for a_word in name_of_pet_ls:
            if (a_word in breeds_df_index[i]):
                pet_label = breeds_df_index[i]
                breeds_score_dict[pet_label] = breeds_score_dict[pet_label] + 2
    

def find_pets_by_category(breeds_score_dict, name_of_pet_ls, breeds_df_category):
	# the function that finds the pet names by the category of the pet
	# in this case, the extracted terms are scanned, and if the terms are matched with anything
	# inside the type of category for pet breeds, it gives the score of 1 to the dictionary that contains
	# the names of the pets and the associated scores

	# Pararmter
	# breeds_score_dict : a dictionary whose key is the name of the pet and whose value is the score
	# name_of_pet_ls : the list containing the terms extracted from the CamFind API reponses
	# breeds_df_category : the category frame that contains the type of breeds (dog, cat) for each pet
    
    for i in range(0, len(breeds_df_category)):
        for a_word in name_of_pet_ls:
            if (breeds_df_category.iloc[i] != None):
                if (a_word in breeds_df_category.iloc[i]):
                    pet_label = breeds_df_category.index[i]
                    breeds_score_dict[pet_label] = breeds_score_dict[pet_label] + 1


def find_pets_by_color(breeds_score_dict, name_of_pet_ls, breeds_df_color):
	# the function that finds the pet names by the color of the pet, if the search term has it
	# in this case, the extracted terms are scanned, and if the terms are matched with anything
	# inside the type of category for pet breeds, it gives the score of 1 to the dictionary that contains
	# the names of the pets and the associated scores

	# Pararmter
	# breeds_score_dict : a dictionary whose key is the name of the pet and whose value is the score
	# name_of_pet_ls : the list containing the terms extracted from the CamFind API reponses
	# breeds_df_category : the category frame that contains the type of breeds (dog, cat) for each pet
    
    for i in range(0, len(breeds_df_color)):
        for a_word in name_of_pet_ls:
            if (breeds_df_color.iloc[i] != None):
                if (breeds_df_color.iloc[i]['colors']!=None):
                    if (a_word in breeds_df_color.iloc[i]['colors']):
                        pet_label = breeds_df_color.index[i]
                        breeds_score_dict[pet_label] = breeds_score_dict[pet_label] + 1