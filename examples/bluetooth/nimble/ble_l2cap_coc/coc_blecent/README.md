| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# BLE Central L2CAP COC Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example performs passive scan and then connects to peripheral device if the device advertises connectability and supports connection oriented channels.

It performs two operations against the specified peer:

* Connects L2CAP connection oriented channels.

* Sends data over formed L2CAP connection oriented channels.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding BLE service discovery, connection and data transfer over L2CAP layer through connection oriented channels.

Maximum data of 512 bytes can be transferred over L2CAP when MTU is set to 512 and MPS is 528.

Note :

* To install the dependency packages needed, please refer to the top level [README file](../../../../README.md#running-test-python-script-ttfw).
* Currently this Python utility is only supported on Linux (BLE communication is via BLuez + DBus).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project

Open the project configuration menu:

```bash
idf.py menuconfig
```

In the `Component Config` menu:

* Change Component config → Bluetooth → NimBLE Options → Maximum number of connection oriented channels to a value greater than 0
* Change Component config → Bluetooth → NimBLE Options → Memory Settings → MSYS_1 Block Size to 536 (For maximum transmission of data)

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection and 512 bytes of data sent over L2CAP connection oriented channel:

```
I (324) BTDM_INIT: BT controller compile version [d913766]
I (334) phy_init: phy_version 909,aa05aec,Apr 16 2022,13:42:08
I (424) system_api: Base MAC address is not set
I (424) system_api: read default base MAC address from EFUSE
I (424) BTDM_INIT: Bluetooth MAC: 84:f7:03:08:4d:8e

I (424) NimBLE_BLE_CENT_L2CAP_COC: BLE Host Task Started
I (434) NimBLE: GAP procedure initiated: stop advertising.

I (444) NimBLE: GAP procedure initiated: discovery;
I (444) NimBLE: own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1
I (454) NimBLE: duration=forever
I (454) NimBLE:

I (544) NimBLE: GAP procedure initiated: connect;
I (544) NimBLE: peer_addr_type=0 peer_addr=
I (544) NimBLE: 84:f7:03:05:a5:f6
I (544) NimBLE:  scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
I (554) NimBLE:

I (604) NimBLE: Connection established
I (604) NimBLE:

I (604) NimBLE: GATT procedure initiated: discover all services

I (774) NimBLE: GATT procedure initiated: discover all characteristics;
I (774) NimBLE: start_handle=1 end_handle=5

I (974) NimBLE: GATT procedure initiated: discover all characteristics;
I (974) NimBLE: start_handle=6 end_handle=9

I (1174) NimBLE: GATT procedure initiated: discover all characteristics;
I (1174) NimBLE: start_handle=10 end_handle=22

I (1474) NimBLE: GATT procedure initiated: discover all characteristics;
I (1474) NimBLE: start_handle=23 end_handle=65535

I (1774) NimBLE: GATT procedure initiated: discover all descriptors;
I (1774) NimBLE: chr_val_handle=8 end_handle=9

I (1874) NimBLE: GATT procedure initiated: discover all descriptors;
I (1874) NimBLE: chr_val_handle=14 end_handle=15

I (1974) NimBLE: GATT procedure initiated: discover all descriptors;
I (1974) NimBLE: chr_val_handle=19 end_handle=20

I (2074) NimBLE: GATT procedure initiated: discover all descriptors;
I (2074) NimBLE: chr_val_handle=27 end_handle=65535

LE COC connected, conn: 1, chan: 0x3fc93160, psm: 0x1002, scid: 0x0040, dcid: 0x0040, our_mps: 528, our_mtu: 512, peer_mps: 528, peer_mtu: 512
I (2284) NimBLE: Sending data :
 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255
 I (2444) NimBLE: Data sent successfully

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
