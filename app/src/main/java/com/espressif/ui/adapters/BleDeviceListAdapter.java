package com.espressif.ui.adapters;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.espressif.wifi_provisioning.R;

import java.util.ArrayList;

public class BleDeviceListAdapter extends ArrayAdapter<BluetoothDevice> {

    private Context context;
    private ArrayList<BluetoothDevice> bluetoothDevices;

    public BleDeviceListAdapter(Context context, int resource, ArrayList<BluetoothDevice> bluetoothDevices) {
        super(context, resource, bluetoothDevices);
        this.context = context;
        this.bluetoothDevices = bluetoothDevices;
    }

    public View getView(int position, View convertView, ViewGroup parent) {

        BluetoothDevice btDevice = bluetoothDevices.get(position);

        //get the inflater and inflate the XML layout for each item
        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Activity.LAYOUT_INFLATER_SERVICE);
        View view = inflater.inflate(R.layout.item_ble_scan, null);

        TextView bleDeviceNameText = view.findViewById(R.id.tv_ble_device_name);
        bleDeviceNameText.setText(btDevice.getName());

        return view;
    }
}
