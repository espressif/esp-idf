| Supported Targets | ESP32 | ESP32-S2 | ESP32-C3 |
| ----------------- | ----- | -------- | -------- |

# ESP-OPENSSL connect test project

Main purpose of this application is to test the ESP-OPENSSL library to correctly connect/refuse connectio with TLS servers.
It is possible to run this example manually without any test to exercise ESP-OPENSSL library.

## Runtime settings
This app waits for user input to provide these parameters:
 test-type: "conn" connection test (host, port, test-case)

## Hardware Required
This test-app can be executed on any ESP32 board, the only required interface is WiFi and connection to a local network and tls server.
