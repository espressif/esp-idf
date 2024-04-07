/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_rom_caps.h"
#include "soc/clic_reg.h"
#include "riscv/interrupt.h"

#if ESP_ROM_CLIC_INT_TYPE_PATCH

/* Rom api esprv_intc_int_set_type, if the configured interrupt type is INTR_TYPE_EDGE,
 * the actual configured type is still INTR_TYPE_LEVEL. So the patch is to solve this issue.
 * Since esprv_intc_int_set_type has an alias defined as esprv_int_set_type in riscv/ld/rom.api.ld,
 * therefore, use esprv_int_set_type to override the rom function.
 */
void esprv_int_set_type(int rv_int_num, enum intr_type type)
{
    REG_SET_FIELD(CLIC_INT_CTRL_REG(rv_int_num + CLIC_EXT_INTR_NUM_OFFSET), CLIC_INT_ATTR_TRIG, type);
}
#endif
