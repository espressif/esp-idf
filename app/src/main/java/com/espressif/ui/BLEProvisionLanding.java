// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
package com.espressif.ui;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.espressif.provision.Provision;
import com.espressif.provision.R;
import com.espressif.provision.transport.BLETransport;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.UUID;

public class BLEProvisionLanding extends AppCompatActivity {
    private static final String TAG = "Espressif::" + BLEProvisionLanding.class.getSimpleName();

    private BluetoothAdapter bleAdapter;
    private BLETransport bleTransport;
    private BLETransport.BLETransportListener transportListener;
    private ArrayAdapter<String> adapter;
    private ArrayList<BluetoothDevice> bluetoothDevices;
    private ProgressBar progressBar;
    private boolean isDeviceConnected;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bleprovision_landing);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.connect_to_device_title);
        setSupportActionBar(toolbar);
        progressBar = findViewById(R.id.ble_landing_progress_indicator);
        progressBar.setVisibility(View.VISIBLE);

        isDeviceConnected = false;
        bluetoothDevices = new ArrayList<>();
        ArrayList<String> bleNames = new ArrayList<>();

        ListView listView = findViewById(R.id.ble_devices_list);
        adapter = new ArrayAdapter<>(this,
                android.R.layout.simple_list_item_1,
                android.R.id.text1,
                bleNames);

        // Assign adapter to ListView
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int pos, long l) {
                isDeviceConnected = false;
                progressBar.setVisibility(View.VISIBLE);
                bleTransport.connect(bluetoothDevices.get(pos));
            }
        });
        final Button rescanButton = findViewById(R.id.rescan_button);
        rescanButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                progressBar.setVisibility(View.VISIBLE);
                adapter.clear();
                bleTransport.scan(transportListener);
            }
        });

        final String serviceUUID = getIntent().getStringExtra(BLETransport.SERVICE_UUID_KEY);
        final String sessionUUID = getIntent().getStringExtra(BLETransport.SESSION_UUID_KEY);
        final String configUUID = getIntent().getStringExtra(BLETransport.CONFIG_UUID_KEY);
        final String deviceNamePrefix = getIntent().getStringExtra(BLETransport.DEVICE_NAME_PREFIX_KEY);
        final Activity thisActivity = this;

        transportListener = new BLETransport.BLETransportListener() {
            @Override
            public void onPeripheralsFound(ArrayList<BluetoothDevice> devices) {

                for(BluetoothDevice device:devices) {
                    bluetoothDevices.add(device);
                    adapter.add(device.getName());
                }
                progressBar.setVisibility(View.GONE);
            }

            @Override
            public void onPeripheralsNotFound() {
                progressBar.setVisibility(View.GONE);
                thisActivity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(thisActivity,
                                "No Bluetooth devices found!",
                                Toast.LENGTH_LONG)
                                .show();
                    }
                });
            }

            @Override
            public void onPeripheralConfigured(BluetoothDevice device) {
                bleDeviceConfigured(true);
            }

            @Override
            public void onPeripheralNotConfigured(BluetoothDevice device) {
                bleDeviceConfigured(false);
            }

            @Override
            public void onPeripheralDisconnected(Exception e) {
                thisActivity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(thisActivity,
                                "Bluetooth device disconnected.",
                                Toast.LENGTH_LONG)
                                .show();
                    }
                });
            }

            @Override
            public void onFailure(final Exception e) {
                thisActivity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(thisActivity,
                                "Bluetooth connection failed : " + e.getMessage(),
                                Toast.LENGTH_LONG)
                                .show();
                    }
                });
            }
        };

        boolean isBLEEnabled = this.checkBLEPermissions();
        if (!isBLEEnabled) {
            rescanButton.setEnabled(false);
            requestBluetoothEnable();
        } else {
            rescanButton.setEnabled(true);
            HashMap<String, String> configUUIDMap = new HashMap<>();
            configUUIDMap.put(Provision.PROVISIONING_CONFIG_PATH, configUUID);
            bleTransport = new BLETransport(this,
                    UUID.fromString(serviceUUID),
                    UUID.fromString(sessionUUID),
                    configUUIDMap,
                    deviceNamePrefix,
                    3000);
            bleTransport.scan(transportListener);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(isDeviceConnected) {
            bleTransport.disconnect();
            final TextView bleInstructions = findViewById(R.id.bluetooth_status_message);
            bleInstructions.setText(R.string.enable_bluetooth_instructions);
        }
    }

    private void bleDeviceConfigured(final Boolean isConfigured) {
        final Activity thisActivity = this;
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                progressBar.setVisibility(View.GONE);
                final TextView bleInstructions = findViewById(R.id.bluetooth_status_message);
                bleInstructions.setText(isConfigured ? R.string.enabled_bluetooth_action : R.string.enable_bluetooth_instructions);
                if(isConfigured) {
                    isDeviceConnected = true;
                    goToProvisionActivity();
                } else {
                    Toast.makeText(thisActivity,
                            "Bluetooth device could not be configured. Please try another device.",
                            Toast.LENGTH_LONG)
                            .show();
                }
            }
        });
    }

    private void goToProvisionActivity() {
        ProvisionActivity.BLE_TRANSPORT = bleTransport;
        Intent launchProvisionInstructions = new Intent(getApplicationContext(), ProvisionActivity.class);
        launchProvisionInstructions.putExtras(getIntent());
        startActivityForResult(launchProvisionInstructions, Provision.REQUEST_PROVISIONING_CODE);
    }

    private boolean checkBLEPermissions() {
        boolean isBLEEnabled = false;
        final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bleAdapter = bluetoothManager.getAdapter();
        if (bleAdapter == null || !bleAdapter.isEnabled()) {
            isBLEEnabled = false;
        } else if (!hasLocationPermissions()) {
            requestLocationPermission();
        } else {
            isBLEEnabled = true;
        }

        return isBLEEnabled;
    }

    private void requestBluetoothEnable() {
        Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(enableBtIntent, Provision.REQUEST_ENABLE_BLE_CODE);
    }

    private boolean hasLocationPermissions() {
        return ContextCompat.checkSelfPermission(this,
                Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestLocationPermission() {
        ActivityCompat.requestPermissions(this,
                new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, 102);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == Provision.REQUEST_PROVISIONING_CODE &&
                resultCode == RESULT_OK) {
            setResult(resultCode);
            finish();
        } else if (requestCode == Provision.REQUEST_ENABLE_BLE_CODE &&
                resultCode == RESULT_OK) {
            this.checkBLEPermissions();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case Provision.REQUEST_PERMISSIONS_CODE: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    checkBLEPermissions();
                }
            }
            break;
        }
    }
}
