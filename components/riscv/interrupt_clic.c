/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
