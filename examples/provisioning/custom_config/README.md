# SoftAP + HTTPD based Provisioning Example featuring Custom configuration

(See the README.md file in the upper level 'examples' directory for more information about examples.)

(Please see the README.md under `softap_prov` example before this.)

`custom_config` example demonstrates the implementation and integration of various IDF components for building a provisioning application.

This is same as `softap_prov` example, with added feature for configuration of some custom data (just like Wi-Fi configuration) during provisioning. The custom data provided during provisioning is simply printed on the serial monitor. The rest of the program functions just like `softap_prov`, ie. the device is configured as Wi-Fi station with supplied AP credentials.


`custom_config` uses the following components :
* `wifi_provisioning` : provides data structures and protocomm endpoint handlers for Wi-Fi configuration
* `protocomm` : for protocol based communication and secure session establishment
* `protobuf` : Google's protocol buffer library for serialization of protocomm data structures

Also, it uses a component provided with this example `custom_provisioning` which provides data structures and protocomm endpoint handlers for custom data configuration

## How to use example

### Hardware Required

Example should be able to run on any commonly available ESP32 development board.

### Application Required

To provision the device running this example, the `esp_prov.py` script needs to be run (found under `$IDF_PATH/tools/esp_prov`). This feature of `esp_prov` should work on all platforms, given the dependencies are satisfied.

### Configure the project

```
make menuconfig
```

* Set serial port under Serial Flasher Options.

* Under Example Configuration set the following :
    * SoftAP SSID (Defaults to PROV_<MACID>)
    * SoftAP Password (Defaults to PROV_PASS)
    * Security Version (default 0)
    * Proof of Possession (by default not needed for security version 0)

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

In a separate terminal run the `esp_prov.py` script under `$IDP_PATH/tools/esp_prov` directory (please replace the values corresponding to the parameters `--custom_info` and `--custom_ver` with your desired values for the custom configuration). Assuming default example configuration, the script should be run as follows :

```
python esp_prov.py --ssid myssid --passphrase mypassword --sec_ver 0 --transport softap --softap_endpoint 192.168.4.1:80  --custom_config --custom_info "some string" --custom_ver 4321
```

Above command will perform the provisioning steps, and the monitor log should display something like this :

```
I (92734) app_prov_handler: Custom config received :
    Info : some string
    Version : 4321
.
.
.
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
