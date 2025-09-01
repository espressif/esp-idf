| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# I2C Tools Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

[I2C Tools](https://i2c.wiki.kernel.org/index.php/I2C_Tools) is a simple but very useful tool for developing I2C related applications, which is also famous in Linux platform. This example just implements some of basic features of [I2C Tools](https://i2c.wiki.kernel.org/index.php/I2C_Tools) based on [esp32 console component](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/console.html). As follows, this example supports five command-line tools:

1. `i2cconfig`: It will configure the I2C bus with specific GPIO number and frequency.
2. `i2cdetect`: It will scan an I2C bus for devices and output a table with the list of detected devices on the bus.
3. `i2cget`: It will read registers visible through the I2C bus.
4. `i2cset`: It will set registers visible through the I2C bus.
5. `i2cdump`: It will examine registers visible through the I2C bus.

If you have some trouble in developing I2C related applications, or just want to test some functions of one I2C device, you can play with this example first.

## How to use example

### Hardware Required

To run this example, you should have any ESP32, ESP32-S, ESP32-C, ESP32-H, ESP32-P based development board. For test purpose, you should have a kind of device with I2C interface as well. Here we will take the CCS811 sensor as an example to show how to test the function of this sensor without writing any code (just use the command-line tools supported by this example). For more information about CCS811, you can consult the [online datasheet](http://ams.com/ccs811).

#### Pin Assignment:

**Note:** You must run the `i2cconfig` command first to setup the I2C bus with proper GPIOs.
**Note:** It is recommended to add external pull-up resistors for SDA/SCL pins to make the communication more stable, though the driver will enable internal pull-up resistors.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project..

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

### Check all supported commands and their usages

```bash
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
i2c-tools> help
help  [<string>] [-v <0|1>]
  Print the summary of all registered commands if no arguments are given,
  otherwise print summary of given command.
      <string>  Name of command
  -v, --verbose=<0|1>  If specified, list console commands with given verbose level

i2cconfig  --scl=<gpio> --sda=<gpio> [--freq=<Hz>]
  Config I2C bus frequency and IOs
  --scl=<gpio>  Set the gpio for I2C SCL
  --sda=<gpio>  Set the gpio for I2C SDA
  --freq=<Hz>  Set the frequency(Hz) of I2C bus

i2cdetect
  Scan I2C bus for devices

i2cget  -c <chip_addr> [-r <register_addr>] [-l <length>]
  Read registers visible through the I2C bus
  -c, --chip=<chip_addr>  Specify the address of the chip on that bus
  -r, --register=<register_addr>  Specify the address on that chip to read from
  -l, --length=<length>  Specify the length to read from that data address

i2cset  -c <chip_addr> [-r <register_addr>] [<data>]...
  Set registers visible through the I2C bus
  -c, --chip=<chip_addr>  Specify the address of the chip on that bus
  -r, --register=<register_addr>  Specify the address on that chip to read from
        <data>  Specify the data to write to that data address

i2cdump  -c <chip_addr> [-s <size>]
  Examine registers visible through the I2C bus
  -c, --chip=<chip_addr>  Specify the address of the chip on that bus
  -s, --size=<size>  Specify the size of each read
```

### Configure the I2C bus

> [!IMPORTANT]
> You must run the `i2cconfig` command before using other I2C commands.

```bash
i2c-tools> i2cconfig --sda=18 --scl=19 --freq=100000
```

* `--sda` and `--scl` options to specify the gpio number used by I2C bus, here we choose GPIO18 as the SDA and GPIO19 as the SCL.
* `--freq` option to specify the frequency of I2C bus, here we set to 100KHz.

### Check the I2C address (7 bits) on the I2C bus

```bash
i2c-tools> i2cdetect
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- 5b -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
```

* Here we found the address of CCS811 is 0x5b.

### Get the value of status register

```bash
i2c-tools> i2cget -c 0x5b -r 0x00 -l 1
0x10
```

* `-c` option to specify the address of I2C device (acquired from `i2cdetect` command).
* `-r` option to specify the register address you want to inspect.
* `-l` option to specify the length of the content.
* Here the returned value 0x10 means that the sensor is just in the boot mode and is ready to go into application mode. For more information about CCS811 you should consult the [official website](http://ams.com/ccs811).

### Change the working mode

```bash
i2c-tools> i2cset -c 0x5b -r 0xF4
I (734717) cmd_i2ctools: Write OK
i2c-tools> i2cset -c 0x5b -r 0x01 0x10
I (1072047) cmd_i2ctools: Write OK
i2c-tools> i2cget -c 0x5b -r 0x00 -l 1
0x98
```

* Here we change the mode from boot to application and set a proper measure mode (by writing 0x10 to register 0x01)
* Now the status value of the sensor is 0x98, which means a valid data is ready to read

### Read the sensor data

```bash
i2c-tools> i2cget -c 0x5b -r 0x02 -l 8
0x01 0xb0 0x00 0x04 0x98 0x00 0x19 0x8f
```

* The register 0x02 will output 8 bytes result, mainly including value of eCO~2~、TVOC and there raw value. So the value of eCO~2~ is 0x01b0 ppm and value of TVOC is 0x04 ppb.

## Troubleshooting

* I don’t find any available address when running `i2cdetect` command.
  * Make sure your wiring connection is right.
  * Some sensor will have a “wake up” pin, via which user can put the sensor into a sleep mode. So make sure your sensor in **not** in the sleep state.
  * Reset you I2C device, and then run `i2cdetect` again.
* I can’t get the right content when running `i2cdump` command.
  * Currently the `i2cdump` only support those who have the same content length of registers inside the I2C device. For example, if a device have three register addresses, and the content length at these address are 1 byte, 2 bytes and 4 bytes. In this case you should not expect this command to dump the register correctly.


(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)

