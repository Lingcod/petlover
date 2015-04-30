package VisualSearchEngine.VisualSearchEngine;

import java.util.Arrays;
import java.util.HashMap;
import java.io.File;

import org.nd4j.linalg.api.ndarray.INDArray;
import org.nd4j.linalg.dataset.DataSet;
import org.deeplearning4j.util.SerializationUtils;
import org.deeplearning4j.models.classifiers.dbn.DBN;
import org.deeplearning4j.eval.Evaluation;

public class App 
{
	public static void main(String[] args) throws Exception{
		
		// crop the images
		/*
		File image_folder = new File("./images");
		
		for (File fileEntry : image_folder.listFiles()) {
			String new_filename = fileEntry.getName();
			if (!new_filename.equals(".DS_Store")){
				Image a_image = new Image(new_filename);
				a_image.crop_image();
			}else{
				continue;
			}
		}
		*/
		
		// load the saved model
		DBN a_model = SerializationUtils.readObject(new File("./output_folder/model_trained_3.ser"));
		
		// get the new image
		Image a_image = new Image("./images/american_bulldog_26.jpg", 0);
		//DataSet vectorized_image = a_image.getVectorizedDataSet();
		
        int[] predict = a_model.predict(a_image.returnImageMatrix());
        System.out.println("Predict " + Arrays.toString(predict));

        
		
	}

}
