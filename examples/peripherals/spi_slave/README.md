| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

## SPI slave example

These two projects illustrate the SPI Slave driver. They're supposed to be flashed into two separate Espressif chips connected to eachother using the SPI pins defined in app_main.c. Once connected and flashed, they will use the spi master and spi slave driver to communicate with eachother. The example also includes a handshaking line to allow the master to only poll the  slave when it is actually ready to parse a transaction.

For different chip and host used, the connections may be different. Here show a example diagram of hardware connection, you can freely change the GPIO settings by editing defines in the top of `main/app_main.c` in the master/slave source code. and change the hardware relatively.

The default GPIOs used in the example are the following:

| Signal    | ESP32  |
|-----------|--------|
| Handshake | GPIO2  |
| MOSI      | GPIO12 |
| MISO      | GPIO13 |
| SCLK      | GPIO15 |
| CS        | GPIO14 |


Please run wires between the following GPIOs between the slave and master to make the example function:

| Slave      | Master    |
|------------|-----------|
| Handshake  | Handshake |
| MOSI       | MOSI      |
| MISO       | MISO      |
| SCLK       | SCLK      |
| CS         | CS        |

Be aware that the example by default uses lines normally reserved for JTAG on ESP32. If this is an issue, either because of hardwired JTAG hardware or because of the need to do JTAG debugging, feel free to change the GPIO settings.
