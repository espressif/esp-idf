// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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
import android.content.IntentSender;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.IntentSenderRequest;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.cardview.widget.CardView;

import com.espressif.AppConstants;
import com.espressif.provisioning.DeviceConnectionEvent;
import com.espressif.provisioning.ESPConstants;
import com.espressif.provisioning.ESPProvisionManager;
import com.espressif.provisioning.WiFiAccessPoint;
import com.espressif.provisioning.listeners.WiFiScanListener;
import com.espressif.ui.utils.Utils;
import com.espressif.wifi_provisioning.R;
import com.google.android.gms.threadnetwork.ThreadNetwork;
import com.google.android.gms.threadnetwork.ThreadNetworkCredentials;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;

public class ThreadConfigActivity extends AppCompatActivity {

    private static final String TAG = ThreadConfigActivity.class.getSimpleName();

    private Handler handler;
    private ProgressBar progressBar;
    private CardView btnNext;
    private TextView txtNextBtn, tvProgress, tvError;
    private ArrayList<WiFiAccessPoint> threadNetworkList;
    private ESPProvisionManager provisionManager;

    private ActivityResultLauncher<IntentSenderRequest> preferredCredentialsLauncher;
    private ThreadNetworkCredentials preferredCredentials;
    private boolean scanCapAvailable = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_thread_scan_list);

        handler = new Handler();
        threadNetworkList = new ArrayList<>();
        provisionManager = ESPProvisionManager.getInstance(getApplicationContext());
        scanCapAvailable = getIntent().getBooleanExtra(AppConstants.KEY_THREAD_SCAN_AVAILABLE, false);

        initViews();
        EventBus.getDefault().register(this);
        getThreadPreferredCredentials();
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

        if (event.getEventType() == ESPConstants.EVENT_DEVICE_DISCONNECTED) {
            if (!isFinishing()) {
                showAlertForDeviceDisconnected();
            }
        }
    }

    private View.OnClickListener nextBtnClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            String btnText = txtNextBtn.getText().toString();

            if (btnText.equals(getString(R.string.btn_next))) {

                showLoading(getString(R.string.progress_thread_networks));
                byte[] activeDataset = preferredCredentials.getActiveOperationalDataset();
                sendActiveDataset(Utils.byteArrayToDs(activeDataset));

            } else if (btnText.equals(getString(R.string.btn_try_again))) {

                showLoading(getString(R.string.progress_thread_networks));
                getThreadPreferredCredentials();

            } else if (btnText.equals(getString(R.string.btn_ok))) {
                finish();
            }
        }
    };

    private void initViews() {
        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.title_activity_thread_config);
        setSupportActionBar(toolbar);

        tvProgress = findViewById(R.id.tv_thread_message);
        tvError = findViewById(R.id.tv_prov_error);
        progressBar = findViewById(R.id.network_search_loading);
        progressBar.setVisibility(View.VISIBLE);

        btnNext = findViewById(R.id.btn_next);
        txtNextBtn = findViewById(R.id.text_btn);
        txtNextBtn.setText(R.string.btn_next);
        btnNext.setEnabled(false);
        btnNext.setAlpha(0.5f);
        btnNext.setOnClickListener(nextBtnClickListener);

        preferredCredentialsLauncher = registerForActivityResult(new ActivityResultContracts.StartIntentSenderForResult(),
                result -> {
                    if (result.getResultCode() == RESULT_OK) {

                        preferredCredentials = ThreadNetworkCredentials.fromIntentSenderResultData(result.getData());
                        Log.d(TAG, "Preferred Credentials Network Name : " + preferredCredentials.getNetworkName());

                        if (scanCapAvailable) {
                            startThreadScan();
                        } else {
                            // If "thread_scan" capability is not available and "thread_prov" is available
                            hideLoading();
                            txtNextBtn.setText(R.string.btn_next);
                            String str = "Available Thread Network : " + preferredCredentials.getNetworkName() + "\n"
                                    + "Do you want to proceed ?";
                            tvProgress.setText(str);
                        }

                    } else {
                        // The user denied to share!
                        Log.e(TAG, "User denied request.");
                        hideLoading();
                        showError(getString(R.string.error_title), getString(R.string.error_read_preferred_creds_request_denied), true);
                    }
                });
    }

    private void getThreadPreferredCredentials() {

        Log.d(TAG, "ThreadClient: getPreferredCredentials intent sent");
        ThreadNetwork.getClient(this)
                .getPreferredCredentials()
                .addOnSuccessListener(intentSenderResult -> {
                    IntentSender intentSender = intentSenderResult.getIntentSender();
                    if (intentSender != null) {
                        preferredCredentialsLauncher.launch(
                                new IntentSenderRequest.Builder(intentSender).build());
                    } else {
                        // No preferred credentials found!
                        Log.e(TAG, "No preferred credentials found!");
                        hideLoading();
                        showError(getString(R.string.error_title), getString(R.string.error_no_preferred_creds), false);
                    }
                })
                .addOnFailureListener(e -> {
                    e.printStackTrace();
                    hideLoading();
                    showError(getString(R.string.error_title), getString(R.string.error_no_preferred_creds), false);
                });
    }

    private void startThreadScan() {

        Log.d(TAG, "Start Thread Scan");
        threadNetworkList.clear();

        runOnUiThread(new Runnable() {

            @Override
            public void run() {
                showLoading(getString(R.string.progress_thread_networks));
            }
        });

        handler.postDelayed(stopScanningTask, 15000);

        provisionManager.getEspDevice().scanThreadNetworks(new WiFiScanListener() {

            @Override
            public void onWifiListReceived(final ArrayList<WiFiAccessPoint> wifiList) {

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {

                        threadNetworkList.addAll(wifiList);
                        handler.removeCallbacks(stopScanningTask);
                        boolean isNetworkAvailable = false;

                        if (!threadNetworkList.isEmpty()) {

                            for (WiFiAccessPoint network : threadNetworkList) {

                                if (preferredCredentials.getNetworkName().equals(network.getWifiName())) {

                                    Log.d(TAG, "Thread Network available : " + network.getWifiName());
                                    isNetworkAvailable = true;
                                    break;
                                }
                            }

                            if (isNetworkAvailable) {

                                hideLoading();
                                txtNextBtn.setText(R.string.btn_next);
                                String str = "Available Thread Network : " + preferredCredentials.getNetworkName() + "\n"
                                        + "Do you want to proceed ?";
                                tvProgress.setText(str);

                            } else {
                                hideLoading();
                                showError(getString(R.string.error_title), getString(R.string.error_no_thread_network), false);
                            }
                        } else {
                            hideLoading();
                            showError(getString(R.string.error_title), getString(R.string.error_no_thread_network), false);
                        }
                    }
                });
            }

            @Override
            public void onWiFiScanFailed(Exception e) {

                Log.e(TAG, "onWiFiScanFailed");
                e.printStackTrace();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        hideLoading();
                        showError(getString(R.string.error_title), "Failed to get thread scan list", false);
                    }
                });
            }
        });
    }

    private void sendActiveDataset(String activeDataset) {

        hideLoading();
        goToProvisioningActivity(activeDataset);
    }

    private void goToProvisioningActivity(String activeDataset) {

        finish();
        Intent provisionIntent = new Intent(getApplicationContext(), ProvisionActivity.class);
        provisionIntent.putExtras(getIntent());
        provisionIntent.putExtra(AppConstants.KEY_THREAD_DATASET, activeDataset);
        startActivity(provisionIntent);
    }

    private Runnable stopScanningTask = new Runnable() {

        @Override
        public void run() {
            hideLoading();
        }
    };

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

    private void showLoading(String msg) {
        btnNext.setEnabled(false);
        btnNext.setAlpha(0.5f);
        tvProgress.setText(msg);
        progressBar.setVisibility(View.VISIBLE);
        tvError.setVisibility(View.GONE);
    }

    private void hideLoading() {
        btnNext.setEnabled(true);
        btnNext.setAlpha(1f);
        progressBar.setVisibility(View.GONE);
    }

    private void showError(String title, String msg, boolean canReadAgain) {

        findViewById(R.id.iv_arrow).setVisibility(View.GONE);

        if (canReadAgain) {
            txtNextBtn.setText(R.string.btn_try_again);
        } else {
            txtNextBtn.setText(R.string.btn_ok);
        }

        tvProgress.setText(title);
        tvError.setText(msg);
        tvError.setVisibility(View.VISIBLE);
    }
}
