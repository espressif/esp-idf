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
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
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
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;

import com.espressif.AppConstants;
import com.espressif.provisioning.DeviceConnectionEvent;
import com.espressif.provisioning.ESPConstants;
import com.espressif.provisioning.listeners.BleScanListener;
import com.espressif.ui.adapters.BleDeviceListAdapter;
import com.espressif.ui.models.BleDevice;
import com.espressif.ui.utils.Utils;
import com.espressif.wifi_provisioning.BuildConfig;
import com.espressif.wifi_provisioning.R;

import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.HashMap;

public class BLEProvisionLanding extends ManualProvBaseActivity {

    private static final String TAG = BLEProvisionLanding.class.getSimpleName();

    // Request codes
    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_FINE_LOCATION = 2;

    // Time out
    private static final long DEVICE_CONNECT_TIMEOUT = 20000;

//    public static boolean isBleWorkDone = false;

    private Button btnScan, btnPrefix;
    private ListView listView;
    private TextView textPrefix;
    private ProgressBar progressBar;
    private RelativeLayout prefixLayout;

    private BleDeviceListAdapter adapter;
    private BluetoothAdapter bleAdapter;
    private ArrayList<BleDevice> deviceList;
    private HashMap<BluetoothDevice, String> bluetoothDevices;
    private Handler handler;

    private int position = -1;
    private String deviceNamePrefix;
    private boolean isDeviceConnected = false, isConnecting = false;
    private boolean isScanning = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bleprovision_landing);
        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.title_activity_connect_device);
        setSupportActionBar(toolbar);

        // Use this check to determine whether BLE is supported on the device.  Then you can
        // selectively disable BLE-related features.
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, R.string.error_ble_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }

        final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bleAdapter = bluetoothManager.getAdapter();

        // Checks if Bluetooth is supported on the device.
        if (bleAdapter == null) {
            Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        isConnecting = false;
        isDeviceConnected = false;
        handler = new Handler();
        bluetoothDevices = new HashMap<>();
        deviceList = new ArrayList<>();
        deviceNamePrefix = sharedPreferences.getString(AppConstants.KEY_BLE_DEVICE_NAME_PREFIX,
                getResources().getString(R.string.ble_device_name_prefix));
        initViews();
    }

    @Override
    protected void onResume() {
        super.onResume();

        // Ensures Bluetooth is enabled on the device.  If Bluetooth is not currently enabled,
        // fire an intent to display a dialog asking the user to grant permission to enable it.
        if (!bleAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                if (ActivityCompat.checkSelfPermission(BLEProvisionLanding.this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
                } else {
                    Log.e(TAG, "BLUETOOTH_CONNECT permission is not granted.");
                    return;
                }
            } else {
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            }

        } else {

            if (!isDeviceConnected && !isConnecting) {
                startScan();
            }
        }
    }

    @Override
    public void onBackPressed() {

        if (isScanning) {
            stopScan();
        }
        super.onBackPressed();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        super.onActivityResult(requestCode, resultCode, data);
        Log.d(TAG, "onActivityResult, requestCode : " + requestCode + ", resultCode : " + resultCode);

        // User chose not to enable Bluetooth.
        if (requestCode == REQUEST_ENABLE_BT && resultCode == Activity.RESULT_CANCELED) {
            finish();
            return;
        }

        if (requestCode == REQUEST_ENABLE_BT && resultCode == RESULT_OK) {
            startScan();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

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
                Log.d(TAG, "Device Connected Event Received");
                progressBar.setVisibility(View.GONE);
                isConnecting = false;
                isDeviceConnected = true;
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

            case ESPConstants.EVENT_DEVICE_DISCONNECTED:

                progressBar.setVisibility(View.GONE);
                isConnecting = false;
                isDeviceConnected = false;
                Toast.makeText(BLEProvisionLanding.this, "Device disconnected", Toast.LENGTH_LONG).show();
                break;

            case ESPConstants.EVENT_DEVICE_CONNECTION_FAILED:
                progressBar.setVisibility(View.GONE);
                isConnecting = false;
                isDeviceConnected = false;
                Utils.displayDeviceConnectionError(this, getString(R.string.error_device_connect_failed));
                break;
        }
    }

    private void initViews() {

        btnScan = findViewById(R.id.btn_scan);
        btnPrefix = findViewById(R.id.btn_change_prefix);
        listView = findViewById(R.id.ble_devices_list);
        textPrefix = findViewById(R.id.prefix_value);
        progressBar = findViewById(R.id.ble_landing_progress_indicator);
        prefixLayout = findViewById(R.id.prefix_layout);
        textPrefix.setText(deviceNamePrefix);

        // Set visibility of Prefix layout
        if (BuildConfig.isFilteringByPrefixAllowed) {

            prefixLayout.setVisibility(View.VISIBLE);

        } else {
            prefixLayout.setVisibility(View.GONE);
        }

        adapter = new BleDeviceListAdapter(this, R.layout.item_ble_scan, deviceList);

        // Assign adapter to ListView
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(onDeviceCLickListener);
        btnScan.setOnClickListener(btnScanClickListener);
        btnPrefix.setOnClickListener(btnPrefixChangeClickListener);
    }

    private boolean hasPermissions() {

        if (bleAdapter == null || !bleAdapter.isEnabled()) {

            requestBluetoothEnable();
            return false;

        } else if (!hasLocationAndBtPermissions()) {

            requestLocationAndBtPermission();
            return false;
        }
        return true;
    }

    private void requestBluetoothEnable() {
        Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            } else {
                requestLocationAndBtPermission();
            }
        } else {
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
    }

    private boolean hasLocationAndBtPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            boolean permissionsGranted = checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED
                    && checkSelfPermission(Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_GRANTED
                    && checkSelfPermission(Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED;
            return permissionsGranted;
        } else {
            return checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
        }
    }

    private void requestLocationAndBtPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.BLUETOOTH_SCAN,
                    Manifest.permission.BLUETOOTH_CONNECT}, REQUEST_FINE_LOCATION);
        } else {
            requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_FINE_LOCATION);
        }
    }

    private void processDeviceCapabilities() {
        ArrayList<String> deviceCaps = provisionManager.getEspDevice().getDeviceCapabilities();

        if (deviceCaps != null && !deviceCaps.contains("no_pop") && securityType != AppConstants.SEC_TYPE_0) {

            goToPopActivity();

        } else if (deviceCaps.contains("wifi_scan")) {

            goToWifiScanListActivity();

        } else {

            goToWiFiConfigActivity();
        }
    }

    private void startScan() {

        if (!hasPermissions() || isScanning) {
            return;
        }

        isScanning = true;
        deviceList.clear();
        bluetoothDevices.clear();

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED) {
            provisionManager.searchBleEspDevices(deviceNamePrefix, bleScanListener);
            updateProgressAndScanBtn();
        } else {
            Log.e(TAG, "Not able to start scan as Location permission is not granted.");
            Toast.makeText(BLEProvisionLanding.this, "Please give location permission to start BLE scan", Toast.LENGTH_LONG).show();
        }
    }

    private void stopScan() {

        isScanning = false;

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED) {
            provisionManager.stopBleScan();
            updateProgressAndScanBtn();
        } else {
            Log.e(TAG, "Not able to stop scan as Location permission is not granted.");
            Toast.makeText(BLEProvisionLanding.this, "Please give location permission to stop BLE scan", Toast.LENGTH_LONG).show();
        }

        if (deviceList.size() <= 0) {
            Toast.makeText(BLEProvisionLanding.this, R.string.error_no_ble_device, Toast.LENGTH_SHORT).show();
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
            progressBar.setVisibility(View.VISIBLE);
            listView.setVisibility(View.GONE);

        } else {

            btnScan.setEnabled(true);
            btnScan.setAlpha(1f);
            progressBar.setVisibility(View.GONE);
            listView.setVisibility(View.VISIBLE);
        }
    }

    private View.OnClickListener btnScanClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            bluetoothDevices.clear();
            adapter.clear();
            startScan();
        }
    };

    private View.OnClickListener btnPrefixChangeClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            askForPrefix();
        }
    };

    private BleScanListener bleScanListener = new BleScanListener() {

        @Override
        public void scanStartFailed() {
            Toast.makeText(BLEProvisionLanding.this, "Please turn on Bluetooth to connect BLE device", Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onPeripheralFound(BluetoothDevice device, ScanResult scanResult) {

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                if (ActivityCompat.checkSelfPermission(BLEProvisionLanding.this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                    Log.d(TAG, "====== onPeripheralFound ===== " + device.getName());
                }
            } else {
                Log.d(TAG, "====== onPeripheralFound ===== " + device.getName());
            }

            boolean deviceExists = false;
            String serviceUuid = "";

            if (scanResult.getScanRecord().getServiceUuids() != null && scanResult.getScanRecord().getServiceUuids().size() > 0) {
                serviceUuid = scanResult.getScanRecord().getServiceUuids().get(0).toString();
            }
            Log.d(TAG, "Add service UUID : " + serviceUuid);

            if (bluetoothDevices.containsKey(device)) {
                deviceExists = true;
            }

            if (!deviceExists) {
                BleDevice bleDevice = new BleDevice();
                bleDevice.setName(scanResult.getScanRecord().getDeviceName());
                bleDevice.setBluetoothDevice(device);

                listView.setVisibility(View.VISIBLE);
                bluetoothDevices.put(device, serviceUuid);
                deviceList.add(bleDevice);
                adapter.notifyDataSetChanged();
            }
        }

        @Override
        public void scanCompleted() {
            isScanning = false;
            updateProgressAndScanBtn();
        }

        @Override
        public void onFailure(Exception e) {
            Log.e(TAG, e.getMessage());
            e.printStackTrace();
        }
    };

    private AdapterView.OnItemClickListener onDeviceCLickListener = new AdapterView.OnItemClickListener() {

        @Override
        public void onItemClick(AdapterView<?> adapterView, View view, int position, long l) {

            stopScan();
            isConnecting = true;
            isDeviceConnected = false;
            btnScan.setVisibility(View.GONE);
            listView.setVisibility(View.GONE);
            progressBar.setVisibility(View.VISIBLE);
            BLEProvisionLanding.this.position = position;
            BleDevice bleDevice = adapter.getItem(position);
            String uuid = bluetoothDevices.get(bleDevice.getBluetoothDevice());
            Log.d(TAG, "=================== Connect to device : " + bleDevice.getName() + " UUID : " + uuid);

            if (ActivityCompat.checkSelfPermission(BLEProvisionLanding.this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED) {
                provisionManager.getEspDevice().connectBLEDevice(bleDevice.getBluetoothDevice(), uuid);
                handler.postDelayed(disconnectDeviceTask, DEVICE_CONNECT_TIMEOUT);
            } else {
                Log.e(TAG, "Not able to connect device as Location permission is not granted.");
                Toast.makeText(BLEProvisionLanding.this, "Please give location permission to connect device", Toast.LENGTH_LONG).show();
            }
        }
    };

    private Runnable disconnectDeviceTask = new Runnable() {

        @Override
        public void run() {

            Log.e(TAG, "Disconnect device");
            progressBar.setVisibility(View.GONE);
            Utils.displayDeviceConnectionError(BLEProvisionLanding.this, getString(R.string.error_device_not_supported));
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
        popIntent.putExtra(AppConstants.KEY_DEVICE_NAME, deviceList.get(position).getName());
        startActivity(popIntent);
    }

    private void goToWifiScanListActivity() {

        finish();
        Intent wifiListIntent = new Intent(getApplicationContext(), WiFiScanActivity.class);
        wifiListIntent.putExtra(AppConstants.KEY_DEVICE_NAME, deviceList.get(position).getName());
        startActivity(wifiListIntent);
    }

    private void goToWiFiConfigActivity() {

        finish();
        Intent wifiConfigIntent = new Intent(getApplicationContext(), WiFiConfigActivity.class);
        startActivity(wifiConfigIntent);
    }
}
