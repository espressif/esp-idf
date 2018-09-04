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
package com.espressif.ui;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.espressif.provision.BuildConfig;
import com.espressif.provision.Provision;
import com.espressif.provision.R;
import com.espressif.provision.session.Session;
import com.espressif.provision.security.Security;
import com.espressif.provision.security.Security0;
import com.espressif.provision.security.Security1;
import com.espressif.provision.transport.BLETransport;
import com.espressif.provision.transport.SoftAPTransport;
import com.espressif.provision.transport.Transport;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.UUID;

import espressif.Constants;
import espressif.WifiConstants;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "Espressif::" + MainActivity.class.getSimpleName();
    private static final String SSID = "Tejonidhi";
    private static final String PASSPHRASE = "Zoey2611";

    private String POP, BASE_URL, NETWORK_NAME_PREFIX, SERVICE_UUID, SESSION_UUID, CONFIG_UUID, DEVICE_NAME_PREFIX;

    private void ProvisionWithDataApis() {
        final Activity thisActivity = this;
        final Security security;
        if(BuildConfig.FLAVOR_security.equals("sec1")) {
            security = new Security1(POP);
        } else {
            security = new Security0();
        }

        if(BuildConfig.FLAVOR_transport.equals("ble")) {
            HashMap<String, String> configUUIDMap = new HashMap<>();
            configUUIDMap.put(Provision.PROVISIONING_CONFIG_PATH, CONFIG_UUID);

            final BLETransport bleTransport = new BLETransport(this,
                    UUID.fromString(SERVICE_UUID),
                    UUID.fromString(SESSION_UUID),
                    configUUIDMap,
                    DEVICE_NAME_PREFIX,
                    3000);
            final Transport transport = bleTransport;
            bleTransport.scan(new BLETransport.BLETransportListener() {
                @Override
                public void onPeripheralsFound(ArrayList<BluetoothDevice> devices) {
                    bleTransport.connect(devices.get(0));
                }

                @Override
                public void onPeripheralsNotFound() {
                    Log.d(TAG, "Peripherals not found");
                }

                @Override
                public void onPeripheralConfigured(BluetoothDevice device) {
                    Log.d(TAG, "Configured BLE endpoints.");
                    provisionSession(transport, security, thisActivity);
                }

                @Override
                public void onPeripheralNotConfigured(BluetoothDevice device) {
                    Log.d(TAG, "Peripherals cannot be configured for : " + device.getName());
                }

                @Override
                public void onPeripheralDisconnected(Exception e) {
                    Log.d(TAG, "BLE device disconnected : ", e);
                }

                @Override
                public void onFailure(final Exception e) {
                    Log.d(TAG, "Failed to connect to BLE endpoint");
                    thisActivity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(thisActivity,
                                    "Provisioning Failed : " + e.getMessage(),
                                    Toast.LENGTH_LONG)
                                    .show();
                            toggleFormState(true);
                        }
                    });

                }
            });
        } else {
            final Transport transport = new SoftAPTransport(BASE_URL);
            this.provisionSession(transport, security, thisActivity);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        final Activity thisActivity = this;
        Button provision = findViewById(R.id.provision_button);
        final String productDSN = generateProductDSN();

        BASE_URL = getResources().getString(R.string.wifi_base_url);
        NETWORK_NAME_PREFIX = getResources().getString(R.string.wifi_network_name_prefix);
        POP =  getResources().getString(R.string.proof_of_possesion);
        SERVICE_UUID = getResources().getString(R.string.ble_service_uuid);
        SESSION_UUID = getResources().getString(R.string.ble_session_uuid);
        CONFIG_UUID = getResources().getString(R.string.ble_config_uuid);
        DEVICE_NAME_PREFIX = getResources().getString(R.string.ble_device_name_prefix);

        final String transportVersion, securityVersion;
        if(BuildConfig.FLAVOR_security.equals("sec1")) {
            securityVersion = Provision.CONFIG_SECURITY_SECURITY1;
        } else {
            securityVersion = Provision.CONFIG_SECURITY_SECURITY0;
        }

        if(BuildConfig.FLAVOR_transport.equals("ble")) {
            transportVersion = Provision.CONFIG_TRANSPORT_BLE;
        } else {
            transportVersion = Provision.CONFIG_TRANSPORT_WIFI;
        }

        provision.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                HashMap<String, String> config = new HashMap<>();
                config.put(Provision.CONFIG_TRANSPORT_KEY, transportVersion);
                config.put(Provision.CONFIG_SECURITY_KEY, securityVersion);

                config.put(Provision.CONFIG_PROOF_OF_POSSESSION_KEY, POP);
                config.put(Provision.CONFIG_BASE_URL_KEY, BASE_URL);
                config.put(Provision.CONFIG_WIFI_AP_KEY, NETWORK_NAME_PREFIX);

                config.put(BLETransport.SERVICE_UUID_KEY, SERVICE_UUID);
                config.put(BLETransport.SESSION_UUID_KEY, SESSION_UUID);
                config.put(BLETransport.CONFIG_UUID_KEY, CONFIG_UUID);
                config.put(BLETransport.DEVICE_NAME_PREFIX_KEY, DEVICE_NAME_PREFIX);
                Provision.showProvisioningUI(thisActivity, config);
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void toggleFormState(boolean isEnabled) {
        final Button provision = findViewById(R.id.provision_button);
        final View loadingIndicator = findViewById(R.id.progress_indicator);
        loadingIndicator.setVisibility(isEnabled ? View.GONE : View.VISIBLE);

        provision.setEnabled(isEnabled);
    }

    private String generateProductDSN() {
        return UUID.randomUUID().toString();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == Provision.REQUEST_PROVISIONING_CODE &&
                resultCode == RESULT_OK) {
            setResult(resultCode);
            finish();
        }
    }

    private void provisionSession(Transport transport,
                                  Security security,
                                  final Activity activity) {
        final Session session = new Session(transport, security);
        final Activity thisActivity = this;
        session.sessionListener = new Session.SessionListener() {
            @Override
            public void OnSessionEstablished() {
                Log.d(TAG, "Session established");
                final Provision provision = new Provision(session);
                provision.provisioningListener = new Provision.ProvisioningListener() {
                    @Override
                    public void OnApplyConfigurationsSucceeded() {
                        Log.d(TAG, "Configurations applied");
                        activity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(activity,
                                        "Configurations successfully applied",
                                        Toast.LENGTH_LONG)
                                        .show();
                            }
                        });
                    }

                    @Override
                    public void OnApplyConfigurationsFailed() {
                        Log.d(TAG, "Configurations cannot be applied");
                        activity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(activity,
                                        "Configurations cannot be applied",
                                        Toast.LENGTH_LONG)
                                        .show();
                            }
                        });
                    }

                    @Override
                    public void OnWifiConnectionStatusUpdated(final WifiConstants.WifiStationState newStatus,
                                                              final WifiConstants.WifiConnectFailedReason failedReason,
                                                              final Exception e) {
                        String statusText = "";
                        if (e != null) {
                            statusText = e.getMessage();
                        } else if (newStatus == WifiConstants.WifiStationState.Connected) {
                            statusText = thisActivity.getResources().getString(R.string.success_text);
                        } else if (newStatus == WifiConstants.WifiStationState.Disconnected) {
                            statusText = thisActivity.getResources().getString(R.string.wifi_disconnected_text);
                        } else {
                            statusText = "Device provisioning failed.\nReason : " +  failedReason + "\nPlease try again";
                        }
                        goToSuccessPage(statusText);
                    }

                    @Override
                    public void OnProvisioningFailed(final Exception e) {
                        activity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(activity,
                                        "Provisioning Failed : " + e.getMessage(),
                                        Toast.LENGTH_LONG)
                                        .show();
                                toggleFormState(true);
                            }
                        });
                    }
                };

                provision.configureWifi(SSID, PASSPHRASE, new Provision.ProvisionActionListener() {
                    @Override
                    public void onComplete(Constants.Status status, Exception e) {
                        provision.applyConfigurations(new Provision.ProvisionActionListener() {
                            @Override
                            public void onComplete(Constants.Status status, Exception e) {
                                Log.d(TAG, "Provisioning completed with status : " + status);
                            }
                        });
                    }
                });
            }

            @Override
            public void OnSessionEstablishFailed(Exception e) {
                activity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(activity,
                                "Cannot establish session",
                                Toast.LENGTH_LONG)
                                .show();
                        toggleFormState(true);
                    }
                });
            }
        };
        session.init(null);
    }

    private void goToSuccessPage(String statusText) {
        Intent goToSuccessPage = new Intent(getApplicationContext(), ProvisionSuccessActivity.class);
        goToSuccessPage.putExtra("status", statusText);
        startActivity(goToSuccessPage);
        this.setResult(RESULT_OK);
        this.finish();
    }
}
