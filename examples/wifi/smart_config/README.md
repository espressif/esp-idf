# smartconfig Example

This example shows how ESP32 connects to AP with ESPTOUCH. Example does the following steps:

* Download ESPTOUCH APP from app store. [Android source code](https://github.com/EspressifApp/EsptouchForAndroid) and [iOS source code](https://github.com/EspressifApp/EsptouchForIOS) is available.

* Compile this example and upload it to an ESP32.

* Make sure your phone connect to target AP (2.4GHz).

* Open ESPTOUCH app and input password. There will be success message after few sec.

### Example output

Here is an example of smartconfig console output.
```
I (372) wifi: mode : sta (24:0a:c4:00:44:86)
I (422) smartconfig: SC version: V2.6.6
I (3802) wifi: ic_enable_sniffer
I (3802) sc: SC_STATUS_FIND_CHANNEL
I (234592) smartconfig: TYPE: ESPTOUCH
I (234592) smartconfig: T|PHONE MAC:68:3e:34:88:59:bf
I (234592) smartconfig: T|AP MAC:a4:56:02:47:30:07
I (234592) sc: SC_STATUS_GETTING_SSID_PSWD
I (239922) smartconfig: T|pswd: 123456789
I (239922) smartconfig: T|ssid: IOT_DEMO_TEST
I (239922) smartconfig: T|bssid: a4:56:02:47:30:07
I (239922) wifi: ic_disable_sniffer
I (239922) sc: SC_STATUS_LINK
I (239932) sc: SSID:IOT_DEMO_TEST
I (239932) sc: PASSWORD:123456789
I (240062) wifi: n:1 0, o:1 0, ap:255 255, sta:1 0, prof:1
I (241042) wifi: state: init -> auth (b0)
I (241042) wifi: state: auth -> assoc (0)
I (241052) wifi: state: assoc -> run (10)
I (241102) wifi: connected with IOT_DEMO_TEST, channel 1
I (244892) event: ip: 192.168.0.152, mask: 255.255.255.0, gw: 192.168.0.1
I (244892) sc: WiFi Connected to ap
I (247952) sc: SC_STATUS_LINK_OVER
I (247952) sc: Phone ip: 192.168.0.31
I (247952) sc: smartconfig over
```
