package edu.montclair.hci.picit.location;

import java.io.IOException;
import java.util.ArrayList;

import org.apache.http.client.ClientProtocolException;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.util.Log;

public class ImageDTO {
	
private static final String TAG = "LocationDTO";
	
	private HttpRequest request;
	
	public ImageDTO() {
		
	}
	
	public ArrayList<Image> getAllImages(int lat, int lon) throws JSONException {
		ArrayList<Image> images = new ArrayList<Image>();
		
		String result = this.getAllString(lat, lon);
    	
    	JSONArray jsonArray = new JSONArray(result);
    	
    	for ( int i = 0; i < jsonArray.length(); i++ ) {
    		JSONObject obj = jsonArray.getJSONObject(i);
    		String url = obj.getString("ImagePath");
    		int lat1 = obj.getInt("Lat");
    		int lon1 = obj.getInt("Lon");
    		images.add(new Image(lat1, lon1, url));
    	}
		
		return images;
	}
	
	public String getAllString(int lat, int lon) {
		request = new HttpRequest("http://hci.montclair.edu/android/get_locations.php");
		request.addValuePair("lat", Integer.toString(lat));
		request.addValuePair("lon", Integer.toString(lon));
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

}
