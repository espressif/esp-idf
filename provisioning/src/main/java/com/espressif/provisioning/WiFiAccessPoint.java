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

import android.os.Parcel;
import android.os.Parcelable;

/**
 * This class holds information related to WiFi Access point like SSID, password, security type and RSSI.
 */
public class WiFiAccessPoint implements Parcelable {

    private String wifiName; // SSID
    private int rssi;
    private int security;
    private String password;

    public String getWifiName() {
        return wifiName;
    }

    public void setWifiName(String wifiName) {
        this.wifiName = wifiName;
    }

    public int getRssi() {
        return rssi;
    }

    public void setRssi(int rssi) {
        this.rssi = rssi;
    }

    public int getSecurity() {
        return security;
    }

    public void setSecurity(int security) {
        this.security = security;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public WiFiAccessPoint() {
    }

    private WiFiAccessPoint(Parcel in) {

        wifiName = in.readString();
        rssi = in.readInt();
        security = in.readInt();
        password = in.readString();
    }

    public static final Creator<WiFiAccessPoint> CREATOR = new Creator<WiFiAccessPoint>() {
        @Override
        public WiFiAccessPoint createFromParcel(Parcel in) {
            return new WiFiAccessPoint(in);
        }

        @Override
        public WiFiAccessPoint[] newArray(int size) {
            return new WiFiAccessPoint[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {

        dest.writeString(wifiName);
        dest.writeInt(rssi);
        dest.writeInt(security);
        dest.writeString(password);
    }
}
