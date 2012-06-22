package edu.montclair.hci.picit.location;

import java.io.IOException;
import java.util.ArrayList;

import org.apache.http.client.ClientProtocolException;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.util.Log;

public class LocationDTO {
	
	private static final String TAG = "ImageDTO";
	
	private HttpRequest request;
	
	public LocationDTO() {
		
	}
	

	public ArrayList<Location> getAllLocations() throws JSONException {
		ArrayList<Location> locations = new ArrayList<Location>();
		
		String result = this.getAllString();
    	
    	JSONArray jsonArray = new JSONArray(result);
    	
    	for ( int i = 0; i < jsonArray.length(); i++ ) {
    		JSONObject obj = jsonArray.getJSONObject(i);
    		String title = obj.getString("Title");
    		String desc = obj.getString("Description");
    		int lat = obj.getInt("Lat");
    		int lon = obj.getInt("Lon");
    		locations.add(new Location(title, desc, lat, lon));
    	}
		
		return locations;
	}
	
	
	public String getAllString() {
		request = new HttpRequest("http://hci.montclair.edu/android/get_locations.php");
		String result = "";
		
		try {
			request.execute();
			result = request.getContent();
		} catch (ClientProtocolException e) {
			Log.e(TAG, "Failed to connect (ClientProtocolException): " + e.getMessage());
		} catch (IOException e) {
			Log.e(TAG, "Failed to connect (IOException): " + e.getMessage());
		} catch (IllegalStateException e) {
			Log.e(TAG, "Failed to get content (IllegalStateException): " + e.getMessage());
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
