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

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.cardview.widget.CardView;

import com.espressif.AppConstants;
import com.espressif.wifi_provisioning.BuildConfig;
import com.espressif.wifi_provisioning.R;
import com.espressif.provisioning.ESPConstants;
import com.espressif.provisioning.ESPProvisionManager;

public class EspMainActivity extends AppCompatActivity {

    private static final String TAG = EspMainActivity.class.getSimpleName();

    // Request codes
    private static final int REQUEST_LOCATION = 1;
    private static final int REQUEST_ENABLE_BT = 2;

    private ESPProvisionManager provisionManager;
    private CardView btnAddDevice, btnScanQRCode;
    private SharedPreferences sharedPreferences;
    private String deviceType;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_esp_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        initViews();

        sharedPreferences = getSharedPreferences(AppConstants.ESP_PREFERENCES, Context.MODE_PRIVATE);
        provisionManager = ESPProvisionManager.getInstance(getApplicationContext());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    protected void onResume() {
        super.onResume();

//        if (BuildConfig.FLAVOR_transport.equals("ble") && BLEProvisionLanding.isBleWorkDone) {
//            BLEProvisionLanding.bleTransport.disconnect();
//        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_settings, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            Intent intent = new Intent(this, SettingsActivity.class);
            startActivity(intent);
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_LOCATION) {

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {

                if (isLocationEnabled()) {
                    askForDeviceType();
                }
            }
        }

        if (requestCode == REQUEST_ENABLE_BT && resultCode == RESULT_OK) {
            Toast.makeText(this, "Bluetooth is turned ON, you can provision device now.", Toast.LENGTH_LONG).show();
        }
    }

    private void initViews() {

        btnAddDevice = findViewById(R.id.btn_provision_device);
        btnScanQRCode = findViewById(R.id.btn_scan_qr_code);

        if (BuildConfig.IS_QR_CODE_SUPPORTED) {

            TextView tvQrCodeBtn = btnScanQRCode.findViewById(R.id.text_btn);
            tvQrCodeBtn.setText(R.string.btn_scan_qr_code);
        } else {
            btnScanQRCode.setVisibility(View.GONE);
        }

        btnScanQRCode.findViewById(R.id.iv_arrow).setVisibility(View.GONE);
        btnAddDevice.findViewById(R.id.iv_arrow).setVisibility(View.GONE);
        btnScanQRCode.setOnClickListener(scanQrCodeBtnClickListener);
        btnAddDevice.setOnClickListener(addDeviceBtnClickListener);
    }

    View.OnClickListener scanQrCodeBtnClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {

                if (!isLocationEnabled()) {
                    askForLocation();
                    return;
                }
            }

            deviceType = sharedPreferences.getString(AppConstants.KEY_DEVICE_TYPES, AppConstants.DEVICE_TYPE_BOTH);

            if (deviceType.equals(AppConstants.DEVICE_TYPE_BLE) || deviceType.equals(AppConstants.DEVICE_TYPE_BOTH)) {

                final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
                BluetoothAdapter bleAdapter = bluetoothManager.getAdapter();

                if (!bleAdapter.isEnabled()) {
                    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
                } else {
                    Intent intent = new Intent(EspMainActivity.this, AddDeviceActivity.class);
                    startActivity(intent);
                }
            } else {
                Intent intent = new Intent(EspMainActivity.this, AddDeviceActivity.class);
                startActivity(intent);
            }
        }
    };

    View.OnClickListener addDeviceBtnClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {

                if (!isLocationEnabled()) {
                    askForLocation();
                    return;
                }
            }

            deviceType = sharedPreferences.getString(AppConstants.KEY_DEVICE_TYPES, AppConstants.DEVICE_TYPE_BOTH);

            if (deviceType.equals(AppConstants.DEVICE_TYPE_BLE) || deviceType.equals(AppConstants.DEVICE_TYPE_BOTH)) {

                final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
                BluetoothAdapter bleAdapter = bluetoothManager.getAdapter();

                if (!bleAdapter.isEnabled()) {
                    Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
                } else {
                    askForDeviceType();
                }
            } else {
                askForDeviceType();
            }
        }
    };

    private void askForDeviceType() {

        String deviceType = sharedPreferences.getString(AppConstants.KEY_DEVICE_TYPES, AppConstants.DEVICE_TYPE_BOTH);
        final boolean isSec1 = sharedPreferences.getBoolean("security_type", true);
        Log.d(TAG, "Device Types : " + deviceType);
        Log.d(TAG, "isSec1 : " + isSec1);
        int securityType = 0;
        if (isSec1) {
            securityType = 1;
        }

        if (deviceType.equals(AppConstants.DEVICE_TYPE_BLE)) {

            if (isSec1) {
                provisionManager.createESPDevice(ESPConstants.TransportType.TRANSPORT_BLE, ESPConstants.SecurityType.SECURITY_1);
            } else {
                provisionManager.createESPDevice(ESPConstants.TransportType.TRANSPORT_BLE, ESPConstants.SecurityType.SECURITY_0);
            }
            Intent intent = new Intent(EspMainActivity.this, BLEProvisionLanding.class);
            intent.putExtra("security_type", securityType);
            startActivity(intent);

        } else if (deviceType.equals(AppConstants.DEVICE_TYPE_WIFI)) {

            if (isSec1) {
                provisionManager.createESPDevice(ESPConstants.TransportType.TRANSPORT_SOFTAP, ESPConstants.SecurityType.SECURITY_1);
            } else {
                provisionManager.createESPDevice(ESPConstants.TransportType.TRANSPORT_SOFTAP, ESPConstants.SecurityType.SECURITY_0);
            }
//            Intent intent1 = new Intent(EspMainActivity.this, WiFiProvisionLanding.class);
            Intent intent1 = new Intent(EspMainActivity.this, ProvisionLanding.class);
            intent1.putExtra("security_type", securityType);
            startActivity(intent1);

        } else {

            final String[] deviceTypes = {"BLE", "Wi-Fi"};
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setCancelable(true);
            builder.setTitle(R.string.dialog_msg_device_selection);
            final int finalSecurityType = securityType;
            builder.setItems(deviceTypes, new DialogInterface.OnClickListener() {

                @Override
                public void onClick(DialogInterface dialog, int position) {

                    switch (position) {
                        case 0:
                            if (isSec1) {
                                provisionManager.createESPDevice(ESPConstants.TransportType.TRANSPORT_BLE, ESPConstants.SecurityType.SECURITY_1);
                            } else {
                                provisionManager.createESPDevice(ESPConstants.TransportType.TRANSPORT_BLE, ESPConstants.SecurityType.SECURITY_0);
                            }
                            Intent intent = new Intent(EspMainActivity.this, BLEProvisionLanding.class);
                            intent.putExtra("security_type", finalSecurityType);
                            startActivity(intent);
                            break;
                        case 1:
                            if (isSec1) {
                                provisionManager.createESPDevice(ESPConstants.TransportType.TRANSPORT_SOFTAP, ESPConstants.SecurityType.SECURITY_1);
                            } else {
                                provisionManager.createESPDevice(ESPConstants.TransportType.TRANSPORT_SOFTAP, ESPConstants.SecurityType.SECURITY_0);
                            }
//                            Intent intent1 = new Intent(EspMainActivity.this, WiFiProvisionLanding.class);
                            Intent intent1 = new Intent(EspMainActivity.this, ProvisionLanding.class);
                            intent1.putExtra("security_type", finalSecurityType);
                            startActivity(intent1);
                            break;
                    }
                    dialog.dismiss();
                }
            });
            builder.show();
        }
    }

    private void askForLocation() {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setCancelable(true);
        builder.setMessage(R.string.dialog_msg_gps);

        // Set up the buttons
        builder.setPositiveButton(R.string.btn_ok, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {

                startActivityForResult(new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS), REQUEST_LOCATION);
            }
        });

        builder.setNegativeButton(R.string.btn_cancel, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });

        builder.show();
    }

    private boolean isLocationEnabled() {

        boolean gps_enabled = false;
        boolean network_enabled = false;
        LocationManager lm = (LocationManager) getApplicationContext().getSystemService(Activity.LOCATION_SERVICE);

        try {
            gps_enabled = lm.isProviderEnabled(LocationManager.GPS_PROVIDER);
        } catch (Exception ex) {
        }

        try {
            network_enabled = lm.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
        } catch (Exception ex) {
        }

        Log.d(TAG, "GPS Enabled : " + gps_enabled + " , Network Enabled : " + network_enabled);

        boolean result = gps_enabled || network_enabled;
        return result;
    }
}
