package edu.montclair.hci.picit;

import edu.montclair.hci.picit.map.*;
import edu.montclair.hci.picit.camera.*;

import android.app.TabActivity;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.widget.TabHost;

//Comment
public class PicItActivity extends TabActivity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        Resources res = getResources();
        TabHost tabHost = getTabHost();
        TabHost.TabSpec spec;
        Intent intent;

        intent = new Intent().setClass(this, PicItMapActivity.class);
        
        spec = tabHost.newTabSpec("map").setIndicator("Map", res.getDrawable(R.drawable.ic_tab_map)).setContent(intent);
        tabHost.addTab(spec);

        intent = new Intent().setClass(this, PicItCameraActivity.class);
        
        spec = tabHost.newTabSpec("camera").setIndicator("Camera", res.getDrawable(R.drawable.ic_tab_camera)).setContent(intent);
        tabHost.addTab(spec);
        
        tabHost.setCurrentTab(2);
    }
}