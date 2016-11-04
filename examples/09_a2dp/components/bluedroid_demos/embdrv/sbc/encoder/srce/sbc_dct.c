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
 *  source file for fast dct operations
 *
 ******************************************************************************/

#include "sbc_encoder.h"
#include "sbc_enc_func_declare.h"
#include "sbc_dct.h"



/*******************************************************************************
**
** Function         SBC_FastIDCT8
**
** Description      implementation of fast DCT algorithm by Feig and Winograd
**
**
** Returns          y = dct(pInVect)
**
**
*******************************************************************************/

#if (SBC_IS_64_MULT_IN_IDCT == FALSE)
#define SBC_COS_PI_SUR_4            (0x00005a82)  /* ((0x8000) * 0.7071)     = cos(pi/4) */
#define SBC_COS_PI_SUR_8            (0x00007641)  /* ((0x8000) * 0.9239)     = (cos(pi/8)) */
#define SBC_COS_3PI_SUR_8           (0x000030fb)  /* ((0x8000) * 0.3827)     = (cos(3*pi/8)) */
#define SBC_COS_PI_SUR_16           (0x00007d8a)  /* ((0x8000) * 0.9808))     = (cos(pi/16)) */
#define SBC_COS_3PI_SUR_16          (0x00006a6d)  /* ((0x8000) * 0.8315))     = (cos(3*pi/16)) */
#define SBC_COS_5PI_SUR_16          (0x0000471c)  /* ((0x8000) * 0.5556))     = (cos(5*pi/16)) */
#define SBC_COS_7PI_SUR_16          (0x000018f8)  /* ((0x8000) * 0.1951))     = (cos(7*pi/16)) */
#define SBC_IDCT_MULT(a,b,c) SBC_MULT_32_16_SIMPLIFIED(a,b,c)
#else
#define SBC_COS_PI_SUR_4            (0x5A827999)  /* ((0x80000000) * 0.707106781)      = (cos(pi/4)   ) */
#define SBC_COS_PI_SUR_8            (0x7641AF3C)  /* ((0x80000000) * 0.923879533)      = (cos(pi/8)   ) */
#define SBC_COS_3PI_SUR_8           (0x30FBC54D)  /* ((0x80000000) * 0.382683432)      = (cos(3*pi/8) ) */
#define SBC_COS_PI_SUR_16           (0x7D8A5F3F)  /* ((0x80000000) * 0.98078528 ))     = (cos(pi/16)  ) */
#define SBC_COS_3PI_SUR_16          (0x6A6D98A4)  /* ((0x80000000) * 0.831469612))     = (cos(3*pi/16)) */
#define SBC_COS_5PI_SUR_16          (0x471CECE6)  /* ((0x80000000) * 0.555570233))     = (cos(5*pi/16)) */
#define SBC_COS_7PI_SUR_16          (0x18F8B83C)  /* ((0x80000000) * 0.195090322))     = (cos(7*pi/16)) */
#define SBC_IDCT_MULT(a,b,c) SBC_MULT_32_32(a,b,c)
#endif /* SBC_IS_64_MULT_IN_IDCT */

#if (SBC_FAST_DCT == FALSE)
extern const SINT16 gas16AnalDCTcoeff8[];
extern const SINT16 gas16AnalDCTcoeff4[];
#endif

void SBC_FastIDCT8(SINT32 *pInVect, SINT32 *pOutVect)
{
#if (SBC_FAST_DCT == TRUE)
#if (SBC_ARM_ASM_OPT==TRUE)
#else
#if (SBC_IPAQ_OPT==TRUE)
#if (SBC_IS_64_MULT_IN_IDCT == TRUE)
    SINT64 s64Temp;
#endif
#else
#if (SBC_IS_64_MULT_IN_IDCT == TRUE)
    SINT32 s32HiTemp;
#else
    SINT32 s32In2Temp;
    register SINT32 s32In1Temp;
#endif
#endif
#endif

    register SINT32 x0, x1, x2, x3, x4, x5, x6, x7,temp;
    SINT32 res_even[4], res_odd[4];
    /*x0= (pInVect[4])/2 ;*/
    SBC_IDCT_MULT(SBC_COS_PI_SUR_4,pInVect[4], x0);
    /*printf("x0 0x%x = %d = %d * %d\n", x0, x0, SBC_COS_PI_SUR_4, pInVect[4]);*/

    x1 = (pInVect[3] + pInVect[5])  >>1;
    x2 = (pInVect[2] + pInVect[6])  >>1;
    x3 = (pInVect[1] + pInVect[7])  >>1;
    x4 = (pInVect[0] + pInVect[8])  >>1;
    x5 = (pInVect[9] - pInVect[15]) >>1;
    x6 = (pInVect[10] - pInVect[14])>>1;
    x7 = (pInVect[11] - pInVect[13])>>1;

    /* 2-point IDCT of x0 and x4 as in (11) */
    temp = x0 ;
    SBC_IDCT_MULT(SBC_COS_PI_SUR_4, ( x0 + x4 ), x0);          /*x0 = ( x0 + x4 ) * cos(1*pi/4) ; */
    SBC_IDCT_MULT(SBC_COS_PI_SUR_4, ( temp - x4 ), x4);        /*x4 = ( temp - x4 ) * cos(1*pi/4) ; */

    /* rearrangement of x2 and x6 as in (15) */
    x2 -=x6;
    x6 <<= 1 ;

    /* 2-point IDCT of x2 and x6 and post-multiplication as in (15) */
    SBC_IDCT_MULT(SBC_COS_PI_SUR_4,x6, x6); /*x6 = x6 * cos(1*pi/4) ; */
    temp = x2 ;
    SBC_IDCT_MULT(SBC_COS_PI_SUR_8,( x2 + x6 ), x2); /*x2 = ( x2 + x6 ) * cos(1*pi/8) ; */
    SBC_IDCT_MULT(SBC_COS_3PI_SUR_8,( temp - x6 ), x6); /*x6 = ( temp - x6 ) * cos(3*pi/8) ;*/

    /* 4-point IDCT of x0,x2,x4 and x6 as in (11) */
    res_even[ 0 ] = x0 + x2 ;
    res_even[ 1 ] = x4 + x6 ;
    res_even[ 2 ] = x4 - x6 ;
    res_even[ 3 ] = x0 - x2 ;


    /* rearrangement of x1,x3,x5,x7 as in (15) */
    x7 <<= 1 ;
    x5 = ( x5 <<1 ) - x7 ;
    x3 = ( x3 <<1 ) - x5 ;
    x1 -= x3 >>1 ;

    /* two-dimensional IDCT of x1 and x5 */
    SBC_IDCT_MULT(SBC_COS_PI_SUR_4, x5, x5);          /*x5 = x5 * cos(1*pi/4) ; */
    temp = x1 ;
    x1 = x1 + x5 ;
    x5 = temp - x5 ;

    /* rearrangement of x3 and x7 as in (15) */
    x3 -= x7;
    x7 <<= 1 ;
    SBC_IDCT_MULT(SBC_COS_PI_SUR_4, x7, x7);          /*x7 = x7 * cos(1*pi/4) ; */

    /* 2-point IDCT of x3 and x7 and post-multiplication as in (15) */
    temp = x3 ;
    SBC_IDCT_MULT( SBC_COS_PI_SUR_8,( x3 + x7 ), x3);          /*x3 = ( x3 + x7 ) * cos(1*pi/8)  ; */
    SBC_IDCT_MULT( SBC_COS_3PI_SUR_8,( temp - x7 ), x7);          /*x7 = ( temp - x7 ) * cos(3*pi/8) ;*/

    /* 4-point IDCT of x1,x3,x5 and x7 and post multiplication by diagonal matrix as in (14) */
    SBC_IDCT_MULT((SBC_COS_PI_SUR_16),   ( x1 + x3 ) ,   res_odd[0]); /*res_odd[ 0 ] = ( x1 + x3 ) * cos(1*pi/16) ; */
    SBC_IDCT_MULT((SBC_COS_3PI_SUR_16),  ( x5 + x7 ) ,   res_odd[1]); /*res_odd[ 1 ] = ( x5 + x7 ) * cos(3*pi/16) ; */
    SBC_IDCT_MULT((SBC_COS_5PI_SUR_16),  ( x5 - x7 ) ,   res_odd[2]); /*res_odd[ 2 ] = ( x5 - x7 ) * cos(5*pi/16) ; */
    SBC_IDCT_MULT((SBC_COS_7PI_SUR_16),  ( x1 - x3 ) ,  res_odd[3]); /*res_odd[ 3 ] = ( x1 - x3 ) * cos(7*pi/16) ; */

    /* additions and subtractions as in (9) */
    pOutVect[0] = (res_even[ 0 ] + res_odd[ 0 ])  ;
    pOutVect[1] = (res_even[ 1 ] + res_odd[ 1 ])  ;
    pOutVect[2] = (res_even[ 2 ] + res_odd[ 2 ])  ;
    pOutVect[3] = (res_even[ 3 ] + res_odd[ 3 ])  ;
    pOutVect[7] = (res_even[ 0 ] - res_odd[ 0 ])  ;
    pOutVect[6] = (res_even[ 1 ] - res_odd[ 1 ])  ;
    pOutVect[5] = (res_even[ 2 ] - res_odd[ 2 ])  ;
    pOutVect[4] = (res_even[ 3 ] - res_odd[ 3 ])  ;
#else
    UINT8 Index, k;
    SINT32 temp;
	/*Calculate 4 subband samples by matrixing*/
    for(Index=0; Index<8; Index++)
    {
        temp = 0;
        for(k=0; k<16; k++)
        {
            /*temp += (SINT32)(((SINT64)M[(Index*strEncParams->numOfSubBands*2)+k] * Y[k]) >> 16 );*/
            temp += (gas16AnalDCTcoeff8[(Index*8*2)+k] * (pInVect[k] >> 16));
            temp += ((gas16AnalDCTcoeff8[(Index*8*2)+k] * (pInVect[k] & 0xFFFF)) >> 16);
        }
        pOutVect[Index] = temp;
    }
#endif
/*    printf("pOutVect: 0x%x;0x%x;0x%x;0x%x;0x%x;0x%x;0x%x;0x%x\n",\
        pOutVect[0],pOutVect[1],pOutVect[2],pOutVect[3],pOutVect[4],pOutVect[5],pOutVect[6],pOutVect[7]);*/
}

/*******************************************************************************
**
** Function         SBC_FastIDCT4
**
** Description      implementation of fast DCT algorithm by Feig and Winograd
**
**
** Returns          y = dct(x0)
**
**
*******************************************************************************/
void SBC_FastIDCT4(SINT32 *pInVect, SINT32 *pOutVect)
{
#if (SBC_FAST_DCT == TRUE)
#if (SBC_ARM_ASM_OPT==TRUE)
#else
#if (SBC_IPAQ_OPT==TRUE)
#if (SBC_IS_64_MULT_IN_IDCT == TRUE)
    SINT64 s64Temp;
#endif
#else
#if (SBC_IS_64_MULT_IN_IDCT == TRUE)
    SINT32 s32HiTemp;
#else
    UINT16 s32In2Temp;
    SINT32 s32In1Temp;
#endif
#endif
#endif
    SINT32 temp,x2;
    SINT32 tmp[8];

    x2=pInVect[2]>>1;
    temp=(pInVect[0]+pInVect[4]);
    SBC_IDCT_MULT((SBC_COS_PI_SUR_4>>1), temp , tmp[0]);
    tmp[1]=x2-tmp[0];
    tmp[0]+=x2;
    temp=(pInVect[1]+pInVect[3]);
    SBC_IDCT_MULT((SBC_COS_3PI_SUR_8>>1), temp , tmp[3]);
    SBC_IDCT_MULT((SBC_COS_PI_SUR_8>>1), temp , tmp[2]);
    temp=(pInVect[5]-pInVect[7]);
    SBC_IDCT_MULT((SBC_COS_3PI_SUR_8>>1), temp , tmp[5]);
    SBC_IDCT_MULT((SBC_COS_PI_SUR_8>>1), temp , tmp[4]);
    tmp[6]=tmp[2]+tmp[5];
    tmp[7]=tmp[3]-tmp[4];
    pOutVect[0] = (tmp[0]+tmp[6]);
    pOutVect[1] = (tmp[1]+tmp[7]);
    pOutVect[2] = (tmp[1]-tmp[7]);
    pOutVect[3] = (tmp[0]-tmp[6]);
#else
    UINT8 Index, k;
    SINT32 temp;
	/*Calculate 4 subband samples by matrixing*/
    for(Index=0; Index<4; Index++)
    {
        temp = 0;
        for(k=0; k<8; k++)
        {
            /*temp += (SINT32)(((SINT64)M[(Index*strEncParams->numOfSubBands*2)+k] * Y[k]) >> 16 ); */
            temp += (gas16AnalDCTcoeff4[(Index*4*2)+k] * (pInVect[k] >> 16));
            temp += ((gas16AnalDCTcoeff4[(Index*4*2)+k] * (pInVect[k] & 0xFFFF)) >> 16);
        }
        pOutVect[Index] = temp;
    }
#endif
}
