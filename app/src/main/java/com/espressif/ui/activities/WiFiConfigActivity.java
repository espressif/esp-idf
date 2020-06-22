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

package com.espressif.ui.activities;

import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.cardview.widget.CardView;

import com.espressif.AppConstants;
import com.espressif.provisioning.ESPProvisionManager;
import com.espressif.wifi_provisioning.R;

public class WiFiConfigActivity extends AppCompatActivity {

    private static final String TAG = WiFiConfigActivity.class.getSimpleName();

    private TextView tvTitle, tvBack, tvCancel;
    private CardView btnNext;
    private TextView txtNextBtn;

    private EditText etSsid, etPassword;
    private ESPProvisionManager provisionManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wifi_config);

        provisionManager = ESPProvisionManager.getInstance(getApplicationContext());
        initViews();
    }

    @Override
    public void onBackPressed() {
        provisionManager.getEspDevice().disconnectDevice();
        super.onBackPressed();
    }

    private View.OnClickListener nextBtnClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            String ssid = etSsid.getText().toString();
            String password = etPassword.getText().toString();

            if (TextUtils.isEmpty(ssid)) {
                etSsid.setError(getString(R.string.error_ssid_empty));
                return;
            }

            goToProvisionActivity(ssid, password);
        }
    };

    private View.OnClickListener cancelBtnClickListener = new View.OnClickListener() {

        @Override
        public void onClick(View v) {

            finish();
        }
    };

    private void initViews() {

        tvTitle = findViewById(R.id.main_toolbar_title);
        tvBack = findViewById(R.id.btn_back);
        tvCancel = findViewById(R.id.btn_cancel);
        etSsid = findViewById(R.id.et_ssid_input);
        etPassword = findViewById(R.id.et_password_input);

        String deviceName = provisionManager.getEspDevice().getDeviceName();
        if (!TextUtils.isEmpty(deviceName)) {
            String msg = String.format(getString(R.string.setup_instructions), deviceName);
            TextView tvInstructionMsg = findViewById(R.id.setup_instructions_view);
            tvInstructionMsg.setText(msg);
        }

        tvTitle.setText(R.string.title_activity_wifi_config);
        tvBack.setVisibility(View.GONE);
        tvCancel.setVisibility(View.VISIBLE);
        tvCancel.setOnClickListener(cancelBtnClickListener);

        btnNext = findViewById(R.id.btn_next);
        txtNextBtn = findViewById(R.id.text_btn);
        txtNextBtn.setText(R.string.btn_next);
        btnNext.setOnClickListener(nextBtnClickListener);
    }

    private void goToProvisionActivity(String ssid, String password) {

        finish();
        Intent provisionIntent = new Intent(getApplicationContext(), ProvisionActivity.class);
        provisionIntent.putExtras(getIntent());
        provisionIntent.putExtra(AppConstants.KEY_WIFI_SSID, ssid);
        provisionIntent.putExtra(AppConstants.KEY_WIFI_PASSWORD, password);
        startActivity(provisionIntent);
    }
}
