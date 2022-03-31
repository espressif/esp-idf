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

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import com.espressif.AppConstants;
import com.espressif.provisioning.DeviceConnectionEvent;
import com.espressif.provisioning.ESPConstants;
import com.espressif.provisioning.ESPProvisionManager;
import com.espressif.provisioning.WiFiAccessPoint;
import com.espressif.provisioning.listeners.WiFiScanListener;
import com.espressif.ui.adapters.WiFiListAdapter;
import com.espressif.wifi_provisioning.R;
import com.google.android.material.textfield.TextInputLayout;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;

public class WiFiScanActivity extends AppCompatActivity {

    private static final String TAG = WiFiScanActivity.class.getSimpleName();

    private Handler handler;
    private ImageView ivRefresh;
    private ListView wifiListView;
    private ProgressBar progressBar;
    private WiFiListAdapter adapter;
    private ArrayList<WiFiAccessPoint> wifiAPList;
    private ESPProvisionManager provisionManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wifi_scan_list);
        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.title_activity_wifi_scan_list);
        setSupportActionBar(toolbar);

        ivRefresh = findViewById(R.id.btn_refresh);
        wifiListView = findViewById(R.id.wifi_ap_list);
        progressBar = findViewById(R.id.wifi_progress_indicator);

        progressBar.setVisibility(View.VISIBLE);

        wifiAPList = new ArrayList<>();
        handler = new Handler();
        provisionManager = ESPProvisionManager.getInstance(getApplicationContext());

        String deviceName = provisionManager.getEspDevice().getDeviceName();
        String wifiMsg = String.format(getString(R.string.setup_instructions), deviceName);
        TextView tvWifiMsg = findViewById(R.id.wifi_message);
        tvWifiMsg.setText(wifiMsg);

        ivRefresh.setOnClickListener(refreshClickListener);
        adapter = new WiFiListAdapter(this, R.id.tv_wifi_name, wifiAPList);

        // Assign adapter to ListView
        wifiListView.setAdapter(adapter);
        wifiListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int pos, long l) {

                Log.d(TAG, "Device to be connected -" + wifiAPList.get(pos));
                String ssid = wifiAPList.get(pos).getWifiName();

                if (ssid.equals(getString(R.string.join_other_network))) {
                    askForNetwork(wifiAPList.get(pos).getWifiName(), wifiAPList.get(pos).getSecurity());
                } else if (wifiAPList.get(pos).getSecurity() == ESPConstants.WIFI_OPEN) {
                    goForProvisioning(wifiAPList.get(pos).getWifiName(), "");
                } else {
                    askForNetwork(wifiAPList.get(pos).getWifiName(), wifiAPList.get(pos).getSecurity());
                }
            }
        });

        wifiListView.addOnLayoutChangeListener(new View.OnLayoutChangeListener() {

            @Override
            public void onLayoutChange(View v, int left, int top, int right, int bottom, int oldLeft, int oldTop, int oldRight, int oldBottom) {
            }
        });

        EventBus.getDefault().register(this);
        startWifiScan();
    }

    @Override
    protected void onDestroy() {
        EventBus.getDefault().unregister(this);
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        provisionManager.getEspDevice().disconnectDevice();
        super.onBackPressed();
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onEvent(DeviceConnectionEvent event) {

        Log.d(TAG, "On Device Connection Event RECEIVED : " + event.getEventType());

        switch (event.getEventType()) {

            case ESPConstants.EVENT_DEVICE_DISCONNECTED:
                if (!isFinishing()) {
                    showAlertForDeviceDisconnected();
                }
                break;
        }
    }

    private void startWifiScan() {

        Log.d(TAG, "Start Wi-Fi Scan");
        wifiAPList.clear();

        runOnUiThread(new Runnable() {

            @Override
            public void run() {
                updateProgressAndScanBtn(true);
            }
        });

        handler.postDelayed(stopScanningTask, 15000);

        provisionManager.getEspDevice().scanNetworks(new WiFiScanListener() {

            @Override
            public void onWifiListReceived(final ArrayList<WiFiAccessPoint> wifiList) {

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        wifiAPList.addAll(wifiList);
                        completeWifiList();
                    }
                });
            }

            @Override
            public void onWiFiScanFailed(Exception e) {

                // TODO
                Log.e(TAG, "onWiFiScanFailed");
                e.printStackTrace();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        updateProgressAndScanBtn(false);
                        Toast.makeText(WiFiScanActivity.this, "Failed to get Wi-Fi scan list", Toast.LENGTH_LONG).show();
                    }
                });
            }
        });
    }

    private void completeWifiList() {

        // Add "Join network" Option as a list item
        WiFiAccessPoint wifiAp = new WiFiAccessPoint();
        wifiAp.setWifiName(getString(R.string.join_other_network));
        wifiAPList.add(wifiAp);

        updateProgressAndScanBtn(false);
        handler.removeCallbacks(stopScanningTask);
    }

    private void askForNetwork(final String ssid, final int authMode) {

        LayoutInflater inflater = this.getLayoutInflater();
        final View dialogView = inflater.inflate(R.layout.dialog_wifi_network, null);
        final EditText etSsid = dialogView.findViewById(R.id.et_ssid);
        final EditText etPassword = dialogView.findViewById(R.id.et_password);

        String title = getString(R.string.join_other_network);
        if (!ssid.equals(title)) {
            title = ssid;
            etSsid.setVisibility(View.GONE);
        }

        final AlertDialog alertDialog = new AlertDialog.Builder(this)
                .setView(dialogView)
                .setTitle(title)
                .setPositiveButton(R.string.btn_provision, null)
                .setNegativeButton(R.string.btn_cancel, null)
                .create();

        alertDialog.setOnShowListener(new DialogInterface.OnShowListener() {
            @Override
            public void onShow(DialogInterface dialog) {

                Button buttonPositive = ((AlertDialog) dialog).getButton(DialogInterface.BUTTON_POSITIVE);
                buttonPositive.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        String password = etPassword.getText().toString();

                        if (ssid.equals(getString(R.string.join_other_network))) {

                            String networkName = etSsid.getText().toString();

                            if (TextUtils.isEmpty(networkName)) {
                                etSsid.setError(getString(R.string.error_ssid_empty));

                            } else {
                                dialog.dismiss();
                                goForProvisioning(networkName, password);
                            }

                        } else {

                            if (TextUtils.isEmpty(password)) {

                                if (authMode != ESPConstants.WIFI_OPEN) {
                                    TextInputLayout passwordLayout = dialogView.findViewById(R.id.layout_password);
                                    passwordLayout.setError(getString(R.string.error_password_empty));

                                } else {
                                    dialog.dismiss();
                                    goForProvisioning(ssid, password);
                                }

                            } else {

                                if (authMode == ESPConstants.WIFI_OPEN) {
                                    password = "";
                                }
                                dialog.dismiss();
                                goForProvisioning(ssid, password);
                            }
                        }
                    }
                });
                Button buttonNegative = ((AlertDialog) dialog).getButton(DialogInterface.BUTTON_NEGATIVE);
                buttonNegative.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        dialog.dismiss();
                    }
                });
            }
        });

        alertDialog.show();
    }

    private void goForProvisioning(String ssid, String password) {

        finish();
        Intent provisionIntent = new Intent(getApplicationContext(), ProvisionActivity.class);
        provisionIntent.putExtras(getIntent());
        provisionIntent.putExtra(AppConstants.KEY_WIFI_SSID, ssid);
        provisionIntent.putExtra(AppConstants.KEY_WIFI_PASSWORD, password);
        startActivity(provisionIntent);
    }

    private View.OnClickListener refreshClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            startWifiScan();
        }
    };

    private Runnable stopScanningTask = new Runnable() {

        @Override
        public void run() {

            updateProgressAndScanBtn(false);
        }
    };

    /**
     * This method will update UI (Scan button enable / disable and progressbar visibility)
     */
    private void updateProgressAndScanBtn(boolean isScanning) {

        if (isScanning) {

            progressBar.setVisibility(View.VISIBLE);
            wifiListView.setVisibility(View.GONE);
            ivRefresh.setVisibility(View.GONE);

        } else {

            progressBar.setVisibility(View.GONE);
            wifiListView.setVisibility(View.VISIBLE);
            ivRefresh.setVisibility(View.VISIBLE);
            adapter.notifyDataSetChanged();
        }
    }

    private void showAlertForDeviceDisconnected() {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setCancelable(false);
        builder.setTitle(R.string.error_title);
        builder.setMessage(R.string.dialog_msg_ble_device_disconnection);

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
}
