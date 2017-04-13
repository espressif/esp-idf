## SPI slave example

These two projects illustrate the SPI Slave driver. They're supposed to be flashed into two separate ESP32s connected to
eachother using the SPI pins defined in app_main.c. Once connected and flashed, they will use the spi master and spi slave
driver to communicate with eachother. The example also includes a handshaking line to allow the master to only poll the 
slave when it is actually ready to parse a transaction.

Please run wires between the following GPIOs between the slave and master to make the example function:

========= ====== =======
Signal    Slave  Master
========= ====== =======
Handshake GPIO2  GPIO2
MOSI      GPIO12 GPIO12
MISO      GPIO13 GPIO13
SCLK      GPIO15 GPIO15
CS        GPIO14 GPIO14
========= ====== =======

Be aware that the example by default uses lines normally reserved for JTAG. If this is an issue, either because of hardwired
JTAG hardware or because of the need to do JTAG debugging, feel free to change the GPIO settings by editing defines in the top
of main.c in the master/slave source code.