
# Asio chat client and server examples

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application aims to demonstrate a simple use of Asio library in different modes.
In project settings it could be configured to run either a Asio chat server, a Asio chat client, or both.

## How to use example

The example is configured by default as an Asio chat client.

Note that the example uses string representation of IP addresses and ports.

You can find the upstream asio chat implementation [here] https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/chat

### Asio Client

In the client mode, the example connects to the configured address, sends the message, which was inserted as an input in the terminal, and receives a response.

### Asio Server

In the server mode, Asio chat server with a specified port number is created and being polled till a connection request from the client arrives.
Chat server echoes a message (received from any client) to all connected clients.

## Configure the project

```
idf.py menuconfig
```

Set following parameters under Example Configuration Options:

* Set `EXAMPLE_CHAT_SERVER` to use the example as an ASIO chat server
    * Configure `EXAMPLE_CHAT_SERVER_BIND_PORT` to the port number.

* Set `EXAMPLE_CHAT_CLIENT` to use the example as an ASIO chat client
    * Configure `EXAMPLE_CHAT_CLIENT_CONNECT_ADDRESS` to a string representation of the address to connect the client to.
    * Configure `EXAMPLE_CHAT_CLIENT_CONNECT_PORT` to the port number.

* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more d    etails.

## Running the example in server mode

- Configure the example according "Configure the project" section.
- Run `idf.py -p PORT flash monitor` to build and upload the example to your board and connect to it's serial terminal.
- Wait for the board to connect to WiFi or Ethernet (note the IP address).
- Connect to the server using multiple clients, for example using any option below.
    - build and run asio chat client on your host machine
    - run chat_client asio example on ESP platform
    - since chat messages consists of ASCII size and message, it is possible to 
        netcat `nc IP PORT` and type for example ` 4ABC<CR>` to transmit 'ABC\n'

## Running the example in client mode

- Configure the example according "Configure the project" section.
- Start chat server either on host machine or as another ESP device running chat_server example.
- Run `idf.py -p PORT flash monitor` to build and upload the example to your board and connect to it's serial terminal.
- Wait for the board to connect to WiFi or Ethernet.
- Receive and send messages to/from other clients on stdin/stdout via serial terminal.

See the README.md file in the upper level 'examples' directory for more information about examples.
