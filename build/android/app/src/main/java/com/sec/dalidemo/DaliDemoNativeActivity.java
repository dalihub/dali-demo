package com.sec.dalidemo;

import android.annotation.TargetApi;
import android.app.NativeActivity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;

public class DaliDemoNativeActivity extends NativeActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    protected void onResume() {
        super.onResume();
    }

    protected void onPause() {
        super.onPause();
    }

    public final void setSoftInputMode(boolean visible) {
        if (visible) {
            getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
        }
        else {
            getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);
        }
    }

    public final String getMetaData(String key) {
        try {
            ActivityInfo ai = getApplicationContext().getPackageManager()
                    .getActivityInfo(getComponentName(), PackageManager.GET_META_DATA);
            Bundle bundle = ai.metaData;
            if (bundle != null)
                return bundle.getString(key);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }

        return null;
    }

    public final String getIntentStringExtra(String key) {
        return getIntent().getStringExtra(key);
    }

    public final void launchExample(String exampleName) {
        Intent intent = new Intent(this, DaliDemoNativeActivity.class);
        intent.putExtra("start", exampleName);
        startActivity(intent);
    }

    /**
     * JNI method to get the graphics backend preference from native code
     * This method can be called from C++ code via JNI
     */
    public String getGraphicsBackendPreference() {
        SharedPreferences prefs = getSharedPreferences("DaliGraphicsPrefs", Context.MODE_PRIVATE);
        return prefs.getString("graphics_backend_preference", "");
    }
}
