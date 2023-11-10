/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The long term plan is to have a single soc_caps.h for each peripheral.
// During the refactoring and multichip support development process, we
// seperate these information into periph_caps.h for each peripheral and
// include them here.

/*
 * These defines are parsed and imported as kconfig variables via the script
 * `tools/gen_soc_caps_kconfig/gen_soc_caps_kconfig.py`
 *
 * If this file is changed the script will automatically run the script
 * and generate the kconfig variables as part of the pre-commit hooks.
 *
 * It can also be ran manually with `./tools/gen_soc_caps_kconfig/gen_soc_caps_kconfig.py 'components/soc/esp32c3/include/soc/'`
 *
 * For more information see `tools/gen_soc_caps_kconfig/README.md`
 *
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
