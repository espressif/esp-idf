/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Compiling this file checks if SoC register access macros
 * can be used from C++ successfully. This check is necessary
 * because C++ 20 deprecates certain uses of 'volatile' qualified
 * variables.
 */
#include "soc/soc.h"
#include "soc/uart_reg.h"


/* non-static, to prevent the value being optimized out */
void sink(uint32_t arg)
{
}

void test_reg_macros(void)
{
    REG_WRITE(UART_DATE_REG(0), 1);
    sink(REG_READ(UART_DATE_REG(0)));
    sink(REG_GET_BIT(UART_DATE_REG(0), BIT(0)));
    REG_SET_BIT(UART_DATE_REG(0), BIT(0));
    REG_CLR_BIT(UART_DATE_REG(0), BIT(0));
    REG_SET_BITS(UART_DATE_REG(0), BIT(0), BIT(0) | BIT(1));
    sink(REG_GET_FIELD(UART_DATE_REG(0), UART_DATE));
    REG_SET_FIELD(UART_DATE_REG(0), UART_DATE, 42);

    sink(READ_PERI_REG(UART_DATE_REG(0)));
    WRITE_PERI_REG(UART_DATE_REG(0), 1);
    CLEAR_PERI_REG_MASK(UART_DATE_REG(0), BIT(0));
    SET_PERI_REG_MASK(UART_DATE_REG(0), BIT(0));
    sink(GET_PERI_REG_MASK(UART_DATE_REG(0), BIT(0)));
    sink(GET_PERI_REG_BITS(UART_DATE_REG(0), 2, 1));
    SET_PERI_REG_BITS(UART_DATE_REG(0), BIT(0) | BIT(1), 1, 2);
    sink(GET_PERI_REG_BITS2(UART_DATE_REG(0), BIT(0) | BIT(1), 1));
}
