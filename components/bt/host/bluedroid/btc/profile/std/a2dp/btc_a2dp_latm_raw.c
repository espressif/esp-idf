/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * MPEG-4 LATM AudioMuxElement helpers for A2DP AAC (MPEG-2,4).
 * Sink: parse LATM and extract one raw AAC frame.
 * Source: build LATM from a raw AAC frame and codec configuration.
 */

#include <stddef.h>
#include <string.h>
#if (BTC_AV_SRC_INCLUDED == TRUE)
#include "stack/a2d_m24.h"
#endif
#include "btc_a2dp_latm_raw.h"

#if (BTC_AV_CODEC_AAC_INCLUDED == TRUE)

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
#define BTC_LATM_MPEG4_OT_AAC_ELD                         (39U) /* MPEG-4 AAC-ELD (A2DP ELD v2) */
#define BTC_LATM_MPEG4_OT_AAC_HE_SBR                      (5U)  /* HE-AAC dual-rate wrapper */
#define BTC_LATM_MPEG4_OT_AAC_HE_PS_WRAPPER               (29U) /* HE-AAC v2 style wrapper */
#define BTC_LATM_ASC_GA_BITS                              (3U)  /* frameLengthFlag + dependsOnCoreCoder + extensionFlag */
#define BTC_LATM_ASC_ELD_SPECIFIC_BITS                    (5U)  /* minimal ELDSpecificConfig (no ldSbr) */
#define BTC_LATM_ASC_ELD_EPCONFIG_BITS                    (2U)  /* epConfig for audioObjectType 39 */

#define BTC_LATM_SAMPLING_FREQ_INDEX_ESCAPE               (15U) /* explicit 24-bit Hz follows */

#define BTC_LATM_MPEG4_SYNC_EXT_SYNCWORD                  (0x2b7U)
#define BTC_LATM_MPEG4_SYNC_EXT_TAG_BITS                  (11U) /* syncExtensionType tag width */
#define BTC_LATM_MPEG4_SYNC_EXT_TYPE_SBR                  (5U)
#define BTC_LATM_MPEG4_PS_EXT_SYNCWORD                    (0x548U)

#define BTC_LATM_PAYLOAD_LEN_ESC_BYTE                     (255U)
#define BTC_LATM_PAYLOAD_LEN_ESC_MAX_LOOPS                (32U)

#define BTC_LATM_STREAM_MUX_ALL_FRAMING_AND_SUBFR_BITS    (7U) /* 1 + 6 bits */
#define BTC_LATM_VALUE_BITS_L0                            (2U) /* put_value L=0: 2 bits */

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

#if (BTC_AV_SRC_INCLUDED == TRUE)
/* LATM header: 5 bytes + max_ASC_len(5 bytes) */
#define BTC_LATM_AU_PREFIX_MAX_BYTES        10U
/* Dynamic StreamMuxConfig */
#define BTC_LATM_MUX_AUDIO_VERSION_A2DP     1U
#define BTC_LATM_MUX_AUDIO_VERSION_A_A2DP   0U
#define BTC_TARA_BUFFER_FULLNESS_A2DP       0xFFU
#define BTC_LATM_BUFFER_FULLNESS_A2DP       0xC0U

typedef struct {
    uint8_t *buf;
    size_t  cap;
    unsigned bit_pos;
} btc_a2dp_latm_bw_t;

typedef struct {
    unsigned asc_obj;      /* MPEG-4 audioObjectType (core type when HE wrapper is set) */
    unsigned he_wrapper;   /* 0, or BTC_LATM_MPEG4_OT_AAC_HE_SBR / _HE_PS_WRAPPER */
} btc_a2dp_latm_m24_asc_t;
#endif /* #if (BTC_AV_SRC_INCLUDED == TRUE) */

#if (BTC_AV_SINK_INCLUDED == TRUE)

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

    if (object_type == BTC_LATM_MPEG4_OT_AAC_ELD) {
        if (btc_a2dp_latm_skip_bits(r, BTC_LATM_ASC_ELD_SPECIFIC_BITS) != 0) {
            return -1;
        }
        if (btc_a2dp_latm_skip_bits(r, BTC_LATM_ASC_ELD_EPCONFIG_BITS) != 0) {
            return -1;
        }
        return 0;
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
            if (dep) {
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

int btc_a2dp_latm_check_hdr_len(const uint8_t *in, size_t in_len, size_t *out_bits_len)
{
    if (in == NULL || out_bits_len == NULL || in_len == 0) {
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

    *out_bits_len = r.bit_pos;
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

#endif /* #if (BTC_AV_SINK_INCLUDED == TRUE) */

#if (BTC_AV_SRC_INCLUDED == TRUE)
/* --- LATM mux writer --- */

static inline unsigned btc_a2dp_latm_bw_bits_written(const btc_a2dp_latm_bw_t *w)
{
    return w->bit_pos;
}

static void btc_a2dp_latm_bw_init(btc_a2dp_latm_bw_t *w, uint8_t *buf, size_t cap)
{
    w->buf = buf;
    w->cap = cap;
    w->bit_pos = 0;
    if (buf != NULL && cap > 0U) {
        memset(buf, 0, cap);
    }
}

static int btc_a2dp_latm_put1(btc_a2dp_latm_bw_t *w, int bit)
{
    size_t need = (size_t)((w->bit_pos + 8U) / 8U);

    if (w->buf == NULL || need > w->cap) {
        return -1;
    }
    unsigned byte = w->bit_pos / 8U;
    unsigned bit_in_byte = 7U - (w->bit_pos % 8U);
    if (bit != 0) {
        w->buf[byte] |= (uint8_t)(1U << bit_in_byte);
    }
    w->bit_pos++;
    return 0;
}

static int btc_a2dp_latm_put_bits(btc_a2dp_latm_bw_t *w, unsigned n, unsigned v)
{
    if (n == 0U || n > 32U) {
        return -1;
    }
    for (unsigned i = 0; i < n; i++) {
        unsigned sh = n - 1U - i;
        if (btc_a2dp_latm_put1(w, (int)((v >> sh) & 1U)) != 0) {
            return -1;
        }
    }
    return 0;
}

/* LATM value: 2-bit length L, then (L+1)*8 bits payload (ISO / common mux). */
static int btc_a2dp_latm_put_value(btc_a2dp_latm_bw_t *w, unsigned v)
{
    unsigned l = 0;

    for (l = 0; l < 4U; l++) {
        unsigned nbytes = l + 1U;
        unsigned maxv;

        if (nbytes >= 4U) {
            maxv = 0xffffffffU;
        } else {
            maxv = (1U << (nbytes * 8U)) - 1U;
        }
        if (v <= maxv) {
            break;
        }
    }
    if (l >= 4U) {
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 2, l) != 0) {
        return -1;
    }
    unsigned nbytes = l + 1U;
    for (unsigned i = 0; i < nbytes; i++) {
        unsigned sh = (nbytes - 1U - i) * 8U;
        unsigned b = (v >> sh) & 0xFFU;
        if (btc_a2dp_latm_put_bits(w, 8, b) != 0) {
            return -1;
        }
    }
    return 0;
}

static inline int btc_a2dp_latm_popcount_u(unsigned v)
{
    int n = 0;
    while (v != 0U) {
        n++;
        v &= (v - 1U);
    }
    return n;
}

static int btc_a2dp_latm_m24_cie0_to_asc_profile(uint8_t cie0, btc_a2dp_latm_m24_asc_t *prof)
{
    unsigned m_bits = (unsigned)((cie0 >> 1) & 0x7FU);

    if (m_bits == 0U || btc_a2dp_latm_popcount_u(m_bits) != 1) {
        return -1;
    }

    prof->he_wrapper = 0U;
    prof->asc_obj = 0U;

    if (cie0 & (unsigned)A2D_M24_IE_OBJ_TYPE_2_AAC_LC) {
        prof->asc_obj = BTC_LATM_MPEG4_OT_AAC_LC;
        return 0;
    }
    if (cie0 & (unsigned)A2D_M24_IE_OBJ_TYPE_4_AAC_LC) {
        prof->asc_obj = BTC_LATM_MPEG4_OT_AAC_LC;
        return 0;
    }
    if (cie0 & (unsigned)A2D_M24_IE_OBJ_TYPE_4_AAC_LTP) {
        prof->asc_obj = BTC_LATM_MPEG4_OT_AAC_LTP;
        return 0;
    }
    if (cie0 & (unsigned)A2D_M24_IE_OBJ_TYPE_4_AAC_SCALABLE) {
        prof->asc_obj = BTC_LATM_MPEG4_OT_AAC_SCALABLE;
        return 0;
    }
    if (cie0 & (unsigned)A2D_M24_IE_OBJ_TYPE_4_HE_AAC) {
        prof->he_wrapper = BTC_LATM_MPEG4_OT_AAC_HE_SBR;
        prof->asc_obj = BTC_LATM_MPEG4_OT_AAC_LC;
        return 0;
    }
    if (cie0 & (unsigned)A2D_M24_IE_OBJ_TYPE_4_HE_AAC_V2) {
        prof->he_wrapper = BTC_LATM_MPEG4_OT_AAC_HE_PS_WRAPPER;
        prof->asc_obj = BTC_LATM_MPEG4_OT_AAC_LC;
        return 0;
    }
    if (cie0 & (unsigned)A2D_M24_IE_OBJ_TYPE_4_AAC_ELD_V2) {
        prof->asc_obj = BTC_LATM_MPEG4_OT_AAC_ELD;
        return 0;
    }
    return -1;
}

static int btc_a2dp_latm_m24_sample_rate_hz(const uint8_t *cie, uint32_t *hz_out)
{
    const uint8_t sf1 = cie[1];
    const uint8_t sf2_hi = (uint8_t)((cie[2] & A2D_M24_IE_SAMP_FREQ2_MSK) >> 4);
    const unsigned c1 = (unsigned)btc_a2dp_latm_popcount_u((unsigned)sf1);
    const unsigned c2 = (unsigned)btc_a2dp_latm_popcount_u((unsigned)sf2_hi);

    if (c1 + c2 != 1U) {
        return -1;
    }
    if (c2 != 0U) {
        if (sf2_hi == (unsigned)(A2D_M24_IE_SAMP_FREQ2_48 >> 4)) {
            *hz_out = 48000U;
        } else if (sf2_hi == (unsigned)(A2D_M24_IE_SAMP_FREQ2_64 >> 4)) {
            *hz_out = 64000U;
        } else if (sf2_hi == (unsigned)(A2D_M24_IE_SAMP_FREQ2_88 >> 4)) {
            *hz_out = 88200U;
        } else if (sf2_hi == (unsigned)(A2D_M24_IE_SAMP_FREQ2_96 >> 4)) {
            *hz_out = 96000U;
        } else {
            return -1;
        }
        return 0;
    }
    if (sf1 & A2D_M24_IE_SAMP_FREQ1_8) {
        *hz_out = 8000U;
    } else if (sf1 & A2D_M24_IE_SAMP_FREQ1_11) {
        *hz_out = 11025U;
    } else if (sf1 & A2D_M24_IE_SAMP_FREQ1_12) {
        *hz_out = 12000U;
    } else if (sf1 & A2D_M24_IE_SAMP_FREQ1_16) {
        *hz_out = 16000U;
    } else if (sf1 & A2D_M24_IE_SAMP_FREQ1_22) {
        *hz_out = 22050U;
    } else if (sf1 & A2D_M24_IE_SAMP_FREQ1_24) {
        *hz_out = 24000U;
    } else if (sf1 & A2D_M24_IE_SAMP_FREQ1_32) {
        *hz_out = 32000U;
    } else if (sf1 & A2D_M24_IE_SAMP_FREQ1_44) {
        *hz_out = 44100U;
    } else {
        return -1;
    }
    return 0;
}

static int btc_a2dp_latm_sf_index_to_hz(unsigned sf_index, uint32_t *hz_out)
{
    switch (sf_index) {
    case 0U:
        *hz_out = 96000U;
        return 0;
    case 1U:
        *hz_out = 88200U;
        return 0;
    case 2U:
        *hz_out = 64000U;
        return 0;
    case 3U:
        *hz_out = 48000U;
        return 0;
    case 4U:
        *hz_out = 44100U;
        return 0;
    case 5U:
        *hz_out = 32000U;
        return 0;
    case 6U:
        *hz_out = 24000U;
        return 0;
    case 7U:
        *hz_out = 22050U;
        return 0;
    case 8U:
        *hz_out = 16000U;
        return 0;
    case 9U:
        *hz_out = 12000U;
        return 0;
    case 10U:
        *hz_out = 11025U;
        return 0;
    case 11U:
        *hz_out = 8000U;
        return 0;
    default:
        return -1;
    }
}

static int btc_a2dp_latm_hz_to_sf_index(uint32_t hz, unsigned *sf_index_out)
{
    switch (hz) {
    case 96000U:
        *sf_index_out = 0U;
        return 0;
    case 88200U:
        *sf_index_out = 1U;
        return 0;
    case 64000U:
        *sf_index_out = 2U;
        return 0;
    case 48000U:
        *sf_index_out = 3U;
        return 0;
    case 44100U:
        *sf_index_out = 4U;
        return 0;
    case 32000U:
        *sf_index_out = 5U;
        return 0;
    case 24000U:
        *sf_index_out = 6U;
        return 0;
    case 22050U:
        *sf_index_out = 7U;
        return 0;
    case 16000U:
        *sf_index_out = 8U;
        return 0;
    case 12000U:
        *sf_index_out = 9U;
        return 0;
    case 11025U:
        *sf_index_out = 10U;
        return 0;
    case 8000U:
        *sf_index_out = 11U;
        return 0;
    default:
        return -1;
    }
}

static int btc_a2dp_latm_m24_channel_config(const uint8_t *cie, unsigned *ch_cfg_out)
{
    const unsigned ch = (unsigned)(cie[2] & A2D_M24_IE_CH_MSK);

    if (ch == 0U || btc_a2dp_latm_popcount_u(ch) != 1) {
        return -1;
    }
    if (ch == (unsigned)A2D_M24_IE_CH_1) {
        *ch_cfg_out = 1U;
        return 0;
    }
    if (ch == (unsigned)A2D_M24_IE_CH_2) {
        *ch_cfg_out = 2U;
        return 0;
    }
    if (ch == (unsigned)A2D_M24_IE_CH_6) {
        *ch_cfg_out = 6U;
        return 0;
    }
    if (ch == (unsigned)A2D_M24_IE_CH_8) {
        *ch_cfg_out = 7U;
        return 0;
    }
    return -1;
}

static unsigned btc_a2dp_latm_asc_object_type_bits(unsigned object_type)
{
    return (object_type <= 30U) ? 5U : 11U;
}

static int btc_a2dp_latm_put_asc_object_type(btc_a2dp_latm_bw_t *w, unsigned object_type)
{
    if (object_type <= 30U) {
        return btc_a2dp_latm_put_bits(w, 5, object_type);
    }
    if (object_type > 63U) {
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 5, BTC_LATM_MPEG4_OT_EXTENDED_ESCAPE) != 0) {
        return -1;
    }
    return btc_a2dp_latm_put_bits(w, 6, object_type - BTC_LATM_MPEG4_OT_EXTENDED_BASE);
}

/* A2DP M24 CIE sample rate is the HE-AAC output rate; ASC core rate is half. */
static int btc_a2dp_latm_sf_index_core_from_output(unsigned output_sf_index, unsigned *core_sf_out)
{
    uint32_t hz = 0;

    if (btc_a2dp_latm_sf_index_to_hz(output_sf_index, &hz) != 0 || hz < 2U) {
        return -1;
    }
    return btc_a2dp_latm_hz_to_sf_index(hz / 2U, core_sf_out);
}

static int btc_a2dp_latm_put_ga_specific_config(btc_a2dp_latm_bw_t *w, unsigned depends_on_core_coder)
{
    if (btc_a2dp_latm_put_bits(w, 1, 0U) != 0) { /* frameLengthFlag */
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 1, depends_on_core_coder) != 0) {
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 1, 0U) != 0) { /* extensionFlag */
        return -1;
    }
    return 0;
}

static unsigned btc_a2dp_latm_bits_pad_to_byte(unsigned bits)
{
    unsigned rem = bits % 8U;

    return (rem == 0U) ? 0U : (8U - rem);
}

static unsigned btc_a2dp_latm_asc_bits_for_profile(const btc_a2dp_latm_m24_asc_t *prof)
{
    if (prof->he_wrapper != 0U) {
        return btc_a2dp_latm_asc_object_type_bits(prof->he_wrapper) + 4U + 4U + 4U +
               btc_a2dp_latm_asc_object_type_bits(prof->asc_obj) + BTC_LATM_ASC_GA_BITS;
    }
    if (prof->asc_obj == BTC_LATM_MPEG4_OT_AAC_ELD) {
        return btc_a2dp_latm_asc_object_type_bits(BTC_LATM_MPEG4_OT_AAC_ELD) + 4U + 4U +
               BTC_LATM_ASC_ELD_SPECIFIC_BITS + BTC_LATM_ASC_ELD_EPCONFIG_BITS;
    }
    return btc_a2dp_latm_asc_object_type_bits(prof->asc_obj) + 4U + 4U + BTC_LATM_ASC_GA_BITS;
}

/* AudioSpecificConfig */
static int btc_a2dp_latm_write_audio_specific_config(btc_a2dp_latm_bw_t *w, const btc_a2dp_latm_m24_asc_t *prof,
                                                     unsigned sf_index, unsigned ch_cfg)
{
    unsigned core_sf = 0;

    if (sf_index == BTC_LATM_SAMPLING_FREQ_INDEX_ESCAPE) {
        return -1;
    }

    if (prof->he_wrapper != 0U) {
        if (btc_a2dp_latm_sf_index_core_from_output(sf_index, &core_sf) != 0) {
            return -1;
        }
        if (btc_a2dp_latm_put_asc_object_type(w, prof->he_wrapper) != 0) {
            return -1;
        }
        if (btc_a2dp_latm_put_bits(w, 4, core_sf) != 0) {
            return -1;
        }
        if (btc_a2dp_latm_put_bits(w, 4, ch_cfg) != 0) {
            return -1;
        }
        if (btc_a2dp_latm_put_bits(w, 4, sf_index) != 0) {
            return -1;
        }
        if (btc_a2dp_latm_put_asc_object_type(w, prof->asc_obj) != 0) {
            return -1;
        }
        return btc_a2dp_latm_put_ga_specific_config(w, 0U);
    }

    if (prof->asc_obj == BTC_LATM_MPEG4_OT_AAC_ELD) {
        if (btc_a2dp_latm_put_asc_object_type(w, BTC_LATM_MPEG4_OT_AAC_ELD) != 0) {
            return -1;
        }
        if (btc_a2dp_latm_put_bits(w, 4, sf_index) != 0) {
            return -1;
        }
        if (btc_a2dp_latm_put_bits(w, 4, ch_cfg) != 0) {
            return -1;
        }
        /* ELDSpecificConfig: frameLengthFlag=0, no resilience, ldSbrPresent=0 */
        if (btc_a2dp_latm_put_bits(w, BTC_LATM_ASC_ELD_SPECIFIC_BITS, 0U) != 0) {
            return -1;
        }
        return btc_a2dp_latm_put_bits(w, BTC_LATM_ASC_ELD_EPCONFIG_BITS, 0U);
    }

    if (btc_a2dp_latm_put_asc_object_type(w, prof->asc_obj) != 0) {
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 4, sf_index) != 0) {
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 4, ch_cfg) != 0) {
        return -1;
    }
    return btc_a2dp_latm_put_ga_specific_config(w, 0U);
}

/*
 * Write StreamMuxConfig (after useSameStreamMux=0). @p cie points to 6 A2DP M24 CIE octets.
 * @p audio_mux_version 0: inline AudioSpecificConfig bits.
 *                       1 with @p audio_mux_version_a 0: LATM value taraBufferFullness, then value-length ASC
 *                          (matches btc_a2dp_latm_skip_stream_mux_config when version != 0).
 */
static int btc_a2dp_latm_write_stream_mux_config(btc_a2dp_latm_bw_t *w, const uint8_t *cie,
                                                 uint8_t latm_buffer_fullness, unsigned audio_mux_version,
                                                 unsigned audio_mux_version_a, unsigned tara_buffer_fullness)
{
    btc_a2dp_latm_m24_asc_t prof;
    uint32_t hz = 0;
    unsigned sf_index = 0;
    unsigned ch_cfg = 0;
    unsigned asc_bits = 0;

    if (btc_a2dp_latm_m24_cie0_to_asc_profile(cie[0], &prof) != 0) {
        return -1;
    }
    asc_bits = btc_a2dp_latm_asc_bits_for_profile(&prof);
    if (btc_a2dp_latm_m24_sample_rate_hz(cie, &hz) != 0) {
        return -1;
    }
    if (btc_a2dp_latm_hz_to_sf_index(hz, &sf_index) != 0) {
        return -1;
    }
    if (btc_a2dp_latm_m24_channel_config(cie, &ch_cfg) != 0) {
        return -1;
    }

    if (audio_mux_version > 1U) {
        return -1;
    }
    if (audio_mux_version != 0U && audio_mux_version_a != 0U) {
        /* audioMuxVersionA == 1 path not supported in this mux writer. */
        return -1;
    }

    if (btc_a2dp_latm_put_bits(w, 1, audio_mux_version) != 0) {
        return -1;
    }
    if (audio_mux_version != 0U) {
        if (btc_a2dp_latm_put_bits(w, 1, audio_mux_version_a) != 0) {
            return -1;
        }
        if (btc_a2dp_latm_put_value(w, tara_buffer_fullness) != 0) {
            return -1;
        }
    }

    /* allStreamsSameTimeFraming=1, numSubFrames=0 */
    if (btc_a2dp_latm_put_bits(w, 1, 1U) != 0) {
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 6, 0U) != 0) {
        return -1;
    }

    if (btc_a2dp_latm_put_bits(w, 4, BTC_LATM_MUX_PROGRAM_IDX_FIRST) != 0) {
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 3, BTC_LATM_MUX_LAYER_IDX_FIRST) != 0) {
        return -1;
    }

    if (audio_mux_version == 0U) {
        if (btc_a2dp_latm_write_audio_specific_config(w, &prof, sf_index, ch_cfg) != 0) {
            return -1;
        }
    } else {
        /* ASC length (LATM value L=0) + payload byte-aligned. */
        unsigned asc_pad = btc_a2dp_latm_bits_pad_to_byte(asc_bits + BTC_LATM_VALUE_BITS_L0);
        unsigned asc_bits_ba = asc_bits + asc_pad;

        if (btc_a2dp_latm_put_value(w, asc_bits_ba) != 0) {
            return -1;
        }
        if (btc_a2dp_latm_write_audio_specific_config(w, &prof, sf_index, ch_cfg) != 0) {
            return -1;
        }
        if (asc_pad != 0U && btc_a2dp_latm_put_bits(w, asc_pad, 0U) != 0) {
            return -1;
        }
    }

    if (btc_a2dp_latm_put_bits(w, 3, BTC_LATM_FRAME_LENGTH_TYPE_FIXED_PAYLOAD) != 0) {
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 8, (unsigned)latm_buffer_fullness) != 0) {
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 1, 0U) != 0) { /* otherDataPresent */
        return -1;
    }
    if (btc_a2dp_latm_put_bits(w, 1, 0U) != 0) { /* crcCheckPresent */
        return -1;
    }
    return 0;
}

static int btc_a2dp_latm_write_payload_length_type0(btc_a2dp_latm_bw_t *w, uint16_t payload_len)
{
    unsigned rem = payload_len;
    unsigned loops = 0;

    while (rem >= BTC_LATM_PAYLOAD_LEN_ESC_BYTE) {
        if (btc_a2dp_latm_put_bits(w, 8, BTC_LATM_PAYLOAD_LEN_ESC_BYTE) != 0) {
            return -1;
        }
        rem -= BTC_LATM_PAYLOAD_LEN_ESC_BYTE;
        loops++;
        if (loops > BTC_LATM_PAYLOAD_LEN_ESC_MAX_LOOPS) {
            return -1;
        }
    }
    if (btc_a2dp_latm_put_bits(w, 8, rem) != 0) {
        return -1;
    }
    return 0;
}

static int btc_a2dp_latm_write_raw_payload(btc_a2dp_latm_bw_t *w, const uint8_t *raw_frame, uint16_t raw_len)
{
    for (unsigned i = 0; i < raw_len; i++) {
        if (btc_a2dp_latm_put_bits(w, 8, (unsigned)raw_frame[i]) != 0) {
            return -1;
        }
    }
    return 0;
}

/* useSameStreamMux=0 + StreamMuxConfig bit length from @p codec_cfg (dry-run, no AU payload). */
uint64_t btc_a2dp_latm_stream_mux_config_bits(uint8_t *codec_cfg)
{
    uint8_t scratch[BTC_LATM_AU_PREFIX_MAX_BYTES];
    btc_a2dp_latm_bw_t w;
    const uint8_t *cie;

    if (codec_cfg == NULL) {
        return 0;
    }
    cie = (const uint8_t *)(codec_cfg + A2D_M24_CIE_OFF);

    btc_a2dp_latm_bw_init(&w, scratch, sizeof(scratch));
    if (btc_a2dp_latm_put_bits(&w, 1, 0U) != 0) {
        return 0;
    }
    if (btc_a2dp_latm_write_stream_mux_config(&w, cie, BTC_LATM_BUFFER_FULLNESS_A2DP,
                                              BTC_LATM_MUX_AUDIO_VERSION_A2DP,
                                              BTC_LATM_MUX_AUDIO_VERSION_A_A2DP,
                                              BTC_TARA_BUFFER_FULLNESS_A2DP) != 0) {
        return 0;
    }
    return (uint64_t)btc_a2dp_latm_bw_bits_written(&w);
}

/* payloadLengthInfo (frameLengthType==0) bit length for @p payload_len bytes. */
uint64_t btc_a2dp_latm_payload_length_bits(uint16_t payload_len)
{
    unsigned bits = 0;
    unsigned rem = payload_len;

    while (rem >= BTC_LATM_PAYLOAD_LEN_ESC_BYTE) {
        bits += 8U;
        rem -= BTC_LATM_PAYLOAD_LEN_ESC_BYTE;
    }
    return bits + 8U;
}

uint8_t btc_a2dp_latm_au_prefix_bytes(uint8_t *codec_cfg, uint16_t raw_len)
{
    uint64_t bits;

    if (codec_cfg == NULL) {
        return 0;
    }
    /* stream_mux_config_bits already includes useSameStreamMux (1 bit). */
    bits = btc_a2dp_latm_stream_mux_config_bits(codec_cfg);
    if (bits == 0U) {
        return 0;
    }
    bits += btc_a2dp_latm_payload_length_bits(raw_len);
    if (bits % 8U) {
        return 0;
    }
    return (uint8_t)(bits / 8U);
}

/* Max LATM AU byte length: StreamMuxConfig + payloadLengthInfo + raw AAC (bit-aligned). */
uint16_t btc_a2dp_latm_frame_byte_len(uint8_t *codec_cfg, uint16_t raw_len)
{
    unsigned frame_bits;

    frame_bits = (unsigned)btc_a2dp_latm_stream_mux_config_bits(codec_cfg);
    if (frame_bits == 0U) {
        return 0;
    }
    frame_bits += (unsigned)btc_a2dp_latm_payload_length_bits(raw_len) + ((unsigned)raw_len * 8U);
    return (uint16_t)((frame_bits + 7U) / 8U);
}

/*
 * Build one complete LATM AudioMuxElement (StreamMuxConfig + payloadLengthInfo + raw AAC)
 * into @p out_buf. Returns total byte length, or 0 on error.
 */
uint16_t btc_a2dp_latm_build_frame(uint8_t *codec_cfg, const uint8_t *raw_frame, uint16_t raw_len,
                                   uint8_t *out_buf, uint16_t out_cap)
{
    btc_a2dp_latm_bw_t w;
    const uint8_t *cie;
    unsigned frame_bits;

    if (codec_cfg == NULL || out_buf == NULL || raw_frame == NULL || raw_len == 0U) {
        return 0;
    }
    if (out_buf == raw_frame) {
        return 0;
    }
    cie = (const uint8_t *)(codec_cfg + A2D_M24_CIE_OFF);

    btc_a2dp_latm_bw_init(&w, out_buf, out_cap);
    if (btc_a2dp_latm_put_bits(&w, 1, 0U) != 0) {
        return 0;
    }
    if (btc_a2dp_latm_write_stream_mux_config(&w, cie, BTC_LATM_BUFFER_FULLNESS_A2DP,
                                              BTC_LATM_MUX_AUDIO_VERSION_A2DP,
                                              BTC_LATM_MUX_AUDIO_VERSION_A_A2DP,
                                              BTC_TARA_BUFFER_FULLNESS_A2DP) != 0) {
        return 0;
    }
    if (btc_a2dp_latm_write_payload_length_type0(&w, raw_len) != 0) {
        return 0;
    }
    if (btc_a2dp_latm_write_raw_payload(&w, raw_frame, raw_len) != 0) {
        return 0;
    }

    frame_bits = btc_a2dp_latm_bw_bits_written(&w);
    if (frame_bits == 0U) {
        return 0;
    }
    {
        uint16_t frame_len = (uint16_t)((frame_bits + 7U) / 8U);
        if (frame_len > out_cap) {
            return 0;
        }
        return frame_len;
    }
}

/*
 * Build one LATM header (StreamMuxConfig + payloadLengthInfo)
 * into @p out_buf. Returns total byte length, or 0 on error.
 */
bool btc_a2dp_latm_build_hdr(uint8_t *codec_cfg, uint16_t raw_len,
                                 uint8_t *out_buf, uint16_t out_cap)
{
    btc_a2dp_latm_bw_t w;
    const uint8_t *cie;
    unsigned frame_bits;

    if (codec_cfg == NULL || out_buf == NULL || raw_len == 0U) {
        return false;
    }
    cie = (const uint8_t *)(codec_cfg + A2D_M24_CIE_OFF);

    btc_a2dp_latm_bw_init(&w, out_buf, out_cap);
    if (btc_a2dp_latm_put_bits(&w, 1, 0U) != 0) {
        return false;
    }
    if (btc_a2dp_latm_write_stream_mux_config(&w, cie, BTC_LATM_BUFFER_FULLNESS_A2DP,
                                              BTC_LATM_MUX_AUDIO_VERSION_A2DP,
                                              BTC_LATM_MUX_AUDIO_VERSION_A_A2DP,
                                              BTC_TARA_BUFFER_FULLNESS_A2DP) != 0) {
        return false;
    }
    if (btc_a2dp_latm_write_payload_length_type0(&w, raw_len) != 0) {
        return false;
    }

    frame_bits = btc_a2dp_latm_bw_bits_written(&w);
    if (frame_bits == 0U || frame_bits % 8U) {
        return false;
    }
    return true;
}

#endif /* #if (BTC_AV_SRC_INCLUDED == TRUE) */

#endif /* #if (BTC_AV_CODEC_AAC_INCLUDED == TRUE) */
