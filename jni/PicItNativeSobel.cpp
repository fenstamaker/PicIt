#include <jni.h>

extern "C" {
	JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output);
};

void meanConvolution(jbyte* src, jint width, jint height, jbyte* dst);
void sobel(jbyte* src, jint width, jint height, jint* dst);

JNIEXPORT void JNICALL Java_edu_montclair_hci_picit_camera_NativeLib_nativeSobel(JNIEnv *env, jclass, jbyteArray frame, jint width, jint height, jobject output) {

	jboolean copy;
	jbyte *src = env->GetByteArrayElements(frame, &copy);
	jint *dst = (jint*) env->GetDirectBufferAddress(output);

	//jbyte mDst[width*height];
	//meanConvolution(src, width, height, mDst);
	sobel(src, width, height, dst);

	env->ReleaseByteArrayElements(frame, src, JNI_ABORT);
}

void meanConvolution(jbyte* src, jint width, jint height, jbyte* dst) {
	int w = width;
	int Y = 0;
	int pos = 0;
	int nW = 0;
	int pW = 0;

	int sum = 0;
	int value = 0;

	for ( int y = 1; y < height - 1; y++ ) {

		Y = y * w + 1;

		for ( int x = 1; x < width - 1; x++ ) {

			pos = (Y+x);
			pW = pos+w;
			nW = pos-w;

			sum  = 0;
			sum += src[nW-1] + src[nW] + src[nW+1] + src[pos-1] + src[pos] + src[pos+1] + src[pW-1] + src[pW] + src[pW+1];

			value = sum / 9;

			dst[nW-1]  = value;
			dst[nW]    = value;
			dst[nW+1]  = value;
			dst[pos-1] = value;
			dst[pos]   = value;
			dst[pos+1] = value;
			dst[pW-1]  = value;
			dst[pW]    = value;
			dst[pW+1]  = value;

		} // End of Inner For Loop

	} // End of Outer For loop
}

void sobel(jbyte* src, jint width, jint height, jint* dst) {
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
	int radius = 2;

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

			paintValue =  	(sobelFinal <<  0) +
							(sobelFinal <<  8) +
							(sobelFinal << 16) +
							(sobelFinal << 24);

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
