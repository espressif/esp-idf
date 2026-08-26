/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTA_HFP_DEFS_H__
#define __BTA_HFP_DEFS_H__

#include "stack/btm_api.h"

#define BTA_HFP_CODEC_NONE           BTM_SCO_CODEC_NONE
#define BTA_HFP_CODEC_CVSD           BTM_SCO_CODEC_CVSD      /* CVSD */
#define BTA_HFP_CODEC_MSBC           BTM_SCO_CODEC_MSBC      /* mSBC */

typedef UINT16 tBTA_HFP_PEER_CODEC;

#ifndef BTA_HFP_SCO_OUT_PKT_SIZE
#define BTA_HFP_SCO_OUT_PKT_SIZE    BTM_SCO_DATA_SIZE_MAX
#endif

#define BTA_HFP_SCO_CODEC_PCM       0       /* used for regular SCO */
#define BTA_HFP_SCO_CODEC_SBC       1       /* used for WBS */
typedef UINT8   tBTA_HFP_SCO_CODEC_TYPE;

#define BTA_HFP_SCO_SAMP_RATE_8K     8000
#define BTA_HFP_SCO_SAMP_RATE_16K    16000

/* SCO codec information */
typedef struct {
    tBTA_HFP_SCO_CODEC_TYPE   codec_type;
} tBTA_HFP_CODEC_INFO;

#define BTA_HFP_SCO_ROUTE_PCM    BTM_SCO_ROUTE_PCM
#define BTA_HFP_SCO_ROUTE_HCI    BTM_SCO_ROUTE_HCI

typedef tBTM_SCO_ROUTE_TYPE tBTA_HFP_SCO_ROUTE_TYPE;

#endif /* __BTA_HFP_DEFS_H__ */
