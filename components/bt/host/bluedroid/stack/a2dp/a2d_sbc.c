/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Utility functions to help build and parse SBC Codec Information Element
 *  and Media Payload.
 *
 ******************************************************************************/

#include "common/bt_target.h"

#include <string.h>
#include "stack/a2d_api.h"
#include "a2d_int.h"
#include "stack/a2d_sbc.h"
#include "common/bt_defs.h"

#if (defined(A2D_INCLUDED) && A2D_INCLUDED == TRUE)

/******************************************************************************
**
** Function         A2D_BldSbcInfo
**
** Description      This function is called by an application to build
**                  the SBC Media Codec Capabilities byte sequence
**                  beginning from the LOSC octet.
**                  Input Parameters:
**                      media_type:  Indicates Audio, or Multimedia.
**
**                      p_ie:  The SBC Codec Information Element information.
**
**                  Output Parameters:
**                      p_result:  the resulting codec info byte sequence.
**
** Returns          A2D_SUCCESS if function execution succeeded.
**                  Error status code, otherwise.
******************************************************************************/
tA2D_STATUS A2D_BldSbcInfo(UINT8 media_type, tA2D_SBC_CIE *p_ie, UINT8 *p_result)
{
    tA2D_STATUS status;

    if ( p_ie == NULL || p_result == NULL ||
            (p_ie->samp_freq & ~A2D_SBC_IE_SAMP_FREQ_MSK) ||
            (p_ie->ch_mode & ~A2D_SBC_IE_CH_MD_MSK) ||
            (p_ie->block_len & ~A2D_SBC_IE_BLOCKS_MSK) ||
            (p_ie->num_subbands & ~A2D_SBC_IE_SUBBAND_MSK) ||
            (p_ie->alloc_mthd & ~A2D_SBC_IE_ALLOC_MD_MSK) ||
            (p_ie->max_bitpool < p_ie->min_bitpool) ||
            (p_ie->max_bitpool < A2D_SBC_IE_MIN_BITPOOL) ||
            (p_ie->max_bitpool > A2D_SBC_IE_MAX_BITPOOL) ||
            (p_ie->min_bitpool < A2D_SBC_IE_MIN_BITPOOL) ||
            (p_ie->min_bitpool > A2D_SBC_IE_MAX_BITPOOL) ) {
        /* if any unused bit is set */
        status = A2D_INVALID_PARAMS;
    } else {
        status = A2D_SUCCESS;
        *p_result++ = A2D_SBC_INFO_LEN;
        *p_result++ = media_type;
        *p_result++ = A2D_MEDIA_CT_SBC;

        /* Media Codec Specific Information Element */
        *p_result++ = p_ie->samp_freq | p_ie->ch_mode;

        *p_result++ = p_ie->block_len | p_ie->num_subbands | p_ie->alloc_mthd;

        *p_result++ = p_ie->min_bitpool;
        *p_result   = p_ie->max_bitpool;
    }
    return status;
}

/******************************************************************************
**
** Function         A2D_ParsSbcInfo
**
** Description      This function is called by an application to parse
**                  the SBC Media Codec Capabilities byte sequence
**                  beginning from the LOSC octet.
**                  Input Parameters:
**                      p_info:  the byte sequence to parse.
**
**                      for_caps:  TRUE, if the byte sequence is for get capabilities response.
**
**                  Output Parameters:
**                      p_ie:  The SBC Codec Information Element information.
**
** Returns          A2D_SUCCESS if function execution succeeded.
**                  Error status code, otherwise.
******************************************************************************/
tA2D_STATUS A2D_ParsSbcInfo(tA2D_SBC_CIE *p_ie, UINT8 *p_info, BOOLEAN for_caps)
{
    tA2D_STATUS status;
    UINT8   losc;

    if ( p_ie == NULL || p_info == NULL) {
        status = A2D_INVALID_PARAMS;
    } else {
        losc    = *p_info++;
        p_info++;
        /* If the function is called for the wrong Media Type or Media Codec Type */
        if (losc != A2D_SBC_INFO_LEN || *p_info != A2D_MEDIA_CT_SBC) {
            status = A2D_WRONG_CODEC;
        } else {
            p_info++;
            p_ie->samp_freq = *p_info & A2D_SBC_IE_SAMP_FREQ_MSK;
            p_ie->ch_mode   = *p_info & A2D_SBC_IE_CH_MD_MSK;
            p_info++;
            p_ie->block_len     = *p_info & A2D_SBC_IE_BLOCKS_MSK;
            p_ie->num_subbands  = *p_info & A2D_SBC_IE_SUBBAND_MSK;
            p_ie->alloc_mthd    = *p_info & A2D_SBC_IE_ALLOC_MD_MSK;
            p_info++;
            p_ie->min_bitpool = *p_info++;
            p_ie->max_bitpool = *p_info;
            status = A2D_SUCCESS;
            if (p_ie->min_bitpool < A2D_SBC_IE_MIN_BITPOOL || p_ie->min_bitpool > A2D_SBC_IE_MAX_BITPOOL ) {
                status = A2D_BAD_MIN_BITPOOL;
            }

            if (p_ie->max_bitpool < A2D_SBC_IE_MIN_BITPOOL || p_ie->max_bitpool > A2D_SBC_IE_MAX_BITPOOL ||
                    p_ie->max_bitpool < p_ie->min_bitpool) {
                status = A2D_BAD_MAX_BITPOOL;
            }

            if (for_caps == FALSE) {
                if (A2D_BitsSet(p_ie->samp_freq) != A2D_SET_ONE_BIT) {
                    status = A2D_BAD_SAMP_FREQ;
                }
                if (A2D_BitsSet(p_ie->ch_mode) != A2D_SET_ONE_BIT) {
                    status = A2D_BAD_CH_MODE;
                }
                if (A2D_BitsSet(p_ie->block_len) != A2D_SET_ONE_BIT) {
                    status = A2D_BAD_BLOCK_LEN;
                }
                if (A2D_BitsSet(p_ie->num_subbands) != A2D_SET_ONE_BIT) {
                    status = A2D_BAD_SUBBANDS;
                }
                if (A2D_BitsSet(p_ie->alloc_mthd) != A2D_SET_ONE_BIT) {
                    status = A2D_BAD_ALLOC_MTHD;
                }
            }
        }
    }
    return status;
}

/******************************************************************************
**
** Function         A2D_BldSbcMplHdr
**
** Description      This function is called by an application to parse
**                  the SBC Media Payload header.
**                  Input Parameters:
**                      frag:  1, if fragmented. 0, otherwise.
**
**                      start:  1, if the starting packet of a fragmented frame.
**
**                      last:  1, if the last packet of a fragmented frame.
**
**                      num:  If frag is 1, this is the number of remaining fragments
**                            (including this fragment) of this frame.
**                            If frag is 0, this is the number of frames in this packet.
**
**                  Output Parameters:
**                      p_dst:  the resulting media payload header byte sequence.
**
** Returns          void.
******************************************************************************/
void A2D_BldSbcMplHdr(UINT8 *p_dst, BOOLEAN frag, BOOLEAN start, BOOLEAN last, UINT8 num)
{
    if (p_dst) {
        *p_dst  = 0;
        if (frag) {
            *p_dst  |= A2D_SBC_HDR_F_MSK;
        }
        if (start) {
            *p_dst  |= A2D_SBC_HDR_S_MSK;
        }
        if (last) {
            *p_dst  |= A2D_SBC_HDR_L_MSK;
        }
        *p_dst  |= (A2D_SBC_HDR_NUM_MSK & num);
    }
}

/******************************************************************************
**
** Function         A2D_ParsSbcMplHdr
**
** Description      This function is called by an application to parse
**                  the SBC Media Payload header.
**                  Input Parameters:
**                      p_src:  the byte sequence to parse..
**
**                  Output Parameters:
**                      frag:  1, if fragmented. 0, otherwise.
**
**                      start:  1, if the starting packet of a fragmented frame.
**
**                      last:  1, if the last packet of a fragmented frame.
**
**                      num:  If frag is 1, this is the number of remaining fragments
**                            (including this fragment) of this frame.
**                            If frag is 0, this is the number of frames in this packet.
**
** Returns          void.
******************************************************************************/
void A2D_ParsSbcMplHdr(UINT8 *p_src, BOOLEAN *p_frag, BOOLEAN *p_start, BOOLEAN *p_last, UINT8 *p_num)
{
    if (p_src && p_frag && p_start && p_last && p_num) {
        *p_frag = (*p_src & A2D_SBC_HDR_F_MSK) ? TRUE : FALSE;
        *p_start = (*p_src & A2D_SBC_HDR_S_MSK) ? TRUE : FALSE;
        *p_last = (*p_src & A2D_SBC_HDR_L_MSK) ? TRUE : FALSE;
        *p_num  = (*p_src & A2D_SBC_HDR_NUM_MSK);
    }
}

#endif /* #if (defined(A2D_INCLUDED) && A2D_INCLUDED == TRUE) */
