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
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;

import com.espressif.AppConstants;
import com.espressif.wifi_provisioning.BuildConfig;
import com.espressif.wifi_provisioning.R;
import com.espressif.provisioning.DeviceConnectionEvent;
import com.espressif.provisioning.ESPConstants;
import com.espressif.provisioning.ESPProvisionManager;
import com.espressif.provisioning.WiFiAccessPoint;
import com.espressif.provisioning.listeners.WiFiScanListener;
import com.espressif.ui.adapters.WiFiListAdapter;
import com.google.android.material.textfield.TextInputLayout;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;

public class WiFiProvisionLanding extends AppCompatActivity {

    private static final String TAG = WiFiProvisionLanding.class.getSimpleName();

    // Request codes
    private static final int REQUEST_FINE_LOCATION = 10;
    private static final int WIFI_SETTINGS_ACTIVITY_REQUEST = 11;

    private Button btnScan, btnPrefix, btnWiFiSettings;
    private ListView listView;
    private TextView textPrefix;
    private ProgressBar progressBar;
    private RelativeLayout prefixLayout;

    private Handler handler;
    private WiFiListAdapter adapter;
    private ESPProvisionManager provisionManager;
    private ArrayList<WiFiAccessPoint> deviceList;
    private SharedPreferences sharedPreferences;

    private int securityType;
    private boolean isScanning = false;
    private String deviceNamePrefix;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wifiprovision_landing);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.title_activity_connect_device);
        setSupportActionBar(toolbar);
        securityType = getIntent().getIntExtra("security_type", 0);

        handler = new Handler();
        deviceList = new ArrayList<>();
        provisionManager = ESPProvisionManager.getInstance(getApplicationContext());
        sharedPreferences = getSharedPreferences(AppConstants.ESP_PREFERENCES, Context.MODE_PRIVATE);
        deviceNamePrefix = sharedPreferences.getString(AppConstants.KEY_WIFI_DEVICE_NAME_PREFIX,
                getResources().getString(R.string.wifi_device_name_prefix));
        initViews();
        EventBus.getDefault().register(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    @Override
    protected void onDestroy() {
        EventBus.getDefault().unregister(this);
        super.onDestroy();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        super.onActivityResult(requestCode, resultCode, data);
        Log.d(TAG, "onActivityResult, requestCode : " + requestCode + ", resultCode : " + resultCode);

        if (requestCode == WIFI_SETTINGS_ACTIVITY_REQUEST) {

            Log.d(TAG, "Returned from WIFI_SETTINGS_ACTIVITY_REQUEST");
            if (ActivityCompat.checkSelfPermission(WiFiProvisionLanding.this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED) {

                btnScan.setEnabled(false);
                btnScan.setAlpha(0.5f);
                btnScan.setTextColor(Color.WHITE);
                btnWiFiSettings.setEnabled(false);
                btnWiFiSettings.setAlpha(0.5f);
                btnWiFiSettings.setTextColor(Color.WHITE);
                progressBar.setVisibility(View.VISIBLE);
                provisionManager.getEspDevice().connectWiFiDevice();
            } else {
                Log.e(TAG, "Not able to connect device as Location permission is not granted.");
                Toast.makeText(WiFiProvisionLanding.this, "Please give location permission to connect device", Toast.LENGTH_LONG).show();
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {

        switch (requestCode) {

            case REQUEST_FINE_LOCATION: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    startScan();
                } else if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_DENIED) {
                    finish();
                }
            }
            break;
        }
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onEvent(DeviceConnectionEvent event) {

        Log.d(TAG, "ON Device Prov Event RECEIVED : " + event.getEventType());
        handler.removeCallbacks(disconnectDeviceTask);

        switch (event.getEventType()) {

            case ESPConstants.EVENT_DEVICE_CONNECTED:
                Log.e(TAG, "Device Connected Event Received");
                ArrayList<String> deviceCaps = provisionManager.getEspDevice().getDeviceCapabilities();
                btnScan.setEnabled(true);
                btnScan.setAlpha(1f);
                btnWiFiSettings.setEnabled(true);
                btnWiFiSettings.setAlpha(1f);
                progressBar.setVisibility(View.GONE);

                if (deviceCaps != null && !deviceCaps.contains("no_pop") && securityType == 1) {

                    goToPopActivity();

                } else if (deviceCaps.contains("wifi_scan")) {

                    goToWifiScanListActivity();

                } else {

                    goToProvisionActivity();
                }
                break;

            case ESPConstants.EVENT_DEVICE_DISCONNECTED:

                btnScan.setEnabled(true);
                btnScan.setAlpha(1f);
                btnWiFiSettings.setEnabled(true);
                btnWiFiSettings.setAlpha(1f);
                progressBar.setVisibility(View.GONE);
                Toast.makeText(WiFiProvisionLanding.this, "Device disconnected", Toast.LENGTH_SHORT).show();
                break;

            case ESPConstants.EVENT_DEVICE_CONNECTION_FAILED:

                btnScan.setEnabled(true);
                btnScan.setAlpha(1f);
                btnWiFiSettings.setEnabled(true);
                btnWiFiSettings.setAlpha(1f);
                progressBar.setVisibility(View.GONE);
                Toast.makeText(WiFiProvisionLanding.this, "Failed to connect with device", Toast.LENGTH_SHORT).show();
//                alertForDeviceNotSupported("Failed to connect with device");
                break;
        }
    }

    private void initViews() {

        btnScan = findViewById(R.id.btn_scan);
        btnPrefix = findViewById(R.id.btn_change_prefix);
        btnWiFiSettings = findViewById(R.id.btn_wifi_settings);
        listView = findViewById(R.id.ble_devices_list);
        textPrefix = findViewById(R.id.prefix_value);
        progressBar = findViewById(R.id.ble_landing_progress_indicator);
        prefixLayout = findViewById(R.id.prefix_layout);
        textPrefix.setText(deviceNamePrefix);

        // Set visibility of Prefix layout
        if (BuildConfig.IS_ALLOWED_FILTERING_BY_PREFIX) {

            prefixLayout.setVisibility(View.VISIBLE);

        } else {
            prefixLayout.setVisibility(View.GONE);
        }

        adapter = new WiFiListAdapter(this, R.layout.item_ble_scan, deviceList);

        // Assign adapter to ListView
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(onDeviceCLickListener);

        btnScan.setOnClickListener(btnScanClickListener);
        btnPrefix.setOnClickListener(btnPrefixChangeClickListener);
        btnWiFiSettings.setOnClickListener(btnWiFiSettingsClickListener);
        startScan();
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

    private void startScan() {

        if (!hasPermissions() || isScanning) {
            return;
        }

        isScanning = true;
        deviceList.clear();
        provisionManager.searchWiFiEspDevices(deviceNamePrefix, wifiScanListener);
        updateProgressAndScanBtn();
    }

    private void stopScan() {

        updateProgressAndScanBtn();

        if (deviceList.size() <= 0) {

            Toast.makeText(WiFiProvisionLanding.this, R.string.error_no_ble_device, Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * This method will update UI (Scan button enable / disable and progressbar visibility)
     */
    private void updateProgressAndScanBtn() {

        if (isScanning) {

            btnScan.setEnabled(false);
            btnScan.setAlpha(0.5f);
            btnScan.setTextColor(Color.WHITE);
            btnWiFiSettings.setEnabled(false);
            btnWiFiSettings.setAlpha(0.5f);
            btnWiFiSettings.setTextColor(Color.WHITE);
            progressBar.setVisibility(View.VISIBLE);
            listView.setVisibility(View.GONE);

        } else {

            btnScan.setEnabled(true);
            btnScan.setAlpha(1f);
            btnWiFiSettings.setEnabled(true);
            btnWiFiSettings.setAlpha(1f);
            progressBar.setVisibility(View.GONE);
            listView.setVisibility(View.VISIBLE);
        }
    }

    private void alertForDeviceNotSupported(String msg) {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setCancelable(false);

        builder.setTitle(R.string.error_title);
        builder.setMessage(msg);

        // Set up the buttons
        builder.setPositiveButton(R.string.btn_ok, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {

                dialog.dismiss();
                finish();
            }
        });

        builder.show();
    }

    private View.OnClickListener btnScanClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            adapter.clear();
            startScan();
        }
    };

    private View.OnClickListener btnWiFiSettingsClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            startActivityForResult(new Intent(Settings.ACTION_WIFI_SETTINGS), WIFI_SETTINGS_ACTIVITY_REQUEST);
        }
    };

    private View.OnClickListener btnPrefixChangeClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            askForPrefix();
        }
    };

    private WiFiScanListener wifiScanListener = new WiFiScanListener() {

        @Override
        public void onWifiListReceived(ArrayList<WiFiAccessPoint> wifiList) {

            isScanning = false;
            deviceList.addAll(wifiList);
            listView.setVisibility(View.VISIBLE);
            adapter.notifyDataSetChanged();
            updateProgressAndScanBtn();
        }

        @Override
        public void onWiFiScanFailed(Exception e) {
            isScanning = false;
            listView.setVisibility(View.VISIBLE);
            adapter.notifyDataSetChanged();
            updateProgressAndScanBtn();
            e.printStackTrace();
        }
    };

    private AdapterView.OnItemClickListener onDeviceCLickListener = new AdapterView.OnItemClickListener() {

        @Override
        public void onItemClick(AdapterView<?> adapterView, View view, int position, long l) {

            WiFiAccessPoint wifiDevice = adapter.getItem(position);
            Log.d(TAG, "=================== Connect to device : " + wifiDevice.getWifiName());

            if (wifiDevice.getSecurity() == ESPConstants.WIFI_OPEN) {

                if (ActivityCompat.checkSelfPermission(WiFiProvisionLanding.this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED) {

                    btnScan.setEnabled(false);
                    btnScan.setAlpha(0.5f);
                    btnScan.setTextColor(Color.WHITE);
                    btnWiFiSettings.setEnabled(false);
                    btnWiFiSettings.setAlpha(0.5f);
                    btnWiFiSettings.setTextColor(Color.WHITE);
                    progressBar.setVisibility(View.VISIBLE);
                    provisionManager.getEspDevice().connectWiFiDevice(WiFiProvisionLanding.this, wifiDevice.getWifiName(), "");
                } else {
                    Log.e(TAG, "Not able to connect device as Location permission is not granted.");
                    Toast.makeText(WiFiProvisionLanding.this, "Please give location permission to connect device", Toast.LENGTH_LONG).show();
                }
            } else {
                askForNetwork(wifiDevice.getWifiName(), wifiDevice.getSecurity());
            }
        }
    };

    private Runnable disconnectDeviceTask = new Runnable() {

        @Override
        public void run() {

            Log.e(TAG, "Disconnect device");
            progressBar.setVisibility(View.GONE);
            alertForDeviceNotSupported(getString(R.string.error_device_not_supported));
        }
    };

    private void askForPrefix() {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setCancelable(true);

        LayoutInflater layoutInflaterAndroid = LayoutInflater.from(this);
        View view = layoutInflaterAndroid.inflate(R.layout.dialog_prefix, null);
        builder.setView(view);
        final EditText etPrefix = view.findViewById(R.id.et_prefix);
        etPrefix.setText(deviceNamePrefix);
        etPrefix.setSelection(etPrefix.getText().length());

        // Set up the buttons
        builder.setPositiveButton(R.string.btn_save, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {

                String prefix = etPrefix.getText().toString();

                if (prefix != null) {
                    prefix = prefix.trim();
                }

                SharedPreferences.Editor editor = sharedPreferences.edit();
                editor.putString(AppConstants.KEY_BLE_DEVICE_NAME_PREFIX, prefix);
                editor.apply();
                deviceNamePrefix = prefix;
                textPrefix.setText(prefix);
                startScan();
            }
        });

        builder.setNegativeButton(R.string.btn_cancel, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
            }
        });

        builder.show();
    }

    private void goToPopActivity() {

        finish();
        Intent popIntent = new Intent(getApplicationContext(), ProofOfPossessionActivity.class);
        popIntent.putExtras(getIntent());
        startActivity(popIntent);
    }

    private void goToWifiScanListActivity() {

        finish();
        Intent wifiListIntent = new Intent(getApplicationContext(), WiFiScanActivity.class);
        wifiListIntent.putExtras(getIntent());
        wifiListIntent.putExtra(AppConstants.KEY_PROOF_OF_POSSESSION, "");
        startActivity(wifiListIntent);
    }

    private void goToProvisionActivity() {

        finish();
        Intent provisionIntent = new Intent(getApplicationContext(), ProvisionActivity.class);
        provisionIntent.putExtras(getIntent());
        provisionIntent.putExtra(AppConstants.KEY_PROOF_OF_POSSESSION, "");
        startActivity(provisionIntent);
    }

    private void askForNetwork(final String ssid, final int authMode) {

        final AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = this.getLayoutInflater();
        final View dialogView = inflater.inflate(R.layout.dialog_wifi_network, null);
        builder.setView(dialogView);

        final EditText etSsid = dialogView.findViewById(R.id.et_ssid);
        final EditText etPassword = dialogView.findViewById(R.id.et_password);

        builder.setTitle(ssid);
        etSsid.setVisibility(View.GONE);

        builder.setPositiveButton(R.string.btn_connect, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {

                String password = etPassword.getText().toString();

                if (TextUtils.isEmpty(password)) {

                    if (authMode != ESPConstants.WIFI_OPEN) {

                        TextInputLayout passwordLayout = dialogView.findViewById(R.id.layout_password);
                        passwordLayout.setError(getString(R.string.error_password_empty));

                    } else {

                        dialog.dismiss();
                        if (ActivityCompat.checkSelfPermission(WiFiProvisionLanding.this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

                            Log.e(TAG, "Not able to connect device as Location permission is not granted.");
                            Toast.makeText(WiFiProvisionLanding.this, "Please give location permission to connect device", Toast.LENGTH_LONG).show();
                        }
                        btnScan.setEnabled(false);
                        btnScan.setAlpha(0.5f);
                        btnScan.setTextColor(Color.WHITE);
                        btnWiFiSettings.setEnabled(false);
                        btnWiFiSettings.setAlpha(0.5f);
                        btnWiFiSettings.setTextColor(Color.WHITE);
                        progressBar.setVisibility(View.VISIBLE);
                        provisionManager.getEspDevice().connectWiFiDevice(WiFiProvisionLanding.this, ssid, password);
                    }

                } else {

                    if (authMode == ESPConstants.WIFI_OPEN) {
                        password = "";
                    }
                    dialog.dismiss();
                    btnScan.setEnabled(false);
                    btnScan.setAlpha(0.5f);
                    btnScan.setTextColor(Color.WHITE);
                    btnWiFiSettings.setEnabled(false);
                    btnWiFiSettings.setAlpha(0.5f);
                    btnWiFiSettings.setTextColor(Color.WHITE);
                    progressBar.setVisibility(View.VISIBLE);
                    provisionManager.getEspDevice().connectWiFiDevice(WiFiProvisionLanding.this, ssid, password);
                }
            }
        });

        builder.setNegativeButton(R.string.btn_cancel, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {

                dialog.dismiss();
            }
        });

        AlertDialog alertDialog = builder.create();
        alertDialog.show();
    }
}
