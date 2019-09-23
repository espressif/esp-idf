# mDNS example

Shows how to use mDNS to advertise lookup services and hosts

## Example workflow

- mDNS is initialized with host name and instance name defined through the project configuration and `_http._tcp` service is added to be advertised
- WiFi STA is started and trying to connect to the access point defined through the project configuration
- The system event handler is used to pass the network events to mDNS so the service is aware when the interface comes up or down
- GPIO0 (BOOT Button) is initialized as pulled-up input that can be monitored for button press
- Example task is started to check if the button is pressed so it can execute the mDNS queries defined

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)

* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../README.md) for more details.
* When using Make build system, set `Default serial port` under `Serial flasher config`.
* Set `mDNS Hostname` as host name prefix for the device and its instance name in `mDNS Instance Name`
* Disable `Resolve test services` to prevent the example from querying defined names/services on startup (cause warnings in example logs, as illustrated below)

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

- Wait for WiFi to connect to your access point
- You can now ping the device at `[board-hostname].local`, where `[board-hostname]` is preconfigured hostname, `esp32-mdns` by default.
- You can also browse for `_http._tcp` on the same network to find the advertised service
- Pressing the BOOT button will start querying the local network for the predefined in `check_button` hosts and services
- Note that for purpose of CI tests, configuration options of `MDNS_RESOLVE_TEST_SERVICES` and `MDNS_ADD_MAC_TO_HOSTNAME` are available, but disabled by default. If enabled, then the hostname suffix of last 3 bytes from device MAC address is added, e.g. `esp32-mdns-80FFFF`, and a query for test service is issued.


(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output
```
I (0) cpu_start: Starting scheduler on APP CPU.
I (276) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (276) mdns-test: mdns hostname set to: [esp32-mdns]
I (286) wifi: wifi driver task: 3ffc2fa4, prio:23, stack:3584, core=0
I (286) wifi: wifi firmware version: a3be639
I (286) wifi: config NVS flash: enabled
I (296) wifi: config nano formating: disabled
I (296) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (306) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (336) wifi: Init dynamic tx buffer num: 32
I (336) wifi: Init data frame dynamic rx buffer num: 32
I (336) wifi: Init management frame dynamic rx buffer num: 32
I (346) wifi: Init static rx buffer size: 1600
I (346) wifi: Init static rx buffer num: 10
I (346) wifi: Init dynamic rx buffer num: 32
I (356) mdns-test: Setting WiFi configuration SSID myssid...
I (426) phy: phy_version: 4000, b6198fa, Sep  3 2018, 15:11:06, 0, 0
I (426) wifi: mode : sta (30:ae:a4:80:FF:FF)
I (426) gpio: GPIO[0]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (1756) wifi: n:11 0, o:1 0, ap:255 255, sta:11 0, prof:1
I (2736) wifi: state: init -> auth (b0)
I (2756) wifi: state: auth -> assoc (0)
I (2766) wifi: state: assoc -> run (10)
I (2786) wifi: connected with myssid, channel 11
I (2786) wifi: pm start, type: 1

I (4786) event: sta ip: 192.168.0.139, mask: 255.255.255.0, gw: 192.168.0.2
I (21126) mdns-test: Query A: esp32.local
W (23176) mdns-test: ESP_ERR_NOT_FOUND: Host was not found!
I (23176) mdns-test: Query PTR: _arduino._tcp.local
W (26276) mdns-test: No results found!
I (26276) mdns-test: Query PTR: _http._tcp.local
1: Interface: STA, Type: V6
  PTR : HP Color LaserJet MFP M277dw (7C2E10)
  SRV : NPI7C2E10.local:80
  A   : 254.128.0.0
2: Interface: STA, Type: V4
  PTR : switch4e4919
  SRV : switch4e4919.local:80
  TXT : [1] path=/config/authentication_page.htm;
  A   : 192.168.0.118
I (29396) mdns-test: Query PTR: _printer._tcp.local
1: Interface: STA, Type: V6
  PTR : HP Color LaserJet MFP M277dw (7C2E10)
  SRV : NPI7C2E10.local:515
  A   : 254.128.0.0
2: Interface: STA, Type: V4
  PTR : HP Color LaserJet MFP M277dw (7C2E10)
```

See the README.md file in the upper level 'examples' directory for more information about examples.
