/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************
 *
 *  interface to MPEG-2, 4 AAC codec
 *
 ******************************************************************************/
#ifndef A2D_M24_H
#define A2D_M24_H
#include "stack/a2d_api.h"
#if (A2D_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/
/* CIE offset in the info byte sequence */
#define A2D_M24_CIE_OFF             3
/* CIE length in the info byte sequence */
#define A2D_M24_CIE_LEN             6

/* the LOSC of M24 media codec capability */
#define A2D_M24_INFO_LEN            8

/* for Codec Specific Information Element */
#define A2D_M24_IE_OBJ_TYPE_MSK             0xFE    /* b7-b1: Object type */
#define A2D_M24_IE_OBJ_TYPE_2_AAC_LC        0x80    /* b7: MPEG-2 AAC LC */
#define A2D_M24_IE_OBJ_TYPE_4_AAC_LC        0x40    /* b6: MPEG-4 AAC LC */
#define A2D_M24_IE_OBJ_TYPE_4_AAC_LTP       0x20    /* b5: MPEG-4 AAC LTP */
#define A2D_M24_IE_OBJ_TYPE_4_AAC_SCALABLE  0x10    /* b4: MPEG-4 AAC Scalable */
#define A2D_M24_IE_OBJ_TYPE_4_HE_AAC        0x08    /* b3: MPEG-4 HE-AAC */
#define A2D_M24_IE_OBJ_TYPE_4_HE_AAC_V2     0x04    /* b2: MPEG-4 HE-AAC v2 */
#define A2D_M24_IE_OBJ_TYPE_4_AAC_ELD_V2    0x02    /* b1: MPEG-4 AAC-ELD v2 */

#define A2D_M24_IE_DRC_MSK                  0x01    /* b0: DRC */
#define A2D_M24_IE_DRC_SUPPORT              0x01    /* DRC support */
#define A2D_M24_IE_DRC_NS                   0x00    /* DRC not support */

#define A2D_M24_IE_SAMP_FREQ1_MSK           0xFF    /* b7-b0: Sampling frequency part 1 */
#define A2D_M24_IE_SAMP_FREQ1_8             0x80    /* b7:8  kHz */
#define A2D_M24_IE_SAMP_FREQ1_11            0x40    /* b6:11.025 kHz */
#define A2D_M24_IE_SAMP_FREQ1_12            0x20    /* b5:12  kHz */
#define A2D_M24_IE_SAMP_FREQ1_16            0x10    /* b4:16  kHz */
#define A2D_M24_IE_SAMP_FREQ1_22            0x08    /* b3:22.05 kHz */
#define A2D_M24_IE_SAMP_FREQ1_24            0x04    /* b2:24  kHz */
#define A2D_M24_IE_SAMP_FREQ1_32            0x02    /* b1:32  kHz */
#define A2D_M24_IE_SAMP_FREQ1_44            0x01    /* b0:44.1kHz */
#define A2D_M24_IE_SAMP_FREQ1_NS            0x00    /* Sampling frequency part 1: not support */

#define A2D_M24_IE_SAMP_FREQ2_MSK           0xF0    /* b7-b4: Sampling frequency part 2 */
#define A2D_M24_IE_SAMP_FREQ2_48            0x80    /* b7:48  kHz */
#define A2D_M24_IE_SAMP_FREQ2_64            0x40    /* b6:64  kHz */
#define A2D_M24_IE_SAMP_FREQ2_88            0x20    /* b5:88.2kHz */
#define A2D_M24_IE_SAMP_FREQ2_96            0x10    /* b4:96  kHz */
#define A2D_M24_IE_SAMP_FREQ2_NS            0x00    /* Sampling frequency part 2: not support */

#define A2D_M24_IE_CH_MSK                   0x0F    /* b3-b0: Channels */
#define A2D_M24_IE_CH_1                     0x08    /* b3: 1 */
#define A2D_M24_IE_CH_2                     0x04    /* b2: 2 */
#define A2D_M24_IE_CH_6                     0x02    /* b1: 6 */
#define A2D_M24_IE_CH_8                     0x01    /* b0: 8 */

#define A2D_M24_IE_VBR_MSK                  0x80    /* b7: VBR */
#define A2D_M24_IE_VBR_SUPPORT              0x80    /* VBR support */
#define A2D_M24_IE_VBR_NS                   0x00    /* VBR not support */

#define A2D_M24_IE_BR1_MSK                  0x7F    /* b6-b0: Bit rate part 1 */
#define A2D_M24_IE_BR2_MSK                  0xFF    /* b7-b0: Bit rate part 2 */
#define A2D_M24_IE_BR3_MSK                  0xFF    /* b7-b0: Bit rate part 3 */

/*****************************************************************************
**  Type Definitions
*****************************************************************************/

/* data type for the M24 Codec Information Element*/
typedef struct {
    UINT8   drc;             /* DRC */
    UINT8   obj_type;        /* Object type */
    UINT8   samp_freq1;      /* Sampling frequency part 1 */
    UINT8   ch;              /* Channels */
    UINT8   samp_freq2;      /* Sampling frequency part 2 */
    UINT8   br1;             /* Bit rate part 1 */
    UINT8   vbr;             /* VBR */
    UINT8   br2;             /* Bit rate part 2 */
    UINT8   br3;             /* Bit rate part 3 */
} tA2D_M24_CIE;

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
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
extern tA2D_STATUS A2D_BldM24Info(UINT8 media_type, tA2D_M24_CIE *p_ie,
                                  UINT8 *p_result);

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
extern tA2D_STATUS A2D_ParsM24Info(tA2D_M24_CIE *p_ie, UINT8 *p_info,
                                   BOOLEAN for_caps);

#ifdef __cplusplus
}
#endif

#endif  ///A2D_INCLUDED == TRUE

#endif /* A2D_M24_H */
