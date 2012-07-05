#include "ImageClassifier.h"



ImageClassifier::ImageClassifier(jbyte *s, int w, int h) {
	src = s;
	height = h;
	width = w;
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

void ImageClassifier::convert(int *rgb){

	int y, u, v;
	int r, g, b;
	int pos;
	int level = 0;


	for ( int i = 0; i < height*width; i++ ) {
		y = src[i];
		if ( level % width == 0 )
			level += width;
		if ( i % 4 == 0 || i % 4 == 1 ) {
			pos = i / (width / 2) + (width * height) + level;
			v = src[pos];
			u = src[pos+1];
		} else {
			pos = i / (width / 2) + (width * height) + 1 + level;
			v = src[pos];
			u = src[pos+1];
		}

		YUV444toRGB8888(y, u, v, r, g, b);

		rgb[i] =	(r <<  0) +
					(g <<  8) +
					(b << 16) +
					(255 << 24);

	}

}

ImageClassifier::~ImageClassifier() {
}

