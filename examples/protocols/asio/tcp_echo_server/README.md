# ASIO tcp echo server example

Simple asio echo server using WiFi STA

## Example workflow

- WiFi STA is started and trying to connect to the access point defined through `make menuconfig`
- Once connected and acquired IP address, ASIO tcp server is started on port number defined through `make menuconfig`
- Server receives and echoes back messages transmitted from client

## Running the example

- Run `make menuconfig` to configure the access point's SSID and Password and port number
- Run `make flash monitor` to build and upload the example to your board and connect to it's serial terminal
- Wait for WiFi to connect to your access point (note the IP address)
- You can now send a tcp message and check it is repeated, for example using netcat `nc IP PORT`

See the README.md file in the upper level 'examples' directory for more information about examples.
