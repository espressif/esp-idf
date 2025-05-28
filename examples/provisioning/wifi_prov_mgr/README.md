| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Wi-Fi Provisioning Manager Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

`wifi_prov_mgr` example demonstrates the usage of `wifi_provisioning` manager component for building a provisioning application.

For this example, BLE is chosen as the default mode of transport, over which the provisioning related communication is to take place. NimBLE has been configured as the default host, but you can also switch to Bluedroid using menuconfig -> Components -> Bluetooth -> Bluetooth Host.

> Note: Since ESP32-S2 does not support BLE, the SoftAP will be the default mode of transport in that case. Even for ESP32, you can change to SoftAP transport from menuconfig.

In the provisioning process the device is configured as a Wi-Fi station with specified credentials. Once configured, the device will retain the Wi-Fi configuration, until a flash erase is performed.

Right after provisioning is complete, BLE is turned off and disabled to free the memory used by the BLE stack. Though, that is specific to this example, and the user can choose to keep BLE stack intact in their own application.

`wifi_prov_mgr` uses the following components :
* `wifi_provisioning` : provides manager, data structures and protocomm endpoint handlers for Wi-Fi configuration
* `protocomm` : for protocol based communication and secure session establishment
* `protobuf` : Google's protocol buffer library for serialization of protocomm data structures
* `bt` : ESP32 BLE stack for transport of protobuf packets

This example can be used, as it is, for adding a provisioning service to any application intended for IoT.

> Note: If you use this example code in your own project, in BLE mode, then remember to enable the BT stack and BTDM BLE control settings in your SDK configuration (e.g. by using the `sdkconfig.defaults` file from this project).

## How to use example

### Hardware Required

Example should be able to run on any commonly available ESP32/ESP32-S2 development board.

### Application Required

Provisioning applications are available for various platforms. See below

#### Platform : Android

For Android, a provisioning application along with source code is available on GitHub : [esp-idf-provisioning-android](https://github.com/espressif/esp-idf-provisioning-android)

#### Platform : iOS

For iOS, a provisioning application along with source code is available on GitHub : [esp-idf-provisioning-ios](https://github.com/espressif/esp-idf-provisioning-ios)

#### Platform : Linux / Windows / macOS

To install the dependency packages needed, please refer to the top level [README file](../../README.md#running-test-python-script-pytest).

`esp_prov` supports BLE and SoftAP transport for Linux, MacOS and Windows platforms. For BLE, however, if dependencies are not met, the script falls back to console mode and requires another application through which the communication can take place. The `esp_prov` console will guide you through the provisioning process of locating the correct BLE GATT services and characteristics, the values to write, and input read values.

### Configure the project

```
idf.py menuconfig
```
* Set the BLE/Soft AP transport under "Example Configuration" options. ESP32-S2 will have only SoftAP option (SoftAP option cannot be used if IPv4 is disabled in lwIP)

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (445) app: Starting provisioning
I (1035) app: Provisioning started
I (1045) wifi_prov_mgr: Provisioning started with service name : PROV_261FCC
```

Make sure to note down the BLE device name (starting with `PROV_`) displayed in the serial monitor log (eg. PROV_261FCC). This will depend on the MAC ID and will be unique for every device.

In a separate terminal run the `esp_prov.py` script under `$IDP_PATH/tools/esp_prov` directory (make sure to replace `myssid` and `mypassword` with the credentials of the AP to which the device is supposed to connect to after provisioning). Assuming default example configuration, which uses the protocomm security scheme 1 with PoP-based (proof-of-possession) authentication :

```
python esp_prov.py --transport ble --service_name PROV_261FCC --sec_ver 1 --pop abcd1234 --ssid myssid --passphrase mypassword
```

For security version 2, the following command can be used:
```
python esp_prov.py --transport ble --service_name PROV_261FCC --sec_ver 2 --sec2_username wifiprov --sec2_pwd abcd1234 --ssid myssid --passphrase mypassword
```

Above command will perform the provisioning steps, and the monitor log should display something like this :

```
I (39725) app: Received Wi-Fi credentials
    SSID     : myssid
    Password : mypassword
.
.
.
I (45335) esp_netif_handlers: sta ip: 192.168.43.243, mask: 255.255.255.0, gw: 192.168.43.1
I (45345) app: Provisioning successful
I (45345) app: Connected with IP Address:192.168.43.243
I (46355) app: Hello World!
I (47355) app: Hello World!
I (48355) app: Hello World!
I (49355) app: Hello World!
.
.
.
I (52315) wifi_prov_mgr: Provisioning stopped
.
.
.
I (52355) app: Hello World!
I (53355) app: Hello World!
I (54355) app: Hello World!
I (55355) app: Hello World!
```

**Note:** For generating the credentials for security version 2 (`SRP6a` salt and verifier) for the device-side, the following example command can be used. The output can then directly be used in this example.

The config option `CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE` should be enabled for the example and in `main/app_main.c`, the macro `EXAMPLE_PROV_SEC2_USERNAME` should be set to the same username used in the salt-verifier generation.

```log
$ python esp_prov.py --transport softap --sec_ver 2 --sec2_gen_cred --sec2_username wifiprov --sec2_pwd abcd1234
==== Salt-verifier for security scheme 2 (SRP6a) ====
static const char sec2_salt[] = {
    0x03, 0x6e, 0xe0, 0xc7, 0xbc, 0xb9, 0xed, 0xa8, 0x4c, 0x9e, 0xac, 0x97, 0xd9, 0x3d, 0xec, 0xf4
};

static const char sec2_verifier[] = {
    0x7c, 0x7c, 0x85, 0x47, 0x65, 0x08, 0x94, 0x6d, 0xd6, 0x36, 0xaf, 0x37, 0xd7, 0xe8, 0x91, 0x43,
    0x78, 0xcf, 0xfd, 0x61, 0x6c, 0x59, 0xd2, 0xf8, 0x39, 0x08, 0x12, 0x72, 0x38, 0xde, 0x9e, 0x24,
    .
    .
    .
    0xe6, 0xf6, 0x53, 0xc8, 0x31, 0xa8, 0x78, 0xde, 0x50, 0x40, 0xf7, 0x62, 0xde, 0x36, 0xb2, 0xba
};

```

### QR Code Scanning

Enabling `CONFIG_EXAMPLE_PROV_SHOW_QR` will display a QR code on the serial terminal, which can be scanned from the ESP Provisioning phone apps to start the Wi-Fi provisioning process.

The monitor log should display something like this :

```
I (1462) app: Provisioning started
I (1472) app: Scan this QR code from the provisioning application for Provisioning.
I (1472) QRCODE: Encoding below text with ECC LVL 0 & QR Code Version 10
I (1482) QRCODE: {"ver":"v1","name":"PROV_EA69FC","pop":"abcd1234","transport":"ble"}
GAP procedure initiated: advertise; disc_mode=2 adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=256 adv_itvl_max=256

  █▀▀▀▀▀█ ▀▀▀█▄█   ▀▀▄ █▄ ▀ █▀▀▀▀▀█
  █ ███ █  ▀▄█ █▄ ▀▄█ ▄██ █ █ ███ █
  █ ▀▀▀ █  ▄▀█▀▄▀ ▀█▄▀  ██  █ ▀▀▀ █
  ▀▀▀▀▀▀▀ █▄▀ █▄█▄█ ▀ █ █ ▀ ▀▀▀▀▀▀▀
  ▀▀ ▀▀ ▀  ▀▄ ▀▄ ▄▀▀▀█ ▀▄ ▀ ▀▄▄ ▄▄▀
  ███▄█▄▀ █▀  ▀▀▀▀▄▄█   █▀ █  ▄█▄█▀
  ▀███▀ ▀▄▄██ ▄▄██▄ ▀▀▀▀   ▄▀█ ▀▄▄▀
  ▄███  ▀██▀▀ ▄ ▄█▄▀▀█▄ ▀▄▀▄▄█  ▄
  ▀█▀ █▄▀▀ ▀▀█▀▀ █▀▄▀▄▀ ▄█  ███▄ ██
  ██▀█  ▀▄█ █▄▀▄███▀▄▀█ ▀█ █▀▀ ▀▄▄▀
  █▄▀▄█▀▀ ▀▄ ▀▄▄█▄▀▀█▄█▄█▀▀█ ▀▄ ▄▀
  █ ▄█▄ ▀ ▄▀ █▄  ▀█▄█▄▀▀█▀█ ▄█ ▀▄▄█
  ▀▀▀▀  ▀ █▀█▀▀▄▄██▄█▀█ ▀██▀▀▀█▄▄▀
  █▀▀▀▀▀█   ▄█▀▀▀██ ▄▀▄ █▄█ ▀ █ ▄ ▄
  █ ███ █ █ ▀▄█▀▀█▀▄█▄▄ ▀██▀▀▀▀▄▄▀▀
  █ ▀▀▀ █ ▄█ ▀ ▄█▀█ █▀ ▀▀███▄▀█ █▄█
  ▀▀▀▀▀▀▀ ▀  ▀  ▀▀ ▀     ▀▀▀▀▀▀


I (1702) app: If QR code is not visible, copy paste the below URL in a browser.
https://espressif.github.io/esp-jumpstart/qrcode.html?data={"ver":"v1","name":"PROV_EA69FC","pop":"abcd1234","transport":"ble"}
```


### Wi-Fi Scanning

Provisioning manager also supports providing real-time Wi-Fi scan results (performed on the device) during provisioning. This allows the client side applications to choose the AP for which the device Wi-Fi station is to be configured. Various information about the visible APs is available, like signal strength (RSSI) and security type, etc. Also, the manager now provides capabilities information which can be used by client applications to determine the security type and availability of specific features (like `wifi_scan`).

When using the scan based provisioning, we don't need to specify the `--ssid` and `--passphrase` fields explicitly:

```
python esp_prov.py --transport ble --service_name PROV_261FCC --pop abcd1234
```

See below the sample output from `esp_prov` tool on running above command:

```
Connecting...
Connected
Getting Services...
Security scheme determined to be : 1

==== Starting Session ====
==== Session Established ====

==== Scanning Wi-Fi APs ====
++++ Scan process executed in 1.9967520237 sec
++++ Scan results : 5

++++ Scan finished in 2.7374596596 sec
==== Wi-Fi Scan results ====
S.N. SSID                              BSSID         CHN RSSI AUTH
[ 1] MyHomeWiFiAP                      788a20841996    1 -45  WPA2_PSK
[ 2] MobileHotspot                     7a8a20841996   11 -46  WPA2_PSK
[ 3] MyHomeWiFiAP                      788a208daa26   11 -54  WPA2_PSK
[ 4] NeighborsWiFiAP                   8a8a20841996    6 -61  WPA2_PSK
[ 5] InsecureWiFiAP                    dca4caf1227c    7 -74  Open

Select AP by number (0 to rescan) : 1
Enter passphrase for MyHomeWiFiAP :

==== Sending Wi-Fi Credentials to Target ====
==== Wi-Fi Credentials sent successfully ====

==== Applying Wi-Fi Config to Target ====
==== Apply config sent successfully ====

==== Wi-Fi connection state  ====
==== WiFi state: Connected ====
==== Provisioning was successful ====
```

### Interactive Provisioning

`esp_prov` supports interactive provisioning. You can trigger the script with a simplified command and input the necessary details
(`Proof-of-possession` for security scheme 1 and `SRP6a username`, `SRP6a password` for security scheme 2) as the provisioning process advances.

The command `python esp_prov.py --transport ble --sec_ver 1` gives out the following sample output:

```
Discovering...
==== BLE Discovery results ====
S.N. Name                              Address
[ 1] PROV_4C33E8                       01:02:03:04:05:06
[ 1] BT_DEVICE_SBC                     0A:0B:0C:0D:0E:0F
Select device by number (0 to rescan) : 1
Connecting...
Getting Services...
Proof of Possession required:

==== Starting Session ====
==== Session Established ====

==== Scanning Wi-Fi APs ====
++++ Scan process executed in 3.8695244789123535 sec
++++ Scan results : 2

++++ Scan finished in 4.4132080078125 sec
==== Wi-Fi Scan results ====
S.N. SSID                              BSSID         CHN RSSI AUTH
[ 1] MyHomeWiFiAP                      788a20841996    1 -45  WPA2_PSK
[ 2] MobileHotspot                     7a8a20841996   11 -46  WPA2_PSK

Select AP by number (0 to rescan) : 1
Enter passphrase for myssid :

==== Sending Wi-Fi Credentials to Target ====
==== Wi-Fi Credentials sent successfully ====

==== Applying Wi-Fi Config to Target ====
==== Apply config sent successfully ====

==== Wi-Fi connection state  ====
==== WiFi state: Connected ====
==== Provisioning was successful ====
```

### Sending Custom Data

The provisioning manager allows applications to send some custom data during provisioning, which may be
required for some other operations like connecting to some cloud service. This is achieved by creating
and registering additional endpoints using the below APIs

```
wifi_prov_mgr_endpoint_create();
wifi_prov_mgr_endpoint_register();
```

In this particular example, we have added an endpoint named "custom-data" which can be tested
by passing the `--custom_data <MyCustomData>` option to the esp\_prov tool. Following output is
expected on success:

```
==== Sending Custom data to esp32 ====
CustomData response: SUCCESS
```

## Troubleshooting

### Provisioning failed

It is possible that the Wi-Fi credentials provided were incorrect, or the device was not able to establish connection to the network, in which the the `esp_prov` script will notify failure (with reason). Serial monitor log will display the failure along with disconnect reason :

```
E (367015) app: Provisioning failed!
    Reason : Wi-Fi AP password incorrect
    Please reset to factory and retry provisioning
```

Once credentials have been applied, even though wrong credentials were provided, the device will no longer go into provisioning mode on subsequent reboots until NVS is erased (see following section).

### Provisioning does not start

If the serial monitor log shows the following :

```
I (465) app: Already provisioned, starting Wi-Fi STA
```

it means either the device has been provisioned earlier with or without success (e.g. scenario covered in above section), or that the Wi-Fi credentials were already set by some other application flashed previously onto your device. On setting the log level to DEBUG this is clearly evident :

```
D (455) wifi_prov_mgr: Found Wi-Fi SSID     : myssid
D (465) wifi_prov_mgr: Found Wi-Fi Password : m********d
I (465) app: Already provisioned, starting Wi-Fi STA
```

To fix this we simple need to erase the NVS partition from flash. First we need to find out its address and size. This can be seen from the monitor log on the top right after reboot.

```
I (47) boot: Partition Table:
I (50) boot: ## Label            Usage          Type ST Offset   Length
I (58) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (65) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (73) boot:  2 factory          factory app      00 00 00010000 00124f80
I (80) boot: End of partition table
```

Now erase NVS partition by running the following commands :

```
$IDF_PATH/components/esptool_py/esptool/esptool.py erase_region 0x9000 0x6000
```

### Bluetooth Pairing Request during provisioning

ESP-IDF now has functionality to enforce link encryption requirement while performing GATT write on characteristics of provisioning service. This will however result in a pairing pop-up dialog, if link is not encrypted. This feature is disabled by default. In order to enable this feature, please set `CONFIG_WIFI_PROV_BLE_FORCE_ENCRYPTION=y` in the sdkconfig or select the configuration using "idf.py menuconfig" .

```
Component Config --> Wi-Fi Provisioning Manager --> Force Link Encryption during Characteristic Read/Write

```
Recompiling the application with above changes should suffice to enable this functionality.


### Unsupported platform

If the platform requirement, for running `esp_prov` is not satisfied, then the script execution will fallback to console mode, in which case the full process (involving user inputs) will look like this :

```
==== Esp_Prov Version: v1.0 ====
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
