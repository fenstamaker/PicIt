#ifndef CONNECTCOMPONENT_H_
#define CONNECTCOMPONENT_H_

#include <jni.h>
#include <vector>

class ConnectComponent {
public:
	ConnectComponent(jbyte *s, jint* d, int w, int h);
	void run();
	virtual ~ConnectComponent();

	int numberOfRegions;
	int mark;
private:
	jbyte *src;
	jint *dst;
	std::vector<int> connections;

	int length;
	int width;
	int height;

	int g(int pos);
};

#endif /* CONNECTCOMPONENT_H_ */
