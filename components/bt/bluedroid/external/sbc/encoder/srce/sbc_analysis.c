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
 *  This file contains the code that performs Analysis of the input audio
 *  stream.
 *
 ******************************************************************************/
#include "bt_target.h"
#include <string.h>
#include "sbc_encoder.h"
#include "sbc_enc_func_declare.h"
/*#include <math.h>*/
#if (defined(SBC_ENC_INCLUDED) && SBC_ENC_INCLUDED == TRUE)

#if (SBC_IS_64_MULT_IN_WINDOW_ACCU == TRUE)
#define WIND_4_SUBBANDS_0_1 (SINT32)0x01659F45  /* gas32CoeffFor4SBs[8] = -gas32CoeffFor4SBs[32] = 0x01659F45 */
#define WIND_4_SUBBANDS_0_2 (SINT32)0x115B1ED2  /* gas32CoeffFor4SBs[16] = -gas32CoeffFor4SBs[24] = 0x115B1ED2 */
#define WIND_4_SUBBANDS_1_0 (SINT32)0x001194E6  /* gas32CoeffFor4SBs[1 et 39] = 0x001194E6 */
#define WIND_4_SUBBANDS_1_1 (SINT32)0x029DBAA3  /* gas32CoeffFor4SBs[9 et 31] = 0x029DBAA3 */
#define WIND_4_SUBBANDS_1_2 (SINT32)0x18F55C90  /* gas32CoeffFor4SBs[17 et 23] = 0x18F55C90 */
#define WIND_4_SUBBANDS_1_3 (SINT32)0xF60FAF37  /* gas32CoeffFor4SBs[15 et 25] = 0xF60FAF37 */
#define WIND_4_SUBBANDS_1_4 (SINT32)0xFF9BB9D5  /* gas32CoeffFor4SBs[7 et 33] = 0xFF9BB9D5 */
#define WIND_4_SUBBANDS_2_0 (SINT32)0x0030E2D3  /* gas32CoeffFor4SBs[2 et 38] = 0x0030E2D3 */
#define WIND_4_SUBBANDS_2_1 (SINT32)0x03B23341  /* gas32CoeffFor4SBs[10 et 30] = 0x03B23341 */
#define WIND_4_SUBBANDS_2_2 (SINT32)0x1F91CA46  /* gas32CoeffFor4SBs[18 et 22] = 0x1F91CA46 */
#define WIND_4_SUBBANDS_2_3 (SINT32)0xFC4F91D4  /* gas32CoeffFor4SBs[14 et 26] = 0xFC4F91D4 */
#define WIND_4_SUBBANDS_2_4 (SINT32)0x003D239B  /* gas32CoeffFor4SBs[6 et 34] = 0x003D239B */
#define WIND_4_SUBBANDS_3_0 (SINT32)0x00599403  /* gas32CoeffFor4SBs[3 et 37] = 0x00599403 */
#define WIND_4_SUBBANDS_3_1 (SINT32)0x041EEE40  /* gas32CoeffFor4SBs[11 et 29] = 0x041EEE40 */
#define WIND_4_SUBBANDS_3_2 (SINT32)0x2412F251  /* gas32CoeffFor4SBs[19 et 21] = 0x2412F251 */
#define WIND_4_SUBBANDS_3_3 (SINT32)0x00C8F2BC  /* gas32CoeffFor4SBs[13 et 27] = 0x00C8F2BC */
#define WIND_4_SUBBANDS_3_4 (SINT32)0x007F88E4  /* gas32CoeffFor4SBs[5 et 35] = 0x007F88E4 */
#define WIND_4_SUBBANDS_4_0 (SINT32)0x007DBCC8  /* gas32CoeffFor4SBs[4 et 36] = 0x007DBCC8 */
#define WIND_4_SUBBANDS_4_1 (SINT32)0x034FEE2C  /* gas32CoeffFor4SBs[12 et 28] = 0x034FEE2C */
#define WIND_4_SUBBANDS_4_2 (SINT32)0x25AC1FF2  /* gas32CoeffFor4SBs[20] = 0x25AC1FF2 */

#define WIND_8_SUBBANDS_0_1 (SINT32)0x00B97348  /* 16 0x00B97348 */
#define WIND_8_SUBBANDS_0_2 (SINT32)0x08B4307A  /* 32 0x08B4307A */
#define WIND_8_SUBBANDS_1_0 (SINT32)0x00052173  /* 1 et 79 = 0x00052173 */
#define WIND_8_SUBBANDS_1_1 (SINT32)0x01071B96  /* 17 et 63 = 0x01071B96 */
#define WIND_8_SUBBANDS_1_2 (SINT32)0x0A9F3E9A  /* 33 et 47 = 0x0A9F3E9A*/
#define WIND_8_SUBBANDS_1_3 (SINT32)0xF9312891  /* 31 et 49 = 0xF9312891 */
#define WIND_8_SUBBANDS_1_4 (SINT32)0xFF8D6793  /* 15 et 65 = 0xFF8D6793 */
#define WIND_8_SUBBANDS_2_0 (SINT32)0x000B3F71  /* 2 et 78 = 0x000B3F71 */
#define WIND_8_SUBBANDS_2_1 (SINT32)0x0156B3CA  /* 18 et 62 = 0x0156B3CA */
#define WIND_8_SUBBANDS_2_2 (SINT32)0x0C7D59B6  /* 34 et 46 = 0x0C7D59B6 */
#define WIND_8_SUBBANDS_2_3 (SINT32)0xFAFF95FC  /* 30 et 50 = 0xFAFF95FC */
#define WIND_8_SUBBANDS_2_4 (SINT32)0xFFC9F10E  /* 14 et 66 = 0xFFC9F10E */
#define WIND_8_SUBBANDS_3_0 (SINT32)0x00122C7D  /* 3 et 77 = 0x00122C7D*/
#define WIND_8_SUBBANDS_3_1 (SINT32)0x01A1B38B  /* 19 et 61 = 0x01A1B38B */
#define WIND_8_SUBBANDS_3_2 (SINT32)0x0E3BB16F  /* 35 et 45 = 0x0E3BB16F */
#define WIND_8_SUBBANDS_3_3 (SINT32)0xFCA86E7E  /* 29 et 51 = 0xFCA86E7E */
#define WIND_8_SUBBANDS_3_4 (SINT32)0xFFFA2413  /* 13 et 67 = 0xFFFA2413 */
#define WIND_8_SUBBANDS_4_0 (SINT32)0x001AFF89  /* 4 et 66 = 0x001AFF89 */
#define WIND_8_SUBBANDS_4_1 (SINT32)0x01E0224C  /* 20 et 60 = 0x01E0224C */
#define WIND_8_SUBBANDS_4_2 (SINT32)0x0FC721F9  /* 36 et 44 = 0x0FC721F9 */
#define WIND_8_SUBBANDS_4_3 (SINT32)0xFE20435D  /* 28 et 52 = 0xFE20435D */
#define WIND_8_SUBBANDS_4_4 (SINT32)0x001D8FD2  /* 12 et 68 = 0x001D8FD2 */
#define WIND_8_SUBBANDS_5_0 (SINT32)0x00255A62  /* 5 et 75 = 0x00255A62 */
#define WIND_8_SUBBANDS_5_1 (SINT32)0x0209291F  /* 21 et 59 = 0x0209291F */
#define WIND_8_SUBBANDS_5_2 (SINT32)0x110ECEF0  /* 37 et 43 = 0x110ECEF0 */
#define WIND_8_SUBBANDS_5_3 (SINT32)0xFF5EEB73  /* 27 et  53 = 0xFF5EEB73 */
#define WIND_8_SUBBANDS_5_4 (SINT32)0x0034F8B6  /* 11 et 69 = 0x0034F8B6 */
#define WIND_8_SUBBANDS_6_0 (SINT32)0x003060F4  /* 6 et 74 = 0x003060F4 */
#define WIND_8_SUBBANDS_6_1 (SINT32)0x02138653  /* 22 et 58 = 0x02138653 */
#define WIND_8_SUBBANDS_6_2 (SINT32)0x120435FA  /* 38 et 42 = 0x120435FA */
#define WIND_8_SUBBANDS_6_3 (SINT32)0x005FD0FF  /* 26 et 54 = 0x005FD0FF */
#define WIND_8_SUBBANDS_6_4 (SINT32)0x00415B75  /* 10 et 70 = 0x00415B75 */
#define WIND_8_SUBBANDS_7_0 (SINT32)0x003A72E7  /* 7 et 73 = 0x003A72E7 */
#define WIND_8_SUBBANDS_7_1 (SINT32)0x01F5F424  /* 23 et 57 = 0x01F5F424 */
#define WIND_8_SUBBANDS_7_2 (SINT32)0x129C226F  /* 39 et 41 = 0x129C226F */
#define WIND_8_SUBBANDS_7_3 (SINT32)0x01223EBA  /* 25 et 55 = 0x01223EBA */
#define WIND_8_SUBBANDS_7_4 (SINT32)0x0044EF48  /* 9 et 71 = 0x0044EF48 */
#define WIND_8_SUBBANDS_8_0 (SINT32)0x0041EC6A  /* 8 et 72 = 0x0041EC6A */
#define WIND_8_SUBBANDS_8_1 (SINT32)0x01A7ECEF  /* 24 et 56 = 0x01A7ECEF */
#define WIND_8_SUBBANDS_8_2 (SINT32)0x12CF6C75  /* 40 = 0x12CF6C75 */
#else
#define WIND_4_SUBBANDS_0_1 (SINT16)0x0166  /* gas32CoeffFor4SBs[8] = -gas32CoeffFor4SBs[32] = 0x01659F45 */
#define WIND_4_SUBBANDS_0_2 (SINT16)0x115B  /* gas32CoeffFor4SBs[16] = -gas32CoeffFor4SBs[24] = 0x115B1ED2 */
#define WIND_4_SUBBANDS_1_0 (SINT16)0x0012  /* gas32CoeffFor4SBs[1 et 39] = 0x001194E6 */
#define WIND_4_SUBBANDS_1_1 (SINT16)0x029E  /* gas32CoeffFor4SBs[9 et 31] = 0x029DBAA3 */
#define WIND_4_SUBBANDS_1_2 (SINT16)0x18F5  /* gas32CoeffFor4SBs[17 et 23] = 0x18F55C90 */
#define WIND_4_SUBBANDS_1_3 (SINT16)0xF610  /* gas32CoeffFor4SBs[15 et 25] = 0xF60FAF37 */
#define WIND_4_SUBBANDS_1_4 (SINT16)0xFF9C  /* gas32CoeffFor4SBs[7 et 33] = 0xFF9BB9D5 */
#define WIND_4_SUBBANDS_2_0 (SINT16)0x0031  /* gas32CoeffFor4SBs[2 et 38] = 0x0030E2D3 */
#define WIND_4_SUBBANDS_2_1 (SINT16)0x03B2  /* gas32CoeffFor4SBs[10 et 30] = 0x03B23341 */
#define WIND_4_SUBBANDS_2_2 (SINT16)0x1F91  /* gas32CoeffFor4SBs[18 et 22] = 0x1F91CA46 */
#define WIND_4_SUBBANDS_2_3 (SINT16)0xFC50  /* gas32CoeffFor4SBs[14 et 26] = 0xFC4F91D4 */
#define WIND_4_SUBBANDS_2_4 (SINT16)0x003D  /* gas32CoeffFor4SBs[6 et 34] = 0x003D239B */
#define WIND_4_SUBBANDS_3_0 (SINT16)0x005A  /* gas32CoeffFor4SBs[3 et 37] = 0x00599403 */
#define WIND_4_SUBBANDS_3_1 (SINT16)0x041F  /* gas32CoeffFor4SBs[11 et 29] = 0x041EEE40 */
#define WIND_4_SUBBANDS_3_2 (SINT16)0x2413  /* gas32CoeffFor4SBs[19 et 21] = 0x2412F251 */
#define WIND_4_SUBBANDS_3_3 (SINT16)0x00C9  /* gas32CoeffFor4SBs[13 et 27] = 0x00C8F2BC */
#define WIND_4_SUBBANDS_3_4 (SINT16)0x0080  /* gas32CoeffFor4SBs[5 et 35] = 0x007F88E4 */
#define WIND_4_SUBBANDS_4_0 (SINT16)0x007E  /* gas32CoeffFor4SBs[4 et 36] = 0x007DBCC8 */
#define WIND_4_SUBBANDS_4_1 (SINT16)0x0350  /* gas32CoeffFor4SBs[12 et 28] = 0x034FEE2C */
#define WIND_4_SUBBANDS_4_2 (SINT16)0x25AC  /* gas32CoeffFor4SBs[20] = 25AC1FF2 */

#define WIND_8_SUBBANDS_0_1 (SINT16)0x00B9  /* 16 0x12CF6C75 */
#define WIND_8_SUBBANDS_0_2 (SINT16)0x08B4  /* 32 0x08B4307A */
#define WIND_8_SUBBANDS_1_0 (SINT16)0x0005  /* 1 et 79 = 0x00052173 */
#define WIND_8_SUBBANDS_1_1 (SINT16)0x0107  /* 17 et 63 = 0x01071B96 */
#define WIND_8_SUBBANDS_1_2 (SINT16)0x0A9F  /* 33 et 47 = 0x0A9F3E9A*/
#define WIND_8_SUBBANDS_1_3 (SINT16)0xF931  /* 31 et 49 = 0xF9312891 */
#define WIND_8_SUBBANDS_1_4 (SINT16)0xFF8D  /* 15 et 65 = 0xFF8D6793 */
#define WIND_8_SUBBANDS_2_0 (SINT16)0x000B  /* 2 et 78 = 0x000B3F71 */
#define WIND_8_SUBBANDS_2_1 (SINT16)0x0157  /* 18 et 62 = 0x0156B3CA */
#define WIND_8_SUBBANDS_2_2 (SINT16)0x0C7D  /* 34 et 46 = 0x0C7D59B6 */
#define WIND_8_SUBBANDS_2_3 (SINT16)0xFB00  /* 30 et 50 = 0xFAFF95FC */
#define WIND_8_SUBBANDS_2_4 (SINT16)0xFFCA  /* 14 et 66 = 0xFFC9F10E */
#define WIND_8_SUBBANDS_3_0 (SINT16)0x0012  /* 3 et 77 = 0x00122C7D*/
#define WIND_8_SUBBANDS_3_1 (SINT16)0x01A2  /* 19 et 61 = 0x01A1B38B */
#define WIND_8_SUBBANDS_3_2 (SINT16)0x0E3C  /* 35 et 45 = 0x0E3BB16F */
#define WIND_8_SUBBANDS_3_3 (SINT16)0xFCA8  /* 29 et 51 = 0xFCA86E7E */
#define WIND_8_SUBBANDS_3_4 (SINT16)0xFFFA  /* 13 et 67 = 0xFFFA2413 */
#define WIND_8_SUBBANDS_4_0 (SINT16)0x001B  /* 4 et 66 = 0x001AFF89 */
#define WIND_8_SUBBANDS_4_1 (SINT16)0x01E0  /* 20 et 60 = 0x01E0224C */
#define WIND_8_SUBBANDS_4_2 (SINT16)0x0FC7  /* 36 et 44 = 0x0FC721F9 */
#define WIND_8_SUBBANDS_4_3 (SINT16)0xFE20  /* 28 et 52 = 0xFE20435D */
#define WIND_8_SUBBANDS_4_4 (SINT16)0x001E  /* 12 et 68 = 0x001D8FD2 */
#define WIND_8_SUBBANDS_5_0 (SINT16)0x0025  /* 5 et 75 = 0x00255A62 */
#define WIND_8_SUBBANDS_5_1 (SINT16)0x0209  /* 21 et 59 = 0x0209291F */
#define WIND_8_SUBBANDS_5_2 (SINT16)0x110F  /* 37 et 43 = 0x110ECEF0 */
#define WIND_8_SUBBANDS_5_3 (SINT16)0xFF5F  /* 27 et  53 = 0xFF5EEB73 */
#define WIND_8_SUBBANDS_5_4 (SINT16)0x0035  /* 11 et 69 = 0x0034F8B6 */
#define WIND_8_SUBBANDS_6_0 (SINT16)0x0030  /* 6 et 74 = 0x003060F4 */
#define WIND_8_SUBBANDS_6_1 (SINT16)0x0214  /* 22 et 58 = 0x02138653 */
#define WIND_8_SUBBANDS_6_2 (SINT16)0x1204  /* 38 et 42 = 0x120435FA */
#define WIND_8_SUBBANDS_6_3 (SINT16)0x0060  /* 26 et 54 = 0x005FD0FF */
#define WIND_8_SUBBANDS_6_4 (SINT16)0x0041  /* 10 et 70 = 0x00415B75 */
#define WIND_8_SUBBANDS_7_0 (SINT16)0x003A  /* 7 et 73 = 0x003A72E7 */
#define WIND_8_SUBBANDS_7_1 (SINT16)0x01F6  /* 23 et 57 = 0x01F5F424 */
#define WIND_8_SUBBANDS_7_2 (SINT16)0x129C  /* 39 et 41 = 0x129C226F */
#define WIND_8_SUBBANDS_7_3 (SINT16)0x0122  /* 25 et 55 = 0x01223EBA */
#define WIND_8_SUBBANDS_7_4 (SINT16)0x0045  /* 9 et 71 = 0x0044EF48 */
#define WIND_8_SUBBANDS_8_0 (SINT16)0x0042  /* 8 et 72 = 0x0041EC6A */
#define WIND_8_SUBBANDS_8_1 (SINT16)0x01A8  /* 24 et 56 = 0x01A7ECEF */
#define WIND_8_SUBBANDS_8_2 (SINT16)0x12CF  /* 40 = 0x12CF6C75 */
#endif

#if (SBC_USE_ARM_PRAGMA==TRUE)
#pragma arm section zidata = "sbc_s32_analysis_section"
#endif
static SINT32   s32DCTY[16]  = {0};
static SINT32   s32X[ENC_VX_BUFFER_SIZE / 2];
static SINT16   *s16X = (SINT16 *) s32X;   /* s16X must be 32 bits aligned cf  SHIFTUP_X8_2*/
#if (SBC_USE_ARM_PRAGMA==TRUE)
#pragma arm section zidata
#endif

/* This macro is for 4 subbands */
#define SHIFTUP_X4                                                               \
{                                                                                   \
    ps32X=(SINT32 *)(s16X+EncMaxShiftCounter+38);                                 \
    for (i=0;i<9;i++)                                                               \
    {                                                                               \
        *ps32X=*(ps32X-2-(ShiftCounter>>1));  ps32X--;                                 \
        *ps32X=*(ps32X-2-(ShiftCounter>>1));  ps32X--;                                 \
    }                                                                               \
}
#define SHIFTUP_X4_2                                                              \
{                                                                                   \
    ps32X=(SINT32 *)(s16X+EncMaxShiftCounter+38);                                   \
    ps32X2=(SINT32 *)(s16X+(EncMaxShiftCounter<<1)+78);                             \
    for (i=0;i<9;i++)                                                               \
    {                                                                               \
        *ps32X=*(ps32X-2-(ShiftCounter>>1));  *(ps32X2)=*(ps32X2-2-(ShiftCounter>>1)); ps32X--;  ps32X2--;                     \
        *ps32X=*(ps32X-2-(ShiftCounter>>1));  *(ps32X2)=*(ps32X2-2-(ShiftCounter>>1)); ps32X--;  ps32X2--;                     \
    }                                                                               \
}

/* This macro is for 8 subbands */
#define SHIFTUP_X8                                                               \
{                                                                                   \
    ps32X=(SINT32 *)(s16X+EncMaxShiftCounter+78);                                 \
    for (i=0;i<9;i++)                                                               \
    {                                                                               \
        *ps32X=*(ps32X-4-(ShiftCounter>>1));  ps32X--;                                 \
        *ps32X=*(ps32X-4-(ShiftCounter>>1));  ps32X--;                                 \
        *ps32X=*(ps32X-4-(ShiftCounter>>1));  ps32X--;                                 \
        *ps32X=*(ps32X-4-(ShiftCounter>>1));  ps32X--;                                 \
    }                                                                               \
}
#define SHIFTUP_X8_2                                                               \
{                                                                                   \
    ps32X=(SINT32 *)(s16X+EncMaxShiftCounter+78);                                   \
    ps32X2=(SINT32 *)(s16X+(EncMaxShiftCounter<<1)+158);                             \
    for (i=0;i<9;i++)                                                               \
    {                                                                               \
        *ps32X=*(ps32X-4-(ShiftCounter>>1));  *(ps32X2)=*(ps32X2-4-(ShiftCounter>>1)); ps32X--;  ps32X2--;                     \
        *ps32X=*(ps32X-4-(ShiftCounter>>1));  *(ps32X2)=*(ps32X2-4-(ShiftCounter>>1)); ps32X--;  ps32X2--;                     \
        *ps32X=*(ps32X-4-(ShiftCounter>>1));  *(ps32X2)=*(ps32X2-4-(ShiftCounter>>1)); ps32X--;  ps32X2--;                     \
        *ps32X=*(ps32X-4-(ShiftCounter>>1));  *(ps32X2)=*(ps32X2-4-(ShiftCounter>>1)); ps32X--;  ps32X2--;                     \
    }                                                                               \
}

#if (SBC_ARM_ASM_OPT==TRUE)
#define WINDOW_ACCU_8_0 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_8_SUBBANDS_0_1,(s16X[ChOffset+16]-s16X[ChOffset+64]);\
        MLA s32Hi,WIND_8_SUBBANDS_0_2,(s16X[ChOffset+32]-s16X[ChOffset+48]),s32Hi;\
        MOV s32DCTY[0],s32Hi;\
    }\
}
#define WINDOW_ACCU_8_1_15 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_8_SUBBANDS_1_0,s16X[ChOffset+1];\
        MUL s32Hi2,WIND_8_SUBBANDS_1_0,s16X[ChOffset+64+15];\
        MLA s32Hi,WIND_8_SUBBANDS_1_1,s16X[ChOffset+16+1],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_1_1,s16X[ChOffset+48+15],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_1_2,s16X[ChOffset+32+1],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_1_2,s16X[ChOffset+32+15],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_1_3,s16X[ChOffset+48+1],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_1_3,s16X[ChOffset+16+15],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_1_4,s16X[ChOffset+64+1],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_1_4,s16X[ChOffset+15],s32Hi2;\
        MOV s32DCTY[1],s32Hi;\
        MOV s32DCTY[15],s32Hi2;\
    }\
}
#define WINDOW_ACCU_8_2_14 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_8_SUBBANDS_2_0,s16X[ChOffset+2];\
        MUL s32Hi2,WIND_8_SUBBANDS_2_0,s16X[ChOffset+64+14];\
        MLA s32Hi,WIND_8_SUBBANDS_2_1,s16X[ChOffset+16+2],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_2_1,s16X[ChOffset+48+14],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_2_2,s16X[ChOffset+32+2],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_2_2,s16X[ChOffset+32+14],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_2_3,s16X[ChOffset+48+2],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_2_3,s16X[ChOffset+16+14],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_2_4,s16X[ChOffset+64+2],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_2_4,s16X[ChOffset+14],s32Hi2;\
        MOV s32DCTY[2],s32Hi;\
        MOV s32DCTY[14],s32Hi2;\
    }\
}
#define WINDOW_ACCU_8_3_13 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_8_SUBBANDS_3_0,s16X[ChOffset+3];\
        MUL s32Hi2,WIND_8_SUBBANDS_3_0,s16X[ChOffset+64+13];\
        MLA s32Hi,WIND_8_SUBBANDS_3_1,s16X[ChOffset+16+3],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_3_1,s16X[ChOffset+48+13],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_3_2,s16X[ChOffset+32+3],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_3_2,s16X[ChOffset+32+13],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_3_3,s16X[ChOffset+48+3],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_3_3,s16X[ChOffset+16+13],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_3_4,s16X[ChOffset+64+3],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_3_4,s16X[ChOffset+13],s32Hi2;\
        MOV s32DCTY[3],s32Hi;\
        MOV s32DCTY[13],s32Hi2;\
    }\
}
#define WINDOW_ACCU_8_4_12 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_8_SUBBANDS_4_0,s16X[ChOffset+4];\
        MUL s32Hi2,WIND_8_SUBBANDS_4_0,s16X[ChOffset+64+12];\
        MLA s32Hi,WIND_8_SUBBANDS_4_1,s16X[ChOffset+16+4],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_4_1,s16X[ChOffset+48+12],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_4_2,s16X[ChOffset+32+4],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_4_2,s16X[ChOffset+32+12],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_4_3,s16X[ChOffset+48+4],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_4_3,s16X[ChOffset+16+12],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_4_4,s16X[ChOffset+64+4],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_4_4,s16X[ChOffset+12],s32Hi2;\
        MOV s32DCTY[4],s32Hi;\
        MOV s32DCTY[12],s32Hi2;\
    }\
}
#define WINDOW_ACCU_8_5_11 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_8_SUBBANDS_5_0,s16X[ChOffset+5];\
        MUL s32Hi2,WIND_8_SUBBANDS_5_0,s16X[ChOffset+64+11];\
        MLA s32Hi,WIND_8_SUBBANDS_5_1,s16X[ChOffset+16+5],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_5_1,s16X[ChOffset+48+11],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_5_2,s16X[ChOffset+32+5],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_5_2,s16X[ChOffset+32+11],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_5_3,s16X[ChOffset+48+5],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_5_3,s16X[ChOffset+16+11],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_5_4,s16X[ChOffset+64+5],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_5_4,s16X[ChOffset+11],s32Hi2;\
        MOV s32DCTY[5],s32Hi;\
        MOV s32DCTY[11],s32Hi2;\
    }\
}
#define WINDOW_ACCU_8_6_10 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_8_SUBBANDS_6_0,s16X[ChOffset+6];\
        MUL s32Hi2,WIND_8_SUBBANDS_6_0,s16X[ChOffset+64+10];\
        MLA s32Hi,WIND_8_SUBBANDS_6_1,s16X[ChOffset+16+6],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_6_1,s16X[ChOffset+48+10],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_6_2,s16X[ChOffset+32+6],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_6_2,s16X[ChOffset+32+10],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_6_3,s16X[ChOffset+48+6],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_6_3,s16X[ChOffset+16+10],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_6_4,s16X[ChOffset+64+6],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_6_4,s16X[ChOffset+10],s32Hi2;\
        MOV s32DCTY[6],s32Hi;\
        MOV s32DCTY[10],s32Hi2;\
    }\
}
#define WINDOW_ACCU_8_7_9 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_8_SUBBANDS_7_0,s16X[ChOffset+7];\
        MUL s32Hi2,WIND_8_SUBBANDS_7_0,s16X[ChOffset+64+9];\
        MLA s32Hi,WIND_8_SUBBANDS_7_1,s16X[ChOffset+16+7],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_7_1,s16X[ChOffset+48+9],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_7_2,s16X[ChOffset+32+7],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_7_2,s16X[ChOffset+32+9],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_7_3,s16X[ChOffset+48+7],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_7_3,s16X[ChOffset+16+9],s32Hi2;\
        MLA s32Hi,WIND_8_SUBBANDS_7_4,s16X[ChOffset+64+7],s32Hi;\
        MLA s32Hi2,WIND_8_SUBBANDS_7_4,s16X[ChOffset+9],s32Hi2;\
        MOV s32DCTY[7],s32Hi;\
        MOV s32DCTY[9],s32Hi2;\
    }\
}
#define WINDOW_ACCU_8_8 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_8_SUBBANDS_8_0,(s16X[ChOffset+8]+s16X[ChOffset+8+64]);\
        MLA s32Hi,WIND_8_SUBBANDS_8_1,(s16X[ChOffset+8+16]+s16X[ChOffset+8+64]),s32Hi;\
        MLA s32Hi,WIND_8_SUBBANDS_8_2,s16X[ChOffset+8+32],s32Hi;\
        MOV s32DCTY[8],s32Hi;\
    }\
}
#define WINDOW_ACCU_4_0 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_4_SUBBANDS_0_1,(s16X[ChOffset+8]-s16X[ChOffset+32]);\
        MLA s32Hi,WIND_4_SUBBANDS_0_2,(s16X[ChOffset+16]-s16X[ChOffset+24]),s32Hi;\
        MOV s32DCTY[0],s32Hi;\
    }\
}
#define WINDOW_ACCU_4_1_7 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_4_SUBBANDS_1_0,s16X[ChOffset+1];\
        MUL s32Hi2,WIND_4_SUBBANDS_1_0,s16X[ChOffset+32+7];\
        MLA s32Hi,WIND_4_SUBBANDS_1_1,s16X[ChOffset+8+1],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_1_1,s16X[ChOffset+24+7],s32Hi2;\
        MLA s32Hi,WIND_4_SUBBANDS_1_2,s16X[ChOffset+16+1],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_1_2,s16X[ChOffset+16+7],s32Hi2;\
        MLA s32Hi,WIND_4_SUBBANDS_1_3,s16X[ChOffset+24+1],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_1_3,s16X[ChOffset+8+7],s32Hi2;\
        MLA s32Hi,WIND_4_SUBBANDS_1_4,s16X[ChOffset+32+1],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_1_4,s16X[ChOffset+7],s32Hi2;\
        MOV s32DCTY[1],s32Hi;\
        MOV s32DCTY[7],s32Hi2;\
    }\
}
#define WINDOW_ACCU_4_2_6 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_4_SUBBANDS_2_0,s16X[ChOffset+2];\
        MUL s32Hi2,WIND_4_SUBBANDS_2_0,s16X[ChOffset+32+6];\
        MLA s32Hi,WIND_4_SUBBANDS_2_1,s16X[ChOffset+8+2],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_2_1,s16X[ChOffset+24+6],s32Hi2;\
        MLA s32Hi,WIND_4_SUBBANDS_2_2,s16X[ChOffset+16+2],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_2_2,s16X[ChOffset+16+6],s32Hi2;\
        MLA s32Hi,WIND_4_SUBBANDS_2_3,s16X[ChOffset+24+2],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_2_3,s16X[ChOffset+8+6],s32Hi2;\
        MLA s32Hi,WIND_4_SUBBANDS_2_4,s16X[ChOffset+32+2],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_2_4,s16X[ChOffset+6],s32Hi2;\
        MOV s32DCTY[2],s32Hi;\
        MOV s32DCTY[6],s32Hi2;\
    }\
}
#define WINDOW_ACCU_4_3_5 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_4_SUBBANDS_3_0,s16X[ChOffset+3];\
        MUL s32Hi2,WIND_4_SUBBANDS_3_0,s16X[ChOffset+32+5];\
        MLA s32Hi,WIND_4_SUBBANDS_3_1,s16X[ChOffset+8+3],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_3_1,s16X[ChOffset+24+5],s32Hi2;\
        MLA s32Hi,WIND_4_SUBBANDS_3_2,s16X[ChOffset+16+3],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_3_2,s16X[ChOffset+16+5],s32Hi2;\
        MLA s32Hi,WIND_4_SUBBANDS_3_3,s16X[ChOffset+24+3],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_3_3,s16X[ChOffset+8+5],s32Hi2;\
        MLA s32Hi,WIND_4_SUBBANDS_3_4,s16X[ChOffset+32+3],s32Hi;\
        MLA s32Hi2,WIND_4_SUBBANDS_3_4,s16X[ChOffset+5],s32Hi2;\
        MOV s32DCTY[3],s32Hi;\
        MOV s32DCTY[5],s32Hi2;\
    }\
}
#define WINDOW_ACCU_4_4 \
{\
    __asm\
    {\
        MUL s32Hi,WIND_4_SUBBANDS_4_0,(s16X[ChOffset+4]+s16X[ChOffset+4+32]);\
        MLA s32Hi,WIND_4_SUBBANDS_4_1,(s16X[ChOffset+4+8]+s16X[ChOffset+4+24]),s32Hi;\
        MLA s32Hi,WIND_4_SUBBANDS_4_2,s16X[ChOffset+4+16],s32Hi;\
        MOV s32DCTY[4],s32Hi;\
    }\
}

#define WINDOW_PARTIAL_4 \
{\
    WINDOW_ACCU_4_0;     WINDOW_ACCU_4_1_7;\
    WINDOW_ACCU_4_2_6;   WINDOW_ACCU_4_3_5;\
    WINDOW_ACCU_4_4;\
}

#define WINDOW_PARTIAL_8 \
{\
    WINDOW_ACCU_8_0;     WINDOW_ACCU_8_1_15;\
    WINDOW_ACCU_8_2_14;  WINDOW_ACCU_8_3_13;\
    WINDOW_ACCU_8_4_12;  WINDOW_ACCU_8_5_11;\
    WINDOW_ACCU_8_6_10;  WINDOW_ACCU_8_7_9;\
    WINDOW_ACCU_8_8;\
}

#else
#if (SBC_IPAQ_OPT==TRUE)

#if (SBC_IS_64_MULT_IN_WINDOW_ACCU == TRUE)
#define WINDOW_ACCU_8_0 \
{\
    s64Temp=(SINT64)WIND_8_SUBBANDS_0_1*(SINT64)(s16X[ChOffset+16]-s16X[ChOffset+64]);\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_0_2*(SINT64)(s16X[ChOffset+32]-s16X[ChOffset+48]);\
    s32DCTY[0]=(SINT32)(s64Temp>>16);\
}
#define WINDOW_ACCU_8_1_15 \
{\
    s64Temp=(SINT64)WIND_8_SUBBANDS_1_0*(SINT64)s16X[ChOffset+1];\
    s64Temp2=(SINT64)WIND_8_SUBBANDS_1_0*(SINT64)s16X[ChOffset+64+15];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_1_1*(SINT64)s16X[ChOffset+16+1];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_1_1*(SINT64)s16X[ChOffset+48+15];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_1_2*(SINT64)s16X[ChOffset+32+1];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_1_2*(SINT64)s16X[ChOffset+32+15];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_1_3*(SINT64)s16X[ChOffset+48+1];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_1_3*(SINT64)s16X[ChOffset+16+15];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_1_4*(SINT64)s16X[ChOffset+64+1];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_1_4*(SINT64)s16X[ChOffset+15];\
    s32DCTY[1]=(SINT32)(s64Temp>>16);\
    s32DCTY[15]=(SINT32)(s64Temp2>>16);\
}
#define WINDOW_ACCU_8_2_14 \
{\
    s64Temp=(SINT64)WIND_8_SUBBANDS_2_0*(SINT64)s16X[ChOffset+2];\
    s64Temp2=(SINT64)WIND_8_SUBBANDS_2_0*(SINT64)s16X[ChOffset+64+14];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_2_1*(SINT64)s16X[ChOffset+16+2];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_2_1*(SINT64)s16X[ChOffset+48+14];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_2_2*(SINT64)s16X[ChOffset+32+2];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_2_2*(SINT64)s16X[ChOffset+32+14];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_2_3*(SINT64)s16X[ChOffset+48+2];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_2_3*(SINT64)s16X[ChOffset+16+14];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_2_4*(SINT64)s16X[ChOffset+64+2];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_2_4*(SINT64)s16X[ChOffset+14];\
    s32DCTY[2]=(SINT32)(s64Temp>>16);\
    s32DCTY[14]=(SINT32)(s64Temp2>>16);\
}
#define WINDOW_ACCU_8_3_13 \
{\
    s64Temp=(SINT64)WIND_8_SUBBANDS_3_0*(SINT64)s16X[ChOffset+3];\
    s64Temp2=(SINT64)WIND_8_SUBBANDS_3_0*(SINT64)s16X[ChOffset+64+13];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_3_1*(SINT64)s16X[ChOffset+16+3];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_3_1*(SINT64)s16X[ChOffset+48+13];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_3_2*(SINT64)s16X[ChOffset+32+3];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_3_2*(SINT64)s16X[ChOffset+32+13];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_3_3*(SINT64)s16X[ChOffset+48+3];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_3_3*(SINT64)s16X[ChOffset+16+13];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_3_4*(SINT64)s16X[ChOffset+64+3];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_3_4*(SINT64)s16X[ChOffset+13];\
    s32DCTY[3]=(SINT32)(s64Temp>>16);\
    s32DCTY[13]=(SINT32)(s64Temp2>>16);\
}
#define WINDOW_ACCU_8_4_12 \
{\
    s64Temp=(SINT64)WIND_8_SUBBANDS_4_0*(SINT64)s16X[ChOffset+4];\
    s64Temp2=(SINT64)WIND_8_SUBBANDS_4_0*(SINT64)s16X[ChOffset+64+12];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_4_1*(SINT64)s16X[ChOffset+16+4];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_4_1*(SINT64)s16X[ChOffset+48+12];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_4_2*(SINT64)s16X[ChOffset+32+4];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_4_2*(SINT64)s16X[ChOffset+32+12];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_4_3*(SINT64)s16X[ChOffset+48+4];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_4_3*(SINT64)s16X[ChOffset+16+12];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_4_4*(SINT64)s16X[ChOffset+64+4];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_4_4*(SINT64)s16X[ChOffset+12];\
    s32DCTY[4]=(SINT32)(s64Temp>>16);\
    s32DCTY[12]=(SINT32)(s64Temp2>>16);\
}
#define WINDOW_ACCU_8_5_11 \
{\
    s64Temp=(SINT64)WIND_8_SUBBANDS_5_0*(SINT64)s16X[ChOffset+5];\
    s64Temp2=(SINT64)WIND_8_SUBBANDS_5_0*(SINT64)s16X[ChOffset+64+11];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_5_1*(SINT64)s16X[ChOffset+16+5];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_5_1*(SINT64)s16X[ChOffset+48+11];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_5_2*(SINT64)s16X[ChOffset+32+5];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_5_2*(SINT64)s16X[ChOffset+32+11];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_5_3*(SINT64)s16X[ChOffset+48+5];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_5_3*(SINT64)s16X[ChOffset+16+11];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_5_4*(SINT64)s16X[ChOffset+64+5];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_5_4*(SINT64)s16X[ChOffset+11];\
    s32DCTY[5]=(SINT32)(s64Temp>>16);\
    s32DCTY[11]=(SINT32)(s64Temp2>>16);\
}
#define WINDOW_ACCU_8_6_10 \
{\
    s64Temp=(SINT64)WIND_8_SUBBANDS_6_0*(SINT64)s16X[ChOffset+6];\
    s64Temp2=(SINT64)WIND_8_SUBBANDS_6_0*(SINT64)s16X[ChOffset+64+10];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_6_1*(SINT64)s16X[ChOffset+16+6];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_6_1*(SINT64)s16X[ChOffset+48+10];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_6_2*(SINT64)s16X[ChOffset+32+6];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_6_2*(SINT64)s16X[ChOffset+32+10];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_6_3*(SINT64)s16X[ChOffset+48+6];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_6_3*(SINT64)s16X[ChOffset+16+10];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_6_4*(SINT64)s16X[ChOffset+64+6];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_6_4*(SINT64)s16X[ChOffset+10];\
    s32DCTY[6]=(SINT32)(s64Temp>>16);\
    s32DCTY[10]=(SINT32)(s64Temp2>>16);\
}
#define WINDOW_ACCU_8_7_9 \
{\
    s64Temp=(SINT64)WIND_8_SUBBANDS_7_0*(SINT64)s16X[ChOffset+7];\
    s64Temp2=(SINT64)WIND_8_SUBBANDS_7_0*(SINT64)s16X[ChOffset+64+9];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_7_1*(SINT64)s16X[ChOffset+16+7];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_7_1*(SINT64)s16X[ChOffset+48+9];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_7_2*(SINT64)s16X[ChOffset+32+7];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_7_2*(SINT64)s16X[ChOffset+32+9];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_7_3*(SINT64)s16X[ChOffset+48+7];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_7_3*(SINT64)s16X[ChOffset+16+9];\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_7_4*(SINT64)s16X[ChOffset+64+7];\
    s64Temp2+=(SINT64)WIND_8_SUBBANDS_7_4*(SINT64)s16X[ChOffset+9];\
    s32DCTY[7]=(SINT32)(s64Temp>>16);\
    s32DCTY[9]=(SINT32)(s64Temp2>>16);\
}
#define WINDOW_ACCU_8_8 \
{\
    s64Temp=(SINT64)WIND_8_SUBBANDS_8_0*(SINT64)(s16X[ChOffset+8]+s16X[ChOffset+64+8]);\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_8_1*(SINT64)(s16X[ChOffset+16+8]+s16X[ChOffset+48+8]);\
    s64Temp+=(SINT64)WIND_8_SUBBANDS_8_2*(SINT64)s16X[ChOffset+32+8];\
    s32DCTY[8]=(SINT32)(s64Temp>>16);\
}
#define WINDOW_ACCU_4_0 \
{\
    s64Temp=(SINT64)WIND_4_SUBBANDS_0_1*(SINT64)(s16X[ChOffset+8]-s16X[ChOffset+32]);\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_0_2*(SINT64)(s16X[ChOffset+16]-s16X[ChOffset+24]);\
    s32DCTY[0]=(SINT32)(s64Temp>>16);\
}
#define WINDOW_ACCU_4_1_7 \
{\
    s64Temp=(SINT64)WIND_4_SUBBANDS_1_0*(SINT64)s16X[ChOffset+1];\
    s64Temp2=(SINT64)WIND_4_SUBBANDS_1_0*(SINT64)s16X[ChOffset+32+7];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_1_1*(SINT64)s16X[ChOffset+8+1];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_1_1*(SINT64)s16X[ChOffset+24+7];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_1_2*(SINT64)s16X[ChOffset+16+1];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_1_2*(SINT64)s16X[ChOffset+16+7];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_1_3*(SINT64)s16X[ChOffset+24+1];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_1_3*(SINT64)s16X[ChOffset+8+7];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_1_4*(SINT64)s16X[ChOffset+32+1];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_1_4*(SINT64)s16X[ChOffset+7];\
    s32DCTY[1]=(SINT32)(s64Temp>>16);\
    s32DCTY[7]=(SINT32)(s64Temp2>>16);\
}
#define WINDOW_ACCU_4_2_6 \
{\
    s64Temp=(SINT64)WIND_4_SUBBANDS_2_0*(SINT64)s16X[ChOffset+2];\
    s64Temp2=(SINT64)WIND_4_SUBBANDS_2_0*(SINT64)s16X[ChOffset+32+6];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_2_1*(SINT64)s16X[ChOffset+8+2];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_2_1*(SINT64)s16X[ChOffset+24+6];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_2_2*(SINT64)s16X[ChOffset+16+2];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_2_2*(SINT64)s16X[ChOffset+16+6];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_2_3*(SINT64)s16X[ChOffset+24+2];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_2_3*(SINT64)s16X[ChOffset+8+6];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_2_4*(SINT64)s16X[ChOffset+32+2];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_2_4*(SINT64)s16X[ChOffset+6];\
    s32DCTY[2]=(SINT32)(s64Temp>>16);\
    s32DCTY[6]=(SINT32)(s64Temp2>>16);\
}
#define WINDOW_ACCU_4_3_5 \
{\
    s64Temp=(SINT64)WIND_4_SUBBANDS_3_0*(SINT64)s16X[ChOffset+3];\
    s64Temp2=(SINT64)WIND_4_SUBBANDS_3_0*(SINT64)s16X[ChOffset+32+5];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_3_1*(SINT64)s16X[ChOffset+8+3];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_3_1*(SINT64)s16X[ChOffset+24+5];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_3_2*(SINT64)s16X[ChOffset+16+3];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_3_2*(SINT64)s16X[ChOffset+16+5];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_3_3*(SINT64)s16X[ChOffset+24+3];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_3_3*(SINT64)s16X[ChOffset+8+5];\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_3_4*(SINT64)s16X[ChOffset+32+3];\
    s64Temp2+=(SINT64)WIND_4_SUBBANDS_3_4*(SINT64)s16X[ChOffset+5];\
    s32DCTY[3]=(SINT32)(s64Temp>>16);\
    s32DCTY[5]=(SINT32)(s64Temp2>>16);\
}

#define WINDOW_ACCU_4_4 \
{\
    s64Temp=(SINT64)WIND_4_SUBBANDS_4_0*(SINT64)(s16X[ChOffset+4]+s16X[ChOffset+4+32]);\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_4_1*(SINT64)(s16X[ChOffset+4+8]+s16X[ChOffset+4+24]);\
    s64Temp+=(SINT64)WIND_4_SUBBANDS_4_2*(SINT64)s16X[ChOffset+4+16];\
    s32DCTY[4]=(SINT32)(s64Temp>>16);\
}
#else /* SBC_IS_64_MULT_IN_WINDOW_ACCU == FALSE */
#define WINDOW_ACCU_8_0 \
{\
    s32Temp=(SINT32)WIND_8_SUBBANDS_0_1*(SINT32)(s16X[ChOffset+16]-s16X[ChOffset+64]);\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_0_2*(SINT32)(s16X[ChOffset+32]-s16X[ChOffset+48]);\
    s32DCTY[0]=(SINT32)s32Temp;\
}
#define WINDOW_ACCU_8_1_15 \
{\
    s32Temp=(SINT32)WIND_8_SUBBANDS_1_0*(SINT32)s16X[ChOffset+1];\
    s32Temp2=(SINT32)WIND_8_SUBBANDS_1_0*(SINT32)s16X[ChOffset+64+15];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_1_1*(SINT32)s16X[ChOffset+16+1];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_1_1*(SINT32)s16X[ChOffset+48+15];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_1_2*(SINT32)s16X[ChOffset+32+1];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_1_2*(SINT32)s16X[ChOffset+32+15];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_1_3*(SINT32)s16X[ChOffset+48+1];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_1_3*(SINT32)s16X[ChOffset+16+15];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_1_4*(SINT32)s16X[ChOffset+64+1];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_1_4*(SINT32)s16X[ChOffset+15];\
    s32DCTY[1]=(SINT32)s32Temp;\
    s32DCTY[15]=(SINT32)s32Temp2;\
}
#define WINDOW_ACCU_8_2_14 \
{\
    s32Temp=(SINT32)WIND_8_SUBBANDS_2_0*(SINT32)s16X[ChOffset+2];\
    s32Temp2=(SINT32)WIND_8_SUBBANDS_2_0*(SINT32)s16X[ChOffset+64+14];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_2_1*(SINT32)s16X[ChOffset+16+2];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_2_1*(SINT32)s16X[ChOffset+48+14];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_2_2*(SINT32)s16X[ChOffset+32+2];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_2_2*(SINT32)s16X[ChOffset+32+14];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_2_3*(SINT32)s16X[ChOffset+48+2];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_2_3*(SINT32)s16X[ChOffset+16+14];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_2_4*(SINT32)s16X[ChOffset+64+2];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_2_4*(SINT32)s16X[ChOffset+14];\
    s32DCTY[2]=(SINT32)s32Temp;\
    s32DCTY[14]=(SINT32)s32Temp2;\
}
#define WINDOW_ACCU_8_3_13 \
{\
    s32Temp=(SINT32)WIND_8_SUBBANDS_3_0*(SINT32)s16X[ChOffset+3];\
    s32Temp2=(SINT32)WIND_8_SUBBANDS_3_0*(SINT32)s16X[ChOffset+64+13];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_3_1*(SINT32)s16X[ChOffset+16+3];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_3_1*(SINT32)s16X[ChOffset+48+13];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_3_2*(SINT32)s16X[ChOffset+32+3];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_3_2*(SINT32)s16X[ChOffset+32+13];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_3_3*(SINT32)s16X[ChOffset+48+3];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_3_3*(SINT32)s16X[ChOffset+16+13];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_3_4*(SINT32)s16X[ChOffset+64+3];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_3_4*(SINT32)s16X[ChOffset+13];\
    s32DCTY[3]=(SINT32)s32Temp;\
    s32DCTY[13]=(SINT32)s32Temp2;\
}
#define WINDOW_ACCU_8_4_12 \
{\
    s32Temp=(SINT32)WIND_8_SUBBANDS_4_0*(SINT32)s16X[ChOffset+4];\
    s32Temp2=(SINT32)WIND_8_SUBBANDS_4_0*(SINT32)s16X[ChOffset+64+12];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_4_1*(SINT32)s16X[ChOffset+16+4];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_4_1*(SINT32)s16X[ChOffset+48+12];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_4_2*(SINT32)s16X[ChOffset+32+4];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_4_2*(SINT32)s16X[ChOffset+32+12];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_4_3*(SINT32)s16X[ChOffset+48+4];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_4_3*(SINT32)s16X[ChOffset+16+12];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_4_4*(SINT32)s16X[ChOffset+64+4];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_4_4*(SINT32)s16X[ChOffset+12];\
    s32DCTY[4]=(SINT32)s32Temp;\
    s32DCTY[12]=(SINT32)s32Temp2;\
}
#define WINDOW_ACCU_8_5_11 \
{\
    s32Temp=(SINT32)WIND_8_SUBBANDS_5_0*(SINT32)s16X[ChOffset+5];\
    s32Temp2=(SINT32)WIND_8_SUBBANDS_5_0*(SINT32)s16X[ChOffset+64+11];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_5_1*(SINT32)s16X[ChOffset+16+5];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_5_1*(SINT32)s16X[ChOffset+48+11];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_5_2*(SINT32)s16X[ChOffset+32+5];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_5_2*(SINT32)s16X[ChOffset+32+11];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_5_3*(SINT32)s16X[ChOffset+48+5];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_5_3*(SINT32)s16X[ChOffset+16+11];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_5_4*(SINT32)s16X[ChOffset+64+5];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_5_4*(SINT32)s16X[ChOffset+11];\
    s32DCTY[5]=(SINT32)s32Temp;\
    s32DCTY[11]=(SINT32)s32Temp2;\
}
#define WINDOW_ACCU_8_6_10 \
{\
    s32Temp=(SINT32)WIND_8_SUBBANDS_6_0*(SINT32)s16X[ChOffset+6];\
    s32Temp2=(SINT32)WIND_8_SUBBANDS_6_0*(SINT32)s16X[ChOffset+64+10];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_6_1*(SINT32)s16X[ChOffset+16+6];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_6_1*(SINT32)s16X[ChOffset+48+10];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_6_2*(SINT32)s16X[ChOffset+32+6];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_6_2*(SINT32)s16X[ChOffset+32+10];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_6_3*(SINT32)s16X[ChOffset+48+6];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_6_3*(SINT32)s16X[ChOffset+16+10];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_6_4*(SINT32)s16X[ChOffset+64+6];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_6_4*(SINT32)s16X[ChOffset+10];\
    s32DCTY[6]=(SINT32)s32Temp;\
    s32DCTY[10]=(SINT32)s32Temp2;\
}
#define WINDOW_ACCU_8_7_9 \
{\
    s32Temp=(SINT32)WIND_8_SUBBANDS_7_0*(SINT32)s16X[ChOffset+7];\
    s32Temp2=(SINT32)WIND_8_SUBBANDS_7_0*(SINT32)s16X[ChOffset+64+9];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_7_1*(SINT32)s16X[ChOffset+16+7];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_7_1*(SINT32)s16X[ChOffset+48+9];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_7_2*(SINT32)s16X[ChOffset+32+7];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_7_2*(SINT32)s16X[ChOffset+32+9];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_7_3*(SINT32)s16X[ChOffset+48+7];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_7_3*(SINT32)s16X[ChOffset+16+9];\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_7_4*(SINT32)s16X[ChOffset+64+7];\
    s32Temp2+=(SINT32)WIND_8_SUBBANDS_7_4*(SINT32)s16X[ChOffset+9];\
    s32DCTY[7]=(SINT32)s32Temp;\
    s32DCTY[9]=(SINT32)s32Temp2;\
}
#define WINDOW_ACCU_8_8 \
{\
    s32Temp=(SINT32)WIND_8_SUBBANDS_8_0*(SINT32)(s16X[ChOffset+8]+s16X[ChOffset+64+8]);\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_8_1*(SINT32)(s16X[ChOffset+16+8]+s16X[ChOffset+48+8]);\
    s32Temp+=(SINT32)WIND_8_SUBBANDS_8_2*(SINT32)s16X[ChOffset+32+8];\
    s32DCTY[8]=(SINT32)s32Temp;\
}
#define WINDOW_ACCU_4_0 \
{\
    s32Temp=(SINT32)WIND_4_SUBBANDS_0_1*(SINT32)(s16X[ChOffset+8]-s16X[ChOffset+32]);\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_0_2*(SINT32)(s16X[ChOffset+16]-s16X[ChOffset+24]);\
    s32DCTY[0]=(SINT32)(s32Temp);\
}
#define WINDOW_ACCU_4_1_7 \
{\
    s32Temp=(SINT32)WIND_4_SUBBANDS_1_0*(SINT32)s16X[ChOffset+1];\
    s32Temp2=(SINT32)WIND_4_SUBBANDS_1_0*(SINT32)s16X[ChOffset+32+7];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_1_1*(SINT32)s16X[ChOffset+8+1];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_1_1*(SINT32)s16X[ChOffset+24+7];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_1_2*(SINT32)s16X[ChOffset+16+1];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_1_2*(SINT32)s16X[ChOffset+16+7];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_1_3*(SINT32)s16X[ChOffset+24+1];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_1_3*(SINT32)s16X[ChOffset+8+7];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_1_4*(SINT32)s16X[ChOffset+32+1];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_1_4*(SINT32)s16X[ChOffset+7];\
    s32DCTY[1]=(SINT32)(s32Temp);\
    s32DCTY[7]=(SINT32)(s32Temp2);\
}
#define WINDOW_ACCU_4_2_6 \
{\
    s32Temp=(SINT32)WIND_4_SUBBANDS_2_0*(SINT32)s16X[ChOffset+2];\
    s32Temp2=(SINT32)WIND_4_SUBBANDS_2_0*(SINT32)s16X[ChOffset+32+6];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_2_1*(SINT32)s16X[ChOffset+8+2];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_2_1*(SINT32)s16X[ChOffset+24+6];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_2_2*(SINT32)s16X[ChOffset+16+2];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_2_2*(SINT32)s16X[ChOffset+16+6];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_2_3*(SINT32)s16X[ChOffset+24+2];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_2_3*(SINT32)s16X[ChOffset+8+6];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_2_4*(SINT32)s16X[ChOffset+32+2];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_2_4*(SINT32)s16X[ChOffset+6];\
    s32DCTY[2]=(SINT32)(s32Temp);\
    s32DCTY[6]=(SINT32)(s32Temp2);\
}
#define WINDOW_ACCU_4_3_5 \
{\
    s32Temp=(SINT32)WIND_4_SUBBANDS_3_0*(SINT32)s16X[ChOffset+3];\
    s32Temp2=(SINT32)WIND_4_SUBBANDS_3_0*(SINT32)s16X[ChOffset+32+5];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_3_1*(SINT32)s16X[ChOffset+8+3];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_3_1*(SINT32)s16X[ChOffset+24+5];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_3_2*(SINT32)s16X[ChOffset+16+3];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_3_2*(SINT32)s16X[ChOffset+16+5];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_3_3*(SINT32)s16X[ChOffset+24+3];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_3_3*(SINT32)s16X[ChOffset+8+5];\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_3_4*(SINT32)s16X[ChOffset+32+3];\
    s32Temp2+=(SINT32)WIND_4_SUBBANDS_3_4*(SINT32)s16X[ChOffset+5];\
    s32DCTY[3]=(SINT32)(s32Temp);\
    s32DCTY[5]=(SINT32)(s32Temp2);\
}

#define WINDOW_ACCU_4_4 \
{\
    s32Temp=(SINT32)WIND_4_SUBBANDS_4_0*(SINT32)(s16X[ChOffset+4]+s16X[ChOffset+4+32]);\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_4_1*(SINT32)(s16X[ChOffset+4+8]+s16X[ChOffset+4+24]);\
    s32Temp+=(SINT32)WIND_4_SUBBANDS_4_2*(SINT32)s16X[ChOffset+4+16];\
    s32DCTY[4]=(SINT32)(s32Temp);\
}
#endif
#define WINDOW_PARTIAL_4 \
{\
    WINDOW_ACCU_4_0;     WINDOW_ACCU_4_1_7;\
    WINDOW_ACCU_4_2_6;   WINDOW_ACCU_4_3_5;\
    WINDOW_ACCU_4_4;\
}

#define WINDOW_PARTIAL_8 \
{\
    WINDOW_ACCU_8_0;         WINDOW_ACCU_8_1_15;\
    WINDOW_ACCU_8_2_14;      WINDOW_ACCU_8_3_13;\
    WINDOW_ACCU_8_4_12;      WINDOW_ACCU_8_5_11;\
    WINDOW_ACCU_8_6_10;      WINDOW_ACCU_8_7_9;\
    WINDOW_ACCU_8_8;\
}
#else
#if (SBC_IS_64_MULT_IN_WINDOW_ACCU == TRUE)
#define WINDOW_ACCU_4(i) \
{\
    s64Temp=((SINT64)gas32CoeffFor4SBs[i] * (SINT64)s16X[ChOffset+i]);                                    \
    s64Temp+=((SINT64)gas32CoeffFor4SBs[(i+8)] * (SINT64)s16X[ChOffset+i+8]);                                    \
    s64Temp+=((SINT64)gas32CoeffFor4SBs[(i+16)] * (SINT64)s16X[ChOffset+i+16]);                                    \
    s64Temp+=((SINT64)gas32CoeffFor4SBs[(i+24)] * (SINT64)s16X[ChOffset+i+24]);                                    \
    s64Temp+=((SINT64)gas32CoeffFor4SBs[(i+32)] * (SINT64)s16X[ChOffset+i+32]);                                    \
    s32DCTY[i]=(SINT32)(s64Temp>>16);\
    /*printf("s32DCTY4: 0x%x \n", s32DCTY[i]);*/\
}
#else
#define WINDOW_ACCU_4(i) \
{\
    s32DCTY[i]=(gas32CoeffFor4SBs[i * 2] * s16X[ChOffset+i])                                    \
            +  (((SINT32)(UINT16)(gas32CoeffFor4SBs[(i * 2) + 1]) * s16X[ChOffset+i]) >> 16);   \
    s32DCTY[i]+=(gas32CoeffFor4SBs[(i+8) * 2] * s16X[ChOffset+i+8])                                    \
            +  (((SINT32)(UINT16)(gas32CoeffFor4SBs[((i+8) * 2) + 1]) * s16X[ChOffset+i+8]) >> 16);   \
    s32DCTY[i]+=(gas32CoeffFor4SBs[(i+16) * 2] * s16X[ChOffset+i+16])                                    \
            +  (((SINT32)(UINT16)(gas32CoeffFor4SBs[((i+16) * 2) + 1]) * s16X[ChOffset+i+16]) >> 16);   \
    s32DCTY[i]+=(gas32CoeffFor4SBs[(i+24) * 2] * s16X[ChOffset+i+24])                                    \
            +  (((SINT32)(UINT16)(gas32CoeffFor4SBs[((i+24) * 2) + 1]) * s16X[ChOffset+i+24]) >> 16);   \
    s32DCTY[i]+=(gas32CoeffFor4SBs[(i+32) * 2] * s16X[ChOffset+i+32])                                    \
            +  (((SINT32)(UINT16)(gas32CoeffFor4SBs[((i+32) * 2) + 1]) * s16X[ChOffset+i+32]) >> 16);   \
}
#endif
#define WINDOW_PARTIAL_4 \
{\
    WINDOW_ACCU_4(0);      WINDOW_ACCU_4(1);\
    WINDOW_ACCU_4(2);      WINDOW_ACCU_4(3);\
    WINDOW_ACCU_4(4);      WINDOW_ACCU_4(5);\
    WINDOW_ACCU_4(6);      WINDOW_ACCU_4(7);\
}

#if (SBC_IS_64_MULT_IN_WINDOW_ACCU == TRUE)
#define WINDOW_ACCU_8(i) \
{\
    s64Temp = ((((SINT64)gas32CoeffFor8SBs[i]      * (SINT64)s16X[ChOffset+i]   )));                 \
    s64Temp+= ((((SINT64)gas32CoeffFor8SBs[(i+16)] * (SINT64)s16X[ChOffset+i+16])));                 \
    s64Temp+= ((((SINT64)gas32CoeffFor8SBs[(i+32)] * (SINT64)s16X[ChOffset+i+32])));                 \
    s64Temp+= ((((SINT64)gas32CoeffFor8SBs[(i+48)] * (SINT64)s16X[ChOffset+i+48])));                 \
    s64Temp+= ((((SINT64)gas32CoeffFor8SBs[(i+64)] * (SINT64)s16X[ChOffset+i+64])));                 \
    /*printf("s32DCTY8: %d= 0x%x * %d\n", s32DCTY[i], gas32CoeffFor8SBs[i], s16X[ChOffset+i]);*/     \
    s32DCTY[i]=(SINT32)(s64Temp>>16);\
}
#else
#define WINDOW_ACCU_8(i) \
{\
    s32DCTY[i]=(gas32CoeffFor8SBs[i * 2] * s16X[ChOffset+i])                                    \
            +  (((SINT32)(UINT16)(gas32CoeffFor8SBs[(i * 2) + 1]) * s16X[ChOffset+i]) >> 16);   \
    s32DCTY[i]+=(gas32CoeffFor8SBs[(i+16) * 2] * s16X[ChOffset+i+16])                                    \
            +  (((SINT32)(UINT16)(gas32CoeffFor8SBs[((i+16) * 2) + 1]) * s16X[ChOffset+i+16]) >> 16);   \
    s32DCTY[i]+=(gas32CoeffFor8SBs[(i+32) * 2] * s16X[ChOffset+i+32])                                    \
            +  (((SINT32)(UINT16)(gas32CoeffFor8SBs[((i+32) * 2) + 1]) * s16X[ChOffset+i+32]) >> 16);   \
    s32DCTY[i]+=(gas32CoeffFor8SBs[(i+48) * 2] * s16X[ChOffset+i+48])                                    \
            +  (((SINT32)(UINT16)(gas32CoeffFor8SBs[((i+48) * 2) + 1]) * s16X[ChOffset+i+48]) >> 16);   \
    s32DCTY[i]+=(gas32CoeffFor8SBs[(i+64) * 2] * s16X[ChOffset+i+64])                                    \
            +  (((SINT32)(UINT16)(gas32CoeffFor8SBs[((i+64) * 2) + 1]) * s16X[ChOffset+i+64]) >> 16);   \
    /*printf("s32DCTY8: %d = 0x%4x%4x * %d\n", s32DCTY[i], gas32CoeffFor8SBs[i * 2], (gas32CoeffFor8SBs[(i * 2) + 1]), s16X[ChOffset+i]);*/\
    /*s32DCTY[i]=(SINT32)(s64Temp>>16);*/\
}
#endif
#define WINDOW_PARTIAL_8 \
{\
    WINDOW_ACCU_8(0);      WINDOW_ACCU_8(1);\
    WINDOW_ACCU_8(2);      WINDOW_ACCU_8(3);\
    WINDOW_ACCU_8(4);      WINDOW_ACCU_8(5);\
    WINDOW_ACCU_8(6);      WINDOW_ACCU_8(7);\
    WINDOW_ACCU_8(8);      WINDOW_ACCU_8(9);\
    WINDOW_ACCU_8(10);     WINDOW_ACCU_8(11);\
    WINDOW_ACCU_8(12);     WINDOW_ACCU_8(13);\
    WINDOW_ACCU_8(14);     WINDOW_ACCU_8(15);\
}
#endif
#endif

static SINT16 ShiftCounter = 0;
extern SINT16 EncMaxShiftCounter;
/****************************************************************************
* SbcAnalysisFilter - performs Analysis of the input audio stream
*
* RETURNS : N/A
*/
void SbcAnalysisFilter4(SBC_ENC_PARAMS *pstrEncParams)
{
    SINT16 *ps16PcmBuf;
    SINT32 *ps32SbBuf;
    SINT32  s32Blk, s32Ch;
    SINT32  s32NumOfChannels, s32NumOfBlocks;
    SINT32 i, *ps32X, *ps32X2;
    SINT32 Offset, Offset2, ChOffset;
#if (SBC_ARM_ASM_OPT==TRUE)
    register SINT32 s32Hi, s32Hi2;
#else
#if (SBC_IPAQ_OPT==TRUE)
#if (SBC_IS_64_MULT_IN_WINDOW_ACCU == TRUE)
    register SINT64 s64Temp, s64Temp2;
#else
    register SINT32 s32Temp, s32Temp2;
#endif
#else

#if (SBC_IS_64_MULT_IN_WINDOW_ACCU == TRUE)
    SINT64 s64Temp;
#endif

#endif
#endif

    s32NumOfChannels = pstrEncParams->s16NumOfChannels;
    s32NumOfBlocks   = pstrEncParams->s16NumOfBlocks;

    ps16PcmBuf = pstrEncParams->ps16NextPcmBuffer;

    ps32SbBuf  = pstrEncParams->s32SbBuffer;
    Offset2 = (SINT32)(EncMaxShiftCounter + 40);
    for (s32Blk = 0; s32Blk < s32NumOfBlocks; s32Blk++) {
        Offset = (SINT32)(EncMaxShiftCounter - ShiftCounter);
        /* Store new samples */
        if (s32NumOfChannels == 1) {
            s16X[3 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
            s16X[2 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
            s16X[1 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
            s16X[0 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
        } else {
            s16X[3 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 3 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
            s16X[2 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 2 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
            s16X[1 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 1 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
            s16X[0 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 0 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
        }
        for (s32Ch = 0; s32Ch < s32NumOfChannels; s32Ch++) {
            ChOffset = s32Ch * Offset2 + Offset;

            WINDOW_PARTIAL_4

            SBC_FastIDCT4(s32DCTY, ps32SbBuf);

            ps32SbBuf += SUB_BANDS_4;
        }
        if (s32NumOfChannels == 1) {
            if (ShiftCounter >= EncMaxShiftCounter) {
                SHIFTUP_X4;
                ShiftCounter = 0;
            } else {
                ShiftCounter += SUB_BANDS_4;
            }
        } else {
            if (ShiftCounter >= EncMaxShiftCounter) {
                SHIFTUP_X4_2;
                ShiftCounter = 0;
            } else {
                ShiftCounter += SUB_BANDS_4;
            }
        }
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// */
void SbcAnalysisFilter8 (SBC_ENC_PARAMS *pstrEncParams)
{
    SINT16 *ps16PcmBuf;
    SINT32 *ps32SbBuf;
    SINT32  s32Blk, s32Ch;                                    /* counter for block*/
    SINT32 Offset, Offset2;
    SINT32  s32NumOfChannels, s32NumOfBlocks;
    SINT32 i, *ps32X, *ps32X2;
    SINT32 ChOffset;
#if (SBC_ARM_ASM_OPT==TRUE)
    register SINT32 s32Hi, s32Hi2;
#else
#if (SBC_IPAQ_OPT==TRUE)
#if (SBC_IS_64_MULT_IN_WINDOW_ACCU == TRUE)
    register SINT64 s64Temp, s64Temp2;
#else
    register SINT32 s32Temp, s32Temp2;
#endif
#else
#if (SBC_IS_64_MULT_IN_WINDOW_ACCU == TRUE)
    SINT64 s64Temp;
#endif
#endif
#endif

    s32NumOfChannels = pstrEncParams->s16NumOfChannels;
    s32NumOfBlocks   = pstrEncParams->s16NumOfBlocks;

    ps16PcmBuf = pstrEncParams->ps16NextPcmBuffer;

    ps32SbBuf  = pstrEncParams->s32SbBuffer;
    Offset2 = (SINT32)(EncMaxShiftCounter + 80);
    for (s32Blk = 0; s32Blk < s32NumOfBlocks; s32Blk++) {
        Offset = (SINT32)(EncMaxShiftCounter - ShiftCounter);
        /* Store new samples */
        if (s32NumOfChannels == 1) {
            s16X[7 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
            s16X[6 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
            s16X[5 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
            s16X[4 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
            s16X[3 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
            s16X[2 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
            s16X[1 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
            s16X[0 + Offset] = *ps16PcmBuf;   ps16PcmBuf++;
        } else {
            s16X[7 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 7 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
            s16X[6 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 6 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
            s16X[5 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 5 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
            s16X[4 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 4 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
            s16X[3 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 3 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
            s16X[2 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 2 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
            s16X[1 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 1 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
            s16X[0 + Offset] = *ps16PcmBuf;        ps16PcmBuf++;
            s16X[Offset2 + 0 + Offset] = *ps16PcmBuf;     ps16PcmBuf++;
        }
        for (s32Ch = 0; s32Ch < s32NumOfChannels; s32Ch++) {
            ChOffset = s32Ch * Offset2 + Offset;

            WINDOW_PARTIAL_8

            SBC_FastIDCT8 (s32DCTY, ps32SbBuf);

            ps32SbBuf += SUB_BANDS_8;
        }
        if (s32NumOfChannels == 1) {
            if (ShiftCounter >= EncMaxShiftCounter) {
                SHIFTUP_X8;
                ShiftCounter = 0;
            } else {
                ShiftCounter += SUB_BANDS_8;
            }
        } else {
            if (ShiftCounter >= EncMaxShiftCounter) {
                SHIFTUP_X8_2;
                ShiftCounter = 0;
            } else {
                ShiftCounter += SUB_BANDS_8;
            }
        }
    }
}

void SbcAnalysisInit (void)
{
    memset(s16X, 0, ENC_VX_BUFFER_SIZE * sizeof(SINT16));
    ShiftCounter = 0;
}

#endif /* #if (defined(SBC_ENC_INCLUDED) && SBC_ENC_INCLUDED == TRUE) */
