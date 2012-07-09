package edu.montclair.hci.picit.ml;

import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;

import android.util.Log;

public class DecisionTree {

	private ArrayList<String> attributes;
	private DecisionNode root;
	
	public DecisionTree(String filename)
	{
		// open file
		BufferedReader file;
		
		try {
			file = new BufferedReader(new FileReader(filename));
			
			String header = file.readLine();
			
			// hashmap of all decision nodes
			HashMap<String, DecisionNode> nodes = new HashMap<String, DecisionNode>();
			
			String rootText = file.readLine();
			
			root = new DecisionNode(rootText, nodes);

			String text = file.readLine();
			// read through file and create nodes
			while ( text != null )
			{
				String[] parts = text.split("@");
				String name;
				if ( parts.length < 3 )
					name = parts[1].split("\"")[0];
				else 
					name = parts[2].split("\"")[0];
				
				DecisionNode dn = new DecisionNode(name);
				nodes.put(name, dn);
				
				text = file.readLine();
			}
		} catch (FileNotFoundException e) {
			Log.e("DECISIONTREE", "Could not find tree file");
			e.printStackTrace();
		} catch (IOException e) {
			Log.e("DECISIONTREE", "IO Error when reading line");
			e.printStackTrace();
		}
		
		
				
	}
	
	
	public int classify(int[] values)
	{
		
		
		return 0;
	}
}
