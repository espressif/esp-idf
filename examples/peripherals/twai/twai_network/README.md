| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# TWAI Network Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the TWAI driver to program a target (ESP32, ESP32-S2, ESP32-S3 or ESP32-C3) as a TWAI node, and have the two nodes (Network Master and Network Slave) communicate on a TWAI network. The Listen Only node is optional and acts as a network monitor meaning that it only receives messages and does not influence the bus in any way (i.e. doesn't not acknowledge or send error frames).

Note that concept of master/slave in this example refers to which node initiates
and stops the transfer of a stream of data messages.

## How to use example

### Hardware Required

This example requires at least two targets (e.g., an ESP32 or ESP32-S2) to act as the Network Master and Network Slave. The third target (Listen Only) is optional. Each target must be connected to an external transceiver (e.g., a SN65HVD23X transceiver). The transceivers must then be interconnected to form a TWAI network.

The following diagram illustrates an example network:

```text
    ----------   ----------   --------------
   |  Master  | |  Slave   | | Listen Only  |
   |          | |          | |              |
   | TX    RX | | TX    RX | | TX    RX     |
    ----------   ----------   --------------
     |      |     |      |     |      |
     |      |     |      |     |      |
    ----------   ----------   ----------
   | D      R | | D      R | | D      R |
   |          | |          | |          |
   |  VP230   | |  VP230   | |  VP230   |
   |          | |          | |          |
   | H      L | | H      L | | H      L |
    ----------   ----------   ----------
     |      |     |      |     |      |
     |      |     |      |     |      |
  |--x------|-----x------|-----x------|--| H
            |            |            |
  |---------x------------x------------x--| L

```

Note: If you don't have an external transceiver, you can still run the [TWAI Self Test example](../twai_self_test/README.md)

### Configure the project

For each node in the TWAI network (i.e., Master, Slave, Listen Only)...

* Set the target of the build (where `{IDF_TARGET}` stands for the target chip such as `esp32` or `esp32s2`).
* Then run `menuconfig` to configure the example.

```sh
idf.py set-target {IDF_TARGET}
idf.py menuconfig
```

* Under `Example Configuration`, configure the pin assignments using the options `TX GPIO Number` and `RX GPIO Number` according to how the target was connected to the transceiver. By default, `TX GPIO Number` and `RX GPIO Number` are set to the following values:
  * On the ESP32, `TX GPIO Number` and `RX GPIO Number` default to `21` and `22` respectively
  * On other chips, `TX GPIO Number` and `RX GPIO Number` default to `0` and `2` respectively

### Build and Flash

For each node, build the project and flash it to the board, then run monitor tool to view serial output:

```sh
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Network Master

```text
I (345) TWAI Master: Driver installed
I (345) TWAI Master: Driver started
I (345) TWAI Master: Transmitting ping
I (3105) TWAI Master: Transmitted start command
I (3105) TWAI Master: Received data value 339
...
I (5545) TWAI Master: Received data value 584
I (5545) TWAI Master: Transmitted stop command
I (5595) TWAI Master: Driver stopped
I (6595) TWAI Master: Driver started
I (6595) TWAI Master: Transmitting ping
I (7095) TWAI Master: Transmitted start command
I (7095) TWAI Master: Received data value 738
...
I (9535) TWAI Master: Received data value 983
I (9535) TWAI Master: Transmitted stop command
I (9585) TWAI Master: Driver stopped
I (10585) TWAI Master: Driver started
I (10585) TWAI Master: Transmitting ping
I (11085) TWAI Master: Transmitted start command
I (11085) TWAI Master: Received data value 1137
...
I (13525) TWAI Master: Received data value 1382
I (13525) TWAI Master: Transmitted stop command
I (13575) TWAI Master: Driver stopped
I (14575) TWAI Master: Driver uninstalled
```

Network Slave

```text
Slave starting in 3
Slave starting in 2
Slave starting in 1
I (6322) TWAI Slave: Driver installed
I (6322) TWAI Slave: Driver started
I (6462) TWAI Slave: Transmitted ping response
I (6712) TWAI Slave: Start transmitting data
I (6712) TWAI Slave: Transmitted data value 339
...
I (9162) TWAI Slave: Transmitted data value 584
I (9212) TWAI Slave: Transmitted stop response
I (9312) TWAI Slave: Driver stopped
I (10312) TWAI Slave: Driver started
I (10452) TWAI Slave: Transmitted ping response
I (10702) TWAI Slave: Start transmitting data
I (10702) TWAI Slave: Transmitted data value 738
...
I (13152) TWAI Slave: Transmitted data value 983
I (13202) TWAI Slave: Transmitted stop response
I (13302) TWAI Slave: Driver stopped
I (14302) TWAI Slave: Driver started
I (14442) TWAI Slave: Transmitted ping response
I (14692) TWAI Slave: Start transmitting data
I (14692) TWAI Slave: Transmitted data value 1137
...
I (17142) TWAI Slave: Transmitted data value 1382
I (17192) TWAI Slave: Transmitted stop response
I (17292) TWAI Slave: Driver stopped
I (18292) TWAI Slave: Driver uninstalled
```

Network Listen Only

```text
I (326) TWAI Listen Only: Driver installed
I (326) TWAI Listen Only: Driver started
I (366) TWAI Listen Only: Received master ping
...
I (1866) TWAI Listen Only: Received master ping
I (1866) TWAI Listen Only: Received slave ping response
I (2116) TWAI Listen Only: Received master start command
I (2116) TWAI Listen Only: Received data value 329
...
I (4566) TWAI Listen Only: Received data value 574
I (4566) TWAI Listen Only: Received master stop command
I (4606) TWAI Listen Only: Received slave stop response
I (5606) TWAI Listen Only: Received master ping
I (5856) TWAI Listen Only: Received master ping
I (5856) TWAI Listen Only: Received slave ping response
I (6106) TWAI Listen Only: Received master start command
I (6106) TWAI Listen Only: Received data value 728
...
I (8556) TWAI Listen Only: Received data value 973
I (8556) TWAI Listen Only: Received master stop command
I (8596) TWAI Listen Only: Received slave stop response
I (9596) TWAI Listen Only: Received master ping
I (9846) TWAI Listen Only: Received master ping
I (9846) TWAI Listen Only: Received slave ping response
I (10096) TWAI Listen Only: Received master start command
I (10096) TWAI Listen Only: Received data value 1127
...
I (12546) TWAI Listen Only: Received data value 1372
I (12546) TWAI Listen Only: Received master stop command
I (12586) TWAI Listen Only: Received slave stop response
I (12586) TWAI Listen Only: Driver stopped
I (12586) TWAI Listen Only: Driver uninstalled

```

## Example Breakdown

The communication between the Network Master and Network Slave execute the following steps over multiple iterations:

1. Both master and slave go through install and start their TWAI drivers independently.
2. The master repeatedly sends **PING** messages until it receives a **PING_RESP** (ping response message) from the slave. The slave will only send a **PING_RESP** message when it receives a **PING** message from the master.
3. Once the master has received the **PING_RESP** from the slave, it will send a **START_CMD** message to the slave.
4. Upon receiving the **START_CMD** message, the slave will start transmitting **DATA** messages until the master sends a **STOP_CMD**. The master will send the **STOP_CMD** after receiving N **DATA** messages from the slave (N = 50 by default).
5. When the slave receives the **STOP_CMD**, it will confirm that it has stopped by sending a **STOP_RESP** message to the master.
