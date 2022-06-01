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

import com.espressif.provisioning.ESPDevice;

/**
 * Interface for QR code scanning callbacks.
 */
public interface QRCodeScanListener {

    /**
     * Called when QR code is scanned so that app can display loading.
     */
    void qrCodeScanned();

    /**
     * Called when received device from QR code is available in scanning.
     *
     * @param espDevice
     */
    void deviceDetected(ESPDevice espDevice);

    /**
     * Failed to scan QR code or device not found in scanning.
     *
     * @param e Exception
     */
    void onFailure(Exception e);

    /**
     * Called when QR code data has different format.
     *
     * @param e    Exception
     * @param data QR code data string.
     */
    void onFailure(Exception e, String data);
}
