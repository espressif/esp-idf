/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/i2c_hal.h"

//////////////////////////////////////////Deprecated Functions//////////////////////////////////////////////////////////
/////////////////////////////The following functions are only used by the legacy driver/////////////////////////////////
/////////////////////////////They might be removed in the next major release (ESP-IDF 6.0)//////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void i2c_hal_master_handle_tx_event(i2c_hal_context_t *hal, i2c_intr_event_t *event)
{
    uint32_t intr_status = 0;
    i2c_ll_get_intr_mask(hal->dev, &intr_status);
    if (intr_status != 0) {
        // If intr status is 0, no need to handle it.
        i2c_ll_master_get_event(hal->dev, event);
        if ((*event < I2C_INTR_EVENT_END_DET) ||
            (*event == I2C_INTR_EVENT_TRANS_DONE)) {
            i2c_ll_master_disable_tx_it(hal->dev);
            i2c_ll_clear_intr_mask(hal->dev, intr_status);
        } else if (*event == I2C_INTR_EVENT_END_DET) {
            i2c_ll_clear_intr_mask(hal->dev, intr_status);
        }
    }
}

void i2c_hal_master_handle_rx_event(i2c_hal_context_t *hal, i2c_intr_event_t *event)
{
    uint32_t intr_status = 0;
    i2c_ll_get_intr_mask(hal->dev, &intr_status);
    if (intr_status != 0) {
        i2c_ll_master_get_event(hal->dev, event);
        if ((*event < I2C_INTR_EVENT_END_DET) ||
            (*event == I2C_INTR_EVENT_TRANS_DONE)) {
            i2c_ll_master_disable_rx_it(hal->dev);
            i2c_ll_clear_intr_mask(hal->dev, intr_status);
        } else if (*event == I2C_INTR_EVENT_END_DET) {
            i2c_ll_clear_intr_mask(hal->dev, intr_status);
        }
    }
}
