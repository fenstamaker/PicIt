package edu.montclair.hci.picit.location;

import android.graphics.Bitmap;

public class Location {
	
	public String title;
	public String description;
	public int latitude;
	public int longitude;
	
	public Location(String t, String d, int lat, int lon) {
		title = t;
		description = d;
		latitude = lat;
		longitude = lon;
	}

}
 