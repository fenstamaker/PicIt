/*
 * Node.h
 *
 *  Created on: Jul 9, 2012
 *      Author: gf102
 */

#ifndef NODE_H_
#define NODE_H_

class Node {
public:
	Node(std::string op, std::string attr, int v);

	void addLeftNode(Node *node);
	void addRightNode(Node *node);
	std::string expand(int r, int g, int b);

	virtual ~Node();

private:

	std::string operation;
	std::string attribute;
	float value;

	Node *right;
	Node *left;

};

#endif /* NODE_H_ */
