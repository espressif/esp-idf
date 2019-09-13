package com.espressif.ble_scanner;

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

import java.util.ArrayList;
import java.util.List;

public class BleScanner {

    private static final String TAG = BleScanner.class.getSimpleName();

    private Context context;
    private BleScanListener bleScanListener;
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner bluetoothLeScanner;

    private long scanTimeout;
    private boolean isScanning = false;

    public BleScanner(Context context, long scanTimeoutInMillis, BleScanListener bleScannerListener) {

        this.context = context;
        this.scanTimeout = scanTimeoutInMillis;
        this.bleScanListener = bleScannerListener;

        final BluetoothManager bluetoothManager = (BluetoothManager) context.getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();
    }

    public void startScan() {

        Log.d(TAG, "startScan");

        bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();
        List<ScanFilter> filters = new ArrayList<>();
        ScanSettings settings = new ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_LOW_POWER)
                .build();

        isScanning = true;
        bluetoothLeScanner.startScan(filters, settings, scanCallback);
        Handler someHandler = new Handler();
        someHandler.postDelayed(new Runnable() {

            @Override
            public void run() {

                stopScan();
            }
        }, scanTimeout);
    }

    public void stopScan() {

        Log.d(TAG, "onStopBleScan()");

        if (bluetoothLeScanner != null && bluetoothAdapter != null && bluetoothAdapter.isEnabled()) {
            try {
                bluetoothLeScanner.stopScan(scanCallback);
            } catch (Exception e) {
                Log.e(TAG, e.toString());
            }
        }
        isScanning = false;
        bleScanListener.scanCompleted();
    }

    public boolean isScanning() {
        return isScanning;
    }

    final ScanCallback scanCallback = new ScanCallback() {

        @Override
        public void onScanResult(int callbackType, ScanResult result) {

            if (result.getDevice() != null && !TextUtils.isEmpty(result.getDevice().getName())) {

                Log.d(TAG, "onScanResult");
                bleScanListener.onPeripheralFound(result.getDevice(), result);
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
            Log.d(TAG, "onScanFailed errorCode:" + errorCode);
            bleScanListener.onFailure(new Exception("BLE connect failed with error code : " + errorCode));
        }
    };
}
