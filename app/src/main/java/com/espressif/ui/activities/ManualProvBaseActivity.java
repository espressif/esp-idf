// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import com.espressif.AppConstants;
import com.espressif.provisioning.ESPConstants;
import com.espressif.provisioning.ESPProvisionManager;
import com.espressif.wifi_provisioning.R;

import org.greenrobot.eventbus.EventBus;
import org.json.JSONException;
import org.json.JSONObject;

public class ManualProvBaseActivity extends AppCompatActivity {

    private static final String TAG = ManualProvBaseActivity.class.getSimpleName();

    int securityType;
    ESPProvisionManager provisionManager;
    SharedPreferences sharedPreferences;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_provision_landing);
        securityType = getIntent().getIntExtra(AppConstants.KEY_SECURITY_TYPE, AppConstants.SEC_TYPE_DEFAULT);
        provisionManager = ESPProvisionManager.getInstance(getApplicationContext());
        sharedPreferences = getSharedPreferences(AppConstants.ESP_PREFERENCES, Context.MODE_PRIVATE);
        EventBus.getDefault().register(this);
    }

    @Override
    protected void onDestroy() {
        EventBus.getDefault().unregister(this);
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {

        if (provisionManager.getEspDevice() != null) {
            provisionManager.getEspDevice().disconnectDevice();
        }

        super.onBackPressed();
    }

    void setSecurityTypeFromVersionInfo() {

        String protoVerStr = provisionManager.getEspDevice().getVersionInfo();

        try {
            JSONObject jsonObject = new JSONObject(protoVerStr);
            JSONObject provInfo = jsonObject.getJSONObject("prov");

            if (provInfo != null) {
                if (provInfo.has("sec_ver")) {

                    int serVer = provInfo.optInt("sec_ver");
                    Log.d(TAG, "Security Version : " + serVer);

                    switch (serVer) {
                        case AppConstants.SEC_TYPE_0:
                            securityType = AppConstants.SEC_TYPE_0;
                            provisionManager.getEspDevice().setSecurityType(ESPConstants.SecurityType.SECURITY_0);
                            break;
                        case AppConstants.SEC_TYPE_1:
                            securityType = AppConstants.SEC_TYPE_1;
                            provisionManager.getEspDevice().setSecurityType(ESPConstants.SecurityType.SECURITY_1);
                            break;
                        case AppConstants.SEC_TYPE_2:
                        default:
                            securityType = AppConstants.SEC_TYPE_2;
                            provisionManager.getEspDevice().setSecurityType(ESPConstants.SecurityType.SECURITY_2);
                            if (TextUtils.isEmpty(provisionManager.getEspDevice().getUserName())) {
                                String userName = sharedPreferences.getString(AppConstants.KEY_USER_NAME, AppConstants.DEFAULT_USER_NAME);
                                provisionManager.getEspDevice().setUserName(userName);
                            }
                            break;
                    }
                } else {
                    if (securityType == AppConstants.SEC_TYPE_2) {
                        securityType = AppConstants.SEC_TYPE_1;
                        provisionManager.getEspDevice().setSecurityType(ESPConstants.SecurityType.SECURITY_1);
                    }
                }
            } else {
                Log.e(TAG, "proto-ver info is not available.");
            }
        } catch (JSONException e) {
            e.printStackTrace();
            Log.d(TAG, "Capabilities JSON not available.");
        }
    }
}
