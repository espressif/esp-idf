# Wifi Power Save Example

This example shows how to use power save mode of wifi.

Power save mode only works in sta mode.

* No power save: This is default mode. And the esp32 will work at full speed.

* modem sleep: If you set power save mode as modem sleep, after connected to AP about 10s, esp32 will wake up and sleep periodically. When it sleeps, the current will be much lower than it works.

* others: not supported yet.
