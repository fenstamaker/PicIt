package edu.montclair.hci.picit.map;

import java.util.ArrayList;
import java.util.List;

import org.json.JSONException;

import android.graphics.drawable.Drawable;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;

import com.google.android.maps.*;

import edu.montclair.hci.picit.R;
import edu.montclair.hci.picit.location.*;

public class PicItMapActivity extends MapActivity {
	
	private final String TAG = "PicItMapActivity";
	
	private LocationDTO locationDTO = new LocationDTO();
	private ArrayList<Location> locations = new ArrayList<Location>();
	
	private ImageDTO imageDTO = new ImageDTO();
	private ArrayList<Image> images = new ArrayList<Image>();
	
	private GeoPoint current = null;
	
	private MapOverlay pointOverlay;
	private MapOverlay imageOverlay;
	private List<Overlay> mapOverlays;
	private MyLocationOverlay myLocationOverlay;
	
	private MapView mapView;
	private MapController mapController;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.map_layout);
		
		mapView = (MapView) findViewById(R.id.mapView);
	    mapView.setBuiltInZoomControls(true);
	    mapController = mapView.getController();
	    
	    mapOverlays = mapView.getOverlays();
	    Drawable drawable = this.getResources().getDrawable(R.drawable.location);
	    pointOverlay = new MapOverlay(drawable, this);
	    Drawable imageDrawable = this.getResources().getDrawable(R.drawable.point);
	    imageOverlay = new MapOverlay(imageDrawable, this);
	    
	    // MyLocationOverlay automatically finds the user's location
	    // and puts it on the back.
	    myLocationOverlay = new MyLocationOverlay(this, mapView);
	    mapOverlays.add(myLocationOverlay);
	    
	    // Runnable object that runs after finding the first location fix
	    // Will center the map and zoom in on user than downloads surrounding images
	    myLocationOverlay.runOnFirstFix(new Runnable() {
			public void run() {
				current = myLocationOverlay.getMyLocation();
				mapController.animateTo(current);
				mapController.setZoom(18);

			    DownloadImages imageTask = new DownloadImages();
			    imageTask.execute(new Void[0]);
			}
		});
	    
	    DownloadLocations task = new DownloadLocations();
	    task.execute(new Void[0]);
	}

	@Override
	public void onResume() {
		super.onResume();

	    myLocationOverlay.enableMyLocation();
	}

	@Override
	public void onPause() {
		super.onPause();

	    myLocationOverlay.disableMyLocation();
	}

	@Override
	protected boolean isRouteDisplayed() {
		// TODO Auto-generated method stub
		return false;
	}

	private class DownloadLocations extends AsyncTask<Void, Void, ArrayList<Location> > {

		@Override
		protected ArrayList<Location> doInBackground(Void... params) {

			try {
				locations = locationDTO.getAllLocations();
			} catch (JSONException e) {
				Log.e(TAG, "Failed to parse JSON: " + e.getLocalizedMessage());
			}
			    			
			return locations;
		}
		
		protected void onPostExecute(ArrayList<Location> locations) {
			for(Location location : locations) {
			    GeoPoint point = new GeoPoint(location.latitude,location.longitude);
			    OverlayItem overlayitem = new OverlayItem(point, location.title, location.description);
			    pointOverlay.addOverlay(overlayitem);
		    }
	 	        
		    mapOverlays.add(pointOverlay);			
		}
		
	}
	
	private class DownloadImages extends AsyncTask<Void, Void, ArrayList<Image> > {

		@Override
		protected ArrayList<Image> doInBackground(Void... params) {
			try {
				images = imageDTO.getAllImages(current.getLatitudeE6(), current.getLongitudeE6());
			} catch (JSONException e) {
				Log.e(TAG, "Failed to parse JSON: " + e.getLocalizedMessage());
			}
			    			
			return images;
		}
		
		protected void onPostExecute(ArrayList<Image> images) {

			if ( !images.isEmpty() ) {
				for(Image image : images) {
				    GeoPoint point = new GeoPoint(image.latitude,image.longitude);
				    OverlayItem overlayitem = new OverlayItem(point, "Image", "Image");
				    imageOverlay.addOverlay(overlayitem);
			    }
		 	        
			    mapOverlays.add(imageOverlay);		
			}
		}
		
	}
}
