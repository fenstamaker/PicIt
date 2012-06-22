package edu.montclair.hci.picit.location;

public class Image {
	public int latitude;
	public int longitude;
	public String url;
	
	public Image(int lat, int lon, String url) {
		latitude = lat;
		longitude = lon;
		this.url = url;
	}
}
