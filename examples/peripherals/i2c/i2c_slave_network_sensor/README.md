| Supported Targets | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |


# I2C slave example

This code demonstrates how to use the I2C slave driver to make a "network sensor". For illustration, the "sensor" will fetch some data from Github. You can attach this "sensor" to any I2C bus, then the master should be able to read the network data. This example uses the stretch mechanism to solve the problem of the i2c slave not knowing when it should send or receive data. In this example, we have a task to periodically update esp-idf's data on github. When the i2c master comes to visit, if the i2c slave fifo has data, it will return the data directly, if there is no data, it will set off the stretch interrupt, then there will be a callback to fill in the updated github data into the i2c fifo, release the stretch, and the master will return the data.

## Overview

This example demonstrates the possible usage of I2C slave driver

## How to use example

### Hardware Required

To run this example, you should have one  ESP32-S, ESP32-C based development board. Also this example need wifi signal to access the internet.

#### Pin Assignment:

**Note:** The following pin assignments are used by default, you can change these in the `menuconfig` .

|                                | SDA            | SCL            |
| ------------------------------ | -------------- | -------------- |
| ESP I2C Master (this example)  | I2C_MASTER_SDA | I2C_MASTER_SCL |
| ESP I2C Slave  (this example)  | I2C_SLAVE_SDA  | I2C_SLAVE_SCL  |

For the actual default value of `I2C_SLAVE_SDA` and `I2C_SLAVE_SCL` see `Example Configuration` in `menuconfig`.

**Note:** There's no need to add an external pull-up resistors for SDA/SCL pin, because the driver will enable the internal pull-up resistors.


### Command Assignment:

|  Command  |    Content   |
| --------- | ------------ |
|   0x10    |     stars    |
|   0x20    |     forks    |
|   0x30    |  open_issues |
|   0x40    |  description |


### Build and Flash

Enter `idf.py menuconfig` to config the internet information. In Example Connection Configuration option to config your wifi ssid and password.

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (11200) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (11210) wifi:set rx beacon pti, rx_bcn_pti: 0, bcn_timeout: 25000, mt_pti: 0, mt_time: 10000
I (11250) wifi:<ba-add>idx:0 (ifx:0, f6:0e:57:fd:ee:8d), tid:0, ssn:1, winSize:64
I (11610) wifi:dp: 2, bi: 102400, li: 4, scale listen interval from 307200 us to 409600 us
I (11620) wifi:AP's beacon interval = 102400 us, DTIM period = 2
I (12460) example_connect: Got IPv6 event: Interface "example_netif_sta" address: fe80:0000:0000:0000:86f7:03ff:fe80:0284, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (12720) esp_netif_handlers: example_netif_sta ip: 192.168.70.133, mask: 255.255.255.0, gw: 192.168.70.132
I (12720) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.70.133
I (12730) example_common: Connected to example_netif_sta
I (12730) example_common: - IPv4 address: 192.168.70.133,
I (12740) example_common: - IPv6 address: fe80:0000:0000:0000:86f7:03ff:fe80:0284, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (12750) gpio: GPIO[5]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 0| Pulldown: 0| Intr:0
I (12760) gpio: GPIO[4]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 0| Pulldown: 0| Intr:0
I (12770) main_task: Returned from app_main()
I (13470) wifi:<ba-add>idx:1 (ifx:0, f6:0e:57:fd:ee:8d), tid:3, ssn:0, winSize:64
Star count: 13401
Forks count: 7248
issue count: 1767
the description is: Espressif IoT Development Framework. Official development framework for Espressif SoCs.
I (16280) HTTP_CLIENT: HTTP GET Status = 200, content_length = 6165
```

Then you can read the information from an I2C master device.

We can get information via `i2c_tool` example with another esp chip plays in a i2c master role:

```
I (203) main_task: Calling app_main()

 ==============================================================
 |             Steps to Use i2c-tools                         |
 |                                                            |
 |  1. Try 'help', check all supported commands               |
 |  2. Try 'i2cconfig' to configure your I2C bus              |
 |  3. Try 'i2cdetect' to scan devices on the bus             |
 |  4. Try 'i2cget' to get the content of specific register   |
 |  5. Try 'i2cset' to set the value of specific register     |
 |  6. Try 'i2cdump' to dump all the register (Experiment)    |
 |                                                            |
 ==============================================================


Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
I (303) main_task: Returned from app_main()
i2c-tools> i2cconfig --port=0 --freq=100000 --sda=5 --scl=4
i2c-tools> i2cdetect
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- 28 -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
i2c-tools> i2cget -c 0x28 -r 0x10 -l 4
0x4b 0x35 0x00 0x00
i2c-tools> i2cget -c 0x28 -r 0x20 -l 4
0x75 0x1c 0x00 0x00
i2c-tools> i2cget -c 0x28 -r 0x40 -l 100
0x45 0x73 0x70 0x72 0x65 0x73 0x73 0x69 0x66 0x20 0x49 0x6f 0x54 0x20 0x44 0x65
0x76 0x65 0x6c 0x6f 0x70 0x6d 0x65 0x6e 0x74 0x20 0x46 0x72 0x61 0x6d 0x65 0x77
0x6f 0x72 0x6b 0x2e 0x20 0x4f 0x66 0x66 0x69 0x63 0x69 0x61 0x6c 0x20 0x64 0x65
0x76 0x65 0x6c 0x6f 0x70 0x6d 0x65 0x6e 0x74 0x20 0x66 0x72 0x61 0x6d 0x65 0x77
0x6f 0x72 0x6b 0x20 0x66 0x6f 0x72 0x20 0x45 0x73 0x70 0x72 0x65 0x73 0x73 0x69
0x66 0x20 0x53 0x6f 0x43 0x73 0x2e 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
0x00 0x00 0x00 0x00
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
