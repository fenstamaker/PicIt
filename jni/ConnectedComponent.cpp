/*
 * ConnectedComponent.cpp
 *
 *  Created on: Jul 6, 2012
 *      Author: gf102
 */

#include "ConnectedComponent.h"

ConnectedComponent::ConnectedComponent(jbyte *s, int w, int h) {
	src = s;
	width = w;
	height = h;
	length = w*h;

	visited = new int[length];

	regionCounter = 1;

}

/*
 * This function runs the above function on every point in the image
 * and check in a given point is a valid edge point and hasen't already
 * been visited.
 *
 * src - the source image from the sobel edge detection
 * dst - the final output
 */

void ConnectedComponent::scan(jbyte* dst) {
	int Y = 0;
	int pos = 0;

	int regionCounter = 1;

	counter.push_back(0);
	counter.push_back(0);

	for ( int y = 1; y < height-1; y++ ) {

		Y = y * width;

		for ( int x = 1; x < width-1; x++ ) {

			pos = (Y+x);

			// If the point is an edge and has not been visited
			if ( src[pos] != 0 ) {
				if ( visited[pos] != 1 ) {
					// This function will label all connected points as regionCounter
					cc(pos, dst);
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

	max = 0;
	for ( int i = 0; i < counter.size(); i++ ) {
		if ( counter[i] > counter[max] ) {
			max = i;
		}
	}
}

void ConnectedComponent::singleOutMax(jbyte *dst) {
	for ( int i = 0; i < length; i++ ) {
		if ( dst[i] == max ) {
			dst[i] = 1;
		} else {
			dst[i] = 0;
		}
	}
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

void ConnectedComponent::cc(int pos, jbyte *dst) {
	std::stack<int> positions;

	positions.push(pos);

	int *temp = new int[8];
	int tempPos = positions.top();

	while ( true ) {
		temp[0] = tempPos - 1;
		temp[1] = tempPos + 1;
		temp[2] = tempPos - width;
		temp[3] = tempPos - width - 1;
		temp[4] = tempPos - width + 1;
		temp[5] = tempPos + width;
		temp[6] = tempPos + width - 1;
		temp[7] = tempPos + width + 1;

		visited[tempPos] = 1;

		// Checks to make sure that all above
		// positions are within the array boundaries
		// and puts them in the values vector
		bool isEmpty = true;
		for ( int i = 0; i < 8; i++ ) {
			if ( temp[i] >= 0 && temp[i] < length && visited[temp[i]] == 0 && src[temp[i]] == 1) {
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

ConnectedComponent::~ConnectedComponent() {
	delete [] visited;
	counter.clear();
}

