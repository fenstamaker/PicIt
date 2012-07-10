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
			
			// Root Node
			
			String rootText = file.readLine();
			
			String rootName = rootText.split("@")[1].split("\"")[0];
			
			root = new DecisionNode(rootName);
			nodes.put(rootName, root);
			
			String rootAttribute = rootText.split("label=\"")[1].split("[ ]<")[0].trim();
			String rootOperation = rootText.split("label=\"[a-z]+")[1].split("[0-9]+")[0].trim();
			float rootValue = Float.parseFloat( rootText.split("[<>][ ]")[1].split("[ ][(]score")[0].trim() );

			root.setAttribute(rootAttribute);
			root.setOperation(rootOperation);
			root.setThreshold(rootValue);
			
			// Root's First Child
			
			rootText = file.readLine();

			String rootChild = rootText.split("@")[2].split("\"")[0];
			
			DecisionNode rootChildNode = new DecisionNode(rootChild);
			root.setLeftNode(rootChildNode);
			nodes.put(rootChild, rootChildNode);
			
			// Root's Second Child
			
			rootText = file.readLine();

			rootChild = rootText.split("@")[2].split("\"")[0];
			
			rootChildNode = new DecisionNode(rootChild);
			root.setRightNode(rootChildNode);
			nodes.put(rootChild, rootChildNode);
			
			// All other Nodes
			String text = file.readLine();
			while ( text != null )
			{
				String nodeType = text.split("[.]")[6].split("@")[0];
				
				if ( nodeType.equals("ScoreTestNode") ) {
					String nodeName = text.split("@")[1].split("\"")[0];
					DecisionNode node;
					
					if ( nodes.containsKey(nodeName) ) {
						node = nodes.get(nodeName);
					} else { 
						node = new DecisionNode(nodeName);
						nodes.put(nodeName, node);
					}
					
					String attribute = text.split("label=\"")[1].split("[ ]<")[0].trim();
					String operation = text.split("label=\"[a-z]+")[1].split("[0-9]+")[0].trim();
					float value = Float.parseFloat( text.split("[<>][ ]")[1].split("[ ][(]score")[0].trim() );
					
					node.setAttribute(attribute);
					node.setOperation(operation);
					node.setThreshold(value);
					
					// Root's First Child
					
					text = file.readLine();

					String nodeChild = text.split("@")[2].split("\"")[0];
					
					DecisionNode child = new DecisionNode(nodeChild);
					node.setLeftNode(child);					
					nodes.put(nodeChild, child);
					
					// Root's Second Child
					
					text = file.readLine();
					nodeChild = text.split("@")[2].split("\"")[0];
					
					child = new DecisionNode(nodeChild);
					node.setRightNode(child);
					nodes.put(nodeChild, child);
				} else if ( nodeType.equals("LeafNode") ) {
					String nodeName = text.split("@")[1].split("\"")[0];
					DecisionNode node;
					
					if ( nodes.containsKey(nodeName) ) {
						node = nodes.get(nodeName);
					} else { 
						node = new DecisionNode(nodeName);
						nodes.put(nodeName, node);
					}

					String classification = text.split("-[ ]")[1].split("Weight")[0].trim();

					node.setAsLeaf(classification);
					
				}
				
				text = file.readLine();
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		
		String result = root.expand(100, 100, 100);
		System.out.println(result);
				
	}
	
	
	public int classify(int[] values)
	{
		
		
		return 0;
	}
}
