package com.espressif.ble_scanner;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.le.ScanResult;

/**
 * Interface for BLE device scanning.
 */
public interface BleScanListener {

    /**
     * Called when any BLE peripheral will be found.
     *
     * @param device     BluetoothDevice
     * @param scanResult Scan result.
     */
    void onPeripheralFound(BluetoothDevice device, ScanResult scanResult);

    /**
     * Callback method for scan completed.
     */
    void scanCompleted();

    /**
     * Failed to scan for BLE bluetoothDevices.
     *
     * @param e Exception
     */
    void onFailure(Exception e);
}
