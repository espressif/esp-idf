/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "audio_sink_service.h"

#if defined(CONFIG_EXAMPLE_A2DP_SINK_OUTPUT_IDLE)

/*******************************
 * EXTERNAL FUNCTION DEFINITIONS
 ******************************/

void audio_sink_srv_open(void)
{
    // do nothing
}

void audio_sink_srv_close(void)
{
    // do nothing
}

void audio_sink_srv_start(void)
{
    // do nothing
}

void audio_sink_srv_stop(void)
{
    // do nothing
}

void audio_sink_srv_codec_info_update(esp_a2d_mcc_t *mcc)
{
    // do nothing
    (void)mcc;
}

size_t audio_sink_srv_data_output(const uint8_t *data, size_t size)
{
    // do nothing
    (void)data;
    (void)size;
    return 0;
}

#endif /* defined(CONFIG_EXAMPLE_A2DP_SINK_OUTPUT_IDLE) */
