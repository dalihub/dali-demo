package com.sec.dalidemo;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Toast;

public class DaliSettingsActivity extends Activity {

    private static final String PREFS_NAME = "DaliGraphicsPrefs";
    private static final String GRAPHICS_BACKEND_KEY = "graphics_backend_preference";
    private static final String VULKAN_VALUE = "vulkan";
    private static final String GLES_VALUE = "gles";

    private RadioGroup graphicsBackendRadioGroup;
    private RadioButton vulkanRadioButton;
    private RadioButton glesRadioButton;
    private Button saveButton;
    private SharedPreferences sharedPreferences;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        // Initialize views
        graphicsBackendRadioGroup = findViewById(R.id.graphicsBackendRadioGroup);
        vulkanRadioButton = findViewById(R.id.vulkanRadioButton);
        glesRadioButton = findViewById(R.id.glesRadioButton);
        saveButton = findViewById(R.id.saveButton);

        // Initialize SharedPreferences
        sharedPreferences = getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);

        // Load current setting
        loadCurrentSetting();

        // Set up save button click listener
        saveButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                saveSetting();
            }
        });
    }

    private void loadCurrentSetting() {
        String currentBackend = sharedPreferences.getString(GRAPHICS_BACKEND_KEY, "");

        if (currentBackend.isEmpty()) {
            // No preference saved, default to GLES
            glesRadioButton.setChecked(true);
        } else if (currentBackend.equals(VULKAN_VALUE)) {
            vulkanRadioButton.setChecked(true);
        } else if (currentBackend.equals(GLES_VALUE)) {
            glesRadioButton.setChecked(true);
        } else {
            // Unknown value, default to GLES
            glesRadioButton.setChecked(true);
        }
    }

    private void saveSetting() {
        String selectedBackend;

        if (vulkanRadioButton.isChecked()) {
            selectedBackend = VULKAN_VALUE;
        } else if (glesRadioButton.isChecked()) {
            selectedBackend = GLES_VALUE;
        } else {
            // No option selected, show error and return
            Toast.makeText(this, "Please select a graphics backend", Toast.LENGTH_SHORT).show();
            return;
        }

        // Save to SharedPreferences
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putString(GRAPHICS_BACKEND_KEY, selectedBackend);
        editor.apply();

        // Show confirmation message
        String message = "Graphics backend set to " + selectedBackend.toUpperCase();
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();

        // Close the settings activity
        finish();
    }

    /**
     * JNI method to get the graphics backend preference from native code
     * This method can be called from C++ code via JNI
     */
    public static String getGraphicsBackendPreference(Context context) {
        if (context == null) {
            return "";
        }

        SharedPreferences prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
        return prefs.getString(GRAPHICS_BACKEND_KEY, "");
    }
}
