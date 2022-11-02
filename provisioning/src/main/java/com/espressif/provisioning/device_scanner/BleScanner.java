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
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.os.Handler;
import android.text.TextUtils;
import android.util.Log;

import androidx.annotation.RequiresPermission;

import com.espressif.provisioning.listeners.BleScanListener;

import java.util.ArrayList;
import java.util.List;

/**
 * This class is used for BLE scan functionality.
 */
public class BleScanner {

    private static final String TAG = "ESP:" + BleScanner.class.getSimpleName();

    private static final long SCAN_TIME_OUT = 6000;

    private Handler handler;
    private BleScanListener bleScanListener;
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner bluetoothLeScanner;

    private boolean isScanning = false;
    private String prefix = "";

    public BleScanner(Context context, BleScanListener bleScannerListener) {

        this.bleScanListener = bleScannerListener;
        handler = new Handler();

        BluetoothManager bluetoothManager = (BluetoothManager) context.getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();
    }

    public BleScanner(Context context, String prefix, BleScanListener bleScannerListener) {

        this(context, bleScannerListener);
        this.prefix = prefix;
    }

    /**
     * This method is used to start BLE scan.
     */
    @RequiresPermission(allOf = {Manifest.permission.BLUETOOTH, Manifest.permission.BLUETOOTH_ADMIN})
    public void startScan() {
        List<ScanFilter> filters = new ArrayList<>();
        ScanSettings settings = new ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_BALANCED)
                .build();
        startScan(filters, settings);
    }

    /**
     * This method is used to start BLE scan.
     *
     * @param filters The scan filters that will be used
     */
    @RequiresPermission(allOf = {Manifest.permission.BLUETOOTH, Manifest.permission.BLUETOOTH_ADMIN})
    public void startScan(List<ScanFilter> filters) {
        ScanSettings settings = new ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_BALANCED)
                .build();
        startScan(filters, settings);
    }

    /**
     * This method is used to start BLE scan.
     *
     * @param scanSettings The scan settings that will be used
     */
    @RequiresPermission(allOf = {Manifest.permission.BLUETOOTH, Manifest.permission.BLUETOOTH_ADMIN})
    public void startScan(ScanSettings scanSettings) {
        List<ScanFilter> filters = new ArrayList<>();
        startScan(filters, scanSettings);
    }

    /**
     * This method is used to start BLE scan.
     *
     * @param filters      The scan filters that will be used
     * @param scanSettings The scan settings that will be used
     */
    @RequiresPermission(allOf = {Manifest.permission.BLUETOOTH, Manifest.permission.BLUETOOTH_ADMIN})
    public void startScan(List<ScanFilter> filters, ScanSettings scanSettings) {

        if (!bluetoothAdapter.isEnabled()) {
            bleScanListener.scanStartFailed();
            return;
        }
        Log.d(TAG, "Starting BLE device scanning...");

        bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();
        isScanning = true;
        bluetoothLeScanner.startScan(filters, scanSettings, scanCallback);
        handler.postDelayed(stopScanTask, SCAN_TIME_OUT);
    }

    /**
     * This method is used to start BLE scan.
     */
    @RequiresPermission(allOf = {Manifest.permission.BLUETOOTH_ADMIN, Manifest.permission.BLUETOOTH})
    public void stopScan() {

        Log.d(TAG, "Stop BLE device scan");
        handler.removeCallbacks(stopScanTask);

        if (bluetoothLeScanner != null && bluetoothAdapter != null && bluetoothAdapter.isEnabled()) {
            try {
                bluetoothLeScanner.stopScan(scanCallback);
            } catch (Exception e) {
                Log.e(TAG, e.toString());
                e.printStackTrace();
            }
        }
        isScanning = false;
        bleScanListener.scanCompleted();
    }

    /**
     * This method is used to check currently scanning is going on or not.
     *
     * @return Returns tru if scanning is going on.
     */
    public boolean isScanning() {
        return isScanning;
    }

    private Runnable stopScanTask = new Runnable() {

        @Override
        @RequiresPermission(allOf = {Manifest.permission.BLUETOOTH_ADMIN, Manifest.permission.BLUETOOTH})
        public void run() {
            stopScan();
        }
    };

    /**
     * ScanCallback to get scanned Peripheral.
     */
    private final ScanCallback scanCallback = new ScanCallback() {

        @Override
        @RequiresPermission(Manifest.permission.BLUETOOTH)
        public void onScanResult(int callbackType, ScanResult result) {

            String deviceName = result.getScanRecord().getDeviceName();

            if (result.getDevice() != null && !TextUtils.isEmpty(deviceName)) {
                // Found BLE device
                Log.d(TAG, "========== Device Found : " + deviceName);

                if (TextUtils.isEmpty(prefix)) {
                    bleScanListener.onPeripheralFound(result.getDevice(), result);
                } else if (deviceName.startsWith(prefix)) {
                    bleScanListener.onPeripheralFound(result.getDevice(), result);
                }
            }
        }

        @Override
        public void onBatchScanResults(List<ScanResult> results) {
            super.onBatchScanResults(results);
            Log.d(TAG, "onBatchScanResults()");
        }

        @Override
        public void onScanFailed(int errorCode) {
            super.onScanFailed(errorCode);
            Log.e(TAG, "onScanFailed, errorCode:" + errorCode);
            bleScanListener.onFailure(new RuntimeException("BLE scanning failed with error code : " + errorCode));
        }
    };
}
