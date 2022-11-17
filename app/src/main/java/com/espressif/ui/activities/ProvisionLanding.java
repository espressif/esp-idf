// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package com.espressif.ui.activities;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.cardview.widget.CardView;
import androidx.core.app.ActivityCompat;
import androidx.core.widget.ContentLoadingProgressBar;

import com.espressif.AppConstants;
import com.espressif.provisioning.DeviceConnectionEvent;
import com.espressif.provisioning.ESPConstants;
import com.espressif.ui.utils.Utils;
import com.espressif.wifi_provisioning.R;

import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;

public class ProvisionLanding extends ManualProvBaseActivity {

    private static final String TAG = ProvisionLanding.class.getSimpleName();

    private static final int REQUEST_FINE_LOCATION = 10;
    private static final int WIFI_SETTINGS_ACTIVITY_REQUEST = 11;

    private TextView tvTitle, tvBack, tvCancel;
    private CardView btnConnect;
    private TextView txtConnectBtn;
    private ImageView arrowImage;
    private ContentLoadingProgressBar progressBar;
    private TextView tvConnectDeviceInstruction, tvDeviceName;

    private String deviceName, pop;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_provision_landing);
        deviceName = getIntent().getStringExtra(AppConstants.KEY_DEVICE_NAME);
        pop = getIntent().getStringExtra(AppConstants.KEY_PROOF_OF_POSSESSION);
        initViews();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        super.onActivityResult(requestCode, resultCode, data);

        switch (requestCode) {
            case WIFI_SETTINGS_ACTIVITY_REQUEST:
                if (hasPermissions()) {
                    connectDevice();
                }
                break;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {

            case REQUEST_FINE_LOCATION:
                // TODO
                break;
        }
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onEvent(DeviceConnectionEvent event) {

        Log.d(TAG, "On Device Prov Event RECEIVED : " + event.getEventType());

        switch (event.getEventType()) {

            case ESPConstants.EVENT_DEVICE_CONNECTED:

                Log.d(TAG, "Device Connected Event Received");
                btnConnect.setEnabled(true);
                btnConnect.setAlpha(1f);
                txtConnectBtn.setText(R.string.btn_connect);
                progressBar.setVisibility(View.GONE);
                arrowImage.setVisibility(View.VISIBLE);
                setSecurityTypeFromVersionInfo();
                boolean isSecure = sharedPreferences.getBoolean(AppConstants.KEY_SECURITY_TYPE, true);
                if (isSecure) {
                    if (securityType == AppConstants.SEC_TYPE_0) {
                        Utils.displayDeviceConnectionError(this, getString(R.string.error_security_mismatch));
                    } else {
                        processDeviceCapabilities();
                    }
                } else {
                    if (securityType != AppConstants.SEC_TYPE_0) {
                        Utils.displayDeviceConnectionError(this, getString(R.string.error_security_mismatch));
                    } else {
                        processDeviceCapabilities();
                    }
                }
                break;

            case ESPConstants.EVENT_DEVICE_CONNECTION_FAILED:

                btnConnect.setEnabled(true);
                btnConnect.setAlpha(1f);
                txtConnectBtn.setText(R.string.btn_connect);
                progressBar.setVisibility(View.GONE);
                arrowImage.setVisibility(View.VISIBLE);
                Utils.displayDeviceConnectionError(this, getString(R.string.error_device_connect_failed));
                break;
        }
    }

    View.OnClickListener btnConnectClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            startActivityForResult(new Intent(Settings.ACTION_WIFI_SETTINGS), WIFI_SETTINGS_ACTIVITY_REQUEST);
        }
    };

    private void connectDevice() {

        btnConnect.setEnabled(false);
        btnConnect.setAlpha(0.5f);
        txtConnectBtn.setText(R.string.btn_connecting);
        progressBar.setVisibility(View.VISIBLE);
        arrowImage.setVisibility(View.GONE);

        if (ActivityCompat.checkSelfPermission(ProvisionLanding.this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED) {

            provisionManager.getEspDevice().connectWiFiDevice();
        } else {
            Log.e(TAG, "Not able to connect device as Location permission is not granted.");
            Toast.makeText(ProvisionLanding.this, "Please give location permission to connect device", Toast.LENGTH_LONG).show();
        }
    }

    private View.OnClickListener cancelButtonClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            finish();
        }
    };

    private void initViews() {

        tvTitle = findViewById(R.id.main_toolbar_title);
        tvBack = findViewById(R.id.btn_back);
        tvCancel = findViewById(R.id.btn_cancel);

        tvTitle.setText(R.string.title_activity_connect_device);
        tvBack.setVisibility(View.GONE);
        tvCancel.setVisibility(View.VISIBLE);

        tvCancel.setOnClickListener(cancelButtonClickListener);

        btnConnect = findViewById(R.id.btn_connect);
        txtConnectBtn = findViewById(R.id.text_btn);
        arrowImage = findViewById(R.id.iv_arrow);
        progressBar = findViewById(R.id.progress_indicator);
        tvConnectDeviceInstruction = findViewById(R.id.tv_connect_device_instruction);
        tvDeviceName = findViewById(R.id.tv_device_name);
        String instruction = getString(R.string.connect_device_instruction_general);

        if (TextUtils.isEmpty(deviceName)) {

            tvConnectDeviceInstruction.setText(instruction);
            tvDeviceName.setVisibility(View.GONE);

        } else {

            instruction = getString(R.string.connect_device_instruction_specific);
            tvConnectDeviceInstruction.setText(instruction);
            tvDeviceName.setVisibility(View.VISIBLE);
            tvDeviceName.setText(deviceName);
        }

        txtConnectBtn.setText(R.string.btn_connect);
        btnConnect.setOnClickListener(btnConnectClickListener);
        hasPermissions();
    }

    private void processDeviceCapabilities() {

        ArrayList<String> deviceCaps = provisionManager.getEspDevice().getDeviceCapabilities();

        if (!TextUtils.isEmpty(pop)) {

            provisionManager.getEspDevice().setProofOfPossession(pop);

            if (deviceCaps != null && deviceCaps.contains("wifi_scan")) {
                goToWifiScanListActivity();
            } else {
                goToWiFiConfigActivity();
            }
        } else {

            if (deviceCaps != null && !deviceCaps.contains("no_pop") && securityType != AppConstants.SEC_TYPE_0) {
                goToPopActivity();
            } else if (deviceCaps != null && deviceCaps.contains("wifi_scan")) {
                goToWifiScanListActivity();
            } else {
                goToWiFiConfigActivity();
            }
        }
    }

    private void goToPopActivity() {

        finish();
        Intent popIntent = new Intent(getApplicationContext(), ProofOfPossessionActivity.class);
        startActivity(popIntent);
    }

    private void goToWifiScanListActivity() {

        finish();
        Intent wifiListIntent = new Intent(getApplicationContext(), WiFiScanActivity.class);
        startActivity(wifiListIntent);
    }

    private void goToWiFiConfigActivity() {

        finish();
        Intent wifiConfigIntent = new Intent(getApplicationContext(), WiFiConfigActivity.class);
        startActivity(wifiConfigIntent);
    }

    private boolean hasPermissions() {

        if (!hasLocationPermissions()) {

            requestLocationPermission();
            return false;
        }
        return true;
    }

    private boolean hasLocationPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            return checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
        }
        return true;
    }

    private void requestLocationPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_FINE_LOCATION);
        }
    }
}
