/******************************************************************************
 *
 *  Copyright (C) 2000-2012 Broadcom Corporation
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
 *  nterface to A2DP Application Programming Interface
 *
 ******************************************************************************/
#ifndef A2D_API_H
#define A2D_API_H
#include "sdp_api.h"
#if (A2D_INCLUDED == TRUE)
/*****************************************************************************
**  constants
*****************************************************************************/

/* Profile supported features */
#define A2D_SUPF_PLAYER     0x0001
#define A2D_SUPF_MIC        0x0002
#define A2D_SUPF_TUNER      0x0004
#define A2D_SUPF_MIXER      0x0008

#define A2D_SUPF_HEADPHONE  0x0001
#define A2D_SUPF_SPEAKER    0x0002
#define A2D_SUPF_RECORDER   0x0004
#define A2D_SUPF_AMP        0x0008

/* AV Media Types */
#define A2D_MEDIA_TYPE_AUDIO    0x00    /* audio media type + RFA */
#define A2D_MEDIA_TYPE_VIDEO    0x10    /* video media type + RFA */
#define A2D_MEDIA_TYPE_MULTI    0x20    /* multimedia media type + RFA */

/* AV Media Codec Type (Audio Codec ID) */
#define A2D_MEDIA_CT_SBC        0x00    /* SBC media codec type */
#define A2D_MEDIA_CT_M12        0x01    /* MPEG-1, 2 Audio media codec type */
#define A2D_MEDIA_CT_M24        0x02    /* MPEG-2, 4 AAC media codec type */
#define A2D_MEDIA_CT_ATRAC      0x04    /* ATRAC family media codec type */

#define A2D_SUCCESS           0     /* Success */
#define A2D_FAIL              0x0A  /* Failed */
#define A2D_BUSY              0x0B  /* A2D_FindService is already in progress */
#define A2D_INVALID_PARAMS    0x0C  /* bad parameters */
#define A2D_WRONG_CODEC       0x0D  /* wrong codec info */
#define A2D_BAD_CODEC_TYPE    0xC1  /* Media Codec Type is not valid  */
#define A2D_NS_CODEC_TYPE     0xC2  /* Media Codec Type is not supported */
#define A2D_BAD_SAMP_FREQ     0xC3  /* Sampling Frequency is not valid or multiple values have been selected  */
#define A2D_NS_SAMP_FREQ      0xC4  /* Sampling Frequency is not supported  */
#define A2D_BAD_CH_MODE       0xC5  /* Channel Mode is not valid or multiple values have been selected  */
#define A2D_NS_CH_MODE        0xC6  /* Channel Mode is not supported */
#define A2D_BAD_SUBBANDS      0xC7  /* None or multiple values have been selected for Number of Subbands */
#define A2D_NS_SUBBANDS       0xC8  /* Number of Subbands is not supported */
#define A2D_BAD_ALLOC_MTHD    0xC9  /* None or multiple values have been selected for Allocation Method */
#define A2D_NS_ALLOC_MTHD     0xCA  /* Allocation Method is not supported */
#define A2D_BAD_MIN_BITPOOL   0xCB  /* Minimum Bitpool Value is not valid */
#define A2D_NS_MIN_BITPOOL    0xCC  /* Minimum Bitpool Value is not supported */
#define A2D_BAD_MAX_BITPOOL   0xCD  /* Maximum Bitpool Value is not valid */
#define A2D_NS_MAX_BITPOOL    0xCE  /* Maximum Bitpool Value is not supported */
#define A2D_BAD_LAYER         0xCF  /* None or multiple values have been selected for Layer */
#define A2D_NS_LAYER          0xD0  /* Layer is not supported */
#define A2D_NS_CRC            0xD1  /* CRC is not supported */
#define A2D_NS_MPF            0xD2  /* MPF-2 is not supported */
#define A2D_NS_VBR            0xD3  /* VBR is not supported */
#define A2D_BAD_BIT_RATE      0xD4  /* None or multiple values have been selected for Bit Rate */
#define A2D_NS_BIT_RATE       0xD5  /* Bit Rate is not supported */
#define A2D_BAD_OBJ_TYPE      0xD6  /* Either 1) Object type is not valid (b3-b0) or 2) None or multiple values have been selected for Object Type */
#define A2D_NS_OBJ_TYPE       0xD7  /* Object type is not supported */
#define A2D_BAD_CHANNEL       0xD8  /* None or multiple values have been selected for Channels */
#define A2D_NS_CHANNEL        0xD9  /* Channels is not supported */
#define A2D_BAD_BLOCK_LEN     0xDD  /* None or multiple values have been selected for Block Length */
#define A2D_BAD_CP_TYPE       0xE0  /* The requested CP Type is not supported. */
#define A2D_BAD_CP_FORMAT     0xE1  /* The format of Content Protection Service Capability/Content Protection Scheme Dependent Data is not correct. */

typedef UINT8 tA2D_STATUS;

/* the return values from A2D_BitsSet() */
#define A2D_SET_ONE_BIT         1   /* one and only one bit is set */
#define A2D_SET_ZERO_BIT        0   /* all bits clear */
#define A2D_SET_MULTL_BIT       2   /* multiple bits are set */

/*****************************************************************************
**  type definitions
*****************************************************************************/

/* This data type is used in A2D_FindService() to initialize the SDP database
 * to hold the result service search. */
typedef struct {
    UINT32              db_len;  /* Length, in bytes, of the discovery database */
    UINT16              num_attr;/* The number of attributes in p_attrs */
    tSDP_DISCOVERY_DB  *p_db;    /* Pointer to the discovery database */
    UINT16             *p_attrs; /* The attributes filter. If NULL, A2DP API sets the attribute filter
                                  * to be ATTR_ID_SERVICE_CLASS_ID_LIST, ATTR_ID_BT_PROFILE_DESC_LIST,
                                  * ATTR_ID_SUPPORTED_FEATURES, ATTR_ID_SERVICE_NAME and ATTR_ID_PROVIDER_NAME.
                                  * If not NULL, the input is taken as the filter. */
} tA2D_SDP_DB_PARAMS;

/* This data type is used in tA2D_FIND_CBACK to report the result of the SDP discovery process. */
typedef struct {
    UINT16  service_len;    /* Length, in bytes, of the service name */
    UINT16  provider_len;   /* Length, in bytes, of the provider name */
    char   *p_service_name; /* Pointer the service name.  This character string may not be null terminated.
                             * Use the service_len parameter to safely copy this string */
    char   *p_provider_name;/* Pointer the provider name.  This character string may not be null terminated.
                             * Use the provider_len parameter to safely copy this string */
    UINT16  features;       /* Profile supported features */
    UINT16  avdt_version;   /* AVDTP protocol version */
} tA2D_Service;

/* This is the callback to notify the result of the SDP discovery process. */
typedef void (tA2D_FIND_CBACK)(BOOLEAN found, tA2D_Service *p_service);


/*****************************************************************************
**  external function declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/******************************************************************************
**
** Function         A2D_AddRecord
**
** Description      This function is called by a server application to add
**                  SRC or SNK information to an SDP record.  Prior to
**                  calling this function the application must call
**                  SDP_CreateRecord() to create an SDP record.
**
**                  Input Parameters:
**                      service_uuid:  Indicates SRC or SNK.
**
**                      p_service_name:  Pointer to a null-terminated character
**                      string containing the service name.
**
**                      p_provider_name:  Pointer to a null-terminated character
**                      string containing the provider name.
**
**                      features:  Profile supported features.
**
**                      sdp_handle:  SDP handle returned by SDP_CreateRecord().
**
**                  Output Parameters:
**                      None.
**
** Returns          A2D_SUCCESS if function execution succeeded,
**                  A2D_INVALID_PARAMS if bad parameters are given.
**                  A2D_FAIL if function execution failed.
**
******************************************************************************/
extern tA2D_STATUS A2D_AddRecord(UINT16 service_uuid, char *p_service_name, char *p_provider_name,
                                 UINT16 features, UINT32 sdp_handle);

/******************************************************************************
**
** Function         A2D_FindService
**
** Description      This function is called by a client application to
**                  perform service discovery and retrieve SRC or SNK SDP
**                  record information from a server.  Information is
**                  returned for the first service record found on the
**                  server that matches the service UUID.  The callback
**                  function will be executed when service discovery is
**                  complete.  There can only be one outstanding call to
**                  A2D_FindService() at a time; the application must wait
**                  for the callback before it makes another call to
**                  the function.
**
**                  Input Parameters:
**                      service_uuid:  Indicates SRC or SNK.
**
**                      bd_addr:  BD address of the peer device.
**
**                      p_db:  Pointer to the information to initialize
**                             the discovery database.
**
**                      p_cback:  Pointer to the A2D_FindService()
**                      callback function.
**
**                  Output Parameters:
**                      None.
**
** Returns          A2D_SUCCESS if function execution succeeded,
**                  A2D_INVALID_PARAMS if bad parameters are given.
**                  A2D_BUSY if discovery is already in progress.
**                  A2D_FAIL if function execution failed.
**
******************************************************************************/
extern tA2D_STATUS A2D_FindService(UINT16 service_uuid, BD_ADDR bd_addr,
                                   tA2D_SDP_DB_PARAMS *p_db, tA2D_FIND_CBACK *p_cback);

/******************************************************************************
**
** Function         A2D_SetTraceLevel
**
** Description      Sets the trace level for A2D. If 0xff is passed, the
**                  current trace level is returned.
**
**                  Input Parameters:
**                      new_level:  The level to set the A2D tracing to:
**                      0xff-returns the current setting.
**                      0-turns off tracing.
**                      >= 1-Errors.
**                      >= 2-Warnings.
**                      >= 3-APIs.
**                      >= 4-Events.
**                      >= 5-Debug.
**
** Returns          The new trace level or current trace level if
**                  the input parameter is 0xff.
**
******************************************************************************/
extern UINT8 A2D_SetTraceLevel (UINT8 new_level);

/******************************************************************************
** Function         A2D_BitsSet
**
** Description      Check the given num for the number of bits set
** Returns          A2D_SET_ONE_BIT, if one and only one bit is set
**                  A2D_SET_ZERO_BIT, if all bits clear
**                  A2D_SET_MULTL_BIT, if multiple bits are set
******************************************************************************/
extern UINT8 A2D_BitsSet(UINT8 num);

#ifdef __cplusplus
}
#endif

/*******************************************************************************
**
** Function         A2D_Init
**
** Description      This function is called at stack startup to allocate the
**                  control block (if using dynamic memory), and initializes the
**                  control block and tracing level.
**
** Returns          void
**
*******************************************************************************/
extern void A2D_Init(void);
#endif  ///A2D_INCLUDED
#endif /* A2D_API_H */
