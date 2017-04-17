# Wifi WPS Example

This example shows how to use wps in esp32.

The wps only works in station mode now.

* PBC_MODE: Start esp32 and press the wps button on router, Then esp32 will get the ssid&password by wps PBC mode.

* PIN_MODE: Start esp32, It will enter wps mode and you'll see a pin code showing by COM. Enter this pin code in router and the esp32 can get ssid&password by wps PIN mode.

More info in the code [wps.c](./main/wps.c).
