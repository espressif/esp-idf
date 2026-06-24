/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************
 *
 *  Utility functions to help build and parse MPEG-2, 4 AAC (M24) Codec
 *  Information Element per Bluetooth A2DP specification (Media Codec Capabilities).
 *
 *  Layout of Media Codec Capability (starting at LOSC octet):
 *    Octet 0: LOSC = 8 (number of octets following LOSC: media type + codec type + 6 CIE)
 *    Octet 1: Media type (Audio = 0x00)
 *    Octet 2: Media codec type (MPEG-2,4 AAC = 0x02)
 *    Octets 3-8: Codec Specific Information Element (6 octets)
 *
 ******************************************************************************/

#include <string.h>
#include "common/bt_defs.h"
#include "common/bt_target.h"
#include "stack/a2d_api.h"
#include "stack/a2d_m24.h"
#include "a2d_int.h"

#if (defined(A2D_AV_CODEC_AAC_INCLUDED) && A2D_AV_CODEC_AAC_INCLUDED == TRUE)

/******************************************************************************
** Local helpers
******************************************************************************/

/* Count how many bits are set in the lower nibble (used for 4-bit freq2 field). */
static UINT8 m24_count_bits_4(UINT8 v)
{
    UINT8 c = 0;
    UINT8 i;

    v &= 0x0F;
    for (i = 0; i < 4; i++) {
        if (v & (1U << i)) {
            c++;
        }
    }
    return c;
}

/* Total number of set bits in one byte. */
static UINT8 m24_popcount8(UINT8 v)
{
    UINT8 c = 0;
    UINT8 i;

    for (i = 0; i < 8; i++) {
        if (v & (1U << i)) {
            c++;
        }
    }
    return c;
}

static tA2D_STATUS m24_validate_ie_for_build(const tA2D_M24_CIE *p_ie)
{
    if (p_ie == NULL) {
        return A2D_INVALID_PARAMS;
    }

    /* Object type uses b7-b1; b0 is carried in drc */
    if ((p_ie->obj_type & ~A2D_M24_IE_OBJ_TYPE_MSK) != 0) {
        return A2D_INVALID_PARAMS;
    }
    if ((p_ie->drc & ~A2D_M24_IE_DRC_MSK) != 0) {
        return A2D_INVALID_PARAMS;
    }
    if ((p_ie->samp_freq1 & ~A2D_M24_IE_SAMP_FREQ1_MSK) != 0) {
        return A2D_INVALID_PARAMS;
    }
    if ((p_ie->samp_freq2 & ~A2D_M24_IE_SAMP_FREQ2_MSK) != 0) {
        return A2D_INVALID_PARAMS;
    }
    if ((p_ie->ch & ~A2D_M24_IE_CH_MSK) != 0) {
        return A2D_INVALID_PARAMS;
    }
    if ((p_ie->vbr & ~A2D_M24_IE_VBR_MSK) != 0) {
        return A2D_INVALID_PARAMS;
    }
    if ((p_ie->br1 & ~A2D_M24_IE_BR1_MSK) != 0) {
        return A2D_INVALID_PARAMS;
    }
    if ((p_ie->br2 & ~A2D_M24_IE_BR2_MSK) != 0) {
        return A2D_INVALID_PARAMS;
    }
    if ((p_ie->br3 & ~A2D_M24_IE_BR3_MSK) != 0) {
        return A2D_INVALID_PARAMS;
    }

    /* Capabilities must select at least one object type (bits b7-b1). */
    if ((p_ie->obj_type & A2D_M24_IE_OBJ_TYPE_MSK) == 0) {
        return A2D_INVALID_PARAMS;
    }
    /* At least one sampling frequency (12 bits across two octets) and one channel mode. */
    if (m24_popcount8(p_ie->samp_freq1) + m24_count_bits_4(((p_ie->samp_freq2 & A2D_M24_IE_SAMP_FREQ2_MSK) >> 4)) < 1) {
        return A2D_INVALID_PARAMS;
    }
    if (m24_count_bits_4(p_ie->ch & A2D_M24_IE_CH_MSK) < 1) {
        return A2D_INVALID_PARAMS;
    }

    return A2D_SUCCESS;
}

/******************************************************************************
**
** Function         A2D_BldM24Info
**
** Description      This function is called by an application to build
**                  the M24 Media Codec Capabilities byte sequence
**                  beginning from the LOSC octet.
**                  Input Parameters:
**                      media_type:  Indicates Audio, or Multimedia.
**
**                      p_ie:  The M24 Codec Information Element information.
**
**                  Output Parameters:
**                      p_result:  the resulting codec info byte sequence.
**
** Returns          A2D_SUCCESS if function execution succeeded.
**                  Error status code, otherwise.
******************************************************************************/
tA2D_STATUS A2D_BldM24Info(UINT8 media_type, tA2D_M24_CIE *p_ie, UINT8 *p_result)
{
    tA2D_STATUS status;

    if (p_ie == NULL || p_result == NULL) {
        return A2D_INVALID_PARAMS;
    }

    status = m24_validate_ie_for_build(p_ie);
    if (status != A2D_SUCCESS) {
        return status;
    }

    *p_result++ = A2D_M24_INFO_LEN;
    *p_result++ = media_type;
    *p_result++ = A2D_MEDIA_CT_M24;

    /* Codec Specific Information Element (6 octets), A2DP spec */
    *p_result++ = (p_ie->obj_type & A2D_M24_IE_OBJ_TYPE_MSK) | (p_ie->drc & A2D_M24_IE_DRC_MSK);
    *p_result++ = p_ie->samp_freq1 & A2D_M24_IE_SAMP_FREQ1_MSK;
    *p_result++ = (p_ie->samp_freq2 & A2D_M24_IE_SAMP_FREQ2_MSK) | (p_ie->ch & A2D_M24_IE_CH_MSK);
    *p_result++ = (p_ie->vbr & A2D_M24_IE_VBR_MSK) | (p_ie->br1 & A2D_M24_IE_BR1_MSK);
    *p_result++ = p_ie->br2 & A2D_M24_IE_BR2_MSK;
    *p_result   = p_ie->br3 & A2D_M24_IE_BR3_MSK;

    return A2D_SUCCESS;
}

/******************************************************************************
**
** Function         A2D_ParsM24Info
**
** Description      This function is called by an application to parse
**                  the M24 Media Codec Capabilities byte sequence
**                  beginning from the LOSC octet.
**                  Input Parameters:
**                      p_info:  the byte sequence to parse.
**
**                      for_caps:  TRUE, if the byte sequence is for get capabilities response.
**
**                  Output Parameters:
**                      p_ie:  The M24 Codec Information Element information.
**
** Returns          A2D_SUCCESS if function execution succeeded.
**                  Error status code, otherwise.
******************************************************************************/
tA2D_STATUS A2D_ParsM24Info(tA2D_M24_CIE *p_ie, UINT8 *p_info, BOOLEAN for_caps)
{
    tA2D_STATUS status = A2D_SUCCESS;
    UINT8 losc;
    UINT8 media_type;
    UINT8 cie[6];
    UINT8 i;
    UINT8 sf2_hi;
    UINT8 freq_bits_total;
    UINT8 ch_bits;

    if (p_ie == NULL || p_info == NULL) {
        return A2D_INVALID_PARAMS;
    }

    memset(p_ie, 0, sizeof(tA2D_M24_CIE));

    losc = *p_info++;
    media_type = *p_info++;

    if (losc != A2D_M24_INFO_LEN || media_type != A2D_MEDIA_TYPE_AUDIO || *p_info != A2D_MEDIA_CT_M24) {
        return A2D_WRONG_CODEC;
    }

    p_info++; /* skip media codec type */

    for (i = 0; i < A2D_M24_CIE_LEN; i++) {
        cie[i] = *p_info++;
    }

    /* Unpack CIE */
    p_ie->drc = cie[0] & A2D_M24_IE_DRC_MSK;
    p_ie->obj_type = cie[0] & A2D_M24_IE_OBJ_TYPE_MSK;

    p_ie->samp_freq1 = cie[1] & A2D_M24_IE_SAMP_FREQ1_MSK;

    p_ie->samp_freq2 = cie[2] & A2D_M24_IE_SAMP_FREQ2_MSK;
    p_ie->ch = cie[2] & A2D_M24_IE_CH_MSK;

    p_ie->vbr = cie[3] & A2D_M24_IE_VBR_MSK;
    p_ie->br1 = cie[3] & A2D_M24_IE_BR1_MSK;

    p_ie->br2 = cie[4] & A2D_M24_IE_BR2_MSK;
    p_ie->br3 = cie[5] & A2D_M24_IE_BR3_MSK;

    if (for_caps) {
        /* Get capabilities: need at least one object type and one frequency / channel bit. */
        if ((p_ie->obj_type & A2D_M24_IE_OBJ_TYPE_MSK) == 0) {
            status = A2D_BAD_OBJ_TYPE;
        } else {
            sf2_hi = p_ie->samp_freq2 >> 4;
            freq_bits_total = m24_popcount8(p_ie->samp_freq1) + m24_count_bits_4(sf2_hi);
            ch_bits = m24_count_bits_4(p_ie->ch);
            if (freq_bits_total < 1) {
                status = A2D_NS_SAMP_FREQ;
            } else if (ch_bits < 1) {
                status = A2D_NS_CHANNEL;
            }
        }
    } else {
        /* Stream configuration: single choice for object type, frequency, channels. */
        if (A2D_BitsSet(p_ie->obj_type) != A2D_SET_ONE_BIT) {
            status = A2D_BAD_OBJ_TYPE;
        } else {
            sf2_hi = p_ie->samp_freq2 >> 4;
            freq_bits_total = m24_popcount8(p_ie->samp_freq1) + m24_count_bits_4(sf2_hi);
            if (freq_bits_total != 1) {
                status = A2D_BAD_SAMP_FREQ;
            } else if (A2D_BitsSet(p_ie->ch) != A2D_SET_ONE_BIT) {
                status = A2D_BAD_CHANNEL;
            }
        }
    }

    return status;
}

#endif /* #if (defined(A2D_AV_CODEC_AAC_INCLUDED) && A2D_AV_CODEC_AAC_INCLUDED == TRUE) */
