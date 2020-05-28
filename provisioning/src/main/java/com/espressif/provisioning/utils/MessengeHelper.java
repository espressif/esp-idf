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


package com.espressif.provisioning.utils;

import com.google.protobuf.ByteString;

import espressif.WifiConfig;
import espressif.WifiScan;

public class MessengeHelper {

    // Send Wi-Fi Scan command
    public static byte[] prepareWiFiScanMsg() {

        WifiScan.CmdScanStart configRequest = WifiScan.CmdScanStart.newBuilder()
                .setBlocking(true)
                .setPassive(false)
                .setGroupChannels(0)
                .setPeriodMs(120)
                .build();
        WifiScan.WiFiScanMsgType msgType = WifiScan.WiFiScanMsgType.TypeCmdScanStart;
        WifiScan.WiFiScanPayload payload = WifiScan.WiFiScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanStart(configRequest)
                .build();

        return payload.toByteArray();
    }

    public static byte[] prepareGetWiFiScanStatusMsg() {

        WifiScan.CmdScanStatus configRequest = WifiScan.CmdScanStatus.newBuilder()
                .build();
        WifiScan.WiFiScanMsgType msgType = WifiScan.WiFiScanMsgType.TypeCmdScanStatus;
        WifiScan.WiFiScanPayload payload = WifiScan.WiFiScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanStatus(configRequest)
                .build();
        return payload.toByteArray();
    }

    // Get Wi-Fi scan list
    public static byte[] prepareGetWiFiScanListMsg(int start, int count) {

        WifiScan.CmdScanResult configRequest = WifiScan.CmdScanResult.newBuilder()
                .setStartIndex(start)
                .setCount(count)
                .build();
        WifiScan.WiFiScanMsgType msgType = WifiScan.WiFiScanMsgType.TypeCmdScanResult;
        WifiScan.WiFiScanPayload payload = WifiScan.WiFiScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanResult(configRequest)
                .build();

        return payload.toByteArray();
    }

    // Send Wi-Fi Config
    public static byte[] prepareWiFiConfigMsg(String ssid, String passphrase) {

        WifiConfig.CmdSetConfig cmdSetConfig;

        if (passphrase != null) {

            cmdSetConfig = WifiConfig.CmdSetConfig
                    .newBuilder()
                    .setSsid(ByteString.copyFrom(ssid.getBytes()))
                    .setPassphrase(ByteString.copyFrom(passphrase.getBytes()))
                    .build();
        } else {
            cmdSetConfig = WifiConfig.CmdSetConfig
                    .newBuilder()
                    .setSsid(ByteString.copyFrom(ssid.getBytes()))
                    .build();
        }
        WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload
                .newBuilder()
                .setCmdSetConfig(cmdSetConfig)
                .setMsg(WifiConfig.WiFiConfigMsgType.TypeCmdSetConfig)
                .build();

        return wiFiConfigPayload.toByteArray();
    }

    // Apply Wi-Fi config
    public static byte[] prepareApplyWiFiConfigMsg() {

        WifiConfig.CmdApplyConfig cmdApplyConfig = WifiConfig.CmdApplyConfig
                .newBuilder()
                .build();
        WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload
                .newBuilder()
                .setCmdApplyConfig(cmdApplyConfig)
                .setMsg(WifiConfig.WiFiConfigMsgType.TypeCmdApplyConfig)
                .build();
        return wiFiConfigPayload.toByteArray();
    }

    // Get Wi-Fi Config status
    public static byte[] prepareGetWiFiConfigStatusMsg() {

        WifiConfig.CmdGetStatus cmdGetStatus = WifiConfig.CmdGetStatus
                .newBuilder()
                .build();
        WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload
                .newBuilder()
                .setCmdGetStatus(cmdGetStatus)
                .setMsg(WifiConfig.WiFiConfigMsgType.TypeCmdGetStatus)
                .build();
        return wiFiConfigPayload.toByteArray();
    }
}
