package edu.montclair.hci.picit;

import edu.montclair.hci.picit.location.PicItLocationManager;
import edu.montclair.hci.picit.map.*;
import edu.montclair.hci.picit.camera.*;

import android.app.TabActivity;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.util.Log;
import android.widget.TabHost;

//Comment
public class PicItActivity extends TabActivity {
    /** Called when the activity is first created. */
	private Intent mapIntent;
	private Intent cameraIntent;
	private TabHost tabHost;
	private PicItLocationManager locationService;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        locationService = PicItLocationManager.INSTANCE;
        locationService.init(this);
        
        setContentView(R.layout.main);

    	Resources res = getResources();
        //TabHost tabHost = getTabHost();
    	tabHost = getTabHost();
        TabHost.TabSpec spec;

        mapIntent = new Intent().setClass(this, PicItMapActivity.class);
        
        spec = tabHost.newTabSpec("map").setIndicator("", res.getDrawable(R.drawable.ic_tab_map)).setContent(mapIntent);
        tabHost.addTab(spec);

        cameraIntent = new Intent().setClass(this, PicItCameraActivity.class);
        //PicItCameraActivity.YOFFSET = tabHost.getTabWidget().getChildAt(0).getLayoutParams().height;
        
        spec = tabHost.newTabSpec("camera").setIndicator("", res.getDrawable(R.drawable.ic_tab_camera)).setContent(cameraIntent);
        tabHost.addTab(spec);
        
        tabHost.setCurrentTab(2); 
        //Log.d("height ", "" +tabHost.getTabWidget().getChildAt(0).getLayoutParams().height);
       
    }
    
    @Override
    public void onPause() {
    	super.onPause();
    	
    	locationService.stop();
    	
    }
    
    @Override
    public void onResume() {
    	super.onResume();
        //Log.d("top ", "" +tabHost.getTop());  
    	
    	locationService.requestUpdate();
    }
}