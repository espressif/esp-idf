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
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
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

public class ProvisionActivity extends AppCompatActivity {
    private static final String TAG = "Espressif::" + ProvisionActivity.class.getSimpleName();
    public static BLETransport BLE_TRANSPORT = null;

    private String ssid;
    private String passphrase;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_provision);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.provision_activity_title);
        setSupportActionBar(toolbar);

        Intent intent = getIntent();
        final String pop = intent.getStringExtra(Provision.CONFIG_PROOF_OF_POSSESSION_KEY);
        final String baseUrl = intent.getStringExtra(Provision.CONFIG_BASE_URL_KEY);
        final String transportVersion = intent.getStringExtra(Provision.CONFIG_TRANSPORT_KEY);
        final String securityVersion = intent.getStringExtra(Provision.CONFIG_SECURITY_KEY);
        final String deviceUUID = intent.getStringExtra(BLETransport.SERVICE_UUID_KEY);
        final String sessionUUID = intent.getStringExtra(BLETransport.SESSION_UUID_KEY);
        final String configUUID = intent.getStringExtra(BLETransport.CONFIG_UUID_KEY);
        final String deviceNamePrefix = intent.getStringExtra(BLETransport.DEVICE_NAME_PREFIX_KEY);

        final EditText ssidInput = findViewById(R.id.ssid_input);
        ssidInput.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                ssid = charSequence.toString().trim();
                validateForm();
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });

        final EditText passphraseInput = findViewById(R.id.password_input);
        passphraseInput.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                passphrase = charSequence.toString().trim();
                validateForm();
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });

        final Button provision = findViewById(R.id.provision_button);
        final Activity thisActivity = this;
        provision.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toggleFormState(false);

                final Security security;
                if (securityVersion.equals(Provision.CONFIG_SECURITY_SECURITY1)) {
                    security = new Security1(pop);
                } else {
                    security = new Security0();
                }

                final Transport transport;
                if (transportVersion.equals(Provision.CONFIG_TRANSPORT_WIFI)) {
                    transport = new SoftAPTransport(baseUrl);
                    provision(transport, security);
                } else if (transportVersion.equals(Provision.CONFIG_TRANSPORT_BLE)) {
                    if(ProvisionActivity.BLE_TRANSPORT == null) {
                        HashMap<String, String> configUUIDMap = new HashMap<>();
                        configUUIDMap.put(Provision.PROVISIONING_CONFIG_PATH, configUUID);
                        final BLETransport bleTransport = new BLETransport(thisActivity,
                                UUID.fromString(deviceUUID),
                                UUID.fromString(sessionUUID),
                                configUUIDMap,
                                deviceNamePrefix,
                                3000);
                        transport = bleTransport;
                        bleTransport.scan(new BLETransport.BLETransportListener() {
                            @Override
                            public void onPeripheralsFound(ArrayList<BluetoothDevice> devices) {
                                bleTransport.connect(devices.get(0));
                            }

                            @Override
                            public void onPeripheralsNotFound() {
                                Log.d(TAG, "Peripherals not found!");
                                thisActivity.runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(thisActivity,
                                                "Peripherals not found!",
                                                Toast.LENGTH_LONG)
                                                .show();
                                        toggleFormState(true);
                                    }
                                });
                            }

                            @Override
                            public void onPeripheralConfigured(BluetoothDevice device) {
                                provision(transport, security);
                            }

                            @Override
                            public void onPeripheralNotConfigured(final BluetoothDevice device) {
                                thisActivity.runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(thisActivity,
                                                "Peripherals cannot be configured for : " + device.getName(),
                                                Toast.LENGTH_LONG)
                                                .show();
                                        toggleFormState(true);
                                    }
                                });
                            }

                            @Override
                            public void onPeripheralDisconnected(final Exception e) {
                                thisActivity.runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(thisActivity,
                                                "Bluetooth device disconnected.",
                                                Toast.LENGTH_LONG)
                                                .show();
                                        toggleFormState(true);
                                    }
                                });
                            }

                            @Override
                            public void onFailure(final Exception e) {
                                thisActivity.runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(thisActivity,
                                                "Bluetooth connection failed : " + e.getMessage(),
                                                Toast.LENGTH_LONG)
                                                .show();
                                        toggleFormState(true);
                                    }
                                });
                            }
                        });
                    } else {
                        provision(ProvisionActivity.BLE_TRANSPORT, security);
                    }
                }
            }
        });
    }

    private void provision(Transport transport, Security security) {
        final Activity thisActivity = this;
        final Session session = new Session(transport, security);
        session.sessionListener = new Session.SessionListener() {
            @Override
            public void OnSessionEstablished() {
                thisActivity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(thisActivity,
                                "Session Established",
                                Toast.LENGTH_SHORT)
                                .show();
                    }
                });

                final Provision provision = new Provision(session);
                provision.provisioningListener = new Provision.ProvisioningListener() {
                    @Override
                    public void OnApplyConfigurationsSucceeded() {
                        thisActivity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(thisActivity,
                                        "Configurations successfully applied",
                                        Toast.LENGTH_LONG)
                                        .show();
                            }
                        });
                    }

                    @Override
                    public void OnApplyConfigurationsFailed() {
                        thisActivity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(thisActivity,
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
                    public void OnProvisioningFailed(Exception e) {
                        thisActivity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(thisActivity,
                                        "Provisioning Failed",
                                        Toast.LENGTH_LONG)
                                        .show();
                                toggleFormState(true);
                            }
                        });
                        thisActivity.setResult(RESULT_CANCELED);
                        thisActivity.finish();
                    }
                };
                provision.configureWifi(ssid, passphrase, new Provision.ProvisionActionListener() {
                    @Override
                    public void onComplete(Constants.Status status, Exception e) {
                        provision.applyConfigurations(null);
                    }
                });
            }

            @Override
            public void OnSessionEstablishFailed(Exception e) {
                thisActivity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(thisActivity,
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

    private void validateForm() {
        Button provision = findViewById(R.id.provision_button);

        boolean enabled = this.ssid != null &&
                this.ssid.length() > 0;
        provision.setEnabled(enabled);
    }

    private void toggleFormState(boolean isEnabled) {
        final View loadingIndicator = findViewById(R.id.progress_indicator);
        final EditText ssidInput = findViewById(R.id.ssid_input);
        final EditText passphraseInput = findViewById(R.id.password_input);
        final Button provision = findViewById(R.id.provision_button);

        if (isEnabled) {
            loadingIndicator.setVisibility(View.GONE);
            provision.setEnabled(true);
            ssidInput.setEnabled(true);
            passphraseInput.setEnabled(true);
        } else {
            loadingIndicator.setVisibility(View.VISIBLE);
            provision.setEnabled(false);
            ssidInput.setEnabled(false);
            passphraseInput.setEnabled(false);
        }
    }

    private void goToSuccessPage(String statusText) {
        Intent goToSuccessPage = new Intent(getApplicationContext(), ProvisionSuccessActivity.class);
        goToSuccessPage.putExtra("status", statusText);
        startActivity(goToSuccessPage);
        this.setResult(RESULT_OK);
        this.finish();
    }
}
