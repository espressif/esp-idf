| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

## SPI slave example

These two projects illustrate the SPI Slave driver. They're supposed to be flashed into two separate Espressif chips connected to eachother using the SPI pins defined in app_main.c. Once connected and flashed, they will use the spi master and spi slave driver to communicate with eachother. The example also includes a handshaking line to allow the master to only poll the  slave when it is actually ready to parse a transaction.

The default GPIOs used in the example are the following:

| Signal    | ESP32  | ESP32-S2 | ESP32-C3 |
|-----------|--------|----------|----------|
| Handshake | GPIO2  | GPIO2    | GPIO3    |
| MOSI      | GPIO12 | GPIO12   | GPIO7    |
| MISO      | GPIO13 | GPIO13   | GPIO2    |
| SCLK      | GPIO15 | GPIO15   | GPIO6    |
| CS        | GPIO14 | GPIO14   | GPIO10   |


Please run wires between the following GPIOs between the slave and master to make the example function:

| Slave      | Master    |
|------------|-----------|
| Handshake  | Handshake |
| MOSI       | MOSI      |
| MISO       | MISO      |
| SCLK       | SCLK      |
| CS         | CS        |

Be aware that the example by default uses lines normally reserved for JTAG on ESP32. If this is an issue, either because of hardwired JTAG hardware or because of the need to do JTAG debugging, feel free to change the GPIO settings by editing defines in the top of main.c in the master/slave source code.