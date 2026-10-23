/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdint.h>
#include "hal/efuse_hal.h"
#include "hal/efuse_ll.h"
#include "soc/regi2c_lp_bias.h"
#include "esp_private/regi2c_ctrl.h"

static void set_ocode_by_efuse(int ocode_scheme_ver)
{
    assert(ocode_scheme_ver == 1);
    unsigned int ocode = efuse_ll_get_ocode();

    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_EXT_CODE, ocode);
    REGI2C_WRITE_MASK(I2C_ULP, I2C_ULP_IR_FORCE_CODE, 1);
}

void esp_ocode_calib_init(void)
{
    uint32_t blk_version = efuse_hal_blk_version();
    // major * 100 + minor. Scheme 1 is only valid before the next major version.
    if (blk_version >= 1 && blk_version < 100) {
        set_ocode_by_efuse(1);
    }
}
