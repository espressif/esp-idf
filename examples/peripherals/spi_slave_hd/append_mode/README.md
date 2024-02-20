# SPI Slave Halfduplex: Append Mode Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the SPI Slave HD driver and ESSL driver in IDF to communicate:
(ESSL driver is an encapsulated layer based on SPI Master driver to communicate with halfduplex mode SPI Slave.)
* Slave waits for Master to initiate ESSL SPI send and receive request using the SPI Slave HD driver.

* Slave will continuously prepare TX / RX buffers to the HW for Master to read / write. It will always load its buffer to the HW when the internal queue (see `queue_size` in `spi_slave_hd_slot_config_t`) is not full. It doesn't need to wait until a transaction finishes to prepare next buffer to the HW, so as to increase the transaction speed.
* Master will receive a bunch of packets from the slave for a fixed number of iterations, then send a bunch of packets for a fixed number of iterations.

If you have your own Master, please follow the [Espressif ESP32-S2 Technical Reference Manual-SPI Slave Halfduplex](https://www.espressif.com/sites/default/files/documentation/esp32-s2_technical_reference_manual_en.pdf) for the data format.


**Suggest building/flashing/monitoring Slave example first**
Usually SPI Slave will try to make its buffer ready to be able to respond immediately. In this example, you could run Slave code first to prepare more buffer to the HW. Therefore you will have `queue_size` number of buffers which are ready for the transaction. And each time there is a vacancy, Slave can load its buffer to the HW.

## How to use example

### Hardware Required

This example requires 2 targets. Here we use 2 ESP32S2 DevKits to act as the SPI Master and SPI Slave respectively. Suggested pin connections are here:

| Signal    | Master | Slave  |
|-----------|--------|--------|
| CS        | GPIO10 | GPIO10 |
| SCLK      | GPIO12 | GPIO12 |
| MOSI      | GPIO11 | GPIO11 |
| MISO      | GPIO13 | GPIO13 |
| GND       | GND    | GND    |

Feel free to modify the pin setting defined on the top of the ``app_main.c``.

### Dependency

The master example is based on [esp_serial_slave_link component](https://components.espressif.com/components/espressif/esp_serial_slave_link), which is used to communicate to a ESP slave device.

The component can be installed by esp component manager. Since this example already installed it, no need to re-installed it again, but if you want to install this component in your own project, you can input the following command:
```
idf.py add-dependency espressif/esp_serial_slave_link
```

If the dependency is added, you can check `idf_component.yml` for more detail. When building this example or other projects with managed components, the component manager will search for the required components online and download them into the `managed_componets` folder.

### Configure the project

* Set the target of the build (where `{IDF_TARGET}` stands for the target chip such as `esp32s2`).

```
idf.py set-target {IDF_TARGET}
```

### Build and Flash

For each target, build the project and flash it to the board, then run monitor tool to view the output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

SPI Master
```
I (320) Master: Receiver
I (470) Receiver: 8 bytes are actually received:
I (470) Receiver: 21 22 23 24 25 26 27 28
I (560) Receiver: 8 bytes are actually received:
I (560) Receiver: 29 2a 2b 2c 2d 2e 2f 30
I (650) Receiver: 8 bytes are actually received:
I (650) Receiver: 2b 2c 2d 2e 2f 30 31 32
I (740) Receiver: 8 bytes are actually received:
I (740) Receiver: 33 34 35 36 37 38 39 3a
I (830) Receiver: 8 bytes are actually received:
I (830) Receiver: 0c 0d 0e 0f 10 11 12 13
I (920) Receiver: 8 bytes are actually received:
I (920) Receiver: 14 15 16 17 18 19 1a 1b
I (1010) Receiver: 8 bytes are actually received:
I (1010) Receiver: 1d 1e 1f 20 21 22 23 24
I (1100) Receiver: 8 bytes are actually received:
I (1100) Receiver: 25 26 27 28 29 2a 2b 2c
I (1190) Receiver: 8 bytes are actually received:
I (1190) Receiver: 00 01 02 03 04 05 06 07
I (1280) Receiver: 8 bytes are actually received:
I (1280) Receiver: 08 09 0a 0b 0c 0d 0e 0f
I (1280) Master: Sender
```

SPI Slave
```
I (6343) Receiver: 16 bytes are actually received:
I (6343) slave RX:: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
I (6523) Receiver: 16 bytes are actually received:
I (6523) slave RX:: 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10
I (6703) Receiver: 16 bytes are actually received:
I (6703) slave RX:: 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11
I (6883) Receiver: 16 bytes are actually received:
I (6883) slave RX:: 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12
I (7063) Receiver: 16 bytes are actually received:
I (7063) slave RX:: 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13
I (7243) Receiver: 16 bytes are actually received:
I (7243) slave RX:: 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14
I (7473) Receiver: 16 bytes are actually received:
I (7473) slave RX:: 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15
I (7653) Receiver: 16 bytes are actually received:
I (7653) slave RX:: 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16
I (7833) Receiver: 16 bytes are actually received:
I (7833) slave RX:: 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17
I (8013) Receiver: 16 bytes are actually received:
I (8013) slave RX:: 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18
```
