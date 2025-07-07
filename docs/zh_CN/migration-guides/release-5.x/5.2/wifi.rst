Wi-Fi
=====

:link_to_translation:`en:[English]`


Wi-Fi 企业级安全
------------------------

在 `esp_wpa2.h` 中定义的 API 已弃用，请使用来自 `esp_eap_client.h` 的新 API。

Wi-Fi 断开连接原因代码
------------------------

对于事件 WIFI_EVENT_STA_DISCONNECTED，原有的断开原因代码 WIFI_REASON_NO_AP_FOUND 已细分为以下几种情况：

- REASON_NO_AP_FOUND（原始原因码，仍在某些场景中使用）
- REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD（在 RSSI 门限范围内未找到 AP）
- REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD（在认证模式门限范围内未找到 AP）
- REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY（未找到具有兼容安全性的 AP）


Wi-Fi 多天线
------------------------

Wi-Fi 多天线相关的 API 将要被弃用，请使用来自 `esp_phy.h` 的新 API。
