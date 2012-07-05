#include "ImageClassifier.h"



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

	int y1, y2, y3, y4, u, v;
	int r, g, b;
	int pos;
	int level = 0;

	for (int j = 0, yp = 0; j < height; j++) {
		int uvp = length + (j >> 1) * width, u = 0, v = 0;
		for (int i = 0; i < width; i++, yp++) {
			int y = (0xff & ((int) src[yp])) - 16;
			if (y < 0)
			  y = 0;
			if ((i & 1) == 0) {
			  v = (0xff & src[uvp++]) - 128;
			  u = (0xff & src[uvp++]) - 128;
			}

			YUV444toRGB8888(y, u, v, r, g, b);

			rgb[yp] = 	(r <<  0) +
						(g <<  8) +
						(b << 16) +
						(255 << 24);
		}
	}

}

void ImageClassifier::greenBlobDetection(jbyte* dst) {
	for ( int i = 0; i < length; i++ ) {
		int g = (rgb[i] >> 8) & 0xff;
		if ( g > 225 ) {
			dst[i] = 1;
		} else {
			dst[i] = 0;
		}
	}
}

ImageClassifier::~ImageClassifier() {
	delete [] rgb;
}

