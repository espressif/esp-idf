# ASIO chat server example

Simple asio chat server using WiFi STA

## Example workflow

- WiFi STA is started and trying to connect to the access point defined through `make menuconfig`
- Once connected and acquired IP address, ASIO chat server is started on port number defined through `make menuconfig`
- Chat server echoes a message (received from any client) to all connected clients

## Running the example

- Run `make menuconfig` to configure the access point's SSID and Password and port number
- Run `make flash monitor` to build and upload the example to your board and connect to it's serial terminal
- Wait for WiFi to connect to your access point (note the IP address)
- Connect to the server using multiple clients, for example using any option below
    - build and run asi chat client on your host machine
    - run chat_client asio example on ESP platform
    - since chat message consist of ascii size and message, it is possible to 
        netcat `nc IP PORT` and type for example `   4ABC<CR>` to transmit 'ABC\n'

See the README.md file in the upper level 'examples' directory for more information about examples.
