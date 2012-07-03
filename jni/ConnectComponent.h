#ifndef CONNECTCOMPONENT_H_
#define CONNECTCOMPONENT_H_

#include <limits.h>
#include <jni.h>
#include <vector>
#include <algorithm>

const int FB = 0;
const int FO = 1;

class ConnectComponent {
public:
	ConnectComponent(jbyte *s, jint* d, int w, int h);
	void run();
	virtual ~ConnectComponent();

	int numberOfRegions;
	int label;
	bool forwards;
private:
	jbyte *src;
	jint *dst;
	std::vector<int> connections;

	int length;
	int width;
	int height;

	int g(int pos, std::vector<int> &mask);
	int b(int pos);
	int mark();
	int mark(int l);
	void generateMask(std::vector<int> &mask, int pos);
};

#endif /* CONNECTCOMPONENT_H_ */
