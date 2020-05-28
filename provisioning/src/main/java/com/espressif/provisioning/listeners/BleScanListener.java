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

package com.espressif.provisioning.listeners;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.le.ScanResult;

/**
 * Interface for BLE device scanning.
 */
public interface BleScanListener {

    /**
     * Callback to inform user BT is off so not able to start scanning.
     */
     void scanStartFailed();

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
     * Failed to scan for BLE Bluetooth Devices.
     *
     * @param e Exception
     */
    void onFailure(Exception e);
}
