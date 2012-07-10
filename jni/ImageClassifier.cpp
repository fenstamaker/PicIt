#include "ImageClassifier.h"
#include <android/log.h>


ImageClassifier::ImageClassifier(jbyte *s, int w, int h) {
	src = s;
	height = h;
	width = w;
	length = w*h;
	rgb = new int[length];
}

void ImageClassifier::YUV444toRGB8888(int y, int u, int v, int &r, int &g, int &b) {
	int c, d, e;
	c = y - 16;
	d = u - 128;
	e = v - 128;

	// Constants gained from Wikipedia
	r = (298 * c + 409 * e + 128) >> 8;
	if ( r < 0 ) r = 0;
	if ( r > 255) r = 255;

	g = (298 * c - 100 * d -208 * e + 128) >> 8;
	if ( g < 0 ) g = 0;
	if ( g > 255) g = 255;

	b = (298 * c + 516 * d + 128) >> 8;
	if ( b < 0 ) b = 0;
	if ( b > 255) b = 255;
}

// Converts NV21 to YUV444 then uses the above function to
// convert YUV444 to RGB8888;
void ImageClassifier::convert(){

	int y, u, v, uvPos, yPos;
	int r, g, b;

	// Idea from the Ketai Code Project
	for (int j = 0; j < height; j++) {

		// The j >> 1 gives the beginning X, Y coords of the required V, U
		uvPos = length + (j >> 1) * width;
		y = 0;
		u = 0;
		v = 0;

		for (int i = 0; i < width; i++) {
			yPos = j*width + i;

			// 0xff clamps the YUV values to 255
			y = ((int) src[yPos]) & 0xff;

			// Clamps Y value to be above 0
			if (y < 0)
			  y = 0;

			// If we're in the first of a new 2x2 square, set initial U, V values
			if ((i & 1) == 0) {
				uvPos++;
				v = src[uvPos] & 0xff;
				uvPos++;
				u = src[uvPos] & 0xff;
			}

			YUV444toRGB8888(y, u, v, r, g, b);

			// Shift the rgba values to their proper place
			rgb[yPos] = 	(r <<  0) +
							(g <<  8) +
							(b << 16) +
							(255 << 24);
		}
	}

}

void ImageClassifier::greenBlobDetection(jbyte* dst, JNIEnv *env, jobject tree, jmethodID mid) {
	int scanRadius = 4;
	int dilationRadius = 8;
	int constant = 16;

	for ( int y = 0; y < height; y++ ) {
		for ( int x = 0; x < width; x++ ) {
			int pos = y*width+x;

			/*int sum = 0;
			int mean = 0;
			int counter = 0;

			for ( int ty = y-scanRadius; ty < y + scanRadius; ty++ ) {
				int Y = ty*width;
				for ( int tx = x-scanRadius; tx < x + scanRadius; tx++ ) {
					int p = Y + tx;
					if ( p > 0 && p < length ) {
						sum += (rgb[p] >> 8) & 0xff;
						counter++;
					}
				}
			}

			if ( counter == 0 ) {
				mean = 0;
			} else {
				mean = sum / counter;
			}
			 */
			int g = (rgb[pos] >> 8) & 0xff;
			int r = (rgb[pos] >> 16) & 0xff;
			int b = rgb[pos] & 0xff;

			/*
			if ( g > 128 && r < 100 && b < 128 ) {
				dst[pos] = 1;
			} else {
				dst[pos] = 0;
			}
			*/
			if ( env->CallIntMethod(tree, mid, r, g, b) == 1 )
				dst[pos] = 1;
			else
				dst[pos] = 0;
		}
	}
}

ImageClassifier::~ImageClassifier() {
	delete [] rgb;
}

