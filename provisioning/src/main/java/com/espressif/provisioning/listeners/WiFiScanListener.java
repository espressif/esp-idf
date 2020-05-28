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

import com.espressif.provisioning.WiFiAccessPoint;

import java.util.ArrayList;

/**
 * Interface for Wi-Fi device scanning. Same interface is used to return list of Wi-Fi access points scanned by phone and device.
 */
public interface WiFiScanListener {

    /**
     * Callback method to return a list of Wi-Fi access points.
     *
     * @param wifiList ArrayList of Wi-Fi access points.
     */
    void onWifiListReceived(ArrayList<WiFiAccessPoint> wifiList);

    /**
     * Failed to scan for Wi-Fi devices.
     *
     * @param e Exception
     */
    void onWiFiScanFailed(Exception e);
}
