/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "riscv/csr.h"
#include "riscv/interrupt.h"
#include "riscv/rv_utils.h"
#include "esp_private/interrupt_plic.h"
#include "hal/interrupt_plic_ll.h"


void intr_matrix_route(int intr_src, int intr_num)
{
    assert_valid_rv_int_num(intr_num);
    interrupt_plic_ll_route(intr_src, intr_num);
}


uint32_t esprv_get_interrupt_unmask(void)
{
    if (IS_PRV_M_MODE()) {
        return REG_READ(PLIC_MXINT_ENABLE_REG);
    } else {
        return REG_READ(PLIC_UXINT_ENABLE_REG);
    }
}


enum intr_type esprv_int_get_type(int rv_int_num)
{
    uint32_t intr_type_reg;

    if (IS_PRV_M_MODE()) {
        intr_type_reg = REG_READ(PLIC_MXINT_TYPE_REG);
    } else {
        intr_type_reg = REG_READ(PLIC_UXINT_TYPE_REG);
    }

    return (intr_type_reg & (1 << rv_int_num)) ? INTR_TYPE_EDGE : INTR_TYPE_LEVEL;
}


int esprv_int_get_priority(int rv_int_num)
{
    if (IS_PRV_M_MODE()) {
        return REG_READ(PLIC_MXINT0_PRI_REG + (rv_int_num) * 4);
    } else {
        return REG_READ(PLIC_UXINT0_PRI_REG + (rv_int_num) * 4);
    }
}


bool esprv_int_is_vectored(int rv_int_num)
{
    return true;
}


#if CONFIG_SECURE_ENABLE_TEE && !NON_OS_BUILD
DRAM_ATTR esprv_int_mgmt_t esp_tee_intr_sec_srv_cb = NULL;

void esprv_int_setup_mgmt_cb(void *fptr)
{
    esp_tee_intr_sec_srv_cb = (esprv_int_mgmt_t)fptr;
}


/* NOTE: Overriding ROM-based interrupt configuration symbols */
void esprv_int_enable(uint32_t unmask)
{
    rv_utils_intr_enable(unmask);
}


void esprv_int_disable(uint32_t mask)
{
    rv_utils_intr_disable(mask);
}


void esprv_int_set_type(int intr_num, enum intr_type type)
{
    rv_utils_intr_set_type(intr_num, type);
}


void esprv_int_set_priority(int rv_int_num, int priority)
{
    rv_utils_intr_set_priority(rv_int_num, priority);
}


void esprv_int_set_threshold(int priority_threshold)
{
    rv_utils_intr_set_threshold(priority_threshold);
}
#endif
