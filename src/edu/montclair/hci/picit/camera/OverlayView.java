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
	
	private final String TAG = "OverlayView";
	
	private SurfaceHolder overlayHolder;
	private Camera camera;
	private Camera.Size frameSize;
	private long sobelTime;
	private long drawTime;
	
	private NativeLib nativeLib;
	
	private IntBuffer nativeFrameSobel;

	public void init() {
		overlayHolder = getHolder();
		nativeLib = new NativeLib();
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
		overlayHolder = null;
		nativeLib = null;
		nativeFrameSobel = null;
		camera.setPreviewCallback(null);
	}
	
	Camera.PreviewCallback previewCallback = new Camera.PreviewCallback() {
		
		public void onPreviewFrame(byte[] frame, Camera camera) {
			
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
			overlayCanvas.drawText("Resolution: " + Integer.toString(resolution.width) + "x" + Integer.toString(resolution.height), 
					overlayCanvas.getWidth()-200, overlayCanvas.getHeight()-40, pt);
			
			overlayHolder.unlockCanvasAndPost(overlayCanvas);
			
		}
	};
	
}
