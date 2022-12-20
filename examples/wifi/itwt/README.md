| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# Wifi itwt Example

## Introduction
This example shows how to use itwt of wifi.

Itwt only works in station mode. And AP needs to support the capability of itwt.

This example support command "itwt, itwt_probe, itwt_info" to config itwt.

* itwt: this command is for itwt setup/teardown.

* itwt_probe: this command will send a probe request to update tsf time with ap

* itwt_info: this command will send a TWT Information frame to AP for suspending/resuming extablished iTWT agreements.

### Typical current consumption with Itwt enabled



### Typical current consumption with Itwt disabled


Note that current consumption and average current are higher when disabled.
