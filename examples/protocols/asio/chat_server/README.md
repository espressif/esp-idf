# Asio chat server example

Simple Asio chat server using WiFi STA or Ethernet.

## Example workflow

- Wi-Fi or Ethernet connection is established, and IP address is obtained.
- Asio chat server is started on port number defined through the project configuration.
- Chat server echoes a message (received from any client) to all connected clients.

## Running the example

- Open project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.
- Set server port number in menuconfig, "Example configuration".
- Run `idf.py -p PORT flash monitor` to build and upload the example to your board and connect to it's serial terminal.
- Wait for the board to connect to WiFi or Ethernet (note the IP address).
- Connect to the server using multiple clients, for example using any option below.
    - build and run asi chat client on your host machine
    - run chat_client asio example on ESP platform
    - since chat message consist of ascii size and message, it is possible to 
        netcat `nc IP PORT` and type for example `   4ABC<CR>` to transmit 'ABC\n'

See the README.md file in the upper level 'examples' directory for more information about examples.
