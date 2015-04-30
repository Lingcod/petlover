package VisualSearchEngine.VisualSearchEngine;

import java.io.File;
import java.io.FileReader;
import java.io.BufferedReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.imageio.ImageIO;

import java.awt.image.BufferedImage;

import org.deeplearning4j.datasets.vectorizer.*;
import org.deeplearning4j.util.ImageLoader;
import org.nd4j.linalg.dataset.DataSet;
import org.nd4j.linalg.api.ndarray.INDArray;

public class Image {
	
	private Vectorizer v;
	//private DataSet d;
	
	private INDArray image_matrix;
	private ImageLoader image_loader_obj;
	private int[][] rastered;
	private int[] flattened;
	int label;
	String label_name;
	int height;
	int width;
	
	public Image(String filename){
		String extension = filename.substring(filename.length()-4, filename.length());
		label_name = filename.replaceAll(extension, "");
	}

	public Image(String filename, int actual_label) throws Exception {
		label_name = filename.replaceAll("[0-9]", "");
		String extension = filename.substring(filename.length()-4, filename.length());
		label_name = filename.replaceAll(extension, "");
		String image_folder_directory = label_name.substring(0, 17);
		label_name = label_name.replaceAll(image_folder_directory, "");
		
		System.out.println(label_name);
		
		image_loader_obj = new ImageLoader(1000, 1000);
		label = actual_label;
		
		image_matrix = image_loader_obj.asMatrix(new File(filename));
		rastered = image_loader_obj.fromFile(new File(filename));
		flattened = image_loader_obj.flattenedImageFromFile(new File(filename));
		
	}
	
	public DataSet getVectorizedDataSet() {
		// returns the vectorized image dataset using the private attribute v
		return v.vectorize();
	}
	
	public INDArray returnImageMatrix() { return image_matrix;}
	
	public int[][] returnRastered() {return rastered;}
	
	public int[] returnFlattened() {return flattened;}
	
	public void crop_image() throws Exception{
		// use the annotation information to crop the image
		File annotations = new File("./annotations/xmls");
		String a_filename = this.listFilesForFolder(annotations);
		
		if (!a_filename.equals("NONE")){
			System.out.println("found the xml file : " + a_filename);
			BufferedReader file_reader = new BufferedReader(new FileReader("./annotations/xmls/" + a_filename));

			String line = file_reader.readLine();
			
			int xmin_idx = line.indexOf("<xmin>") + 6;
			int xmin_slash_idx = line.indexOf("</xmin>");
			int xmax_idx = line.indexOf("<xmax>") + 6;
			int xmax_slash_idx = line.indexOf("</xmax>");
			int ymin_idx = line.indexOf("<ymin>") + 6;
			int ymin_slash_idx = line.indexOf("</ymin>");
			int ymax_idx = line.indexOf("<ymax>") + 6;
			int ymax_slash_idx = line.indexOf("</ymax>");
			
			int xmin = Integer.valueOf(line.substring(xmin_idx, xmin_slash_idx));
			int xmax = Integer.valueOf(line.substring(xmax_idx, xmax_slash_idx));
			int ymin = Integer.valueOf(line.substring(ymin_idx, ymin_slash_idx));
			int ymax = Integer.valueOf(line.substring(ymax_idx, ymax_slash_idx));
			
			file_reader.close();
			
			a_filename = a_filename.replace(".xml", ".jpg");
			
			// crop the image
			BufferedImage a_image = ImageIO.read(new File("./images/" + a_filename));
			BufferedImage out = a_image.getSubimage(xmin, ymin, xmax-xmin, ymax-ymin);
			height = ymax - ymin;
			width = xmax - xmin;
			
			ImageIO.write(out, "jpg", new File("./cropped_images/" + a_filename));
		}
	}
	
    public String listFilesForFolder(File folder) throws Exception{
    	// given the folder directory, read the names of all the image files
    	// and store them into the ArrayList<String>
    	
    	// Parameter
    	// folder : a File object that contains the list of image filenames
    	
    	// for each file read from the directory, check whether it is the directory instead of the file
    	// if so, make a recursive call to read the actual file
    	// if not, it is the file, so the function must read the filename and add that to the list
    
        for (File fileEntry : folder.listFiles()) {
            if (fileEntry.isDirectory()) {
                listFilesForFolder(fileEntry);
            } else {
	            String filename = fileEntry.getName();
	            String extension = filename.substring(filename.length()-4, filename.length());
	            String string_to_compare = filename.replace(extension, "");
	            if (string_to_compare.equals(label_name)){
	            	return filename;
	            }
            }
        }
        
        return "NONE";

    }	
}
