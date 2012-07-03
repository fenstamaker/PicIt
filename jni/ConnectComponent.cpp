#include "ConnectComponent.h"
#include <vector>

ConnectComponent::ConnectComponent(jbyte* s, jint* d, int w, int h) {
	src = s;
	dst = d;
	width = w;
	height = h;
	length = w*h;
}

void ConnectComponent::run() {
	int Y = 0;
	int pos = 0;

	int regionCounter = 1;

	for ( int y = 1; y < height - 1; y++ ) {

		Y = y * width + 1;

		for ( int x = 1; x < width - 1; x++ ) {

			pos = (Y+x);

			dst[pos] = g(pos);

		}
	}
}

int ConnectComponent::g(int pos) {
	std::vector<int> mask;

	int west = pos - 1;
	int east = pos + 1;
	int north = pos - width;
	int northWest = north - 1;
	int northEast = north + 1;
	int south = pos + width;
	int southWest = south - 1;
	int southEast = south + 1;

	int temp[8]    = { west, east, north, northWest, northEast, south, southWest, southEast };

	// Checks to make sure that all above
	// positions are within the array boundaries
	// and puts them in the values vector
	for ( int i = 0; i < 8; i++ ) {
		if ( temp[i] > 0 && temp[i] < length ) {
			mask.push_back(temp[i]);
		}
	}


}

ConnectComponent::~ConnectComponent() {
	// TODO Auto-generated destructor stub
}

