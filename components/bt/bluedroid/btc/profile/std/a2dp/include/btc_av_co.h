// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __BTC_AV_CO_H__
#define __BTC_AV_CO_H__

#include "btc_a2dp.h"
#include "bta/bta_av_co.h"

#if (BTA_AV_INCLUDED == TRUE)
/*******************************************************************************
**  Constants & Macros
********************************************************************************/

enum {
    BTC_SV_AV_AA_SBC_INDEX = 0,
    BTC_SV_AV_AA_SBC_SINK_INDEX,
    BTC_SV_AV_AA_SEP_INDEX  /* Last index */
};

/*****************************************************************************
**  Local data
*****************************************************************************/
typedef struct {
    UINT8 sep_info_idx;                 /* local SEP index (in BTA tables) */
    UINT8 seid;                         /* peer SEP index (in peer tables) */
    UINT8 codec_type;                   /* peer SEP codec type */
    UINT8 codec_caps[AVDT_CODEC_SIZE];  /* peer SEP codec capabilities */
    UINT8 num_protect;                  /* peer SEP number of CP elements */
    UINT8 protect_info[BTA_AV_CP_INFO_LEN];  /* peer SEP content protection info */
} tBTA_AV_CO_SINK;

typedef struct {
    BD_ADDR         addr;               /* address of audio/video peer */
    tBTA_AV_CO_SINK snks[BTC_SV_AV_AA_SEP_INDEX]; /* array of supported sinks */
    tBTA_AV_CO_SINK srcs[BTC_SV_AV_AA_SEP_INDEX]; /* array of supported srcs */
    UINT8           num_snks;           /* total number of sinks at peer */
    UINT8           num_srcs;           /* total number of srcs at peer */
    UINT8           num_seps;           /* total number of seids at peer */
    UINT8           num_rx_snks;        /* number of received sinks */
    UINT8           num_rx_srcs;        /* number of received srcs */
    UINT8           num_sup_snks;       /* number of supported sinks in the snks array */
    UINT8           num_sup_srcs;       /* number of supported srcs in the srcs array */
    tBTA_AV_CO_SINK *p_snk;             /* currently selected sink */
    tBTA_AV_CO_SINK *p_src;             /* currently selected src */
    UINT8           codec_cfg[AVDT_CODEC_SIZE]; /* current codec configuration */
    BOOLEAN         cp_active;          /* current CP configuration */
    BOOLEAN         acp;                /* acceptor */
    BOOLEAN         recfg_needed;       /* reconfiguration is needed */
    BOOLEAN         opened;             /* opened */
    UINT16          mtu;                /* maximum transmit unit size */
    UINT16          uuid_to_connect;    /* uuid of peer device */
} tBTA_AV_CO_PEER;

typedef struct {
    BOOLEAN active;
    UINT8 flag;
} tBTA_AV_CO_CP;

typedef struct {
    /* Connected peer information */
    tBTA_AV_CO_PEER peers[BTA_AV_NUM_STRS];
    /* Current codec configuration - access to this variable must be protected */
    tBTC_AV_CODEC_INFO codec_cfg;
    tBTC_AV_CODEC_INFO codec_cfg_setconfig; /* remote peer setconfig preference */

    tBTA_AV_CO_CP cp;
} tBTA_AV_CO_CB;

/* Control block instance */
#if AVRC_DYNAMIC_MEMORY == FALSE
extern tBTA_AV_CO_CB bta_av_co_cb;
#else
extern tBTA_AV_CO_CB *bta_av_co_cb_ptr;
#define bta_av_co_cb (*bta_av_co_cb_ptr)
#endif
/*******************************************************************************
**  Functions
********************************************************************************/

/*******************************************************************************
 **
 ** Function         bta_av_co_cp_is_active
 **
 ** Description      Get the current configuration of content protection
 **
 ** Returns          TRUE if the current streaming has CP, FALSE otherwise
 **
 *******************************************************************************/
BOOLEAN bta_av_co_cp_is_active(void);

/*******************************************************************************
 **
 ** Function         bta_av_co_cp_get_flag
 **
 ** Description      Get content protection flag
 **                  BTA_AV_CP_SCMS_COPY_NEVER
 **                  BTA_AV_CP_SCMS_COPY_ONCE
 **                  BTA_AV_CP_SCMS_COPY_FREE
 **
 ** Returns          The current flag value
 **
 *******************************************************************************/
UINT8 bta_av_co_cp_get_flag(void);

/*******************************************************************************
 **
 ** Function         bta_av_co_cp_set_flag
 **
 ** Description      Set content protection flag
 **                  BTA_AV_CP_SCMS_COPY_NEVER
 **                  BTA_AV_CP_SCMS_COPY_ONCE
 **                  BTA_AV_CP_SCMS_COPY_FREE
 **
 ** Returns          TRUE if setting the SCMS flag is supported else FALSE
 **
 *******************************************************************************/
BOOLEAN bta_av_co_cp_set_flag(UINT8 cp_flag);

/*******************************************************************************
 **
 ** Function         bta_av_co_audio_codec_reset
 **
 ** Description      Reset the current codec configuration
 **
 ** Returns          void
 **
 *******************************************************************************/
void bta_av_co_audio_codec_reset(void);

/*******************************************************************************
 **
 ** Function         bta_av_co_audio_codec_supported
 **
 ** Description      Check if all opened connections are compatible with a codec
 **                  configuration
 **
 ** Returns          TRUE if all opened devices support this codec, FALSE otherwise
 **
 *******************************************************************************/
BOOLEAN bta_av_co_audio_codec_supported(tBTC_AV_STATUS *p_status);

/*******************************************************************************
 **
 ** Function         bta_av_co_audio_set_codec
 **
 ** Description      Set the current codec configuration from the feeding type.
 **                  This function is starting to modify the configuration, it
 **                  should be protected.
 **
 ** Returns          TRUE if successful, FALSE otherwise
 **
 *******************************************************************************/
BOOLEAN bta_av_co_audio_set_codec(const tBTC_AV_MEDIA_FEEDINGS *p_feeding, tBTC_AV_STATUS *p_status);

/*******************************************************************************
 **
 ** Function         bta_av_co_audio_get_sbc_config
 **
 ** Description      Retrieves the SBC codec configuration.  If the codec in use
 **                  is not SBC, return the default SBC codec configuration.
 **
 ** Returns          TRUE if codec is SBC, FALSE otherwise
 **
 *******************************************************************************/
BOOLEAN bta_av_co_audio_get_sbc_config(tA2D_SBC_CIE *p_sbc_config, UINT16 *p_minmtu);

/*******************************************************************************
 **
 ** Function         bta_av_co_audio_discard_config
 **
 ** Description      Discard the codec configuration of a connection
 **
 ** Returns          Nothing
 **
 *******************************************************************************/
void bta_av_co_audio_discard_config(tBTA_AV_HNDL hndl);

/*******************************************************************************
 **
 ** Function         bta_av_co_init
 **
 ** Description      Initialization
 **
 ** Returns          Nothing
 **
 *******************************************************************************/
void bta_av_co_init(void);


/*******************************************************************************
 **
 ** Function         bta_av_co_peer_cp_supported
 **
 ** Description      Checks if the peer supports CP
 **
 ** Returns          TRUE if the peer supports CP
 **
 *******************************************************************************/
BOOLEAN bta_av_co_peer_cp_supported(tBTA_AV_HNDL hndl);

/*******************************************************************************
 **
 ** Function         bta_av_co_get_remote_bitpool_pref
 **
 ** Description      Check if remote side did a setconfig within the limits
 **                  of our exported bitpool range. If set we will set the
 **                  remote preference.
 **
 ** Returns          TRUE if config set, FALSE otherwize
 **
 *******************************************************************************/
BOOLEAN bta_av_co_get_remote_bitpool_pref(UINT8 *min, UINT8 *max);

#endif  ///BTA_AV_INCLUDED == TRUE

#endif
