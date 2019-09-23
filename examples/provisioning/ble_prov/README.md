# BLE based Provisioning Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

`ble_prov` example demonstrates the implementation and integration of various IDF components for building a provisioning application.

For this example BLE is chosen as the mode of transport, over which the provisioning related communication is to take place, between the device (to be provisioned) and the client (owner of the device).

In the provisioning process the device is configured as a Wi-Fi station with specified credentials. Once configured, the device will retain the Wi-Fi configuration, until a flash erase is performed.

Right after provisioning is complete, BLE is turned off and disabled to free the memory used by the BLE stack. Though, that is specific to this example, and the user can choose to keep BLE on in their own application.

`ble_prov` uses the following components :
* `wifi_provisioning` : provides data structures and protocomm endpoint handlers for Wi-Fi configuration
* `protocomm` : for protocol based communication and secure session establishment
* `protobuf` : Google's protocol buffer library for serialization of protocomm data structures
* `bt` : ESP32 BLE stack for transport of protobuf packets

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

To provision the device running this example, the `esp_prov.py` script needs to be run (found under `$IDF_PATH/tools/esp_prov`). Make sure to satisfy all the dependencies prior to running the script.

Presently, `esp_prov` supports BLE transport only for Linux platform. For Windows/macOS it falls back to console mode and requires another application (for BLE) through which the communication can take place.

There are various applications, specific to Windows and macOS platform which can be used. The `esp_prov` console will guide you through the provisioning process of locating the correct BLE GATT services and characteristics, the values to write, and input read values.

### Configure the project

```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options.

* Under Example Configuration set the following :
    * Security Version (default 1)
    * Proof of Possession (default "abcd1234")

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (550) app: Starting BLE provisioning
I (1130) app_prov: Provisioning started with BLE devname : PROV_261FCC
```

Make sure to note down the BLE device name (starting with PROV_) displayed in the serial monitor log (eg. PROV_261FCC). This will depend on the MAC ID and will be unique for every device.

In a separate terminal run the `esp_prov.py` script under `$IDP_PATH/tools/esp_prov` directory (please replace `myssid` and `mypassword` with the credentials of the AP to which the device is supposed to connect to after provisioning). Assuming default example configuration :

```
python esp_prov.py --transport ble --service_name PROV_261FCC --sec_ver 1 --pop abcd1234 --ssid myssid --passphrase mypassword
```

Above command will perform the provisioning steps, and the monitor log should display something like this :

```
I (682950) app_prov_handler: WiFi Credentials Received :
    ssid     : myssid
    password : mypassword
.
.
.
I (683130) app_prov: STA Start
I (683130) app_prov_handler: WiFi Credentials Applied
.
.
.
I (688270) app_prov_handler: Connecting state
.
.
.
I (688390) app_prov: STA Got IP
I (688390) app: got ip:192.168.43.220
I (693410) app_prov_handler: Connected state
```

After sometime the provisioning app will exit and BLE will be turned off

```
I (718390) app_prov: Stopping provisioning
I (718670) app_prov: Provisioning stopped
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
idf.py -p PORT flash monitor
```

Or, enable `Reset Provisioning` option under `Example Configuration` under menuconfig. But this will erase the saved Wi-Fi credentials every time the device boots, so this is not the preferred solution.

### Unsupported platform

If the platform requirement, for running `esp_prov` is not satisfied, then the script execution will fallback to console mode, in which case the full process (involving user inputs) will look like this :

```
BLE client is running in console mode
    This could be due to your platform not being supported or dependencies not being met
    Please ensure all pre-requisites are met to run the full fledged client
BLECLI >> Please connect to BLE device `PROV_261FCC` manually using your tool of choice
BLECLI >> Was the device connected successfully? [y/n] y
BLECLI >> List available attributes of the connected device
BLECLI >> Is the service UUID '0000ffff-0000-1000-8000-00805f9b34fb' listed among available attributes? [y/n] y
BLECLI >> Is the characteristic UUID '0000ff53-0000-1000-8000-00805f9b34fb' listed among available attributes? [y/n] y
BLECLI >> Is the characteristic UUID '0000ff51-0000-1000-8000-00805f9b34fb' listed among available attributes? [y/n] y
BLECLI >> Is the characteristic UUID '0000ff52-0000-1000-8000-00805f9b34fb' listed among available attributes? [y/n] y

==== Verifying protocol version ====
BLECLI >> Write following data to characteristic with UUID '0000ff53-0000-1000-8000-00805f9b34fb' :
    >> 56302e31
BLECLI >> Enter data read from characteristic (in hex) :
    << 53554343455353
==== Verified protocol version successfully ====

==== Starting Session ====
BLECLI >> Write following data to characteristic with UUID '0000ff51-0000-1000-8000-00805f9b34fb' :
    >> 10015a25a201220a20ae6d9d5d1029f8c366892252d2d5a0ffa7ce1ee5829312545dd5f2aba057294d
BLECLI >> Enter data read from characteristic (in hex) :
    << 10015a390801aa0134122048008bfc365fad4753dc75912e0c764d60749cb26dd609595b6fbc72e12614031a1089733af233c7448e7d7fb7963682c6d8
BLECLI >> Write following data to characteristic with UUID '0000ff51-0000-1000-8000-00805f9b34fb' :
    >> 10015a270802b2012212204051088dc294fe4621fac934a8ea22e948fcc3e8ac458aac088ce705c65dbfb9
BLECLI >> Enter data read from characteristic (in hex) :
    << 10015a270803ba01221a20c8d38059d5206a3d92642973ac6ba8ac2f6ecf2b7a3632964eb35a0f20133adb
==== Session Established ====

==== Sending Wifi credential to esp32 ====
BLECLI >> Write following data to characteristic with UUID '0000ff52-0000-1000-8000-00805f9b34fb' :
    >> 98471ac4019a46765c28d87df8c8ae71c1ae6cfe0bc9c615bc6d2c
BLECLI >> Enter data read from characteristic (in hex) :
    << 3271f39a
==== Wifi Credentials sent successfully ====

==== Applying config to esp32 ====
BLECLI >> Write following data to characteristic with UUID '0000ff52-0000-1000-8000-00805f9b34fb' :
    >> 5355
BLECLI >> Enter data read from characteristic (in hex) :
    << 1664db24
==== Apply config sent successfully ====

==== Wifi connection state  ====
BLECLI >> Write following data to characteristic with UUID '0000ff52-0000-1000-8000-00805f9b34fb' :
    >> 290d
BLECLI >> Enter data read from characteristic (in hex) :
    << 505f72a9f8521025c1964d7789c4d7edc56aedebd144e1b667bc7c0975757b80cc091aa9f3e95b06eaefbc30290fa1
++++ WiFi state: connected ++++
==== Provisioning was successful ====
```

The write data is to be copied from the console output ```>>``` to the platform specific application and the data read from the application is to be pasted at the user input prompt ```<<``` of the console, in the format (hex) indicated in above sample log.
