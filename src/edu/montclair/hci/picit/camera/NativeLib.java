package edu.montclair.hci.picit.camera;

import java.nio.IntBuffer;

import edu.montclair.hci.picit.ml.DecisionTree;

public class NativeLib {
	
	static {
		System.loadLibrary("PicItNativeSobel");
	}
	
	public native void nativeSobel(byte[] frame, int width, int height, IntBuffer output, DecisionTree tree);
	
}
