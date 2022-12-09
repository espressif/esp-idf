---
name: Bug report
about: ESP-IDF crashes, produces incorrect output, or has incorrect behavior
title: ''
labels: ''
assignees: ''

---

----------------------------- Delete below -----------------------------

**Reminder: If your issue is a general question, starts similar to "How do I..", or is related to 3rd party development kits/libs, please discuss this on our community forum at https://esp32.com instead.**

INSTRUCTIONS
============

Before submitting a new issue, please follow the checklist and try to find the answer.

- [ ] I have read the documentation [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/) and the issue is not addressed there.
- [ ] I have updated my IDF branch (master or release) to the latest version and checked that the issue is present there.
- [ ] I have searched the issue tracker for a similar issue and not found a similar issue.

If the issue cannot be solved after the steps before, please follow these instructions so we can get the needed information to help you in a quick and effective fashion.

1. Fill in all the fields under **Environment** marked with [ ] by picking the correct option for you in each case and deleting the others.
2. Describe your problem.
3. Include [debug logs from the "monitor" tool](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html#automatically-decoding-addresses), or [coredumps](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/core_dump.html).
4. Providing as much information as possible under **Other items if possible** will help us locate and fix the problem.
5. Use [Markdown](https://guides.github.com/features/mastering-markdown/) (see formatting buttons above) and the Preview tab to check what the issue will look like.
6. Delete these instructions from the above to the below marker lines before submitting this issue.

**IMPORTANT: If you do not follow these instructions and provide the necessary details, your issue may not be resolved.**

----------------------------- Delete above -----------------------------

## Environment

- Development Kit:      [ESP32-Wrover-Kit|ESP32-DevKitC|ESP32-PICO-Kit|ESP32-LyraT|ESP32-LyraTD-MSC|none]
- Kit version (for WroverKit/PicoKit/DevKitC): [v1|v2|v3|v4]
- Module or chip used:  [ESP32-WROOM-32|ESP32-WROOM-32D|ESP32-WROOM-32U|ESP32-WROVER|ESP32-WROVER-I|ESP32-WROVER-B|ESP32-WROVER-IB|ESP32-SOLO-1|ESP32-PICO-D4|ESP32]
- IDF version (run ``git describe`` to find it):
    // v3.2-dev-1148-g96cd3b75c
- Build System:         [Make|CMake|idf.py]
- Compiler version (run ``xtensa-esp32-elf-gcc --version`` to find it):
    // 1.22.0-80-g6c4433a
- Operating System:     [Windows|Linux|macOS]
- (Windows only) environment type: [MSYS2 mingw32|ESP Command Prompt|Plain Command Prompt|PowerShell].
- Using an IDE?: [No|Yes (please give details)]
- Power Supply:         [USB|external 5V|external 3.3V|Battery]

## Problem Description

//Detailed problem description goes here.

### Expected Behavior

### Actual Behavior

### Steps to reproduce

1. step1
2. ...

// If possible, attach a picture of your setup/wiring here.


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
