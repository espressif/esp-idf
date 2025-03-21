| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- |

# Partition OTA Example

This example demonstrates how to perform OTA updates for various partitions using the `esp_https_ota` component's APIs. The following partitions can be updated:
- Application (app)
- Bootloader
- Partition Table
- Storage

This example can be easily modified to perform OTA updates for any partition type.

> ⚠️ **WARNING**: Updating the bootloader and partition table is not safe. If the final copying to the destination partition is interrupted, the chip may fail to boot. Ensure that the final copy occurs only when power is stable and there is a low risk of errors.

## OTA Update
Currently, there is no rollback mechanism for non-application partitions. In case of failure, only the application partition can safely rollback.

### Application

Application OTA updates use one active and one passive partition. The new image is downloaded into the passive partition. Once the download is complete and verified, the device switches to the new image. This approach ensures that unexpected reboots during the update do not render the device unusable.

### Bootloader

Bootloader OTA updates are not inherently safe because the ROM bootloader does not support fallback to a recovery bootloader partition. Only the primary bootloader partition can be loaded by the ROM bootloader. Updating the bootloader is rarely necessary, and it is generally not recommended. However, if required, it can be done using the following approaches:

- Register the primary bootloader partition in the partition table, if not already present (see `test/partitions_efuse_emul_2.csv`).
- Decide where to download the new bootloader image:
  - Use a passive app partition, if app rollback is not used at the same time.
  - Use a dedicated OTA bootloader partition (type=bootloader, subtype=ota). You can find unallocated flash space with `partition_utils_find_unallocated()`.
  - Download directly into the primary bootloader partition (this approach is highly unsafe).

After verification, if `finalize_with_copy` is set to `true`, the tool will automatically copy the new image to the primary bootloader partition. Set `finalize_with_copy` to `false` if you wish to control the final copy step manually.

Limitations for Bootloader OTA updates:
- Secure Boot V1-enabled devices do not support bootloader updates.
- There is always a risk of device bricking when updating the bootloader.

### Partition Table

Updating the partition table via OTA is similarly unsafe because the bootloader cannot use a backup partition table in case of failure. If the update fails, the device will be unable to boot. Partition table updates are rarely needed, and caution is strongly advised. The workflow for partition table updates is the same as for the bootloader.

### Storage

Updating storage partitions via OTA is also risky due to the potential for data loss in the partition. There is no fallback mechanism if an error occurs during the update. The workflow for Storage updates is the same as for the bootloader.

## Configuration

Refer to the README.md file in the parent directory for setup instructions.

This example supports binding to a specific network interface ("Ethernet" or "WiFi Station") for firmware updates. You can configure this in `idf.py menuconfig -> Example Configuration -> Support firmware upgrade bind specified interface -> Choose OTA data bind interface`.

The CSV partition table file may include these entries at the beginning (see `test/partitions_efuse_emul_2.csv`):
```
PrimaryBTLDR, bootloader, primary, N/A, N/A, encrypted
PrimaryPrtTable, partition_table, primary, N/A, N/A, encrypted
```

## Example output

Running the exapmple with sdkconfig.ci.on_update_no_sb_rsa settings:

```
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2820,len:0x15a0
load:0x403c8700,len:0x4
load:0x403c8704,len:0xd20
load:0x403cb700,len:0x2f3c
entry 0x403c8928
I (27) boot: ESP-IDF 95a1e4cd 2nd stage bootloader
I (27) boot: compile time Oct 28 2024 15:26:41
I (27) boot: Multicore bootloader
I (27) boot: chip revision: v0.1
I (30) boot: efuse block revision: v1.2
I (34) boot.esp32s3: Boot SPI Speed : 80MHz
I (37) boot.esp32s3: SPI Mode       : DIO
I (41) boot.esp32s3: SPI Flash Size : 4MB
I (45) boot: Enabling RNG early entropy source...
I (49) boot: Partition Table:
I (52) boot: ## Label            Usage          Type ST Offset   Length
I (58) boot:  0 nvs              WiFi data        01 02 0000d000 00006000
I (65) boot:  1 nvs_key          NVS keys         01 04 00013000 00001000
I (71) boot:  2 storage          Unknown data     01 06 00014000 00001000
I (78) boot:  3 otadata          OTA data         01 00 00015000 00002000
I (84) boot:  4 phy_init         RF data          01 01 00017000 00001000
I (91) boot:  5 emul_efuse       efuse            01 05 00018000 00002000
I (97) boot:  6 ota_0            OTA app          00 10 00020000 001b0000
I (104) boot:  7 ota_1            OTA app          00 11 001d0000 001b0000
I (111) boot: End of partition table
I (114) boot: No factory image, trying OTA 0
I (118) esp_image: segment 0: paddr=00020020 vaddr=3c090020 size=24288h (148104) map
I (152) esp_image: segment 1: paddr=000442b0 vaddr=3fc98900 size=04970h ( 18800) load
I (156) esp_image: segment 2: paddr=00048c28 vaddr=40374000 size=073f0h ( 29680) load
I (162) esp_image: segment 3: paddr=00050020 vaddr=42000020 size=8fc00h (588800) map
I (267) esp_image: segment 4: paddr=000dfc28 vaddr=4037b3f0 size=0d4ach ( 54444) load
I (279) esp_image: segment 5: paddr=000ed0dc vaddr=600fe100 size=0001ch (    28) load
I (287) boot: Loaded app from partition at offset 0x20000
I (429) boot: Set actual ota_seq=1 in otadata[0]
I (430) boot: Disabling RNG early entropy source...
I (439) cpu_start: Multicore app
I (449) cpu_start: Pro cpu start user code
I (449) cpu_start: cpu freq: 160000000 Hz
I (449) app_init: Application information:
I (449) app_init: Project name:     partitions_ota
I (453) app_init: App version:      95a1e4cd
I (457) app_init: Compile time:     Oct 28 2024 15:26:38
I (462) app_init: ELF file SHA256:  0810d7fd3...
I (467) app_init: ESP-IDF:          95a1e4cd
I (471) efuse_init: Min chip rev:     v0.0
I (475) efuse_init: Max chip rev:     v0.99 
I (479) efuse_init: Chip rev:         v0.1
I (482) heap_init: Initializing. RAM available for dynamic allocation:
I (489) heap_init: At 3FCA14A0 len 00048270 (288 KiB): RAM
I (494) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (499) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (504) heap_init: At 600FE11C len 00001ECC (7 KiB): RTCRAM
I (511) spi_flash: detected chip: generic
I (513) spi_flash: flash io: dio
W (516) spi_flash: Detected size(8192k) larger than the size in the binary image header(4096k). Using the size in the binary image header.
I (530) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (535) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (542) main_task: Started on CPU0
I (562) main_task: Calling app_main()
I (562) ota_example: OTA example app_main start
I (572) example_connect: Start example_connect.
I (572) pp: pp rom version: e7ae62f
I (572) net80211: net80211 rom version: e7ae62f
I (592) wifi:wifi driver task: 3fcab44c, prio:23, stack:6656, core=0
I (592) wifi:wifi firmware version: 2d9c351b0
I (592) wifi:wifi certification version: v7.0
I (592) wifi:config NVS flash: enabled
I (592) wifi:config nano formatting: disabled
I (602) wifi:Init data frame dynamic rx buffer num: 32
I (602) wifi:Init static rx mgmt buffer num: 5
I (612) wifi:Init management short buffer num: 32
I (612) wifi:Init dynamic tx buffer num: 32
I (612) wifi:Init static tx FG buffer num: 2
I (622) wifi:Init static rx buffer size: 1600
I (622) wifi:Init static rx buffer num: 10
I (632) wifi:Init dynamic rx buffer num: 32
I (632) wifi_init: rx ba win: 6
I (632) wifi_init: accept mbox: 6
I (642) wifi_init: tcpip mbox: 32
I (642) wifi_init: udp mbox: 6
I (642) wifi_init: tcp mbox: 6
I (642) wifi_init: tcp tx win: 5760
I (652) wifi_init: tcp rx win: 5760
I (652) wifi_init: tcp mss: 1440
I (652) wifi_init: WiFi IRAM OP enabled
I (662) wifi_init: WiFi RX IRAM OP enabled
I (662) phy_init: phy_version 680,a6008b2,Jun  4 2024,16:41:10
W (672) phy_init: failed to load RF calibration data (0x1102), falling back to full calibration
W (702) phy_init: saving new calibration data because of checksum failure, mode(2)
I (722) wifi:mode : sta (68:b6:b3:4d:91:08)
I (722) wifi:enable tsf
I (722) example_connect: Please input ssid password:
I (752) example_connect: Connecting to myssid...
W (752) wifi:Password length matches WPA2 standards, authmode threshold changes from OPEN to WPA2
I (762) example_connect: Waiting for IP(s)
I (3592) wifi:new:<2,1>, old:<1,0>, ap:<255,255>, sta:<2,1>, prof:1, snd_ch_cfg:0x0
I (3592) wifi:state: init -> auth (0xb0)
I (3592) wifi:state: auth -> assoc (0x0)
I (3602) wifi:state: assoc -> run (0x10)
I (3622) wifi:connected with myssid, aid = 3, channel 2, 40U, bssid = 22:d8:d0:45:23:7a
I (3622) wifi:security: WPA2-PSK, phy: bgn, rssi: -15
I (3622) wifi:pm start, type: 1

I (3632) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (3632) wifi:set rx beacon pti, rx_bcn_pti: 0, bcn_timeout: 25000, mt_pti: 0, mt_time: 10000
I (3722) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (4722) esp_netif_handlers: example_netif_sta ip: 192.168.3.26, mask: 255.255.255.0, gw: 192.168.3.1
I (4722) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.3.26
I (4732) example_common: Connected to example_netif_sta
I (4732) example_common: - IPv4 address: 192.168.3.26,
I (4742) wifi:Set ps type: 0, coexist: 0

I (4742) ota_example: Starting OTA example task
I (4742) ota_example: Bind interface name is st1
I (4752) main_task: Returned from app_main()
I (4762) ota_example: Attempting to download update from https://192.168.3.9:8000/partitions_ota.bin
I (4782) wifi:<ba-add>idx:0 (ifx:0, 22:d8:d0:45:23:7a), tid:0, ssn:1, winSize:64
I (4812) esp-x509-crt-bundle: Certificate validated
I (5142) esp_https_ota: Starting OTA...
I (5142) esp_https_ota: Writing to <ota_1> partition at offset 0x1d0000
I (29702) esp_image: segment 0: paddr=001d0020 vaddr=3c090020 size=24288h (148104) map
I (29722) esp_image: segment 1: paddr=001f42b0 vaddr=3fc98900 size=04970h ( 18800) 
I (29732) esp_image: segment 2: paddr=001f8c28 vaddr=40374000 size=073f0h ( 29680) 
I (29732) esp_image: segment 3: paddr=00200020 vaddr=42000020 size=8fc00h (588800) map
I (29822) esp_image: segment 4: paddr=0028fc28 vaddr=4037b3f0 size=0d4ach ( 54444) 
I (29832) esp_image: segment 5: paddr=0029d0dc vaddr=600fe100 size=0001ch (    28) 
I (29832) esp_image: Verifying image signature...
I (29842) secure_boot_v2: Take trusted digest key(s) from running app
I (29852) secure_boot_v2: #0 app key digest == #0 trusted key digest
I (29852) secure_boot_v2: Verifying with RSA-PSS...
I (29912) secure_boot_v2_rsa: Signature verified successfully!
I (29912) esp_image: segment 0: paddr=001d0020 vaddr=3c090020 size=24288h (148104) map
I (29932) esp_image: segment 1: paddr=001f42b0 vaddr=3fc98900 size=04970h ( 18800) 
I (29942) esp_image: segment 2: paddr=001f8c28 vaddr=40374000 size=073f0h ( 29680) 
I (29942) esp_image: segment 3: paddr=00200020 vaddr=42000020 size=8fc00h (588800) map
I (30032) esp_image: segment 4: paddr=0028fc28 vaddr=4037b3f0 size=0d4ach ( 54444) 
I (30042) esp_image: segment 5: paddr=0029d0dc vaddr=600fe100 size=0001ch (    28) 
I (30042) esp_image: Verifying image signature...
I (30042) secure_boot_v2: Take trusted digest key(s) from running app
I (30062) secure_boot_v2: #0 app key digest == #0 trusted key digest
I (30062) secure_boot_v2: Verifying with RSA-PSS...
I (30122) secure_boot_v2_rsa: Signature verified successfully!
I (30292) ota_example: OTA Succeed, Rebooting...
I (30292) wifi:state: run -> init (0x0)
I (30292) wifi:pm stop, total sleep time: 737814 us / 26659208 us

I (30292) wifi:<ba-del>idx:0, tid:0
I (30302) wifi:new:<2,0>, old:<2,1>, ap:<255,255>, sta:<2,1>, prof:1, snd_ch_cfg:0x0
I (30352) wifi:flush txq
I (30352) wifi:stop sw txq
I (30352) wifi:lmac stop hw txq
I (30352) wifi:Deinit lldesc rx mblock:10

ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0xc (RTC_SW_CPU_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
Saved PC:0x40376c31
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2820,len:0x15a0
load:0x403c8700,len:0x4
load:0x403c8704,len:0xd20
load:0x403cb700,len:0x2f3c
entry 0x403c8928
I (31) boot: ESP-IDF 95a1e4cd 2nd stage bootloader
I (31) boot: compile time Oct 28 2024 15:26:41
I (31) boot: Multicore bootloader
I (32) boot: chip revision: v0.1
I (34) boot: efuse block revision: v1.2
I (38) boot.esp32s3: Boot SPI Speed : 80MHz
I (42) boot.esp32s3: SPI Mode       : DIO
I (46) boot.esp32s3: SPI Flash Size : 4MB
I (49) boot: Enabling RNG early entropy source...
I (54) boot: Partition Table:
I (56) boot: ## Label            Usage          Type ST Offset   Length
I (63) boot:  0 nvs              WiFi data        01 02 0000d000 00006000
I (69) boot:  1 nvs_key          NVS keys         01 04 00013000 00001000
I (76) boot:  2 storage          Unknown data     01 06 00014000 00001000
I (82) boot:  3 otadata          OTA data         01 00 00015000 00002000
I (89) boot:  4 phy_init         RF data          01 01 00017000 00001000
I (95) boot:  5 emul_efuse       efuse            01 05 00018000 00002000
I (102) boot:  6 ota_0            OTA app          00 10 00020000 001b0000
I (108) boot:  7 ota_1            OTA app          00 11 001d0000 001b0000
I (115) boot: End of partition table
I (118) esp_image: segment 0: paddr=001d0020 vaddr=3c090020 size=24288h (148104) map
I (152) esp_image: segment 1: paddr=001f42b0 vaddr=3fc98900 size=04970h ( 18800) load
I (156) esp_image: segment 2: paddr=001f8c28 vaddr=40374000 size=073f0h ( 29680) load
I (163) esp_image: segment 3: paddr=00200020 vaddr=42000020 size=8fc00h (588800) map
I (267) esp_image: segment 4: paddr=0028fc28 vaddr=4037b3f0 size=0d4ach ( 54444) load
I (279) esp_image: segment 5: paddr=0029d0dc vaddr=600fe100 size=0001ch (    28) load
I (288) boot: Loaded app from partition at offset 0x1d0000
I (288) boot: Disabling RNG early entropy source...
I (298) cpu_start: Multicore app
I (308) cpu_start: Pro cpu start user code
I (308) cpu_start: cpu freq: 160000000 Hz
I (308) app_init: Application information:
I (308) app_init: Project name:     partitions_ota
I (312) app_init: App version:      95a1e4cd
I (316) app_init: Compile time:     Oct 28 2024 15:26:38
I (321) app_init: ELF file SHA256:  0810d7fd3...
I (326) app_init: ESP-IDF:          95a1e4cd
I (330) efuse_init: Min chip rev:     v0.0
I (334) efuse_init: Max chip rev:     v0.99 
I (338) efuse_init: Chip rev:         v0.1
I (341) heap_init: Initializing. RAM available for dynamic allocation:
I (348) heap_init: At 3FCA14A0 len 00048270 (288 KiB): RAM
I (353) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (358) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (363) heap_init: At 600FE11C len 00001ECC (7 KiB): RTCRAM
I (370) spi_flash: detected chip: generic
I (372) spi_flash: flash io: dio
W (375) spi_flash: Detected size(8192k) larger than the size in the binary image header(4096k). Using the size in the binary image header.
I (389) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (394) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (401) main_task: Started on CPU0
I (421) main_task: Calling app_main()
I (421) ota_example: OTA example app_main start
I (431) example_connect: Start example_connect.
I (431) pp: pp rom version: e7ae62f
I (431) net80211: net80211 rom version: e7ae62f
I (441) wifi:wifi driver task: 3fcab48c, prio:23, stack:6656, core=0
I (441) wifi:wifi firmware version: 2d9c351b0
I (441) wifi:wifi certification version: v7.0
I (451) wifi:config NVS flash: enabled
I (451) wifi:config nano formatting: disabled
I (451) wifi:Init data frame dynamic rx buffer num: 32
I (461) wifi:Init static rx mgmt buffer num: 5
I (461) wifi:Init management short buffer num: 32
I (471) wifi:Init dynamic tx buffer num: 32
I (471) wifi:Init static tx FG buffer num: 2
I (471) wifi:Init static rx buffer size: 1600
I (481) wifi:Init static rx buffer num: 10
I (481) wifi:Init dynamic rx buffer num: 32
I (491) wifi_init: rx ba win: 6
I (491) wifi_init: accept mbox: 6
I (491) wifi_init: tcpip mbox: 32
I (501) wifi_init: udp mbox: 6
I (501) wifi_init: tcp mbox: 6
I (501) wifi_init: tcp tx win: 5760
I (501) wifi_init: tcp rx win: 5760
I (511) wifi_init: tcp mss: 1440
I (511) wifi_init: WiFi IRAM OP enabled
I (511) wifi_init: WiFi RX IRAM OP enabled
I (521) phy_init: phy_version 680,a6008b2,Jun  4 2024,16:41:10
I (561) wifi:mode : sta (68:b6:b3:4d:91:08)
I (561) wifi:enable tsf
I (561) example_connect: Please input ssid password:
I (611) example_connect: Connecting to myssid...
W (611) wifi:Password length matches WPA2 standards, authmode threshold changes from OPEN to WPA2
I (611) example_connect: Waiting for IP(s)
I (3441) wifi:new:<2,1>, old:<1,0>, ap:<255,255>, sta:<2,1>, prof:1, snd_ch_cfg:0x0
I (3441) wifi:state: init -> auth (0xb0)
I (3451) wifi:state: auth -> assoc (0x0)
I (3451) wifi:state: assoc -> run (0x10)
I (3471) wifi:connected with myssid, aid = 3, channel 2, 40U, bssid = 22:d8:d0:45:23:7a
I (3471) wifi:security: WPA2-PSK, phy: bgn, rssi: -15
I (3471) wifi:pm start, type: 1

I (3491) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (3491) wifi:set rx beacon pti, rx_bcn_pti: 0, bcn_timeout: 25000, mt_pti: 0, mt_time: 10000
I (3571) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (4591) esp_netif_handlers: example_netif_sta ip: 192.168.3.26, mask: 255.255.255.0, gw: 192.168.3.1
I (4591) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.3.26
I (4601) example_common: Connected to example_netif_sta
I (4601) example_common: - IPv4 address: 192.168.3.26,
I (4611) wifi:Set ps type: 0, coexist: 0

I (4611) ota_example: Starting OTA example task
I (4611) ota_example: Bind interface name is st1
I (4621) main_task: Returned from app_main()
I (4671) ota_example: Attempting to download update from https://192.168.3.9:8000/bootloader/bootloader.bin
I (4671) ota_example: Use <PrimaryBTLDR> partition (0x00000000)
I (4701) wifi:<ba-add>idx:0 (ifx:0, 22:d8:d0:45:23:7a), tid:0, ssn:1, winSize:64
I (4731) esp-x509-crt-bundle: Certificate validated
I (5061) esp_https_ota: Starting OTA...
I (5061) esp_https_ota: Writing to <ota_0> partition at offset 0x20000
I (5061) esp_ota_ops: Staging partition - <ota_0>. Final partition - <PrimaryBTLDR>.
I (5071) esp_image: Bootloader offsets for PRIMARY: 0x0, Secondary: 0x20000
I (6121) esp_image: segment 0: paddr=00020020 vaddr=3fce2820 size=015a0h (  5536) 
I (6121) esp_image: segment 1: paddr=000215c8 vaddr=403c8700 size=00004h (     4) 
I (6131) esp_image: segment 2: paddr=000215d4 vaddr=403c8704 size=00d20h (  3360) 
I (6131) esp_image: segment 3: paddr=000222fc vaddr=403cb700 size=02f3ch ( 12092) 
I (6151) esp_image: Bootloader offsets for PRIMARY: 0x0, Secondary: 0x0
W (6151) ota_example: Ensure stable power supply! Loss of power at this stage leads to a chip bricking
I (6151) ota_example: Copy from <ota_0> staging partition to <PrimaryBTLDR>...
I (7801) ota_example: OTA Succeed, Rebooting...
I (7801) wifi:state: run -> init (0x0)
I (7801) wifi:pm stop, total sleep time: 742270 us / 4318054 us

I (7801) wifi:<ba-del>idx:0, tid:0
I (7821) wifi:new:<2,0>, old:<2,1>, ap:<255,255>, sta:<2,1>, prof:1, snd_ch_cfg:0x0
I (7831) wifi:flush txq
I (7831) wifi:stop sw txq
I (7831) wifi:lmac stop hw txq
I (7831) wifi:Deinit lldesc rx mblock:10

ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0xc (RTC_SW_CPU_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
Saved PC:0x40376c31
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2820,len:0x15a0
load:0x403c8700,len:0x4
load:0x403c8704,len:0xd20
load:0x403cb700,len:0x2f3c
entry 0x403c8928
I (31) boot: ESP-IDF 95a1e4cd 2nd stage bootloader
I (31) boot: compile time Oct 28 2024 15:26:41
I (31) boot: Multicore bootloader
I (32) boot: chip revision: v0.1
I (34) boot: efuse block revision: v1.2
I (38) boot.esp32s3: Boot SPI Speed : 80MHz
I (42) boot.esp32s3: SPI Mode       : DIO
I (46) boot.esp32s3: SPI Flash Size : 4MB
I (49) boot: Enabling RNG early entropy source...
I (54) boot: Partition Table:
I (56) boot: ## Label            Usage          Type ST Offset   Length
I (63) boot:  0 nvs              WiFi data        01 02 0000d000 00006000
I (69) boot:  1 nvs_key          NVS keys         01 04 00013000 00001000
I (76) boot:  2 storage          Unknown data     01 06 00014000 00001000
I (82) boot:  3 otadata          OTA data         01 00 00015000 00002000
I (89) boot:  4 phy_init         RF data          01 01 00017000 00001000
I (95) boot:  5 emul_efuse       efuse            01 05 00018000 00002000
I (102) boot:  6 ota_0            OTA app          00 10 00020000 001b0000
I (108) boot:  7 ota_1            OTA app          00 11 001d0000 001b0000
I (115) boot: End of partition table
I (118) esp_image: segment 0: paddr=001d0020 vaddr=3c090020 size=24288h (148104) map
I (152) esp_image: segment 1: paddr=001f42b0 vaddr=3fc98900 size=04970h ( 18800) load
I (156) esp_image: segment 2: paddr=001f8c28 vaddr=40374000 size=073f0h ( 29680) load
I (163) esp_image: segment 3: paddr=00200020 vaddr=42000020 size=8fc00h (588800) map
I (267) esp_image: segment 4: paddr=0028fc28 vaddr=4037b3f0 size=0d4ach ( 54444) load
I (279) esp_image: segment 5: paddr=0029d0dc vaddr=600fe100 size=0001ch (    28) load
I (288) boot: Loaded app from partition at offset 0x1d0000
I (288) boot: Disabling RNG early entropy source...
I (298) cpu_start: Multicore app
I (308) cpu_start: Pro cpu start user code
I (308) cpu_start: cpu freq: 160000000 Hz
I (308) app_init: Application information:
I (308) app_init: Project name:     partitions_ota
I (312) app_init: App version:      95a1e4cd
I (316) app_init: Compile time:     Oct 28 2024 15:26:38
I (321) app_init: ELF file SHA256:  0810d7fd3...
I (326) app_init: ESP-IDF:          95a1e4cd
I (330) efuse_init: Min chip rev:     v0.0
I (334) efuse_init: Max chip rev:     v0.99 
I (338) efuse_init: Chip rev:         v0.1
I (341) heap_init: Initializing. RAM available for dynamic allocation:
I (348) heap_init: At 3FCA14A0 len 00048270 (288 KiB): RAM
I (353) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (358) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (363) heap_init: At 600FE11C len 00001ECC (7 KiB): RTCRAM
I (370) spi_flash: detected chip: generic
I (372) spi_flash: flash io: dio
W (375) spi_flash: Detected size(8192k) larger than the size in the binary image header(4096k). Using the size in the binary image header.
I (389) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (394) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (401) main_task: Started on CPU0
I (421) main_task: Calling app_main()
I (421) ota_example: OTA example app_main start
I (431) example_connect: Start example_connect.
I (431) pp: pp rom version: e7ae62f
I (431) net80211: net80211 rom version: e7ae62f
I (441) wifi:wifi driver task: 3fcab48c, prio:23, stack:6656, core=0
I (441) wifi:wifi firmware version: 2d9c351b0
I (441) wifi:wifi certification version: v7.0
I (451) wifi:config NVS flash: enabled
I (451) wifi:config nano formatting: disabled
I (451) wifi:Init data frame dynamic rx buffer num: 32
I (461) wifi:Init static rx mgmt buffer num: 5
I (461) wifi:Init management short buffer num: 32
I (471) wifi:Init dynamic tx buffer num: 32
I (471) wifi:Init static tx FG buffer num: 2
I (471) wifi:Init static rx buffer size: 1600
I (481) wifi:Init static rx buffer num: 10
I (481) wifi:Init dynamic rx buffer num: 32
I (491) wifi_init: rx ba win: 6
I (491) wifi_init: accept mbox: 6
I (491) wifi_init: tcpip mbox: 32
I (501) wifi_init: udp mbox: 6
I (501) wifi_init: tcp mbox: 6
I (501) wifi_init: tcp tx win: 5760
I (501) wifi_init: tcp rx win: 5760
I (511) wifi_init: tcp mss: 1440
I (511) wifi_init: WiFi IRAM OP enabled
I (511) wifi_init: WiFi RX IRAM OP enabled
I (521) phy_init: phy_version 680,a6008b2,Jun  4 2024,16:41:10
I (561) wifi:mode : sta (68:b6:b3:4d:91:08)
I (561) wifi:enable tsf
I (561) example_connect: Please input ssid password:
I (601) example_connect: Connecting to myssid...
W (601) wifi:Password length matches WPA2 standards, authmode threshold changes from OPEN to WPA2
I (601) example_connect: Waiting for IP(s)
I (3431) wifi:new:<2,1>, old:<1,0>, ap:<255,255>, sta:<2,1>, prof:1, snd_ch_cfg:0x0
I (3431) wifi:state: init -> auth (0xb0)
I (3431) wifi:state: auth -> assoc (0x0)
I (3441) wifi:state: assoc -> run (0x10)
I (3461) wifi:connected with myssid, aid = 1, channel 2, 40U, bssid = 22:d8:d0:45:23:7a
I (3461) wifi:security: WPA2-PSK, phy: bgn, rssi: -15
I (3461) wifi:pm start, type: 1

I (3471) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (3471) wifi:set rx beacon pti, rx_bcn_pti: 0, bcn_timeout: 25000, mt_pti: 0, mt_time: 10000
I (3511) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (4571) esp_netif_handlers: example_netif_sta ip: 192.168.3.26, mask: 255.255.255.0, gw: 192.168.3.1
I (4571) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.3.26
I (4581) example_common: Connected to example_netif_sta
I (4581) example_common: - IPv4 address: 192.168.3.26,
I (4591) wifi:Set ps type: 0, coexist: 0

I (4591) ota_example: Starting OTA example task
I (4591) ota_example: Bind interface name is st1
I (4601) main_task: Returned from app_main()
I (4621) ota_example: Attempting to download update from https://192.168.3.9:8000/partition_table/partition-table.bin
I (4631) ota_example: Use <PrimaryPrtTable> partition (0x0000c000)
I (4651) wifi:<ba-add>idx:0 (ifx:0, 22:d8:d0:45:23:7a), tid:0, ssn:1, winSize:64
I (4681) esp-x509-crt-bundle: Certificate validated
I (5021) esp_https_ota: Starting OTA...
I (5021) esp_https_ota: Writing to <ota_0> partition at offset 0x20000
I (5021) esp_ota_ops: Staging partition - <ota_0>. Final partition - <PrimaryPrtTable>.
W (5181) ota_example: Ensure stable power supply! Loss of power at this stage leads to a chip bricking.
I (5181) ota_example: Copy from <ota_0> staging partition to <PrimaryPrtTable>...
I (5261) ota_example: OTA Succeed, Rebooting...
I (5261) wifi:state: run -> init (0x0)
I (5261) wifi:pm stop, total sleep time: 715566 us / 1792923 us

I (5261) wifi:<ba-del>idx:0, tid:0
I (5281) wifi:new:<2,0>, old:<2,1>, ap:<255,255>, sta:<2,1>, prof:1, snd_ch_cfg:0x0
I (5291) wifi:flush txq
I (5291) wifi:stop sw txq
I (5291) wifi:lmac stop hw txq
I (5291) wifi:Deinit lldesc rx mblock:10

ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0xc (RTC_SW_CPU_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
Saved PC:0x40376c31
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce2820,len:0x15a0
load:0x403c8700,len:0x4
load:0x403c8704,len:0xd20
load:0x403cb700,len:0x2f3c
entry 0x403c8928
I (31) boot: ESP-IDF 95a1e4cd 2nd stage bootloader
I (31) boot: compile time Oct 28 2024 15:26:41
I (31) boot: Multicore bootloader
I (32) boot: chip revision: v0.1
I (34) boot: efuse block revision: v1.2
I (38) boot.esp32s3: Boot SPI Speed : 80MHz
I (42) boot.esp32s3: SPI Mode       : DIO
I (46) boot.esp32s3: SPI Flash Size : 4MB
I (49) boot: Enabling RNG early entropy source...
I (54) boot: Partition Table:
I (56) boot: ## Label            Usage          Type ST Offset   Length
I (63) boot:  0 nvs              WiFi data        01 02 0000d000 00006000
I (69) boot:  1 nvs_key          NVS keys         01 04 00013000 00001000
I (76) boot:  2 storage          Unknown data     01 06 00014000 00001000
I (82) boot:  3 otadata          OTA data         01 00 00015000 00002000
I (89) boot:  4 phy_init         RF data          01 01 00017000 00001000
I (95) boot:  5 emul_efuse       efuse            01 05 00018000 00002000
I (102) boot:  6 ota_0            OTA app          00 10 00020000 001b0000
I (108) boot:  7 ota_1            OTA app          00 11 001d0000 001b0000
I (115) boot: End of partition table
I (118) esp_image: segment 0: paddr=001d0020 vaddr=3c090020 size=24288h (148104) map
I (152) esp_image: segment 1: paddr=001f42b0 vaddr=3fc98900 size=04970h ( 18800) load
I (156) esp_image: segment 2: paddr=001f8c28 vaddr=40374000 size=073f0h ( 29680) load
I (163) esp_image: segment 3: paddr=00200020 vaddr=42000020 size=8fc00h (588800) map
I (267) esp_image: segment 4: paddr=0028fc28 vaddr=4037b3f0 size=0d4ach ( 54444) load
I (279) esp_image: segment 5: paddr=0029d0dc vaddr=600fe100 size=0001ch (    28) load
I (288) boot: Loaded app from partition at offset 0x1d0000
I (288) boot: Disabling RNG early entropy source...
I (298) cpu_start: Multicore app
I (308) cpu_start: Pro cpu start user code
I (308) cpu_start: cpu freq: 160000000 Hz
I (308) app_init: Application information:
I (308) app_init: Project name:     partitions_ota
I (312) app_init: App version:      95a1e4cd
I (316) app_init: Compile time:     Oct 28 2024 15:26:38
I (321) app_init: ELF file SHA256:  0810d7fd3...
I (326) app_init: ESP-IDF:          95a1e4cd
I (330) efuse_init: Min chip rev:     v0.0
I (334) efuse_init: Max chip rev:     v0.99 
I (338) efuse_init: Chip rev:         v0.1
I (341) heap_init: Initializing. RAM available for dynamic allocation:
I (348) heap_init: At 3FCA14A0 len 00048270 (288 KiB): RAM
I (353) heap_init: At 3FCE9710 len 00005724 (21 KiB): RAM
I (358) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (363) heap_init: At 600FE11C len 00001ECC (7 KiB): RTCRAM
I (370) spi_flash: detected chip: generic
I (372) spi_flash: flash io: dio
W (375) spi_flash: Detected size(8192k) larger than the size in the binary image header(4096k). Using the size in the binary image header.
I (389) sleep_gpio: Configure to isolate all GPIO pins in sleep state
I (394) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (401) main_task: Started on CPU0
I (421) main_task: Calling app_main()
I (421) ota_example: OTA example app_main start
I (431) example_connect: Start example_connect.
I (431) pp: pp rom version: e7ae62f
I (431) net80211: net80211 rom version: e7ae62f
I (441) wifi:wifi driver task: 3fcab48c, prio:23, stack:6656, core=0
I (441) wifi:wifi firmware version: 2d9c351b0
I (441) wifi:wifi certification version: v7.0
I (451) wifi:config NVS flash: enabled
I (451) wifi:config nano formatting: disabled
I (451) wifi:Init data frame dynamic rx buffer num: 32
I (461) wifi:Init static rx mgmt buffer num: 5
I (461) wifi:Init management short buffer num: 32
I (471) wifi:Init dynamic tx buffer num: 32
I (471) wifi:Init static tx FG buffer num: 2
I (471) wifi:Init static rx buffer size: 1600
I (481) wifi:Init static rx buffer num: 10
I (481) wifi:Init dynamic rx buffer num: 32
I (491) wifi_init: rx ba win: 6
I (491) wifi_init: accept mbox: 6
I (491) wifi_init: tcpip mbox: 32
I (501) wifi_init: udp mbox: 6
I (501) wifi_init: tcp mbox: 6
I (501) wifi_init: tcp tx win: 5760
I (501) wifi_init: tcp rx win: 5760
I (511) wifi_init: tcp mss: 1440
I (511) wifi_init: WiFi IRAM OP enabled
I (511) wifi_init: WiFi RX IRAM OP enabled
I (521) phy_init: phy_version 680,a6008b2,Jun  4 2024,16:41:10
I (561) wifi:mode : sta (68:b6:b3:4d:91:08)
I (561) wifi:enable tsf
I (561) example_connect: Please input ssid password:
I (591) example_connect: Connecting to myssid...
W (591) wifi:Password length matches WPA2 standards, authmode threshold changes from OPEN to WPA2
I (591) example_connect: Waiting for IP(s)
I (3421) wifi:new:<2,1>, old:<1,0>, ap:<255,255>, sta:<2,1>, prof:1, snd_ch_cfg:0x0
I (3421) wifi:state: init -> auth (0xb0)
I (3431) wifi:state: auth -> assoc (0x0)
I (3431) wifi:state: assoc -> run (0x10)
I (3451) wifi:connected with myssid, aid = 1, channel 2, 40U, bssid = 22:d8:d0:45:23:7a
I (3451) wifi:security: WPA2-PSK, phy: bgn, rssi: -15
I (3451) wifi:pm start, type: 1

I (3461) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (3461) wifi:set rx beacon pti, rx_bcn_pti: 0, bcn_timeout: 25000, mt_pti: 0, mt_time: 10000
I (3541) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (4551) esp_netif_handlers: example_netif_sta ip: 192.168.3.26, mask: 255.255.255.0, gw: 192.168.3.1
I (4551) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.3.26
I (4561) example_common: Connected to example_netif_sta
I (4561) example_common: - IPv4 address: 192.168.3.26,
I (4571) wifi:Set ps type: 0, coexist: 0

I (4571) ota_example: Starting OTA example task
I (4571) ota_example: Bind interface name is st1
I (4581) main_task: Returned from app_main()
I (4601) ota_example: Attempting to download update from https://192.168.3.9:8000/storage.bin
I (4621) wifi:<ba-add>idx:0 (ifx:0, 22:d8:d0:45:23:7a), tid:0, ssn:1, winSize:64
I (4641) esp-x509-crt-bundle: Certificate validated
I (4981) esp_https_ota: Starting OTA...
I (4981) esp_https_ota: Writing to <ota_0> partition at offset 0x20000
I (4981) esp_ota_ops: Staging partition - <ota_0>. Final partition - <storage>.
I (5161) esp_ota_ops: Copy from <ota_0> staging partition to <storage>...
I (5351) ota_example: 7296406769363431
I (5351) ota_example: OTA Succeed, Rebooting...
I (5361) wifi:state: run -> init (0x0)
I (5361) wifi:pm stop, total sleep time: 772888 us / 1893437 us

I (5371) wifi:<ba-del>idx:0, tid:0
I (5371) wifi:new:<2,0>, old:<2,1>, ap:<255,255>, sta:<2,1>, prof:1, snd_ch_cfg:0x0
```