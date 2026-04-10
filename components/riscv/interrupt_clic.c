/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "riscv/rv_utils.h"
#include "hal/interrupt_clic_ll.h"
#include "esp_private/interrupt_clic.h"


void intr_matrix_route(int intr_src, int intr_num)
{
    assert_valid_rv_int_num(intr_num);

    const int core_id = rv_utils_get_core_id();
    interrupt_clic_ll_route(core_id, intr_src, intr_num + RV_EXTERNAL_INT_OFFSET);
}


enum intr_type esprv_int_get_type(int rv_int_num)
{
    return interrupt_clic_ll_get_type(rv_int_num + RV_EXTERNAL_INT_OFFSET) ? INTR_TYPE_EDGE : INTR_TYPE_LEVEL;
}


int esprv_int_get_priority(int rv_int_num)
{
    return interrupt_clic_ll_get_priority(rv_int_num + RV_EXTERNAL_INT_OFFSET);
}


bool esprv_int_is_vectored(int rv_int_num)
{
    return interrupt_clic_ll_is_vectored(rv_int_num + RV_EXTERNAL_INT_OFFSET);
}


void esprv_int_set_vectored(int rv_int_num, bool vectored)
{
    interrupt_clic_ll_set_vectored(rv_int_num + RV_EXTERNAL_INT_OFFSET, vectored);
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
