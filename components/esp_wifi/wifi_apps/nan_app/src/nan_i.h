/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Internal declarations shared between nan_app.c and nan_security.c.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/queue.h>
#include "esp_err.h"
#include "esp_wifi_types_generic.h"
#include "esp_private/wifi.h"
#include "esp_nan.h"
#include "os.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Macros */
#ifndef MACADDR_LEN
#define MACADDR_LEN             6
#endif
#define MACADDR_EQUAL(a1, a2)   (memcmp(a1, a2, MACADDR_LEN) == 0)
#define MACADDR_COPY(dst, src)  (memcpy(dst, src, MACADDR_LEN))

/*
 * Shared lock used by both files.
 *
 * NAN_DATA_LOCK contract
 * ----------------------
 * s_nan_data_lock guards s_nan_ctx (NDL[], own_svc[], state, event,
 * netif). Anything that reads or mutates these fields takes the lock.
 *
 * !!! MUST NOT be held across any esp_nan_internal_* call !!!
 *
 * The blob-side esp_nan_internal_* entry points (datapath_req,
 * datapath_resp, datapath_end, publish_service, subscribe_service,
 * send_followup, register_callbacks) re-enter host code from the WiFi
 * task to:
 *   - assemble M1 / M2 / M3 / M4 NDP frames (esp_nan_get_ndp_*_key_desc,
 *     esp_nan_construct_csia / scia, esp_nan_capture_m1_auth_token,
 *     esp_nan_update_ndp_*_mic, esp_nan_verify_ndp_*_mic)
 *   - parse inbound NDP frames (esp_nan_parse_ndp_*)
 *   - fire indication / confirm / response_indication / terminated
 *     callbacks (nan_app_ndp_*_cb)
 *
 * All of those re-entry points take NAN_DATA_LOCK themselves. Holding
 * the lock around the blob call deadlocks the WiFi task as soon as it
 * tries to call back. Pattern:
 *
 *     NAN_DATA_LOCK();
 *     ... mutate / capture state needed by the call ...
 *     NAN_DATA_UNLOCK();
 *     err = esp_nan_internal_datapath_req(...);
 *     NAN_DATA_LOCK();
 *     ... record result ...
 *     NAN_DATA_UNLOCK();
 *
 * Several past bug-fix commits on this branch (`fix(nan): release
 * NAN_DATA_LOCK before initiator datapath_req`, `fix(nan): unlock
 * NAN_DATA on all paths in ndp_indication_cb`) trace back to forgotten
 * unlocks; new sites that call the blob must follow this pattern.
 */
extern void *s_nan_data_lock;
#define NAN_DATA_LOCK()   os_mutex_lock(s_nan_data_lock)
#define NAN_DATA_UNLOCK() os_mutex_unlock(s_nan_data_lock)

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
/* NAN 4-way handshake constants (RSNA key descriptor layout) */
#define NAN_NONCE_LEN           32
#define NAN_REPLAY_COUNTER_LEN  8
#define NAN_KEY_RSC_LEN         8
#define NAN_KEY_MIC_LEN         16
/* KCK/KEK/TK buffer sizes are sized for the largest cipher suite the host
 * could potentially run (NCS-SK-256 KCK=24, KEK=32, TK=32). Only NCS-SK-128
 * is wired through M1–M4 today (see nan_get_first_csid) so the *_set fields
 * mark the live length in the buffer. */
#define NAN_ND_KCK_MAX_LEN      24
#define NAN_ND_KEK_MAX_LEN      32
#define NAN_ND_TK_MAX_LEN       32
#define NAN_GTK_MAX_LEN         32
#define NAN_AUTH_TOKEN_MAX_LEN  24

/* RSNA Key Descriptor offsets (same as 802.11 EAPOL-Key) */
#define NAN_KEY_DESC_TYPE_OFF        0
#define NAN_KEY_DESC_KEY_INFO_OFF    1
#define NAN_KEY_DESC_KEY_LEN_OFF     3
#define NAN_KEY_DESC_REPLAY_OFF      5
#define NAN_KEY_DESC_NONCE_OFF       13
#define NAN_KEY_DESC_IV_OFF          45
#define NAN_KEY_DESC_RSC_OFF         61
#define NAN_KEY_DESC_KEY_ID_OFF      69
#define NAN_KEY_DESC_MIC_OFF         77
#define NAN_KEY_DESC_DATA_LEN_OFF    93
#define NAN_KEY_DESC_DATA_OFF        95
#define NAN_KEY_DESC_MIN_LEN         95

/* Key Descriptor Type (same as 802.11 EAPOL-Key) */
#define NAN_KEY_DESC_TYPE_RSN  2

/* Security Context Identifier (SCID) types (Table 123) */
#define NAN_SEC_CTX_TYPE_ND_PMKID  1

/* Key Info bits (same semantics as RSNA) */
#define NAN_KEY_INFO_MIC      BIT(8)
#define NAN_KEY_INFO_SECURE   BIT(9)
#define NAN_KEY_INFO_INSTALL  BIT(6)
#define NAN_KEY_INFO_ACK      BIT(7)
#define NAN_KEY_INFO_ENC_KEY  BIT(12)
#define NAN_KEY_INFO_KEY_TYPE BIT(3)   /* 1=Pairwise, 0=Group */

/* NCS-SK-128 only for now (Table 21): KCK 128 bits, KEK 128 bits, TK 128 bits, MIC 16 bytes */
#define NAN_NCS_SK_128_KCK_LEN  16
#define NAN_NCS_SK_128_KEK_LEN  16
#define NAN_NCS_SK_128_TK_LEN   16
#define NAN_NCS_SK_128_MIC_LEN  16
#define NAN_NCS_SK_128_PTK_LEN  (NAN_NCS_SK_128_KCK_LEN + NAN_NCS_SK_128_KEK_LEN + NAN_NCS_SK_128_TK_LEN)

/* Internal key-install constants matching esp_wifi_set_sta_key_internal semantics. */
#define NAN_WIFI_WPA_ALG_CCMP    3
#define NAN_KEY_FLAG_RX          BIT(2)
#define NAN_KEY_FLAG_TX          BIT(3)
#define NAN_KEY_FLAG_PAIRWISE    BIT(5)

/* Handshake state */
enum nan_handshake_state {
    NAN_HANDSHAKE_IDLE = 0,
    NAN_HANDSHAKE_M1_SENT,            /* Initiator: sent NDP Request (M1) */
    NAN_HANDSHAKE_M1_RCVD,
    NAN_HANDSHAKE_M2_SENT,
    NAN_HANDSHAKE_M2_RCVD,
    NAN_HANDSHAKE_M3_SENT,
    NAN_HANDSHAKE_M3_PENDING_VERIFY,  /* Responder: M3 parsed, awaits Auth_Token||body MIC verify */
    NAN_HANDSHAKE_M3_RCVD,
    NAN_HANDSHAKE_M4_RCVD,
    NAN_HANDSHAKE_COMPLETE
};
#endif /* CONFIG_ESP_WIFI_NAN_SECURITY */

/* Per-peer service info */
struct peer_svc_info {
    SLIST_ENTRY(peer_svc_info) next;
    uint8_t peer_svc_info[ESP_WIFI_MAX_SVC_INFO_LEN];
    uint8_t svc_id;
    uint8_t own_svc_id;
    uint8_t type;
    uint8_t peer_nmi[MACADDR_LEN];
    uint32_t device_caps;
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    /* Per-(own_svc, peer) result of subscriber-side PMKID match at SDF RX.
     * 0xFF = no match; otherwise the index into own_svc->user_cfg.creds[]
     * whose ND-PMKID matched the publisher SCIA. The initiator NDP-req path
     * uses this to pick the right credential for M1's pair-PMKID. */
    uint8_t matched_cred_idx;
#endif
};

/* Own (locally registered) service info */
struct own_svc_info {
    char svc_name[ESP_WIFI_MAX_SVC_NAME_LEN];
    uint8_t svc_id;
    uint8_t type;

    bool ndp_resp_needed;
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    /* App-provided input (creds[] / csid_bitmap / group prot flags). */
    wifi_nan_discovery_security_params_t user_cfg;
    /* Derived material — one ND-PMK + ND-PMKID per credential. Mirrors the
     * per-service array the blob caches in svc_entry->self_security_params[].
     * Valid entries: [0, user_cfg.num_credentials). */
    wifi_nan_security_params_t derived_security[ESP_WIFI_NAN_MAX_CREDS_PER_SVC];
#endif
    uint8_t num_peer_records;
    SLIST_HEAD(peer_list_t, peer_svc_info) peer_list;
};

/* Per-NDP link state */
struct ndl_info {
    uint8_t ndp_id;
    uint8_t peer_ndi[MACADDR_LEN];
    uint8_t peer_nmi[MACADDR_LEN];
    uint8_t publisher_id;
    uint8_t own_role;
    uint32_t device_caps;
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    wifi_nan_security_params_t security_ctx;

    uint8_t anonce[NAN_NONCE_LEN];
    uint8_t snonce[NAN_NONCE_LEN];

    uint8_t nd_kck[NAN_ND_KCK_MAX_LEN];
    uint8_t nd_kek[NAN_ND_KEK_MAX_LEN];
    uint8_t nd_tk[NAN_ND_TK_MAX_LEN];
    uint8_t kck_len;
    uint8_t kek_len;
    uint8_t tk_len;
    uint8_t ptk_set: 1;
    uint8_t ptk_reserved: 7;

    uint8_t tx_replay_counter[NAN_REPLAY_COUNTER_LEN];
    uint8_t rx_replay_counter[NAN_REPLAY_COUNTER_LEN];
    uint8_t rx_replay_counter_set: 1;
    uint8_t replay_reserved: 7;

    uint8_t auth_token[NAN_AUTH_TOKEN_MAX_LEN];
    uint8_t auth_token_len;

    uint8_t handshake_state;

    /* Group key state (unsupported -- pairwise-only M1-M4 flow today;
     * fields kept to match the spec-defined RSNA key descriptor layout
     * and stay forward-compatible). */
    uint8_t gtk[NAN_GTK_MAX_LEN];
    uint8_t igtk[NAN_GTK_MAX_LEN];
    uint8_t bigtk[NAN_GTK_MAX_LEN];
    uint8_t gtk_len;
    uint8_t igtk_len;
    uint8_t bigtk_len;
    uint8_t gtk_set: 1;
    uint8_t igtk_set: 1;
    uint8_t bigtk_set: 1;
    uint8_t group_keys_reserved: 5;

    uint8_t key_rsc[NAN_KEY_RSC_LEN];
#endif
};

/* NAN context shared between files */
typedef struct {
    uint8_t state;
    uint8_t event;
    struct ndl_info ndl[ESP_WIFI_NAN_DATAPATH_MAX_PEERS];
    struct own_svc_info own_svc[ESP_WIFI_NAN_MAX_SVC_SUPPORTED];
    esp_netif_t *nan_netif;
} nan_ctx_t;

extern nan_ctx_t s_nan_ctx;

/* Helpers defined in nan_app.c, used by nan_security.c */
struct own_svc_info *nan_find_own_svc(uint8_t svc_id);
struct own_svc_info *nan_find_own_svc_by_name(const char *svc_name);
struct ndl_info *nan_find_ndl(uint8_t ndp_id, uint8_t peer_nmi[]);
struct ndl_info *nan_find_ndl_by_pub_id_and_peer(uint8_t pub_id, const uint8_t *peer_nmi);

/* === nan_secure_dp_funcs initializer targets === */

/* Always-present (defined in nan_app.c) */
void     esp_nan_ndp_tx_done_cb(uint8_t ndp_id, const uint8_t *peer_nmi,
                                uint8_t msg_type, bool tx_status);

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
/* Security-gated (defined in nan_security.c) */
uint32_t esp_nan_get_csia_len(uint16_t own_csid_bitmap, uint16_t peer_csid_bitmap);
uint32_t esp_nan_get_scia_len(uint8_t num_pmkids);
uint32_t esp_nan_get_shared_key_desc_attr_len(uint16_t key_data_len);
int      esp_nan_ndp_security_install_get_shared_desc_len(void);

int  esp_nan_construct_csia(uint8_t *frm, uint8_t pub_id,
                            uint16_t own_csid_bitmap, uint16_t peer_csid_bitmap);
int  esp_nan_construct_scia_publish(uint8_t *frm, uint8_t pub_id,
                                    uint8_t num_pmkids,
                                    const uint8_t pmkids[][ESP_WIFI_NAN_NDP_PMKID_LEN]);
int  esp_nan_construct_scia_ndp_req(uint8_t *frm, uint8_t ndp_id,
                                    const uint8_t *peer_nmi);
int  esp_nan_construct_scia_ndp_resp(uint8_t *frm, uint8_t ndp_id,
                                     const uint8_t *peer_nmi);

int  esp_nan_get_ndp_req_shared_key_desc(uint8_t *buf, size_t buf_len,
                                         uint8_t ndp_id, const uint8_t *peer_nmi);
int  esp_nan_get_ndp_resp_shared_key_desc(uint8_t *buf, size_t buf_len,
                                          uint8_t ndp_id, const uint8_t *peer_nmi);
int  esp_nan_get_ndp_confirm_shared_key_desc(uint8_t *buf, size_t buf_len,
                                             uint8_t ndp_id, const uint8_t *peer_nmi);
int  esp_nan_get_ndp_security_install_key_desc(uint8_t *buf, size_t buf_len,
                                               uint8_t ndp_id, const uint8_t *peer_nmi);

int  esp_nan_capture_m1_auth_token(const uint8_t *m1_body, size_t body_len,
                                   uint8_t ndp_id, const uint8_t *peer_nmi);

int  esp_nan_update_ndp_resp_mic(uint8_t *m2_body, size_t body_len,
                                 uint8_t *key_desc_attr,
                                 uint8_t ndp_id, const uint8_t *peer_nmi);
int  esp_nan_update_ndp_confirm_mic(uint8_t *m3_body, size_t body_len,
                                    uint8_t *key_desc_attr,
                                    uint8_t ndp_id, const uint8_t *peer_nmi);
int  esp_nan_update_ndp_security_install_mic(uint8_t *m4_body, size_t body_len,
                                             uint8_t *key_desc_attr,
                                             uint8_t ndp_id, const uint8_t *peer_nmi);

int  esp_nan_verify_ndp_resp_mic(uint8_t *m2_body, size_t body_len,
                                 uint8_t *key_desc_attr,
                                 uint8_t ndp_id, const uint8_t *peer_nmi);
int  esp_nan_verify_ndp_confirm_mic(uint8_t *m3_body, size_t body_len,
                                    uint8_t *key_desc_attr,
                                    uint8_t ndp_id, const uint8_t *peer_nmi);
int  esp_nan_verify_ndp_security_install_mic(uint8_t *m4_body, size_t body_len,
                                             uint8_t *key_desc_attr,
                                             uint8_t ndp_id, const uint8_t *peer_nmi);

void esp_nan_parse_ndp_csia(void *frm, size_t buf_len, wifi_nan_security_params_t *param);
void esp_nan_parse_ndp_scia(void *frm, size_t buf_len, wifi_nan_security_params_t *param);
void esp_nan_parse_ndp_key_desc(void *frm, size_t buf_len, uint8_t ndp_id, const uint8_t *peer_nmi);

esp_err_t esp_nan_parse_publish_security(const uint8_t *attrs, size_t attrs_len,
                                         wifi_nan_peer_sdf_security_t *security);

/* Helpers defined in nan_security.c, used by nan_app.c */

/*
 * Apply any pending CSIA/SCIA/M1 state captured by the NDP key-desc parser
 * onto the freshly-created NDL. Called from nan_app_ndp_indication_cb once
 * (ndp_id, peer_nmi, peer_ndi, p_own_svc) are all known.
 */
void nan_security_apply_pending(struct ndl_info *ndl,
                                struct own_svc_info *p_own_svc,
                                uint8_t pub_id,
                                const uint8_t *peer_nmi,
                                const uint8_t *peer_ndi);

/* PMK / PMKID derivation entry point used by the publish path. Derives from
 * (service_name, sec_cfg) (passphrase or PMK) and writes the result into
 * out_derived. */
esp_err_t nan_derive_security_params(const char *service_name,
                                     const wifi_nan_discovery_security_params_t *sec_cfg,
                                     wifi_nan_security_params_t *out_derived);

/* Blob-side gate query: returns ndl->security_ctx.csid_bitmap for the NDP
 * keyed on (ndp_id, peer_nmi), or 0 if no NDL match. ndp_id=0 is valid for
 * the initiator pre-claim window (peer-only lookup). */
uint16_t nan_get_ndp_security_csid(uint8_t ndp_id, const uint8_t *peer_nmi);

/* Subscribe path: populate the initiator NDL's security_ctx (cipher + pair-PMKID +
 * ND-PMK) from the local subscribe's own_svc and the SDF-time matched credential
 * recorded on peer_svc. The blob's CSIA / SCIA / Shared-Key-Descriptor builder
 * callbacks look up security keyed by (ndp_id, peer_nmi). No-op if the local
 * subscribe didn't request encrypted datapath. */
esp_err_t nan_security_populate_initiator_ndl(struct ndl_info *ndl,
                                              const struct own_svc_info *own_svc,
                                              const struct peer_svc_info *peer_svc,
                                              const uint8_t *peer_nmi);

/*
 * Compare locally derived ND-PMKID (subscriber passphrase, publisher NMI) to
 * peer discovery security params. own_svc identifies the local subscribe
 * (service_name + creds[]) and peer_svc is updated with matched_cred_idx on
 * success. Returns true if any local cred's PMKID matches a peer-advertised one.
 */
bool nan_security_service_match(const struct own_svc_info *own_svc,
                                struct peer_svc_info *peer_svc,
                                const uint8_t *publisher_nmi,
                                const wifi_nan_peer_sdf_security_t *peer_sec);
#else
static inline esp_err_t nan_derive_security_params(const char *service_name,
                                                   const wifi_nan_discovery_security_params_t *sec_cfg,
                                                   wifi_nan_security_params_t *out_derived)
{
    (void)service_name;
    (void)sec_cfg;
    (void)out_derived;
    return ESP_FAIL;
}
#endif /* CONFIG_ESP_WIFI_NAN_SECURITY */

#ifdef __cplusplus
}
#endif
