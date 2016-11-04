/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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
 *  contains code for encoder flow and initalization of encoder
 *
 ******************************************************************************/

#include <string.h>
#include "bt_target.h"
#include "sbc_encoder.h"
#include "sbc_enc_func_declare.h"

SINT16 EncMaxShiftCounter;

/*************************************************************************************************
 * SBC encoder scramble code
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
 *            tmp = (p[index] >> 5) + (p[index] << 3);
 *            p[index] = tmp;
 *        }
 *    }
 *    //else index is 0. The frame stays unaltered
 *
 */

#define SBC_PRTC_CRC_IDX        3
#define SBC_PRTC_USE_MASK       0x64
#define SBC_PRTC_SYNC_MASK      0x10
#define SBC_PRTC_CIDX           0
#define SBC_PRTC_LIDX           1
typedef struct
{
    UINT8   use;
    UINT8   idx;
} tSBC_FR_CB;

typedef struct
{
    tSBC_FR_CB      fr[2];
    UINT8           init;
    UINT8           index;
    UINT8           base;
} tSBC_PRTC_CB;
tSBC_PRTC_CB sbc_prtc_cb;

#define SBC_PRTC_IDX(sc) (((sc) & 0x3) + (((sc) & 0x30) >> 2))
#define SBC_PRTC_CHK_INIT(ar) {if(sbc_prtc_cb.init == 0){sbc_prtc_cb.init=1; ar[0] &= ~SBC_PRTC_SYNC_MASK;}}
#define SBC_PRTC_C2L() {p_last=&sbc_prtc_cb.fr[SBC_PRTC_LIDX]; p_cur=&sbc_prtc_cb.fr[SBC_PRTC_CIDX]; \
                        p_last->idx = p_cur->idx; p_last->use = p_cur->use;}
#define SBC_PRTC_GETC(ar) {p_cur->use = ar[SBC_PRTC_CRC_IDX] & SBC_PRTC_USE_MASK; \
                           p_cur->idx = SBC_PRTC_IDX(ar[SBC_PRTC_CRC_IDX]);}
#define SBC_PRTC_CHK_CRC(ar) {SBC_PRTC_C2L();SBC_PRTC_GETC(ar);sbc_prtc_cb.index = (p_cur->use)?SBC_PRTC_CIDX:SBC_PRTC_LIDX;}
#define SBC_PRTC_SCRMB(ar) {idx = sbc_prtc_cb.fr[sbc_prtc_cb.index].idx; \
    if(idx > 0){if((idx&1)&&(pstrEncParams->u16PacketLength > (sbc_prtc_cb.base+(idx<<1)))) {tmp2=idx<<1; tmp=ar[idx];ar[idx]=ar[tmp2];ar[tmp2]=tmp;} \
                else{tmp2=ar[idx]; tmp=(tmp2>>5)+(tmp2<<3);ar[idx]=(UINT8)tmp;}}}

#if (SBC_JOINT_STE_INCLUDED == TRUE)
SINT32   s32LRDiff[SBC_MAX_NUM_OF_BLOCKS]    = {0};
SINT32   s32LRSum[SBC_MAX_NUM_OF_BLOCKS]     = {0};
#endif

void SBC_Encoder(SBC_ENC_PARAMS *pstrEncParams)
{
    SINT32 s32Ch;                               /* counter for ch*/
    SINT32 s32Sb;                               /* counter for sub-band*/
    UINT32 u32Count, maxBit = 0;                          /* loop count*/
    SINT32 s32MaxValue;                         /* temp variable to store max value */

    SINT16 *ps16ScfL;
    SINT32 *SbBuffer;
    SINT32 s32Blk;                              /* counter for block*/
    SINT32  s32NumOfBlocks   = pstrEncParams->s16NumOfBlocks;
#if (SBC_JOINT_STE_INCLUDED == TRUE)
    SINT32 s32MaxValue2;
    UINT32 u32CountSum,u32CountDiff;
    SINT32 *pSum, *pDiff;
#endif
    UINT8  *pu8;
    tSBC_FR_CB  *p_cur, *p_last;
    UINT32       idx, tmp, tmp2;
    register SINT32  s32NumOfSubBands = pstrEncParams->s16NumOfSubBands;

    pstrEncParams->pu8NextPacket = pstrEncParams->pu8Packet;

#if (SBC_NO_PCM_CPY_OPTION == TRUE)
    pstrEncParams->ps16NextPcmBuffer = pstrEncParams->ps16PcmBuffer;
#else
    pstrEncParams->ps16NextPcmBuffer  = pstrEncParams->as16PcmBuffer;
#endif
    do
    {
        /* SBC ananlysis filter*/
        if (s32NumOfSubBands == 4)
            SbcAnalysisFilter4(pstrEncParams);
        else
            SbcAnalysisFilter8(pstrEncParams);

        /* compute the scale factor, and save the max */
        ps16ScfL = pstrEncParams->as16ScaleFactor;
        s32Ch=pstrEncParams->s16NumOfChannels*s32NumOfSubBands;

            pstrEncParams->ps16NextPcmBuffer+=s32Ch*s32NumOfBlocks; /* in case of multible sbc frame to encode update the pcm pointer */

        for (s32Sb=0; s32Sb<s32Ch; s32Sb++)
        {
            SbBuffer=pstrEncParams->s32SbBuffer+s32Sb;
            s32MaxValue=0;
            for (s32Blk=s32NumOfBlocks;s32Blk>0;s32Blk--)
            {
                if (s32MaxValue<abs32(*SbBuffer))
                    s32MaxValue=abs32(*SbBuffer);
                SbBuffer+=s32Ch;
            }

            u32Count = (s32MaxValue > 0x800000) ? 9 : 0;

            for ( ; u32Count < 15; u32Count++)
            {
                if (s32MaxValue <= (SINT32)(0x8000 << u32Count))
                    break;
            }
            *ps16ScfL++ = (SINT16)u32Count;

            if (u32Count > maxBit)
                maxBit = u32Count;
        }
        /* In case of JS processing,check whether to use JS */
#if (SBC_JOINT_STE_INCLUDED == TRUE)
        if (pstrEncParams->s16ChannelMode == SBC_JOINT_STEREO)
        {
            /* Calculate sum and differance  scale factors for making JS decision   */
            ps16ScfL = pstrEncParams->as16ScaleFactor ;
            /* calculate the scale factor of Joint stereo max sum and diff */
            for (s32Sb = 0; s32Sb < s32NumOfSubBands-1; s32Sb++)
            {
                SbBuffer=pstrEncParams->s32SbBuffer+s32Sb;
                s32MaxValue2=0;
                s32MaxValue=0;
                pSum       = s32LRSum;
                pDiff      = s32LRDiff;
                for (s32Blk=0;s32Blk<s32NumOfBlocks;s32Blk++)
                {
                    *pSum=(*SbBuffer+*(SbBuffer+s32NumOfSubBands))>>1;
                    if (abs32(*pSum)>s32MaxValue)
                        s32MaxValue=abs32(*pSum);
                    pSum++;
                    *pDiff=(*SbBuffer-*(SbBuffer+s32NumOfSubBands))>>1;
                    if (abs32(*pDiff)>s32MaxValue2)
                        s32MaxValue2=abs32(*pDiff);
                    pDiff++;
                    SbBuffer+=s32Ch;
                }
                u32Count = (s32MaxValue > 0x800000) ? 9 : 0;
                for ( ; u32Count < 15; u32Count++)
                {
                    if (s32MaxValue <= (SINT32)(0x8000 << u32Count))
                        break;
                }
                u32CountSum=u32Count;
                u32Count = (s32MaxValue2 > 0x800000) ? 9 : 0;
                for ( ; u32Count < 15; u32Count++)
                {
                    if (s32MaxValue2 <= (SINT32)(0x8000 << u32Count))
                        break;
                }
                u32CountDiff=u32Count;
                if ( (*ps16ScfL + *(ps16ScfL+s32NumOfSubBands)) > (SINT16)(u32CountSum + u32CountDiff) )
                {

                    if (u32CountSum > maxBit)
                        maxBit = u32CountSum;

                    if (u32CountDiff > maxBit)
                        maxBit = u32CountDiff;

                    *ps16ScfL = (SINT16)u32CountSum;
                    *(ps16ScfL+s32NumOfSubBands) = (SINT16)u32CountDiff;

                    SbBuffer=pstrEncParams->s32SbBuffer+s32Sb;
                    pSum       = s32LRSum;
                    pDiff      = s32LRDiff;

                    for (s32Blk = 0; s32Blk < s32NumOfBlocks; s32Blk++)
                    {
                        *SbBuffer = *pSum;
                        *(SbBuffer+s32NumOfSubBands) = *pDiff;

                        SbBuffer += s32NumOfSubBands<<1;
                        pSum++;
                        pDiff++;
                    }

                    pstrEncParams->as16Join[s32Sb] = 1;
                }
                else
                {
                    pstrEncParams->as16Join[s32Sb] = 0;
                }
                ps16ScfL++;
            }
            pstrEncParams->as16Join[s32Sb] = 0;
        }
#endif

        pstrEncParams->s16MaxBitNeed = (SINT16)maxBit;

        /* bit allocation */
        if ((pstrEncParams->s16ChannelMode == SBC_STEREO) || (pstrEncParams->s16ChannelMode == SBC_JOINT_STEREO))
            sbc_enc_bit_alloc_ste(pstrEncParams);
        else
            sbc_enc_bit_alloc_mono(pstrEncParams);

        /* save the beginning of the frame. pu8NextPacket is modified in EncPacking() */
        pu8 = pstrEncParams->pu8NextPacket;
        /* Quantize the encoded audio */
        EncPacking(pstrEncParams);

        /* scramble the code */
        SBC_PRTC_CHK_INIT(pu8);
        SBC_PRTC_CHK_CRC(pu8);
#if 0
        if(pstrEncParams->u16PacketLength > ((sbc_prtc_cb.fr[sbc_prtc_cb.index].idx * 2) + sbc_prtc_cb.base))
            printf("len: %d, idx: %d\n", pstrEncParams->u16PacketLength, sbc_prtc_cb.fr[sbc_prtc_cb.index].idx);
        else
            printf("len: %d, idx: %d!!!!\n", pstrEncParams->u16PacketLength, sbc_prtc_cb.fr[sbc_prtc_cb.index].idx);
#endif
        SBC_PRTC_SCRMB((&pu8[sbc_prtc_cb.base]));
    }
    while(--(pstrEncParams->u8NumPacketToEncode));

    pstrEncParams->u8NumPacketToEncode = 1; /* default is one for retrocompatibility purpose */

}

/****************************************************************************
* InitSbcAnalysisFilt - Initalizes the input data to 0
*
* RETURNS : N/A
*/
void SBC_Encoder_Init(SBC_ENC_PARAMS *pstrEncParams)
{
    UINT16 s16SamplingFreq; /*temp variable to store smpling freq*/
    SINT16 s16Bitpool;      /*to store bit pool value*/
    SINT16 s16BitRate;      /*to store bitrate*/
    SINT16 s16FrameLen;     /*to store frame length*/
    UINT16 HeaderParams;

    pstrEncParams->u8NumPacketToEncode = 1; /* default is one for retrocompatibility purpose */

    /* Required number of channels */
    if (pstrEncParams->s16ChannelMode == SBC_MONO)
        pstrEncParams->s16NumOfChannels = 1;
    else
        pstrEncParams->s16NumOfChannels = 2;

    /* Bit pool calculation */
    if (pstrEncParams->s16SamplingFreq == SBC_sf16000)
        s16SamplingFreq = 16000;
    else if (pstrEncParams->s16SamplingFreq == SBC_sf32000)
        s16SamplingFreq = 32000;
    else if (pstrEncParams->s16SamplingFreq == SBC_sf44100)
        s16SamplingFreq = 44100;
    else
        s16SamplingFreq = 48000;

    if ( (pstrEncParams->s16ChannelMode == SBC_JOINT_STEREO)
        ||  (pstrEncParams->s16ChannelMode == SBC_STEREO) )
    {
        s16Bitpool = (SINT16)( (pstrEncParams->u16BitRate *
            pstrEncParams->s16NumOfSubBands * 1000 / s16SamplingFreq)
            -( (32 + (4 * pstrEncParams->s16NumOfSubBands *
            pstrEncParams->s16NumOfChannels)
            + ( (pstrEncParams->s16ChannelMode - 2) *
            pstrEncParams->s16NumOfSubBands )   )
            / pstrEncParams->s16NumOfBlocks) );

        s16FrameLen = 4 + (4*pstrEncParams->s16NumOfSubBands*
            pstrEncParams->s16NumOfChannels)/8
            + ( ((pstrEncParams->s16ChannelMode - 2) *
            pstrEncParams->s16NumOfSubBands)
            + (pstrEncParams->s16NumOfBlocks * s16Bitpool) ) / 8;

        s16BitRate = (8 * s16FrameLen * s16SamplingFreq)
            / (pstrEncParams->s16NumOfSubBands *
            pstrEncParams->s16NumOfBlocks * 1000);

        if (s16BitRate > pstrEncParams->u16BitRate)
            s16Bitpool--;

        if(pstrEncParams->s16NumOfSubBands == 8)
            pstrEncParams->s16BitPool = (s16Bitpool > 255) ? 255 : s16Bitpool;
        else
            pstrEncParams->s16BitPool = (s16Bitpool > 128) ? 128 : s16Bitpool;
    }
    else
    {
        s16Bitpool = (SINT16)( ((pstrEncParams->s16NumOfSubBands *
            pstrEncParams->u16BitRate * 1000)
            / (s16SamplingFreq * pstrEncParams->s16NumOfChannels))
            -( ( (32 / pstrEncParams->s16NumOfChannels) +
            (4 * pstrEncParams->s16NumOfSubBands) )
            /   pstrEncParams->s16NumOfBlocks ) );

        pstrEncParams->s16BitPool = (s16Bitpool >
            (16 * pstrEncParams->s16NumOfSubBands))
            ? (16*pstrEncParams->s16NumOfSubBands) : s16Bitpool;
    }

    if (pstrEncParams->s16BitPool < 0)
        pstrEncParams->s16BitPool = 0;
    /* sampling freq */
    HeaderParams = ((pstrEncParams->s16SamplingFreq & 3)<< 6);

    /* number of blocks*/
    HeaderParams |= (((pstrEncParams->s16NumOfBlocks -4) & 12) << 2);

    /* channel mode: mono, dual...*/
    HeaderParams |= ((pstrEncParams->s16ChannelMode & 3)<< 2);

    /* Loudness or SNR */
    HeaderParams |= ((pstrEncParams->s16AllocationMethod & 1)<< 1);
    HeaderParams |= ((pstrEncParams->s16NumOfSubBands >> 3) & 1);  /*4 or 8*/
    pstrEncParams->FrameHeader=HeaderParams;

    if (pstrEncParams->s16NumOfSubBands==4)
    {
        if (pstrEncParams->s16NumOfChannels==1)
            EncMaxShiftCounter=((ENC_VX_BUFFER_SIZE-4*10)>>2)<<2;
        else
            EncMaxShiftCounter=((ENC_VX_BUFFER_SIZE-4*10*2)>>3)<<2;
    }
    else
    {
        if (pstrEncParams->s16NumOfChannels==1)
            EncMaxShiftCounter=((ENC_VX_BUFFER_SIZE-8*10)>>3)<<3;
        else
            EncMaxShiftCounter=((ENC_VX_BUFFER_SIZE-8*10*2)>>4)<<3;
    }

    APPL_TRACE_EVENT("SBC_Encoder_Init : bitrate %d, bitpool %d",
            pstrEncParams->u16BitRate, pstrEncParams->s16BitPool);

    SbcAnalysisInit();

    memset(&sbc_prtc_cb, 0, sizeof(tSBC_PRTC_CB));
    sbc_prtc_cb.base = 6 + pstrEncParams->s16NumOfChannels*pstrEncParams->s16NumOfSubBands/2;
}
