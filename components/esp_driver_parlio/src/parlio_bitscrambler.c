/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "driver/parlio_tx.h"
#include "driver/bitscrambler.h"
#include "parlio_priv.h"

static esp_err_t parlio_tx_unit_bs_enable(parlio_tx_unit_handle_t tx_unit, parlio_tx_trans_desc_t *t)
{
    if (t->bitscrambler_program) {
        ESP_RETURN_ON_ERROR(bitscrambler_enable(tx_unit->bs_handle), TAG, "enable bitscrambler failed");
        ESP_RETURN_ON_ERROR(bitscrambler_load_program(tx_unit->bs_handle, t->bitscrambler_program), TAG, "load bitscrambler program failed");
        ESP_RETURN_ON_ERROR(bitscrambler_reset(tx_unit->bs_handle), TAG, "reset bitscrambler failed");
        ESP_RETURN_ON_ERROR(bitscrambler_start(tx_unit->bs_handle), TAG, "start bitscrambler failed");
    }
    return ESP_OK;
}

static esp_err_t parlio_tx_unit_bs_disable(parlio_tx_unit_handle_t tx_unit)
{
    ESP_RETURN_ON_ERROR(bitscrambler_disable(tx_unit->bs_handle), TAG, "disable bitscrambler failed");
    return ESP_OK;
}

esp_err_t parlio_tx_unit_decorate_bitscrambler(parlio_tx_unit_handle_t tx_unit)
{
    ESP_RETURN_ON_FALSE(tx_unit, ESP_ERR_INVALID_ARG, TAG, "invalid TX unit");

    // bitscrambler function is used under Parlio TX ISR context
    // if the cache is disabled, all functions called by ISR must be in IRAM
#if CONFIG_PARLIO_TX_ISR_CACHE_SAFE && !CONFIG_BITSCRAMBLER_CTRL_FUNC_IN_IRAM
    ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "CONFIG_BITSCRAMBLER_CTRL_FUNC_IN_IRAM must be enabled");
#endif

    // check if already decorated
    if (tx_unit->bs_handle) {
        ESP_RETURN_ON_ERROR(ESP_ERR_INVALID_STATE, TAG, "TX unit already decorated with BitScrambler");
    }

    // create BitScrambler instance
    bitscrambler_config_t bs_config = {
        .dir = BITSCRAMBLER_DIR_TX,
        .attach_to = SOC_BITSCRAMBLER_ATTACH_PARL_IO,
    };

    parlio_tx_fsm_t expected_fsm = PARLIO_TX_FSM_INIT;
    if (atomic_compare_exchange_strong(&tx_unit->fsm, &expected_fsm, PARLIO_TX_FSM_WAIT)) {
        ESP_RETURN_ON_ERROR(bitscrambler_new(&bs_config, &tx_unit->bs_handle), TAG, "create bitscrambler failed");
        tx_unit->bs_enable_fn = parlio_tx_unit_bs_enable;
        tx_unit->bs_disable_fn = parlio_tx_unit_bs_disable;
        atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_INIT);
    } else {
        ESP_RETURN_ON_ERROR(ESP_ERR_INVALID_STATE, TAG, "TX unit is not in init state");
    }
    return ESP_OK;
}

esp_err_t parlio_tx_unit_undecorate_bitscrambler(parlio_tx_unit_handle_t tx_unit)
{
    ESP_RETURN_ON_FALSE(tx_unit != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid TX unit");

    // check if already decorated
    if (!tx_unit->bs_handle) {
        ESP_RETURN_ON_ERROR(ESP_ERR_INVALID_STATE, TAG, "TX unit not decorated with BitScrambler yet");
    }
    parlio_tx_fsm_t expected_fsm = PARLIO_TX_FSM_INIT;
    if (atomic_compare_exchange_strong(&tx_unit->fsm, &expected_fsm, PARLIO_TX_FSM_WAIT)) {
        bitscrambler_free(tx_unit->bs_handle);
        tx_unit->bs_handle = NULL;
        tx_unit->bs_enable_fn = NULL;
        tx_unit->bs_disable_fn = NULL;
        atomic_store(&tx_unit->fsm, PARLIO_TX_FSM_INIT);
    } else {
        ESP_RETURN_ON_ERROR(ESP_ERR_INVALID_STATE, TAG, "TX unit is not in init state");
    }
    return ESP_OK;
}
