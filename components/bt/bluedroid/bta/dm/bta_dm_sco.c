/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  This file contains the down sampling utility to convert PCM samples in
 *  16k/32k/48k/44.1k/22050/11025 sampling rate into 8K/16bits samples
 *  required for SCO channel format. One API function isprovided and only
 *  possible to be used when transmitting SCO data is sent via HCI
 *  interface.
 *
 ******************************************************************************/
#include <string.h>
#include "bta_api.h"
#include "bta_sys.h"

#if (BTM_SCO_HCI_INCLUDED == TRUE)

#ifndef BTA_DM_SCO_DEBUG
#define BTA_DM_SCO_DEBUG    FALSE
#endif
/*****************************************************************************
**  Constants
*****************************************************************************/

#define BTA_DM_PCM_OVERLAP_SIZE			     48

#define BTA_DM_PCM_SMPL_RATE_44100     44100
#define BTA_DM_PCM_SMPL_RATE_22050     22050
#define BTA_DM_PCM_SMPL_RATE_11025     11025

/*****************************************************************************
**  Data types for PCM Resampling utility
*****************************************************************************/

typedef INT32   (*PCONVERT_TO_BT_FILTERED)  (UINT8 *pSrc, void *pDst, UINT32 dwSrcSamples,
                             UINT32 dwSrcSps,INT32 *pLastCurPos, UINT8 *pOverlapArea);
typedef INT32   (*PCONVERT_TO_BT_NOFILTER)  (void *pSrc, void *pDst, UINT32 dwSrcSamples,
                                             UINT32 dwSrcSps);
typedef struct
{
    UINT8               overlap_area[BTA_DM_PCM_OVERLAP_SIZE * 4];
    UINT32              cur_pos;    /* current position */
    UINT32              src_sps;    /* samples per second (source audio data) */
    PCONVERT_TO_BT_FILTERED     filter;    /* the action function to do the
                                    conversion 44100, 22050, 11025*/
    PCONVERT_TO_BT_NOFILTER     nofilter;    /* the action function to do
                                        the conversion 48000, 32000, 16000*/
    UINT32              bits;       /* number of bits per pcm sample */
    UINT32              n_channels; /* number of channels (i.e. mono(1), stereo(2)...) */
    UINT32              sample_size;
    UINT32              can_be_filtered;
    UINT32	            divisor;
} tBTA_DM_PCM_RESAMPLE_CB;

tBTA_DM_PCM_RESAMPLE_CB bta_dm_pcm_cb;

/*****************************************************************************
**  Macro Definition
*****************************************************************************/


#define CHECK_SATURATION16(x)                                           \
            if (x > 32767)                                              \
                x = 32767;                                              \
            else if (x < -32768)                                        \
                x = -32768;

////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define CONVERT_44100_TO_BLUETOOTH(pStart, pEnd)                            \
    {                                                                       \
        INT32         out1, out2, out3, out4, out5;                         \
        SRC_TYPE    *pS = (SRC_TYPE *)pStart;                               \
        SRC_TYPE    *pSEnd = (SRC_TYPE *)pEnd;                              \
                                                                            \
        while (pS < pSEnd)                                                  \
        {                                                                   \
            CurrentPos -= 8000;                                             \
                                                                            \
            if (CurrentPos >= 0)                                            \
            {                                                               \
                pS += SRC_CHANNELS;                                         \
                continue;                                                   \
            }                                                               \
            CurrentPos += dwSrcSps;                                         \
                                                                            \
            out1 = (SRC_SAMPLE(0) * 1587)                                   \
                 + ((SRC_SAMPLE(1) + SRC_SAMPLE(-1)) * 1522)                \
                 + ((SRC_SAMPLE(2) + SRC_SAMPLE(-2)) * 1337)                \
                 + ((SRC_SAMPLE(3) + SRC_SAMPLE(-3)) * 1058);               \
                                                                            \
            out1 = out1 / 30000;                                            \
                                                                            \
            out2 = ((SRC_SAMPLE(4) + SRC_SAMPLE(-4)) * 725)                 \
                 + ((SRC_SAMPLE(5) + SRC_SAMPLE(-5)) * 384)                 \
                 + ((SRC_SAMPLE(6) + SRC_SAMPLE(-6)) * 79);                 \
                                                                            \
            out2 = out2 / 30000;                                            \
                                                                            \
            out3 = ((SRC_SAMPLE(7) + SRC_SAMPLE(-7)) * 156)                 \
                 + ((SRC_SAMPLE(8) + SRC_SAMPLE(-8)) * 298)                 \
                 + ((SRC_SAMPLE(9) + SRC_SAMPLE(-9)) * 345);                \
                                                                            \
            out3 = out3 / 30000;                                            \
                                                                            \
            out4 = ((SRC_SAMPLE(10) + SRC_SAMPLE(-10)) * 306)               \
                 + ((SRC_SAMPLE(11) + SRC_SAMPLE(-11)) * 207)               \
                 + ((SRC_SAMPLE(12) + SRC_SAMPLE(-12)) * 78);               \
                                                                            \
            out4 = out4 / 30000;                                            \
                                                                            \
            out5 = out1 + out2 - out3 - out4;                               \
                                                                            \
            CHECK_SATURATION16(out5);                                       \
            *psBtOut++ = (INT16)out5;                                       \
                                                                            \
            pS += SRC_CHANNELS;                                             \
        }                                                                   \
    }


////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define CONVERT_22050_TO_BLUETOOTH(pStart, pEnd)                            \
    {                                                                       \
        INT32         out1, out2, out3, out4, out5;                         \
        SRC_TYPE    *pS = (SRC_TYPE *)pStart;                               \
        SRC_TYPE    *pSEnd = (SRC_TYPE *)pEnd;                              \
                                                                            \
        while (pS < pSEnd)                                                  \
        {                                                                   \
            CurrentPos -= 8000;                                             \
                                                                            \
            if (CurrentPos >= 0)                                            \
            {                                                               \
                pS += SRC_CHANNELS;                                         \
                continue;                                                   \
            }                                                               \
            CurrentPos += dwSrcSps;                                         \
                                                                            \
            out1 = (SRC_SAMPLE(0) * 2993)                                   \
                 + ((SRC_SAMPLE(1) + SRC_SAMPLE(-1)) * 2568)                \
                 + ((SRC_SAMPLE(2) + SRC_SAMPLE(-2)) * 1509)                \
                 + ((SRC_SAMPLE(3) + SRC_SAMPLE(-3)) * 331);                \
                                                                            \
            out1 = out1 / 30000;                                            \
                                                                            \
            out2 = ((SRC_SAMPLE(4) + SRC_SAMPLE(-4)) * 454)                 \
                 + ((SRC_SAMPLE(5) + SRC_SAMPLE(-5)) * 620)                 \
                 + ((SRC_SAMPLE(6) + SRC_SAMPLE(-6)) * 305);                \
                                                                            \
            out2 = out2 / 30000;                                            \
                                                                            \
            out3 = ((SRC_SAMPLE(7) + SRC_SAMPLE(-7)) * 127)                 \
                 + ((SRC_SAMPLE(8) + SRC_SAMPLE(-8)) * 350)                 \
                 + ((SRC_SAMPLE(9) + SRC_SAMPLE(-9)) * 265)                 \
                 + ((SRC_SAMPLE(10) + SRC_SAMPLE(-10)) * 6);                \
                                                                            \
            out3 = out3 / 30000;                                            \
                                                                            \
            out4 = ((SRC_SAMPLE(11) + SRC_SAMPLE(-11)) * 201);              \
                                                                            \
            out4 = out4 / 30000;                                            \
                                                                            \
            out5 = out1 - out2 + out3 - out4;                               \
                                                                            \
            CHECK_SATURATION16(out5);                                       \
            *psBtOut++ = (INT16)out5;                                       \
                                                                            \
            pS += SRC_CHANNELS;                                             \
        }                                                                   \
    }


////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define CONVERT_11025_TO_BLUETOOTH(pStart, pEnd)                            \
    {                                                                       \
        INT32         out1;                                                   \
        SRC_TYPE    *pS = (SRC_TYPE *)pStart;                               \
        SRC_TYPE    *pSEnd = (SRC_TYPE *)pEnd;                              \
                                                                            \
        while (pS < pSEnd)                                                  \
        {                                                                   \
            CurrentPos -= 8000;                                             \
                                                                            \
            if (CurrentPos >= 0)                                            \
            {                                                               \
                pS += SRC_CHANNELS;                                         \
                continue;                                                   \
            }                                                               \
            CurrentPos += dwSrcSps;                                         \
                                                                            \
            out1 = (SRC_SAMPLE(0) * 6349)                                   \
                 + ((SRC_SAMPLE(1) + SRC_SAMPLE(-1)) * 2874)                \
                 - ((SRC_SAMPLE(2) + SRC_SAMPLE(-2)) * 1148)                \
                 - ((SRC_SAMPLE(3) + SRC_SAMPLE(-3)) * 287)                 \
                 + ((SRC_SAMPLE(4) + SRC_SAMPLE(-4)) * 675)                 \
                 - ((SRC_SAMPLE(5) + SRC_SAMPLE(-5)) * 258)                 \
                 - ((SRC_SAMPLE(6) + SRC_SAMPLE(-6)) * 206)                 \
                 + ((SRC_SAMPLE(7) + SRC_SAMPLE(-7)) * 266);                \
                                                                            \
            out1 = out1 / 30000;                                            \
                                                                            \
            CHECK_SATURATION16(out1);                                       \
            *psBtOut++ = (INT16)out1;                                       \
                                                                            \
            pS += SRC_CHANNELS;                                             \
        }                                                                   \
    }


////////////////////////////////////////////////////////////////////////////////////////////////////
//
#undef  SRC_CHANNELS
#undef  SRC_SAMPLE
#undef  SRC_TYPE

#define SRC_TYPE        UINT8
#define SRC_CHANNELS    1
#define SRC_SAMPLE(x)   ((pS[x]  - 0x80) << 8)

/*****************************************************************************
**  Local Function
*****************************************************************************/
INT32 Convert_8M_ToBT_Filtered (UINT8 *pSrc, void *pDst, UINT32 dwSrcSamples,
                    UINT32 dwSrcSps, INT32 *pLastCurPos, UINT8 *pOverlapArea)
{
    INT32             CurrentPos = *pLastCurPos;
    SRC_TYPE        *pIn, *pInEnd;
    SRC_TYPE        *pOv, *pOvEnd;
    INT16           *psBtOut = (INT16 *)pDst;
#if BTA_DM_SCO_DEBUG
    APPL_TRACE_DEBUG("Convert_8M_ToBT_Filtered,  CurrentPos %d\n", CurrentPos);
#endif
    memcpy (pOverlapArea + (BTA_DM_PCM_OVERLAP_SIZE * 2), pSrc, BTA_DM_PCM_OVERLAP_SIZE * 2);

    pOv    = (SRC_TYPE *)(pOverlapArea + BTA_DM_PCM_OVERLAP_SIZE);
	pOvEnd = (SRC_TYPE *)(pOverlapArea + (BTA_DM_PCM_OVERLAP_SIZE * 3));

    pIn     = (SRC_TYPE *)(pSrc + BTA_DM_PCM_OVERLAP_SIZE);
	pInEnd  = (SRC_TYPE *)(pSrc + (dwSrcSamples * SRC_CHANNELS * sizeof (SRC_TYPE)) - \
        BTA_DM_PCM_OVERLAP_SIZE);

    if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_44100)
    {
        CONVERT_44100_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_44100_TO_BLUETOOTH(pIn, pInEnd);
    }
    else if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_22050)
    {
        CONVERT_22050_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_22050_TO_BLUETOOTH(pIn, pInEnd);
    }
    else if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_11025)
    {
        CONVERT_11025_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_11025_TO_BLUETOOTH(pIn, pInEnd);
    }

    memcpy (pOverlapArea, pSrc + (dwSrcSamples * SRC_CHANNELS * sizeof (SRC_TYPE)) - \
        (BTA_DM_PCM_OVERLAP_SIZE * 2), BTA_DM_PCM_OVERLAP_SIZE * 2);

    *pLastCurPos = CurrentPos;

    return (psBtOut - (INT16 *)pDst);
}

INT32 Convert_8M_ToBT_NoFilter (void *pSrc, void *pDst, UINT32 dwSrcSamples, UINT32 dwSrcSps)
{
    INT32             CurrentPos;
    UINT8            *pbSrc = (UINT8 *)pSrc;
    INT16           *psDst = (INT16 *)pDst;
    INT16           sWorker;

    //      start at dwSpsSrc / 2, decrement by 8000
    //
    CurrentPos = (dwSrcSps >> 1);

    while (dwSrcSamples--)
    {
        CurrentPos -= 8000;

        if (CurrentPos >= 0)
            pbSrc++;
        else
        {
            sWorker = *pbSrc++;
            sWorker -= 0x80;
            sWorker <<= 8;

            *psDst++ = sWorker;

            CurrentPos += dwSrcSps;
        }
    }

    return (psDst - (INT16 *)pDst);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
#undef  SRC_CHANNELS
#undef  SRC_SAMPLE
#undef  SRC_TYPE

#define SRC_TYPE        INT16
#define SRC_CHANNELS    1
#define SRC_SAMPLE(x)   pS[x]

INT32 Convert_16M_ToBT_Filtered (UINT8 *pSrc, void *pDst, UINT32 dwSrcSamples,
                                 UINT32 dwSrcSps, INT32 *pLastCurPos, UINT8 *pOverlapArea)
{
    INT32             CurrentPos = *pLastCurPos;
    SRC_TYPE        *pIn, *pInEnd;
    SRC_TYPE        *pOv, *pOvEnd;
    INT16           *psBtOut = (INT16 *)pDst;

    memcpy (pOverlapArea + (BTA_DM_PCM_OVERLAP_SIZE * 2), pSrc, BTA_DM_PCM_OVERLAP_SIZE * 2);

    pOv    = (SRC_TYPE *)(pOverlapArea + BTA_DM_PCM_OVERLAP_SIZE);
	pOvEnd = (SRC_TYPE *)(pOverlapArea + (BTA_DM_PCM_OVERLAP_SIZE * 3));

    pIn     = (SRC_TYPE *)(pSrc + BTA_DM_PCM_OVERLAP_SIZE);
	pInEnd  = (SRC_TYPE *)(pSrc + (dwSrcSamples * SRC_CHANNELS * sizeof (SRC_TYPE)) - BTA_DM_PCM_OVERLAP_SIZE);

    if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_44100)
    {
        CONVERT_44100_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_44100_TO_BLUETOOTH(pIn, pInEnd);
    }
    else if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_22050)
    {
        CONVERT_22050_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_22050_TO_BLUETOOTH(pIn, pInEnd);
    }
    else if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_11025)
    {
        CONVERT_11025_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_11025_TO_BLUETOOTH(pIn, pInEnd);
    }

    memcpy (pOverlapArea, pSrc + (dwSrcSamples * SRC_CHANNELS * sizeof (SRC_TYPE)) - \
        (BTA_DM_PCM_OVERLAP_SIZE * 2), BTA_DM_PCM_OVERLAP_SIZE * 2);

    *pLastCurPos = CurrentPos;

    return (psBtOut - (INT16 *)pDst);
}

INT32 Convert_16M_ToBT_NoFilter (void *pSrc, void *pDst, UINT32 dwSrcSamples, UINT32 dwSrcSps)
{
    INT32             CurrentPos;
    INT16           *psSrc = (INT16 *)pSrc;
    INT16           *psDst = (INT16 *)pDst;

    //      start at dwSpsSrc / 2, decrement by 8000
    //
    CurrentPos = (dwSrcSps >> 1);

    while (dwSrcSamples--)
    {
        CurrentPos -= 8000;

        if (CurrentPos >= 0)
            psSrc++;
        else
        {
            *psDst++ = *psSrc++;

            CurrentPos += dwSrcSps;
        }
    }

    return (psDst - (INT16 *)pDst);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
#undef  SRC_CHANNELS
#undef  SRC_SAMPLE
#undef  SRC_TYPE

#define SRC_TYPE        UINT8
#define SRC_CHANNELS    2
#define SRC_SAMPLE(x) ((((pS[x * 2]  - 0x80) << 8) + ((pS[(x * 2) + 1]  - 0x80) << 8)) >> 1)

INT32 Convert_8S_ToBT_Filtered (UINT8 *pSrc, void *pDst, UINT32 dwSrcSamples,
                                UINT32 dwSrcSps, INT32 *pLastCurPos, UINT8 *pOverlapArea)
{
    INT32             CurrentPos = *pLastCurPos;
    SRC_TYPE        *pIn, *pInEnd;
    SRC_TYPE        *pOv, *pOvEnd;
    INT16           *psBtOut = (INT16 *)pDst;

#if BTA_DM_SCO_DEBUG
    APPL_TRACE_DEBUG("Convert_8S_ToBT_Filtered CurrentPos %d, SRC_TYPE %d, SRC_CHANNELS %d, \
        dwSrcSamples %d,  dwSrcSps %d",  	CurrentPos, sizeof (SRC_TYPE), SRC_CHANNELS, \
        dwSrcSamples, dwSrcSps);
#endif
    memcpy (pOverlapArea + (BTA_DM_PCM_OVERLAP_SIZE * 2), pSrc, BTA_DM_PCM_OVERLAP_SIZE * 2);

    pOv    = (SRC_TYPE *)(pOverlapArea + BTA_DM_PCM_OVERLAP_SIZE);
	pOvEnd = (SRC_TYPE *)(pOverlapArea + (BTA_DM_PCM_OVERLAP_SIZE * 3));

    pIn     = (SRC_TYPE *)(pSrc + BTA_DM_PCM_OVERLAP_SIZE);
	pInEnd  = (SRC_TYPE *)(pSrc + (dwSrcSamples * SRC_CHANNELS * sizeof (SRC_TYPE)) - BTA_DM_PCM_OVERLAP_SIZE);

    if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_44100)
    {
        CONVERT_44100_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_44100_TO_BLUETOOTH(pIn, pInEnd);
    }
    else if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_22050)
    {
        CONVERT_22050_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_22050_TO_BLUETOOTH(pIn, pInEnd);
    }
    else if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_11025)
    {
        CONVERT_11025_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_11025_TO_BLUETOOTH(pIn, pInEnd);
    }

    memcpy (pOverlapArea, pSrc + (dwSrcSamples * SRC_CHANNELS * sizeof (SRC_TYPE)) - \
        (BTA_DM_PCM_OVERLAP_SIZE * 2), BTA_DM_PCM_OVERLAP_SIZE * 2);

    *pLastCurPos = CurrentPos;

    return (psBtOut - (INT16 *)pDst);
}

INT32 Convert_8S_ToBT_NoFilter (void *pSrc, void *pDst, UINT32 dwSrcSamples, UINT32 dwSrcSps)
{
    INT32             CurrentPos;
    UINT8            *pbSrc = (UINT8 *)pSrc;
    INT16           *psDst = (INT16 *)pDst;
    INT16           sWorker, sWorker2;

    //      start at dwSpsSrc / 2, decrement by 8000
    //
    CurrentPos = (dwSrcSps >> 1);

    while (dwSrcSamples--)
    {
        CurrentPos -= 8000;

        if (CurrentPos >= 0)
            pbSrc += 2;
        else
        {
            sWorker = *(unsigned char *)pbSrc;
            sWorker -= 0x80;
            sWorker <<= 8;
            pbSrc++;

            sWorker2 = *(unsigned char *)pbSrc;
            sWorker2 -= 0x80;
            sWorker2 <<= 8;
            pbSrc++;

            sWorker += sWorker2;
            sWorker >>= 1;

            *psDst++ = sWorker;

            CurrentPos += dwSrcSps;
        }
    }

    return (psDst - (INT16 *)pDst);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
#undef  SRC_CHANNELS
#undef  SRC_SAMPLE
#undef  SRC_TYPE

#define SRC_TYPE        INT16
#define SRC_CHANNELS    2
#define SRC_SAMPLE(x) ((pS[x * 2] + pS[(x * 2) + 1]) >> 1)

INT32 Convert_16S_ToBT_Filtered (UINT8 *pSrc, void *pDst, UINT32 dwSrcSamples,
                                 UINT32 dwSrcSps, INT32 *pLastCurPos, UINT8 *pOverlapArea)
{
    INT32             CurrentPos = *pLastCurPos;
    SRC_TYPE        *pIn, *pInEnd;
    SRC_TYPE        *pOv, *pOvEnd;
    INT16           *psBtOut = (INT16 *)pDst;

    memcpy (pOverlapArea + (BTA_DM_PCM_OVERLAP_SIZE * 2), pSrc, BTA_DM_PCM_OVERLAP_SIZE * 2);

    pOv    = (SRC_TYPE *)(pOverlapArea + BTA_DM_PCM_OVERLAP_SIZE);
	pOvEnd = (SRC_TYPE *)(pOverlapArea + (BTA_DM_PCM_OVERLAP_SIZE * 3));

    pIn     = (SRC_TYPE *)(pSrc + BTA_DM_PCM_OVERLAP_SIZE);
	pInEnd  = (SRC_TYPE *)(pSrc + (dwSrcSamples * SRC_CHANNELS * sizeof (SRC_TYPE)) - BTA_DM_PCM_OVERLAP_SIZE);

    if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_44100)
    {
        CONVERT_44100_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_44100_TO_BLUETOOTH(pIn, pInEnd);
    }
    else if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_22050)
    {
        CONVERT_22050_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_22050_TO_BLUETOOTH(pIn, pInEnd);
    }
    else if (dwSrcSps == BTA_DM_PCM_SMPL_RATE_11025)
    {
        CONVERT_11025_TO_BLUETOOTH(pOv, pOvEnd);
        CONVERT_11025_TO_BLUETOOTH(pIn, pInEnd);
    }

    memcpy (pOverlapArea, pSrc + (dwSrcSamples * SRC_CHANNELS * sizeof (SRC_TYPE)) - \
        (BTA_DM_PCM_OVERLAP_SIZE * 2), BTA_DM_PCM_OVERLAP_SIZE * 2);

    *pLastCurPos = CurrentPos;

    return (psBtOut - (INT16 *)pDst);
}

INT32 Convert_16S_ToBT_NoFilter (void *pSrc, void *pDst, UINT32 dwSrcSamples, UINT32 dwSrcSps)
{
    INT32             CurrentPos;
    INT16           *psSrc = (INT16 *)pSrc;
    INT16           *psDst = (INT16 *)pDst;
    INT16           sWorker;

    //      start at dwSpsSrc / 2, decrement by 8000
    //
    CurrentPos = (dwSrcSps >> 1);

    while (dwSrcSamples--)
    {
        CurrentPos -= 8000;

        if (CurrentPos >= 0)
            psSrc += 2;
        else
        {
            /* CR 82894, to avoid overflow, divide before add */
            sWorker  = ((*psSrc) >> 1 );
            psSrc++;
            sWorker += ((*psSrc) >> 1 );
            psSrc++;

            *psDst++ = sWorker;

            CurrentPos += dwSrcSps;
        }
    }

    return (psDst - (INT16 *)pDst);
}

/*******************************************************************************
**
** Function         BTA_DmPcmInitSamples
**
** Description      initialize the down sample converter.
**
**                  src_sps: original samples per second (source audio data)
**                            (ex. 44100, 48000)
**                  bits: number of bits per pcm sample (16)
**                  n_channels: number of channels (i.e. mono(1), stereo(2)...)
**
** Returns          none
**
*******************************************************************************/
void BTA_DmPcmInitSamples (UINT32 src_sps, UINT32 bits, UINT32 n_channels)
{
    tBTA_DM_PCM_RESAMPLE_CB *p_cb = &bta_dm_pcm_cb;

    p_cb->cur_pos       = src_sps / 2;
    p_cb->src_sps       = src_sps;
    p_cb->bits          = bits;
    p_cb->n_channels    = n_channels;
    p_cb->sample_size   = 2;
    p_cb->divisor	    = 2;

    memset(p_cb->overlap_area, 0, sizeof(p_cb->overlap_area) );

    if ((src_sps == BTA_DM_PCM_SMPL_RATE_44100) ||
        (src_sps == BTA_DM_PCM_SMPL_RATE_22050) ||
        (src_sps == BTA_DM_PCM_SMPL_RATE_11025))
         p_cb->can_be_filtered = 1;
    else
         p_cb->can_be_filtered = 0;

#if BTA_DM_SCO_DEBUG
    APPL_TRACE_DEBUG("bta_dm_pcm_init_samples: n_channels = %d bits = %d", n_channels, bits);
#endif
    if(n_channels == 1)
    {
        /* mono */
        if(bits == 8)
        {
            p_cb->filter = (PCONVERT_TO_BT_FILTERED) Convert_8M_ToBT_Filtered;
            p_cb->nofilter = (PCONVERT_TO_BT_NOFILTER) Convert_8M_ToBT_NoFilter;
	        p_cb->divisor	 = 1;
        }
        else
        {
            p_cb->filter = (PCONVERT_TO_BT_FILTERED) Convert_16M_ToBT_Filtered;
            p_cb->nofilter = (PCONVERT_TO_BT_NOFILTER) Convert_16M_ToBT_NoFilter;
        }
    }
    else
    {
        /* stereo */
        if(bits == 8)
        {
            p_cb->filter = (PCONVERT_TO_BT_FILTERED) Convert_8S_ToBT_Filtered;
            p_cb->nofilter = (PCONVERT_TO_BT_NOFILTER) Convert_8S_ToBT_NoFilter;
        }
        else
        {
            p_cb->filter = (PCONVERT_TO_BT_FILTERED) Convert_16S_ToBT_Filtered;
            p_cb->nofilter = (PCONVERT_TO_BT_NOFILTER) Convert_16S_ToBT_NoFilter;
	        p_cb->divisor	 = 4;
        }
    }

#if BTA_DM_SCO_DEBUG
    APPL_TRACE_DEBUG("bta_pcm_init_dwn_sample: cur_pos %d, src_sps %d", \
		p_cb->cur_pos, p_cb->src_sps);
    APPL_TRACE_DEBUG("bta_pcm_init_dwn_sample: bits %d, n_channels %d, sample_size %d, ", \
		p_cb->bits, p_cb->n_channels, p_cb->sample_size);
    APPL_TRACE_DEBUG("bta_pcm_init_dwn_sample: can_be_filtered %d, n_channels: %d, \
        divisor %d", p_cb->can_be_filtered, p_cb->n_channels, p_cb->divisor);
#endif

}

/**************************************************************************************
** Function         BTA_DmPcmResample
**
** Description      Down sampling utility to convert higher sampling rate into 8K/16bits
**                  PCM samples.
**
** Parameters       p_src: pointer to the buffer where the original sampling PCM
**                              are stored.
**                  in_bytes:  Length of the input PCM sample buffer in byte.
**                  p_dst:      pointer to the buffer which is to be used to store
**                              the converted PCM samples.
**
**
** Returns          INT32: number of samples converted.
**
**************************************************************************************/
INT32 BTA_DmPcmResample (void *p_src, UINT32 in_bytes, void *p_dst)
{
    UINT32 out_sample;

#if BTA_DM_SCO_DEBUG
    APPL_TRACE_DEBUG("bta_pcm_resample : insamples  %d",  (in_bytes  / bta_dm_pcm_cb.divisor));
#endif
    if(bta_dm_pcm_cb.can_be_filtered)
    {
        out_sample = (*bta_dm_pcm_cb.filter) (p_src, p_dst, (in_bytes  / bta_dm_pcm_cb.divisor),
            bta_dm_pcm_cb.src_sps, (INT32 *) &bta_dm_pcm_cb.cur_pos, bta_dm_pcm_cb.overlap_area);
    }
    else
    {
        out_sample = (*bta_dm_pcm_cb.nofilter) (p_src, p_dst,
            (in_bytes / bta_dm_pcm_cb.divisor), bta_dm_pcm_cb.src_sps);
    }

#if BTA_DM_SCO_DEBUG
    APPL_TRACE_DEBUG("bta_pcm_resample : outsamples  %d",  out_sample);
#endif

    return (out_sample * bta_dm_pcm_cb.sample_size);
}
#endif
