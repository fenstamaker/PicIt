#include <jni.h>
#include <limits.h>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <android/log.h>

#include "SobelEdgeDetection.h"
#include "ImageClassifier.h"

#define APPNAME "PicItNativeSobel"

using namespace std;

extern "C" {
	JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output);
};

void sobel(jbyte* src, jint width, jint height, jbyte* dst);
void connectedComponent(jbyte* src, jint width, jint height, jint* dst);
void cc(jbyte* src, jint* dst, int *visited, vector<int> &counter, int pos, int w, int h, int regionCounter);

JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output) {

	jboolean copy;
	jbyte *src = env->GetByteArrayElements(frame, &copy);
	jint *dst = (jint*) env->GetDirectBufferAddress(output);

	jbyte *sobelDst = new jbyte[width*height];
	jbyte *greenDst = new jbyte[width*height];

	SobelEdgeDetection *sobel = new SobelEdgeDetection(src, (int)width, (int)height);
	sobel->performSobel(sobelDst);

	ImageClassifier *classifier = new ImageClassifier(src, width, height);
	classifier->convert();
	classifier->greenBlobDetection(greenDst);

	connectedComponent(greenDst, width, height, dst);

	delete [] sobelDst;
	delete [] greenDst;

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

void cc(jbyte* src, jint* dst, int *visited, vector<int> &counter, int pos, int w, int h, int regionCounter) {
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
			counter[regionCounter]++;
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
	vector<int> counter;
	counter.push_back(0);
	counter.push_back(0);

	for ( int y = 1; y < h-1; y++ ) {

		Y = y * w;

		for ( int x = 1; x < w-1; x++ ) {

			pos = (Y+x);

			// If the point is an edge and has not been visited
			if ( src[pos] != 0 ) {
				if ( visited[pos] != 1 ) {
					// This function will label all connected points as regionCounter
					cc(src, dst, visited, counter, pos, w, h, regionCounter);
					// Go to the next region
					regionCounter++;
					counter.push_back(0);
				}
			} else {
				// Clears the dst buffer and prevents ghosting
				dst[pos] = 0;
			}

		}
	}

	delete [] visited;

	int max = 0;
	for ( int i = 0; i < counter.size(); i++ ) {
		if ( counter[i] > counter[max] ) {
			max = i;
		}
	}

	// Paints everything a certian color
	for ( int i = 0; i < w*h; i++ ) {
		if ( dst[i] != 0 ) {
			if ( dst[i] == max ) {
				int paintValue = (int) ( ( (float)dst[i] /regionCounter) * 256);

				dst[i] = 	(255 <<  0) +
							(0 <<  8) +
							(0 << 16) +
							(255 << 24);
			} else {
				int paintValue = (int) ( ( (float)dst[i] /regionCounter) * 256);

				dst[i] = 	(paintValue <<  0) +
							(paintValue <<  8) +
							(paintValue << 16) +
							(255 << 24);
			}

		}
	}

}



