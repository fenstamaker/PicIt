#include <jni.h>

extern "C" {
	JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output);
};

void sobel(jbyte* src, jint width, jint height, jbyte* dst);
void connectedComponent(jbyte* src, jint width, jint height, jint* dst);

JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output) {

	jboolean copy;
	jbyte *src = env->GetByteArrayElements(frame, &copy);
	jint *dst = (jint*) env->GetDirectBufferAddress(output);

	jbyte tempDst[width*height];
	sobel(src, width, height, tempDst);
	connectedComponent(tempDst, width, height, dst);

	env->ReleaseByteArrayElements(frame, src, JNI_ABORT);
}

void connectedComponent(jbyte* src, jint width, jint height, jint* dst) {
	int w = width;
	int h = height;

	int equals[256];

	int Y = 0;
	int pos = 0;
	int north = 0;
	int northWest = 0;
	int northEast = 0;
	int west = 0;
	int east = 0;

	int regionCounter = 1;

	for ( int y = 1; y < h - 1; y++ ) {

		Y = y * w + 1;

		for ( int x = 1; x < w - 1; x++ ) {

			pos = (Y+x);

			if ( src[pos] > 0 ) {
				north = pos-w;
				northWest = north-1;
				northEast = north+1;
				west = pos-1;
				east = pos+1;

				if ( src[north] == 0 && src[northWest] == 0 && src[northEast] == 0 && src[west] == 0 ) {
					dst[pos] = regionCounter;
					regionCounter++;
				} else {
					int values[4] = { dst[north], dst[northWest], dst[northEast], dst[west] };
					int min = dst[north];

					// Finds minimum value
					for ( int i = 1; i < 4; i++ ) {
						if ( values[i] < min )
							min = values[i];
					}

					// Sets current pixel to minimum
					dst[pos] = min;
				}
			}
		}
	}

	for ( int i = 0; i < w*h; i++ ) {
		if ( dst[i] != 0) {
			int paintValue =  	(dst[i]*2 <<  0) +
								(dst[i]*2 <<  8) +
								(dst[i]*2 << 16) +
								(dst[i]*2 << 24);
			dst[i] = paintValue;
		}
	}
}

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

		Y = y * w + 1;

		for ( int x = 1; x < w - 1; x++ ) {

			pos = (Y+x);
			pW = pos+w;
			nW = pos-w;

			sobelHorizontal =  src[pW-1] + src[pW+1] - src[nW+1] - src[nW-1] - src[nW]    - src[nW]    + src[pW]    + src[pW];
			sobelVertical   =  src[pW+1] + src[nW+1] - src[nW-1] - src[pW-1] - src[pos-1] - src[pos-1] + src[pos+1] + src[pos+1];

			sobelFinal = (sobelHorizontal + sobelVertical) / 2;

			if ( sobelFinal < 88 ) sobelFinal = 0;
			else sobelFinal = 255;

			/*
			paintValue =  	(sobelFinal <<  0) +
							(sobelFinal <<  8) +
							(sobelFinal << 16) +
							(sobelFinal << 24);
			 */

			paintValue = 0;
			if (sobelFinal > 0) {
				paintValue = 1;
			}

			dst[pos] = paintValue;

			if ( paintValue != 0 ) {

				for ( int r = 0; r < radius; r++ ) {
					if ( nW-r*w > 0 )
						dst[nW-r*w] = paintValue;
					if ( nW+r*w < w*h )
						dst[nW+r*w] = paintValue;
					if ( pW-r*w > 0 )
						dst[pW-r*w] = paintValue;
					if ( pW+r*w < w*h )
						dst[pW+r*w] = paintValue;
					if ( pos-r > 0 )
						dst[pos-r] = paintValue;
					if ( pos+r < w*h )
						dst[pos+r] = paintValue;
				}

			}

		} // End Inner For Loop

	} // End Outer For Loop
}
