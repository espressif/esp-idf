/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * These defines are parsed and imported as kconfig variables via the script
 * `tools/gen_soc_caps_kconfig/gen_soc_caps_kconfig.py`
 *
 * If this file is changed the script will automatically run the script
 * and generate the kconfig variables as part of the pre-commit hooks.
 *
 * It can also be run manually. For more information, see `${IDF_PATH}/tools/gen_soc_caps_kconfig/README.md`
 */

/*
 * NOTE: These definitions are only meant to allow host-based unit testing of some features using CMock.
 * They DO NOT imply any functionality on the host.
 */

#pragma once

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_EFUSE_SUPPORTED                 (1)
#define SOC_EFUSE_KEY_PURPOSE_FIELD         (1)

/*-------------------------- GPIO CAPS ---------------------------------------*/
#define SOC_GPIO_IN_RANGE_MAX           (65535)
#define SOC_GPIO_OUT_RANGE_MAX          (65535)

/*-------------------------- I2C CAPS ----------------------------------------*/
#define SOC_I2C_SUPPORT_SLAVE   (1)
#define SOC_I2C_SUPPORT_10BIT_ADDR   (1)

/*-------------------------- eFuse CAPS----------------------------*/
#define SOC_EFUSE_DIS_DOWNLOAD_ICACHE  (1)
#define SOC_EFUSE_DIS_DOWNLOAD_DCACHE  (1)
#define SOC_EFUSE_HARD_DIS_JTAG        (1)
#define SOC_EFUSE_DIS_USB_JTAG         (1)
#define SOC_EFUSE_SOFT_DIS_JTAG        (1)
#define SOC_EFUSE_DIS_DIRECT_BOOT      (1)
#define SOC_EFUSE_DIS_ICACHE           (1)

/*-------------------------- Secure Boot CAPS----------------------------*/
#define SOC_SECURE_BOOT_V2_RSA              (1)
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS   (3U)
#define SOC_EFUSE_REVOKE_BOOT_KEY_DIGESTS   (1)
#define SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY  (1)

/*-------------------------- Flash Encryption CAPS----------------------------*/
#define SOC_FLASH_ENCRYPTION_XTS_AES         (1)
#define SOC_FLASH_ENCRYPTION_XTS_AES_OPTIONS (1)
#define SOC_FLASH_ENCRYPTION_XTS_AES_128     (1)
#define SOC_FLASH_ENCRYPTION_XTS_AES_256     (1)
