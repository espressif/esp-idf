// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_compiler.h"
#include "driver/rmt.h"
#include "musical_buzzer.h"

static const char *TAG = "buzzer_rmt";

#define BUZZER_CHECK(a, msg, tag, ret, ...)                                       \
    do {                                                                          \
        if (unlikely(!(a))) {                                                     \
            ESP_LOGE(TAG, "%s(%d): " msg, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret_code = ret;                                                       \
            goto tag;                                                             \
        }                                                                         \
    } while (0)

typedef struct {
    musical_buzzer_t parent;
    rmt_channel_t channel;
    uint32_t counter_clk_hz;
    const musical_buzzer_notation_t *notation;
    uint32_t notation_length;
    uint32_t next_notation_index;
} rmt_buzzer_t;

static IRAM_ATTR rmt_item32_t update_notation_freq_duration(rmt_buzzer_t *rmt_buzzer)
{
    rmt_item32_t notation_code = {.level0 = 1, .duration0 = 1, .level1 = 0, .duration1 = 1};
    const musical_buzzer_notation_t *notation = &rmt_buzzer->notation[rmt_buzzer->next_notation_index];

    // convert frequency to RMT item format
    notation_code.duration0 = rmt_buzzer->counter_clk_hz / notation->note_freq_hz / 2;
    notation_code.duration1 = notation_code.duration0;
    // convert duration to RMT loop count
    rmt_set_tx_loop_count(rmt_buzzer->channel, notation->note_duration_ms * notation->note_freq_hz / 1000);

    rmt_buzzer->next_notation_index++;
    return notation_code;
}

static esp_err_t buzzer_play(musical_buzzer_t *buzzer, const musical_buzzer_notation_t *notation, uint32_t notation_length)
{
    esp_err_t ret_code = ESP_OK;
    rmt_buzzer_t *rmt_buzzer = __containerof(buzzer, rmt_buzzer_t, parent);

    BUZZER_CHECK(notation, "notation can't be null", err, ESP_ERR_INVALID_ARG);

    // update notation with the new one
    rmt_buzzer->notation = notation;
    rmt_buzzer->next_notation_index = 0;
    rmt_buzzer->notation_length = notation_length;

    rmt_item32_t notation_code = update_notation_freq_duration(rmt_buzzer);
    // start TX
    rmt_write_items(rmt_buzzer->channel, &notation_code, 1, false);
err:
    return ret_code;
}

static esp_err_t buzzer_stop(musical_buzzer_t *buzzer)
{
    rmt_buzzer_t *rmt_buzzer = __containerof(buzzer, rmt_buzzer_t, parent);
    rmt_tx_stop(rmt_buzzer->channel);
    return ESP_OK;
}

static esp_err_t buzzer_del(musical_buzzer_t *buzzer)
{
    rmt_buzzer_t *rmt_buzzer = __containerof(buzzer, rmt_buzzer_t, parent);
    free(rmt_buzzer);
    return ESP_OK;
}

static void rmt_tx_loop_end(rmt_channel_t channel, void *args)
{
    rmt_buzzer_t *rmt_buzzer = (rmt_buzzer_t *)args;

    // stop it firstly, RMT TX engine won't stop automatically in loop mode
    rmt_tx_stop(rmt_buzzer->channel);

    // update rmt loop freq and duration if the notation doesn't reach the end
    if (rmt_buzzer->next_notation_index < rmt_buzzer->notation_length) {
        rmt_item32_t notation_code = update_notation_freq_duration(rmt_buzzer);
        // issue a new TX transaction
        rmt_write_items(rmt_buzzer->channel, &notation_code, 1, false);
    }
}

esp_err_t musical_buzzer_create_rmt(const musical_buzzer_config_t *config, musical_buzzer_t **ret_handle)
{
    esp_err_t ret_code = ESP_OK;
    rmt_buzzer_t *rmt_buzzer = NULL;
    BUZZER_CHECK(config, "configuration can't be null", err, ESP_ERR_INVALID_ARG);
    BUZZER_CHECK(ret_handle, "can't assign handle to null", err, ESP_ERR_INVALID_ARG);

    rmt_buzzer = calloc(1, sizeof(rmt_buzzer_t));
    BUZZER_CHECK(rmt_buzzer, "allocate context memory failed", err, ESP_ERR_NO_MEM);

    rmt_buzzer->channel = (rmt_channel_t)config->dev;

    rmt_get_counter_clock(rmt_buzzer->channel, &rmt_buzzer->counter_clk_hz);

    // register tx end callback function, which got invoked when tx loop comes to the end
    rmt_register_tx_end_callback(rmt_tx_loop_end, rmt_buzzer);

    rmt_buzzer->parent.del = buzzer_del;
    rmt_buzzer->parent.play = buzzer_play;
    rmt_buzzer->parent.stop = buzzer_stop;

    *ret_handle = &(rmt_buzzer->parent);
    return ESP_OK;

err:
    if (rmt_buzzer) {
        free(rmt_buzzer);
    }
    return ret_code;
}
