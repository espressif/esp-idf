/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************
 *
 *  MPEG-2, 4 AAC (M24) codec helpers for A2DP.
 *
 ******************************************************************************/

#include "common/bt_target.h"
#include "stack/a2d_api.h"
#include "stack/a2d_m24.h"
#include "bta/bta_av_m24.h"

#if (BTA_AV_INCLUDED == TRUE) && (BTA_AV_CODEC_AAC_INCLUDED == TRUE)

#define BTA_AV_M24_VBR_BR1_OFF       6
#define BTA_AV_M24_BR2_OFF           7
#define BTA_AV_M24_BR3_OFF           8

static UINT32 bta_av_m24_br(const tA2D_M24_CIE *p)
{
    return (((UINT32)(p->br1 & A2D_M24_IE_BR1_MSK)) << 16) |
           (((UINT32)(p->br2 & A2D_M24_IE_BR2_MSK)) << 8) |
           ((UINT32)(p->br3 & A2D_M24_IE_BR3_MSK));
}

static void bta_av_m24_set_br(tA2D_M24_CIE *p, UINT32 v, UINT8 vbr_flag)
{
    v &= 0x7FFFFFU;
    p->vbr = (UINT8)(vbr_flag & A2D_M24_IE_VBR_MSK);
    p->br1 = (UINT8)((v >> 16) & A2D_M24_IE_BR1_MSK);
    p->br2 = (UINT8)((v >> 8) & A2D_M24_IE_BR2_MSK);
    p->br3 = (UINT8)(v & A2D_M24_IE_BR3_MSK);
}

UINT32 bta_av_m24_br_info(const UINT8 *codec_info)
{
    return ((UINT32)((codec_info[BTA_AV_M24_VBR_BR1_OFF] & A2D_M24_IE_BR1_MSK) << 16) |
            (UINT32)((codec_info[BTA_AV_M24_BR2_OFF]     & A2D_M24_IE_BR2_MSK) << 8 ) |
            (UINT32)( codec_info[BTA_AV_M24_BR3_OFF]     & A2D_M24_IE_BR3_MSK));
}

void bta_av_m24_set_br_info(UINT8 *codec_info, UINT32 bit_rate)
{
    bit_rate &= 0x7FFFFFU;
    codec_info[BTA_AV_M24_VBR_BR1_OFF] = (UINT8)(codec_info[BTA_AV_M24_VBR_BR1_OFF] & A2D_M24_IE_VBR_MSK) |
                                         (UINT8)((bit_rate >> 16) & A2D_M24_IE_BR1_MSK);
    codec_info[BTA_AV_M24_BR2_OFF] = (UINT8)((bit_rate >> 8) & A2D_M24_IE_BR2_MSK);
    codec_info[BTA_AV_M24_BR3_OFF] = (UINT8)(bit_rate & A2D_M24_IE_BR3_MSK);
}

UINT8 bta_av_m24_cap_matches_cap(UINT8 *p_cap1, UINT8 *p_cap2)
{
    tA2D_M24_CIE cap1;
    tA2D_M24_CIE cap2;
    UINT8 st;
    UINT8 cap1_sf1;
    UINT8 cap2_sf1;
    UINT8 cap1_sf2;
    UINT8 cap2_sf2;
    UINT32 cap1_br;
    UINT32 cap2_br;

    st = A2D_ParsM24Info(&cap1, p_cap1, TRUE);
    if (st != A2D_SUCCESS) {
        return st;
    }
    st = A2D_ParsM24Info(&cap2, p_cap2, TRUE);
    if (st != A2D_SUCCESS) {
        return st;
    }

    if ((cap1.obj_type & cap2.obj_type) == 0) {
        return A2D_NS_OBJ_TYPE;
    }

    cap1_sf1 = (UINT8)(cap1.samp_freq1 & A2D_M24_IE_SAMP_FREQ1_MSK);
    cap2_sf1 = (UINT8)(cap2.samp_freq1 & A2D_M24_IE_SAMP_FREQ1_MSK);
    cap1_sf2 = (UINT8)(cap1.samp_freq2 & A2D_M24_IE_SAMP_FREQ2_MSK);
    cap2_sf2 = (UINT8)(cap2.samp_freq2 & A2D_M24_IE_SAMP_FREQ2_MSK);

    if (!(cap1_sf1 & cap2_sf1) && !(cap1_sf2 & cap2_sf2)) {
        return A2D_NS_SAMP_FREQ;
    }
    if ((cap1.ch & cap2.ch) == 0) {
        return A2D_NS_CHANNEL;
    }

    cap1_br = bta_av_m24_br(&cap1);
    cap2_br = bta_av_m24_br(&cap2);

    if (!cap1_br || !cap2_br) {
        return A2D_NS_BIT_RATE;
    }

    return A2D_SUCCESS;

}

UINT8 bta_av_m24_cfg_in_external_codec_cap(UINT8 *p_cfg, UINT8 *p_cap)
{
    tA2D_M24_CIE cfg;
    tA2D_M24_CIE cap;
    UINT8 st;
    UINT8 cfg_sf1;
    UINT8 cap_sf1;
    UINT8 cfg_sf2;
    UINT8 cap_sf2;
    UINT32 cfg_br;
    UINT32 cap_br;

    st = A2D_ParsM24Info(&cfg, p_cfg, FALSE);
    if (st != A2D_SUCCESS) {
        return st;
    }
    st = A2D_ParsM24Info(&cap, p_cap, TRUE);
    if (st != A2D_SUCCESS) {
        return st;
    }

    if ((cfg.obj_type & cap.obj_type) != cfg.obj_type) {
        return A2D_NS_OBJ_TYPE;
    }
    if (cfg.drc && !(cap.drc & A2D_M24_IE_DRC_SUPPORT)) {
        return A2D_FAIL;
    }

    cfg_sf1 = (UINT8)(cfg.samp_freq1 & A2D_M24_IE_SAMP_FREQ1_MSK);
    cap_sf1 = (UINT8)(cap.samp_freq1 & A2D_M24_IE_SAMP_FREQ1_MSK);
    cfg_sf2 = (UINT8)(cfg.samp_freq2 & A2D_M24_IE_SAMP_FREQ2_MSK);
    cap_sf2 = (UINT8)(cap.samp_freq2 & A2D_M24_IE_SAMP_FREQ2_MSK);

    if (!((cfg_sf1 & cap_sf1) ^ (cfg_sf2 & cap_sf2))) {
        return A2D_NS_SAMP_FREQ;
    }
    if ((cfg.ch & cap.ch) != cfg.ch) {
        return A2D_NS_CHANNEL;
    }

    if (cfg.vbr && !(cap.vbr & A2D_M24_IE_VBR_SUPPORT)) {
        return A2D_NS_VBR;
    }

    cfg_br = bta_av_m24_br(&cfg);
    cap_br = bta_av_m24_br(&cap);
    if (cfg_br > cap_br) {
        return A2D_NS_BIT_RATE;
    }

    return A2D_SUCCESS;
}

tA2D_STATUS bta_av_m24_pick_pref_from_src_cap(const tA2D_M24_CIE *src_cap, tA2D_M24_CIE *p_pref)
{
    UINT8 i;
    UINT32 src_br_max;
    UINT32 p_pref_br;
    static const UINT8 sf2_order[] = {
        A2D_M24_IE_SAMP_FREQ2_96,
        A2D_M24_IE_SAMP_FREQ2_88,
        A2D_M24_IE_SAMP_FREQ2_64,
        A2D_M24_IE_SAMP_FREQ2_48,
    };
    static const UINT8 sf1_order[] = {
        A2D_M24_IE_SAMP_FREQ1_44,
        A2D_M24_IE_SAMP_FREQ1_32,
        A2D_M24_IE_SAMP_FREQ1_24,
        A2D_M24_IE_SAMP_FREQ1_22,
        A2D_M24_IE_SAMP_FREQ1_16,
        A2D_M24_IE_SAMP_FREQ1_12,
        A2D_M24_IE_SAMP_FREQ1_11,
        A2D_M24_IE_SAMP_FREQ1_8,
    };

    if (!((p_pref->obj_type & src_cap->obj_type) & A2D_M24_IE_OBJ_TYPE_MSK)) {
        /* Object type: prefer MPEG-4 AAC LC, then MPEG-2 AAC LC */
        if (src_cap->obj_type & A2D_M24_IE_OBJ_TYPE_4_AAC_LC) {
            p_pref->obj_type = A2D_M24_IE_OBJ_TYPE_4_AAC_LC;
        } else if (src_cap->obj_type & A2D_M24_IE_OBJ_TYPE_2_AAC_LC) {
            p_pref->obj_type = A2D_M24_IE_OBJ_TYPE_2_AAC_LC;
        } else {
            UINT8 o = (UINT8)(src_cap->obj_type & A2D_M24_IE_OBJ_TYPE_MSK);
            if (o == 0) {
                return A2D_FAIL;
            }
            p_pref->obj_type = (UINT8)(o & (UINT8)(-o));
        }
    }

    if ((p_pref->drc & A2D_M24_IE_DRC_MSK) && !(src_cap->drc & A2D_M24_IE_DRC_MSK)) {
        p_pref->drc = A2D_M24_IE_DRC_NS;
    }

    if ((p_pref->obj_type & A2D_M24_IE_OBJ_TYPE_MSK) == A2D_M24_IE_OBJ_TYPE_2_AAC_LC) {
        p_pref->drc = A2D_M24_IE_DRC_NS;
    }

    if (!((p_pref->samp_freq1 & src_cap->samp_freq1) & A2D_M24_IE_SAMP_FREQ1_MSK) &&
        !((p_pref->samp_freq2 & src_cap->samp_freq2) & A2D_M24_IE_SAMP_FREQ2_MSK)) {
        /* One frequency: prefer 96..48 kHz, else 44.1..8 kHz (byte 1) */
        p_pref->samp_freq1 = 0;
        p_pref->samp_freq2 = 0;
        for (i = 0; i < sizeof(sf2_order) / sizeof(sf2_order[0]); i++) {
            if (src_cap->samp_freq2 & sf2_order[i]) {
                p_pref->samp_freq2 = (UINT8)(sf2_order[i] & A2D_M24_IE_SAMP_FREQ2_MSK);
                p_pref->samp_freq1 = A2D_M24_IE_SAMP_FREQ1_NS;
                goto got_sf;
            }
        }
        for (i = 0; i < sizeof(sf1_order) / sizeof(sf1_order[0]); i++) {
            if (src_cap->samp_freq1 & sf1_order[i]) {
                p_pref->samp_freq1 = sf1_order[i];
                p_pref->samp_freq2 = A2D_M24_IE_SAMP_FREQ2_NS;
                goto got_sf;
            }
        }
        return A2D_FAIL;
    }

got_sf:
    if (!((p_pref->ch & src_cap->ch) & A2D_M24_IE_CH_MSK)) {
        /* Channels: prefer stereo */
        if (src_cap->ch & A2D_M24_IE_CH_2) {
            p_pref->ch = A2D_M24_IE_CH_2;
        } else if (src_cap->ch & A2D_M24_IE_CH_1) {
            p_pref->ch = A2D_M24_IE_CH_1;
        } else if (src_cap->ch & A2D_M24_IE_CH_6) {
            p_pref->ch = A2D_M24_IE_CH_6;
        } else if (src_cap->ch & A2D_M24_IE_CH_8) {
            p_pref->ch = A2D_M24_IE_CH_8;
        } else {
            return A2D_FAIL;
        }
    }

    if ((p_pref->vbr & A2D_M24_IE_VBR_MSK) && !(src_cap->vbr & A2D_M24_IE_VBR_MSK)) {
        p_pref->vbr = A2D_M24_IE_VBR_NS;
    }

    src_br_max = bta_av_m24_br(src_cap);
    p_pref_br  = bta_av_m24_br(p_pref);
    if (p_pref_br > src_br_max) {
        p_pref_br = src_br_max;
    }

    bta_av_m24_set_br(p_pref, p_pref_br, (UINT8)(p_pref->vbr & A2D_M24_IE_VBR_MSK));

    return A2D_SUCCESS;
}

#endif /* (BTA_AV_INCLUDED == TRUE) && (BTA_AV_CODEC_AAC_INCLUDED == TRUE) */
