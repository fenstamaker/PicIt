package edu.montclair.hci.picit.location;

import android.util.Log;

public class LocationDTO {
	
	private static final String TAG = "LocationDTO";
	
	HttpRequest request;
	
	public LocationDTO() {
		
	}
	
	public String getAll() {
		request = new HttpRequest("http://hci.montclair.edu/android/get_locations.php");
		String result = "";
		
		try {
			request.execute();
			result = request.getContent();
		} catch (Exception e) {
			Log.e(TAG, "Failed to connect: " + e.getLocalizedMessage());
		}
		
		return result;
	}
	
	public String get(int Id) {
		request = new HttpRequest("http://hci.montclair.edu/android/get_locations.php");
		request.addValuePair("id", Integer.toString(Id));
		String result = "";
		
		try {
			request.execute();
			result = request.getContent();
		} catch (Exception e) {
			Log.e(TAG, "Failed to connect: " + e.getLocalizedMessage());
		}
		
		return result;
	}
	
	public String get(long lat, long lon) {
		request = new HttpRequest("http://hci.montclair.edu/android/get_locations.php");
		request.addValuePair("lat", Long.toString(lat));
		request.addValuePair("lon", Long.toString(lon));
		String result = "";
		
		try {
			request.execute();
			result = request.getContent();
		} catch (Exception e) {
			Log.e(TAG, "Failed to connect: " + e.getLocalizedMessage());
		}
		
		return result;
	}
	
	public boolean add(String title, String desc, double lat, double lon) {
		request = new HttpRequest("http://hci.montclair.edu/android/add_locations.php");
		
		request.addValuePair("submit", "Submit");
		request.addValuePair("title", title);
		request.addValuePair("description", desc);
		request.addValuePair("lat", Double.toString(lat));
		request.addValuePair("long", Double.toString(lon));
		
		String result = "";
		try {
			request.execute();
			result = request.getContent();
		} catch (Exception e) {
			Log.e(TAG, "Failed to connect: " + e.getLocalizedMessage());
			return false;
		}
		
		if ( result.equals("YES") ) {
			return true;
		}
		
		Log.e(TAG, "add_location.php: Failed to add: " + result);
		return false;
	}

}
