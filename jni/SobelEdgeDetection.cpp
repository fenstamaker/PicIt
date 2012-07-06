/*
 * SobelEdgeDetection.cpp
 *
 *  Created on: Jul 5, 2012
 *      Author: gf102
 */

#include "SobelEdgeDetection.h"

SobelEdgeDetection::SobelEdgeDetection(jbyte *s, int w, int h) {
	src = s;
	width = w;
	height = h;
	length = w*h;
}

/*
 * This function performs the Sobel Edge detection on a given
 * image and output it to a given dst. It will label all edge
 * points as 1 and non-edge points as 0
 *
 * src - the source image from the camera in NV21 format
 * dst - the final binary image produced by the edge detection
 */

void SobelEdgeDetection::performSobel(jbyte *dst) {
	int sobelHorizontal = 0;
	int sobelVertical = 0;

	int sobelFinal = 0;
	int Y = 0;
	int pos = 0;
	int nW = 0;
	int pW = 0;
	int paintValue = 0;
	int radius = 5;

	for ( int y = 1; y < height - 1; y++ ) {

		Y = y * width;

		for ( int x = 1; x < width - 1; x++ ) {

			pos = (Y+x);
			pW = pos+width;
			nW = pos-width;

			// Optimize Sobel operation
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

			/*
			if ( paintValue != 0 ) {

				// Dilation code
				for ( int tx = x-radius; tx < x + radius; tx++ ) {
					for ( int ty = y-radius; ty < y + radius; ty++ ) {
						int p = (ty*width+1) + tx;
						if ( p > 0 && p < length )
							dst[p] = paintValue;
					}
				}

			}
			*/

		} // End Inner For Loop

	} // End Outer For Loop
}

SobelEdgeDetection::~SobelEdgeDetection() {
	// TODO Auto-generated destructor stub
}

