/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include "riscv/interrupt.h"
#include "esp_private/interrupt_plic.h"
#include "hal/interrupt_plic_ll.h"


void intr_matrix_route(int intr_src, int intr_num)
{
    assert_valid_rv_int_num(intr_num);
    interrupt_plic_ll_route(intr_src, intr_num);
}


uint32_t esprv_get_interrupt_unmask(void)
{
    return interrupt_plic_ll_get_unmask();
}


enum intr_type esprv_int_get_type(int rv_int_num)
{
    return interrupt_plic_ll_get_type(rv_int_num) ? INTR_TYPE_EDGE : INTR_TYPE_LEVEL;
}


int esprv_int_get_priority(int rv_int_num)
{
    return interrupt_plic_ll_get_priority(rv_int_num);
}


bool esprv_int_is_vectored(int rv_int_num)
{
    return true;
}
