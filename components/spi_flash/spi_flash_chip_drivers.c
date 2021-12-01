/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
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
#include "sdkconfig.h"

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
#else
//When the config option is enabled, user should provide this struct themselves.
extern const spi_flash_chip_t *default_registered_chips[];
#endif //!CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST

const spi_flash_chip_t **esp_flash_registered_chips = default_registered_chips;
