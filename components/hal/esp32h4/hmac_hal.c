/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdio.h"
#include "hal/hmac_hal.h"
#include "hal/hmac_ll.h"

void hmac_hal_start(void)
{
    hmac_ll_wait_idle();
    hmac_ll_start();
}

uint32_t hmac_hal_configure(hmac_hal_output_t config, uint32_t key_id)
{
    hmac_ll_wait_idle();
    hmac_ll_config_output(config);
    hmac_ll_config_hw_key_id(key_id);
    hmac_ll_config_finish();
    hmac_ll_wait_idle();

    uint32_t conf_error = hmac_ll_config_error();
    if (conf_error) {
        hmac_ll_calc_finish();
        return 1;
    } else if (config != HMAC_OUTPUT_USER) {
        // In "downstream" mode, this will be the last hmac operation. Make sure HMAC is ready for
        // the other peripheral.
        hmac_ll_wait_idle();
    }

    return 0;
}

void hmac_hal_write_one_block_512(const void *block)
{
    hmac_ll_wait_idle();
    hmac_ll_write_block_512(block);
    hmac_ll_wait_idle();
    hmac_ll_msg_one_block();
}

void hmac_hal_write_block_512(const void *block)
{
    hmac_ll_wait_idle();
    hmac_ll_write_block_512(block);
}

void hmac_hal_next_block_padding(void)
{
    hmac_ll_wait_idle();
    hmac_ll_msg_padding();
}

void hmac_hal_next_block_normal(void)
{
    hmac_ll_wait_idle();
    hmac_ll_msg_continue();
}

void hmac_hal_read_result_256(void *result)
{
    hmac_ll_wait_idle();
    hmac_ll_read_result_256(result);
    hmac_ll_calc_finish();
}

void hmac_hal_clean(void)
{
    hmac_ll_wait_idle();
    hmac_ll_clean();
}
