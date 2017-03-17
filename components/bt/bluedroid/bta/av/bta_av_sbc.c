/******************************************************************************
 *
 *  Copyright (C) 2004-2012 Broadcom Corporation
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
 *  This module contains utility functions for dealing with SBC data frames
 *  and codec capabilities.
 *
 ******************************************************************************/

#include "bt_target.h"
#include "a2d_api.h"
#include "a2d_sbc.h"
#include "bta_av_sbc.h"
#include "utl.h"
#include "bt_utils.h"

#if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)

typedef int (tBTA_AV_SBC_ACT)(void *p_src, void *p_dst,
                              UINT32 src_samples, UINT32 dst_samples,
                              UINT32 *p_ret);

typedef struct {
    INT32               cur_pos;    /* current position */
    UINT32              src_sps;    /* samples per second (source audio data) */
    UINT32              dst_sps;    /* samples per second (converted audio data) */
    tBTA_AV_SBC_ACT     *p_act;     /* the action function to do the conversion */
    UINT16              bits;       /* number of bits per pcm sample */
    UINT16              n_channels; /* number of channels (i.e. mono(1), stereo(2)...) */
    INT16               worker1;
    INT16               worker2;
    UINT8               div;
} tBTA_AV_SBC_UPS_CB;

tBTA_AV_SBC_UPS_CB bta_av_sbc_ups_cb;

/*******************************************************************************
**
** Function         bta_av_sbc_init_up_sample
**
** Description      initialize the up sample
**
**                  src_sps: samples per second (source audio data)
**                  dst_sps: samples per second (converted audio data)
**                  bits: number of bits per pcm sample
**                  n_channels: number of channels (i.e. mono(1), stereo(2)...)
**
** Returns          none
**
*******************************************************************************/
void bta_av_sbc_init_up_sample (UINT32 src_sps, UINT32 dst_sps, UINT16 bits, UINT16 n_channels)
{
    bta_av_sbc_ups_cb.cur_pos   = -1;
    bta_av_sbc_ups_cb.src_sps   = src_sps;
    bta_av_sbc_ups_cb.dst_sps   = dst_sps;
    bta_av_sbc_ups_cb.bits      = bits;
    bta_av_sbc_ups_cb.n_channels = n_channels;

    if (n_channels == 1) {
        /* mono */
        if (bits == 8) {
            bta_av_sbc_ups_cb.p_act = bta_av_sbc_up_sample_8m;
            bta_av_sbc_ups_cb.div   = 1;
        } else {
            bta_av_sbc_ups_cb.p_act = bta_av_sbc_up_sample_16m;
            bta_av_sbc_ups_cb.div   = 2;
        }
    } else {
        /* stereo */
        if (bits == 8) {
            bta_av_sbc_ups_cb.p_act = bta_av_sbc_up_sample_8s;
            bta_av_sbc_ups_cb.div   = 2;
        } else {
            bta_av_sbc_ups_cb.p_act = bta_av_sbc_up_sample_16s;
            bta_av_sbc_ups_cb.div   = 4;
        }
    }
}

/*******************************************************************************
**
** Function         bta_av_sbc_up_sample
**
** Description      Given the source (p_src) audio data and
**                  source speed (src_sps, samples per second),
**                  This function converts it to audio data in the desired format
**
**                  p_src: the data buffer that holds the source audio data
**                  p_dst: the data buffer to hold the converted audio data
**                  src_samples: The number of source samples (number of bytes)
**                  dst_samples: The size of p_dst (number of bytes)
**
** Note:            An AE reported an issue with this function.
**                  When called with bta_av_sbc_up_sample(src, uint8_array_dst..)
**                  the byte before uint8_array_dst may get overwritten.
**                  Using uint16_array_dst avoids the problem.
**                  This issue is related to endian-ness and is hard to resolve
**                  in a generic manner.
** **************** Please use uint16 array as dst.
**
** Returns          The number of bytes used in p_dst
**                  The number of bytes used in p_src (in *p_ret)
**
*******************************************************************************/
int bta_av_sbc_up_sample (void *p_src, void *p_dst,
                          UINT32 src_samples, UINT32 dst_samples,
                          UINT32 *p_ret)
{
    UINT32 src;
    UINT32 dst;

    if (bta_av_sbc_ups_cb.p_act) {
        src = src_samples / bta_av_sbc_ups_cb.div;
        dst = dst_samples / bta_av_sbc_ups_cb.div;
        return (*bta_av_sbc_ups_cb.p_act)(p_src, p_dst, src, dst, p_ret);
    } else {
        *p_ret = 0;
        return 0;
    }
}

/*******************************************************************************
**
** Function         bta_av_sbc_up_sample_16s (16bits-stereo)
**
** Description      Given the source (p_src) audio data and
**                  source speed (src_sps, samples per second),
**                  This function converts it to audio data in the desired format
**
**                  p_src: the data buffer that holds the source audio data
**                  p_dst: the data buffer to hold the converted audio data
**                  src_samples: The number of source samples (in uint of 4 bytes)
**                  dst_samples: The size of p_dst (in uint of 4 bytes)
**
** Returns          The number of bytes used in p_dst
**                  The number of bytes used in p_src (in *p_ret)
**
*******************************************************************************/
int bta_av_sbc_up_sample_16s (void *p_src, void *p_dst,
                              UINT32 src_samples, UINT32 dst_samples,
                              UINT32 *p_ret)
{
    INT16   *p_src_tmp = (INT16 *)p_src;
    INT16   *p_dst_tmp = (INT16 *)p_dst;
    INT16   *p_worker1 = &bta_av_sbc_ups_cb.worker1;
    INT16   *p_worker2 = &bta_av_sbc_ups_cb.worker2;
    UINT32  src_sps = bta_av_sbc_ups_cb.src_sps;
    UINT32  dst_sps = bta_av_sbc_ups_cb.dst_sps;

    while (bta_av_sbc_ups_cb.cur_pos > 0 && dst_samples) {
        *p_dst_tmp++    = *p_worker1;
        *p_dst_tmp++    = *p_worker2;

        bta_av_sbc_ups_cb.cur_pos -= src_sps;
        dst_samples--;
    }

    bta_av_sbc_ups_cb.cur_pos = dst_sps;

    while (src_samples-- && dst_samples) {
        *p_worker1 = *p_src_tmp++;
        *p_worker2 = *p_src_tmp++;

        do {
            *p_dst_tmp++    = *p_worker1;
            *p_dst_tmp++    = *p_worker2;

            bta_av_sbc_ups_cb.cur_pos -= src_sps;
            dst_samples--;
        } while (bta_av_sbc_ups_cb.cur_pos > 0 && dst_samples);

        bta_av_sbc_ups_cb.cur_pos += dst_sps;
    }

    if (bta_av_sbc_ups_cb.cur_pos == (INT32)dst_sps) {
        bta_av_sbc_ups_cb.cur_pos = 0;
    }

    *p_ret = ((char *)p_src_tmp - (char *)p_src);
    return ((char *)p_dst_tmp - (char *)p_dst);
}

/*******************************************************************************
**
** Function         bta_av_sbc_up_sample_16m (16bits-mono)
**
** Description      Given the source (p_src) audio data and
**                  source speed (src_sps, samples per second),
**                  This function converts it to audio data in the desired format
**
**                  p_src: the data buffer that holds the source audio data
**                  p_dst: the data buffer to hold the converted audio data
**                  src_samples: The number of source samples (in uint of 2 bytes)
**                  dst_samples: The size of p_dst (in uint of 2 bytes)
**
** Returns          The number of bytes used in p_dst
**                  The number of bytes used in p_src (in *p_ret)
**
*******************************************************************************/
int bta_av_sbc_up_sample_16m (void *p_src, void *p_dst,
                              UINT32 src_samples, UINT32 dst_samples,
                              UINT32 *p_ret)
{
    INT16   *p_src_tmp = (INT16 *)p_src;
    INT16   *p_dst_tmp = (INT16 *)p_dst;
    INT16   *p_worker = &bta_av_sbc_ups_cb.worker1;
    UINT32  src_sps = bta_av_sbc_ups_cb.src_sps;
    UINT32  dst_sps = bta_av_sbc_ups_cb.dst_sps;

    while (bta_av_sbc_ups_cb.cur_pos > 0 && dst_samples) {
        *p_dst_tmp++ = *p_worker;
        *p_dst_tmp++ = *p_worker;

        bta_av_sbc_ups_cb.cur_pos -= src_sps;
        dst_samples--;
        dst_samples--;
    }


    bta_av_sbc_ups_cb.cur_pos = dst_sps;

    while (src_samples-- && dst_samples) {
        *p_worker = *p_src_tmp++;

        do {
            *p_dst_tmp++ = *p_worker;
            *p_dst_tmp++ = *p_worker;

            bta_av_sbc_ups_cb.cur_pos -= src_sps;
            dst_samples--;
            dst_samples--;

        } while (bta_av_sbc_ups_cb.cur_pos > 0 && dst_samples);

        bta_av_sbc_ups_cb.cur_pos += dst_sps;
    }

    if (bta_av_sbc_ups_cb.cur_pos == (INT32)dst_sps) {
        bta_av_sbc_ups_cb.cur_pos = 0;
    }

    *p_ret = ((char *)p_src_tmp - (char *)p_src);
    return ((char *)p_dst_tmp - (char *)p_dst);
}

/*******************************************************************************
**
** Function         bta_av_sbc_up_sample_8s (8bits-stereo)
**
** Description      Given the source (p_src) audio data and
**                  source speed (src_sps, samples per second),
**                  This function converts it to audio data in the desired format
**
**                  p_src: the data buffer that holds the source audio data
**                  p_dst: the data buffer to hold the converted audio data
**                  src_samples: The number of source samples (in uint of 2 bytes)
**                  dst_samples: The size of p_dst (in uint of 2 bytes)
**
** Returns          The number of bytes used in p_dst
**                  The number of bytes used in p_src (in *p_ret)
**
*******************************************************************************/
int bta_av_sbc_up_sample_8s (void *p_src, void *p_dst,
                             UINT32 src_samples, UINT32 dst_samples,
                             UINT32 *p_ret)
{
    UINT8   *p_src_tmp = (UINT8 *)p_src;
    INT16   *p_dst_tmp = (INT16 *)p_dst;
    INT16   *p_worker1 = &bta_av_sbc_ups_cb.worker1;
    INT16   *p_worker2 = &bta_av_sbc_ups_cb.worker2;
    UINT32  src_sps = bta_av_sbc_ups_cb.src_sps;
    UINT32  dst_sps = bta_av_sbc_ups_cb.dst_sps;

    while (bta_av_sbc_ups_cb.cur_pos > 0 && dst_samples) {
        *p_dst_tmp++    = *p_worker1;
        *p_dst_tmp++    = *p_worker2;

        bta_av_sbc_ups_cb.cur_pos -= src_sps;
        dst_samples--;
        dst_samples--;
    }

    bta_av_sbc_ups_cb.cur_pos = dst_sps;

    while (src_samples -- && dst_samples) {
        *p_worker1 = *(UINT8 *)p_src_tmp++;
        *p_worker1 -= 0x80;
        *p_worker1 <<= 8;
        *p_worker2 = *(UINT8 *)p_src_tmp++;
        *p_worker2 -= 0x80;
        *p_worker2 <<= 8;

        do {
            *p_dst_tmp++    = *p_worker1;
            *p_dst_tmp++    = *p_worker2;

            bta_av_sbc_ups_cb.cur_pos -= src_sps;
            dst_samples--;
            dst_samples--;
        } while (bta_av_sbc_ups_cb.cur_pos > 0 && dst_samples);

        bta_av_sbc_ups_cb.cur_pos += dst_sps;
    }

    if (bta_av_sbc_ups_cb.cur_pos == (INT32)dst_sps) {
        bta_av_sbc_ups_cb.cur_pos = 0;
    }

    *p_ret = ((char *)p_src_tmp - (char *)p_src);
    return ((char *)p_dst_tmp - (char *)p_dst);
}

/*******************************************************************************
**
** Function         bta_av_sbc_up_sample_8m (8bits-mono)
**
** Description      Given the source (p_src) audio data and
**                  source speed (src_sps, samples per second),
**                  This function converts it to audio data in the desired format
**
**                  p_src: the data buffer that holds the source audio data
**                  p_dst: the data buffer to hold the converted audio data
**                  src_samples: The number of source samples (number of bytes)
**                  dst_samples: The size of p_dst (number of bytes)
**
** Returns          The number of bytes used in p_dst
**                  The number of bytes used in p_src (in *p_ret)
**
*******************************************************************************/
int bta_av_sbc_up_sample_8m (void *p_src, void *p_dst,
                             UINT32 src_samples, UINT32 dst_samples,
                             UINT32 *p_ret)
{
    UINT8   *p_src_tmp = (UINT8 *)p_src;
    INT16   *p_dst_tmp = (INT16 *)p_dst;
    INT16   *p_worker = &bta_av_sbc_ups_cb.worker1;
    UINT32  src_sps = bta_av_sbc_ups_cb.src_sps;
    UINT32  dst_sps = bta_av_sbc_ups_cb.dst_sps;

    while (bta_av_sbc_ups_cb.cur_pos > 0 && dst_samples) {
        *p_dst_tmp++ = *p_worker;
        *p_dst_tmp++ = *p_worker;

        bta_av_sbc_ups_cb.cur_pos -= src_sps;
        dst_samples -= 4;
    }


    bta_av_sbc_ups_cb.cur_pos = dst_sps;

    while (src_samples-- && dst_samples) {
        *p_worker = *(UINT8 *)p_src_tmp++;
        *p_worker -= 0x80;
        *p_worker <<= 8;

        do {
            *p_dst_tmp++ = *p_worker;
            *p_dst_tmp++ = *p_worker;

            bta_av_sbc_ups_cb.cur_pos -= src_sps;
            dst_samples -= 4;

        } while (bta_av_sbc_ups_cb.cur_pos > 0 && dst_samples);

        bta_av_sbc_ups_cb.cur_pos += dst_sps;
    }

    if (bta_av_sbc_ups_cb.cur_pos == (INT32)dst_sps) {
        bta_av_sbc_ups_cb.cur_pos = 0;
    }

    *p_ret = ((char *)p_src_tmp - (char *)p_src);
    return ((char *)p_dst_tmp - (char *)p_dst);
}

/*******************************************************************************
**
** Function         bta_av_sbc_cfg_for_cap
**
** Description      Determine the preferred SBC codec configuration for the
**                  given codec capabilities.  The function is passed the
**                  preferred codec configuration and the peer codec
**                  capabilities for the stream.  The function attempts to
**                  match the preferred capabilities with the configuration
**                  as best it can.  The resulting codec configuration is
**                  returned in the same memory used for the capabilities.
**
** Returns          0 if ok, nonzero if error.
**                  Codec configuration in p_cap.
**
*******************************************************************************/
UINT8 bta_av_sbc_cfg_for_cap(UINT8 *p_peer, tA2D_SBC_CIE *p_cap, tA2D_SBC_CIE *p_pref)
{
    UINT8           status = A2D_SUCCESS;
    tA2D_SBC_CIE    peer_cie;
    UNUSED(p_cap);

    /* parse peer capabilities */
    if ((status = A2D_ParsSbcInfo(&peer_cie, p_peer, TRUE)) != 0) {
        return status;
    }

    /* Check if the peer supports our channel mode */
    if (peer_cie.ch_mode & p_pref->ch_mode) {
        peer_cie.ch_mode = p_pref->ch_mode;
    } else {
        APPL_TRACE_ERROR("bta_av_sbc_cfg_for_cap: ch_mode(0x%02X) not supported", p_pref->ch_mode);
        return A2D_FAIL;
    }

    /* Check if the peer supports our sampling freq */
    if (peer_cie.samp_freq & p_pref->samp_freq) {
        peer_cie.samp_freq = p_pref->samp_freq;
    } else {
        APPL_TRACE_ERROR("bta_av_sbc_cfg_for_cap: samp_freq(0x%02X) not supported", p_pref->samp_freq);
        return A2D_FAIL;
    }

    /* Check if the peer supports our block len */
    if (peer_cie.block_len & p_pref->block_len) {
        peer_cie.block_len = p_pref->block_len;
    } else {
        APPL_TRACE_ERROR("bta_av_sbc_cfg_for_cap: block_len(0x%02X) not supported", p_pref->block_len);
        return A2D_FAIL;
    }

    /* Check if the peer supports our num subbands */
    if (peer_cie.num_subbands & p_pref->num_subbands) {
        peer_cie.num_subbands = p_pref->num_subbands;
    } else {
        APPL_TRACE_ERROR("bta_av_sbc_cfg_for_cap: num_subbands(0x%02X) not supported", p_pref->num_subbands);
        return A2D_FAIL;
    }

    /* Check if the peer supports our alloc method */
    if (peer_cie.alloc_mthd & p_pref->alloc_mthd) {
        peer_cie.alloc_mthd = p_pref->alloc_mthd;
    } else {
        APPL_TRACE_ERROR("bta_av_sbc_cfg_for_cap: alloc_mthd(0x%02X) not supported", p_pref->alloc_mthd);
        return A2D_FAIL;
    }

    /* max bitpool */
    if (p_pref->max_bitpool != 0 && p_pref->max_bitpool < peer_cie.max_bitpool) {
        peer_cie.max_bitpool = p_pref->max_bitpool;
    }

    /* min bitpool */
    if (p_pref->min_bitpool != 0 && p_pref->min_bitpool > peer_cie.min_bitpool) {
        peer_cie.min_bitpool = p_pref->min_bitpool;
    }

    if (status == A2D_SUCCESS) {
        /* build configuration */
        A2D_BldSbcInfo(A2D_MEDIA_TYPE_AUDIO, &peer_cie, p_peer);
    }
    return status;
}

/*******************************************************************************
**
** Function         bta_av_sbc_cfg_matches_cap
**
** Description      This function checks whether an SBC codec configuration
**                  matched with capabilities. Here we check subset.
**
** Returns          0 if ok, nonzero if error.
**
*******************************************************************************/
UINT8 bta_av_sbc_cfg_matches_cap(UINT8 *p_cfg, tA2D_SBC_CIE *p_cap)
{
    UINT8           status = 0;
    tA2D_SBC_CIE    cfg_cie;

    /* parse configuration */
    if ((status = A2D_ParsSbcInfo(&cfg_cie, p_cfg, TRUE)) != 0) {
        APPL_TRACE_ERROR(" bta_av_sbc_cfg_matches_cap Parsing Failed %d", status);
        return status;
    }

    /* verify that each parameter is in range */

    APPL_TRACE_DEBUG(" FREQ peer: 0%x, capability  0%x", cfg_cie.samp_freq, p_cap->samp_freq);
    APPL_TRACE_DEBUG(" CH_MODE peer: 0%x, capability  0%x", cfg_cie.ch_mode, p_cap->ch_mode);
    APPL_TRACE_DEBUG(" BLOCK_LEN peer: 0%x, capability  0%x", cfg_cie.block_len, p_cap->block_len);
    APPL_TRACE_DEBUG(" SUB_BAND peer: 0%x, capability  0%x", cfg_cie.num_subbands, p_cap->num_subbands);
    APPL_TRACE_DEBUG(" ALLOC_MTHD peer: 0%x, capability  0%x", cfg_cie.alloc_mthd, p_cap->alloc_mthd);
    APPL_TRACE_DEBUG(" MAX_BitPool peer: 0%x, capability  0%x", cfg_cie.max_bitpool, p_cap->max_bitpool);
    APPL_TRACE_DEBUG(" Min_bitpool peer: 0%x, capability  0%x", cfg_cie.min_bitpool, p_cap->min_bitpool);

    /* sampling frequency */
    if ((cfg_cie.samp_freq & p_cap->samp_freq) == 0) {
        status = A2D_NS_SAMP_FREQ;
    }
    /* channel mode */
    else if ((cfg_cie.ch_mode & p_cap->ch_mode) == 0) {
        status = A2D_NS_CH_MODE;
    }
    /* block length */
    else if ((cfg_cie.block_len & p_cap->block_len) == 0) {
        status = A2D_BAD_BLOCK_LEN;
    }
    /* subbands */
    else if ((cfg_cie.num_subbands & p_cap->num_subbands) == 0) {
        status = A2D_NS_SUBBANDS;
    }
    /* allocation method */
    else if ((cfg_cie.alloc_mthd & p_cap->alloc_mthd) == 0) {
        status = A2D_NS_ALLOC_MTHD;
    }
    /* max bitpool */
    else if (cfg_cie.max_bitpool > p_cap->max_bitpool) {
        status = A2D_NS_MAX_BITPOOL;
    }
    /* min bitpool */
    else if (cfg_cie.min_bitpool < p_cap->min_bitpool) {
        status = A2D_NS_MIN_BITPOOL;
    }

    return status;
}


/*******************************************************************************
**
** Function         bta_av_sbc_cfg_in_cap
**
** Description      This function checks whether an SBC codec configuration
**                  is allowable for the given codec capabilities.
**
** Returns          0 if ok, nonzero if error.
**
*******************************************************************************/
UINT8 bta_av_sbc_cfg_in_cap(UINT8 *p_cfg, tA2D_SBC_CIE *p_cap)
{
    UINT8           status = 0;
    tA2D_SBC_CIE    cfg_cie;

    /* parse configuration */
    if ((status = A2D_ParsSbcInfo(&cfg_cie, p_cfg, FALSE)) != 0) {
        return status;
    }

    /* verify that each parameter is in range */


    /* sampling frequency */
    if ((cfg_cie.samp_freq & p_cap->samp_freq) == 0) {
        status = A2D_NS_SAMP_FREQ;
    }
    /* channel mode */
    else if ((cfg_cie.ch_mode & p_cap->ch_mode) == 0) {
        status = A2D_NS_CH_MODE;
    }
    /* block length */
    else if ((cfg_cie.block_len & p_cap->block_len) == 0) {
        status = A2D_BAD_BLOCK_LEN;
    }
    /* subbands */
    else if ((cfg_cie.num_subbands & p_cap->num_subbands) == 0) {
        status = A2D_NS_SUBBANDS;
    }
    /* allocation method */
    else if ((cfg_cie.alloc_mthd & p_cap->alloc_mthd) == 0) {
        status = A2D_NS_ALLOC_MTHD;
    }
    /* max bitpool */
    else if (cfg_cie.max_bitpool > p_cap->max_bitpool) {
        status = A2D_NS_MAX_BITPOOL;
    }
    /* min bitpool */
    else if (cfg_cie.min_bitpool < p_cap->min_bitpool) {
        status = A2D_NS_MIN_BITPOOL;
    }

    return status;
}

/*******************************************************************************
**
** Function         bta_av_sbc_bld_hdr
**
** Description      This function builds the packet header for MPF1.
**
** Returns          void
**
*******************************************************************************/
void bta_av_sbc_bld_hdr(BT_HDR *p_buf, UINT16 fr_per_pkt)
{
    UINT8   *p;

    p_buf->offset -= BTA_AV_SBC_HDR_SIZE;
    p = (UINT8 *) (p_buf + 1) + p_buf->offset;
    p_buf->len += BTA_AV_SBC_HDR_SIZE;
    A2D_BldSbcMplHdr(p, FALSE, FALSE, FALSE, (UINT8) fr_per_pkt);
}

#endif /* #if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE) */
