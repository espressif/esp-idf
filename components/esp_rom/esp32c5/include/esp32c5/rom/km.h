/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _KM_H
#define _KM_H

#include "soc/soc_caps.h"
#if SOC_KEY_MANAGER_SUPPORTED

#include <stdint.h>
#include "soc/soc.h"
#include "ets_sys.h"

#if __cplusplus
extern "C" {
#endif

/* huk mode type */
typedef enum {
    HUK_MODE_RECOVER = 0,
    HUK_MODE_GEN = 1,
} huk_mode_t;

/**
  * @brief Recover efuse key or key manager key if flash encryption is enabled
  *
  * @param do_log : if km process print log
  *
  * @return ETS_OK when key is recovered, ETS_FAILED when key not recovered
  */
ETS_STATUS esp_rom_check_recover_key(int do_log);

/**
  * @brief Configure huk mode
  *
  * @param mode : HUK_MODE_RECOVER or HUK_MODE_GEN
  *
  * @param huk_info : uint8_t pointer to the buffer which will feed the huk info or
  *                   gain the huk info
  *
  * @return ETS_OK when huk configuration is done, else ETS_FAILED
  */
ETS_STATUS esp_rom_km_huk_conf(huk_mode_t mode, uint8_t *huk_info);

/**
  * @brief Get huk risk. The risk level of HUK is 0-6: the higher the risk level is,
  *        the more error bits there are in the PUF SRAM. 7: Error level, HUK is invalid
  *
  * @param None
  *
  * @return The huk risk
  */
int esp_rom_km_huk_risk(void);

#ifdef __cplusplus
}
#endif
#endif /* SOC_KEY_MANAGER_SUPPORTED */

#endif /* _KM_H */
