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
package com.espressif.provision.transport;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.ParcelUuid;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;

import com.espressif.provision.Provision;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Semaphore;

/**
 * Bluetooth implementation of the Transport protocol.
 *
 */
public class BLETransport implements Transport {
    public static final String SERVICE_UUID_KEY = "serviceUUID";
    public static final String SESSION_UUID_KEY = "sessionUUID";
    public static final String CONFIG_UUID_KEY = "configUUID";
    public static final String DEVICE_NAME_PREFIX_KEY = "deviceNamePrefix";
    private static final String TAG = "Espressif::" + BLETransport.class.getSimpleName();
    private final UUID sessionCharacteristicUuid;
    private Activity context;
    private UUID serviceUuid;
    private String deviceNamePrefix;
    private BluetoothAdapter bleAdapter;
    private ArrayList<BluetoothDevice> bluetoothDevices;
    private long scanTimeoutInMillis;
    private BluetoothDevice currentDevice;
    private BluetoothGatt bluetoothGatt;
    private BluetoothGattService service;
    private BluetoothGattCharacteristic sessionCharacteristic;
    private HashMap<String, String> configUUIDMap;
    private BLETransportListener transportListener;
    private ResponseListener currentResponseListener;
    private Semaphore transportToken;
    private ExecutorService dispatcherThreadPool;

    /***
     * Create BLETransport implementation
     * @param context
     * @param serviceUuid string representation of the BLE Service UUID
     * @param sessionUuid string representation of the BLE Session characteristic UUID
     * @param configUuidMap map of config paths and string representations of the BLE characteristic UUID
     * @param deviceNamePrefix device name prefix
     * @param scanTimeoutInMillis timeout in milliseconds for which BLE scan should happen
     */
    public BLETransport(Activity context,
                        UUID serviceUuid,
                        UUID sessionUuid,
                        HashMap<String, String> configUuidMap,
                        String deviceNamePrefix,
                        long scanTimeoutInMillis) {
        this.context = context;
        this.serviceUuid = serviceUuid;
        this.sessionCharacteristicUuid = sessionUuid;
        this.configUUIDMap = configUuidMap;
        this.deviceNamePrefix = deviceNamePrefix;
        this.transportToken = new Semaphore(1);
        this.dispatcherThreadPool = Executors.newSingleThreadExecutor();
        this.bluetoothDevices = new ArrayList<>();
        this.scanTimeoutInMillis = scanTimeoutInMillis;
    }

    /***
     * BLE implementation of Transport protocol
     * @param data data to be sent
     * @param listener listener implementation which receives events when response is received.
     */
    @Override
    public void sendSessionData(byte[] data, ResponseListener listener) {
        if(sessionCharacteristic != null) {
            try {
                this.transportToken.acquire();
                sessionCharacteristic.setValue(data);
                bluetoothGatt.writeCharacteristic(sessionCharacteristic);
                currentResponseListener = listener;
            } catch (Exception e) {
                e.printStackTrace();
                listener.onFailure(e);
                this.transportToken.release();
            }
        }
    }

    /***
     * BLE implementation of Transport protocol
     * @param path path of the config endpoint.
     * @param data config data to be sent
     * @param listener listener implementation which receives events when response is received.
     */
    @Override
    public void sendConfigData(String path, byte[] data, ResponseListener listener) {
        try {
            this.transportToken.acquire();
            BluetoothGattCharacteristic characteristic = service.getCharacteristic(UUID.fromString(configUUIDMap.get(path)));
            if (characteristic != null) {
                characteristic.setValue(data);
                bluetoothGatt.writeCharacteristic(characteristic);
                currentResponseListener = listener;
            } else {
                this.transportToken.release();
            }
        } catch (Exception e) {
            e.printStackTrace();
            listener.onFailure(e);
            this.transportToken.release();
        }
    }

    /***
     * Scan for BLE bluetoothDevices
     * @param transportListener listener implementation which will receive resulting events
     */
    public void scan(BLETransportListener transportListener) {
        this.transportListener = transportListener;
        this.scanForPeripherals();
    }

    /***
     * Connect to a BLE peripheral device.
     * @param bluetoothDevice The peripheral device
     */
    public void connect(BluetoothDevice bluetoothDevice) {
        this.currentDevice = bluetoothDevice;
        this.currentDevice.connectGatt(context, false, new BluetoothGattCallback() {
            @Override
            public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
                super.onConnectionStateChange(gatt, status, newState);
                if (status == BluetoothGatt.GATT_FAILURE) {
                    if(transportListener != null) {
                        transportListener.onFailure(new Exception("GATT failure in connection"));
                    }
                    return;
                } else if (status != BluetoothGatt.GATT_SUCCESS) {
                    return;
                }
                if (newState == BluetoothProfile.STATE_CONNECTED) {
                    gatt.discoverServices();
                } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                    if(transportListener != null) {
                        transportListener.onPeripheralDisconnected(new Exception("Bluetooth device disconnected"));
                    }
                }
            }

            @Override
            public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                super.onServicesDiscovered(gatt, status);
                if (status != BluetoothGatt.GATT_SUCCESS) {
                    return;
                }
                service = gatt.getService(serviceUuid);
                bluetoothGatt = gatt;
                bluetoothGatt.requestMtu(400);

                for (BluetoothGattCharacteristic characteristic:service.getCharacteristics()) {
                    characteristic.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                }

                sessionCharacteristic = service.getCharacteristic(sessionCharacteristicUuid);
                if(transportListener != null) {
                    if(sessionCharacteristic != null) {
                        transportListener.onPeripheralConfigured(currentDevice);
                    } else {
                        transportListener.onPeripheralNotConfigured(currentDevice);
                    }
                }
            }

            @Override
            public void onMtuChanged(BluetoothGatt gatt, int mtu, int status) {
                super.onMtuChanged(gatt, mtu, status);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    Log.d(TAG, "Supported MTU = " + mtu);
                }
            }

            @Override
            public void onCharacteristicRead(BluetoothGatt gatt,
                                             final BluetoothGattCharacteristic characteristic,
                                             int status) {
                super.onCharacteristicRead(gatt, characteristic, status);
                if(currentResponseListener != null) {
                    if (status == BluetoothGatt.GATT_SUCCESS) {
                        /**
                         * Need to dispatch this on another thread since the caller
                         * might decide to enqueue another send operation on success
                         * of the first.
                         */
                        final ResponseListener responseListener = currentResponseListener;
                        dispatcherThreadPool.submit(new Runnable() {
                            @Override
                            public void run() {
                                byte[] charValue = characteristic.getValue();
                                responseListener.onSuccess(charValue);
                            }
                        });
                        currentResponseListener = null;
                    } else {
                        currentResponseListener.onFailure(new Exception("Read from BLE failed"));
                    }
                }
                transportToken.release();
            }

            @Override
            public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
                super.onCharacteristicWrite(gatt, characteristic, status);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    bluetoothGatt.readCharacteristic(characteristic);
                } else {
                    if(currentResponseListener != null) {
                        currentResponseListener.onFailure(new Exception("Write to BLE failed"));
                    }
                    transportToken.release();
                }
            }
        });
    }

    /***
     * Disconnect from the current connected peripheral
     */
    public void disconnect() {
        if(this.bluetoothGatt != null) {
            this.bluetoothGatt.disconnect();
        }
    }

    private void scanForPeripherals () {
        final BluetoothManager bluetoothManager = (BluetoothManager) context.getSystemService(Context.BLUETOOTH_SERVICE);
        bleAdapter = bluetoothManager.getAdapter();
        bluetoothDevices.clear();
        if(!this.hasPermissions()) {
            if(transportListener != null) {
                transportListener.onFailure(new Exception("Not enough permissions to connect over BLE"));
            }
        } else {
            final BluetoothLeScanner scanner = bleAdapter.getBluetoothLeScanner();
            List<ScanFilter> filters = new ArrayList<>();
            ScanFilter scanFilter = new ScanFilter.Builder()
                    .setServiceUuid(new ParcelUuid(this.serviceUuid))
                    .build();
            filters.add(scanFilter);
            ScanSettings settings = new ScanSettings.Builder()
                    .setScanMode(ScanSettings.SCAN_MODE_LOW_POWER)
                    .build();

            final ScanCallback scanCallback = new ScanCallback() {
                @Override
                public void onScanResult(int callbackType, ScanResult result) {
                    boolean deviceExists = false;
                    Log.d(TAG, "Got scan result");

                    for(BluetoothDevice device:bluetoothDevices) {
                        if(device.equals(result.getDevice())) {
                            deviceExists = true;
                            break;
                        }
                    }

                    if (!deviceExists) {
                        String deviceName = result.getDevice().getName();
                        if(deviceName != null && deviceName.startsWith(deviceNamePrefix)) {
                            bluetoothDevices.add(result.getDevice());
                        }
                    }
                }

                @Override
                public void onBatchScanResults(List<ScanResult> results) {
                    super.onBatchScanResults(results);
                }

                @Override
                public void onScanFailed(int errorCode) {
                    super.onScanFailed(errorCode);
                    if(transportListener != null) {
                        transportListener.onFailure(new Exception("BLE connect failed with error code : " + errorCode));
                    }
                }
            };
            scanner.startScan(filters, settings, scanCallback);
            Handler someHandler = new Handler();
            someHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (bluetoothDevices.size() == 0 && transportListener != null) {
                        transportListener.onPeripheralsNotFound();
                    } else if (transportListener != null) {
                        transportListener.onPeripheralsFound(bluetoothDevices);
                    }
                    scanner.stopScan(scanCallback);
                    bluetoothDevices.clear();
                }
            }, this.scanTimeoutInMillis);
        }
    }

    private boolean hasPermissions() {
        if (bleAdapter == null || !bleAdapter.isEnabled()) {
            requestBluetoothEnable();
            return false;
        } else if (!hasLocationPermissions()) {
            requestLocationPermission();
            return false;
        }
        return true;
    }

    private void requestBluetoothEnable() {
        Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        context.startActivityForResult(enableBtIntent, Provision.REQUEST_ENABLE_BLE_CODE);
    }

    private boolean hasLocationPermissions() {
        return ContextCompat.checkSelfPermission(context,
                Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestLocationPermission() {
        ActivityCompat.requestPermissions(context,
                new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, 102);
    }

    /***
     * Listener which will receive events relating to BLE device scanning
     */
    public interface BLETransportListener {
        /***
         * Peripheral bluetoothDevices found with matching Service UUID
         * Callers should call the BLETransport.connect method with
         * one of the peripherals found here
         * @param devices
         */
        void onPeripheralsFound(ArrayList<BluetoothDevice> devices);

        /***
         * No peripherals found with matching Service UUID
         */
        void onPeripheralsNotFound();

        /***
         * Peripheral device configured.
         * This tells the caller that the connected BLE device is now configured
         * and can be provisioned
         * @param device
         */
        void onPeripheralConfigured(BluetoothDevice device);

        /***
         * Peripheral device could not be configured.
         * This tells the called that the connected device cannot be configured for provisioning
         * @param device
         */
        void onPeripheralNotConfigured(BluetoothDevice device);

        /***
         * Peripheral device disconnected
         * @param e
         */
        void onPeripheralDisconnected(Exception e);

        /***
         * Failed to scan for BLE bluetoothDevices
         * @param e
         */
        void onFailure(Exception e);
    }
}
