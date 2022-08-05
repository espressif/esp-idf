# Demo Function

This demo demonstrates the fast provisioning of ESP BLE Mesh network and how to use the EspBleMesh app to control an individual provisioned node or all the provisioned nodes.
 
A video of this example can be seen from [here](https://dl.espressif.com/BLE/public/ESP32_BLE_Mesh_Fast_Provision.mp4).

# What You Need 

* [EspBleMesh App for Android](https://github.com/EspressifApp/EspBLEMeshForAndroid/releases/tag/v1.1.0)
* [esp-idf](https://github.com/espressif/esp-idf)
* ESP32 Development Boards

> Note:
> 
> 1. Please flash the [`fast_prov_server`](https://github.com/espressif/esp-idf/tree/release/v4.2/examples/bluetooth/esp_ble_mesh/ble_mesh_fast_provision/fast_prov_server) to the development boards first;
> 2. To have a better understanding of the performance of the BLE Mesh network, we recommend that at least 3 devices should be added in your network.
> 3. We recommend that you solder LED indicators if your development board does not come with lights. 
> 4. Please check the type of board and LED pin definition enabled in `Example BLE Mesh Config` by running `idf.py menuconfig`

![Board](images/device.png)


# Flash and Monitor

1.	Enter the directory: 
examples/bluetooth/esp_ble_mesh/ble_mesh_fast_provision/fast_prov_server
2.	Make sure that the `IDF_PATH` environment variable was set in accordance with your current IDF path
3. Check the version of your toolchain. Version 4.1 or newer should be used.
 
![Checkenvironment](images/picture1.png)

4. Run `idf.py -p PORT flash` to compile codes and flash the codes to the device.

![compiledcode](images/picture2.png)

> Note: 
> 
> Please click on the Exit button if you see the following windows.
                   

5. Please establish a connection between your device and PC, using the correct serial number, if you want to monitor the operation of this device on PC. 

# How to Use the App

Please launch the `EspBleMesh` app, and follow the steps described below to establish a BLE Mesh network and control any individual node or all the nodes.

![App steps](images/app_ble.png)
1. Click on the upper left corner to see more options;
2. Click on **Provisioning** to scan nearby unprovisioned devices;
3. Choose any unprovisioned devices in the scanned list;
4. Enter the number of devices you want to add in your mesh network;
> Note: 
> 
> If you only want to use the normal provisioning feature，You don't check the option for fast provisioning.
5. Wait until all the devices are provisioned;
6. Click on the upper left corner to see more options;
7. Click on **Fast Provisioned** to see all the provisioned devices;
8. Control your devices.

> Note: 
> 
> Please disable your Bluetooth function on your phone, enable it and try again, if you have encountered any connection issues.


# Procedure

## Role

* Phone - Top Provisioner
* The device that has been provisioned by Phone - Primary Provisioner
* Devices that have been provisioned and changed to the role of a provisioner - Temporary Provisioner
* Devices that have been provisioned but not changed to the role of a provisioner - Node

## Interaction

![Interaction](images/time.png)
1. The Top Provisioner configures the first device to access the network with the GATT bearer.
2. The Top Provisioner sends the `send_config_appkey_add` message to allocate the Appkey to this device. 
3. The Top Provisioner sends the `send_fast_prov_info_set` message to provide the necessary information so the device can be changed to a Primary Provisioner.
4. The device calls the `esp_ble_mesh_set_fast_prov_action` API to change itself into the role of a Primary Provisioner and disconnects with the Top Provisioner.
5. The Primary Provisioner sends the `send_config_appkey_add` message to allocate the Appkey to an other device.
6. The Primary Provisioner sends the `send_fast_prov_info_set` message to provide the necessary information so the device can be changed to a Temporary Provisioner.
7. The device calls the `esp_ble_mesh_set_fast_prov_action` API to change itself into the role of a Temporary Provisioner and starts its address timer.
8. The Temporary Provisioner collects the addresses of nodes that it has provisioned and sends these addresses to the Primary Provisioner, when its address timer times out, which indicates the Temporary Provisioner hasn't provisioned any devices for 10s.
9. The Primary Provisioner reconnects to the Top Provisioner when its address timer times out, which indicates the Primary Provisioner hasn't received any messages from the Temporary Provisioners for 10s.
10. The Top Provisioner sends the `node_adress_Get` message automatically after reconnecting with the Primary Provisioner.
11. At this point, the Top Provisioner is able to control any nodes in the BLE Mesh Network.

> Note:
> 
> The nodes in the BLE Mesh Network only disable its provisioner functionality after it has been controlled by the Top Provisioner for at least one time. 

