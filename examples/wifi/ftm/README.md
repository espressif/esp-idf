| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# FTM Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Introduction
One of the ways in which WiFi enabled devices can measure their distance to the Access Point is by measuring Wi-Fi Round Trip Time (Wi-Fi RTT). Wi-Fi RTT is the time a WiFi signal takes to travel from Station to an AP. This time is proportional to the actual distance between them. Given the RTT, the distance can be calculated with below simple formula -

> distance = RTT * c / 2
> (Where c is the speed of light)

Wi-Fi RTT is calculated using a procedure called Fine Timing Measurement(FTM). During FTM procedure, a burst of Action frames is transmitted by one device(FTM Responder) to another(FTM Initiator) and each of them is ACK'ed. Hardware in both the devices mark time-of-arrival (TOA) and time-of-departure (TOD) of both Action frame and its ACK. In the end, the FTM Initiator collects the data for all pairs of Action frame and ACK and calculates RTT for each pair with below formula -

> RTT[i] = (T4[i] - T1[i]) - (T3[i] - T2[i]) Where
> T1[i] : TOD of i'th Action frame from Responder
> T2[i] : TOA of i'th Action frame at Initiator
> T3[i] : TOD of i'th ACK from Initiator
> T4[i] : TOA of i'th ACK at Responder

Average RTT is calculated over all such pairs to get a more accurate result.
Use this example to perform FTM between a Station and a SoftAP or en external AP that supports FTM Responder mode. Both Station and SoftAP need to be run on the supported ESP targets that support FTM and have it enabled.

## How to use example

With this example, users can scan for AP's that support FTM Responder role and perform FTM procedure with different configurations. Below steps show how to do this using 2 devices in Station and SoftAP mode.
First make sure that `WiFi FTM` is enabled in the project configuration menu (`idf.py menuconfig`). This option is located in `Component config -> Wi-Fi`. `FTM Initiator support` needs to be enabled on Station side and `FTM Responder support` needs to be enabled on SoftAP side, both are enabled by default. Furthermore for getting a detailed report on FTM sessions, enable `FTM Report logging` option in the `Example Configuration`.
Build and flash the example on a supported device to see below output -

```bash
 ==========================================================
 |                      Steps to test FTM                 |
 |                                                        |
 |  1. Use 'help' for detailed information on parameters  |
 |  2. Start SoftAP with command 'ap <SSID> <password>'   |
 |                          OR                            |
 |  2. Use 'scan' command to search for external AP's     |
 |  3. On second device initiate FTM with an AP using     |
 |     command 'ftm -I -s <SSID>'                         |
 ==========================================================
ftm>
```

Use `help` to get a list of available commands and options. Use `scan` command to scan for AP's that support FTM Responder mode. Before initiating FTM with an external AP, make sure that `FTM Responder` tag is visible in the respective scan result entry. Alternatively, start SoftAP on another device using `ap` command, it supports FTM Responder by default. If external FTM Initiators get a large error in distance readings with the SoftAP, note down the reading at zero distance in centimeters, say `cm0`. This distance can be offset using command `ftm -R -o <cm0>` to give accurate readings with the Initiator.
It is recommended to keep SoftAP bandwidth at 20MHz as it gives more accurate results.

```bash
ftm> scan
I (476765) ftm_station: sta start to scan
ftm> I (478805) ftm_station: [Abeeys Palace][rssi=84]
I (478805) ftm_station: [privateproperty][rssi=76]
I (478805) ftm_station: [C904][rssi=69]
I (478815) ftm_station: [FTM][rssi=-94][FTM Responder]
I (478815) ftm_station: [Velop][rssi=-115]
I (478825) ftm_station: sta scan done
```

AP's that support FTM Responder mode can be seen in the scan results. Or setup a SoftAP on another device using the `ap` command -

```bash
ftm> ap FTM password
I (91271) ftm_ap: Starting SoftAP with FTM Responder support, SSID - FTM, Password - password
ftm>
```

Issue `ftm -I` to initiate a session with default configuration of 32 FTM frames. For more configurations below options are available -
`ftm  [-I] [-c <0/8/16/24/32/64>] [-p <2-255 (x 100 mSec)>] [-s SSID]`
Where -
* `-I` OR `--ftm_initiator`:  FTM Initiator mode
* `-c` OR `--frm_count`: FTM frames to be exchanged (Valid values: 0=No preference, 8, 16, 24, 32, 64, default: 32)
* `-p` OR `--burst_period`: Periodicity of FTM bursts in 100's of milliseconds (0: No preference, default: 2)
* `-s` OR `--ssid=SSID`: SSID of AP that supports FTM Responder mode

Currently FTM is only supported in below configuration -
1. Station as Initiator and SoftAP as Responder on supported ESP devices
2. Station as Initiator and an external AP that supports FTM in Responder mode
The first option should be preferred since ESP devices are self calibrated for high resolution measurement.

## Example Output
Example output of an FTM Procedure -

```bash
ftm> scan
I (356414) ftm_station: sta start to scan
I (358514) ftm_station: [DigitalFortress][rssi=114]
I (358524) ftm_station: [TEST][rssi=-96][FTM Responder]
I (358524) ftm_station: sta scan done
ftm> ftm -I -s TEST
Starting FTM with 18:fe:34:72:50:c9 on channel 1
I (391824) ftm_station: Starting FTM Initiator with Frm Count 32, Burst Period - 200mSec
W (391834) wifi:Starting FTM session in 0.200 Sec
W (393564) wifi:FTM session ends with 26 valid readings out of 31, Avg raw RTT: 49.218 nSec, Avg RSSI: -1
I (393564) ftm_station: Estimated RTT - 33 nSec, Estimated Distance - 5.07 meters
```

The final statement gives the average calculated RTT along with an estimated distance between the Station and the AP. This distance is measured by first adjusting the RTT with any physical analog delays and a calibration delta. Distances measured using RTT are not perfectly accurate, and are subjected to various errors like RF interference, multi-path, path loss, orientations etc.
The design requires line-of-sight with straightforward propagation path with no less than -70dBm RSSI for better results.
