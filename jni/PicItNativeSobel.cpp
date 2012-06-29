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

JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output) {

	jboolean copy;
	jbyte *src = env->GetByteArrayElements(frame, &copy);
	jint *dst = (jint*) env->GetDirectBufferAddress(output);

	jbyte tempDst[width*height];
	sobel(src, width, height, tempDst);
	connectedComponent(tempDst, width, height, dst);

	env->ReleaseByteArrayElements(frame, src, JNI_ABORT);
}

void connectedComponent(jbyte* src, jint width, jint height, jint* dst) {
	int w = width;
	int h = height;

	int Y = 0;
	int pos = 0;
	int north = 0;
	int northWest = 0;
	int northEast = 0;
	int west = 0;
	int east = 0;

	int regionCounter = 1;

	vector<int> equals; // To be replaced with
	vector<int> counters; // Counts the number of instances of region #
	counters.push_back(0); // There is no region 0

	for ( int y = 1; y < h - 1; y++ ) {

		Y = y * w + 1;

		for ( int x = 1; x < w - 1; x++ ) {

			pos = (Y+x);

			if ( src[pos] != 0 ) {
				north = pos-w;
				northWest = north-1;
				northEast = north+1;
				west = pos-1;
				east = pos+1;

				// If there is nothing surrounding the pixel
				if ( src[north] == 0 && src[northWest] == 0 && src[northEast] == 0 && src[west] == 0 ) {
					dst[pos] = regionCounter;
					counters.push_back(1);
					equals.push_back(0);
					regionCounter++;
				} else {
					int values[4] = { dst[west], dst[north], dst[northWest], dst[northEast] };
					int min = INT_MAX;
					int counter = 0; // Counter ensures that there are a same number of ids as equals

					// Finds minimum value
					for ( int i = 0; i < 4; i++ ) {
						// Makes sure the min is not zero
						if ( values[i] != 0 ) {
							if (values[i] < min) {
								if (min != INT_MAX) {
									equals[min] = values[i];
								}
								min = values[i];
							}

						}
					}



					// If min is INT_MAX, then something went wrong so set it to regionCounter
					if ( min == INT_MAX ) {
						dst[pos] = regionCounter;
						counters.push_back(1);
						equals.push_back(0);
						regionCounter++;
					} else {
						// Sets current pixel to minimum
						dst[pos] = min;
						counters[min]++;
					}

				}
			} else {
				dst[pos] = 0;
			}
		}
	}

	// Replace all occurances of ids[i] with respective equals[i] in dst
	int length = w*h;
	for ( int i = 0; i < length; i++ ) {
		if (equals[dst[i]] != 0) {
			dst[i] = equals[dst[i]];
		}
	}

	// Finds max region
	int max = 0;
	for ( int i = 1; i < regionCounter; i++ ) {
		if ( counters[i] > counters[max] ) {
			max = i;
		}
	}

	// Paints everything a certian color
	for ( int i = 0; i < w*h; i++ ) {
		if ( dst[i] != 0 && dst[i] == max) {
			int paintValue = (int) ( ( (float)dst[i] /regionCounter) * 256);

			dst[i] = 	(paintValue <<  0) +
						(paintValue <<  8) +
						(paintValue << 16) +
						(255 << 24);
		} else if ( dst[i] != 0 ) {
			dst[i] =	(255 <<  0) +
						(255 <<  8) +
						(255 << 16) +
						(255 << 24);;
		}
	}
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
	int radius = 10;

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

					int start = nW-(r*w);
					for ( int i = start; i > start-(r/2); i-- ) {
						if (i > 0)
							dst[i] = paintValue;
					}
					start = nW+(r*w);
					for ( int i = start; i < start+(r/2); i++ ) {
						if (i < w*h)
							dst[i] = paintValue;
					}
					start = pW-(r*w);
					for ( int i = start; i > start-(r/2); i-- ) {
						if (i > 0)
							dst[i] = paintValue;
					}
					start = pW+(r*w);
					for ( int i = start; i < start+(r/2); i++ ) {
						if (i < w*h)
							dst[i] = paintValue;
					}
					start = pos-r;
					for ( int i = start; i > start-(r/2); i-- ) {
						if (i > 0)
							dst[i] = paintValue;
					}
					start = pos+r;
					for ( int i = start; i < start+(r/2); i++ ) {
						if (i < w*h)
							dst[i] = paintValue;
					}

				}

			}

		} // End Inner For Loop

	} // End Outer For Loop
}
