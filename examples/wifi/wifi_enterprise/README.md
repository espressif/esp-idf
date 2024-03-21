| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |


# Understanding different WiFi enterprise modes:

**WPA2 Enterprise**: WPA2-Enterprise is an advanced Wi-Fi security mode primarily used in business environments. It employs a RADIUS server for user-based authentication, supporting various EAP methods like EAP-TLS and EAP-PEAP. This mode enhances security by requiring individual user credentials, establishes secure encryption keys, and allows for efficient user management. It's a scalable and robust solution ideal for large-scale networks seeking strong protection against unauthorized access.

**WPA3 Enterprise**: WPA2-Enterprise + PMF mandatory + CA certificate validaion(required)

**WPA3 Enterprise(192 bit)**: WPA3 Enterprise + AES256 Keys(GCMP256/CCMP256) + BIP256 + RSA3096/EC certs + NSA SuiteB ciphers in EAP authentication.

*Note:* 
Note that when using bigger certificates on low-power chips without crypto hardware acceleration, it is recommended to adjust the task watchdog timer (TWDT) if it is enabled. For precise information on timing requirements, you can check performance numbers at https://github.com/espressif/mbedtls/wiki/Performance-Numbers.

# WiFi Enterprise Example

This example shows how ESP32 connects to AP with Wi-Fi enterprise encryption. The example does the following steps:

1. Install CA certificate which is optional.
2. Install client certificate and client key which is required in TLS method and optional in PEAP and TTLS methods.
3. Set identity of phase 1 which is optional.
4. Set user name and password of phase 2 which is required in PEAP and TTLS methods.
5. Enable WiFi enterprise mode.
6. Connect to AP using esp_wifi_connect().

*Note:* 
1. The certificates currently are generated and are present in examples/wifi/wifi_enterprise/main folder.
2. The expiration date of the certificates is 2027/06/05.
3. In case using suite-b, please go into `generate_certs` directory, then execute the script as `python generate_certs.py <cert_type> sha384` to create appropriate certificates such as RSA-3072 or p384 EC certificates.

The steps to create new certificates are given below.

The file ca.pem, ca.key, server.pem, server.crt and server.key can be used to configure AP with enterprise encryption.

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
```
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
```
2. ca.pem: root certificate, foundation of certificate verigy
```
  openssl req -new -x509 -keyout ca.key -out ca.pem
```
3. generate rsa keys for client and server
```
  openssl genrsa -out client.key 2048
  openssl genrsa -out server.key 2048
```
4. generate certificate signing req for both client and server
```
  openssl req -new -key client.key -out client.csr
  openssl req -new -key server.key -out server.csr
```
5. create certs (.crt) for client nd server
```
  openssl ca -batch -keyfile ca.key -cert ca.pem -in client.csr -key (password) -out client.crt -extensions xpclient_ext -extfile xpextensions
  openssl ca -batch -keyfile ca.key -cert ca.pem -in server.csr -key (password) -out server.crt -extensions xpserver_ext -extfile xpextensions
```
6. export .p12 files
```
  openssl pkcs12 -export -out client.p12 -inkey client.key -in client.crt
  openssl pkcs12 -export -out server.p12 -inkey server.key -in server.crt
```
7. create .pem files
```
  openssl pkcs12 -in client.p12 -out client.pem
  openssl pkcs12 -in server.p12 -out server.pem
```


### Example output

I (31) boot: ESP-IDF v5.2-dev-2787-g40cf6433be-dirty 2nd stage bootloader
I (31) boot: compile time Sep 12 2023 13:39:03
I (33) boot: Multicore bootloader
I (38) boot: chip revision: v3.0
I (41) boot.esp32: SPI Speed      : 40MHz
I (46) boot.esp32: SPI Mode       : DIO
I (51) boot.esp32: SPI Flash Size : 2MB
I (55) boot: Enabling RNG early entropy source...
I (61) boot: Partition Table:
I (64) boot: ## Label            Usage          Type ST Offset   Length
I (71) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (79) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (86) boot:  2 factory          factory app      00 00 00010000 00100000
I (94) boot: End of partition table
I (98) esp_image: segment 0: paddr=00010020 vaddr=3f400020 size=2aa0ch (174604) map
I (170) esp_image: segment 1: paddr=0003aa34 vaddr=3ffb0000 size=037a0h ( 14240) load
I (175) esp_image: segment 2: paddr=0003e1dc vaddr=40080000 size=01e3ch (  7740) load
I (179) esp_image: segment 3: paddr=00040020 vaddr=400d0020 size=954c8h (611528) map
I (405) esp_image: segment 4: paddr=000d54f0 vaddr=40081e3c size=13de0h ( 81376) load
I (449) boot: Loaded app from partition at offset 0x10000
I (450) boot: Disabling RNG early entropy source...
I (461) cpu_start: Multicore app
I (461) cpu_start: Pro cpu up.
I (462) cpu_start: Starting app cpu, entry point is 0x400811e0
I (0) cpu_start: App cpu up.
I (479) cpu_start: Pro cpu start user code
I (479) cpu_start: cpu freq: 160000000 Hz
I (479) cpu_start: Application information:
I (484) cpu_start: Project name:     wifi_enterprise
I (490) cpu_start: App version:      c6_h2_rng_final_tests-2032-g40c
I (497) cpu_start: Compile time:     Sep 12 2023 13:38:55
I (503) cpu_start: ELF file SHA256:  8d1ba00d3...
I (508) cpu_start: ESP-IDF:          v5.2-dev-2787-g40cf6433be-dirty
I (515) cpu_start: Min chip rev:     v0.0
I (520) cpu_start: Max chip rev:     v3.99 
I (525) cpu_start: Chip rev:         v3.0
I (530) heap_init: Initializing. RAM available for dynamic allocation:
I (537) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (543) heap_init: At 3FFB7A20 len 000285E0 (161 KiB): DRAM
I (549) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (555) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (562) heap_init: At 40095C1C len 0000A3E4 (40 KiB): IRAM
I (570) spi_flash: detected chip: generic
I (573) spi_flash: flash io: dio
W (577) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (591) app_start: Starting scheduler on CPU0
I (595) app_start: Starting scheduler on CPU1
I (595) main_task: Started on CPU0
I (605) main_task: Calling app_main()
I (635) wifi:wifi driver task: 3ffbf930, prio:23, stack:6656, core=0
I (635) wifi:wifi firmware version: e03c1ca
I (635) wifi:wifi certification version: v7.0
I (635) wifi:config NVS flash: enabled
I (635) wifi:config nano formating: disabled
I (645) wifi:Init data frame dynamic rx buffer num: 32
I (645) wifi:Init management frame dynamic rx buffer num: 32
I (655) wifi:Init management short buffer num: 32
I (655) wifi:Init dynamic tx buffer num: 32
I (665) wifi:Init static rx buffer size: 1600
I (665) wifi:Init static rx buffer num: 10
I (665) wifi:Init dynamic rx buffer num: 32
I (675) wifi_init: rx ba win: 6
I (675) wifi_init: tcpip mbox: 32
I (675) wifi_init: udp mbox: 6
I (685) wifi_init: tcp mbox: 6
I (685) wifi_init: tcp tx win: 5744
I (695) wifi_init: tcp rx win: 5744
I (695) wifi_init: tcp mss: 1440
I (695) wifi_init: WiFi IRAM OP enabled
I (705) wifi_init: WiFi RX IRAM OP enabled
I (705) example: Setting WiFi configuration SSID ESP_ENTERPRISE_AP...
I (715) wpa: WiFi Enterprise enable
I (725) phy_init: phy_version 4771,450c73b,Aug 16 2023,11:03:10
I (835) wifi:mode : sta (e0:e2:e6:6a:7c:20)
I (845) wifi:enable tsf
I (845) main_task: Returned from app_main()
I (1465) wpa: BSS: Add new id 0 BSSID 38:94:ed:34:07:66 SSID 'ESP_ENTERPRISE_AP' chan 6
I (1465) wifi:new:<6,0>, old:<1,0>, ap:<255,255>, sta:<6,0>, prof:1
I (7385) wifi:state: init -> auth (b0)
I (7395) wifi:state: auth -> assoc (0)
I (7405) wifi:state: assoc -> run (10)
I (7405) wpa: wifi_task prio:7, stack:6656
I (7495) wpa: len=5 not available in input
I (7555) wpa: SSL: Need 1595 bytes more input data
I (7605) wpa: SSL: Need 601 bytes more input data
I (8575) wpa: len=5 not available in input
I (8695) wpa: application data is null, adding one byte for ack
I (8725) wpa: >>>>>EAP FINISH
I (8785) wifi:connected with ESP_ENTERPRISE_AP, aid = 1, channel 6, BW20, bssid = 38:94:ed:34:07:66
I (8785) wifi:security: WPA2-ENT, phy: bgn, rssi: -22
I (8785) wifi:pm start, type: 1

I (8795) wifi:AP's beacon interval = 102400 us, DTIM period = 3

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
