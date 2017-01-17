# WPA2 Enterprise Example

This example shows how ESP32 connects to AP with wpa2 enterprise encryption. Example does the following steps:

1. Install CA certificate which is optional.
2. Install client certificate and client key which is required in TLS method and optional in PEAP and TTLS methods.
3. Set identity of phase 1 which is optional.
4. Set user name and password of phase 2 which is required in PEAP and TTLS methods.
5. Enable wpa2 enterprise.
6. Connect to AP.

*Note:* certificate currently is generated when compiling the example and then stored in flash.

## The file wpa2_ca.pem, wpa2_ca.key, wpa2_server.pem, wpa2_server.crt and wpa2_server.key can be used to configure AP with
   wpa2 enterprise encryption. The steps how to generate new certificates and keys using openssl is as follows:
   
1. wpa2_ca.pem wpa2_ca.key:
    openssl req -new -x509 -keyout wpa2_ca.key -out wpa2_ca.pem
2. wpa2_server.key:
    openssl req -new -key wpa2_server.key -out wpa2_server.csr
3. wpa2_csr:
    openssl req -new -key server.key -out server.csr
4. wpa2_server.crt:
    openssl ca -batch -keyfile wpa2_ca.key -cert wpa2_ca.pem -in wpa2_server.csr -key ca1234 -out wpa2_server.crt -extensions xpserver_ext -extfile xpextensions
5. wpa2_server.p12:
    openssl pkcs12 -export -in wpa2_server.crt -inkey wpa2_server.key -out wpa2_server.p12 -passin pass:sv1234 -passout pass:sv1234
6. wpa2_server.pem:
    openssl pkcs12 -in wpa2_server.p12 -out wpa2_server.pem -passin pass:sv1234 -passout pass:sv1234
7. wpa2_client.key:
    openssl genrsa -out wpa2_client.key 1024
8. wpa2_client.csr:
    openssl req -new -key wpa2_client.key -out wpa2_client.csr
9. wpa2_client.crt:
    openssl ca -batch -keyfile wpa2_ca.key -cert wpa2_ca.pem -in wpa2_client.csr -key ca1234 -out wpa2_client.crt -extensions xpclient_ext -extfile xpextensions
10. wpa2_client.p12:
    openssl pkcs12 -export -in wpa2_client.crt -inkey wpa2_client.key -out wpa2_client.p12
11. wpa2_client.pem:
    openssl pkcs12 -in wpa2_client.p12 -out wpa2_client.pem

### Example output

Here is an example of wpa2 enterprise(PEAP method) console output.

I (1352) example: Setting WiFi configuration SSID wpa2_test...
I (1362) wpa: WPA2 ENTERPRISE VERSION: [v2.0] enable

I (1362) wifi: rx_ba=1 tx_ba=1

I (1372) wifi: mode : sta (24:0a:c4:03:b8:dc)
I (3002) wifi: n:11 0, o:1 0, ap:255 255, sta:11 0, prof:11
I (3642) wifi: state: init -> auth (b0)
I (3642) wifi: state: auth -> assoc (0)
I (3652) wifi: state: assoc -> run (10)
I (3652) wpa: wpa2_task prio:24, stack:6144

I (3972) wpa: >>>>>wpa2 FINISH

I (3982) wpa: wpa2 task delete

I (3992) wifi: connected with wpa2_test, channel 11
I (5372) example: ~~~~~~~~~~~
I (5372) example: IP:0.0.0.0
I (5372) example: MASK:0.0.0.0
I (5372) example: GW:0.0.0.0
I (5372) example: ~~~~~~~~~~~
I (6832) event: ip: 192.168.1.112, mask: 255.255.255.0, gw: 192.168.1.1
I (7372) example: ~~~~~~~~~~~
I (7372) example: IP:192.168.1.112
I (7372) example: MASK:255.255.255.0
I (7372) example: GW:192.168.1.1
I (7372) example: ~~~~~~~~~~~
I (9372) example: ~~~~~~~~~~~
I (9372) example: IP:192.168.1.112
I (9372) example: MASK:255.255.255.0
I (9372) example: GW:192.168.1.1
I (9372) example: ~~~~~~~~~~~
