/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <errno.h>

#include "esp_log.h"

#include "sdkconfig.h"

#include "esp_ble_audio_lc3_defs.h"
#include "esp_ble_audio_aics_api.h"
#include "esp_ble_audio_cap_api.h"
#include "esp_ble_audio_pacs_api.h"
#include "esp_ble_audio_vcp_api.h"
#include "esp_ble_audio_tbs_api.h"
#include "esp_ble_audio_tmap_api.h"
#include "esp_ble_audio_csip_api.h"
#include "esp_ble_audio_mcs_defs.h"
#include "esp_ble_audio_mcc_api.h"
#include "esp_ble_audio_media_proxy_api.h"
#include "esp_ble_audio_vocs_api.h"
#include "esp_ble_audio_tbs_api.h"

#include "ble_audio_example_init.h"
#include "ble_audio_example_utils.h"

#define TAG "TMAP_CEN"

#define CONN_HANDLE_INIT        0xFFFF

#define LOCAL_DEVICE_NAME       "TMAP Central"

/* Topology from EXAMPLE_TMAP_CEN_TOPOLOGY. CIS #0 is bidirectional, a second is
 * TX-only. Note: CONFIG_BT_ACL_CONNECTIONS is Bluedroid-only. */
#if CONFIG_EXAMPLE_TMAP_CEN_TOPO_1P_1CIS
#define TMAP_CEN_PEER_COUNT     1
#define TMAP_CEN_CIS_COUNT      1
#elif CONFIG_EXAMPLE_TMAP_CEN_TOPO_1P_2CIS
#define TMAP_CEN_PEER_COUNT     1
#define TMAP_CEN_CIS_COUNT      2
#else /* CONFIG_EXAMPLE_TMAP_CEN_TOPO_2P_2CIS */
#define TMAP_CEN_PEER_COUNT     2
#define TMAP_CEN_CIS_COUNT      2
#endif

/* Sink ASEs recorded per peripheral: as many as the client caches, so a peer
 * offering more than this topology streams on is not flagged as unhandled. */
#define TMAP_CEN_SINK_EP_MAX    CONFIG_BT_BAP_UNICAST_CLIENT_ASE_SNK_COUNT

#if TMAP_CEN_PEER_COUNT == 1
_Static_assert(TMAP_CEN_SINK_EP_MAX >= TMAP_CEN_CIS_COUNT,
               "One peripheral must expose a sink ASE per CIS");
#endif

#define SCAN_INTERVAL           160     /* 100ms */
#define SCAN_WINDOW             160     /* 100ms */

/* ACL init parameters shared between bluedroid and nimble host wrappers.
 * Raw HCI units (scan: 0.625ms; conn interval: 1.25ms; timeout: 10ms). */
#define INIT_SCAN_INTERVAL      16      /* 10ms */
#define INIT_SCAN_WINDOW        16      /* 10ms */
#define CONN_INTERVAL           24      /* 30ms */
#define CONN_LATENCY            0
#define CONN_TIMEOUT            500     /* 5s */
#define CONN_MIN_CE_LEN         0xFFFF
#define CONN_MAX_CE_LEN         0xFFFF

int app_host_init(void);

int set_device_name(void);

int ext_scan_start(void);
int ext_scan_stop(void);

int conn_create(uint8_t addr_type, const uint8_t addr[6]);

int pairing_start(uint16_t conn_handle);

int exchange_mtu(uint16_t conn_handle);

void security_failed_recover(uint16_t conn_handle, uint8_t status);

size_t set_member_count(void);

uint16_t set_member_handle(size_t index);

const uint8_t *set_member_addr(uint16_t conn_handle);

void set_member_discovered(uint16_t conn_handle, int err);

void set_lock_complete(int err);

void set_release_lock(void);

#if TMAP_CEN_PEER_COUNT > 1
int csip_set_coordinator_init(void);

bool csip_adv_is_set_member(uint8_t ad_type, const uint8_t *data, uint8_t data_len);

int csip_discover_start(uint16_t conn_handle);

uint8_t csip_member_rank_get(uint16_t conn_handle);

int csip_lock_set(const uint16_t *conn_handles, size_t count);

int csip_release_set(const uint16_t *conn_handles, size_t count);
#else /* TMAP_CEN_PEER_COUNT > 1 */
static inline int csip_set_coordinator_init(void)
{
    return 0;
}

static inline bool csip_adv_is_set_member(uint8_t ad_type, const uint8_t *data, uint8_t data_len)
{
    (void)ad_type;
    (void)data;
    (void)data_len;
    return false;
}

static inline int csip_discover_start(uint16_t conn_handle)
{
    (void)conn_handle;
    return -ENOTSUP;
}

static inline uint8_t csip_member_rank_get(uint16_t conn_handle)
{
    (void)conn_handle;
    return 0;
}

static inline int csip_lock_set(const uint16_t *conn_handles, size_t count)
{
    (void)conn_handles;
    (void)count;
    return -ENOTSUP;
}

static inline int csip_release_set(const uint16_t *conn_handles, size_t count)
{
    (void)conn_handles;
    (void)count;
    return -ENOTSUP;
}
#endif /* TMAP_CEN_PEER_COUNT > 1 */

int mcp_server_init(void);

int ccp_server_init(void);

int vcp_vol_ctlr_init(void);

int vcp_vol_ctlr_discover(uint16_t conn_handle);

void vcp_vol_ctlr_streams_started(void);

void vcp_vol_ctlr_streams_stopped(void);

void vcp_vol_ctlr_forget(uint16_t conn_handle);

void vcp_vol_ctlr_reset(void);

int vcp_vol_ctlr_set_volume(uint8_t volume);

esp_ble_audio_cap_set_type_t set_type_get(void);

bool set_member_fill(esp_ble_audio_cap_set_member_t *member,
                     esp_ble_audio_cap_set_type_t type,
                     uint16_t conn_handle);

int cap_initiator_init(void);

int cap_initiator_setup(void);

void peer_forget(uint16_t conn_handle);

void cap_initiator_reset(void);
