package com.espressif.ui.adapters;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.core.content.ContextCompat;

import com.espressif.provisioning.ESPConstants;
import com.espressif.provisioning.WiFiAccessPoint;
import com.espressif.wifi_provisioning.R;

import java.util.ArrayList;

public class WiFiListAdapter extends ArrayAdapter<WiFiAccessPoint> {

    private Context context;
    private ArrayList<WiFiAccessPoint> wifiApList;

    public WiFiListAdapter(Context context, int resource, ArrayList<WiFiAccessPoint> wifiList) {
        super(context, resource, wifiList);
        this.context = context;
        this.wifiApList = wifiList;
    }

    public View getView(int position, View convertView, ViewGroup parent) {

        WiFiAccessPoint wiFiAccessPoint = wifiApList.get(position);

        //get the inflater and inflate the XML layout for each item
        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Activity.LAYOUT_INFLATER_SERVICE);
        View view = inflater.inflate(R.layout.item_wifi_access_point, null);

        TextView wifiNameText = view.findViewById(R.id.tv_wifi_name);
        ImageView rssiImage = view.findViewById(R.id.iv_wifi_rssi);
        ImageView lockImage = view.findViewById(R.id.iv_wifi_security);

        wifiNameText.setText(wiFiAccessPoint.getWifiName());
        rssiImage.setImageLevel(getRssiLevel(wiFiAccessPoint.getRssi()));

        if (wiFiAccessPoint.getSecurity() == ESPConstants.WIFI_OPEN) {
            lockImage.setVisibility(View.GONE);
        } else {
            lockImage.setVisibility(View.VISIBLE);
        }

        if (wiFiAccessPoint.getWifiName().equals(context.getString(R.string.join_other_network))) {

            wifiNameText.setTextColor(ContextCompat.getColor(context.getApplicationContext(), R.color.colorPrimary));
            rssiImage.setVisibility(View.VISIBLE);
            rssiImage.setImageResource(R.drawable.ic_right_arrow);
        }

        return view;
    }

    private int getRssiLevel(int rssiValue) {

        if (rssiValue > -50) {
            return 3;
        } else if (rssiValue >= -60) {
            return 2;
        } else if (rssiValue >= -67) {
            return 1;
        } else {
            return 0;
        }
    }
}
