/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Define the configuration structure */
typedef struct {
    const char *dns_server;                     /*!< DNS server url or IP address */
    const char *dns_service_path;               /*!< Path to server DNS-over-HTTPS Service */
    const char *cert_pem;                       /*!< SSL server certification, PEM format as string, if the client requires to verify server */
    esp_err_t (*crt_bundle_attach)(void *conf); /*!< Function pointer to esp_crt_bundle_attach. Enables the use of certification
                                                bundle for server verification, must be enabled in menuconfig */
} dns_over_https_config_t;

/**
 * @brief Initializes the DNS over HTTPS (DoH) component with the given configuration.
 *
 * This function sets up the DNS over HTTPS component by configuring the DNS server,
 * service path, and root certificates required for secure communication. It validates
 * the provided configuration and ensures necessary parameters are set.
 *
 * @param[in] config Pointer to the configuration structure containing DNS server details,
 *                   service path, and root certificate options.
 *
 * @return ESP_OK on success, otherwise \see esp_err_t
 *
 * @warning This function must be called at least once before making any DNS-over-HTTPS requests
 */
esp_err_t dns_over_https_init(const dns_over_https_config_t *config);

#ifdef __cplusplus
}
#endif
