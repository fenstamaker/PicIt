package edu.montclair.hci.picit.location;

import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;

public class Image {
	public int latitude;
	public int longitude;
	public String url;
	public Drawable photo;
	public String tag;
	
	public Image(int lat, int lon, String url, String tag) throws MalformedURLException, IOException {
		latitude = lat;
		longitude = lon;
		this.url = url;
		this.tag = tag;
		HttpURLConnection connection = (HttpURLConnection)new URL(this.url).openConnection();
	    photo = new BitmapDrawable( BitmapFactory.decodeStream(connection.getInputStream()) );
	    photo.setBounds(0, 0, photo.getIntrinsicWidth(), photo.getIntrinsicHeight());
	    connection.disconnect();
	}
}
