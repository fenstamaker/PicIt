package edu.montclair.hci.picit.camera;

import edu.montclair.hci.picit.R;
import edu.montclair.hci.picit.camera.OverlayView;
import edu.montclair.hci.picit.location.*;

import java.io.IOException;
import java.util.List;

import com.google.android.maps.GeoPoint;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Size;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Base64;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class PicItCameraActivity extends Activity implements SurfaceHolder.Callback, Camera.AutoFocusCallback, OnClickListener {
	
	private final String TAG = "PicItCameraActivity";
	
	private Camera camera = null;
	
	private SurfaceView surfaceView = null;
	private OverlayView overlayView = null;
	private SurfaceHolder surfaceHolder = null;
	
	private int numberOfCameras;
	private int defaultCameraId;
	
	private ProgressDialog progressDialog;
	
	private byte[] photoData;
	
	private PicItLocationManager locationService = PicItLocationManager.INSTANCE;

	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.camera_layout);
        surfaceView = (SurfaceView)findViewById(R.id.preview);       
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        
        overlayView = (OverlayView)findViewById(R.id.drawOverlay);
        overlayView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        
        Button button = (Button)findViewById(R.id.button1);
        button.setOnClickListener(this);
        
    	numberOfCameras = Camera.getNumberOfCameras();
        
        // Grabs the back facing camera
        CameraInfo cameraInfo = new CameraInfo();
        for ( int i = 0; i < numberOfCameras; i++ ) {
        	Camera.getCameraInfo(i, cameraInfo);
        	if ( cameraInfo.facing == CameraInfo.CAMERA_FACING_BACK ) {
        		defaultCameraId = i;
        	}
        }
        
    }
    
    @Override
    public void onPause() {
    	super.onPause();
    	
    	if ( camera != null ) {
    		//preview.setCamera(null);
    		overlayView.removePreview();
    		camera.stopPreview();
    		camera.release();
    		camera = null;
    		Log.d(TAG, "Camera Released");
    	}
    }
    
    @Override
    public void onStop() {
    	super.onStop();
    	
    	Log.d(TAG, "Finished");
    	finish();
    }
    
    @Override
    public void onResume() {
    	super.onResume();

		Log.d("PicItCameraActivity", "onResume");
    	camera = Camera.open(defaultCameraId);
        overlayView.setCamera(camera);
        
        overlayView.init();
        surfaceView.requestLayout();
        overlayView.invalidate();
    }
    
    public void startPreview() {
    	if ( camera != null ) {
        	camera.startPreview();
    	}
    }
    
    private Size getOptimalSize(int width, int height) {
    	
    	Camera.Parameters params = camera.getParameters();
    	List<Size> sizes = params.getSupportedPreviewSizes();
    	
    	int targetHeight = height;
    	int targetWidth = width;
    	final double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) width / height;
    	Size optimalSize = null;
        double minDiff = Double.MAX_VALUE;
        
    	// Try to find an size match aspect ratio and size
        for (Size size : sizes) {
        	Log.d(TAG, "Width: " + size.width + " Height: " + size.height);
            double ratio = (double) size.width / size.height;
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE) continue;
            if (Math.abs(size.width - targetWidth) < minDiff) {
                optimalSize = size;
                minDiff = Math.abs(size.width - targetWidth);
            }
        }
    	
        if (optimalSize == null) {
            minDiff = Double.MAX_VALUE;
            for (Size size : sizes) {
                if (Math.abs(size.width - targetWidth) < minDiff) {
                    optimalSize = size;
                    minDiff = Math.abs(size.width - targetWidth);
                }
            }
        }

    	Log.d(TAG, "Optimal Width: " + optimalSize.width + " Optimal Height: " + optimalSize.height);
    	return optimalSize;    	
    }
    
    private Size getOptimalPictureSize(int width, int height) {
    	
    	Camera.Parameters params = camera.getParameters();
    	List<Size> sizes = params.getSupportedPictureSizes();
    	
    	int targetHeight = height;
    	int targetWidth = width;
    	final double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) width / height;
    	Size optimalSize = null;
        double minDiff = Double.MAX_VALUE;
        
    	// Try to find an size match aspect ratio and size
        for (Size size : sizes) {
        	Log.d(TAG, "Width: " + size.width + " Height: " + size.height);
            double ratio = (double) size.width / size.height;
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE) continue;
            if (Math.abs(size.width - targetWidth) < minDiff) {
                optimalSize = size;
                minDiff = Math.abs(size.width - targetWidth);
            }
        }
    	
        if (optimalSize == null) {
            minDiff = Double.MAX_VALUE;
            for (Size size : sizes) {
                if (Math.abs(size.width - targetWidth) < minDiff) {
                    optimalSize = size;
                    minDiff = Math.abs(size.width - targetWidth);
                }
            }
        }

    	Log.d(TAG, "Optimal Width: " + optimalSize.width + " Optimal Height: " + optimalSize.height);
    	return optimalSize;    	
    }
    
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		try {
			camera.setPreviewDisplay(surfaceHolder);
		} catch ( IOException e ) {
			Log.e(TAG, "IOException: setPreviewDisplay()", e);
		}
		
		Size optimalSize = null;
		
		if (height > 480) {
			optimalSize = getOptimalSize(800, 480);
		} else {
			optimalSize = getOptimalSize(width, height);
		}
		
		Size pictureSize = getOptimalPictureSize(800, 480);
		
		Camera.Parameters params = camera.getParameters();
		params.setPreviewSize(optimalSize.width, optimalSize.height);
		params.setPictureSize(pictureSize.width, pictureSize.height);
		params.setPictureFormat(ImageFormat.JPEG);
		
		camera.setParameters(params);
		
		surfaceView.layout(0, 0, optimalSize.width, optimalSize.height);
		overlayView.setPreviewSize(optimalSize);
		
    	startPreview();
	}

	public void surfaceCreated(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		
	}

	public void surfaceDestroyed(SurfaceHolder arg0) {
		// TODO Auto-generated method stub
		
	}

	public void onClick(View arg0) {
		camera.autoFocus(this);	
	}

	public void onAutoFocus(boolean arg0, Camera arg1) {
		progressDialog = ProgressDialog.show(this, "Uploading", "");
		camera.takePicture(null, null, photoCallback);	
	}

	private Camera.PictureCallback photoCallback = new Camera.PictureCallback() {
		
		public void onPictureTaken(byte[] data, Camera camera) {
			if ( data != null ) {
				photoData = data;
				new Thread(uploadPhoto).start();
			}
		}
	};
	
	private Runnable uploadPhoto = new Runnable() {

		public void run() {
			try {
				
				HttpRequest upload = new HttpRequest("http://hci.montclair.edu/android/add_image.php");
				upload.addValuePair("submit", "Submit");
				upload.addValuePair("image", Base64.encodeToString(photoData, Base64.DEFAULT) );
				upload.addValuePair("lat", Integer.toString(locationService.getCurrent().getLatitudeE6()) );
				upload.addValuePair("lon", Integer.toString(locationService.getCurrent().getLongitudeE6()) );
				upload.execute();
				
			} catch ( Exception e ) {
				Log.e(TAG+".photoCallback", "Upload Error: ", e);
			}
			
			progressDialog.dismiss();
			startPreview();
		}
	};
    
}