package edu.montclair.hci.picit.location;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;

public class HttpRequest {
	
	private HttpClient client;
	private HttpPost post;
	private List<NameValuePair> nameValuePairs;
	private HttpResponse response;
	
	public HttpRequest(String url) {
		client = new DefaultHttpClient();
		post = new HttpPost(url);
		nameValuePairs = new ArrayList<NameValuePair>();
	}
	
	public void addValuePair(String key, String value) {
		nameValuePairs.add(new BasicNameValuePair(key, value));
	}
	
	public void execute() throws ClientProtocolException, IOException {
		post.setEntity(new UrlEncodedFormEntity(nameValuePairs));
        response = client.execute(post);
	}
	
	public String getContent() throws IllegalStateException, IOException {

        HttpEntity entity = response.getEntity();
		InputStream input = null;
		
		input = entity.getContent();
		
	    BufferedReader reader = null;
	    StringBuilder stringBuilder = null;
	    String line = "";
	    
		reader = new BufferedReader(new InputStreamReader(input ,"iso-8859-1"),8);
		
    	stringBuilder = new StringBuilder();
    	line = null;
    	
		while ( (line = reader.readLine()) != null ) {
			stringBuilder.append(line + "\n");
		}

		input.close();
    	
    	return stringBuilder.toString();
	}

}
