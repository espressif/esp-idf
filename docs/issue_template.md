----------------------------- Delete below -----------------------------

If your issue is a general question, starts similar to "How do I..", or is related to 3rd party development kits/libs, please discuss this on our community forum at esp32.com instead.

INSTRUCTIONS
============

Before submitting a new issue, please follow the checklist and try to find the answer.

- [ ] I have read the documentation [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/) and the issue is not addressed there.
- [ ] I have updated my IDF branch (master or release) to the latest version and checked that the issue is present there.
- [ ] I have searched the issue tracker for a similar issue and not found a similar issue.

If the issue cannot be solved after the steps before, please follow these instructions so we can get the needed information to help you in a quick and effective fashion.

1. Fill in all the fields under **Environment** marked with [ ] by picking the correct option for you in each case and deleting the others.
2. Describe your problem.
3. Include [debug logs on the monitor](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html#automatically-decoding-addresses) or the [coredump](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/core_dump.html).
4. Provide more items under **Other items if possible** can help us better locate your problem.
5. Use markup (buttons above) and the Preview tab to check what the issue will look like.
6. Delete these instructions from the above to the below marker lines before submitting this issue.

----------------------------- Delete above -----------------------------

## Environment

- Development Kit:      [ESP32-Wrover-Kit|ESP32-DevKitC|ESP32-PICO-Kit|ESP32-LyraT|ESP32-LyraTD-MSC|none]
- Kit version (for WroverKit/PicoKit/DevKitC): [v1|v2|v3|v4]
- Core (if using chip or module): [ESP32-Wrover-I|ESP32-Wrover|ESP32-PICO-D4|ESP32-SOLO-I|ESP32-WROOM-32U|ESP-WROOM-32D|ESP-WROOM32|ESP32]
- IDF version (``git rev-parse --short HEAD`` to get the commit id.): 
    //bd6ea4393c7d2f059fc4decc70f1ec3eb3597268
- Development Env:		[Arduino IDE|Make|Eclipse|other]
- Operating System:		[Windows|Ubuntu|MacOS]
- Power Supply:         [USB|external 5V|external 3.3V|Battery]

## Problem Description

//Detailed problem description goes here.

### Expected Behavior

### Actual Behavior

### Steps to repropduce

1. step1
2. ...

// It helps if you attach a picture of your setup/wiring here.


### Code to reproduce this issue

```cpp
// the code should be wrapped in the ```cpp tag so that it will be displayed better.
#include "esp_log.h"

void app_main()
{
    
}

```
// If your code is longer than 30 lines, [GIST](https://gist.github.com) is preferred.

## Debug Logs

```
Debug log goes here, should contain the backtrace, as well as the reset source if it is a crash.
Please copy the plain text here for us to search the error log. Or attach the complete logs but leave the main part here if the log is *too* long.
```

## Other items if possible

- [ ] sdkconfig file (attach the sdkconfig file from your project folder)
- [ ] elf file in the ``build`` folder (**note this may contain all the code details and symbols of your project.**)
- [ ] coredump (This provides stacks of tasks.) 

