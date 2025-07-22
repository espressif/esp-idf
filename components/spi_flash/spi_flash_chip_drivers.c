/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "spi_flash_chip_driver.h"
#include "spi_flash_chip_generic.h"
#include "spi_flash_chip_issi.h"
#include "spi_flash_chip_mxic.h"
#include "spi_flash_chip_gd.h"
#include "spi_flash_chip_winbond.h"
#include "spi_flash_chip_boya.h"
#include "spi_flash_chip_th.h"
#include "spi_flash_defs.h"
#include "sdkconfig.h"
#include "esp_log.h"

__attribute__((unused)) static const char *TAG = "spi_flash";

#define FORMAT_STR "Detected %s flash chip but using generic driver. For optimal functionality, enable `SPI_FLASH_SUPPORT_%s_CHIP` in menuconfig"

#if !CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST
/*
 * Default registered chip drivers. Note these are tested in order and first
 * match is taken, so generic/catchall entries should go last. Note that the
 * esp_flash_registered_flash_ops pointer can be changed to point to a different
 * array of registered ops, if desired.
 *
 * It can be configured to support only available chips in the sdkconfig, to
 * avoid possible issues, and speed up the auto-detecting.
 */
static const spi_flash_chip_t *default_registered_chips[] = {
#ifdef CONFIG_SPI_FLASH_SUPPORT_ISSI_CHIP
    &esp_flash_chip_issi,
#endif
#ifdef CONFIG_SPI_FLASH_SUPPORT_GD_CHIP
    &esp_flash_chip_gd,
#endif
#ifdef CONFIG_SPI_FLASH_SUPPORT_MXIC_CHIP
    &esp_flash_chip_mxic,
#endif
#ifdef CONFIG_SPI_FLASH_SUPPORT_WINBOND_CHIP
    &esp_flash_chip_winbond,
#endif
#ifdef CONFIG_SPI_FLASH_SUPPORT_BOYA_CHIP
    &esp_flash_chip_boya,
#endif
#ifdef CONFIG_SPI_FLASH_SUPPORT_TH_CHIP
    &esp_flash_chip_th,
#endif
#ifdef CONFIG_SPI_FLASH_SUPPORT_MXIC_OPI_CHIP
    &esp_flash_chip_mxic_opi,
#endif
    // Default chip drivers that will accept all chip ID.
    // FM, Winbond and XMC chips are supposed to be supported by this chip driver.
    &esp_flash_chip_generic,
    NULL,
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress"
void spi_flash_chip_list_check(esp_flash_t *chip, uint32_t device_id) {
    uint8_t vendor_id = device_id >> 16;
    switch (vendor_id)
    {
    case SPI_FLASH_GD:
        if (&esp_flash_chip_gd == NULL) {
            ESP_EARLY_LOGW(TAG, FORMAT_STR, "GigaDevice", "GD");
        }
        break;
    case SPI_FLASH_ISSI:
        if (&esp_flash_chip_issi == NULL) {
            ESP_EARLY_LOGW(TAG, FORMAT_STR, "ISSI", "ISSI");
        }
        break;
    case SPI_FLASH_TH:
        if (&esp_flash_chip_th == NULL) {
            ESP_EARLY_LOGW(TAG, FORMAT_STR, "TH", "TH");
        }
        break;
    case SPI_FLASH_WINBOND:
        if (&esp_flash_chip_winbond == NULL) {
            ESP_EARLY_LOGW(TAG, FORMAT_STR, "winbond", "WINBOND");
        }
        break;
    case SPI_FLASH_MXIC:
        // Need to tell the difference between octal and quad flash.
        if (chip->read_mode < SPI_FLASH_OPI_FLAG) {
            if (&esp_flash_chip_mxic == NULL) {
                ESP_EARLY_LOGW(TAG, FORMAT_STR, "MXIC", "MXIC");
            }
        } else {
            if (&esp_flash_chip_mxic_opi == NULL) {
                ESP_EARLY_LOGW(TAG, FORMAT_STR, "MXIC", "MXIC_OPI");
            }
        }
        break;
    case SPI_FLASH_BY:
        if (&esp_flash_chip_boya == NULL) {
            ESP_EARLY_LOGW(TAG, FORMAT_STR, "boya", "BOYA");
        }
        break;
    default:
        break;
    }
}
#pragma GCC diagnostic pop

#else
//When the config option is enabled, user should provide this struct themselves.
extern const spi_flash_chip_t *default_registered_chips[];
#endif //!CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST

const spi_flash_chip_t **esp_flash_registered_chips = default_registered_chips;
