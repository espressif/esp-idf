package com.espressif;

import android.app.Application;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.os.Build;
import android.util.Log;

public class EspApplication extends Application {

    private static final String TAG = EspApplication.class.getSimpleName();

    private ConnectivityManager connectivityManager;
    private ConnectivityManager.NetworkCallback networkCallback;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "ESP Application is created");
    }

    public void enableOnlyWifiNetwork() {

        if (Build.VERSION.SDK_INT >= 21) {

            connectivityManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);

            NetworkRequest.Builder request = new NetworkRequest.Builder();
            request.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);

            networkCallback = new ConnectivityManager.NetworkCallback() {

                @Override
                public void onAvailable(Network network) {

                    if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
                        connectivityManager.setProcessDefaultNetwork(network);
                    } else {
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                            Log.e(TAG, "bindProcessToNetwork : " + connectivityManager.bindProcessToNetwork(network));
                            connectivityManager.bindProcessToNetwork(network);
                        }
                    }
                }
            };
            connectivityManager.registerNetworkCallback(request.build(), networkCallback);
        }
    }

    public void disableOnlyWifiNetwork() {

        Log.d(TAG, "disableOnlyWifiNetwork()");

        if (connectivityManager != null) {
            if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
                connectivityManager.setProcessDefaultNetwork(null);
            } else {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    connectivityManager.bindProcessToNetwork(null);
                }
            }
            connectivityManager.unregisterNetworkCallback(networkCallback);
        }
    }
}
