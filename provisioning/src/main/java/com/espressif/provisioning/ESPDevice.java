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

import static java.lang.Thread.sleep;

import android.Manifest;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiNetworkSpecifier;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresPermission;

import com.espressif.provisioning.listeners.ProvisionListener;
import com.espressif.provisioning.listeners.ResponseListener;
import com.espressif.provisioning.listeners.WiFiScanListener;
import com.espressif.provisioning.security.Security;
import com.espressif.provisioning.security.Security0;
import com.espressif.provisioning.security.Security1;
import com.espressif.provisioning.security.Security2;
import com.espressif.provisioning.transport.BLETransport;
import com.espressif.provisioning.transport.SoftAPTransport;
import com.espressif.provisioning.transport.Transport;
import com.espressif.provisioning.utils.MessengeHelper;
import com.google.protobuf.InvalidProtocolBufferException;

import org.greenrobot.eventbus.EventBus;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import espressif.Constants;
import espressif.WifiConfig;
import espressif.WifiConstants;
import espressif.WifiScan;

/**
 * ESPDevice class to hold device information. This will give facility to connect device, send data to device and
 * do provisioning of it.
 */
public class ESPDevice {

    private static final String TAG = "ESP:" + ESPDevice.class.getSimpleName();

    private Context context;
    private Handler handler;

    private Session session;
    private Security security;
    private Transport transport;

    private WiFiScanListener wifiScanListener;
    private ProvisionListener provisionListener;
    private ResponseListener responseListener;

    // Transport & security type must be set before session init.
    private ESPConstants.TransportType transportType;
    private ESPConstants.SecurityType securityType;

    private String proofOfPossession = "";
    private String userName = "";
    private String versionInfo;
    private int totalCount;
    private int startIndex;
    private ArrayList<WiFiAccessPoint> wifiApList;
    private ArrayList<String> deviceCapabilities = new ArrayList<>();
    private BluetoothDevice bluetoothDevice;
    private WiFiAccessPoint wifiDevice;
    private String primaryServiceUuid;
    private String deviceName;
    private WifiManager wifiManager;
    private ConnectivityManager connectivityManager;
    private ConnectivityManager.NetworkCallback networkCallback;

    public ESPDevice(Context context, ESPConstants.TransportType transportType, ESPConstants.SecurityType securityType) {

        this.context = context;
        handler = new Handler(Looper.getMainLooper());
        this.transportType = transportType;
        this.securityType = securityType;
        wifiManager = (WifiManager) context.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);

        switch (transportType) {

            case TRANSPORT_BLE:
                transport = new BLETransport(context);
                break;

            case TRANSPORT_SOFTAP:
                transport = new SoftAPTransport();
                break;
        }
    }

    /**
     * This method is used to connect ESPDevice.
     */
    @RequiresPermission(allOf = {Manifest.permission.CHANGE_WIFI_STATE, Manifest.permission.ACCESS_WIFI_STATE, Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.ACCESS_FINE_LOCATION})
    public void connectToDevice() {

        switch (transportType) {

            case TRANSPORT_BLE:
                ((BLETransport) transport).connect(bluetoothDevice, UUID.fromString(primaryServiceUuid));
                break;

            case TRANSPORT_SOFTAP:
                deviceConnectionReqCount = 0;
                connectWiFiDevice(wifiDevice.getWifiName(), wifiDevice.getPassword());
                break;
        }
    }

    /**
     * This method is used to connect ESPDevice using BLE transport.
     *
     * @param bluetoothDevice    BluetoothDevice
     * @param primaryServiceUuid Primary service UUID.
     */
    @RequiresPermission(Manifest.permission.BLUETOOTH)
    public void connectBLEDevice(BluetoothDevice bluetoothDevice, String primaryServiceUuid) {

        if (transport instanceof BLETransport) {
            deviceName = bluetoothDevice.getName();
            ((BLETransport) transport).connect(bluetoothDevice, UUID.fromString(primaryServiceUuid));
        } else {
            Log.e(TAG, "Trying to connect device with wrong transport.");
            EventBus.getDefault().post(new DeviceConnectionEvent(ESPConstants.EVENT_DEVICE_CONNECTION_FAILED));
        }
    }

    /**
     * This method is used to connect ESPDevice using Wi-Fi transport.
     */
    @RequiresPermission(Manifest.permission.ACCESS_NETWORK_STATE)
    public void connectWiFiDevice() {

        if (transport instanceof SoftAPTransport) {

            enableOnlyWifiNetwork();
            deviceConnectionReqCount = 0;
            getCapabilitiesFromDevice();

        } else {
            Log.e(TAG, "Trying to connect device with wrong transport.");
            EventBus.getDefault().post(new DeviceConnectionEvent(ESPConstants.EVENT_DEVICE_CONNECTION_FAILED));
        }
    }

    /**
     * This method is used to connect ESPDevice using Wi-Fi transport.
     *
     * @param ssid     SSID of the device.
     * @param password Password of Wi-Fi device.
     */
    @RequiresPermission(allOf = {Manifest.permission.CHANGE_WIFI_STATE, Manifest.permission.ACCESS_WIFI_STATE, Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.ACCESS_FINE_LOCATION})
    public void connectWiFiDevice(String ssid, String password) {

        Log.d(TAG, "connectWiFiDevice ========== SSID : " + ssid + " and Password : " + password);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {

            WifiNetworkSpecifier.Builder builder = new WifiNetworkSpecifier.Builder();
            builder.setSsid(ssid);
            builder.setWpa2Passphrase(password);

            WifiNetworkSpecifier wifiNetworkSpecifier = builder.build();

            NetworkRequest.Builder networkRequestBuilder = new NetworkRequest.Builder();
            networkRequestBuilder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
            networkRequestBuilder.removeCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
            networkRequestBuilder.setNetworkSpecifier(wifiNetworkSpecifier);

            NetworkRequest networkRequest = networkRequestBuilder.build();

            networkCallback = new ConnectivityManager.NetworkCallback() {

                @Override
                public void onAvailable(Network network) {

                    Log.e(TAG, "Network is available - 1");
                    connectivityManager.bindProcessToNetwork(network);
                    getCapabilitiesFromDevice();
                }

                @Override
                public void onUnavailable() {
                    super.onUnavailable();
                    Log.e(TAG, "Network is Unavailable - 1");
                    handler.postDelayed(wifiConnectionFailedTask, 200);
                }

                @Override
                public void onLost(@NonNull Network network) {
                    super.onLost(network);
                    Log.e(TAG, "Lost Network Connection - 1");
                }
            };

            connectivityManager.requestNetwork(networkRequest, networkCallback);

        } else {

            NetworkRequest.Builder request = new NetworkRequest.Builder();
            request.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
            request.removeCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);// Internet not required

            networkCallback = new ConnectivityManager.NetworkCallback() {

                @Override
                public void onAvailable(Network network) {

                    Log.e(TAG, "Network is available - 2");
                    connectivityManager.bindProcessToNetwork(network);
                }

                @Override
                public void onUnavailable() {
                    super.onUnavailable();
                    Log.e(TAG, "Network is Unavailable - 2");
                }

                @Override
                public void onLost(@NonNull Network network) {
                    super.onLost(network);
                    Log.e(TAG, "Lost Network Connection - 2");
                }
            };
            connectivityManager.registerNetworkCallback(request.build(), networkCallback);

            if (!wifiManager.isWifiEnabled()) {
                wifiManager.setWifiEnabled(true);
            }

            Log.d(TAG, "Device name : " + ssid);
            Log.d(TAG, "Device password : " + password);

            WifiConfiguration config = new WifiConfiguration();
            config.SSID = String.format("\"%s\"", ssid);

            int netId = -1;
            List<WifiConfiguration> apList = wifiManager.getConfiguredNetworks();
            Log.d(TAG, "List Size : " + apList.size());

            for (WifiConfiguration i : apList) {

                if (i.SSID != null && i.SSID.equals("\"" + ssid + "\"")) {
                    netId = i.networkId;
                }
            }

            if (netId == -1) {

                if (TextUtils.isEmpty(password)) {
                    Log.i(TAG, "Connect to open network");
                    config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
                } else {
                    Log.i(TAG, "Connect to secure network");
                    config.preSharedKey = String.format("\"%s\"", password);
                }

                netId = wifiManager.addNetwork(config);
                Log.d(TAG, "Network Id : " + netId);
            }

            if (netId != -1) {

                Log.d(TAG, "Connect to network : " + netId);
                wifiManager.enableNetwork(netId, true);
                scheduleWiFiConnectionFailure();
                checkDeviceConnection(ssid);

            } else {
                Log.e(TAG, "Failed to add network");
                EventBus.getDefault().post(new DeviceConnectionEvent(ESPConstants.EVENT_DEVICE_CONNECTION_FAILED));
            }
        }
    }

    /**
     * This method is used to disconnect ESPDevice.
     * Note : It will disconnect only if device is connected thorough BLE transport.
     */
    public void disconnectDevice() {

        if (transport instanceof BLETransport) {
            ((BLETransport) transport).disconnect();
        }
        session = null;
        disableOnlyWifiNetwork();
    }

    public void refreshServicesOfBleDevice() {

        if (transport instanceof BLETransport) {
            ((BLETransport) transport).refreshServices();
        }
    }

    /**
     * This method is used to set Proof Of Possession.
     *
     * @param pop Proof Of Possession of the device.
     */
    public void setProofOfPossession(String pop) {
        this.proofOfPossession = pop;
    }

    /**
     * This method is used to get Proof Of Possession.
     *
     * @return Returns Proof Of Possession of the device.
     */
    public String getProofOfPossession() {
        return proofOfPossession;
    }

    /**
     * This method is used to set username. It is used for Sec2 security type.
     *
     * @param username Username.
     */
    public void setUserName(String username) {
        this.userName = username;
    }

    /**
     * This method is used to get username. It is used for Sec2 security type.
     *
     * @return Returns Username.
     */
    public String getUserName() {
        return userName;
    }

    /**
     * This method is used to get version information to determine what features are enabled in a device and act accordingly.
     *
     * @return Returns device version information.
     */
    public String getVersionInfo() {

        if (transport instanceof BLETransport) {
            return ((BLETransport) transport).versionInfo;
        } else {
            return versionInfo;
        }
    }

    /**
     * This method is used to set device name.
     *
     * @param deviceName Device name to be set.
     */
    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    /**
     * This method is used to get Proof Of Possession.
     *
     * @return Returns device name.
     */
    public String getDeviceName() {
        return deviceName;
    }

    /**
     * This method is used to get device capabilities.
     *
     * @return Returns device capabilities.
     */
    public ArrayList<String> getDeviceCapabilities() {

        if (transport instanceof BLETransport) {
            return ((BLETransport) transport).deviceCapabilities;
        } else {
            return deviceCapabilities;
        }
    }

    /**
     * This method is used to get transport type.
     *
     * @return Returns transport type.
     */
    public ESPConstants.TransportType getTransportType() {
        return transportType;
    }

    /**
     * This method is used to get security type.
     *
     * @return Returns security type.
     */
    public ESPConstants.SecurityType getSecurityType() {
        return securityType;
    }

    /**
     * This method is used to set / change security type.
     * This should be call before session creation.
     *
     * @return Returns security type.
     */
    public void setSecurityType(ESPConstants.SecurityType secType) {
        securityType = secType;
    }

    /**
     * This method is used to get Wi-Fi access point.
     *
     * @return Returns Wi-Fi access point.
     */
    public WiFiAccessPoint getWifiDevice() {
        return wifiDevice;
    }

    public void setWifiDevice(WiFiAccessPoint wifiDevice) {
        this.wifiDevice = wifiDevice;
    }

    /**
     * This method is used to get BluetoothDevice object to connect with device using BLE.
     *
     * @return Returns BluetoothDevice object of the device.
     */
    public BluetoothDevice getBluetoothDevice() {
        return bluetoothDevice;
    }

    /**
     * This method is used to set BluetoothDevice.
     *
     * @param bluetoothDevice BluetoothDevice
     */
    public void setBluetoothDevice(BluetoothDevice bluetoothDevice) {
        this.bluetoothDevice = bluetoothDevice;
    }

    /**
     * This method is used to get primary service UUID of the BLE device.
     *
     * @return Returns Primary service UUID of the device.
     */
    public String getPrimaryServiceUuid() {
        return primaryServiceUuid;
    }

    /**
     * This method is used to set primary service UUID.
     *
     * @param primaryServiceUuid Primary service UUID of the device.
     */
    public void setPrimaryServiceUuid(String primaryServiceUuid) {
        this.primaryServiceUuid = primaryServiceUuid;
    }

    /**
     * Send scan command to device to get available Wi-Fi access points.
     *
     * @param wifiScanListener WiFiScanListener to get callbacks of scanning networks.
     */
    public void scanNetworks(final WiFiScanListener wifiScanListener) {

        Log.d(TAG, "Send Wi-Fi scan command to device");
        this.wifiScanListener = wifiScanListener;

        if (session == null || !session.isEstablished()) {

            initSession(new ResponseListener() {

                @Override
                public void onSuccess(byte[] returnData) {
                    startNetworkScan();
                }

                @Override
                public void onFailure(Exception e) {
                    e.printStackTrace();
                    if (wifiScanListener != null) {
                        wifiScanListener.onWiFiScanFailed(new RuntimeException("Failed to create session."));
                    }
                }
            });
        } else {
            startNetworkScan();
        }
    }

    /**
     * Send data to custom endpoint of the device.
     *
     * @param path     Endpoint.
     * @param data     Data to be send.
     * @param listener Listener to get success and failure.
     */
    public void sendDataToCustomEndPoint(final String path, final byte[] data, final ResponseListener listener) {

        this.responseListener = listener;

        if (session == null || !session.isEstablished()) {

            initSession(new ResponseListener() {

                @Override
                public void onSuccess(byte[] returnData) {
                    sendData(path, data, listener);
                }

                @Override
                public void onFailure(Exception e) {
                    e.printStackTrace();
                    if (responseListener != null) {
                        responseListener.onFailure(e);
                    }
                }
            });
        } else {
            sendData(path, data, listener);
        }
    }

    /**
     * Send Wi-Fi credentials to device for provisioning.
     *
     * @param ssid              SSID of the Wi-Fi which is to be configure in device.
     * @param passphrase        Password of the Wi-Fi which is to be configure in device.
     * @param provisionListener Listener for provisioning callbacks.
     */
    public void provision(final String ssid, final String passphrase, final ProvisionListener provisionListener) {

        this.provisionListener = provisionListener;

        if (session == null || !session.isEstablished()) {

            initSession(new ResponseListener() {

                @Override
                public void onSuccess(byte[] returnData) {
                    sendWiFiConfig(ssid, passphrase, provisionListener);
                }

                @Override
                public void onFailure(Exception e) {
                    e.printStackTrace();
                    disableOnlyWifiNetwork();
                    if (provisionListener != null) {
                        provisionListener.createSessionFailed(new RuntimeException("Failed to create session."));
                    }
                }
            });
        } else {
            sendWiFiConfig(ssid, passphrase, provisionListener);
        }
    }

    public void initSession(final ResponseListener listener) {

        try {
            JSONObject jsonObject = new JSONObject(getVersionInfo());
            JSONObject provInfo = jsonObject.getJSONObject("prov");

            String deviceVersion = provInfo.getString("ver");
            Log.d(TAG, "Device Version : " + deviceVersion);
            Log.d(TAG, "sec_ver value : " + provInfo.optInt("sec_ver"));
            Log.d(TAG, "Has sec_ver key : " + provInfo.has("sec_ver"));

            if (provInfo.has("sec_ver")) {

                int serVer = provInfo.optInt("sec_ver");
                Log.d(TAG, "Security Version : " + serVer);

                switch (serVer) {
                    case 0:
                        securityType = ESPConstants.SecurityType.SECURITY_0;
                        break;
                    case 1:
                        securityType = ESPConstants.SecurityType.SECURITY_1;
                        break;
                    case 2:
                    default:
                        securityType = ESPConstants.SecurityType.SECURITY_2;
                        break;
                }
            } else {
                Log.e(TAG, "Older firmware as Sec version not found.");
                if (securityType == ESPConstants.SecurityType.SECURITY_2) {
                    securityType = ESPConstants.SecurityType.SECURITY_1;
                }
            }
        } catch (JSONException e) {
            e.printStackTrace();
            Log.d(TAG, "Capabilities JSON not available.");
        }

        try {
            Log.d(TAG, "Init session with : " + securityType);

            switch (securityType) {
                case SECURITY_0:
                    security = new Security0();
                    break;
                case SECURITY_1:
                    security = new Security1(proofOfPossession);
                    break;
                case SECURITY_2:
                    security = new Security2(userName, proofOfPossession);
                    break;
            }

            session = new Session(transport, security);

            session.init(null, new Session.SessionListener() {

                @Override
                public void OnSessionEstablished() {
                    listener.onSuccess(null);
                }

                @Override
                public void OnSessionEstablishFailed(Exception e) {
                    listener.onFailure(e);
                }
            });
        } catch (Exception e) {
            e.printStackTrace();
            listener.onFailure(e);
        }
    }

    private void sendData(final String path, byte[] data, final ResponseListener listener) {

        session.sendDataToDevice(path, data, new ResponseListener() {

            @Override
            public void onSuccess(byte[] returnData) {
                if (listener != null) {
                    listener.onSuccess(returnData);
                }
            }

            @Override
            public void onFailure(Exception e) {
                e.printStackTrace();
                if (listener != null) {
                    listener.onFailure(e);
                }
            }
        });
    }

    private void startNetworkScan() {

        totalCount = 0;
        startIndex = 0;
        wifiApList = new ArrayList<>();
        byte[] scanCommand = MessengeHelper.prepareWiFiScanMsg();

        session.sendDataToDevice(ESPConstants.HANDLER_PROV_SCAN, scanCommand, new ResponseListener() {

            @Override
            public void onSuccess(byte[] returnData) {

                processStartScanResponse(returnData);

                byte[] getScanStatusCmd = MessengeHelper.prepareGetWiFiScanStatusMsg();
                session.sendDataToDevice(ESPConstants.HANDLER_PROV_SCAN, getScanStatusCmd, new ResponseListener() {

                    @Override
                    public void onSuccess(byte[] returnData) {
                        processWifiStatusResponse(returnData);
                    }

                    @Override
                    public void onFailure(Exception e) {
                        e.printStackTrace();
                        if (wifiScanListener != null) {
                            wifiScanListener.onWiFiScanFailed(new RuntimeException("Failed to send Wi-Fi scan command."));
                        }
                    }
                });
            }

            @Override
            public void onFailure(Exception e) {
                e.printStackTrace();
                if (wifiScanListener != null) {
                    wifiScanListener.onWiFiScanFailed(new RuntimeException("Failed to send Wi-Fi scan command."));
                }
            }
        });
    }

    private void getFullWiFiList() {

        Log.d(TAG, "Total count : " + totalCount + " and start index is : " + startIndex);

        if (totalCount < 4) {

            getWiFiScanList(0, totalCount);

        } else {

            int temp = totalCount - startIndex;

            if (temp > 0) {

                if (temp > 4) {
                    getWiFiScanList(startIndex, 4);
                } else {
                    getWiFiScanList(startIndex, temp);
                }

            } else {
                Log.d(TAG, "Nothing to do. Wifi list completed.");
                completeWifiList();
            }
        }
    }

    private void getWiFiScanList(int start, int count) {

        Log.d(TAG, "Getting " + count + " SSIDs");

        if (count <= 0) {
            completeWifiList();
            return;
        }

        byte[] data = MessengeHelper.prepareGetWiFiScanListMsg(start, count);
        session.sendDataToDevice(ESPConstants.HANDLER_PROV_SCAN, data, new ResponseListener() {

            @Override
            public void onSuccess(byte[] returnData) {
                Log.d(TAG, "Successfully got SSID list");
                processGetSSIDs(returnData);
            }

            @Override
            public void onFailure(Exception e) {
                e.printStackTrace();
                if (wifiScanListener != null) {
                    wifiScanListener.onWiFiScanFailed(new RuntimeException("Failed to get Wi-Fi Networks."));
                }
            }
        });
    }

    private void completeWifiList() {

        if (wifiScanListener != null) {
            wifiScanListener.onWifiListReceived(wifiApList);
        }
    }

    private void sendWiFiConfig(final String ssid, final String passphrase, final ProvisionListener provisionListener) {

        byte[] scanCommand = MessengeHelper.prepareWiFiConfigMsg(ssid, passphrase);

        session.sendDataToDevice(ESPConstants.HANDLER_PROV_CONFIG, scanCommand, new ResponseListener() {

            @Override
            public void onSuccess(byte[] returnData) {

                Constants.Status status = processWifiConfigResponse(returnData);
                if (provisionListener != null) {
                    if (status != Constants.Status.Success) {
                        provisionListener.wifiConfigFailed(new RuntimeException("Failed to send wifi credentials to device"));
                    } else {
                        provisionListener.wifiConfigSent();
                    }
                }

                if (status == Constants.Status.Success) {
                    applyWiFiConfig();
                } else {
                    disableOnlyWifiNetwork();
                }
            }

            @Override
            public void onFailure(Exception e) {
                e.printStackTrace();
                disableOnlyWifiNetwork();
                if (provisionListener != null) {
                    provisionListener.wifiConfigFailed(new RuntimeException("Failed to send wifi credentials to device"));
                }
            }
        });
    }

    private void applyWiFiConfig() {

        byte[] scanCommand = MessengeHelper.prepareApplyWiFiConfigMsg();

        session.sendDataToDevice(ESPConstants.HANDLER_PROV_CONFIG, scanCommand, new ResponseListener() {

            @Override
            public void onSuccess(byte[] returnData) {

                Constants.Status status = processApplyConfigResponse(returnData);

                if (status == Constants.Status.Success) {
                    if (provisionListener != null) {
                        provisionListener.wifiConfigApplied();
                    }

                    try {
                        Thread.sleep(2000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    pollForWifiConnectionStatus();
                } else {
                    disableOnlyWifiNetwork();
                    if (provisionListener != null) {
                        provisionListener.wifiConfigApplyFailed(new RuntimeException("Failed to apply wifi credentials"));
                    }
                }
            }

            @Override
            public void onFailure(Exception e) {
                e.printStackTrace();
                disableOnlyWifiNetwork();
                if (provisionListener != null) {
                    provisionListener.wifiConfigApplyFailed(new RuntimeException("Failed to apply wifi credentials"));
                }
            }
        });
    }

    private void pollForWifiConnectionStatus() {

        byte[] message = MessengeHelper.prepareGetWiFiConfigStatusMsg();
        session.sendDataToDevice(ESPConstants.HANDLER_PROV_CONFIG, message, new ResponseListener() {

            @Override
            public void onSuccess(byte[] returnData) {

                Object[] statuses = processProvisioningStatusResponse(returnData);
                WifiConstants.WifiStationState wifiStationState = (WifiConstants.WifiStationState) statuses[0];
                WifiConstants.WifiConnectFailedReason failedReason = (WifiConstants.WifiConnectFailedReason) statuses[1];

                if (wifiStationState == WifiConstants.WifiStationState.Connected) {

                    // Provision success
                    if (provisionListener != null) {
                        provisionListener.deviceProvisioningSuccess();
                    }
                    session = null;
                    disableOnlyWifiNetwork();

                } else if (wifiStationState == WifiConstants.WifiStationState.Disconnected) {

                    // Device disconnected but Provision may got success / failure
                    if (provisionListener != null) {
                        provisionListener.provisioningFailedFromDevice(ESPConstants.ProvisionFailureReason.DEVICE_DISCONNECTED);
                    }
                    session = null;
                    disableOnlyWifiNetwork();

                } else if (wifiStationState == WifiConstants.WifiStationState.Connecting) {

                    try {
                        sleep(5000);
                        pollForWifiConnectionStatus();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                        session = null;
                        disableOnlyWifiNetwork();
                        provisionListener.onProvisioningFailed(new RuntimeException("Provisioning Failed"));
                    }
                } else {

                    if (failedReason == WifiConstants.WifiConnectFailedReason.AuthError) {

                        provisionListener.provisioningFailedFromDevice(ESPConstants.ProvisionFailureReason.AUTH_FAILED);

                    } else if (failedReason == WifiConstants.WifiConnectFailedReason.NetworkNotFound) {

                        provisionListener.provisioningFailedFromDevice(ESPConstants.ProvisionFailureReason.NETWORK_NOT_FOUND);

                    } else {
                        provisionListener.provisioningFailedFromDevice(ESPConstants.ProvisionFailureReason.UNKNOWN);
                    }
                    session = null;
                    disableOnlyWifiNetwork();
                }
            }

            @Override
            public void onFailure(Exception e) {
                e.printStackTrace();
                disableOnlyWifiNetwork();
                provisionListener.onProvisioningFailed(new RuntimeException("Provisioning Failed"));
            }
        });
    }

    private void processStartScanResponse(byte[] responseData) {

        Log.d(TAG, "Process Wi-Fi start scan command response");

        try {
            WifiScan.WiFiScanPayload payload = WifiScan.WiFiScanPayload.parseFrom(responseData);
            WifiScan.RespScanStart response = WifiScan.RespScanStart.parseFrom(payload.toByteArray());
            // TODO Proto should send status as ok started or failed
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
    }

    private void processWifiStatusResponse(byte[] responseData) {

        Log.d(TAG, "Process Wi-Fi scan status command response");
        try {
            WifiScan.WiFiScanPayload payload = WifiScan.WiFiScanPayload.parseFrom(responseData);
            WifiScan.RespScanStatus response = payload.getRespScanStatus();
            boolean scanFinished = response.getScanFinished();

            if (scanFinished) {
                totalCount = response.getResultCount();
                getFullWiFiList();
            } else {
                // TODO Error case
            }

        } catch (InvalidProtocolBufferException e) {

            e.printStackTrace();
            if (wifiScanListener != null) {
                wifiScanListener.onWiFiScanFailed(new RuntimeException("Failed to get Wi-Fi status."));
            }
        }
    }

    private void processGetSSIDs(byte[] responseData) {

        try {
            WifiScan.WiFiScanPayload payload = WifiScan.WiFiScanPayload.parseFrom(responseData);
            final WifiScan.RespScanResult response = payload.getRespScanResult();

            Log.d(TAG, "Response count : " + response.getEntriesCount());

            for (int i = 0; i < response.getEntriesCount(); i++) {

                Log.d(TAG, "SSID : " + response.getEntries(i).getSsid().toStringUtf8());
                String ssid = response.getEntries(i).getSsid().toStringUtf8();
                int rssi = response.getEntries(i).getRssi();
                boolean isAvailable = false;

                for (int index = 0; index < wifiApList.size(); index++) {

                    if (ssid.equals(wifiApList.get(index).getWifiName())) {

                        isAvailable = true;

                        if (wifiApList.get(index).getRssi() < rssi) {

                            wifiApList.get(index).setRssi(rssi);
                        }
                        break;
                    }
                }

                if (!isAvailable) {

                    WiFiAccessPoint wifiAp = new WiFiAccessPoint();
                    wifiAp.setWifiName(ssid);
                    wifiAp.setRssi(response.getEntries(i).getRssi());
                    wifiAp.setSecurity(response.getEntries(i).getAuthValue());
                    wifiApList.add(wifiAp);
                }

                Log.d(TAG, "Size of  list : " + wifiApList.size());
            }

            startIndex = startIndex + 4;

            int temp = totalCount - startIndex;

            if (temp > 0) {

                getFullWiFiList();

            } else {

                Log.e(TAG, "Wi-Fi LIST Completed");
                completeWifiList();
            }
        } catch (InvalidProtocolBufferException e) {

            e.printStackTrace();
        }
    }

    private Constants.Status processWifiConfigResponse(byte[] responseData) {

        Constants.Status status = Constants.Status.InvalidSession;
        try {
            WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload.parseFrom(responseData);
            status = wiFiConfigPayload.getRespSetConfig().getStatus();
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
        return status;
    }

    private Constants.Status processApplyConfigResponse(byte[] responseData) {
        Constants.Status status = Constants.Status.InvalidSession;
        try {
            WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload.parseFrom(responseData);
            status = wiFiConfigPayload.getRespApplyConfig().getStatus();
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
        return status;
    }

    private Object[] processProvisioningStatusResponse(byte[] responseData) {

        WifiConstants.WifiStationState wifiStationState = WifiConstants.WifiStationState.Disconnected;
        WifiConstants.WifiConnectFailedReason failedReason = WifiConstants.WifiConnectFailedReason.UNRECOGNIZED;

        if (responseData == null) {
            return new Object[]{wifiStationState, failedReason};
        }

        try {
            WifiConfig.WiFiConfigPayload wiFiConfigPayload = WifiConfig.WiFiConfigPayload.parseFrom(responseData);
            wifiStationState = wiFiConfigPayload.getRespGetStatus().getStaState();
            failedReason = wiFiConfigPayload.getRespGetStatus().getFailReason();
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
        return new Object[]{wifiStationState, failedReason};
    }

    private int deviceConnectionReqCount = 0;

    @RequiresPermission(Manifest.permission.ACCESS_NETWORK_STATE)
    private void enableOnlyWifiNetwork() {

        Log.d(TAG, "enableOnlyWifiNetwork()");

        NetworkRequest.Builder request = new NetworkRequest.Builder();
        request.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);

        networkCallback = new ConnectivityManager.NetworkCallback() {

            @Override
            public void onAvailable(Network network) {

                Log.e(TAG, "Network is available - 3");
                connectivityManager.bindProcessToNetwork(network);
            }

            @Override
            public void onUnavailable() {
                super.onUnavailable();
                Log.e(TAG, "Network is Unavailable - 3");
            }

            @Override
            public void onLost(@NonNull Network network) {
                super.onLost(network);
                Log.e(TAG, "Lost Network Connection - 3");
            }
        };
        connectivityManager.registerNetworkCallback(request.build(), networkCallback);
    }

    private void disableOnlyWifiNetwork() {

        Log.d(TAG, "disableOnlyWifiNetwork()");

        if (connectivityManager != null) {

            try {
                connectivityManager.bindProcessToNetwork(null);
                connectivityManager.unregisterNetworkCallback(networkCallback);
            } catch (Exception e) {
                Log.e(TAG, "Connectivity Manager is already unregistered");
            }
        }
    }

    private Runnable getCapabilitiesTask = new Runnable() {

        @Override
        public void run() {

            Log.d(TAG, "Connecting to device");
            deviceConnectionReqCount++;
            String tempData = "ESP";

            transport.sendConfigData(ESPConstants.HANDLER_PROTO_VER, tempData.getBytes(), new ResponseListener() {

                @RequiresPermission(allOf = {Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.ACCESS_WIFI_STATE})
                @Override
                public void onSuccess(byte[] returnData) {

                    String data = new String(returnData, StandardCharsets.UTF_8);
                    Log.d(TAG, "Value : " + data);
                    versionInfo = data;
                    deviceCapabilities = new ArrayList<>();

                    try {
                        JSONObject jsonObject = new JSONObject(data);
                        JSONObject provInfo = jsonObject.getJSONObject("prov");

                        String versionInfo = provInfo.getString("ver");
                        Log.d(TAG, "Device Version : " + versionInfo);

                        JSONArray capabilities = provInfo.getJSONArray("cap");

                        for (int i = 0; i < capabilities.length(); i++) {
                            String cap = capabilities.getString(i);
                            deviceCapabilities.add(cap);
                        }
                        Log.d(TAG, "Capabilities : " + deviceCapabilities);

                    } catch (JSONException e) {
                        e.printStackTrace();
                        Log.d(TAG, "Capabilities JSON not available.");
                    }
                    deviceName = fetchWiFiSSID();
                    handler.removeCallbacks(wifiConnectionFailedTask);
                    EventBus.getDefault().post(new DeviceConnectionEvent(ESPConstants.EVENT_DEVICE_CONNECTED));
                }

                @Override
                public void onFailure(Exception e) {
                    e.printStackTrace();

                    if (deviceConnectionReqCount == 3) {

                        handler.removeCallbacks(getCapabilitiesTask);
                        sendDeviceConnectionFailure();
                    } else {
                        getCapabilitiesFromDevice();
                    }
                }
            });
        }
    };

    private Runnable deviceConnectionFailedTask = new Runnable() {

        @Override
        public void run() {

            handler.removeCallbacks(getCapabilitiesTask);
            handler.removeCallbacks(deviceConnectionFailedTask);
            Log.e(TAG, "deviceConnectionFailedTask");
            EventBus.getDefault().post(new DeviceConnectionEvent(ESPConstants.EVENT_DEVICE_CONNECTION_FAILED));
        }
    };

    private void sendDeviceConnectionFailure() {
        handler.postDelayed(deviceConnectionFailedTask, 1000);
    }

    private void getCapabilitiesFromDevice() {

        handler.removeCallbacks(getCapabilitiesTask);
        handler.postDelayed(getCapabilitiesTask, 100);
    }

    private void scheduleWiFiConnectionFailure() {
        Log.e(TAG, "Schedule wifiConnectionFailedTask");
        handler.postDelayed(wifiConnectionFailedTask, 12000);
    }

    @RequiresPermission(allOf = {Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.ACCESS_WIFI_STATE})
    private String fetchWiFiSSID() {

        String ssid = null;
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        if (wifiInfo.getSupplicantState() == SupplicantState.COMPLETED) {

            ssid = wifiInfo.getSSID();
            ssid = ssid.replace("\"", "");
        }
        Log.d(TAG, "Returning ssid : " + ssid);
        return ssid;
    }

    private Runnable wifiConnectionFailedTask = new Runnable() {

        @Override
        public void run() {

            handler.removeCallbacks(task);
            Log.e(TAG, "wifiConnectionFailedTask");
            EventBus.getDefault().post(new DeviceConnectionEvent(ESPConstants.EVENT_DEVICE_CONNECTION_FAILED));
        }
    };

    private class FetchNetworkName implements Runnable {

        private String ssid;

        FetchNetworkName(String ssid) {
            this.ssid = ssid;
        }

        @Override
        @RequiresPermission(allOf = {Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.ACCESS_WIFI_STATE})
        public void run() {

            String networkName = fetchWiFiSSID();
            Log.d(TAG, "Fetch SSID : " + networkName);
            Log.d(TAG, "SSID : " + ssid);

            if (!TextUtils.isEmpty(ssid) && !TextUtils.isEmpty(networkName) && ssid.equals(networkName)) {

                try {
                    Thread.sleep(2500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                Log.e(TAG, "Removed wifiConnectionFailedTask");
                handler.removeCallbacks(wifiConnectionFailedTask);
                deviceName = ssid;
                getCapabilitiesFromDevice();

            } else {

                handler.removeCallbacks(task);
                checkDeviceConnection(ssid);
            }
        }
    }

    FetchNetworkName task;

    private void checkDeviceConnection(String ssid) {

        task = new FetchNetworkName(ssid);
        handler.postDelayed(task, 2000);
    }
}
