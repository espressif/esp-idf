/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if (BTC_AV_SINK_INCLUDED == TRUE) && (BTC_AV_CODEC_AAC_INCLUDED == TRUE)

/**
 * @brief Extract one AAC RawDataBlock from one MPEG-4 LATM AudioMuxElement.
 *
 * Parses useSameStreamMux, skips StreamMuxConfig when useSameStreamMux is 0,
 * then reads payloadLengthInfo and copies raw AAC bytes to @p out.
 * Supported subset: single program/layer, frameLengthType 0 (typical A2DP).
 *
 * @return 0 on success, -1 on error or if @p out_cap is too small
 */
int btc_a2dp_latm_extract_raw_data_block(const uint8_t *in, size_t in_len,
                                         uint8_t *out, size_t out_cap, size_t *out_len);

#endif

#ifdef __cplusplus
}
#endif
