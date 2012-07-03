#include <jni.h>
#include <limits.h>
#include <vector>
#include <algorithm>
#include "ConnectComponent.h"
using namespace std;

extern "C" {
	JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output);
};

void sobel(jbyte* src, jint width, jint height, jbyte* dst);
void connectedComponent(jbyte* src, jint width, jint height, jint* dst);
void cc(jbyte* src, jint* dst, vector<int> &visited, int pos, int w, int h, int regionCounter);
int g(jbyte* src, int pos, int w, int h);

JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output) {

	jboolean copy;
	jbyte *src = env->GetByteArrayElements(frame, &copy);
	jint *dst = (jint*) env->GetDirectBufferAddress(output);

	jbyte tempDst[width*height];
	sobel(src, width, height, tempDst);
	connectedComponent(tempDst, width, height, dst);

	env->ReleaseByteArrayElements(frame, src, JNI_ABORT);
}

/*
 * This function runs the above function on every point in the image
 * and check in a given point is a valid edge point and hasen't already
 * been visited.
 *
 * src - the source image from the sobel edge detection
 * dst - the final output
 */

void connectedComponent(jbyte* src, jint width, jint height, jint* dst) {

	ConnectComponent *CC = new ConnectComponent(src, dst, width, height);

	// Paints everything a certain color
	for ( int i = 0; i < width*height; i++ ) {
		if ( dst[i] != 0 ) {
			int paintValue = (int) ( ( (float)dst[i] /CC->numberOfRegions) * 256);

			dst[i] = 	(paintValue <<  0) +
						(paintValue <<  8) +
						(paintValue << 16) +
						(255 << 24);
		}
	}

}

/*
 * This function performs the Sobel Edge detection on a given
 * image and output it to a given dst. It will label all edge
 * points as 1 and non-edge points as 0
 *
 * src - the source image from the camera in NV21 format
 * dst - the final binary image produced by the edge detection
 */

void sobel(jbyte* src, jint width, jint height, jbyte* dst) {
	int w = width;
	int h = height;

	int sobelHorizontal = 0;
	int sobelVertical = 0;

	int sobelFinal = 0;
	int Y = 0;
	int pos = 0;
	int nW = 0;
	int pW = 0;
	int paintValue = 0;
	int radius = 5;

	for ( int y = 1; y < h - 1; y++ ) {

		Y = y * w + 1;

		for ( int x = 1; x < w - 1; x++ ) {

			pos = (Y+x);
			pW = pos+w;
			nW = pos-w;

			// Optimize sobel operation
			sobelHorizontal =  src[pW-1] + src[pW+1] - src[nW+1] - src[nW-1] - src[nW]    - src[nW]    + src[pW]    + src[pW];
			sobelVertical   =  src[pW+1] + src[nW+1] - src[nW-1] - src[pW-1] - src[pos-1] - src[pos-1] + src[pos+1] + src[pos+1];

			// Used average instead of Pythagoras addition for speed purposes
			sobelFinal = (sobelHorizontal + sobelVertical) / 2;

			if ( sobelFinal < 88 ) sobelFinal = 0;
			else sobelFinal = 255;

			paintValue = 0;
			if (sobelFinal > 0) {
				paintValue = 1;
			}

			dst[pos] = paintValue;

			if ( paintValue != 0 ) {

				// Dilation code
				for ( int tx = x-radius; tx < x + radius; tx++ ) {
					for ( int ty = y-radius; ty < y + radius; ty++ ) {
						int p = (ty*w+1) + tx;
						if ( p > 0 && p < w*h )
							dst[p] = paintValue;
					}
				}

			}

		} // End Inner For Loop

	} // End Outer For Loop
}
