/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************
 *
 *  Utility functions for MPEG-2, 4 AAC (M24) codec configuration (A2DP).
 *
 ******************************************************************************/
#ifndef BTA_AV_M24_H
#define BTA_AV_M24_H

#include "common/bt_target.h"
#include "stack/a2d_m24.h"

#if (BTA_AV_INCLUDED == TRUE) && (BTA_AV_CODEC_AAC_INCLUDED == TRUE)

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
**
** Function         bta_av_m24_br_info
**
** Description      Calculate the bit rate from codec info.
**
** Returns          Bit rate value.
**
*******************************************************************************/
extern UINT32 bta_av_m24_br_info(const UINT8 *codec_info);

/*******************************************************************************
**
** Function         bta_av_m24_set_br_info
**
** Description      set the bit rate for codec info.
**
*******************************************************************************/
extern void bta_av_m24_set_br_info(UINT8 *codec_info, UINT32 bit_rate);

/*******************************************************************************
**
** Function         bta_av_m24_cap_matches_cap
**
** Description      Compare two proposed M24 codec capability sets (full Media
**                  Codec Element sequences) to determine if they are compatible,
**                  i.e., whether all features of cap2 are supported by cap1.
**
** Returns          A2D_SUCCESS if ok, nonzero A2D status otherwise.
**
*******************************************************************************/
extern UINT8 bta_av_m24_cap_matches_cap(UINT8 *p_cap1, UINT8 *p_cap2);

/*******************************************************************************
**
** Function         bta_av_m24_cfg_in_external_codec_cap
**
** Description      Check whether p_cfg is allowable for external codec
**                  capability buffer p_cap (both full LOSC.. sequences).
**
** Returns          A2D_SUCCESS if ok, nonzero A2D status otherwise.
**
*******************************************************************************/
extern UINT8 bta_av_m24_cfg_in_external_codec_cap(UINT8 *p_cfg, UINT8 *p_cap);

/*******************************************************************************
**
** Function         bta_av_m24_pick_pref_from_src_cap
**
** Description      Build a single-configuration tA2D_M24_CIE from peer SRC
**                  capability bitmask (for Sink initiator GetConfiguration).
**
** Returns          A2D_SUCCESS or error.
**
*******************************************************************************/
extern tA2D_STATUS bta_av_m24_pick_pref_from_src_cap(const tA2D_M24_CIE *src_cap, tA2D_M24_CIE *p_pref);

#ifdef __cplusplus
}
#endif

#endif /* (BTA_AV_INCLUDED == TRUE) && (BTA_AV_CODEC_AAC_INCLUDED == TRUE) */

#endif /* BTA_AV_M24_H */
