# PPP over Serial test

Testing connection of PPP client (ESP) to PPP server (raspberry pi)

## Pin Assignment

| ESP    | rpi            |
| ------ | -------------- |
| GPIO25 | UART-RX        |
| GPIO26 | UART-TX        |
| GND    | GND            |

## Test workflow

* Starts PPP server on a linux host
* Starts PPPoS client on ESP board
* Connects with both IPv4 and IPv6
* Test IPv6 connection
    - Start server on ESP
    - Have linux host to connect and exchange some data
* Test IPv4 connection
    - Start server on linux host
    - Connect with ESP to the server and exchange some data
* Pass/fail

## Manual test

### Server side

```
sudo pppd /dev/ttyAMA0 115200 CONFIG_TEST_APP_PPP_SERVER_IP:TEST_APP_PPP_CLIENT_IP  modem local noauth debug nodetach dump nocrtscts
```

or with `+ipv6` to support ipv6 assignment

### IPv6 test

Connect to the ESP local link address and send data, e.g.

```
nc fe80::74d7:edc5:9907:5457 2222
```

### IPv4 tst

Server side expects to run tcp server on port 2222

```
nc -l 2222
```

## Test environment

configuration `Example_PPP` see wikis/test-docs/example-test-env#example_ppp
