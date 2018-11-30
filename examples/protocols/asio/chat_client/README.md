# ASIO chat server example

Simple asio chat client using WiFi STA

## Example workflow

- WiFi STA is started and trying to connect to the access point defined through `make menuconfig`
- Once connected and acquired IP address ASIO chat client connects to a corresponding server whose port number and ip are defined through `make menuconfig`
- Chat client receives all messages from other chat clients, also it sends message received from stdin using `make monitor`

## Running the example

- Run `make menuconfig` to configure the access point's SSID and Password and server ip address and port number
- Start chat server either on host machine or as another ESP device running chat_server example
- Run `make flash monitor` to build and upload the example to your board and connect to it's serial terminal
- Wait for WiFi to connect to your access point
- Receive and send messages to/from other clients on stdin/stdout via serial terminal.

See the README.md file in the upper level 'examples' directory for more information about examples.
