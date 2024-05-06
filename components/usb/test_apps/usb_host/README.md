| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# USB: Host test application

There are two sets of tests in this application:
1. Low-speed: Expects low-speed USB mouse with interrupt endpoint to be connected
2. Full-speed: Expects full-speed USB flash disk with 2 bulk endpoints to be connected

For running these tests locally, you will have to update device definitions (VID, PID, ...) in [mock_msc.h](../common/mock_msc.h) and [mock_hid.h](../common/mock_hid.h).

