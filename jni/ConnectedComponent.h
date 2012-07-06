/*
 * ConnectedComponent.h
 *
 *  Created on: Jul 6, 2012
 *      Author: gf102
 */

#ifndef CONNECTEDCOMPONENT_H_
#define CONNECTEDCOMPONENT_H_

#include <jni.h>
#include <vector>
#include <stack>

class ConnectedComponent {
public:
	ConnectedComponent(jbyte *s, int w, int h);

	void scan(jbyte* dst);
	void singleOutMax(jbyte *dst);

	virtual ~ConnectedComponent();

	int regionCounter;
	int max;
	std::vector<int> counter;

private:
	jbyte *src;
	int *visited;
	int width;
	int height;
	int length;

	void cc(int pos, jbyte *dst);
};

#endif /* CONNECTEDCOMPONENT_H_ */
