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
package com.espressif.ui.activities;

import android.Manifest;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Vibrator;
import android.provider.Settings;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.Spannable;
import android.text.SpannableStringBuilder;
import android.util.Log;
import android.view.HapticFeedbackConstants;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.espressif.AppConstants;
import com.espressif.EspApplication;
import com.espressif.provision.BuildConfig;
import com.espressif.provision.Provision;
import com.espressif.provision.R;
import com.espressif.provision.transport.ResponseListener;
import com.espressif.provision.transport.SoftAPTransport;
import com.espressif.provision.transport.Transport;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;

public class ProvisionLanding extends AppCompatActivity {

    private static final String TAG = "Espressif::" + ProvisionLanding.class.getSimpleName();

    private String currentSSID;
    private String deviceNamePrefix;
    private String securityVersion;
    public static ArrayList<String> deviceCapabilities;
    private SharedPreferences sharedPreferences;

    private RelativeLayout prefixLayout;
    private Button btnPrefix, btnConnect;
    private TextView textPrefix, textWiFiInstruction, textNoInternet;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_provision_landing);
        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.title_activity_connect_device);
        setSupportActionBar(toolbar);

        sharedPreferences = getSharedPreferences(AppConstants.ESP_PREFERENCES, Context.MODE_PRIVATE);
        deviceNamePrefix = sharedPreferences.getString(AppConstants.KEY_WIFI_NETWORK_NAME_PREFIX, "");
        securityVersion = getIntent().getStringExtra(Provision.CONFIG_SECURITY_KEY);
        deviceCapabilities = new ArrayList<>();

        initViews();
        updateUi();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        if (requestCode == Provision.REQUEST_PROVISIONING_CODE && resultCode == RESULT_OK) {

            setResult(resultCode);
            finish();

        } else if (requestCode == 100) {

            updateUi();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {

        switch (requestCode) {

            case Provision.REQUEST_PERMISSIONS_CODE: {

                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {

                    updateUi();
                }
            }
            break;
        }
    }

    View.OnClickListener btnConnectClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            Vibrator vib = (Vibrator) getSystemService(VIBRATOR_SERVICE);
            vib.vibrate(HapticFeedbackConstants.VIRTUAL_KEY);

            deviceNamePrefix = sharedPreferences.getString(AppConstants.KEY_WIFI_NETWORK_NAME_PREFIX, "");
            currentSSID = fetchWifiSSID();

            if (currentSSID != null && currentSSID.startsWith(deviceNamePrefix)) {

                String baseUrl = getIntent().getStringExtra(Provision.CONFIG_BASE_URL_KEY);
                Transport transport = new SoftAPTransport(baseUrl);
                String tempData = "ESP";

                transport.sendConfigData(AppConstants.HANDLER_PROTO_VER, tempData.getBytes(), new ResponseListener() {

                    @Override
                    public void onSuccess(byte[] returnData) {

                        String data = new String(returnData, StandardCharsets.UTF_8);
                        Log.d(TAG, "Value : " + data);

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

                        if (!deviceCapabilities.contains("no_pop") && securityVersion.equals(Provision.CONFIG_SECURITY_SECURITY1)) {

                            goToPopActivity();

                        } else if (deviceCapabilities.contains("wifi_scan")) {

                            goToWifiScanListActivity();

                        } else {

                            goToProvisionActivity();
                        }
                    }

                    @Override
                    public void onFailure(Exception e) {
                        e.printStackTrace();
                    }
                });

            } else {
                startActivityForResult(new Intent(Settings.ACTION_WIFI_SETTINGS), 100);
            }
        }
    };

    private View.OnClickListener btnPrefixChangeClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            Vibrator vib = (Vibrator) getSystemService(VIBRATOR_SERVICE);
            vib.vibrate(HapticFeedbackConstants.VIRTUAL_KEY);
            askForPrefix();
        }
    };

    private void initViews() {

        prefixLayout = findViewById(R.id.prefix_layout);
        btnPrefix = findViewById(R.id.btn_change_prefix);
        textPrefix = findViewById(R.id.prefix_value);
        btnConnect = findViewById(R.id.connect_button);
        textWiFiInstruction = findViewById(R.id.start_provisioning_message);
        textNoInternet = findViewById(R.id.no_internet_note);

        // Set visibility of Prefix layout
        if (BuildConfig.IS_ALLOWED_FILTERING_BY_PREFIX) {

            prefixLayout.setVisibility(View.VISIBLE);

        } else {
            prefixLayout.setVisibility(View.GONE);
        }

        textPrefix.setText(deviceNamePrefix);
        btnConnect.setOnClickListener(btnConnectClickListener);
        btnPrefix.setOnClickListener(btnPrefixChangeClickListener);
    }

    private void updateUi() {

        ((EspApplication) getApplicationContext()).enableOnlyWifiNetwork();

        currentSSID = fetchWifiSSID();
        Log.d(TAG, "SSID : " + currentSSID);

        if (currentSSID != null && (currentSSID.startsWith(deviceNamePrefix) || currentSSID.equals(deviceNamePrefix))) {
            btnConnect.setText(R.string.connected_to_device_action);
            textWiFiInstruction.setText(R.string.connected_to_device_instructions);
            textNoInternet.setVisibility(View.VISIBLE);
        } else {
            btnConnect.setText(R.string.connect_to_device_action);
            SpannableStringBuilder instructions = getWifiInstructions();
            textWiFiInstruction.setText(instructions);
            textNoInternet.setVisibility(View.GONE);
        }
    }

    private String fetchWifiSSID() {

        String ssid = null;
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED ||
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

        deviceNamePrefix = sharedPreferences.getString(AppConstants.KEY_WIFI_NETWORK_NAME_PREFIX, "");
        String instructions = getResources().getString(R.string.connect_to_device_instructions);
        SpannableStringBuilder str = new SpannableStringBuilder(instructions +
                " : " +
                "\n" +
                deviceNamePrefix +
                "XXXXXX");
        int startIndex = instructions.length() + 3;
        str.setSpan(new android.text.style.StyleSpan(android.graphics.Typeface.BOLD),
                startIndex,
                str.length(),
                Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        return str;
    }

    private void askForPrefix() {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setCancelable(true);

        LayoutInflater layoutInflaterAndroid = LayoutInflater.from(this);
        View view = layoutInflaterAndroid.inflate(R.layout.dialog_prefix, null);
        builder.setView(view);
        final EditText etPrefix = view.findViewById(R.id.et_prefix);
        etPrefix.setText(deviceNamePrefix);
        etPrefix.setSelection(etPrefix.getText().length());

        // Set up the buttons
        builder.setPositiveButton(R.string.btn_save, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {

                String prefix = etPrefix.getText().toString();

                if (prefix != null) {
                    prefix = prefix.trim();
                }

                SharedPreferences.Editor editor = sharedPreferences.edit();
                editor.putString(AppConstants.KEY_WIFI_NETWORK_NAME_PREFIX, prefix);
                editor.apply();
                deviceNamePrefix = prefix;
                textPrefix.setText(prefix);
                updateUi();
            }
        });

        builder.setNegativeButton(R.string.btn_cancel, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });

        builder.show();
    }

    private void goToPopActivity() {

        Intent popIntent = new Intent(getApplicationContext(), ProofOfPossessionActivity.class);
        popIntent.putExtra(AppConstants.KEY_DEVICE_NAME, currentSSID);
        popIntent.putExtras(getIntent());
        startActivity(popIntent);
    }

    private void goToWifiScanListActivity() {

        Intent wifiListIntent = new Intent(getApplicationContext(), WiFiScanActivity.class);
        wifiListIntent.putExtras(getIntent());
        wifiListIntent.putExtra(AppConstants.KEY_PROOF_OF_POSSESSION, "");
        startActivity(wifiListIntent);
    }

    private void goToProvisionActivity() {

        Intent launchProvisionInstructions = new Intent(getApplicationContext(), ProvisionActivity.class);
        launchProvisionInstructions.putExtras(getIntent());
        launchProvisionInstructions.putExtra(AppConstants.KEY_PROOF_OF_POSSESSION, "");
        startActivityForResult(launchProvisionInstructions, Provision.REQUEST_PROVISIONING_CODE);
    }
}
