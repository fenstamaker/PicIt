#ifndef DECISIONTREE_H_
#define DECISIONTREE_H_

#include <jni.h>
#include <string>
#include <vector>
#include <stack>
#include <istream>

class DecisionTree {
public:
	DecisionTree();
	virtual ~DecisionTree();

private:
	std::string fileName;
	std::string dir;

	Node *root;
};

#endif /* DECISIONTREE_H_ */
