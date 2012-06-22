package edu.montclair.hci.picit.location;

import android.content.Context;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;

import com.google.android.maps.GeoPoint;

public enum PicItLocationManager {
	INSTANCE;
	
	private final String TAG = "PicItLocationManager";

	private LocationManager locationManager = null;
	private String provider;
	private GeoPoint current;
	
	public void init(Context context) {
		if ( locationManager == null ) {
			locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
			provider = LocationManager.NETWORK_PROVIDER;
		}
		requestUpdate();
		setCurrent();
	}
	
	public void setCurrent() {
		android.location.Location location = locationManager.getLastKnownLocation(provider);
		if (location != null) {
			current = new GeoPoint((int)(location.getLatitude() * 1e6), (int)(location.getLongitude() * 1e6));
		}
	}
	
	public GeoPoint getCurrent() {
		return current;
	}
	
	public void requestUpdate() {
      	locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 0, 0, locationListener);
      	locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 0, 0, locationListener);
	}
	
	public void stop() {
		locationManager.removeUpdates(locationListener);
	}
	
	LocationListener locationListener = new LocationListener() {

		public void onLocationChanged(android.location.Location location) {	
			Log.d(TAG, "Current set");
			current = new GeoPoint((int)(location.getLatitude() * 1e6), (int)(location.getLongitude() * 1e6));
		}

		public void onProviderDisabled(String provider) {
			// TODO Auto-generated method stub
			
		}

		public void onProviderEnabled(String provider) {
			// TODO Auto-generated method stub
			
		}

		public void onStatusChanged(String provider, int status, Bundle extras) {
			// TODO Auto-generated method stub
			
		}
		
	};	
	
}
