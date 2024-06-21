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

import java.util.ArrayList;
import java.util.List;

import espressif.NetworkConfig;
import espressif.NetworkScan;

public class MessengeHelper {

    // Send Wi-Fi Scan command
    public static byte[] prepareWiFiScanMsg() {

        NetworkScan.CmdScanWifiStart configRequest = NetworkScan.CmdScanWifiStart.newBuilder()
                .setBlocking(true)
                .setPassive(false)
                .setGroupChannels(0)
                .setPeriodMs(120)
                .build();
        NetworkScan.NetworkScanMsgType msgType = NetworkScan.NetworkScanMsgType.TypeCmdScanWifiStart;
        NetworkScan.NetworkScanPayload payload = NetworkScan.NetworkScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanWifiStart(configRequest)
                .build();

        return payload.toByteArray();
    }

    public static byte[] prepareThreadScanMsg() {

        NetworkScan.CmdScanThreadStart configRequest = NetworkScan.CmdScanThreadStart.newBuilder()
                .setBlocking(true)
                .setChannelMask(0)
                .build();
        NetworkScan.NetworkScanMsgType msgType = NetworkScan.NetworkScanMsgType.TypeCmdScanThreadStart;
        NetworkScan.NetworkScanPayload payload = NetworkScan.NetworkScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanThreadStart(configRequest)
                .build();

        return payload.toByteArray();
    }

    public static byte[] prepareGetWiFiScanStatusMsg() {

        NetworkScan.CmdScanWifiStatus configRequest = NetworkScan.CmdScanWifiStatus.newBuilder()
                .build();
        NetworkScan.NetworkScanMsgType msgType = NetworkScan.NetworkScanMsgType.TypeCmdScanWifiStatus;
        NetworkScan.NetworkScanPayload payload = NetworkScan.NetworkScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanWifiStatus(configRequest)
                .build();
        return payload.toByteArray();
    }

    public static byte[] prepareGetThreadScanStatusMsg() {

        NetworkScan.CmdScanThreadStatus configRequest = NetworkScan.CmdScanThreadStatus.newBuilder()
                .build();
        NetworkScan.NetworkScanMsgType msgType = NetworkScan.NetworkScanMsgType.TypeCmdScanThreadStatus;
        NetworkScan.NetworkScanPayload payload = NetworkScan.NetworkScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanThreadStatus(configRequest)
                .build();
        return payload.toByteArray();
    }

    // Get Wi-Fi scan list
    public static byte[] prepareGetWiFiScanListMsg(int start, int count) {

        NetworkScan.CmdScanWifiResult configRequest = NetworkScan.CmdScanWifiResult.newBuilder()
                .setStartIndex(start)
                .setCount(count)
                .build();
        NetworkScan.NetworkScanMsgType msgType = NetworkScan.NetworkScanMsgType.TypeCmdScanWifiResult;
        NetworkScan.NetworkScanPayload payload = NetworkScan.NetworkScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanWifiResult(configRequest)
                .build();

        return payload.toByteArray();
    }

    public static byte[] prepareGetThreadScanListMsg(int start, int count) {

        NetworkScan.CmdScanThreadResult configRequest = NetworkScan.CmdScanThreadResult.newBuilder()
                .setStartIndex(start)
                .setCount(count)
                .build();
        NetworkScan.NetworkScanMsgType msgType = NetworkScan.NetworkScanMsgType.TypeCmdScanThreadResult;
        NetworkScan.NetworkScanPayload payload = NetworkScan.NetworkScanPayload.newBuilder()
                .setMsg(msgType)
                .setCmdScanThreadResult(configRequest)
                .build();

        return payload.toByteArray();
    }

    // Send Wi-Fi Config
    public static byte[] prepareWiFiConfigMsg(String ssid, String passphrase) {

        NetworkConfig.CmdSetWifiConfig cmdSetConfig;

        if (passphrase != null) {

            cmdSetConfig = NetworkConfig.CmdSetWifiConfig
                    .newBuilder()
                    .setSsid(ByteString.copyFrom(ssid.getBytes()))
                    .setPassphrase(ByteString.copyFrom(passphrase.getBytes()))
                    .build();
        } else {
            cmdSetConfig = NetworkConfig.CmdSetWifiConfig
                    .newBuilder()
                    .setSsid(ByteString.copyFrom(ssid.getBytes()))
                    .build();
        }
        NetworkConfig.NetworkConfigPayload wiFiConfigPayload = NetworkConfig.NetworkConfigPayload
                .newBuilder()
                .setMsg(NetworkConfig.NetworkConfigMsgType.TypeCmdSetWifiConfig)
                .setCmdSetWifiConfig(cmdSetConfig)
                .build();

        return wiFiConfigPayload.toByteArray();
    }

    public static byte[] prepareThreadConfigMsg(String activeDataset) {

        byte[] dataset = dsToByteArray(activeDataset);

        NetworkConfig.CmdSetThreadConfig cmdSetConfig = NetworkConfig.CmdSetThreadConfig
                .newBuilder()
                .setDataset(ByteString.copyFrom(dataset))
                .build();
        NetworkConfig.NetworkConfigPayload threadConfigPayload = NetworkConfig.NetworkConfigPayload
                .newBuilder()
                .setMsg(NetworkConfig.NetworkConfigMsgType.TypeCmdSetThreadConfig)
                .setCmdSetThreadConfig(cmdSetConfig)
                .build();

        return threadConfigPayload.toByteArray();
    }

    private static byte[] dsToByteArray(String input) {
        List<Byte> bytes = new ArrayList<>();
        for (int i = 0; i < input.length(); i += 2) {
            String hex = input.substring(i, i + 2);
            bytes.add((byte) Integer.parseInt(hex, 16));
        }
        byte[] result = new byte[bytes.size()];
        for (int i = 0; i < bytes.size(); i++) {
            result[i] = bytes.get(i);
        }
        return result;
    }

    // Apply Wi-Fi config
    public static byte[] prepareApplyWiFiConfigMsg() {

        NetworkConfig.CmdApplyWifiConfig cmdApplyConfig = NetworkConfig.CmdApplyWifiConfig
                .newBuilder()
                .build();
        NetworkConfig.NetworkConfigPayload wiFiConfigPayload = NetworkConfig.NetworkConfigPayload
                .newBuilder()
                .setMsg(NetworkConfig.NetworkConfigMsgType.TypeCmdApplyWifiConfig)
                .setCmdApplyWifiConfig(cmdApplyConfig)
                .build();
        return wiFiConfigPayload.toByteArray();
    }

    public static byte[] prepareApplyThreadConfigMsg() {

        NetworkConfig.CmdApplyThreadConfig cmdApplyConfig = NetworkConfig.CmdApplyThreadConfig
                .newBuilder()
                .build();
        NetworkConfig.NetworkConfigPayload threadConfigPayload = NetworkConfig.NetworkConfigPayload
                .newBuilder()
                .setMsg(NetworkConfig.NetworkConfigMsgType.TypeCmdApplyThreadConfig)
                .setCmdApplyThreadConfig(cmdApplyConfig)
                .build();
        return threadConfigPayload.toByteArray();
    }

    // Get Wi-Fi Config status
    public static byte[] prepareGetWiFiConfigStatusMsg() {

        NetworkConfig.CmdGetWifiStatus cmdGetStatus = NetworkConfig.CmdGetWifiStatus
                .newBuilder()
                .build();
        NetworkConfig.NetworkConfigPayload wiFiConfigPayload = NetworkConfig.NetworkConfigPayload
                .newBuilder()
                .setMsg(NetworkConfig.NetworkConfigMsgType.TypeCmdGetWifiStatus)
                .setCmdGetWifiStatus(cmdGetStatus)
                .build();
        return wiFiConfigPayload.toByteArray();
    }

    // Get Wi-Fi Config status
    public static byte[] prepareGetThreadConfigStatusMsg() {

        NetworkConfig.CmdGetThreadStatus cmdGetStatus = NetworkConfig.CmdGetThreadStatus
                .newBuilder()
                .build();
        NetworkConfig.NetworkConfigPayload threadConfigPayload = NetworkConfig.NetworkConfigPayload
                .newBuilder()
                .setMsg(NetworkConfig.NetworkConfigMsgType.TypeCmdGetThreadStatus)
                .setCmdGetThreadStatus(cmdGetStatus)
                .build();
        return threadConfigPayload.toByteArray();
    }
}
