# ESP Provisioning Tool

## Description

`esp_prov` - A python-based utility for testing the provisioning examples over a host machine.

Usage of `esp-prov` assumes that the provisioning app has specific protocomm endpoints active. These endpoints are active in the provisioning examples and accept specific protobuf data structure:

| Endpoint Name | URI (HTTP server on ip:port) | Description                                                                              |
|---------------|------------------------------|------------------------------------------------------------------------------------------|
| prov-session  | http://ip:port/prov-session  | Security endpoint used for session establishment                                         |
| prov-config   | http://ip:port/prov-config   | Endpoint used for configuring Wi-Fi credentials on device                                |
| proto-ver     | http://ip:port/proto-ver     | Version endpoint for checking protocol compatibility                                     |
| prov-scan     | http://ip:port/prov-scan     | Endpoint used for scanning Wi-Fi APs                                                     |
| prov-ctrl     | http://ip:port/prov-ctrl     | Endpoint used for controlling Wi-Fi provisioning state                                   |
| custom-data   | http://ip:port/custom-data   | Optional endpoint for sending custom data (refer `wifi_prov_mgr` example)                |


## Usage

```
python esp_prov.py --transport < mode of provisioning : softap \ ble \ console > [ Optional parameters... ]
```
### Parameters

* `--help`
    Print the list of options along with brief descriptions

* `--verbose`, `-v`
    Sets the verbosity level of output log

* `--transport <mode>`
    - Three options are available:
      * `softap` - for SoftAP + HTTPD based provisioning
        * Requires the device to be running in Wi-Fi SoftAP mode and hosting an HTTP server supporting specific endpoint URIs
        * The client needs to be connected to the device softAP network before running the `esp_prov` tool.
      * `ble` - for BLE based provisioning
        * Supports Linux, Windows and macOS; redirected to console if dependencies are not met
        * Assumes that the provisioning endpoints are active on the device with specific BLE service UUIDs
      * `console` - for debugging via console-based provisioning
        * The client->device commands are printed to STDOUT and device->client messages are accepted via STDIN.
        * This is to be used when the device is accepting provisioning commands on UART console.
      * `httpd` - the script works the same as for `softap`. This could be used on any other network interface than WiFi soft AP, e.g. Ethernet or USB.

* `--service_name <name>` (Optional)
    - When transport mode is `ble`, this specifies the BLE device name to which connection is to be established for provisioned. If not provided, BLE scanning is initiated and a list of nearby devices, as seen by the host, is displayed, of which the target device can be chosen.
    - When transport mode is `softap` or `httpd`, this specifies the HTTP server hostname / IP which is running the provisioning service, on the SoftAP network (or any other interface for `httpd` mode) of the device which is to be provisioned. This defaults to `192.168.4.1:80` if not specified

* `--ssid <AP SSID>` (Optional)
    - For specifying the SSID of the Wi-Fi AP to which the device is to connect after provisioning.
    - If not provided, scanning is initiated and scan results, as seen by the device, are displayed, of which an SSID can be picked and the corresponding password specified.

* `--passphrase <AP Password>` (Optional)
    - For specifying the password of the Wi-Fi AP to which the device is to connect after provisioning.
    - Only used when corresponding SSID is provided using the `--ssid` option

* `--sec_ver <Security version number>`
    - For specifying the version of protocomm endpoint security to use. Following 3 versions are supported:
      * `0` for `protocomm_security0` - No security
      * `1` for `protocomm_security1` - X25519 key exchange + Authentication using Proof of Possession (PoP) + AES-CTR encryption
      * `2` for `protocomm_security2` - Secure Remote Password protocol (SRP6a) + AES-GCM encryption

* `--pop <Proof of possession string>` (Optional)
    - For specifying optional Proof of Possession string to use for protocomm endpoint security version 1
    - Ignored when other security versions are used

* `--sec2_username <SRP6a Username>` (Optional)
    - For specifying optional username to use for protocomm endpoint security version 2
    - Ignored when other security versions are used

* `--sec2_pwd <SRP6a Password>` (Optional)
    - For specifying optional password to use for protocomm endpoint security version 2
    - Ignored when other security versions are used

* `--sec2_gen_cred` (Optional)
    - For generating the `SRP6a` credentials (salt and verifier) from the provided username and password for protocomm endpoint security version 2
    - Ignored when other security versions are used

* `--sec2_salt_len <SRP6a Salt Length>` (Optional)
    - For specifying the optional `SRP6a` salt length to be used for generating protocomm endpoint security version 2 credentials
    - Ignored when other security versions are used and the ``--sec2_gen_cred` option is not set

* `--reset` (Optional)
    - Resets internal state machine of the device and clears provisioned credentials; to be used only in case of provisioning failures

* `--reprov` (Optional)
    - Resets internal state machine of the device and clears provisioned credentials; to be used only in case the device is to be provisioned again for new credentials after a previous successful provisioning

* `--custom_data <some string>` (Optional)
    An information string can be sent to the `custom-data` endpoint during provisioning using this argument.
    (Assumes the provisioning app has an endpoint called `custom-data` - see [provisioning/wifi_prov_mgr](https://github.com/espressif/esp-idf/tree/master/examples/provisioning/wifi_prov_mgr) example for implementation details).


### Example Usage

Please refer to the `README.md` file with the `wifi_prov_mgr` example present under `$IDF_PATH/examples/provisioning/`.

This example uses specific options of the `esp_prov` tool and gives an overview of simple as well as advanced usage scenarios.

## Availability

For Android, a provisioning tool along with source code is available [here](https://github.com/espressif/esp-idf-provisioning-android).

## Dependencies

This requires the following python libraries to run:
* `bleak`
* `protobuf`
* `cryptography`

To install the dependency packages needed, please run the following command:

```shell
bash install.sh --enable-pytest
```

**Note:** For troubleshooting errors with BLE transport, please refer this [link](https://bleak.readthedocs.io/en/latest/troubleshooting.html).
