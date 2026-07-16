/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

int ulp_shared_add_bias(int value)
{
    return value + 10;
}

int ulp_shared_multiply_by_two(int value)
{
    return value * 2;
}
