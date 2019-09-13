package com.espressif.ui.activities;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.espressif.AppConstants;
import com.espressif.provision.Provision;
import com.espressif.provision.R;

public class ProofOfPossessionActivity extends AppCompatActivity {

    private static final String TAG = "Espressif::" + ProofOfPossessionActivity.class.getSimpleName();

    private Button btnNext;
    private TextView textDeviceName;
    private EditText etDeviceKey;
    private String securityVersion, transportVersion;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pop);

        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle(R.string.title_activity_pop);
        setSupportActionBar(toolbar);

        btnNext = findViewById(R.id.btn_next);
        textDeviceName = findViewById(R.id.device_name);
        etDeviceKey = findViewById(R.id.et_pop);

        String deviceName = getIntent().getStringExtra(AppConstants.KEY_DEVICE_NAME);
        transportVersion = getIntent().getStringExtra(Provision.CONFIG_TRANSPORT_KEY);
        securityVersion = getIntent().getStringExtra(Provision.CONFIG_SECURITY_KEY);

        textDeviceName.setText(deviceName);
        btnNext.setOnClickListener(nextBtnClickListener);
        String key = getString(R.string.proof_of_possesion);

        if (!TextUtils.isEmpty(key)) {

            etDeviceKey.setText(key);
            etDeviceKey.setSelection(etDeviceKey.getText().length());
        }
    }

    @Override
    public void onBackPressed() {
        BLEProvisionLanding.isBleWorkDone = true;
        super.onBackPressed();
    }

    private View.OnClickListener nextBtnClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            final String pop = etDeviceKey.getText().toString();
            Log.d(TAG, "POP : " + pop);

            if (transportVersion.equals(Provision.CONFIG_TRANSPORT_WIFI)) {

                if (ProvisionLanding.deviceCapabilities.contains("wifi_scan")) {
                    goToWiFiScanListActivity();
                } else {
                    goToProvisionActivity();
                }

            } else if (transportVersion.equals(Provision.CONFIG_TRANSPORT_BLE)) {

                if (BLEProvisionLanding.bleTransport.deviceCapabilities.contains("wifi_scan")) {
                    goToWiFiScanListActivity();
                } else {
                    goToProvisionActivity();
                }
            }
        }
    };

    private void goToWiFiScanListActivity() {

        Intent launchWiFiScanList = new Intent(getApplicationContext(), WiFiScanActivity.class);
        launchWiFiScanList.putExtras(getIntent());
        launchWiFiScanList.putExtra(AppConstants.KEY_PROOF_OF_POSSESSION, etDeviceKey.getText().toString());
        startActivity(launchWiFiScanList);
        finish();
    }

    private void goToProvisionActivity() {

        Intent launchProvisionInstructions = new Intent(getApplicationContext(), ProvisionActivity.class);
        launchProvisionInstructions.putExtras(getIntent());
        launchProvisionInstructions.putExtra(AppConstants.KEY_PROOF_OF_POSSESSION, etDeviceKey.getText().toString());
        startActivity(launchProvisionInstructions);
        finish();
    }
}
