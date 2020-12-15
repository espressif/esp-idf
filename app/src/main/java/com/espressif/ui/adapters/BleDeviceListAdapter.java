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

package com.espressif.ui.adapters;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.espressif.ui.models.BleDevice;
import com.espressif.wifi_provisioning.R;

import java.util.ArrayList;

public class BleDeviceListAdapter extends ArrayAdapter<BleDevice> {

    private Context context;
    private ArrayList<BleDevice> bluetoothDevices;

    public BleDeviceListAdapter(Context context, int resource, ArrayList<BleDevice> bluetoothDevices) {
        super(context, resource, bluetoothDevices);
        this.context = context;
        this.bluetoothDevices = bluetoothDevices;
    }

    public View getView(int position, View convertView, ViewGroup parent) {

        BleDevice btDevice = bluetoothDevices.get(position);

        //get the inflater and inflate the XML layout for each item
        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Activity.LAYOUT_INFLATER_SERVICE);
        View view = inflater.inflate(R.layout.item_ble_scan, null);

        TextView bleDeviceNameText = view.findViewById(R.id.tv_ble_device_name);
        bleDeviceNameText.setText(btDevice.getName());

        return view;
    }
}
