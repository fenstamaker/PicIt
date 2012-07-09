package edu.montclair.hci.picit.ml;

import java.util.HashMap;

public class DecisionNode {

	private String name;
	private float threshold;
	private DecisionNode left;
	private DecisionNode right;
	private boolean isLeaf;
	private String classification;
	
	public DecisionNode(String name)
	{
		
	}
	
	public DecisionNode(String lineFromFile, HashMap<String, DecisionNode> otherNodes)
	{
		// parse the node
		
		// get name
		
		otherNodes.add(name, this);
		
	}
	
	public void setAttributes(String fileLine)
	{
		
	}
	
}
