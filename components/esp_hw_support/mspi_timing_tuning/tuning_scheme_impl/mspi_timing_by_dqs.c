/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 *
 * This file contains configuration APIs doing MSPI timing tuning by MSPI dqs
 * This file will only be built, when `SOC_MEMSPI_TIMING_TUNING_BY_DQS == 1`
 */

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_log.h"
#include "esp_private/mspi_timing_by_dqs.h"
#include "mspi_timing_tuning_configs.h"
#include "esp_private/mspi_timing_config.h"
#include "esp_private/rtc_clk.h"
#include "esp_private/periph_ctrl.h"
#include "hal/psram_ctrlr_ll.h"
#include "hal/mspi_ll.h"

#define AP_HEX_PSRAM_SYNC_READ             0x0000
#define AP_HEX_PSRAM_SYNC_WRITE            0x8080
#define AP_HEX_PSRAM_RD_CMD_BITLEN         16
#define AP_HEX_PSRAM_WR_CMD_BITLEN         16
#define AP_HEX_PSRAM_ADDR_BITLEN           32
#if CONFIG_SPIRAM_SPEED_250M
#define AP_HEX_PSRAM_RD_DUMMY_BITLEN       (2*(18-1))
#define AP_HEX_PSRAM_WR_DUMMY_BITLEN       (2*(9-1))
#elif CONFIG_SPIRAM_SPEED_200M
#define AP_HEX_PSRAM_RD_DUMMY_BITLEN       (2*(14-1))
#define AP_HEX_PSRAM_WR_DUMMY_BITLEN       (2*(7-1))
#else
#define AP_HEX_PSRAM_RD_DUMMY_BITLEN       (2*(10-1))
#define AP_HEX_PSRAM_WR_DUMMY_BITLEN       (2*(5-1))
#endif

#define WRONG_DELAYLINE 16

const static char *TAG = "MSPI DQS";
const static uint32_t s_test_data[MSPI_TIMING_TEST_DATA_LEN] = {0x7f786655, 0xa5ff005a, 0x3f3c33aa, 0xa5ff5a00, 0x1f1e9955, 0xa5005aff, 0x0f0fccaa, 0xa55a00ff,
                                                                0x07876655, 0xffa55a00, 0x03c333aa, 0xff00a55a, 0x01e19955, 0xff005aa5, 0x00f0ccaa, 0xff5a00a5,
                                                                0x80786655, 0x00a5ff5a, 0xc03c33aa, 0x00a55aff, 0xe01e9355, 0x00ff5aa5, 0xf00fccaa, 0x005affa5,
                                                                0xf8876655, 0x5aa5ff00, 0xfcc333aa, 0x5affa500, 0xfee19955, 0x5a00a5ff, 0x11f0ccaa, 0x5a00ffa5};
const static mspi_timing_config_t s_test_delayline_config = {
        .delayline_table = {{0, 15}, {0, 14}, {0, 13}, {0, 12}, {0, 11}, {0, 10}, {0, 9}, {0, 8}, {0, 7}, {0, 6}, {0, 5}, {0, 4}, {0, 3}, {0, 2}, {0, 1},
                            {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0}},
        .available_config_num = 31,
    };
static mspi_ll_dqs_phase_t s_psram_best_phase = MSPI_LL_DQS_PHASE_MAX;
static delayline_config_t s_psram_best_delayline = {WRONG_DELAYLINE, WRONG_DELAYLINE};

void mspi_timing_psram_init(uint32_t psram_freq_mhz)
{
    psram_ctrlr_ll_enable_variable_dummy(PSRAM_CTRLR_LL_MSPI_ID_3, false);
    mspi_timing_config_set_psram_clock(psram_freq_mhz, MSPI_TIMING_SPEED_MODE_NORMAL_PERF, true);
}

void mspi_timing_config_psram_prepare_reference_data(uint8_t *buf, uint32_t len)
{
    assert(len == MSPI_TIMING_TEST_DATA_LEN);

    memcpy(buf, &s_test_data, len);
}

void mspi_timing_config_psram_write_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    uint8_t *w_ptr = buf;

    while (len) {
        int len_to_send = MIN(len, PSRAM_CTRLR_LL_FIFO_MAX_BYTES);
        psram_ctrlr_ll_common_transaction(PSRAM_CTRLR_LL_MSPI_ID_3,
                                          AP_HEX_PSRAM_SYNC_WRITE, AP_HEX_PSRAM_WR_CMD_BITLEN,
                                          addr, AP_HEX_PSRAM_ADDR_BITLEN,
                                          AP_HEX_PSRAM_WR_DUMMY_BITLEN,
                                          w_ptr, len_to_send * 8,
                                          NULL, 0,
                                          false);
        w_ptr += len_to_send;
        addr += len_to_send;
        len -= len_to_send;
    }
}

void mspi_timing_config_psram_read_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    uint8_t *r_ptr = buf;

    while (len) {
        int len_to_recv = MIN(len, PSRAM_CTRLR_LL_FIFO_MAX_BYTES);
        psram_ctrlr_ll_common_transaction(PSRAM_CTRLR_LL_MSPI_ID_3,
                                          AP_HEX_PSRAM_SYNC_READ, AP_HEX_PSRAM_RD_CMD_BITLEN,
                                          addr, AP_HEX_PSRAM_ADDR_BITLEN,
                                          AP_HEX_PSRAM_RD_DUMMY_BITLEN,
                                          NULL, 0,
                                          r_ptr, len_to_recv * 8,
                                          false);
        r_ptr += len_to_recv;
        addr += len_to_recv;
        len -= len_to_recv;
    }
}

void mspi_timing_get_psram_tuning_phases(mspi_timing_config_t *configs)
{
    *configs = (mspi_timing_config_t) {
        .phase = {MSPI_LL_DQS_PHASE_67_5, MSPI_LL_DQS_PHASE_78_75, MSPI_LL_DQS_PHASE_90, MSPI_LL_DQS_PHASE_101_25},
        .available_phase_num = 4,
    };
}

void mspi_timing_config_psram_set_tuning_phase(const void *configs, uint8_t id)
{
    mspi_ll_dqs_phase_t phase = ((mspi_timing_config_t *)configs)->phase[id];
    mspi_timing_ll_set_dqs_phase(MSPI_LL_DQS_ID_0, phase);
    mspi_timing_ll_set_dqs_phase(MSPI_LL_DQS_ID_1, phase);
    ESP_EARLY_LOGD(TAG, "set to phase: %d", phase);
}

uint32_t mspi_timing_psram_select_best_tuning_phase(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr)
{
    assert(consecutive_length < 5);
    uint32_t best_phase_id = 0;

    bool success = true;
    if (consecutive_length == 0) {
        best_phase_id = 0;
        success = false;
    } else {
        best_phase_id = (end - consecutive_length + 1);
    }

    if (success) {
        ESP_EARLY_LOGI(TAG, "tuning success, best phase id is %"PRIu32, best_phase_id);
    } else {
        ESP_EARLY_LOGW(TAG, "tuning fail, best phase id is fallen back to index %"PRIu32"", best_phase_id);
    }

    return best_phase_id;
}

void mspi_timing_psram_set_best_tuning_phase(const void *configs, uint8_t best_id)
{
    s_psram_best_phase = ((const mspi_timing_config_t *)configs)->phase[best_id];
}

void mspi_timing_get_psram_tuning_delaylines(mspi_timing_config_t *configs)
{
    ESP_EARLY_LOGD(TAG, "sizeof(delayline_config_t): %d, sizeof(test_config): %d", sizeof(delayline_config_t), sizeof(s_test_delayline_config));
    memcpy(configs, &s_test_delayline_config, sizeof(s_test_delayline_config));
}

void mspi_timing_config_psram_set_tuning_delayline(const void *configs, uint8_t id)
{
    assert(s_psram_best_phase != MSPI_LL_DQS_PHASE_MAX);
    mspi_timing_ll_set_dqs_phase(MSPI_LL_DQS_ID_0, s_psram_best_phase);
    mspi_timing_ll_set_dqs_phase(MSPI_LL_DQS_ID_1, s_psram_best_phase);
    ESP_EARLY_LOGD(TAG, "set to best phase: %d", s_psram_best_phase);

    const delayline_config_t *delayline_config = &((mspi_timing_config_t *)configs)->delayline_table[id];
    for (int i = 0; i < MSPI_LL_PIN_MAX; i++) {
        if (i == MSPI_LL_PIN_DQS0 || i == MSPI_LL_PIN_DQS1) {
            mspi_timing_ll_set_delayline(i, delayline_config->dqs_delayline);
        } else {
            mspi_timing_ll_set_delayline(i, delayline_config->data_delayline);
        }
    }
}

uint32_t mspi_timing_psram_select_best_tuning_delayline(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr)
{
    assert(consecutive_length <= 32);
    uint32_t bset_delayline_id = 0;

    if (consecutive_length <= 1) {
        bset_delayline_id = 0;
        ESP_EARLY_LOGW(TAG, "tuning fail, best delayline id is fallen back to index %"PRIu32"", bset_delayline_id);
    } else {
        bset_delayline_id = end - consecutive_length / 2;
        ESP_EARLY_LOGI(TAG, "tuning success, best delayline id is %"PRIu32, bset_delayline_id);
    }

    return bset_delayline_id;
}

void mspi_timing_psram_set_best_tuning_delayline(const void *configs, uint8_t best_id)
{
    s_psram_best_delayline = ((mspi_timing_config_t *)configs)->delayline_table[best_id];
}

void mspi_timing_psram_config_clear_tuning_regs(bool control_both_mspi)
{
    mspi_timing_ll_set_dqs_phase(MSPI_LL_DQS_ID_0, 0);
    mspi_timing_ll_set_dqs_phase(MSPI_LL_DQS_ID_1, 0);

    for (int i = 0; i < MSPI_LL_PIN_MAX; i++) {
        mspi_timing_ll_set_delayline(i, 0);
    }
}

void mspi_timing_psram_config_set_tuning_regs(bool control_both_mspi)
{
    mspi_timing_ll_set_dqs_phase(MSPI_LL_DQS_ID_0, s_psram_best_phase);
    mspi_timing_ll_set_dqs_phase(MSPI_LL_DQS_ID_1, s_psram_best_phase);

    for (int i = 0; i < MSPI_LL_PIN_MAX; i++) {
        if (i == MSPI_LL_PIN_DQS0 || i == MSPI_LL_PIN_DQS1) {
            mspi_timing_ll_set_delayline(i, s_psram_best_delayline.dqs_delayline);
        } else {
            mspi_timing_ll_set_delayline(i, s_psram_best_delayline.data_delayline);
        }
    }
}
