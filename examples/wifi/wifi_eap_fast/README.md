| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# WPA2 Enterprise Example

This example shows how ESP32 connects to AP with Wi-Fi enterprise encryption using the EAP-FAST method. The example does the following steps:

1. Install CA certificate which is optional.
2. Set user name and password and identity.
3. Set the PAC file which may be empty.
4. Enable WiFi enterprise mode.
5. Connect to AP.

*Note:*
1. EAP-FAST is not supported with `CONFIG_ESP_WIFI_MBEDTLS_TLS_CLIENT` and so is disabled by default.
2. Setting the config `fast_provisioning` to methods 0 and 1 do not support saving the PAC credentials in case of a restart or loss of power.
3. The certificates present in the `examples/wifi/wifi_eap_fast/main` folder contain server certificates which have the corresponding CA as well. These can be used for server validation which is opptional.
4. The expiration date of these certificates is 2027/06/05.

### Configuration

```
idf.py menuconfig
```
* Set SSID of Access Point to connect in Example Configuration.
* Enter EAP-ID.
* Enter Username and Password.
* Enable or disable Validate Server option.

### Build and Flash the project.

```
idf.py -p PORT flash monitor
```

### Example output

Here is an example of wpa2 enterprise (FAST method) console output.
```
I (690) example: Setting WiFi configuration SSID wpa2_test...
I (690) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (800) wifi:mode : sta (24:6f:28:80:41:78)
I (800) wifi:enable tsf
I (1410) wifi:new:<6,0>, old:<1,0>, ap:<255,255>, sta:<6,0>, prof:1
I (2410) wifi:state: init -> auth (b0)
I (2420) wifi:state: auth -> assoc (0)
E (2420) wifi:Association refused temporarily, comeback time 3072 mSec
I (5500) wifi:state: assoc -> assoc (0)
I (5500) wifi:state: assoc -> init (6c0)
I (5500) wifi:new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (7560) wifi:new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (7560) wifi:state: init -> auth (b0)
I (7560) wifi:state: auth -> assoc (0)
I (7570) wifi:state: assoc -> run (10)
I (7770) wifi:connected with wpa2_test, aid = 1, channel 6, BW20, bssid = 24:4b:fe:ab:be:99
I (7770) wifi:security: WPA2-ENT, phy: bg, rssi: -80
I (7780) wifi:pm start, type: 1

I (7800) example: ~~~~~~~~~~~
I (7800) example: IP:0.0.0.0
I (7800) example: MASK:0.0.0.0
I (7800) example: GW:0.0.0.0
I (7800) example: ~~~~~~~~~~~
I (7870) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (8580) esp_netif_handlers: sta ip: 192.168.5.3, mask: 255.255.255.0, gw: 192.168.5.1
I (12800) example: ~~~~~~~~~~~
I (12800) example: IP:192.168.5.3
I (12800) example: MASK:255.255.255.0
I (12800) example: GW:192.168.5.1
I (12800) example: ~~~~~~~~~~~
```
