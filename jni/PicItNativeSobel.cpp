#include <jni.h>
#include <limits.h>
#include <vector>
#include <stack>
#include <algorithm>
#include <android/log.h>

#define APPNAME "PicItNativeSobel"

using namespace std;

extern "C" {
	JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output);
};

void sobel(jbyte* src, jint width, jint height, jbyte* dst);
void connectedComponent(jbyte* src, jint width, jint height, jint* dst);
void cc(jbyte* src, jint* dst, int *visited, int pos, int w, int h, int regionCounter);

JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output) {

	jboolean copy;
	jbyte *src = env->GetByteArrayElements(frame, &copy);
	jint *dst = (jint*) env->GetDirectBufferAddress(output);

	jbyte *tempDst = new jbyte[width*height];

	sobel(src, width, height, tempDst);
	connectedComponent(tempDst, width, height, dst);

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

void cc(jbyte* src, jint* dst, int *visited, int pos, int w, int h, int regionCounter) {
	stack<int> positions;

	positions.push(pos);

	int *temp = new int[8];
	int tempPos = positions.top();

	while ( true ) {
		temp[0] = tempPos - 1;
		temp[1] = tempPos + 1;
		temp[2] = tempPos - w;
		temp[3] = tempPos - w - 1;
		temp[4] = tempPos - w + 1;
		temp[5] = tempPos + w;
		temp[6] = tempPos + w - 1;
		temp[7] = tempPos + w + 1;

		visited[tempPos] = 1;

		// Checks to make sure that all above
		// positions are within the array boundaries
		// and puts them in the values vector
		bool isEmpty = true;
		for ( int i = 0; i < 8; i++ ) {
			if ( temp[i] >= 0 && temp[i] < w*h && visited[temp[i]] == 0 && src[temp[i]] == 1) {
				isEmpty = false;
				positions.push(temp[i]);
			}
		}

		if ( isEmpty ) {
			dst[positions.top()] = regionCounter;
			positions.pop();
		}

		if ( positions.empty() )
			break;

		tempPos = positions.top();
	}

	delete [] temp;
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

	for ( int y = 0; y < h; y++ ) {

		Y = y * w;

		for ( int x = 0; x < w; x++ ) {

			pos = (Y+x);

			// If the point is an edge and has not been visited
			if ( src[pos] != 0 ) {
				if ( visited[pos] != 1 ) {
					// This function will label all connected points as regionCounter
					cc(src, dst, visited, pos, w, h, regionCounter);
					// Go to the next region
					regionCounter++;
				}
			} else {
				// Clears the dst buffer and prevents ghosting
				dst[pos] = 0;
			}

		}
	}

	delete [] visited;

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
