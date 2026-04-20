/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_INIT_H_
#define HOST_NIMBLE_INIT_H_

#include <stdint.h>

#include "nimble/profiles/ascs.h"
#include "nimble/profiles/bass.h"
#include "nimble/profiles/cas.h"
#include "nimble/profiles/csis.h"
#include "nimble/profiles/mcs.h"
#include "nimble/profiles/mics.h"
#include "nimble/profiles/pacs.h"
#include "nimble/profiles/tbs.h"
#include "nimble/profiles/tmas.h"
#include "nimble/profiles/vcs.h"
#include "nimble/profiles/has.h"

#ifdef __cplusplus
extern "C" {
#endif

int bt_le_nimble_audio_init(void);

int bt_le_nimble_media_proxy_pl_init(void);

int bt_le_nimble_vcp_vol_rend_init(void);

int bt_le_nimble_micp_mic_dev_init(void);

int bt_le_nimble_audio_start(void *info);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_INIT_H_ */
