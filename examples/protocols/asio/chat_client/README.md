# Asio chat client example

Simple Asio chat client using WiFi STA or Ethernet.

## Example workflow

- Wi-Fi or Ethernet connection is established, and IP address is obtained.
- Asio chat client connects to the corresponding server whose port number and IP are defined through the project configuration menu.
- Chat client receives all messages from other chat clients, also it sends message received from stdin using `idf.py -p PORT monitor`.

## Running the example

- Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.
- Set server IP address and port number in menuconfig, "Example configuration".
- Start chat server either on host machine or as another ESP device running chat_server example.
- Run `idf.py -p PORT flash monitor` to build and upload the example to your board and connect to it's serial terminal.
- Wait for the board to connect to WiFi or Ethernet.
- Receive and send messages to/from other clients on stdin/stdout via serial terminal.

See the README.md file in the upper level 'examples' directory for more information about examples.
