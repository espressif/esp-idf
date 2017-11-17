# Wifi SCAN Example

This example shows how to use scan of ESP32.

We have two way to scan, fast scan and all channel scan:

* fast scan: in this mode, scan will finish after find match AP even didn't scan all the channel, you can set thresholds for signal and authmode, it will ignore the AP which below the thresholds.

* all channel scan : scan will end after checked all the channel, it will store four of the whole matched AP, you can set the sort method base on rssi or authmode, after scan, it will choose the best one 

and try to connect. Because it need malloc dynamic memory to store match AP, and most of cases is to connect to better signal AP, so it needn't record all the AP matched. The number of matches is limited to 4 in order to limit dynamic memory usage. Four matches allows APs with the same SSID name and all possible auth modes - Open, WEP, WPA and WPA2.
