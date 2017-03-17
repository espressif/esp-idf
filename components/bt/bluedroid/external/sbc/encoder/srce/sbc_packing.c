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
 *  This file contains code for packing the Encoded data into bit streams.
 *
 ******************************************************************************/
#include "bt_target.h"
#include "sbc_encoder.h"
#include "sbc_enc_func_declare.h"

#if (defined(SBC_ENC_INCLUDED) && SBC_ENC_INCLUDED == TRUE)

#if (SBC_ARM_ASM_OPT==TRUE)
#define Mult32(s32In1,s32In2,s32OutLow)                                                 \
{                                                                                       \
   __asm                                                                                \
   {                                                                                    \
        MUL s32OutLow,s32In1,s32In2;                                                    \
    }                                                                                   \
}
#define Mult64(s32In1, s32In2, s32OutLow, s32OutHi)                                     \
{                                                                                       \
    __asm                                                                               \
    {                                                                                   \
        SMULL s32OutLow,s32OutHi,s32In1,s32In2                                          \
    }                                                                                   \
}
#else
#define Mult32(s32In1,s32In2,s32OutLow) s32OutLow=(SINT32)s32In1*(SINT32)s32In2;
#define Mult64(s32In1, s32In2, s32OutLow, s32OutHi)                                     \
{                                                                                       \
    s32OutLow   = ((SINT32)(UINT16)s32In1  * (UINT16)s32In2);                           \
    s32TempVal2 = (SINT32)((s32In1 >> 16) * (UINT16)s32In2);                            \
    s32Carry    = ( (((UINT32)(s32OutLow)>>16)&0xFFFF) +                                \
                                        + (s32TempVal2 & 0xFFFF) ) >> 16;               \
    s32OutLow   += (s32TempVal2 << 16);                                                 \
    s32OutHi     = (s32TempVal2 >> 16) + s32Carry;                                      \
}
#endif

void EncPacking(SBC_ENC_PARAMS *pstrEncParams)
{
    UINT8       *pu8PacketPtr;                      /* packet ptr*/
    UINT8 Temp;
    SINT32      s32Blk;                             /* counter for block*/
    SINT32      s32Ch;                              /* counter for channel*/
    SINT32      s32Sb;                              /* counter for sub-band*/
    SINT32 s32PresentBit;                      /* represents bit to be stored*/
    /*SINT32 s32LoopCountI;                       loop counter*/
    SINT32 s32LoopCountJ;                      /* loop counter*/
    UINT32 u32QuantizedSbValue, u32QuantizedSbValue0; /* temp variable to store quantized sb val*/
    SINT32 s32LoopCount;                       /* loop counter*/
    UINT8 u8XoredVal;                         /* to store XORed value in CRC calculation*/
    UINT8 u8CRC;                              /* to store CRC value*/
    SINT16 *ps16GenPtr;
    SINT32 s32NumOfBlocks;
    SINT32 s32NumOfSubBands = pstrEncParams->s16NumOfSubBands;
    SINT32 s32NumOfChannels = pstrEncParams->s16NumOfChannels;
    UINT32 u32SfRaisedToPow2;   /*scale factor raised to power 2*/
    SINT16 *ps16ScfPtr;
    SINT32 *ps32SbPtr;
    UINT16 u16Levels;   /*to store levels*/
    SINT32 s32Temp1;    /*used in 64-bit multiplication*/
    SINT32 s32Low;  /*used in 64-bit multiplication*/
#if (SBC_IS_64_MULT_IN_QUANTIZER==TRUE)
    SINT32 s32Hi1, s32Low1, s32Carry, s32TempVal2, s32Hi, s32Temp2;
#endif

    pu8PacketPtr    = pstrEncParams->pu8NextPacket;    /*Initialize the ptr*/
    *pu8PacketPtr++ = (UINT8)0x9C;  /*Sync word*/
    *pu8PacketPtr++ = (UINT8)(pstrEncParams->FrameHeader);

    *pu8PacketPtr = (UINT8)(pstrEncParams->s16BitPool & 0x00FF);
    pu8PacketPtr += 2;  /*skip for CRC*/

    /*here it indicate if it is byte boundary or nibble boundary*/
    s32PresentBit = 8;
    Temp = 0;
#if (SBC_JOINT_STE_INCLUDED == TRUE)
    if (pstrEncParams->s16ChannelMode == SBC_JOINT_STEREO) {
        /* pack join stero parameters */
        for (s32Sb = 0; s32Sb < s32NumOfSubBands; s32Sb++) {
            Temp <<= 1;
            Temp |= pstrEncParams->as16Join[s32Sb];
        }

        /* pack RFA */
        if (s32NumOfSubBands == SUB_BANDS_4) {
            s32PresentBit = 4;
        } else {
            *(pu8PacketPtr++) = Temp;
            Temp = 0;
        }
    }
#endif

    /* Pack Scale factor */
    ps16GenPtr = pstrEncParams->as16ScaleFactor;
    s32Sb = s32NumOfChannels * s32NumOfSubBands;
    /*Temp=*pu8PacketPtr;*/
    for (s32Ch = s32Sb; s32Ch > 0; s32Ch--) {
        Temp <<= 4;
        Temp |= *ps16GenPtr++;

        if (s32PresentBit == 4) {
            s32PresentBit = 8;
            *(pu8PacketPtr++) = Temp;
            Temp = 0;
        } else {
            s32PresentBit = 4;
        }
    }

    /* Pack samples */
    ps32SbPtr   = pstrEncParams->s32SbBuffer;
    /*Temp=*pu8PacketPtr;*/
    s32NumOfBlocks = pstrEncParams->s16NumOfBlocks;
    for (s32Blk = s32NumOfBlocks - 1; s32Blk >= 0; s32Blk--) {
        ps16GenPtr  = pstrEncParams->as16Bits;
        ps16ScfPtr  = pstrEncParams->as16ScaleFactor;
        for (s32Ch = s32Sb - 1; s32Ch >= 0; s32Ch--) {
            s32LoopCount = *ps16GenPtr++;
            if (s32LoopCount != 0) {
#if (SBC_IS_64_MULT_IN_QUANTIZER==TRUE)
                /* finding level from reconstruction part of decoder */
                u32SfRaisedToPow2 = ((UINT32)1 << ((*ps16ScfPtr) + 1));
                u16Levels = (UINT16)(((UINT32)1 << s32LoopCount) - 1);

                /* quantizer */
                s32Temp1 = (*ps32SbPtr >> 2) + (u32SfRaisedToPow2 << 12);
                s32Temp2 = u16Levels;

                Mult64 (s32Temp1, s32Temp2, s32Low, s32Hi);

                s32Low1   = s32Low >> ((*ps16ScfPtr) + 2);
                s32Low1  &= ((UINT32)1 << (32 - ((*ps16ScfPtr) + 2))) - 1;
                s32Hi1    = s32Hi << (32 - ((*ps16ScfPtr) + 2));

                u32QuantizedSbValue0 = (UINT16)((s32Low1 | s32Hi1) >> 12);
#else
                /* finding level from reconstruction part of decoder */
                u32SfRaisedToPow2 = ((UINT32)1 << *ps16ScfPtr);
                u16Levels = (UINT16)(((UINT32)1 << s32LoopCount) - 1);

                /* quantizer */
                s32Temp1 = (*ps32SbPtr >> 15) + u32SfRaisedToPow2;
                Mult32(s32Temp1, u16Levels, s32Low);
                s32Low >>= (*ps16ScfPtr + 1);
                u32QuantizedSbValue0 = (UINT16)s32Low;
#endif
                /*store the number of bits required and the quantized s32Sb
                sample to ease the coding*/
                u32QuantizedSbValue = u32QuantizedSbValue0;

                if (s32PresentBit >= s32LoopCount) {
                    Temp <<= s32LoopCount;
                    Temp |= u32QuantizedSbValue;
                    s32PresentBit -= s32LoopCount;
                } else {
                    while (s32PresentBit < s32LoopCount) {
                        s32LoopCount -= s32PresentBit;
                        u32QuantizedSbValue >>= s32LoopCount;

                        /*remove the unwanted msbs*/
                        /*u32QuantizedSbValue <<= 16 - s32PresentBit;
                        u32QuantizedSbValue >>= 16 - s32PresentBit;*/

                        Temp <<= s32PresentBit;

                        Temp |= u32QuantizedSbValue ;
                        /*restore the original*/
                        u32QuantizedSbValue = u32QuantizedSbValue0;

                        *(pu8PacketPtr++) = Temp;
                        Temp = 0;
                        s32PresentBit = 8;
                    }
                    Temp <<= s32LoopCount;

                    /* remove the unwanted msbs */
                    /*u32QuantizedSbValue <<= 16 - s32LoopCount;
                    u32QuantizedSbValue >>= 16 - s32LoopCount;*/

                    Temp |= u32QuantizedSbValue;

                    s32PresentBit -= s32LoopCount;
                }
            }
            ps16ScfPtr++;
            ps32SbPtr++;
        }
    }

    Temp <<= s32PresentBit;
    *pu8PacketPtr = Temp;
    pstrEncParams->u16PacketLength = pu8PacketPtr - pstrEncParams->pu8NextPacket + 1;
    /*find CRC*/
    pu8PacketPtr = pstrEncParams->pu8NextPacket + 1;  /*Initialize the ptr*/
    u8CRC = 0x0F;
    s32LoopCount = s32Sb >> 1;

    /*
    The loops is run from the start of the packet till the scale factor
    parameters. In case of JS, 'join' parameter is included in the packet
    so that many more bytes are included in CRC calculation.
    */
    Temp = *pu8PacketPtr;
    for (s32Ch = 1; s32Ch < (s32LoopCount + 4); s32Ch++) {
        /* skip sync word and CRC bytes */
        if (s32Ch != 3) {
            for (s32LoopCountJ = 7; s32LoopCountJ >= 0; s32LoopCountJ--) {
                u8XoredVal = ((u8CRC >> 7) & 0x01) ^ ((Temp >> s32LoopCountJ) & 0x01);
                u8CRC <<= 1;
                u8CRC ^= (u8XoredVal * 0x1D);
                u8CRC &= 0xFF;
            }
        }
        Temp = *(++pu8PacketPtr);
    }

    if (pstrEncParams->s16ChannelMode == SBC_JOINT_STEREO) {
        for (s32LoopCountJ = 7; s32LoopCountJ >= (8 - s32NumOfSubBands); s32LoopCountJ--) {
            u8XoredVal = ((u8CRC >> 7) & 0x01) ^ ((Temp >> s32LoopCountJ) & 0x01);
            u8CRC <<= 1;
            u8CRC ^= (u8XoredVal * 0x1D);
            u8CRC &= 0xFF;
        }
    }

    /* CRC calculation ends here */

    /* store CRC in packet */
    pu8PacketPtr = pstrEncParams->pu8NextPacket;    /*Initialize the ptr*/
    pu8PacketPtr += 3;
    *pu8PacketPtr = u8CRC;
    pstrEncParams->pu8NextPacket += pstrEncParams->u16PacketLength; /* move the pointer to the end in case there is more than one frame to encode */
}

#endif /* #if (defined(SBC_ENC_INCLUDED) && SBC_ENC_INCLUDED == TRUE) */
