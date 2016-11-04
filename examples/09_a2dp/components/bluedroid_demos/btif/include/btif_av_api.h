/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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

/*****************************************************************************
 **
 **  Name:           btif_av_api.h
 **
 **  Description:    This is the public interface file for the advanced
 **                  audio/video streaming (AV) subsystem of BTIF, Broadcom's
 **                  Bluetooth application layer for mobile phones.
 **
 *****************************************************************************/

#ifndef BTIF_AV_API_H
#define BTIF_AV_API_H

#include "bt_target.h"
#include "bta_av_api.h"
#include "uipc.h"

#include "btif_media.h"
#include "a2d_api.h"
#include "a2d_sbc.h"


/*****************************************************************************
 **  Constants and data types
 *****************************************************************************/

/* Codec type */
#define BTIF_AV_CODEC_NONE       0xFF
#define BTIF_AV_CODEC_SBC        A2D_MEDIA_CT_SBC        /* SBC media codec type */

#define BTIF_AV_CODEC_PCM        0x5                     /* Raw PCM */

typedef UINT8 tBTIF_AV_CODEC_ID;

/* AV features masks */
#define BTIF_AV_FEAT_RCTG        BTA_AV_FEAT_RCTG      /* remote control target */
#define BTIF_AV_FEAT_RCCT        BTA_AV_FEAT_RCCT      /* remote control controller */
#define BTIF_AV_FEAT_METADATA    BTA_AV_FEAT_METADATA  /* remote control Metadata Transfer command/response */

typedef UINT16 tBTIF_AV_FEAT;

/* AV channel values */
#define BTIF_AV_CHNL_MSK         BTA_AV_CHNL_MSK
#define BTIF_AV_CHNL_AUDIO       BTA_AV_CHNL_AUDIO       /* audio channel */
#define BTIF_AV_CHNL_VIDEO       BTA_AV_CHNL_VIDEO       /* video channel */
typedef UINT8 tBTIF_AV_CHNL;

typedef UINT8 tBTIF_AV_HNDL;

/* Operation id list for BTIF_AvRemoteCmd */
#define BTIF_AV_ID_SELECT      0x00    /* select */
#define BTIF_AV_ID_UP          0x01    /* up */
#define BTIF_AV_ID_DOWN        0x02    /* down */
#define BTIF_AV_ID_LEFT        0x03    /* left */
#define BTIF_AV_ID_RIGHT       0x04    /* right */
#define BTIF_AV_ID_RIGHT_UP    0x05    /* right-up */
#define BTIF_AV_ID_RIGHT_DOWN  0x06    /* right-down */
#define BTIF_AV_ID_LEFT_UP     0x07    /* left-up */
#define BTIF_AV_ID_LEFT_DOWN   0x08    /* left-down */
#define BTIF_AV_ID_ROOT_MENU   0x09    /* root menu */
#define BTIF_AV_ID_SETUP_MENU  0x0A    /* setup menu */
#define BTIF_AV_ID_CONT_MENU   0x0B    /* contents menu */
#define BTIF_AV_ID_FAV_MENU    0x0C    /* favorite menu */
#define BTIF_AV_ID_EXIT        0x0D    /* exit */
#define BTIF_AV_ID_0           0x20    /* 0 */
#define BTIF_AV_ID_1           0x21    /* 1 */
#define BTIF_AV_ID_2           0x22    /* 2 */
#define BTIF_AV_ID_3           0x23    /* 3 */
#define BTIF_AV_ID_4           0x24    /* 4 */
#define BTIF_AV_ID_5           0x25    /* 5 */
#define BTIF_AV_ID_6           0x26    /* 6 */
#define BTIF_AV_ID_7           0x27    /* 7 */
#define BTIF_AV_ID_8           0x28    /* 8 */
#define BTIF_AV_ID_9           0x29    /* 9 */
#define BTIF_AV_ID_DOT         0x2A    /* dot */
#define BTIF_AV_ID_ENTER       0x2B    /* enter */
#define BTIF_AV_ID_CLEAR       0x2C    /* clear */
#define BTIF_AV_ID_CHAN_UP     0x30    /* channel up */
#define BTIF_AV_ID_CHAN_DOWN   0x31    /* channel down */
#define BTIF_AV_ID_PREV_CHAN   0x32    /* previous channel */
#define BTIF_AV_ID_SOUND_SEL   0x33    /* sound select */
#define BTIF_AV_ID_INPUT_SEL   0x34    /* input select */
#define BTIF_AV_ID_DISP_INFO   0x35    /* display information */
#define BTIF_AV_ID_HELP        0x36    /* help */
#define BTIF_AV_ID_PAGE_UP     0x37    /* page up */
#define BTIF_AV_ID_PAGE_DOWN   0x38    /* page down */
#define BTIF_AV_ID_POWER       0x40    /* power */
#define BTIF_AV_ID_VOL_UP      0x41    /* volume up */
#define BTIF_AV_ID_VOL_DOWN    0x42    /* volume down */
#define BTIF_AV_ID_MUTE        0x43    /* mute */
#define BTIF_AV_ID_PLAY        0x44    /* play */
#define BTIF_AV_ID_STOP        0x45    /* stop */
#define BTIF_AV_ID_PAUSE       0x46    /* pause */
#define BTIF_AV_ID_RECORD      0x47    /* record */
#define BTIF_AV_ID_REWIND      0x48    /* rewind */
#define BTIF_AV_ID_FAST_FOR    0x49    /* fast forward */
#define BTIF_AV_ID_EJECT       0x4A    /* eject */
#define BTIF_AV_ID_FORWARD     0x4B    /* forward */
#define BTIF_AV_ID_BACKWARD    0x4C    /* backward */
#define BTIF_AV_ID_ANGLE       0x50    /* angle */
#define BTIF_AV_ID_SUBPICT     0x51    /* subpicture */
#define BTIF_AV_ID_F1          0x71    /* F1 */
#define BTIF_AV_ID_F2          0x72    /* F2 */
#define BTIF_AV_ID_F3          0x73    /* F3 */
#define BTIF_AV_ID_F4          0x74    /* F4 */
#define BTIF_AV_ID_F5          0x75    /* F5 */
#define BTIF_AV_ID_VENDOR      0x7E    /* vendor unique */
#define BTIF_AV_KEYPRESSED_RELEASE 0x80

typedef UINT8 tBTIF_AV_RC;

/* State flag for pass through command */
#define BTIF_AV_STATE_PRESS      0    /* key pressed */
#define BTIF_AV_STATE_RELEASE    1  /* key released */

typedef UINT8 tBTIF_AV_STATE;

typedef UINT8 tBTIF_AV_RC_HNDL;

/* Command codes for BTIF_AvVendorCmd */
#define BTIF_AV_CMD_CTRL         0
#define BTIF_AV_CMD_STATUS       1
#define BTIF_AV_CMD_SPEC_INQ     2
#define BTIF_AV_CMD_NOTIF        3
#define BTIF_AV_CMD_GEN_INQ      4

typedef UINT8 tBTIF_AV_CMD;

/* AV callback events */
#define BTIF_AV_OPEN_EVT         0       /* connection opened */
#define BTIF_AV_CLOSE_EVT        1       /* connection closed */
#define BTIF_AV_START_EVT        2       /* stream data transfer started */
#define BTIF_AV_STOP_EVT         3       /* stream data transfer stopped */
#define BTIF_AV_RC_OPEN_EVT      4       /* remote control channel open */
#define BTIF_AV_RC_CLOSE_EVT     5       /* remote control channel closed */
#define BTIF_AV_REMOTE_CMD_EVT   6      /* remote control command */
#define BTIF_AV_REMOTE_RSP_EVT   7      /* remote control response */
#define BTIF_AV_META_MSG_EVT     8      /* metadata messages */

typedef UINT8 tBTIF_AV_EVT;

#define BTIF_AV_FEEDING_ASYNCHRONOUS 0   /* asynchronous feeding, use tx av timer */
#define BTIF_AV_FEEDING_SYNCHRONOUS  1   /* synchronous feeding, no av tx timer */

#define BTIF_AV_MAX_SYNCHRONOUS_LATENCY 80 /* max latency in ms for BTIF_AV_FEEDING_SYNCHRONOUS */
#define BTIF_AV_MIN_SYNCHRONOUS_LATENCY 4 /* min latency in ms for BTIF_AV_FEEDING_SYNCHRONOUS */

typedef UINT8 tBTIF_AV_FEEDING_MODE;

#define BTIF_AV_CHANNEL_MODE_MONO    A2D_SBC_IE_CH_MD_MONO
#define BTIF_AV_CHANNEL_MODE_STEREO  A2D_SBC_IE_CH_MD_STEREO
#define BTIF_AV_CHANNEL_MODE_JOINT   A2D_SBC_IE_CH_MD_JOINT
#define BTIF_AV_CHANNEL_MODE_DUAL    A2D_SBC_IE_CH_MD_DUAL

typedef UINT8 tBTIF_AV_CHANNEL_MODE;

/**
 * Structure used to configure the AV codec capabilities/config
 */
typedef struct
{
    tBTIF_AV_CODEC_ID id;            /* Codec ID (in terms of BTIF) */
    UINT8 info[AVDT_CODEC_SIZE];     /* Codec info (can be config or capabilities) */
} tBTIF_AV_CODEC_INFO;

/**
 * Structure used to configure the AV media feeding
 */
typedef struct
{
    UINT16 sampling_freq;   /* 44100, 48000 etc */
    UINT16 num_channel;     /* 1 for mono or 2 stereo */
    UINT8  bit_per_sample;  /* Number of bits per sample (8, 16) */
} tBTIF_AV_MEDIA_FEED_CFG_PCM;

typedef union
{
    tBTIF_AV_MEDIA_FEED_CFG_PCM pcm;     /* Raw PCM feeding format */
}tBTIF_AV_MEDIA_FEED_CFG;

typedef struct
{
    tBTIF_AV_CODEC_ID format;        /* Media codec identifier */
    tBTIF_AV_MEDIA_FEED_CFG cfg;     /* Media codec configuration */
} tBTIF_AV_MEDIA_FEEDINGS;


#ifdef __cplusplus
}
#endif

#endif /* BTIF_AV_API_H */
