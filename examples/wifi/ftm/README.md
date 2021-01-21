| Supported Targets | ESP32-S2 |
| ----------------- | -------- |

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
In this example, FTM procedure is supported only between a Station(ESP32S2) and an AP it is connected to. The AP can be a SoftAP(ESP32S2) or an External AP that supports FTM Responder mode.

## How to use example

With this example, users can scan for AP's that support FTM Responder role, connect with them and perform FTM procedure with different configurations. Below steps show how to do this using 2 ESP32-S2's in Station and SoftAP mode.
First make sure that FTM Initiator support on Station and FTM Responder support on SoftAP is enabled in the example configuration menu. For this, open project configuration menu (`idf.py menuconfig`), navigate to `Component config -> Wi-Fi` and check `FTM Initiator support` on Station build and `FTM Responder support` on SoftAP build. Furthermore for getting a per frame detailed report of the FTM procedure, enable `FTM Report logging` option.
Build and flash the example on respective ESP32-S2's to see below output -

```bash
 ==========================================================
 |                      Steps to test FTM                 |
 |                                                        |
 |  1. Print 'help' to gain overview of commands          |
 |  2. Use 'scan' command for AP that support FTM         |
 |                          OR                            |
 |  2. Start SoftAP on another ESP32S2 with 'ap' command  |
 |  3. Setup connection with the AP using 'sta' command   |
 |  4. Initiate FTM from Station using 'ftm -I' command   |
 |                                                        |
 ==========================================================

ftm>
```

Use `help` to get a list of available commands and options. Use `scan` command to scan for AP's that support FTM Responder mode.
Before testing FTM with an external AP, make sure that `FTM Responder` is visible in the respective scan result entry.

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

AP's that support FTM Responder mode can be seen in the scan results. Or setup a SoftAP using another ESP32-S2 device using the `ap` command -

```bash
ftm> ap FTM password
I (91271) ftm_ap: Starting SoftAP with FTM Responder support, SSID - FTM, Password - password
ftm>
```

Use command `sta <ssid> [<pass>]` to connect with an eligible AP. Then simply issue `ftm -I` to initiate a session with default configuration of 32 FTM frames. For more configurations below options are available -
`ftm  [-I] [-c <0/16/24/32/64>] [-p <0-255 (x 100 mSec)>]`
Where -
* `-I` OR `--ftm_initiator`:  FTM Initiator mode
* `-c` OR `--frm_count`: FTM frames to be exchanged (Valid values: 0=No preference, 16, 24, 32, 64, default: 32)
* `-p` OR `--burst_period`: Periodicity of FTM bursts in 100's of miliseconds (0: No preference, default: 2)

Currently FTM is only supported in below configuration -
 1. Station(ESP32-S2) as Initiator in connected mode with SoftAP(ESP32-S2) as Responder
 2. Station(ESP32-S2) as Initiator in connected mode with external AP as Responder
The first option should be preferred since ESP32-S2 is self calibrated for high resolution measurement. Support for more configurations like STA to STA with ASAP mode will follow in future updates.

## Example Output
Example output of an FTM Procedure -

```bash
ftm> ftm -I
I (13796) ftm_station: Starting FTM Initiator with Frm Count 32, Burst Period - 200mSec
ftm> W (23696) wifi:FTM report:
W (23696) wifi:| Diag |   RTT   |  RSSI  |       T1       |       T2       |       T3       |       T4       |
W (23706) wifi:|     3|  24850  |   -18  |13598650592600  | 3973101843750  | 3973205662500  |13598754436200  |
W (23716) wifi:|     5|  32662  |   -18  |13600546592600  | 3974997826562  | 3975101662500  |13600650461200  |
W (23726) wifi:|     7|  31100  |   -18  |14498977692600  | 4873420043750  | 4873523662500  |14499081342450  |
W (23736) wifi:|     9|  29412  |   -18  |14500856692600  | 4875299026562  | 4875402662500  |14500960357950  |
W (23746) wifi:|    11|  24850  |   -18  |15399235817600  | 5773669262500  | 5773785662500  |15399352242450  |
W (23756) wifi:|    12|  27975  |   -18  |15400258817600  | 5774692253125  | 5774796662500  |15400363254950  |
W (23766) wifi:|    13|  26287  |   -18  |15401215817600  | 5775649242187  | 5775753662500  |15401320264200  |
W (23776) wifi:|    14|  24725  |   -18  |15402158817600  | 5776592234375  | 5776696662500  |15402263270450  |
W (23786) wifi:|    15|  24850  |   -18  |16298632917600  | 6673057456250  | 6673173662500  |16298749148700  |
W (23796) wifi:|    16|  29412  |   -18  |16299698917600  | 6674123445312  | 6674227662500  |16299803164200  |
W (23796) wifi:|    17|  26287  |   -18  |16300637917600  | 6675062435937  | 6675166662500  |16300742170450  |
W (23816) wifi:|    18|  27975  |   -18  |16301580917600  | 6676005428125  | 6676109662500  |16301685179950  |
W (23826) wifi:|    19|  27850  |   -18  |17198977042600  |  862506262500  |  862622262500  |17199093070450  |
W (23836) wifi:|    20|  23162  |   -18  |17200051042600  |  863580251562  |  863684262500  |17200155076700  |
W (23846) wifi:|    21|  31100  |   -18  |17200991042600  |  864520243750  |  864624262500  |17201095092450  |
W (23846) wifi:|    22|  26412  |   -18  |17201930042600  |  865459232812  |  865563262500  |17202034098700  |
W (23856) wifi:|    23|  31100  |   -18  |18099333142600  | 1762853443750  | 1762969262500  |18099448992450  |
W (23876) wifi:|    24|  23162  |   -18  |18100397142600  | 1763917432812  | 1764021262500  |18100500995450  |
W (23886) wifi:|    25|  26287  |   -18  |18101336142600  | 1764856423437  | 1764960262500  |18101440007950  |
W (23896) wifi:|    26|  24850  |   -18  |18102343142600  | 1765863412500  | 1765967262500  |18102447017450  |
W (23896) wifi:|    27|  31100  |   -18  |19000187242600  | 2663698618750  | 2663814262500  |19000302917450  |
W (23906) wifi:|    28|  24725  |   -18  |19001274242600  | 2664785609375  | 2664889262500  |19001377920450  |
W (23916) wifi:|    29|  31100  |   -18  |19002225242600  | 2665736600000  | 2665840262500  |19002328936200  |
W (23936) wifi:|    31|  23287  |   -19  |19004133242600  | 2667644579687  | 2667748262500  |19004236948700  |
W (23936) wifi:|    32|  26287  |   -18  |19899976367600  | 3563478835937  | 3563595262500  |19900092820450  |
W (23946) wifi:FTM session ends with 25 valid readings out of 31, Avg raw RTT: 27.232 nSec, Avg RSSI: -18
I (23956) ftm_station: Estimated RTT - 13 nSec, Estimated Distance - 1.95 meters
```

The final statement gives the average calculated RTT along with an estimated distance between the Station and the AP. This distance is measured by first adjusting the RTT with any physical analog delays and a calibration delta. Distances measured using RTT are not perfectly accurate, and are subjected to various errors like RF interference, multi-path, path loss, orientations etc.
The design requires line-of-sight with straightforward propagation path with no less than -70dBm RSSI for better results.
