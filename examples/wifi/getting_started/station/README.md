# WiFi station example

(See the README.md file in the upper level 'examples' directory for more information about examples.)


## How to use example

### Configure the project

```
make menuconfig
```

* Set serial port under Serial Flasher Options.

* Set WiFi SSID and WiFi Password and Maximum retry under Example Configuration Options.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

There is the console output for station connects to ap successfully:
```
I (727) wifi station: ESP_WIFI_MODE_STA
I (727) wifi: wifi driver task: 3ffc0c68, prio:23, stack:3584, core=0
I (727) wifi: wifi firmware version: 19b3110
I (727) wifi: config NVS flash: enabled
I (727) wifi: config nano formating: disabled
I (737) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (747) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (777) wifi: Init dynamic tx buffer num: 32
I (777) wifi: Init data frame dynamic rx buffer num: 32
I (777) wifi: Init management frame dynamic rx buffer num: 32
I (777) wifi: Init static rx buffer size: 1600
I (787) wifi: Init static rx buffer num: 10
I (787) wifi: Init dynamic rx buffer num: 32
I (907) phy: phy_version: 3960, 5211945, Jul 18 2018, 10:40:07, 0, 0
I (907) wifi: mode : sta (30:ae:a4:80:45:68)
I (907) wifi station: wifi_init_sta finished.
I (907) wifi station: connect to ap SSID:myssid password:mypassword
I (1027) wifi: n:6 0, o:1 0, ap:255 255, sta:6 0, prof:1
I (2017) wifi: state: init -> auth (b0)
I (2017) wifi: state: auth -> assoc (0)
I (2027) wifi: state: assoc -> run (10)
I (2067) wifi: connected with myssid, channel 6
I (2067) wifi: pm start, type: 1

I (3227) event: sta ip: 172.20.10.7, mask: 255.255.255.240, gw: 172.20.10.1
I (3227) wifi station: got ip:172.20.10.7
```

There is the console output for station connects to ap failed:
```
I (728) wifi station: ESP_WIFI_MODE_STA
I (728) wifi: wifi driver task: 3ffc0c68, prio:23, stack:3584, core=0
I (728) wifi: wifi firmware version: 19b3110
I (728) wifi: config NVS flash: enabled
I (738) wifi: config nano formating: disabled
I (738) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (748) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (778) wifi: Init dynamic tx buffer num: 32
I (778) wifi: Init data frame dynamic rx buffer num: 32
I (778) wifi: Init management frame dynamic rx buffer num: 32
I (788) wifi: Init static rx buffer size: 1600
I (788) wifi: Init static rx buffer num: 10
I (788) wifi: Init dynamic rx buffer num: 32
I (908) phy: phy_version: 3960, 5211945, Jul 18 2018, 10:40:07, 0, 0
I (908) wifi: mode : sta (30:ae:a4:80:45:68)
I (908) wifi station: wifi_init_sta finished.
I (918) wifi station: connect to ap SSID:myssid password:mypassword
I (3328) wifi station: retry to connect to the AP
I (3328) wifi station: connect to the AP fail

I (5738) wifi station: retry to connect to the AP
I (5738) wifi station: connect to the AP fail

I (8148) wifi station: retry to connect to the AP
I (8148) wifi station: connect to the AP fail

I (10558) wifi station: retry to connect to the AP
I (10558) wifi station: connect to the AP fail

I (12968) wifi station: retry to connect to the AP
I (12968) wifi station: connect to the AP fail

I (15378) wifi station: connect to the AP fail
```
