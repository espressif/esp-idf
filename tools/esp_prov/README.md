# ESP Provisioning Tool

# NAME
`esp_prov` - A python based utility for testing the provisioning examples over a host

# SYNOPSIS

```
python esp_prov.py --transport < mode of provisioning : softap \ ble \ console > [ Optional parameters... ]
```

# DESCRIPTION

Usage of `esp-prov` assumes that the provisioning app has specific protocomm endpoints active. These endpoints are active in the provisioning examples and accept specific protobuf data structures:

| Endpoint Name | URI (HTTP server on ip:port) | Description                                               |
|---------------|------------------------------|-----------------------------------------------------------|
| prov-session  | http://ip:port/prov-session  | Security endpoint used for session establishment          |
| prov-config   | http://ip:port/prov-config   | Endpoint used for configuring Wi-Fi credentials on device |
| proto-ver     | http://ip:port/proto-ver     | Version endpoint for checking protocol compatibility      |
| prov-scan     | http://ip:port/prov-scan     | Endpoint used for scanning Wi-Fi APs                      |
| custom-config | http://ip:port/custom-config | Optional endpoint for configuring custom credentials      |


# PARAMETERS

* `--help`
    Print the list of options along with brief descriptions

* `--verbose`, `-v`
    Sets the verbosity level of output log

* `--transport <mode>`
    Three options are available:
    * `softap`
        For SoftAP + HTTPD based provisioning. This assumes that the device is running in Wi-Fi SoftAP mode and hosts an HTTP server supporting specific endpoint URIs. Also client needs to connect to the device softAP network before running `esp_prov`
    * `ble`
        For BLE based provisioning (Linux support only. In Windows/macOS it redirects to console). This assumes that the provisioning endpoints are active on the device with specific BLE service UUIDs
    * `console`
        For debugging via console based provisioning. The client->device commands are printed to STDOUT and device->client messages are accepted via STDIN. This is to be used when device is accepting provisioning commands on UART console.

* `--ssid <AP SSID>` (Optional)
    For specifying the SSID of the Wi-Fi AP to which the device is to connect after provisioning. If not provided, scanning is initiated and scan results, as seen by the device, are displayed, of which an SSID can be picked and the corresponding password specified.

* `--passphrase <AP Password>` (Optional)
    For specifying the password of the Wi-Fi AP to which the device is to connect after provisioning. Only used when corresponding SSID is provided using `--ssid`

* `--sec_ver <Security version number>`
    For specifying version of protocomm endpoint security to use. For now two versions are supported:
    * `0` for `protocomm_security0`
    * `1` for `protocomm_security1`

* `--pop <Proof of possession string>` (Optional)
    For specifying optional Proof of Possession string to use for protocomm endpoint security version 1. This option is ignored when security version 0 is in use

* `--service_name <name> (Optional)
    When transport mode is ble, this specifies the BLE device name to which connection is to be established for provisioned.
    When transport mode is softap, this specifies the HTTP server hostname / IP which is running the provisioning service, on the SoftAP network of the device which is to be provisioned. This defaults to `192.168.4.1:80` if not specified

* `--custom_config` (Optional)
    This flag assumes the provisioning app has an endpoint called `custom-config`. Use `--custom_info` and `--custom_ver` options to specify the fields accepted by this endpoint

* `--custom_info <some string>` (Optional) (Only use along with `--custom_config`)
    For specifying an information string to be sent to the `custom-config` endpoint during provisioning

* `--custom_ver <some integer>` (Optional) (Only use along with `--custom_config`)
    For specifying a version number (int) to be sent to the `custom-config` endpoint during provisioning

# AVAILABILITY

`esp_prov` is intended as a cross-platform tool, but currently BLE communication functionality is only available on Linux (via BlueZ and DBus)

For android, a provisioning tool along with source code is available [here](https://github.com/espressif/esp-idf-provisioning-android)

On macOS and Windows, running with `--transport ble` option falls back to console mode, ie. write data and target UUID are printed to STDOUT and read data is input through STDIN. Users are free to use their app of choice to connect to the BLE device, send the write data to the target characteristic and read from it.

## Dependencies

This requires the following python libraries to run (included in requirements.txt):
* `future`
* `protobuf`
* `cryptography`

Run `pip install -r $IDF_PATH/tools/esp_prov/requirements.txt`

Note :
* The packages listed in requirements.txt are limited only to the ones needed AFTER fully satisfying the requirements of ESP-IDF
* BLE communication is only supported on Linux (via Bluez and DBus), therefore, the dependencies for this have been made optional

## Optional Dependencies (Linux Only)

These dependencies are for enabling communication with BLE devices using Bluez and DBus on Linux:
* `dbus-python`

Run `pip install -r $IDF_PATH/tools/esp_prov/requirements_linux_extra.txt`

# EXAMPLE USAGE

Please refer to the README.md files with the examples present under `$IDF_PATH/examples/provisioning/`, namely:

* `ble_prov`
* `softap_prov`
* `custom_config`
* `console_prov`

Each of these examples use specific options of the `esp_prov` tool and give an overview to simple as well as advanced usage scenarios.
