# WPA2 Enterprise Example

This example shows how ESP32 connects to AP with Wi-Fi enterprise encryption. The example does the following steps:

1. Install CA certificate which is optional.
2. Install client certificate and client key which is required in TLS method and optional in PEAP and TTLS methods.
3. Set identity of phase 1 which is optional.
4. Set user name and password of phase 2 which is required in PEAP and TTLS methods.
5. Enable wpa2 enterprise.
6. Connect to AP.

*Note:* 1. The certificates currently are generated and are present in examples/wifi/wifi_enterprise/main folder.
        2. The expiration date of the certificates is 2027/06/05.
        3. In case using suite-b, please use appropriate certificates such as RSA-3072 or p384 EC certificates.

The steps to create new certificates are given below.

## The file ca.pem, ca.key, server.pem, server.crt and server.key can be used to configure AP with enterprise encryption.

## How to use Example

### Configuration

```
idf.py menuconfig
```
* Set SSID of Access Point to connect in Example Configuration.
* Select EAP method (TLS, TTLS or PEAP).
* Select Phase2 method (only for TTLS).
* Enter EAP-ID.
* Enter Username and Password (only for TTLS and PEAP).
* Enable or disable Validate Server option.

### Build and Flash the project.

```
idf.py -p PORT flash monitor
```

## Steps to create enterprise openssl certs

1. make directry tree

  mkdir demoCA
  mkdir demoCA/newcerts
  mkdir demoCA/private
  sh -c "echo '01' > ./demoCA/serial"
  touch ./demoCA/index.txt
  touch xpextensions 

     add following lines in xpextensions file 

      [ xpclient_ext ]
      extendedKeyUsage = 1.3.6.1.5.5.7.3.2

      [ xpserver_ext ]
      extendedKeyUsage = 1.3.6.1.5.5.7.3.1

2. ca.pem: root certificate, foundation of certificate verigy
  openssl req -new -x509 -keyout ca.key -out ca.pem

3. generate rsa keys for client and server
  openssl genrsa -out client.key 2048
  openssl genrsa -out server.key 2048

4. generate certificate signing req for both client and server
  openssl req -new -key client.key -out client.csr
  openssl req -new -key server.key -out server.csr

5. create certs (.crt) for client nd server
  openssl ca -batch -keyfile ca.key -cert ca.pem -in client.csr -key (password) -out client.crt -extensions xpserver_ext -extfile xpextensions
  openssl ca -batch -keyfile ca.key -cert ca.pem -in server.csr -key (password) -out server.crt -extensions xpserver_ext -extfile xpextensions

6. export .p12 files
  openssl pkcs12 -export -out client.p12 -inkey client.key -in client.crt
  openssl pkcs12 -export -out server.p12 -inkey server.key -in server.crt

7. create .pem files
  openssl pkcs12 -in client.p12 -out client.pem
  openssl pkcs12 -in server.p12 -out server.pem

   

### Example output

Here is an example of wpa2 enterprise(PEAP method) console output.
```
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
```
