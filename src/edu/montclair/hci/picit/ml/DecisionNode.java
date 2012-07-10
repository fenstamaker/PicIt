package edu.montclair.hci.picit.ml;

import java.util.HashMap;

public class DecisionNode {

	private String name;
	private float threshold;
	private DecisionNode left;
	private DecisionNode right;
	private boolean isLeaf;
	private String classification;
	private String attr;
	private String operation;
	
	public DecisionNode(String name) {
		this.name = name;
	}
	
	public String expand(int r, int g, int b) {
		if (!isLeaf) {
			
			if ( attr.equals("red") ) {
				
				if ( operation.equals("<") ) {
					if ( r < threshold ) {
						return right.expand(r, g, b);
					} else {
						return left.expand(r, g, b);
					}
				} else {
					if ( r > threshold ) {
						return right.expand(r, g, b);
					} else {
						return left.expand(r, g, b);
					}
				}
				
			} else if ( attr.equals("green") ) {
				
				if ( operation.equals("<") ) {
					if ( g < threshold ) {
						return right.expand(r, g, b);
					} else {
						return left.expand(r, g, b);
					}
				} else {
					if ( g > threshold ) {
						return right.expand(r, g, b);
					} else {
						return left.expand(r, g, b);
					}
				}
				
			} else if ( attr.equals("blue") ) {
				
				if ( operation.equals("<") ) {
					if ( b < threshold ) {
						return right.expand(r, g, b);
					} else {
						return left.expand(r, g, b);
					}
				} else {
					if ( b > threshold ) {
						return right.expand(r, g, b);
					} else {
						return left.expand(r, g, b);
					}
				}
				
			}
			
		}
		
		return classification;
	}
	
	public void setRightNode(DecisionNode node) {
		right = node;
	}
	
	public void setLeftNode(DecisionNode node) {
		left = node;
	}
	
	public void setAsLeaf(String classification) {
		isLeaf = true;
		this.classification = classification;
	}
	
	public void setAttribute(String attr) {
		this.attr = attr;
	}
	
	public void setThreshold(float value) {
		threshold = value;
	}
	
	public void setOperation(String op) {
		operation = op;
	}
	
}
