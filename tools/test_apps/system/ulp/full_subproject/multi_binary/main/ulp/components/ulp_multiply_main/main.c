/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

extern int ulp_shared_multiply_by_two(int value);

int result;

int main(void)
{
    result = ulp_shared_multiply_by_two(21);
    return 0;
}
