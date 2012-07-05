#ifndef IMAGECLASSIFIER_H_
#define IMAGECLASSIFIER_H_

#include <jni.h>
#include <vector>

class ImageClassifier {
public:
	ImageClassifier(jbyte *s, int w, int h);
	void convert();
	void YUV444toRGB8888(int y, int u, int v, int &r, int &g, int &b);

	void greenBlobDetection(jbyte* dst);

	virtual ~ImageClassifier();
private:
	jbyte *src;
	int width;
	int height;
	int length;
	int *rgb;
};

#endif /* IMAGECLASSIFIER_H_ */
