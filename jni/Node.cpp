/*
 * Node.cpp
 *
 *  Created on: Jul 9, 2012
 *      Author: gf102
 */

#include "Node.h"

Node::Node(std::string op, std::string attr, int v) {
	operation = op;
	attribute = attr; // Attribute will equal type if leaf node
	value = v;
}

std::string Node::expand(int r, int g, int b) {
	if ( attribute.compare("red") ) {
		if ( operation.compare(">") ) {
			if ( r > value ) {
				return right->expand(r, g, b);
			} else {
				return left->expand(r, g, b);
			}
		} else if ( operation.compare("<") ) {
			if ( r < value ) {
				return right->expand(r, g, b);
			} else {
				return left->expand(r, g, b);
			}
		}
	} else if ( attribute.compare("green") ) {
		if ( operation.compare(">") ) {
			if ( g > value ) {
				return right->expand(r, g, b);
			} else {
				return left->expand(r, g, b);
			}
		} else if ( operation.compare("<") ) {
			if ( g < value ) {
				return right->expand(r, g, b);
			} else {
				return left->expand(r, g, b);
			}
		}
	} else if ( attribute.compare("blue") ) {
		if ( operation.compare(">") ) {
			if ( b > value ) {
				return right->expand(r, g, b);
			} else {
				return left->expand(r, g, b);
			}
		} else if ( operation.compare("<") ) {
			if ( b < value ) {
				return right->expand(r, g, b);
			} else {
				return left->expand(r, g, b);
			}
		}
	}

	return attribute;
}

void Node::addLeftNode(Node *node) {
	left = node;
}

void Node::addRightNode(Node *node) {
	right = node;
}

Node::~Node() {
	delete left;
	delete right;
}

