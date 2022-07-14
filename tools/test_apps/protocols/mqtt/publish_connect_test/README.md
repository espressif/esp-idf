| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 |
| ----------------- | ----- | -------- | -------- |

# ESP-MQTT advanced publish and connect test project

Main purpose of this application is to test the MQTT library to correctly publish and receive messages (of different size and sequences) over different transports.
It is possible to run this example manually without any test to exercise how the MQTT library deals with

- reception of fragmented messages
- runtime updates of URI

## Runtime settings

This app waits for user input to provide these parameters:
- test-type: "conn" if connection test (host, port, test-case number)
- publish test:
  * transport: string parameter, one of: tcp, ssl, ws, wss
  * pattern: sample string to be transmitted as message
  * pattern repeats: number of repeats of pattern in one MQTT message
  * repeated: number of repeats ESP32 publishes the message, also ESP32 expects to receive the same message the same number of repeats
  * qos: number specifying qos, one of: 0, 1, 2

## Hardware Required

This test-app can be executed on any ESP32 board, the only required interface is WiFi and connection to a local network, then depending on the test either a mqtt test broker or a tls server.
