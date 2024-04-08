| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

This project is for testing if the application can be built with a particular sdkconfig setting.

To add new configuration, create one more sdkconfig.ci.NAME file in this directory.
To make the configuration target-specific, add a CONFIG_IDF_TARGET="name" line.

If you need to test for anything other than building, create another test project.
