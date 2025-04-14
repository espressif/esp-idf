#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"

/* WiFi設定 */
#define WIFI_MAXIMUM_RETRY  5

/* WiFi接続ステータス */
typedef enum {
    WIFI_STATUS_DISCONNECTED,
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_FAILED
} wifi_connection_status_t;

/* 関数プロトタイプ */
esp_err_t wifi_init_sta(void);
wifi_connection_status_t wifi_get_status(void);

#endif /* WIFI_MANAGER_H */