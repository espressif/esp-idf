// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
package com.espressif.provision;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import com.espressif.provision.security.Security;
import com.espressif.provision.session.Session;
import com.espressif.provision.transport.ResponseListener;
import com.espressif.provision.transport.Transport;
import com.espressif.ui.BLEProvisionLanding;
import com.espressif.ui.ProvisionLanding;
import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;

import java.util.HashMap;
import java.util.Map;

import espressif.Constants;
import espressif.WifiConfig;
import espressif.WifiConstants;

import static java.lang.Thread.sleep;

/**
 * Provision class which exposes the main API for provisioning
 * the device with Wifi credentials.
 */
public class Provision {
    public static final int REQUEST_PROVISIONING_CODE = 101;
    public static final int REQUEST_PERMISSIONS_CODE = 102;
    public static final int REQUEST_ENABLE_BLE_CODE = 103;
    public static final String CONFIG_TRANSPORT_KEY = "transport";
    public static final String CONFIG_SECURITY_KEY = "security";
    public static final String CONFIG_TRANSPORT_WIFI = "wifi";
    public static final String CONFIG_TRANSPORT_BLE = "ble";
    public static final String CONFIG_PROOF_OF_POSSESSION_KEY = "proofOfPossession";
    public static final String CONFIG_BASE_URL_KEY = "baseUrl";
    public static final String CONFIG_SECURITY_SECURITY1 = "security1";
    public static final String CONFIG_SECURITY_SECURITY0 = "security0";
    public static final String CONFIG_WIFI_AP_KEY = "wifiAPPrefix";
    public static final String PROVISIONING_CONFIG_PATH = "prov-config";
    private static final String TAG = "Espressif::" + Provision.class.getSimpleName();
    public ProvisioningListener provisioningListener;
    private Security security;
    private Transport transport;
    private Session session;

    /**
     * Initialize Provision class with a Session object.
     * Session object should already have been
     * successfully initialized by calling Session.init.
     * @param session
     */
    public Provision(Session session) {
        this.session = session;
        this.security = session.getSecurity();
        this.transport = session.getTransport();
    }

    /**
     * Launch default UI for provisioning Wifi credentials on the device.
     * This UI will take the user through the following flow
     * 1. Connect to the device via Wifi (AP) or Bluetooth (BLE)
     * 2. Provide Network information like SSID and Passphrase
     * @param activity parent activity which is typically part of the App
     * @param config Config dictionary.
     *               Currently supported configurations are
     *                HashMap<String, String> config = new HashMap<>();
     *                config.put(Provision.CONFIG_TRANSPORT_KEY, transportVersion);
     *                config.put(Provision.CONFIG_SECURITY_KEY, securityVersion);
     *                config.put(Provision.CONFIG_PROOF_OF_POSSESSION_KEY, POP);
     *                config.put(Provision.CONFIG_BASE_URL_KEY, BASE_URL);
     *                config.put(Provision.CONFIG_WIFI_AP_KEY, NETWORK_NAME_PREFIX);
     *                config.put(BLETransport.SERVICE_UUID_KEY, SERVICE_UUID);
     *                config.put(BLETransport.SESSION_UUID_KEY, SESSION_UUID);
     *                config.put(BLETransport.CONFIG_UUID_KEY, CONFIG_UUID);
     *                config.put(BLETransport.DEVICE_NAME_PREFIX_KEY, DEVICE_NAME_PREFIX);
     */
    public static void showProvisioningUI (Activity activity,
                                           HashMap<String, String> config) {
        final String transportVersion = config.get(Provision.CONFIG_TRANSPORT_KEY);
        Class landingActivity = ProvisionLanding.class;
        if (transportVersion.equals(Provision.CONFIG_TRANSPORT_BLE)) {
            landingActivity = BLEProvisionLanding.class;
        }

        Intent provisionIntent = new Intent(activity, landingActivity);
        Bundle optionsBundle = new Bundle();
        for (Map.Entry<String, String> entry : config.entrySet()) {
            optionsBundle.putString(entry.getKey(), entry.getValue());
        }
        provisionIntent.putExtras(optionsBundle);

        activity.startActivityForResult(provisionIntent, REQUEST_PROVISIONING_CODE);
    }

    /**
     * Send Configuration information relating to the Home
     * Wifi network which the device should use for Internet access
     * @param ssid ssid of the home network
     * @param passphrase passphrase
     * @param actionListener listener to be called when config data is sent
     */
    public void configureWifi(final String ssid,
                              final String passphrase,
                              final ProvisionActionListener actionListener) {
        if(session.isEstablished()) {
            byte[] message = createSetWifiConfigRequest(ssid, passphrase);
            transport.sendConfigData(PROVISIONING_CONFIG_PATH, message, new ResponseListener() {
                @Override
                public void onSuccess(byte[] returnData) {
                    Constants.Status status = processSetWifiConfigResponse(returnData);
                    if (status != Constants.Status.Success && provisioningListener != null) {
                        provisioningListener.OnProvisioningFailed(new RuntimeException("Could not sent wifi credentials to device"));
                    }
                    if(actionListener != null) {
                        actionListener.onComplete(status, null);
                    }
                }

                @Override
                public void onFailure(Exception e) {
                    if(provisioningListener != null) {
                        provisioningListener.OnProvisioningFailed(e);
                    }
                    if(actionListener != null) {
                        actionListener.onComplete(Constants.Status.InternalError, e);
                    }
                }
            });
        }
    }

    /**
     * Apply all current configurations on the device.
     * A typical flow will be
     * Initialize session -> Set config (1 or more times) -> Apply config
     * This API call will also start a poll for getting Wifi connection status from the device
     * @param actionListener listener to be called when apply config message is sent
     */
    public void applyConfigurations(final ProvisionActionListener actionListener) {
        if(session.isEstablished()) {
            byte[] message = createApplyConfigRequest();
            transport.sendConfigData(PROVISIONING_CONFIG_PATH, message, new ResponseListener() {
                @Override
                public void onSuccess(byte[] returnData) {
                    Constants.Status status = processApplyConfigResponse(returnData);
                    if (status == Constants.Status.Success) {
                        if (provisioningListener != null) {
                            provisioningListener.OnApplyConfigurationsSucceeded();
                        }
                        pollForWifiConnectionStatus(new WifiStateListener() {
                            @Override
                            public void onWifiStateUpdated(WifiConstants.WifiStationState stationState,
                                                           WifiConstants.WifiConnectFailedReason failedReason,
                                                           Exception e) {
                                if (provisioningListener != null) {
                                    provisioningListener.OnWifiConnectionStatusUpdated(stationState,
                                            failedReason,
                                            e);
                                }
                            }
                        });
                    } else {
                        if (provisioningListener != null) {
                            provisioningListener.OnApplyConfigurationsFailed();
                        }
                    }
                    if(actionListener != null) {
                        actionListener.onComplete(status, null);
                    }
                }

                @Override
                public void onFailure(Exception e) {
                    if (provisioningListener != null) {
                        provisioningListener.OnApplyConfigurationsFailed();
                    }
                    if(actionListener != null) {
                        actionListener.onComplete(Constants.Status.InternalError, e);
                    }
                }
            });
        }
    }

    private byte[] createSetWifiConfigRequest(String ssid, String passphrase) {
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
        return this.security.encrypt(wiFiConfigPayload.toByteArray());
    }

    private byte[] getWifiConfigStatusRequest() {
        WifiConfig.CmdGetStatus cmdGetStatus = WifiConfig.CmdGetStatus
                .newBuilder()
                .build();
        WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload
                .newBuilder()
                .setCmdGetStatus(cmdGetStatus)
                .setMsg(WifiConfig.WiFiConfigMsgType.TypeCmdGetStatus)
                .build();
        return this.security.encrypt(wiFiConfigPayload.toByteArray());
    }

    private Constants.Status processSetWifiConfigResponse(byte[] responseData) {
        byte[] decryptedData = this.security.decrypt(responseData);
        Constants.Status status = Constants.Status.InvalidSession;
        try {
            WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload.parseFrom(decryptedData);
            status = wiFiConfigPayload.getRespSetConfig().getStatus();
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
        return status;
    }

    private Object[] processGetWifiConfigStatusResponse(byte[] responseData) {
        WifiConstants.WifiStationState wifiStationState = WifiConstants.WifiStationState.Disconnected;
        WifiConstants.WifiConnectFailedReason failedReason = WifiConstants.WifiConnectFailedReason.UNRECOGNIZED;

        if(responseData == null) {
            return new Object[] { wifiStationState, failedReason };
        }

        byte[] decryptedData = this.security.decrypt(responseData);
        try {
            WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload.parseFrom(decryptedData);
            wifiStationState = wiFiConfigPayload.getRespGetStatus().getStaState();
            failedReason = wiFiConfigPayload.getRespGetStatus().getFailReason();
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
        return new Object[] {wifiStationState, failedReason};
    }

    private void pollForWifiConnectionStatus(final WifiStateListener wifiStateListener) {
        byte[] message = this.getWifiConfigStatusRequest();
        this.transport.sendConfigData(PROVISIONING_CONFIG_PATH, message, new ResponseListener() {
            @Override
            public void onSuccess(byte[] returnData) {
                Object[] statuses = processGetWifiConfigStatusResponse(returnData);
                WifiConstants.WifiStationState wifiStationState = (WifiConstants.WifiStationState) statuses[0];
                WifiConstants.WifiConnectFailedReason failedReason = (WifiConstants.WifiConnectFailedReason) statuses[1];

                if (wifiStationState == WifiConstants.WifiStationState.Connected) {
                    wifiStateListener.onWifiStateUpdated(wifiStationState, failedReason, null);
                } else if (wifiStationState == WifiConstants.WifiStationState.Connecting) {
                    try {
                        sleep(5000);
                        pollForWifiConnectionStatus(wifiStateListener);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                        wifiStateListener.onWifiStateUpdated(wifiStationState, failedReason, null);
                    }
                } else {
                    wifiStateListener.onWifiStateUpdated(wifiStationState, failedReason, null);
                }
            }

            @Override
            public void onFailure(Exception e) {
                wifiStateListener.onWifiStateUpdated(WifiConstants.WifiStationState.Disconnected,
                        WifiConstants.WifiConnectFailedReason.UNRECOGNIZED,
                        e);
            }
        });
    }

    private byte[] createApplyConfigRequest() {
        WifiConfig.CmdApplyConfig cmdApplyConfig = WifiConfig.CmdApplyConfig
                .newBuilder()
                .build();
        WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload
                .newBuilder()
                .setCmdApplyConfig(cmdApplyConfig)
                .setMsg(WifiConfig.WiFiConfigMsgType.TypeCmdApplyConfig)
                .build();
        return this.security.encrypt(wiFiConfigPayload.toByteArray());
    }

    private Constants.Status processApplyConfigResponse(byte[] responseData) {
        byte[] decryptedData = this.security.decrypt(responseData);
        Constants.Status status = Constants.Status.InvalidSession;
        try {
            WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload.parseFrom(decryptedData);
            status = wiFiConfigPayload.getRespApplyConfig().getStatus();
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
        return status;
    }

    private interface WifiStateListener {
        void onWifiStateUpdated(WifiConstants.WifiStationState stationState,
                                WifiConstants.WifiConnectFailedReason failedReason,
                                Exception e);
    }

    /***
     * Interface for listening to Provisioning related events.
     */
    public interface ProvisioningListener {
        /***
         * Called when configurations are successfully applied on the device
         */
        void OnApplyConfigurationsSucceeded();

        /***
         * Called when configurations cannot be applied on the device
         */
        void OnApplyConfigurationsFailed();

        /***
         * Called when wifi status is updated on the device
         * @param newStatus
         * @param failedReason
         * @param e
         */
        void OnWifiConnectionStatusUpdated(WifiConstants.WifiStationState newStatus,
                                           WifiConstants.WifiConnectFailedReason failedReason,
                                           Exception e);

        /***
         * Called when provisioning on the device fails.
         * @param e
         */
        void OnProvisioningFailed(Exception e);
    }

    /***
     * Interface for listening to results from the configureWifi and applyConfigurations API.
     */
    public interface ProvisionActionListener {
        /***
         * Called when provisioning related action is completed
         * @param status
         * @param e
         */
        void onComplete(Constants.Status status, Exception e);
    }
}
