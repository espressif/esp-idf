# SoftAP + HTTPD based Provisioning Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

`softap_prov` example demonstrates the implementation and integration of various IDF components for building a provisioning application.

For this example Wi-Fi SoftAP is chosen as the mode of transport, over which the provisioning related communication is to take place, between the device (to be provisioned) and the client (owner of the device). The provisioning service is hosted by an HTTP server which accepts requests to specific URIs corresponding to the available provisioning endpoints (eg. for session establishment, for Wi-Fi credentials configuration, etc.).

In the provisioning process the device is configured as a Wi-Fi station with specified credentials. Once configured, the device will retain the Wi-Fi configuration, until a flash erase is performed.

Right after provisioning is complete, Wi-Fi SoftAP and the HTTP server are deactivated. Though, that is specific to this example, and the user can choose to keep SoftAP / HTTP server active in their own application.

`softap_prov` uses the following components :
* `wifi_provisioning` : provides data structures and protocomm endpoint handlers for Wi-Fi configuration
* `protocomm` : for protocol based communication and secure session establishment
* `protobuf` : Google's protocol buffer library for serialization of protocomm data structures

This example can be used, as it is, for adding a provisioning service to any application intended for IoT.

## How to use example

### Hardware Required

Example should be able to run on any commonly available ESP32 development board.

### Application Required

Provisioning applications are available for various platforms. See below

#### Platform : Android

For Android, a provisioning application along with source code is available on GitHub : [esp-idf-provisioning-android](https://github.com/espressif/esp-idf-provisioning-android)

#### Platform : iOS

For iOS, a provisioning application along with source code is available on GitHub : [esp-idf-provisioning-ios](https://github.com/espressif/esp-idf-provisioning-ios)

#### Platform : Linux / Windows / macOS

To provision the device running this example, the `esp_prov.py` script needs to be run (found under `$IDF_PATH/tools/esp_prov`). This feature of `esp_prov` should work on all platforms, given the dependencies are satisfied.

### Configure the project

```
make menuconfig
```

* Set serial port under Serial Flasher Options.

* Under Example Configuration set the following :
    * SoftAP SSID (Defaults to PROV_<MACID>)
    * SoftAP Password (Defaults to PROV_PASS)
    * Security Version (default 1)
    * Proof of Possession (default "abcd1234")

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (1562) app: SoftAP started
I (1572) app_prov: SoftAP Provisioning started with SSID 'PROV_261FCC', Password 'PROV_PASS'
```

Make sure to connect the client computer to the SoftAP network, whose SSID and Password are displayed in the serial monitor log. On successful connection the monitor log will show :

```
I (519482) tcpip_adapter: softAP assign IP to station,IP is: 192.168.4.2
```

In a separate terminal run the `esp_prov.py` script under `$IDP_PATH/tools/esp_prov` directory (please replace `myssid` and `mypassword` with the credentials of the AP to which the device is supposed to connect to after provisioning). The SoftAP endpoint corresponds to the IP and port of the device on the SoftAP network, but this is usually same as the default value and may be left out. Assuming default example configuration, the script should be run as follows :

```
python esp_prov.py --ssid myssid --passphrase mypassword --sec_ver 1 --pop abcd1234 --transport softap --softap_endpoint 192.168.4.1:80
```

Above command will perform the provisioning steps, and the monitor log should display something like this :

```
I (634572) app_prov_handler: WiFi Credentials Received :
    ssid     : myssid
    password : mypassword
.
.
.
I (634652) app_prov_handler: WiFi Credentials Applied
I (634652) app_prov: STA Start
.
.
.
I (688270) app_prov_handler: Connecting state
.
.
.
I (637732) app_prov: STA Got IP
I (637732) app: got ip:192.168.43.220
.
.
.
I (654562) app_prov_handler: Connected state
```

After sometime the provisioning app will exit, SoftAP will be turned off and HTTP server will be stopped

```
I (667732) app_prov: Stopping provisioning
I (668732) app_prov: Provisioning stopped
I (668742) app: SoftAP stopped
```

## Troubleshooting

### Provisioning failed

It is possible that the Wi-Fi credentials provided were incorrect, or the device was not able to establish connection to the network, in which the the `esp_prov` script will notify failure (with reason) and the provisioning app will continue running, allowing the user to retry the process. Serial monitor log will display the failure along with disconnect reason :

```
E (39291) app_prov: STA Disconnected
E (39291) app_prov: Disconnect reason : 201
I (39291) app_prov: STA AP Not found
I (42021) app_prov_handler: Disconnected state
```

### Provisioning does not start

If the serial monitor log is different, as shown below :

```
I (539) app_prov: Found ssid myssid
I (539) app_prov: Found password mypassword
I (549) app: Starting WiFi station
```

It means the Wi-Fi credentials were already set by some other application flashed previously to your device. To erase these credentials either do full erase and then flash the example

```
make erase_flash
make -j4 flash monitor
```

Or, enable `Reset Provisioning` option under `Example Configuration` under menuconfig. But this will erase the saved Wi-Fi credentials every time the device boots, so this is not the preferred solution.
