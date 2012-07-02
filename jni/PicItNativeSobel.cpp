#include <jni.h>
#include <limits.h>
#include <vector>
#include <algorithm>
using namespace std;

extern "C" {
	JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output);
};

void sobel(jbyte* src, jint width, jint height, jbyte* dst);
void connectedComponent(jbyte* src, jint width, jint height, jint* dst);
void cc(jbyte* src, jint* dst, int* visited, int pos, int w, int h, int regionCounter);

JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output) {

	jboolean copy;
	jbyte *src = env->GetByteArrayElements(frame, &copy);
	jint *dst = (jint*) env->GetDirectBufferAddress(output);

	jbyte tempDst[width*height];
	sobel(src, width, height, tempDst);
	connectedComponent(tempDst, width, height, dst);

	env->ReleaseByteArrayElements(frame, src, JNI_ABORT);
}

void cc(jbyte* src, jint* dst, int* visited, int pos, int w, int h, int regionCounter) {
	vector<int> values;

	int west = pos - 1;
	int east = pos + 1;
	int north = pos - w;
	int northWest = north - 1;
	int northEast = north + 1;
	int south = pos + w;
	int southWest = south - 1;
	int southEast = south + 1;

	int temp[8]    = { west, east, north, northWest, northEast, south, southWest, southEast };

	for ( int i = 0; i < 8; i++ ) {
		if ( temp[i] > 0 && temp[i] < w*h ) {
			values.push_back(temp[i]);
		}
	}

	visited[pos] = 1;

	for ( int i = 0; i < values.size(); i++ ) {

		if ( src[values[i]] == 1 && visited[values[i]] == 0 ) {
			visited[values[i]] = 1;
			cc(src, dst, visited, values[i], w, h, regionCounter);
		}
	}

	dst[pos] = regionCounter;

}

void connectedComponent(jbyte* src, jint width, jint height, jint* dst) {
	int w = width;
	int h = height;
	int length = w*h;

	int Y = 0;
	int pos = 0;

	int regionCounter = 1;

	int *visited = new int[w*h];

	for ( int y = 1; y < h - 1; y++ ) {

		Y = y * w + 1;

		for ( int x = 1; x < w - 1; x++ ) {

			pos = (Y+x);

			if ( src[pos] != 0 ) {
				if ( visited[pos] != 1 ) {
					cc(src, dst, visited, pos, w, h, regionCounter);
					regionCounter++;
				}
			} else {
				dst[pos] = 0;
			}

		}
	}

	// Paints everything a certian color
	for ( int i = 0; i < w*h; i++ ) {
		if ( dst[i] != 0 ) {
			int paintValue = (int) ( ( (float)dst[i] /regionCounter) * 256);

			dst[i] = 	(paintValue <<  0) +
						(paintValue <<  8) +
						(paintValue << 16) +
						(255 << 24);
		}
	}

	delete [] visited;
}

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

			sobelHorizontal =  src[pW-1] + src[pW+1] - src[nW+1] - src[nW-1] - src[nW]    - src[nW]    + src[pW]    + src[pW];
			sobelVertical   =  src[pW+1] + src[nW+1] - src[nW-1] - src[pW-1] - src[pos-1] - src[pos-1] + src[pos+1] + src[pos+1];

			sobelFinal = (sobelHorizontal + sobelVertical) / 2;

			if ( sobelFinal < 88 ) sobelFinal = 0;
			else sobelFinal = 255;

			/*
			paintValue =  	(sobelFinal <<  0) +
							(sobelFinal <<  8) +
							(sobelFinal << 16) +
							(sobelFinal << 24);
			 */

			paintValue = 0;
			if (sobelFinal > 0) {
				paintValue = 1;
			}

			dst[pos] = paintValue;

			if ( paintValue != 0 ) {

				for ( int r = 0; r < radius; r++ ) {

					for ( int tx = x-r; tx < x + r; tx++ ) {
						for ( int ty = y-r; ty < y + r; ty++ ) {
							int p = (ty*w+1) + tx;
							if ( p > 0 && p < w*h )
								dst[p] = paintValue;
						}
					}
				}

			}

		} // End Inner For Loop

	} // End Outer For Loop
}
