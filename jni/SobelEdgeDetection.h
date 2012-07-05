/*
 * SobelEdgeDetection.h
 *
 *  Created on: Jul 5, 2012
 *      Author: gf102
 */

#ifndef SOBELEDGEDETECTION_H_
#define SOBELEDGEDETECTION_H_

#include <jni.h>
#include <vector>

class SobelEdgeDetection {
public:
	SobelEdgeDetection(jbyte *src, int w, int h);
	void performSobel(jbyte *dst);
	virtual ~SobelEdgeDetection();
private:
	jbyte *src;

	int width;
	int height;
	int length;
};

#endif /* SOBELEDGEDETECTION_H_ */
