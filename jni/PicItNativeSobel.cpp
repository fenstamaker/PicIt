#include <jni.h>
#include <limits.h>
#include <vector>
#include <algorithm>
#include <android/log.h>

#define APPNAME "PicItNativeSobel"

using namespace std;

extern "C" {
	JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output);
};

void sobel(jbyte* src, jint width, jint height, jbyte* dst);
void connectedComponent(jbyte* src, jint width, jint height, jint* dst);
void cc(jbyte* src, jint* dst, int *visited, int *temp, int pos, int w, int h, int regionCounter);

JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output) {

	jboolean copy;
	jbyte *src = env->GetByteArrayElements(frame, &copy);
	jint *dst = (jint*) env->GetDirectBufferAddress(output);

	jbyte *tempDst = new jbyte[width*height];
	sobel(src, width, height, tempDst);
	try {
		connectedComponent(tempDst, width, height, dst);
	} catch (int e) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Error caught:", e);
	}
	delete [] tempDst;

	env->ReleaseByteArrayElements(frame, src, JNI_ABORT);
}

/*
 * A recursive function that labels each region to the regionCounter
 * Will go through each neighbor of a given position (pos) and
 * recursively run this function until their are no more valid
 * neighbors. Essentially, it is performing a depth-first search and
 * creating a tree in the stack that can then be labeled with
 * region counter.
 *
 * src - the source binary image from the Sobel edge detection
 * dst - the final image to be created with the proper labeling
 * visited - the array that marks visited pixels, 1 = visited 0 = not visited
 * pos - the position of the given point, guaranteed to be an edge point and not already visited
 * w - the image's width
 * h - the image's height
 * regionCounter - region label currently on
 *
 */

void cc(jbyte* src, jint* dst, int *visited, int *temp, int pos, int w, int h, int regionCounter) {
	vector<int> values; // Holds all neighbor positions

	//int temp[8]    = { west, east, north, northWest, northEast, south, southWest, southEast };
	temp[0] = pos - 1;
	temp[1] = pos + 1;
	temp[2] = pos - w;
	temp[3] = pos - w - 1;
	temp[4] = pos - w + 1;
	temp[5] = pos + w;
	temp[6] = pos + w - 1;
	temp[7] = pos + w + 1;

	// Checks to make sure that all above
	// positions are within the array boundaries
	// and puts them in the values vector
	for ( int i = 0; i < 8; i++ ) {
		if ( temp[i] >= 0 && temp[i] < w*h && visited[temp[i]] == 0 && src[temp[i]] == 1) {
			values.push_back(temp[i]);
		}
	}

	visited[pos] = 1;

	// Goes through each valid neighbor and
	// and recursively runs this function
	for ( int i = 0; i < values.size(); i++ ) {
		visited[values[i]] = 1;
		cc(src, dst, visited, temp, values[i], w, h, regionCounter);
	}

	// Sets the point as the region counter
	// Will set all neighbors to regionCounter too
	dst[pos] = regionCounter;

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
	int w = width;
	int h = height;
	int length = w*h;

	int Y = 0;
	int pos = 0;

	int regionCounter = 1;

	int *visited = new int[w*h];
	int *temp = new int[8];

	for ( int y = 0; y < h; y++ ) {

		Y = y * w;

		for ( int x = 0; x < w; x++ ) {

			pos = (Y+x);

			// If the point is an edge and has not been visited
			if ( src[pos] != 0 ) {
				if ( visited[pos] != 1 ) {
					// This function will label all connected points as regionCounter
					cc(src, dst, visited, temp, pos, w, h, regionCounter);
					// Go to the next region
					regionCounter++;
				}
			} else {
				// Clears the dst buffer and prevents ghosting
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

		Y = y * w;

		for ( int x = 1; x < w - 1; x++ ) {

			pos = (Y+x);
			pW = pos+w;
			nW = pos-w;

			// Optimize sobel operation
			sobelHorizontal =  src[pW-1] + src[pW+1] - src[nW+1] - src[nW-1] - src[nW]    - src[nW]    + src[pW]    + src[pW];
			sobelVertical   =  src[pW+1] + src[nW+1] - src[nW-1] - src[pW-1] - src[pos-1] - src[pos-1] + src[pos+1] + src[pos+1];

			// Used average instead of Pythagoras addition for speed purposes
			sobelFinal = (sobelHorizontal + sobelVertical) / 2;

			if ( sobelFinal < 128 ) sobelFinal = 0;
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
