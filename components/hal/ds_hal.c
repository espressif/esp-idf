// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "hal/systimer_hal.h"
#include "hal/ds_hal.h"
#include "hal/ds_ll.h"

void ds_hal_start(void)
{
    ds_ll_start();
}

void ds_hal_finish(void)
{
    ds_ll_finish();
}

void ds_hal_configure_iv(const uint32_t *iv)
{
    ds_ll_configure_iv(iv);
}

void ds_hal_write_message(const uint8_t *msg, size_t size)
{
    ds_ll_write_message(msg, size);
}

void ds_hal_write_private_key_params(const uint8_t *key_params)
{
    ds_ll_write_private_key_params(key_params);
}

void ds_hal_start_sign(void)
{
    ds_ll_start_sign();
}

bool ds_hal_busy(void)
{
    return ds_ll_busy();
}

ds_signature_check_t ds_hal_read_result(uint8_t *result, size_t size)
{
    ds_signature_check_t check_result = ds_ll_check_signature();

    if (check_result == DS_SIGNATURE_OK || check_result == DS_SIGNATURE_PADDING_FAIL) {
        ds_ll_read_result(result, size);
    }

    return check_result;
}
