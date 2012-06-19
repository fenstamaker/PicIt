package edu.montclair.hci.picit.camera;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import java.util.Arrays;

import edu.montclair.hci.picit.R;

import android.content.Context;
import android.graphics.*;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.util.Log;
import android.view.*;
import android.widget.TextView;

public class OverlayView extends SurfaceView {
	
	private SurfaceHolder overlayHolder;
	private Camera camera;
	private Camera.Size frameSize;
	private int[] frameSobel;
	private byte[] convolutedFrame;
	private long sobelTime;
	private long drawTime;
	private TextView textView;
	
	private NativeLib nativeLib;
	
	private IntBuffer nativeFrameSobel;

	public void init() {
		overlayHolder = getHolder();
	}
	
	public OverlayView(Context context) {
		super(context);
		init();
	}
	
	public OverlayView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}
	
	public OverlayView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();
	}
	
	public void setPreviewSize(Camera.Size size) {
		frameSize = size;
		nativeFrameSobel = ByteBuffer.allocateDirect(frameSize.width*frameSize.height*4).asIntBuffer();
	}
	
	public void setCamera(Camera c) {
		camera = c;
		camera.setPreviewCallback(previewCallback);
	}
	
	public void removePreview() {
		camera.setPreviewCallback(null);
	}
	
	Camera.PreviewCallback previewCallback = new Camera.PreviewCallback() {
		
		public void onPreviewFrame(byte[] frame, Camera camera) {
			if ( nativeLib == null ) {
				nativeLib = new NativeLib();
			}
			
			Canvas overlayCanvas = overlayHolder.lockCanvas(null);

			long timeSobel1 = System.currentTimeMillis();
			
			nativeLib.nativeSobel(frame, frameSize.width, frameSize.height, nativeFrameSobel);
			
			sobelTime = System.currentTimeMillis() - timeSobel1;
			
			long timeDraw1 = System.currentTimeMillis();
			
			Paint pt = new Paint();
			overlayCanvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
			
			Rect dst = new Rect(0, 0, frameSize.width, frameSize.height);
			
			nativeFrameSobel.position(0);
			Bitmap bmp = Bitmap.createBitmap(frameSize.width, frameSize.height, Bitmap.Config.ARGB_8888);
			bmp.copyPixelsFromBuffer(nativeFrameSobel);
			nativeFrameSobel.clear();
			
			overlayCanvas.drawBitmap(bmp, null, dst, pt);
			
			drawTime = (System.currentTimeMillis() - timeDraw1);
			
			pt.setTextSize(16);
			pt.setColor(Color.BLACK);
			
			overlayCanvas.drawRect(overlayCanvas.getWidth()-215, overlayCanvas.getHeight()-125, overlayCanvas.getWidth(), overlayCanvas.getHeight(), pt);
			
			pt.setColor(Color.WHITE);
			
			Camera.Parameters params = camera.getParameters();
			int[] fpsRange = new int[2];
			params.getPreviewFpsRange(fpsRange);
			Camera.Size resolution = params.getPreviewSize();
			
			overlayCanvas.drawText("Sobel Time: " + Long.toString(sobelTime) + "ms", 
					overlayCanvas.getWidth()-200, overlayCanvas.getHeight()-100, pt);
			overlayCanvas.drawText("Draw Time : " + Long.toString(drawTime) + "ms", 
					overlayCanvas.getWidth()-200, overlayCanvas.getHeight()-80, pt);
			overlayCanvas.drawText("Camera FPS: " + Integer.toString(fpsRange[params.PREVIEW_FPS_MIN_INDEX]/1000) + "-" + Integer.toString(fpsRange[params.PREVIEW_FPS_MAX_INDEX]/1000),
					overlayCanvas.getWidth()-200, overlayCanvas.getHeight()-60, pt);
			overlayCanvas.drawText("Resolution: " + Integer.toString(resolution.height) + "x" + Integer.toString(resolution.width), 
					overlayCanvas.getWidth()-200, overlayCanvas.getHeight()-40, pt);
			
			overlayHolder.unlockCanvasAndPost(overlayCanvas);
			
		}
	};
	
	private void meanConvolution(byte[] frame) {
		if ( convolutedFrame == null )
			convolutedFrame = new byte[frame.length];
		
		int w = frameSize.width;
		int Y = 0;
		int pos = 0;
		int nW = 0;
		int pW = 0;
		
		int sum = 0;
		byte value = 0;		
		
		for ( int y = 1; y < frameSize.height - 1; y++ ) {
			
			Y = y * w + 1;
			
			for ( int x = 1; x < frameSize.width - 1; x++ ) {
				
				pos = (Y+x);
				pW = pos+w;
				nW = pos-w;
				
				sum  = 0;
				sum += frame[nW-1] + frame[nW] + frame[nW+1] + frame[pos-1] + frame[pos] + frame[pos+1] + frame[pW-1] + frame[pW] + frame[pW+1];
				
				value = Integer.valueOf(sum / 9).byteValue();
				
				convolutedFrame[nW-1]  = value;
				convolutedFrame[nW]    = value;
				convolutedFrame[nW+1]  = value;
				convolutedFrame[pos-1] = value;
				convolutedFrame[pos]   = value;
				convolutedFrame[pos+1] = value;
				convolutedFrame[pW-1]  = value;
				convolutedFrame[pW]    = value;
				convolutedFrame[pW+1]  = value;
				
			} // End of Inner For Loop
			
		} // End of Outer For loop
		
	} // End of Method
	
	private void medianConvolution(byte[] frame) {
		convolutedFrame = Arrays.copyOf(frame, frame.length);
		
		int w = frameSize.width;
		int Y = 0;
		int pos = 0;
		int nW = 0;
		int pW = 0;
		
		byte value;		
		byte[] values = new byte[9];
		
		for ( int y = 1; y < frameSize.height - 1; y++ ) {
			
			Y = y * w + 1;
			
			for ( int x = 1; x < frameSize.width - 1; x++ ) {
				
				pos = (Y+x);
				pW = pos+w;
				nW = pos-w;
				
				values[0] = frame[nW-1];
				values[1] = frame[nW];
				values[2] = frame[nW+1];
				values[3] = frame[pos-1];
				values[4] = frame[pos];
				values[5] = frame[pos+1];
				values[6] = frame[pW-1];
				values[7] = frame[pW];
				values[8] = frame[pW+1];
				
				Arrays.sort(values);
				
				value = values[4];
				
				convolutedFrame[nW-1]  = value;
				convolutedFrame[nW]    = value;
				convolutedFrame[nW+1]  = value;
				convolutedFrame[pos-1] = value;
				convolutedFrame[pos]   = value;
				convolutedFrame[pos+1] = value;
				convolutedFrame[pW-1]  = value;
				convolutedFrame[pW]    = value;
				convolutedFrame[pW+1]  = value;
				
			} // End of Inner For Loop
			
		} // End of Outer For loop
		
	} // End of Method
	
}
