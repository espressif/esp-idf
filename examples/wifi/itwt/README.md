| Supported Targets | ESP32-C5 | ESP32-C6 |
| ----------------- | -------- | -------- |

# Wifi itwt Example

## Introduction
This example shows how to use itwt of wifi.

Itwt only works in station mode. And AP needs to support the capability of itwt.

Itwt can works under different power save mode.

### Modem sleep (supported)
This is default mode. Under this mode, can support console command "itwt, probe" to config itwt.

*              itwt: this command is for itwt setup/teardown/suspend.
*              probe: this command will send a probe request to update tsf time with ap

### Light Sleep (will support)
Need system support light sleep. Console command will not support in this mode.

### Typical current consumption with Itwt enabled


### Typical current consumption with Itwt disabled


Note that current consumption and average current are higher when disabled.
