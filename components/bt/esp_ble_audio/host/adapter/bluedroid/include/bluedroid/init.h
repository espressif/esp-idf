/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_BLUEDROID_INIT_H_
#define HOST_BLUEDROID_INIT_H_

#include <stdint.h>

#include "bluedroid/server.h"
#include "bluedroid/profiles/ascs.h"
#include "bluedroid/profiles/bass.h"
#include "bluedroid/profiles/cas.h"
#include "bluedroid/profiles/csis.h"
#include "bluedroid/profiles/mcs.h"
#include "bluedroid/profiles/mics.h"
#include "bluedroid/profiles/pacs.h"
#include "bluedroid/profiles/tbs.h"
#include "bluedroid/profiles/tmas.h"
#include "bluedroid/profiles/vcs.h"
#include "bluedroid/profiles/has.h"

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_bluedroid_audio_init(void);

int bt_le_bluedroid_media_proxy_pl_init(void);

int bt_le_bluedroid_vcp_vol_rend_init(void);

int bt_le_bluedroid_micp_mic_dev_init(void);

int bt_le_bluedroid_audio_start(void *info);

#ifdef __cplusplus
}
#endif

#endif /* HOST_BLUEDROID_INIT_H_ */
