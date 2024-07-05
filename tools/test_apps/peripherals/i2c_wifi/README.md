| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# I2C-WIFI Test

## Overview

This test demonstrates basic usage of I2C driver with wifi softAP.

### I2C MASTER

1. Switch on wifi softAP, and connect to your personal device, like your mobile phone.
2. Send data to slave board.

### I2C SLAVE

1. Receive data from master board.

## How to use test

### Hardware Required

To run this test, you should have at least one ESP development board for master. But to make this test convenient, we also use a ESP development board for slave. (You can change your slave device, but don't forget to change code as well if necessary.)

#### Pin Assignment

**Note:** The following pin assignments are used by default, you can change these  in the `menuconfig` .

|                    | SDA    | SCL    |
| ------------------ | ------ | ------ |
| ESP32/ESP32-S2 I2C | GPIO18 | GPIO19 |
| ESP32-S3       I2C | GPIO1  | GPIO2  |
| ESP32-C3       I2C | GPIO5  | GPIO9  |

**Note:** It is recommended to add external pull-up resistors for SDA/SCL pins to make the communication more stable, though the driver will enable internal pull-up resistors.

### Configure the project

Open the project configuration menu (`idf.py menuconfig`). Then go into `Test Configuration` menu.

#### I2C MASTER configuration

- In the `I2C working mode select` menu, you can set the working mode of i2c, choose `i2c master mode`.
- In the `I2C Configuration` menu, you can set the pin number of SDA/SCL. Also you can modify the I2C port number and frequency of the master. And you can set the slave address here as well.
- Enable `WIFI AP` if you need it.
- In the `WIFI softAP Configuration` menu, you can set the wifi information here.

#### I2C SLAVE configuration

- In the `I2C working mode select` menu, you can set the working mode of i2c, choose `i2c slave mode`.
- In the `I2C Configuration` menu, you can set the pin number of SDA/SCL. Also you can modify the I2C port number and address of the slave.

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Test Output

### MASTER

```bash
I (339) cpu_start: Starting scheduler.
I (359) I2C-wifi test: ESP_WIFI_MODE_AP
I (359) pp: pp rom version: 9387209
I (359) net80211: net80211 rom version: 9387209
I (369) wifi:wifi driver task: 3fc9def0, prio:23, stack:6656, core=0
I (369) system_api: Base MAC address is not set
I (379) system_api: read default base MAC address from EFUSE
I (379) wifi:wifi firmware version: d062fdb
I (389) wifi:wifi certification version: v7.0
I (389) wifi:config NVS flash: enabled
I (389) wifi:config nano formatting: disabled
I (399) wifi:Init data frame dynamic rx buffer num: 32
I (399) wifi:Init management frame dynamic rx buffer num: 32
I (409) wifi:Init management short buffer num: 32
I (409) wifi:Init dynamic tx buffer num: 32
I (419) wifi:Init static tx FG buffer num: 2
I (419) wifi:Init static rx buffer size: 1600
I (419) wifi:Init static rx buffer num: 10
I (429) wifi:Init dynamic rx buffer num: 32
I (429) wifi_init: rx ba win: 6
I (439) wifi_init: tcpip mbox: 32
I (439) wifi_init: udp mbox: 6
I (439) wifi_init: tcp mbox: 6
I (449) wifi_init: tcp tx win: 5744
I (449) wifi_init: tcp rx win: 5744
I (459) wifi_init: tcp mss: 1440
I (459) wifi_init: WiFi IRAM OP enabled
I (459) wifi_init: WiFi RX IRAM OP enabled
I (469) phy_init: phy_version 500,985899c,Apr 19 2021,16:05:08
I (589) wifi:set rx active PTI: 0, rx ack PTI: 0, and default PTI: 0
I (599) wifi:mode : softAP (7c:df:a1:86:d8:a9)
I (599) wifi:Total power save buffer number: 16
I (599) wifi:Init max length of beacon: 752/752
I (599) wifi:Init max length of beacon: 752/752
I (609) I2C-wifi test: wifi_init_softap finished. SSID:myssid password:mypassword channel:1
```

### SLAVE

```bash
I (277) cpu_start: Starting scheduler.
the clk config is 0, speed is 0
slave mode.read thread start...
I (23609) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23619) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23629) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23639) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23649) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23659) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23669) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23679) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23689) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23699) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23709) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23719) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23729) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23739) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23749) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23759) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23769) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23779) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23789) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23799) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23809) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23819) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23829) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23839) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23849) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23859) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23869) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23879) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23889) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23899) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23909) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23919) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23929) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23939) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23949) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23959) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23969) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23979) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23989) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (23999) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24009) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24019) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24029) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24039) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24049) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24059) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24069) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24079) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24089) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24099) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24109) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
I (24119) I2C-wifi test: 6a 1b 05 1f 1f 08 01 20 19 03 27 
```