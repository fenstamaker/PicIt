#include "ConnectComponent.h"

ConnectComponent::ConnectComponent(jbyte* s, jint* d, int w, int h) {
	src = s;
	dst = d;
	width = w;
	height = h;
	length = w*h;
	label = 1;
	connections.push_back(FB);
	connections.push_back(FO);
}

void ConnectComponent::run() {
	int Y = 0;
	int pos = 0;

	forwards = true;

	int regionCounter = 1;

	for ( int y = 1; y < height - 1; y++ ) {

		Y = y * width + 1;

		for ( int x = 1; x < width - 1; x++ ) {

			pos = (Y+x);

			std::vector<int> mask;

			generateMask(mask, pos);

			dst[pos] = g(pos, mask);

			forwards = false;
		}
	}
}

int ConnectComponent::g(int pos, std::vector<int> &mask) {
	if ( b(pos) == FB )
		return FB;

	std::vector<int> values;

	for ( int  i = 0; i < mask.size(); i++ ) {
		std::vector<int> tempMask;
		generateMask(tempMask, mask[i]);
		int value = g(mask[i], tempMask);
		dst[pos] = value;
		values.push_back( value );
	}

	int zeros = std::count(values.begin(), values.end(), FB);
	if ( zeros == values.size() ) {
		return mark();
	}

	int min = INT_MAX;
	for ( int i = 0; i < mask.size(); i++ ) {
		if ( connections[values[i]] < min &&  connections[values[i]] != FB ) {
			min = connections[values[i]];
		}
	}

	for ( int i = 0; i < mask.size(); i++ ) {
		if ( values[i] != FB ) {
			connections[values[i]] = min;
		}
	}

	return min;
}

void ConnectComponent::generateMask(std::vector<int> &mask, int pos) {
	int west = pos - 1;
	int east = pos + 1;
	int north = pos - width;
	int northWest = north - 1;
	int northEast = north + 1;
	int south = pos + width;
	int southWest = south - 1;
	int southEast = south + 1;

	int forward[4] = { northWest, north, northEast, west };
	int backward[4] = { southEast, south, southWest, east };


	// Checks to make sure that all above
	// positions are within the array boundaries
	// and puts them in the values vector
	// Also checks which mask to implement
	if ( forwards ) {
		for ( int i = 0; i < 4; i++ ) {
			if ( forward[i] > 0 && forward[i] < length ) {
				mask.push_back(forward[i]);
			}
		}
	} else {
		for ( int i = 0; i < 4; i++ ) {
			if ( backward[i] > 0 && backward[i] < length ) {
				mask.push_back(backward[i]);
			}
		}
	}
}

int ConnectComponent::b(int pos) {
	return dst[pos];
}

int ConnectComponent::mark() {
	int temp = label;
	connections[label] = label;
	label++;
	connections.push_back(0);
	return temp;
}

int ConnectComponent::mark(int l) {
	return l;
}

ConnectComponent::~ConnectComponent() {
	// TODO Auto-generated destructor stub
}

