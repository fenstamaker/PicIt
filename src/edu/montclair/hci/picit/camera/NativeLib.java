package edu.montclair.hci.picit.camera;

import java.nio.IntBuffer;

public class NativeLib {
	
	static {
		System.loadLibrary("PicItNativeSobel");
	}
	
	public native void nativeSobel(byte[] frame, int width, int height, IntBuffer output);
	
}
