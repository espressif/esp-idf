| Supported Targets | ESP32-C3 |
| ----------------- | -------- |

Unit tests for `esp_https_ota`. No case connects to a server, so they need no network. They run with `CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP` disabled (`sdkconfig.ci.defaults`) and enabled (`sdkconfig.ci.allow_http`).
