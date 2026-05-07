/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * MPEG-4 LATM AudioMuxElement parsing for A2DP AAC (MPEG-2,4): skip StreamMuxConfig
 * when useSameStreamMux is 0, then extract one raw AAC frame (payloadLengthInfo +
 * RawDataBlock). Used by the BTC A2DP sink external-codec path.
 */

#include <stddef.h>
#include <string.h>
#include "btc_a2dp_latm_raw.h"

#if (BTC_AV_SINK_INCLUDED == TRUE) && (BTC_AV_CODEC_AAC_INCLUDED == TRUE)

/*
 * ISO/IEC 14496-3 (LATM / AudioSpecificConfig / GASpecificConfig) — numeric literals
 * used only where needed for parsing StreamMuxConfig; names reflect MPEG-4 audio tables.
 */
#define BTC_LATM_MPEG4_AUDIO_SPECIFIC_CFG_BITS_MAX        (8192U)

#define BTC_LATM_MPEG4_OT_EXTENDED_ESCAPE                 (31U) /* object_type escape → +6 bits */
#define BTC_LATM_MPEG4_OT_EXTENDED_BASE                   (32U)

#define BTC_LATM_MPEG4_OT_AAC_MAIN                        (1U)
#define BTC_LATM_MPEG4_OT_AAC_LC                          (2U)
#define BTC_LATM_MPEG4_OT_AAC_SSR                         (3U)
#define BTC_LATM_MPEG4_OT_AAC_LTP                         (4U)
#define BTC_LATM_MPEG4_OT_AAC_SCALABLE                    (6U)
#define BTC_LATM_MPEG4_OT_TWINVQ                          (7U)
#define BTC_LATM_MPEG4_OT_ER_AAC_LC                       (17U)
#define BTC_LATM_MPEG4_OT_ER_AAC_LTP                      (19U)
#define BTC_LATM_MPEG4_OT_ER_AAC_SCALABLE                 (20U)
#define BTC_LATM_MPEG4_OT_ER_TWINVQ                       (21U)
#define BTC_LATM_MPEG4_OT_ER_BSAC                         (22U)
#define BTC_LATM_MPEG4_OT_ER_AAC_LD                       (23U)
#define BTC_LATM_MPEG4_OT_AAC_HE_SBR                      (5U)  /* HE-AAC dual-rate wrapper */
#define BTC_LATM_MPEG4_OT_AAC_HE_PS_WRAPPER               (29U) /* HE-AAC v2 style wrapper */

#define BTC_LATM_SAMPLING_FREQ_INDEX_ESCAPE               (15U) /* explicit 24-bit Hz follows */

#define BTC_LATM_MPEG4_SYNC_EXT_SYNCWORD                  (0x2b7U)
#define BTC_LATM_MPEG4_SYNC_EXT_TAG_BITS                  (11U) /* syncExtensionType tag width */
#define BTC_LATM_MPEG4_SYNC_EXT_TYPE_SBR                  (5U)
#define BTC_LATM_MPEG4_PS_EXT_SYNCWORD                    (0x548U)

#define BTC_LATM_PAYLOAD_LEN_ESC_BYTE                     (255U)
#define BTC_LATM_PAYLOAD_LEN_ESC_MAX_LOOPS               (32U)

#define BTC_LATM_STREAM_MUX_ALL_FRAMING_AND_SUBFR_BITS    (7U) /* 1 + 6 bits */

#define BTC_LATM_MUX_PROGRAM_IDX_FIRST                    (0U)
#define BTC_LATM_MUX_LAYER_IDX_FIRST                      (0U)

#define BTC_LATM_SYNC_EXT_SCAN_MIN_REMAINING_BITS         (15U)
#define BTC_LATM_PS_EXT_PROBE_MIN_REMAINING_BITS          (11U)

#define BTC_LATM_FRAME_LENGTH_TYPE_FIXED_PAYLOAD          (0U)
#define BTC_LATM_FRAME_LENGTH_TYPE_VARIABLE_PAYLOAD       (1U)
#define BTC_LATM_FRAME_LENGTH_TYPE_FIXED_IDX_3            (3U)
#define BTC_LATM_FRAME_LENGTH_TYPE_FIXED_IDX_4            (4U)
#define BTC_LATM_FRAME_LENGTH_TYPE_FIXED_IDX_5            (5U)
#define BTC_LATM_FRAME_LENGTH_TYPE_FIXED_IDX_6            (6U)
#define BTC_LATM_FRAME_LENGTH_TYPE_FIXED_IDX_7            (7U)

typedef struct {
    const uint8_t *buf;
    size_t         byte_len;
    unsigned       bit_pos;
} btc_a2dp_latm_br_t;

static inline unsigned btc_a2dp_latm_bits_total(const btc_a2dp_latm_br_t *r)
{
    return (unsigned)(r->byte_len * 8);
}

static inline int btc_a2dp_latm_get1(btc_a2dp_latm_br_t *r)
{
    if (r->bit_pos >= btc_a2dp_latm_bits_total(r)) {
        return -1;
    }
    unsigned byte = r->bit_pos / 8U;
    unsigned bit_in_byte = 7U - (r->bit_pos % 8U);
    int v = (int)((r->buf[byte] >> bit_in_byte) & 1U);
    r->bit_pos++;
    return v;
}

static int btc_a2dp_latm_get_bits(btc_a2dp_latm_br_t *r, unsigned n, unsigned *out)
{
    unsigned v = 0;
    for (unsigned i = 0; i < n; i++) {
        int b = btc_a2dp_latm_get1(r);
        if (b < 0) {
            return -1;
        }
        v = (v << 1) | (unsigned)b;
    }
    *out = v;
    return 0;
}

/* LATM value: 2-bit length L, then (L+1)*8 bits payload (ISO / common mux). */
static int btc_a2dp_latm_get_value(btc_a2dp_latm_br_t *r, unsigned *out_value)
{
    unsigned l = 0;
    if (btc_a2dp_latm_get_bits(r, 2, &l) != 0) {
        return -1;
    }
    unsigned nbytes = l + 1U;
    unsigned acc = 0;
    for (unsigned i = 0; i < nbytes * 8U; i++) {
        int b = btc_a2dp_latm_get1(r);
        if (b < 0) {
            return -1;
        }
        acc = (acc << 1) | (unsigned)b;
    }
    *out_value = acc;
    return 0;
}

static int btc_a2dp_latm_skip_bits(btc_a2dp_latm_br_t *r, unsigned n)
{
    for (unsigned i = 0; i < n; i++) {
        if (btc_a2dp_latm_get1(r) < 0) {
            return -1;
        }
    }
    return 0;
}

/* Skip AudioSpecificConfig / GASpecific for common AAC-LC and typical extensions. */
static int btc_a2dp_latm_skip_audio_specific_config(btc_a2dp_latm_br_t *r)
{
    unsigned object_type = 0;
    if (btc_a2dp_latm_get_bits(r, 5, &object_type) != 0) {
        return -1;
    }
    if (object_type == BTC_LATM_MPEG4_OT_EXTENDED_ESCAPE) {
        unsigned ext = 0;
        if (btc_a2dp_latm_get_bits(r, 6, &ext) != 0) {
            return -1;
        }
        object_type = BTC_LATM_MPEG4_OT_EXTENDED_BASE + ext;
    }

    unsigned sf_index = 0;
    if (btc_a2dp_latm_get_bits(r, 4, &sf_index) != 0) {
        return -1;
    }
    if (sf_index == BTC_LATM_SAMPLING_FREQ_INDEX_ESCAPE) {
        if (btc_a2dp_latm_skip_bits(r, 24) != 0) {
            return -1;
        }
    }

    if (btc_a2dp_latm_skip_bits(r, 4) != 0) { /* channelConfiguration */
        return -1;
    }

    /* HE-AAC v1/v2 dual-rate wrapper (AAC scalable / PS wrapper object types). */
    if (object_type == BTC_LATM_MPEG4_OT_AAC_HE_SBR || object_type == BTC_LATM_MPEG4_OT_AAC_HE_PS_WRAPPER) {
        unsigned ext_sf = 0;
        if (btc_a2dp_latm_get_bits(r, 4, &ext_sf) != 0) {
            return -1;
        }
        if (ext_sf == BTC_LATM_SAMPLING_FREQ_INDEX_ESCAPE) {
            if (btc_a2dp_latm_skip_bits(r, 24) != 0) {
                return -1;
            }
        }
        if (btc_a2dp_latm_get_bits(r, 5, &object_type) != 0) {
            return -1;
        }
        if (object_type == BTC_LATM_MPEG4_OT_EXTENDED_ESCAPE) {
            unsigned ext = 0;
            if (btc_a2dp_latm_get_bits(r, 6, &ext) != 0) {
                return -1;
            }
            object_type = BTC_LATM_MPEG4_OT_EXTENDED_BASE + ext;
        }
        if (object_type == BTC_LATM_MPEG4_OT_ER_BSAC) {
            if (btc_a2dp_latm_skip_bits(r, 4) != 0) {
                return -1;
            }
        }
    }

    /* GASpecificConfig for core object types (incl. AAC LC). */
    switch (object_type) {
    case BTC_LATM_MPEG4_OT_AAC_MAIN:
    case BTC_LATM_MPEG4_OT_AAC_LC:
    case BTC_LATM_MPEG4_OT_AAC_SSR:
    case BTC_LATM_MPEG4_OT_AAC_LTP:
    case BTC_LATM_MPEG4_OT_AAC_SCALABLE:
    case BTC_LATM_MPEG4_OT_TWINVQ:
    case BTC_LATM_MPEG4_OT_ER_AAC_LC:
    case BTC_LATM_MPEG4_OT_ER_AAC_LTP:
    case BTC_LATM_MPEG4_OT_ER_AAC_SCALABLE:
    case BTC_LATM_MPEG4_OT_ER_TWINVQ:
    case BTC_LATM_MPEG4_OT_ER_BSAC:
    case BTC_LATM_MPEG4_OT_ER_AAC_LD:
        if (btc_a2dp_latm_skip_bits(r, 1) != 0) { /* frameLengthFlag */
            return -1;
        }
        {
            unsigned dep = 0;
            if (btc_a2dp_latm_get_bits(r, 1, &dep) != 0) {
                return -1;
            }
            if (dep == 0U) {
                if (btc_a2dp_latm_skip_bits(r, 14) != 0) { /* coreCoderDelay */
                    return -1;
                }
            }
        }
        {
            unsigned ext_flag = 0;
            if (btc_a2dp_latm_get_bits(r, 1, &ext_flag) != 0) {
                return -1;
            }
            if (ext_flag != 0U) {
                /* Uncommon for A2DP; skip conservatively until alignment risk. */
                return -1;
            }
        }
        break;
    default:
        /* Fall through to generic sync-extension scan. */
        break;
    }

    /* Sync extension and trailing bits (similar role to ff_mpeg4audio_get_config_gb). */
    while (btc_a2dp_latm_bits_total(r) - r->bit_pos > BTC_LATM_SYNC_EXT_SCAN_MIN_REMAINING_BITS) {
        unsigned show = 0;
        unsigned save = r->bit_pos;
        if (btc_a2dp_latm_get_bits(r, BTC_LATM_MPEG4_SYNC_EXT_TAG_BITS, &show) != 0) {
            r->bit_pos = save;
            break;
        }
        if (show == BTC_LATM_MPEG4_SYNC_EXT_SYNCWORD) {
            unsigned ext_type = 0;
            if (btc_a2dp_latm_get_bits(r, 5, &ext_type) != 0) {
                return -1;
            }
            if (ext_type == BTC_LATM_MPEG4_SYNC_EXT_TYPE_SBR) {
                if (btc_a2dp_latm_skip_bits(r, 1) != 0) {
                    return -1;
                }
                unsigned sbr_rate = 0;
                if (btc_a2dp_latm_get_bits(r, 4, &sbr_rate) != 0) {
                    return -1;
                }
                if (sbr_rate == BTC_LATM_SAMPLING_FREQ_INDEX_ESCAPE) {
                    if (btc_a2dp_latm_skip_bits(r, 24) != 0) {
                        return -1;
                    }
                }
                if (btc_a2dp_latm_bits_total(r) - r->bit_pos > BTC_LATM_PS_EXT_PROBE_MIN_REMAINING_BITS) {
                    unsigned show2 = 0;
                    unsigned s2 = r->bit_pos;
                    if (btc_a2dp_latm_get_bits(r, BTC_LATM_MPEG4_SYNC_EXT_TAG_BITS, &show2) != 0) {
                        r->bit_pos = s2;
                        break;
                    }
                    if (show2 == BTC_LATM_MPEG4_PS_EXT_SYNCWORD) {
                        if (btc_a2dp_latm_skip_bits(r, 1) != 0) {
                            return -1;
                        }
                    } else {
                        r->bit_pos = s2;
                    }
                }
            } else {
                r->bit_pos = save;
                if (btc_a2dp_latm_get1(r) < 0) {
                    break;
                }
            }
        } else {
            r->bit_pos = save;
            if (btc_a2dp_latm_get1(r) < 0) {
                break;
            }
        }
    }

    return 0;
}

static int btc_a2dp_latm_skip_stream_mux_config(btc_a2dp_latm_br_t *r, unsigned *audio_mux_version_a, unsigned *frame_length_type_o)
{
    unsigned audio_mux_version = 0;
    if (btc_a2dp_latm_get_bits(r, 1, &audio_mux_version) != 0) {
        return -1;
    }
    *audio_mux_version_a = 0;
    if (audio_mux_version != 0U) {
        if (btc_a2dp_latm_get_bits(r, 1, audio_mux_version_a) != 0) {
            return -1;
        }
    }

    if (*audio_mux_version_a != 0U) {
        /* audioMuxVersionA == 1 path not supported in this example stripper. */
        return -1;
    }

    if (audio_mux_version != 0U) {
        unsigned dummy = 0;
        if (btc_a2dp_latm_get_value(r, &dummy) != 0) {
            return -1;
        }
    }

    if (btc_a2dp_latm_skip_bits(r, BTC_LATM_STREAM_MUX_ALL_FRAMING_AND_SUBFR_BITS) != 0) {
        /* allStreamsSameTimeFraming + numSubFrames */
        return -1;
    }

    unsigned num_programs = 0;
    if (btc_a2dp_latm_get_bits(r, 4, &num_programs) != 0) {
        return -1;
    }
    if (num_programs != BTC_LATM_MUX_PROGRAM_IDX_FIRST) {
        return -1;
    }

    unsigned num_layer = 0;
    if (btc_a2dp_latm_get_bits(r, 3, &num_layer) != 0) {
        return -1;
    }
    if (num_layer != BTC_LATM_MUX_LAYER_IDX_FIRST) {
        return -1;
    }

    if (audio_mux_version == 0U) {
        if (btc_a2dp_latm_skip_audio_specific_config(r) != 0) {
            return -1;
        }
    } else {
        unsigned asc_bits = 0;
        if (btc_a2dp_latm_get_value(r, &asc_bits) != 0) {
            return -1;
        }
        if (asc_bits > BTC_LATM_MPEG4_AUDIO_SPECIFIC_CFG_BITS_MAX) {
            return -1;
        }
        if (btc_a2dp_latm_skip_bits(r, asc_bits) != 0) {
            return -1;
        }
    }

    unsigned frame_length_type = 0;
    if (btc_a2dp_latm_get_bits(r, 3, &frame_length_type) != 0) {
        return -1;
    }
    if (frame_length_type_o != NULL) {
        *frame_length_type_o = frame_length_type;
    }
    switch (frame_length_type) {
    case BTC_LATM_FRAME_LENGTH_TYPE_FIXED_PAYLOAD:
        if (btc_a2dp_latm_skip_bits(r, 8) != 0) {
            return -1;
        }
        break;
    case BTC_LATM_FRAME_LENGTH_TYPE_VARIABLE_PAYLOAD:
        if (btc_a2dp_latm_skip_bits(r, 9) != 0) {
            return -1;
        }
        break;
    case BTC_LATM_FRAME_LENGTH_TYPE_FIXED_IDX_3:
    case BTC_LATM_FRAME_LENGTH_TYPE_FIXED_IDX_4:
    case BTC_LATM_FRAME_LENGTH_TYPE_FIXED_IDX_5:
        if (btc_a2dp_latm_skip_bits(r, 6) != 0) {
            return -1;
        }
        break;
    case BTC_LATM_FRAME_LENGTH_TYPE_FIXED_IDX_6:
    case BTC_LATM_FRAME_LENGTH_TYPE_FIXED_IDX_7:
        if (btc_a2dp_latm_skip_bits(r, 1) != 0) {
            return -1;
        }
        break;
    default:
        break;
    }

    unsigned other_data = 0;
    if (btc_a2dp_latm_get_bits(r, 1, &other_data) != 0) {
        return -1;
    }
    if (other_data != 0U) {
        if (audio_mux_version != 0U) {
            unsigned bits = 0;
            if (btc_a2dp_latm_get_value(r, &bits) != 0) {
                return -1;
            }
            if (bits > BTC_LATM_MPEG4_AUDIO_SPECIFIC_CFG_BITS_MAX) {
                return -1;
            }
            if (btc_a2dp_latm_skip_bits(r, bits) != 0) {
                return -1;
            }
        } else {
            unsigned esc_u = 0;
            do {
                if (btc_a2dp_latm_bits_total(r) - r->bit_pos < 9U) {
                    return -1;
                }
                if (btc_a2dp_latm_get_bits(r, 1, &esc_u) != 0) {
                    return -1;
                }
                if (btc_a2dp_latm_skip_bits(r, 8) != 0) {
                    return -1;
                }
            } while (esc_u != 0U);
        }
    }

    unsigned crc = 0;
    if (btc_a2dp_latm_get_bits(r, 1, &crc) != 0) {
        return -1;
    }
    if (crc != 0U) {
        if (btc_a2dp_latm_skip_bits(r, 8) != 0) {
            return -1;
        }
    }
    return 0;
}

static int btc_a2dp_latm_get_payload_length_type0(btc_a2dp_latm_br_t *r, unsigned *payload_len_bytes)
{
    unsigned total = 0;
    unsigned loops = 0;
    while (1) {
        unsigned tmp = 0;
        if (btc_a2dp_latm_get_bits(r, 8, &tmp) != 0) {
            return -1;
        }
        total += tmp;
        loops++;
        if (tmp != BTC_LATM_PAYLOAD_LEN_ESC_BYTE) {
            break;
        }
        /* Defensive loop bound for malformed data. */
        if (loops > BTC_LATM_PAYLOAD_LEN_ESC_MAX_LOOPS) {
            return -1;
        }
    }
    *payload_len_bytes = total;
    return 0;
}

int btc_a2dp_latm_extract_raw_data_block(const uint8_t *in, size_t in_len,
                                         uint8_t *out, size_t out_cap, size_t *out_len)
{
    if (in == NULL || out == NULL || out_len == NULL || in_len == 0) {
        return -1;
    }
    if (in == out) {
        return -1;
    }

    btc_a2dp_latm_br_t r = { .buf = in, .byte_len = in_len, .bit_pos = 0 };
    unsigned use_same = 0;
    if (btc_a2dp_latm_get_bits(&r, 1, &use_same) != 0) {
        return -1;
    }

    unsigned frame_length_type = 0;
    if (use_same == 0U) {
        unsigned audio_mux_version_a = 0;
        btc_a2dp_latm_br_t mux = { .buf = in, .byte_len = in_len, .bit_pos = 1U };
        if (btc_a2dp_latm_skip_stream_mux_config(&mux, &audio_mux_version_a, &frame_length_type) != 0) {
            return -1;
        }
        r.bit_pos = mux.bit_pos;
    } else {
        /*
         * useSameStreamMux=1 reuses previous StreamMuxConfig. In A2DP AAC-LATM
         * streams this is typically frameLengthType=0.
         */
        frame_length_type = BTC_LATM_FRAME_LENGTH_TYPE_FIXED_PAYLOAD;
    }

    if (frame_length_type != BTC_LATM_FRAME_LENGTH_TYPE_FIXED_PAYLOAD) {
        return -1;
    }

    unsigned payload_len_bytes = 0;
    if (btc_a2dp_latm_get_payload_length_type0(&r, &payload_len_bytes) != 0) {
        return -1;
    }
    if (payload_len_bytes > out_cap) {
        return -1;
    }

    unsigned payload_bits = payload_len_bytes * 8U;
    if (btc_a2dp_latm_bits_total(&r) - r.bit_pos < payload_bits) {
        return -1;
    }

    /* Payload may start mid-byte after StreamMuxConfig. */
    if ((r.bit_pos % 8U) == 0U) {
        const size_t off = (size_t)(r.bit_pos / 8U);
        if (off + (size_t)payload_len_bytes > in_len) {
            return -1;
        }
        memcpy(out, in + off, (size_t)payload_len_bytes);
        r.bit_pos += payload_bits;
    } else {
        for (unsigned j = 0; j < payload_len_bytes; j++) {
            unsigned v = 0;
            if (btc_a2dp_latm_get_bits(&r, 8, &v) != 0) {
                return -1;
            }
            out[j] = (uint8_t)v;
        }
    }
    *out_len = payload_len_bytes;
    return 0;
}

#endif /* #if (BTC_AV_SINK_INCLUDED == TRUE) && (BTC_AV_CODEC_AAC_INCLUDED == TRUE) */
