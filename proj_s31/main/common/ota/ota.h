#ifndef __OTA_H__
#define __OTA_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

typedef enum _E_OTA_TYPE
{
    OTA_TYPE_HTTPS = 0,
    OTA_TYPE_XMODEM,
}E_OTA_TYPE;

typedef struct _T_HTTPS_OTA_PARAM
{
    char *url;
    void *func_param;
}T_HTTPS_OTA_PARAM;

typedef esp_err_t (*ota_start_cb)(void *param);
typedef esp_err_t (*ota_write_cb)(void *param, const uint8_t *data, size_t size);
typedef esp_err_t (*ota_stop_cb)(void *param, bool success);

typedef struct _T_OTA_CTX
{
    void *ctx;                              // ota module private context
    // start the ota process with callback functions
    esp_err_t (*start)(void *ctx, ota_start_cb start_cb, ota_write_cb write_cb, ota_stop_cb stop_cb, void *param);
    // stop the ota process
    esp_err_t (*stop)(void *ctx);

    // deinitialize the ota module
    esp_err_t (*deinit)(void *ctx);
}T_OTA_MODE_CTX;                            // ota module context

esp_err_t ota_init(void);
esp_err_t ota_start(E_OTA_TYPE type);
esp_err_t ota_stop(void);
esp_err_t ota_mark_valid(void);
void      ota_print_info(void);

#ifdef __cplusplus
}
#endif

#endif // __OTA_H__
