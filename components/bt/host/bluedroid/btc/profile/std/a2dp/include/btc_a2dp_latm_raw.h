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

#if (BTC_AV_CODEC_AAC_INCLUDED == TRUE)

#if (BTC_AV_SINK_INCLUDED == TRUE)

/**
 * @brief Parse a LATM AudioMuxElement header and validate the AAC payload size.
 *
 * @param in            LATM input buffer (full AudioMuxElement or prefix)
 * @param in_len        Length of @p in in bytes
 * @param out_bits_len  On success, number of header bits consumed before AAC payload
 *
 * @return 0 on success, -1 on parse error, unsupported mux, insufficient @p in_len,
 *         or if decoded payload length exceeds @p out_cap
 */
int btc_a2dp_latm_check_hdr_len(const uint8_t *in, size_t in_len, size_t *out_bits_len);

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
#endif /* #if (BTC_AV_SINK_INCLUDED == TRUE) */

#if (BTC_AV_SRC_INCLUDED == TRUE)
/**
 * @brief Compute the byte length of one LATM AudioMuxElement for a raw AAC frame.
 *
 * Sums useSameStreamMux + StreamMuxConfig (from @p codec_cfg), payloadLengthInfo
 * (frameLengthType 0), and @p raw_len bytes of AAC payload, rounded up to whole bytes.
 *
 * @param codec_cfg  A2DP codec configuration (M24 CIE at A2D_M24_CIE_OFF)
 * @param raw_len    Raw AAC frame length in bytes
 *
 * @return Required output size in bytes, or 0 if @p codec_cfg is invalid or unsupported
 */
uint16_t btc_a2dp_latm_frame_byte_len(uint8_t *codec_cfg, uint16_t raw_len);

/**
 * @brief Build one MPEG-4 LATM AudioMuxElement from a raw AAC frame.
 *
 * Writes useSameStreamMux=0, StreamMuxConfig derived from @p codec_cfg,
 * payloadLengthInfo, and @p raw_frame into @p out_buf (bit-aligned).
 * Supported subset: single program/layer, frameLengthType 0 (typical A2DP source).
 * M24 object types: MPEG-2/4 AAC LC, AAC LTP, AAC scalable, HE-AAC, HE-AAC v2, AAC-ELD v2.
 *
 * @param codec_cfg  A2DP codec configuration (M24 CIE at A2D_M24_CIE_OFF)
 * @param raw_frame  Raw AAC access unit (no ADTS)
 * @param raw_len    Length of @p raw_frame in bytes
 * @param out_buf    Output buffer (must not overlap @p raw_frame)
 * @param out_cap    Capacity of @p out_buf in bytes
 *
 * @return Total bytes written, or 0 on error or if @p out_cap is too small
 */
uint16_t btc_a2dp_latm_build_frame(uint8_t *codec_cfg, const uint8_t *raw_frame, uint16_t raw_len,
                                   uint8_t *out_buf, uint16_t out_cap);

/**
 * @brief Write the LATM header
 *
 * The header bit length must be a multiple of 8 so the AAC payload stays byte-aligned.
 *
 * @param codec_cfg  A2DP codec configuration (M24 CIE at A2D_M24_CIE_OFF)
 * @param raw_len    Raw AAC access unit length in bytes (also encoded in payloadLengthInfo)
 * @param out_buf    Output buffer start; must not overlap the raw AAC region that follows
 * @param out_cap    Bytes available from @p out_buf up to (but not including) the raw AAC data
 *
 * @return true on success, false if @p codec_cfg or @p out_buf is NULL, @p raw_len is 0,
 *         @p codec_cfg is unsupported, @p out_cap is too small, or the header is not byte-aligned
 */
bool btc_a2dp_latm_build_hdr(uint8_t *codec_cfg, uint16_t raw_len,
                             uint8_t *out_buf, uint16_t out_cap);

/**
 * @brief Bit length of useSameStreamMux=0 plus StreamMuxConfig for @p codec_cfg.
 *
 * Dry-run mux write into a scratch buffer; does not include payloadLengthInfo or AAC data.
 *
 * @param codec_cfg  A2DP codec configuration (M24 CIE at A2D_M24_CIE_OFF)
 *
 * @return Number of bits, or 0 if @p codec_cfg is NULL or unsupported
 */
uint64_t btc_a2dp_latm_stream_mux_config_bits(uint8_t *codec_cfg);

/**
 * @brief Byte length of the LATM header before raw AAC (in-place build with btc_a2dp_latm_build_hdr).
 *
 * Covers useSameStreamMux=0, StreamMuxConfig (from @p codec_cfg), and payloadLengthInfo for
 * @p raw_len. The result is byte-aligned so raw AAC can follow immediately after the prefix.
 * Typical values: 10 bytes (AAC LC / LTP / Scalable) or 11 bytes (HE-AAC, HE-AAC v2, AAC-ELD v2)
 * when @p raw_len is under 255 bytes.
 *
 * @param codec_cfg  A2DP codec configuration (M24 CIE at A2D_M24_CIE_OFF)
 * @param raw_len    Raw AAC access unit length in bytes (encoded in payloadLengthInfo)
 *
 * @return Prefix size in bytes, or 0 if @p codec_cfg is NULL, unsupported, or not byte-aligned
 */
uint8_t btc_a2dp_latm_au_prefix_bytes(uint8_t *codec_cfg, uint16_t raw_len);

/**
 * @brief Bit length of payloadLengthInfo for frameLengthType 0.
 *
 * @param payload_len  Raw AAC payload length in bytes
 *
 * @return Number of bits in the ISO 14496-3 length escape sequence
 */
uint64_t btc_a2dp_latm_payload_length_bits(uint16_t payload_len);
#endif /* #if (BTC_AV_SRC_INCLUDED == TRUE) */

#endif /* #if (BTC_AV_CODEC_AAC_INCLUDED == TRUE) */

#ifdef __cplusplus
}
#endif
