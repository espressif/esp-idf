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

package com.espressif.provisioning;

/**
 * This class has constants and enums used in library.
 */
public class ESPConstants {

    public static final String DEFAULT_WIFI_BASE_URL = "192.168.4.1:80";

    public enum TransportType {
        TRANSPORT_BLE,
        TRANSPORT_SOFTAP
    }

    public enum SecurityType {
        SECURITY_0,
        SECURITY_1,
        SECURITY_2
    }

    public enum ProvisionFailureReason {
        AUTH_FAILED,
        NETWORK_NOT_FOUND,
        DEVICE_DISCONNECTED,
        UNKNOWN
    }

    // End point names
    public static final String HANDLER_PROV_SCAN = "prov-scan";
    public static final String HANDLER_PROTO_VER = "proto-ver";
    public static final String HANDLER_PROV_SESSION = "prov-session";
    public static final String HANDLER_PROV_CONFIG = "prov-config";

    // Event types
    public static final short EVENT_DEVICE_CONNECTED = 1;
    public static final short EVENT_DEVICE_CONNECTION_FAILED = 2;
    public static final short EVENT_DEVICE_DISCONNECTED = 3;

    // Constants for WiFi Security values (As per proto files)
    public static final short WIFI_OPEN = 0;
    public static final short WIFI_WEP = 1;
    public static final short WIFI_WPA_PSK = 2;
    public static final short WIFI_WPA2_PSK = 3;
    public static final short WIFI_WPA_WPA2_PSK = 4;
    public static final short WIFI_WPA2_ENTERPRISE = 5;
    public static final short WIFI_WPA3_PSK = 6;
    public static final short WIFI_WPA2_WPA3_PSK = 7;
}
