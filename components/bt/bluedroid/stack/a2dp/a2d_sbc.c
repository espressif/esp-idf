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

#include "bt_target.h"

#include <string.h>
#include "a2d_api.h"
#include "a2d_int.h"
#include "a2d_sbc.h"
#include "bt_defs.h"

#if (defined(A2D_INCLUDED) && A2D_INCLUDED == TRUE)

/*************************************************************************************************
 * SBC descramble code
 * Purpose: to tie the SBC code with BTE/mobile stack code,
 *          especially for the case when the SBC is ported into a third-party Multimedia chip
 *
 * Algorithm:
 *  init process: all counters reset to 0,
 *                calculate base_index: (6 + s16NumOfChannels*s16NumOfSubBands/2)
 *    scramble side:    the init process happens every time SBC_Encoder_Init() is called.
 *    descramble side:  it would be nice to know if he "init" process has happened.
 *                      alter the SBC SYNC word 0x9C (1001 1100) to 0x8C (1000 1100).
 *
 *  scramble process:
 *    The CRC byte:
 *    Every SBC frame has a frame header.
 *    The 1st byte is the sync word and the following 2 bytes are about the stream format.
 *    They are supposed to be "constant" within a "song"
 *    The 4th byte is the CRC byte. The CRC byte is bound to be random.
 *    Derive 2 items from the CRC byte; one is the "use" bit, the other is the "index".
 *
 *    SBC keeps 2 sets of "use" & "index"; derived the current and the previous frame.
 *
 *    The "use" bit is any bit in SBC_PRTC_USE_MASK is set.
 *    If set, SBC uses the "index" from the current frame.
 *    If not set, SBC uses the "index" from the previous frame or 0.
 *
 *    index = (CRC & 0x3) + ((CRC & 0x30) >> 2) // 8 is the max index
 *
 *    if(index > 0)
 *    {
 *        p = &u8frame[base_index];
 *        if((index&1)&&(u16PacketLength > (base_index+index*2)))
 *        {
 *            // odd index: swap 2 bytes
 *            tmp = p[index];
 *            p[index] = p[index*2];
 *            p[index*2] = tmp;
 *        }
 *        else
 *        {
 *            // even index: shift by 3
 *            tmp = (p[index] >> 3) + (p[index] << 5);
 *            p[index] = tmp;
 *        }
 *    }
 *    //else index is 0. The frame stays unaltered
 *
 */
#define A2D_SBC_SYNC_WORD       0x9C
#define A2D_SBC_CRC_IDX         3
#define A2D_SBC_USE_MASK        0x64
#define A2D_SBC_SYNC_MASK       0x10
#define A2D_SBC_CIDX            0
#define A2D_SBC_LIDX            1
#define A2D_SBC_CH_M_BITS       0xC /* channel mode bits: 0: mono; 1 ch */
#define A2D_SBC_SUBBAND_BIT     0x1 /* num of subband bit: 0:4; 1: 8 */

#define A2D_SBC_GET_IDX(sc) (((sc) & 0x3) + (((sc) & 0x30) >> 2))

typedef struct {
    UINT8   use;
    UINT8   idx;
} tA2D_SBC_FR_CB;

typedef struct {
    tA2D_SBC_FR_CB  fr[2];
    UINT8           index;
    UINT8           base;
} tA2D_SBC_DS_CB;

static tA2D_SBC_DS_CB a2d_sbc_ds_cb;
/*int a2d_count = 0;*/
/******************************************************************************
**
** Function         A2D_SbcChkFrInit
**
** Description      check if need to init the descramble control block.
**
** Returns          nothing.
******************************************************************************/
void A2D_SbcChkFrInit(UINT8 *p_pkt)
{
    UINT8   fmt;
    UINT8   num_chnl = 1;
    UINT8   num_subband = 4;

    if ((p_pkt[0] & A2D_SBC_SYNC_MASK) == 0) {
        a2d_cb.use_desc = TRUE;
        fmt = p_pkt[1];
        p_pkt[0] |= A2D_SBC_SYNC_MASK;
        memset(&a2d_sbc_ds_cb, 0, sizeof(tA2D_SBC_DS_CB));
        if (fmt & A2D_SBC_CH_M_BITS) {
            num_chnl = 2;
        }
        if (fmt & A2D_SBC_SUBBAND_BIT) {
            num_subband = 8;
        }
        a2d_sbc_ds_cb.base = 6 + num_chnl * num_subband / 2;
        /*printf("base: %d\n", a2d_sbc_ds_cb.base);
        a2d_count = 0;*/
    }
}

/******************************************************************************
**
** Function         A2D_SbcDescramble
**
** Description      descramble the packet.
**
** Returns          nothing.
******************************************************************************/
void A2D_SbcDescramble(UINT8 *p_pkt, UINT16 len)
{
    tA2D_SBC_FR_CB *p_cur, *p_last;
    UINT32   idx, tmp, tmp2;

    if (a2d_cb.use_desc) {
        /* c2l */
        p_last  = &a2d_sbc_ds_cb.fr[A2D_SBC_LIDX];
        p_cur   = &a2d_sbc_ds_cb.fr[A2D_SBC_CIDX];
        p_last->idx = p_cur->idx;
        p_last->use = p_cur->use;
        /* getc */
        p_cur->use = p_pkt[A2D_SBC_CRC_IDX] & A2D_SBC_USE_MASK;
        p_cur->idx = A2D_SBC_GET_IDX(p_pkt[A2D_SBC_CRC_IDX]);
        a2d_sbc_ds_cb.index = (p_cur->use) ? A2D_SBC_CIDX : A2D_SBC_LIDX;
        /*
        printf("%05d: ar[%02d]: x%02x, msk: x%02x, use: %s, idx: %02d, ",
            a2d_count++,
            A2D_SBC_CRC_IDX, p_pkt[A2D_SBC_CRC_IDX], A2D_SBC_USE_MASK,
            (p_cur->use)?"cur":"lst", p_cur->idx);
        */
        /* descramble */
        idx = a2d_sbc_ds_cb.fr[a2d_sbc_ds_cb.index].idx;
        if (idx > 0) {
            p_pkt = &p_pkt[a2d_sbc_ds_cb.base];
            if ((idx & 1) && (len > (a2d_sbc_ds_cb.base + (idx << 1)))) {
                tmp2        = (idx << 1);
                tmp         = p_pkt[idx];
                p_pkt[idx]  = p_pkt[tmp2];
                p_pkt[tmp2]  = tmp;
                /*
                printf("tmp2: %02d, len: %d, idx: %d\n",
                    tmp2, len, a2d_sbc_ds_cb.fr[a2d_sbc_ds_cb.index].idx);
                    */
            } else {
                tmp2        = p_pkt[idx];
                tmp         = (tmp2 >> 3) + (tmp2 << 5);
                p_pkt[idx]  = (UINT8)tmp;
                /*
                printf("tmp: x%02x, len: %d, idx: %d(cmp:%d)\n",
                    (UINT8)tmp2, len, a2d_sbc_ds_cb.fr[a2d_sbc_ds_cb.index].idx,
                    (a2d_sbc_ds_cb.base+(idx<<1)));
                    */
            }
        }
        /*
        else
        {
            printf("!!!!\n");
        }
        */
    }
}

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
