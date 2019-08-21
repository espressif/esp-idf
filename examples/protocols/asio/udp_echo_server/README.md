# Asio UDP echo server example

Simple Asio UDP echo server using WiFi STA or Ethernet.

## Example workflow

- Wi-Fi or Ethernet connection is established, and IP address is obtained.
- Asio UDP server is started on port number defined through the project configuration
- Server receives and echoes back messages transmitted from client

## Running the example

- Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.
- Set server port number in menuconfig, "Example configuration".
- Run `idf.py -p PORT flash monitor` to build and upload the example to your board and connect to it's serial terminal.
- Wait for the board to connect to WiFi or Ethernet (note the IP address).
- You can now send a UDP message and check it is repeated, for example using netcat `nc -u IP PORT`.

See the README.md file in the upper level 'examples' directory for more information about examples.
