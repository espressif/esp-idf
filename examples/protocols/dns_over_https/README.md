| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

# DNS over HTTPS Example

This example demonstrates how to use the DNS over HTTPS (DoH) component in an ESP32 application. The example resolves domain names securely via HTTPS using the configured DNS over HTTPS provider (Google(default), Cloudflare, or a custom server).

## Features

- **DNS over HTTPS**: Resolves domain names securely using HTTPS.

## Certificate Options
This example provides two certificate options for DNS over HTTPS:

1. Internal Certificate Bundle (Default): By default, the example uses an internal certificate bundle, making it easy to get started with popular DoH providers like Google and Cloudflare.
2. Custom Certificate: If you prefer to use your own DoH server and certificate, you can configure the server name in menuconfig and place your custom certificate in the `cert_custom_root.pem` file. This option provides flexibility if you have specific security or server requirements.

To configure the certificate option in menuconfig, navigate to `Example DNS-over-HTTPS Configuration → Use internal certificate bundle` to enable or disable the internal bundle.

## Configuration

Before building and running the example, you need to configure the DNS over HTTPS provider in `menuconfig`:

1. Run `idf.py menuconfig`.
2. Look for the **Example DNS-over-HTTPS Configuration** section.
3. Choose your preferred DNS server:
    * Google DNS (default: dns.google)
    * Cloudflare DNS (cloudflare-dns.com)
    * Custom DNS-over-HTTPS Server
4. For custom DNS configuration enter the custom DNS-over-HTTPS server URL.
5. Specify a custom DNS-over-HTTPS query path (default: dns-query).
6. Toggle whether to use the internal certificate bundle:
    * If disabled, specify the DNS certificate (cert_custom_root.pem).
7. Save your changes and exit the configuration menu.
8. Configure Wi-Fi or Ethernet to join a network. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

## How It Works

1. **Network Initialization**: The application initializes the network interfaces (Wi-Fi or Ethernet) and establishes a connection.
2. **NVS Initialization**: Non-Volatile Storage (NVS) is initialized to store and retrieve system time across reboots.
3. **DNS over HTTPS Initialization**: The `init_dns_over_https()` function initializes the DNS over HTTPS resolver, which securely handles DNS queries using HTTPS.
4. **Performing getaddrinfo**: The application executes the getaddrinfo operation for several domain names.

## How to use example
Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Troubleshooting Tips

* **Connectivity**:
	Ensure that the network connection details are accurate. For example, verify the Wi-Fi SSID and password or check that the Ethernet connection is secure and not faulty.

* **Stack Overflow Errors**:
	If you encounter a stack overflow, it might indicate that the DNS server is returning a lengthy error message. To diagnose the issue, try increasing the stack size to retrieve more detailed information.
 * **Incorrect Path Issues**:
	Occasionally, stack overflow error is caused by an incorrect server path. Verify the server details and ensure the server path is correctly configured.


## Example Output

```
I (4652) esp_netif_handlers: example_netif_sta ip: 192.168.50.136, mask: 255.255.255.0, gw: 192.168.50.1
I (4652) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.50.136
I (5552) example_connect: Got IPv6 event: Interface "example_netif_sta" address: fe80:0000:0000:0000:5abf:25ff:fee0:4100, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5552) example_common: Connected to example_netif_sta
I (5562) example_common: - IPv4 address: 192.168.50.136,
I (5562) example_common: - IPv6 address: fe80:0000:0000:0000:5abf:25ff:fee0:4100, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5572) time_sync: Updating time from NVS
I (5582) main_task: Returned from app_main()
I (5592) wifi:<ba-add>idx:1 (ifx:0, a0:36:bc:0e:c4:f0), tid:7, ssn:3, winSize:64
I (5622) wifi:<ba-del>idx:0, tid:6
I (5622) wifi:<ba-add>idx:0 (ifx:0, a0:36:bc:0e:c4:f0), tid:0, ssn:1, winSize:64
I (5962) esp-x509-crt-bundle: Certificate validated
I (6772) example_dns_over_https: Using DNS Over HTTPS server: dns.google
I (6772) example_dns_over_https: Resolving IP addresses for yahoo.com:
I (6772) example_dns_over_https: IPv4: 74.6.143.26
I (6782) example_dns_over_https: IPv4: 74.6.231.21
I (6782) example_dns_over_https: IPv4: 98.137.11.163
I (6792) example_dns_over_https: IPv4: 74.6.231.20

I (7192) esp-x509-crt-bundle: Certificate validated
I (8382) example_dns_over_https: Using DNS Over HTTPS server: dns.google
I (8382) example_dns_over_https: Resolving IP addresses for www.google.com:
I (8382) example_dns_over_https: IPv6: 2404:6800:4015:803::2004

I (9032) esp-x509-crt-bundle: Certificate validated
I (9862) example_dns_over_https: Using DNS Over HTTPS server: dns.google
I (9862) example_dns_over_https: Resolving IP addresses for www.google.com:
I (9862) example_dns_over_https: IPv4: 142.250.70.228
```
