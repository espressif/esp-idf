# Wifi WPS Example

This example shows how to use wps enrollee in esp32.

Now we support to use enrollee feature in STA and APSTA mode.

* PBC_MODE: Start esp32 and press the wps button on router, Then esp32 will get the ssid&password by wps PBC mode.

* PIN_MODE: Start esp32, It will enter wps mode and you'll see a pin code showing by COM. Enter this pin code in router and the esp32 can get ssid&password by wps PIN mode.

More info in the code [wps.c](./main/wps.c).
