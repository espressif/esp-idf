/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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

/*-------------------------- GPIO CAPS ---------------------------------------*/
#define SOC_GPIO_IN_RANGE_MAX           (65535)
#define SOC_GPIO_OUT_RANGE_MAX          (65535)

/*-------------------------- I2C CAPS ----------------------------------------*/
#define SOC_I2C_SUPPORT_SLAVE   (1)
#define SOC_I2C_SUPPORT_10BIT_ADDR   (1)
