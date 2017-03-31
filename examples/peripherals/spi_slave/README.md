## SPI slave example

These two projects illustrate the SPI Slave driver. They're supposed to be flashed into two separate ESP32s connected to
eachother using the SPI pins defined in app_main.c. Once connected and flashed, they will use the spi master and spi slave
driver to communicate with eachother. The example also includes a handshaking line to allow the master to only poll the 
slave when it is actually ready to parse a transaction.