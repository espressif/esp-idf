| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# BLE Peripheral with ICMP Echo-Reply

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example aims to run `ping` network utility along with BLE GATT server simultaneously using NimBLE host stack. It is a combination of 2 examples in IDF: `bluetooth/nimble/bleprph` and `protocols/icmp_echo`. See the README.md files of these examples to know more about them.

### BLE peripheral

This example creates GATT server and then starts advertising, waiting to be connected to a GATT client.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

### ICMP Echo-Reply

Ping is a useful network utility used to test if a remote host is reachable on the IP network. It measures the round-trip time for messages sent from the source host to a destination target that are echoed back to the source.

Ping operates by sending Internet Control Message Protocol (ICMP) echo request packets to the target host and waiting for an ICMP echo reply.

**Notes:** Currently this example only supports IPv4.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Configure the project

Open the project configuration menu: 

```bash
idf.py menuconfig
```

In the `Example Configuration` menu:

* Enter SSID and password of known Wi-Fi AP with connectivity to internet.
* Enter desired ping IP Address. Default is set to `93.184.216.34` ( This is the IP address of https://example.com ).

* Enter other related parameters like count of ping and maximum numbers of retry.

## Testing

To test this demo, any BLE scanner app and a WiFi access point with internet connectivity can be used.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
