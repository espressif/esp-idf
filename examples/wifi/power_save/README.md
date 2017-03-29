# Wifi Power Save Example

This example shows how to use power save mode of wifi.

Power save mode only works in sta mode.

* No power save: This is default mode. And the esp32 will work with full power.

* modem sleep: If you set power save mode as modem sleep, 10s later after connecting to AP, esp32 will wake up and sleep(turn off RF and PHY) periodically.

* others: not supported yet.
