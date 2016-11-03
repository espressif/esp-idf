# WiFi AP scaning Example

Starts a FreeRTOS task to scan the Wi-Fi APs every 6s:

```bash
ets Jun  8 2016 00:22:57

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
ets Jun  8 2016 00:22:57

rst:0x10 (RTCWDT_RTC_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0x00
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3ffc0000,len:0
load:0x3ffc0000,len:920
load:0x40078000,len:2724
ho 0 tail 12 room 4
load:0x40098000,len:508
entry 0x40098118
I (92) heap_alloc_caps: Initializing heap allocator:
I (92) heap_alloc_caps: Region 19: 3FFC01F0 len 0001FE10 tag 0
I (93) heap_alloc_caps: Region 25: 3FFE8000 len 00018000 tag 1
I (103) cpu_start: Pro cpu up.
I (108) cpu_start: Single core mode
I (114) cpu_start: Pro cpu start user code
rtc v112 Sep 26 2016 22:32:10
XTAL 40M
I (149) cpu_start: Starting scheduler on PRO CPU.
frc2_timer_task_hdl:3ffc3a24, prio:22, stack:2048
tcpip_task_hdlxxx : 3ffc49ec, prio:18,stack:2048
phy_version: 123, Sep 13 2016, 20:01:58, 0
pp_task_hdl : 3ffc8244, prio:23, stack:8192
mode : sta(24:0a:c4:01:cd:6c)
Welcome to Noduino Quantum
WiFi AP SSID Scanning... 
Scanned 13 APs 
                         Noduino (e0:46:90:80:32:47) rssi: -58 auth: 02
                      MJY_3F74DE (5e:cf:7f:02:0d:44) rssi: -66 auth: 00
                             608 (08:bd:43:86:fa:a2) rssi: -81 auth: 03
                      JCG-2C9160 (04:5f:a7:2c:91:60) rssi: -82 auth: 04
                         xmy1997 (fc:d7:33:f2:47:0e) rssi: -83 auth: 04
                    TP-LINK_CB78 (14:75:90:a4:cb:78) rssi: -84 auth: 04
          gehua01141311300376643 (08:10:78:2a:63:f9) rssi: -87 auth: 03
                             708 (a8:15:4d:e0:67:90) rssi: -89 auth: 04
                   FYX_ECB4_2.4G (ec:6c:9f:39:ec:b4) rssi: -89 auth: 03
                  TP-LINK_65FBFC (88:25:93:65:fb:fc) rssi: -89 auth: 04
                    TP-LINK_1028 (80:89:17:db:10:28) rssi: -89 auth: 04
                    Guest_65FBFC (8c:25:93:65:fb:fc) rssi: -90 auth: 04
                           Yati1 (f4:83:cd:54:50:f5) rssi: -92 auth: 04
------------ delay 6s to start a new scanning ... --------------
```

See the README.md file in the upper level 'examples' directory for more information about examples.
