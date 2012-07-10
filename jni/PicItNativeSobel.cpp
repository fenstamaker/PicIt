#include <jni.h>
#include <limits.h>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <android/log.h>

#include "SobelEdgeDetection.h"
#include "ImageClassifier.h"
#include "ConnectedComponent.h"

#define APPNAME "PicItNativeSobel"

using namespace std;

extern "C" {
	JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output, jobject tree);
};

void paint(jbyte *src, jint width, jint height, jint *dst);
void dilation(jbyte* src, jint width, jint height, jbyte *dst, int radius);
void erosion(jbyte* src, jint width, jint height, jbyte *dst, int radius);
void combine(jbyte* top, jbyte* bottom, int width, int height, jbyte *dst);

JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output, jobject tree) {

	jclass decisionTree = env->GetObjectClass(tree);
	jmethodID classifierId = env->GetMethodID(decisionTree, "classify", "(III)I");
	if ( classifierId == 0 ) {
		__android_log_print(ANDROID_LOG_ERROR, "Picit Native Call", "Failed to get classifier from DecisionTree");
		return;
	}

	jboolean copy;
	jbyte *src = env->GetByteArrayElements(frame, &copy);
	jint *dst = (jint*) env->GetDirectBufferAddress(output);

	jbyte *sobelDst = new jbyte[width*height];
	jbyte *ccDst = new jbyte[width*height];

	jbyte *greenDst = new jbyte[width*height];
	jbyte *dilationDst = new jbyte[width*height];
	jbyte *erosionDst = new jbyte[width*height];
	jbyte *combinedDst = new jbyte[width*height];

	SobelEdgeDetection *sobel = new SobelEdgeDetection(src, (int)width, (int)height);
	sobel->performSobel(sobelDst);

	ImageClassifier *classifier = new ImageClassifier(src, width, height);
	classifier->convert();
	classifier->greenBlobDetection(greenDst, env, tree, classifierId);

	dilation(greenDst, width, height, dilationDst, 16);
	erosion(dilationDst, width, height, erosionDst, 3);

	//ConnectedComponent *connected = new ConnectedComponent(erosionDst, width, height);
	//connected->scan(ccDst);
	//connected->singleOutMax(ccDst);

	combine(sobelDst, erosionDst, width, height, combinedDst);

	paint(combinedDst, width, height, dst);

	//delete connected;
	delete classifier;
	delete sobel;

	delete [] combinedDst;
	delete [] erosionDst;
	delete [] dilationDst;
	delete [] greenDst;

	delete [] ccDst;
	delete [] sobelDst;

	env->ReleaseByteArrayElements(frame, src, JNI_ABORT);
}

void combine(jbyte* top, jbyte* bottom, int width, int height, jbyte *dst) {
	for ( int i = 0; i < width*height; i++ ) {
		if ( top[i] > 0 ) {
			dst[i] = 2;
		} else if ( bottom[i] > 0 ) {
			dst[i] = 1;
		} else {
			dst[i] = 0;
		}
	}
}

void erosion(jbyte* src, jint width, jint height, jbyte *dst, int radius) {
	for ( int y = 0; y < height; y++ ) {
		for ( int x = 0; x < width; x++ ) {
			int pos = y*width+x;
			if ( src[pos] == 1 ) {

				bool surround = true;
				for ( int tx = x-radius; tx < x + radius; tx++ ) {
					for ( int ty = y-radius; ty < y + radius; ty++ ) {
						int p = ty*width + tx;
						if ( p > 0 && p < width*height && src[p] != 1 ) {
							surround = false;
						}
					}
				}

				if (surround) {
					dst[pos] = 1;
				} else {
					dst[pos] = 0;
				}

			} // End If
		} // End For
	} // End For
}

void dilation(jbyte* src, jint width, jint height, jbyte *dst, int radius) {
	for ( int y = 0; y < height; y++ ) {
		for ( int x = 0; x < width; x++ ) {
			int pos = y*width+x;
			if ( src[pos] == 1 ) {
				dst[pos] = 1;
				for ( int tx = x-radius; tx < x + radius; tx++ ) {
					for ( int ty = y-radius; ty < y + radius; ty++ ) {
						int p = ty*width + tx;
						if ( p > 0 && p < width*height ) {
							dst[p] = 1;
						}
					}
				}
			}// End If
		} // End For
	} // End For
}

void paint(jbyte *src, jint w, jint h, jint *dst) {
	// Paints everything a certian color
	for ( int i = 0; i < w*h; i++ ) {
		if ( src[i] != 0 ) {
			if ( src[i] == 2 ) {
				dst[i] = 	(255 <<  0) +
							(0 <<  8) +
							(0 << 16) +
							(128 << 24);
			} else if ( src[i] == 1 ) {
				//int paintValue = (int) ( ( (float)dst[i] /regionCounter) * 256);
				int paintValue = src[i];

				dst[i] = 	(255 <<  0) +
							(255 <<  8) +
							(255 << 16) +
							(255 << 24);
			}


		} else {
			dst[i] = 0;
		}
	}
}




