/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "soc/soc_caps.h"
#include "hal/efuse_ll.h"
#include "hal/assert.h"
#include "hal/efuse_hal.h"
#include "esp_attr.h"

#define FLASH_SECTOR_SIZE (4096)


void efuse_hal_get_mac(uint8_t *mac)
{
    *((uint32_t*)&mac[0]) = efuse_ll_get_mac0();
    *((uint16_t*)&mac[4]) = (uint16_t) efuse_ll_get_mac1();
}

IRAM_ATTR uint32_t efuse_hal_chip_revision(void)
{
    return efuse_hal_get_major_chip_version() * 100 + efuse_hal_get_minor_chip_version();
}

IRAM_ATTR uint32_t efuse_hal_blk_version(void)
{
    return efuse_ll_get_blk_version_major() * 100 + efuse_ll_get_blk_version_minor();
}

IRAM_ATTR bool efuse_hal_get_disable_wafer_version_major(void)
{
    return efuse_ll_get_disable_wafer_version_major();
}

IRAM_ATTR uint32_t efuse_hal_get_chip_ver_pkg(void)
{
    return efuse_ll_get_chip_ver_pkg();
}

IRAM_ATTR bool efuse_hal_get_disable_blk_version_major(void)
{
    return efuse_ll_get_disable_blk_version_major();
}

IRAM_ATTR bool efuse_hal_flash_encryption_enabled(void)
{
    uint32_t flash_crypt_cnt = efuse_ll_get_flash_crypt_cnt();
    bool enabled = false;
    while (flash_crypt_cnt) {
        if (flash_crypt_cnt & 1) {
            enabled = !enabled;
        }
        flash_crypt_cnt >>= 1;
    }
    return enabled;
}

#if SOC_EFUSE_ECDSA_KEY
void efuse_hal_set_ecdsa_key(ecdsa_curve_t curve, int efuse_blk)
{
    efuse_ll_set_ecdsa_key_blk(curve, efuse_blk);

    efuse_ll_rs_bypass_update();

    efuse_hal_read();
}
#endif

#if SOC_RECOVERY_BOOTLOADER_SUPPORTED
uint32_t efuse_hal_get_recovery_bootloader_address(void)
{
    return efuse_ll_get_recovery_bootloader_sector() * FLASH_SECTOR_SIZE;
}

uint32_t efuse_hal_convert_recovery_bootloader_address_to_flash_sectors(uint32_t address)
{
    return address / FLASH_SECTOR_SIZE;
}

bool efuse_hal_recovery_bootloader_enabled(void)
{
    return EFUSE_RECOVERY_BOOTLOADER_ENABLED(efuse_ll_get_recovery_bootloader_sector());
}
#endif // SOC_RECOVERY_BOOTLOADER_SUPPORTED
