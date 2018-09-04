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

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.provider.Settings;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.Spannable;
import android.text.SpannableStringBuilder;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.espressif.provision.Provision;
import com.espressif.provision.R;

public class ProvisionLanding extends AppCompatActivity {
    private static final String TAG = "Espressif::" + ProvisionLanding.class.getSimpleName();

    private String currentSSID;
    private String wifiAPPrefix;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_provision_landing);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.connect_to_device_title);
        setSupportActionBar(toolbar);

        wifiAPPrefix = getIntent().getStringExtra(Provision.CONFIG_WIFI_AP_KEY);
        if(wifiAPPrefix == null) {
            wifiAPPrefix = "ESP-Alexa-";
        }

        TextView wifiInstructions = findViewById(R.id.start_provisioning_message);
        Button connectButton = findViewById(R.id.connect_button);
        TextView noInternetInstructions = findViewById(R.id.no_internet_note);

        currentSSID = this.fetchWifiSSID();
        if(currentSSID != null && (currentSSID.startsWith(wifiAPPrefix) || currentSSID.equals(wifiAPPrefix))) {
            connectButton.setText(R.string.connected_to_device_action);
            wifiInstructions.setText(R.string.connected_to_device_instructions);
            noInternetInstructions.setVisibility(View.VISIBLE);
        } else {
            connectButton.setText(R.string.connect_to_device_action);
            SpannableStringBuilder instructions = getWifiInstructions();
            wifiInstructions.setText(instructions);
            noInternetInstructions.setVisibility(View.GONE);
        }

        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                currentSSID = fetchWifiSSID();

                if(currentSSID != null && currentSSID.startsWith(wifiAPPrefix)) {
                    Intent launchProvisionInstructions = new Intent(getApplicationContext(), ProvisionActivity.class);
                    launchProvisionInstructions.putExtras(getIntent());
                    startActivityForResult(launchProvisionInstructions, Provision.REQUEST_PROVISIONING_CODE);
                }
                else {
                    startActivityForResult(new Intent(Settings.ACTION_WIFI_SETTINGS), 100);
                }
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == Provision.REQUEST_PROVISIONING_CODE &&
                resultCode == RESULT_OK) {
            setResult(resultCode);
            finish();
        } else if (requestCode == 100) {
            Button connectButton = findViewById(R.id.connect_button);
            TextView wifiInstructions = findViewById(R.id.start_provisioning_message);
            TextView noInternetInstructions = findViewById(R.id.no_internet_note);
            currentSSID = this.fetchWifiSSID();
            if(currentSSID != null && (currentSSID.startsWith(wifiAPPrefix) || currentSSID.equals(wifiAPPrefix))) {
                connectButton.setText(R.string.connected_to_device_action);
                wifiInstructions.setText(R.string.connected_to_device_instructions);
                noInternetInstructions.setVisibility(View.VISIBLE);
            } else {
                connectButton.setText(R.string.connect_to_device_action);
                SpannableStringBuilder instructions = getWifiInstructions();
                wifiInstructions.setText(instructions);
                noInternetInstructions.setVisibility(View.GONE);
            }

        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case Provision.REQUEST_PERMISSIONS_CODE: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Button connectButton = findViewById(R.id.connect_button);
                    TextView wifiInstructions = findViewById(R.id.start_provisioning_message);
                    TextView noInternetInstructions = findViewById(R.id.no_internet_note);
                    currentSSID = this.fetchWifiSSID();
                    if(currentSSID != null && (currentSSID.startsWith(wifiAPPrefix) || currentSSID.equals(wifiAPPrefix))) {
                        connectButton.setText(R.string.connected_to_device_action);
                        wifiInstructions.setText(R.string.connected_to_device_instructions);
                        noInternetInstructions.setVisibility(View.VISIBLE);
                    } else {
                        connectButton.setText(R.string.connect_to_device_action);
                        SpannableStringBuilder instructions = getWifiInstructions();
                        wifiInstructions.setText(instructions);
                        noInternetInstructions.setVisibility(View.GONE);
                    }
                }
            }
            break;
        }
    }

    private String fetchWifiSSID() {
        String ssid = null;
        if(ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED ||
                ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED) {
            ConnectivityManager cm = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkInfo networkInfo = cm.getActiveNetworkInfo();
            if (networkInfo == null) {
                return null;
            }

            if (networkInfo.isConnected()) {
                final WifiManager wifiManager = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
                final WifiInfo connectionInfo = wifiManager.getConnectionInfo();
                if (connectionInfo != null) {
                    ssid = connectionInfo.getSSID();
                    ssid = ssid.replaceAll("^\"|\"$", "");
                }
            }
        } else {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, Provision.REQUEST_PERMISSIONS_CODE);
        }
        return ssid;
    }

    private SpannableStringBuilder getWifiInstructions() {
        String instructions = getResources().getString(R.string.connect_to_device_instructions);
        SpannableStringBuilder str = new SpannableStringBuilder(  instructions +
                " : " +
                "\n" +
                wifiAPPrefix +
                "xxxx");
        int startIndex = instructions.length() + 3;
        str.setSpan(new android.text.style.StyleSpan(android.graphics.Typeface.BOLD),
                startIndex,
                str.length(),
                Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        return str;
    }
}
