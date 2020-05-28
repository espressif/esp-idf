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

package com.espressif.provisioning.device_scanner;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.text.TextUtils;
import android.util.Log;

import androidx.annotation.RequiresPermission;

import com.espressif.provisioning.ESPConstants;
import com.espressif.provisioning.WiFiAccessPoint;
import com.espressif.provisioning.listeners.WiFiScanListener;

import java.util.ArrayList;
import java.util.List;

/**
 * This class is used for Wi-Fi scan functionality.
 */
public class WiFiScanner {

    private static final String TAG = "ESP:" + WiFiScanner.class.getSimpleName();

    private Context context;
    private WiFiScanListener wiFiScanListener;
    private WifiManager wifiManager;
    private ArrayList<WiFiAccessPoint> results;

    private boolean isScanning = false;
    private String prefix;

    @RequiresPermission(Manifest.permission.CHANGE_WIFI_STATE)
    public WiFiScanner(Context context, WiFiScanListener wiFiScanListener) {

        this.context = context;
        this.wiFiScanListener = wiFiScanListener;
        results = new ArrayList<>();
        wifiManager = (WifiManager) context.getApplicationContext().getSystemService(Context.WIFI_SERVICE);

        if (!wifiManager.isWifiEnabled()) {

            wifiManager.setWifiEnabled(true);
        }
    }

    @RequiresPermission(Manifest.permission.CHANGE_WIFI_STATE)
    public WiFiScanner(Context context, String prefix, WiFiScanListener wiFiScanListener) {

        this(context, wiFiScanListener);
        this.prefix = prefix;
    }

    @RequiresPermission(allOf = {Manifest.permission.CHANGE_WIFI_STATE, Manifest.permission.ACCESS_WIFI_STATE})
    public void startScan() {

        Log.d(TAG, "Starting Wi-Fi device scanning...");
        isScanning = true;
        results.clear();
        context.registerReceiver(wifiReceiver, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));
        wifiManager.startScan();
    }

    /**
     * This method is used to check currently scanning is going on or not.
     *
     * @return Returns tru if scanning is going on.
     */
    public boolean isScanning() {
        return isScanning;
    }

    private BroadcastReceiver wifiReceiver = new BroadcastReceiver() {

        @Override
        @RequiresPermission(Manifest.permission.ACCESS_WIFI_STATE)
        public void onReceive(Context context, Intent intent) {

            List<ScanResult> scanResults = wifiManager.getScanResults();
            context.unregisterReceiver(this);

            for (ScanResult scanResult : scanResults) {

                Log.d(TAG, "========== Device Found : " + scanResult.SSID + " - " + scanResult.capabilities);
                String deviceName = scanResult.SSID;
                WiFiAccessPoint wiFiAccessPoint = new WiFiAccessPoint();
                wiFiAccessPoint.setWifiName(scanResult.SSID);
                wiFiAccessPoint.setRssi(scanResult.level);

                if (scanResult.capabilities.contains("WPA") || scanResult.capabilities.contains("WEP")) {
                    wiFiAccessPoint.setSecurity(ESPConstants.WIFI_WEP);
                } else {
                    wiFiAccessPoint.setSecurity(ESPConstants.WIFI_OPEN);
                }

                if (TextUtils.isEmpty(prefix)) {

                    results.add(wiFiAccessPoint);

                } else if (deviceName.startsWith(prefix)) {

                    results.add(wiFiAccessPoint);
                }

            }
            isScanning = false;
            wiFiScanListener.onWifiListReceived(results);
        }
    };
}
