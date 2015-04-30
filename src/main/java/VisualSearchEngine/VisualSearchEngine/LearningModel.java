package VisualSearchEngine.VisualSearchEngine;

import java.io.File;

import java.util.ArrayList;
import java.util.HashMap;

import org.apache.commons.math3.random.MersenneTwister;
import org.apache.commons.math3.random.RandomGenerator;

import org.nd4j.linalg.api.ndarray.INDArray;
import org.nd4j.linalg.lossfunctions.LossFunctions;
import org.nd4j.linalg.api.activation.Activations;

import org.deeplearning4j.nn.conf.NeuralNetConfiguration;
import org.deeplearning4j.models.featuredetectors.rbm.RBM;
import org.deeplearning4j.distributions.Distributions;
import org.deeplearning4j.nn.WeightInit;
import org.deeplearning4j.models.classifiers.dbn.DBN;
import org.deeplearning4j.util.SerializationUtils;


public class LearningModel {
	
	private HashMap<Integer, String> labels = new HashMap<Integer, String>();
	private ArrayList<String> filenames = new ArrayList<String>();
	
	private NeuralNetConfiguration conf_neural;
	private DBN dbn_model;
	
	int n_features;
	int n_labels;
	
	public LearningModel(String folder_dir, int n_columns, int n_classes){
		// the constructor that initializes the learning model
		// at first, it generates the random number generator 
		// and it uses the NueralNetConfiguration as well as DBN from Deeplearning4j to build
		// the basic foundation of the neural network model
		
		// Parameter
		// folder_dir : a string contaning the directory to the  files
		// n_examples : the number of examples to be used to train the model
		// n_classes : the number of labels
		
		/*GET THE LABEL INFORMATION*/
		// create a variable named filenames to contain a list of filenames of the images
		// in the variable filenames
        File folder = new File(folder_dir);
        this.listFilesForFolder(folder);
        
        // create a list that contains the name of pets (the name of classes), using the 
        // attribute filenames. The result should be stored in a hashMap
        this.create_labels();
        
        
		RandomGenerator gen = new MersenneTwister(123);
		
		n_features = n_columns; // n_examples must be 7389
		n_labels = n_classes; // n_classes must be 37
		
		// configure the neural netƒ
		conf_neural = new NeuralNetConfiguration.Builder()
			.weightInit(WeightInit.DISTRIBUTION)
        	.visibleUnit(RBM.VisibleUnit.GAUSSIAN).render(10)
        	.hiddenUnit(RBM.HiddenUnit.RECTIFIED)
        	.lossFunction(LossFunctions.LossFunction.RECONSTRUCTION_CROSSENTROPY).rng(gen)
        	.learningRate(1e-3f).nIn(n_features).nOut(n_labels).build();
		
		// use the Deep Belief Network model 
		dbn_model = new DBN.Builder().configure(conf_neural)
		        .hiddenLayerSizes(new int[]{500, 250, 125, 60})
		        .build();
		
		dbn_model.getOutputLayer().conf().setNumIterations(10);

		NeuralNetConfiguration.setClassifier(dbn_model.getOutputLayer().conf());
		
	}
	
    private void listFilesForFolder(File folder) {
    	// given the folder directory, read the names of all the image files
    	// and store them into the ArrayList<String>
    	
    	// Parameter
    	// folder : a File object that contains the list of image filenames
    	
    	// for each file read from the directory, check whether it is the directory instead of the file
    	// if so, make a recursive call to read the actual file
    	// if not, it is the file, so the function must read the filename and add that to the list
    	
    	HashMap<String, Integer> a_hashmap = new HashMap<String, Integer>();
        for (File fileEntry : folder.listFiles()) {
            if (fileEntry.isDirectory()) {
                listFilesForFolder(fileEntry);
            } else {
	            	if (!filenames.contains(folder + "/" + fileEntry.getName())){
	            		// if this is a new file, don't contain it
	            		String a_filename = folder + "/" + fileEntry.getName();
	            		if (a_filename.equals("./cropped_images/.DS_Store")){
	            			continue;
	            		}else{
	                		String new_filename = fileEntry.getName().replaceAll("[0-9]", "");
	                		String extension = new_filename.substring(new_filename.length()-5, new_filename.length());
	                		String new_label = new_filename.replaceAll(extension, "");
	                		
	                		if (a_hashmap.containsKey(new_label)){
	                			int new_val = a_hashmap.get(new_label) + 1;
	                			a_hashmap.put(new_label, new_val);
	                		}else{
	                			int first_val = 1;
	                			a_hashmap.put(new_label, first_val);
	                		}
	                		
	                		if (a_hashmap.get(new_label) < 50){
	                			filenames.add(folder + "/" + fileEntry.getName());
	                		}
	            		}
	            	}
            }
        }
        if (filenames.contains(folder + "/.DS_Store")){
        	System.out.println("the list contains DS_Store");
        }else{
        	System.out.println("the list does not contains DS_Store");
        }
        boolean a = filenames.remove(folder + "/.DS_Store");
        if(a){System.out.println("filename removed");}
    }
    
    private void create_labels(){
    	// using the names of the files read from the image folder directory,
    	// the function creates a hash map whose key is the name of the image and 
    	// the integer is the label number
    	
    	// Parameter:
    	// filenames : the list of filenames of the images to read
    	
    	int val = 0;
    	for (int i = 0; i < filenames.size(); i++){
    		String new_filename = filenames.get(i).replaceAll("[0-9]", "");
    		String extension = new_filename.substring(new_filename.length()-5, new_filename.length());
    		new_filename = new_filename.replaceAll(extension, "");
    		String image_folder_directory = new_filename.substring(0, 17);
    		new_filename = new_filename.replaceAll(image_folder_directory, "");
    		if (!labels.containsValue(new_filename)){
    			labels.put(val, new_filename);
    			val++; // increment it, and insert the value afterward
    		}
    		
    	}
    }
	
	public void train_model() throws Exception {
		// train the model using the data read from the attribute filenames
		// it goes through the loop to fit each DataSet object to the model
		
		// for each image, get the label as well as the feature data about the image
		// get the DataSet object from this data
		// and fit the data into the model at each iterations
		
        for (int i = 0; i < filenames.size(); i++){
        	try{
        		// get the label
        		String label = filenames.get(i).replaceAll("[0-9]", "");
        		String extension = label.substring(label.length()-5, label.length());
        		label = label.replaceAll(extension, "");
        		String image_folder_directory = label.substring(0, 17);
        		label = label.replaceAll(image_folder_directory, "");
        		
        		// use the Image class created from Image.java to label the image as well as vectorize
        		// the image in order to convert to DataSet object
        		int label_num = -1;
        		for (int j = 0; j < labels.size(); j++){
        			if (labels.get(j)==label){
        				label_num = j;
        			}
        		}
        		
        		Image a_image = new Image(filenames.get(i), label_num);
        		//a_image = new Image(filenames.get(i), label_num);
        		
        		INDArray a_data = a_image.returnImageMatrix();
        		System.out.println(Integer.toString(i));
        	
        		// fit the data into the learning model
        		dbn_model.fit(a_data);
        		
        	}catch(java.lang.NullPointerException e){
        		continue;
        	}catch(java.awt.color.CMMException e){
        		continue;
        	}catch(java.lang.IllegalArgumentException e){
        		continue;
        	} // USE MAPREDUCE FOR THIS!!!!!!!!!
        }
		
	}

	public void save_model(String modelname){
		// saves the model to somewhere for future use
		SerializationUtils.saveObject(dbn_model, new File(modelname));
	}
	
    public int n_examples() {return filenames.size();}
    
    public HashMap<Integer, String> labels() {
    	// return the labels of the type HashMap
    	// Returns
    	// labels : the HashMap type whose key is the label number and whose value is the name of the pet 
    	return labels;
    }
    
    public ArrayList<String> getFilenames() { return filenames;}
        
    public static void main(String[] args) throws Exception{
    	// the main file
    	
		// read the data file
		LearningModel learning_model_obj = new LearningModel("./cropped_images", 1000, 37);
		System.out.println(learning_model_obj.n_examples());
				
		learning_model_obj.train_model();
		learning_model_obj.save_model("./output_folder/model_trained_4.ser");
		
		
	}
}
