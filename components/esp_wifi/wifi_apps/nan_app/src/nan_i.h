/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Internal declarations shared between nan_app.c, nan_security.c, and nan_pairing.c.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/queue.h>
#include "esp_err.h"
#include "esp_bit_defs.h"
#include "esp_wifi_types_generic.h"
#include "esp_private/wifi.h"
#include "esp_nan.h"
#include "utils/common.h"      /* u8/u16 typedefs required by esp_wifi_driver.h */
#include "esp_wifi_driver.h"   /* wifi_nan_peer_creds_t + NAN credential NVS APIs */
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

/* NAN sync event-group bits and follow-up TX timeout (nan_app.c, nan_pairing.c). */
#ifndef NAN_DW_INTVL_MS
#define NAN_DW_INTVL_MS         524
#endif
#ifndef NAN_TX_SUCCESS
#define NAN_TX_SUCCESS          BIT(2)
#define NAN_TX_FAILURE          BIT(3)
#endif
#ifndef NAN_ACTION_TIMEOUT
#define NAN_ACTION_TIMEOUT      (4 * NAN_DW_INTVL_MS)
#endif

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

/* NAN KDE OUIs and Data Types carried in the Key Data field (Wi-Fi Aware
 * v4.0 §9.5.21.5 Table 126; formats per 802.11 Fig 12-36/12-42/12-47). */
#define NAN_KDE_OUI_RSN_0  0x00
#define NAN_KDE_OUI_RSN_1  0x0F
#define NAN_KDE_OUI_RSN_2  0xAC
#define NAN_KDE_OUI_WFA_0  0x50
#define NAN_KDE_OUI_WFA_1  0x6F
#define NAN_KDE_OUI_WFA_2  0x9A
#define NAN_KDE_OUI_RSN    0x000FACUL
#define NAN_KDE_OUI_WFA    0x506F9AUL
#define NAN_KDE_TYPE_GTK         1   /* 00-0F-AC GTK KDE */
#define NAN_KDE_TYPE_MAC         3   /* 00-0F-AC MAC address KDE */
#define NAN_KDE_TYPE_IGTK        9   /* 00-0F-AC IGTK KDE */
#define NAN_KDE_TYPE_BIGTK       14  /* 00-0F-AC BIGTK KDE */
#define NAN_KDE_TYPE_NIK         36  /* 50-6F-9A NIK KDE */
#define NAN_KDE_TYPE_KEY_LIFE    37  /* 50-6F-9A NAN Key Lifetime KDE */

/* KDE inner-prefix lengths (bytes before the actual key material). */
#define NAN_KDE_HDR_LEN          6   /* DD(1) + len(1) + OUI(3) + DataType(1) */
#define NAN_GTK_KDE_PREFIX_LEN   2   /* KeyID/Tx(1) + Reserved(1) */
#define NAN_IGTK_KDE_PREFIX_LEN  8   /* KeyID(2) + IPN(6) */
#define NAN_BIGTK_KDE_PREFIX_LEN 8   /* KeyID(2) + BIPN(6) */

/* CSIA Capabilities group-SA support (§9.5.21.2 Table 122, bits 1-2, bit 2 high
 * order). Mirrors hostap nan_defs.h NAN_CS_INFO_CAPA_GTK_SUPP_*. */
#define NAN_CSIA_CAP_GTK_SUPP_POS    1
#define NAN_CSIA_CAP_GTK_SUPP_MASK   0x06
#define NAN_CSIA_CAP_GTK_SUPP_NONE   0   /* 00: no group SA */
#define NAN_CSIA_CAP_GTK_SUPP_IGTK   1   /* 01: GTKSA + IGTKSA */
#define NAN_CSIA_CAP_GTK_SUPP_ALL    2   /* 10: GTKSA + IGTKSA + BIGTKSA */

/* ND-GTK is the data-path group key (CCMP-128). */
#define NAN_ND_GTK_LEN           16
/* Host-side bound for the group Key Data scratch buffers (plaintext + AES-wrap),
 * sized for GTK+IGTK+BIGTK: GTK 24B + IGTK 30B + BIGTK 30B + optional Key Lifetime
 * KDE(s), padded to a multiple of 8, + 8B NIST AES Key Wrap overhead (~104B worst
 * case). NOT the descriptor-len reservation — the getters now return the EXACT
 * per-NDP length, so the blob allocates exactly what the builder writes. */
#define NAN_GROUP_KEY_DATA_MAX   128

/* NCS-SK-128 only for now (Table 21): KCK 128 bits, KEK 128 bits, TK 128 bits, MIC 16 bytes */
#define NAN_NCS_SK_128_KCK_LEN  16
#define NAN_NCS_SK_128_KEK_LEN  16
#define NAN_NCS_SK_128_TK_LEN   16
#define NAN_NCS_SK_128_MIC_LEN  16
#define NAN_NCS_SK_128_PTK_LEN  (NAN_NCS_SK_128_KCK_LEN + NAN_NCS_SK_128_KEK_LEN + NAN_NCS_SK_128_TK_LEN)

/* Internal key-install constants matching esp_wifi_set_sta_key_internal semantics. */
#define NAN_WIFI_WPA_ALG_CCMP          3
#define NAN_WIFI_WPA_ALG_BIP_CMAC_128  7   /* IGTK/BIGTK BIP = blob WIFI_WPA_ALG_IGTK (confirmed by han2; 4 is SMS4) */
#define NAN_KEY_FLAG_RX          BIT(2)
#define NAN_KEY_FLAG_TX          BIT(3)
#define NAN_KEY_FLAG_PAIRWISE    BIT(5)

/* NAN key-type selector passed as the last arg of esp_wifi_set_nan_key_internal;
 * tells the blob which NAN SA the key belongs to. IGTK/BIGTK values are
 * provisional — confirm with han2 before the lib bump. */
#define NAN_KEY_ND_TK     0
#define NAN_KEY_ND_GTK    1
#define NAN_KEY_NM_TK     2
#define NAN_KEY_ND_IGTK   3
#define NAN_KEY_ND_BIGTK  4

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

/* NIK length for cipher version 0 (Wi-Fi Aware spec v4.0). */
#define NAN_APP_PEER_NIK_LEN  16

#if defined(CONFIG_ESP_WIFI_NAN_SECURITY) && defined(CONFIG_ESP_WIFI_NAN_PAIRING)
/*
 * Per-peer pairing record: holds the ND-PMK derived from NM-KDK during the
 * PASN/Pairing handshake, together with the cipher that paired NDPs must use.
 * Wi-Fi Aware v4.0 §7.6.4.2 — ND-PMK is keyed by (Initiator NMI, Responder NMI)
 * and shared across all secured NDPs between the same paired peers, so the
 * cache is keyed by peer NMI alone (independent of svc_id).
 */
struct nan_paired_peer {
    bool     valid;
    uint8_t  peer_nmi[MACADDR_LEN];
    uint8_t  role;                              /* enum nan_role value */
    uint8_t  ndp_csid;                          /* WIFI_NAN_CSID_NCS_SK_128 / _256 */
    uint8_t  nd_pmk[ESP_WIFI_NAN_NDP_PMK_LEN];
    uint32_t lifetime_sec;
    int64_t  established_us;
};
#endif /* CONFIG_ESP_WIFI_NAN_SECURITY && CONFIG_ESP_WIFI_NAN_PAIRING */

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
    /* Set at SDF match if the publisher advertised an NCS-GTK suite in its CSIA;
     * the initiator NDP-req path uses it (with our own group_data_prot) to
     * decide whether to distribute a GTK during NDP setup. */
    uint8_t peer_group_data_cap: 1;
    uint8_t peer_group_mgmt_cap: 1;     /* peer advertised IGTKSA (CSIA caps bits 1-2 != 0) */
    uint8_t peer_group_bigtk_cap: 1;    /* peer advertised BIGTKSA (CSIA caps bits 1-2 == 10) */
    uint8_t peer_sec_reserved: 5;
#endif
#if CONFIG_ESP_WIFI_NAN_PAIRING
    /* Peer NIK / cipher version / lifetime extracted from a NAN Shared Key
     * Descriptor attribute received in a follow-up frame. @c has_nik is set
     * once a valid NIK has been decrypted and stored.
     */
    uint8_t peer_nik[NAN_APP_PEER_NIK_LEN];
    uint8_t peer_nik_cipher_ver;
    uint32_t peer_nik_lifetime_sec;
    bool has_nik;
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
#ifdef CONFIG_ESP_WIFI_NAN_PAIRING
    wifi_nan_pairing_cfg_t pairing;
#endif
#endif
    uint8_t num_peer_records;
    SLIST_HEAD(peer_list_t, peer_svc_info) peer_list;
#if CONFIG_ESP_WIFI_NAN_PAIRING
    bool nik_fup_pending;
    uint8_t nik_fup_pending_peer_nmi[MACADDR_LEN];
    /* Set when the current PASN session for @c verify_session_peer_nmi is a
     * pairing verification (re-pair). Consumed once in the key-installed
     * callback to skip the NIK follow-up exchange. */
    bool verify_session_pending;
    uint8_t verify_session_peer_nmi[MACADDR_LEN];
#endif
    uint8_t svc_hash[6];
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

    /* Received peer group keys (RX GTKSA). GTK protects group-addressed data
     * frames the peer transmits; installed against the peer NDI. IGTK/BIGTK
     * protect group-addressed management/Beacon frames (NMI plane). */
    uint8_t gtk[NAN_GTK_MAX_LEN];
    uint8_t igtk[NAN_GTK_MAX_LEN];
    uint8_t bigtk[NAN_GTK_MAX_LEN];
    uint8_t gtk_len;
    uint8_t igtk_len;
    uint8_t bigtk_len;
    uint8_t gtk_keyid;                  /* peer GTK Key ID (1 or 2) */
    uint8_t igtk_keyid;                 /* peer IGTK Key ID (4 or 5) */
    uint8_t bigtk_keyid;                /* peer BIGTK Key ID (6 or 7) */
    uint8_t gtk_rsc[NAN_KEY_RSC_LEN];   /* peer GTK RSC from Key RSC field */
    uint8_t gtk_set: 1;
    uint8_t igtk_set: 1;
    uint8_t bigtk_set: 1;
    uint8_t group_keys_reserved: 5;

    /* Own group key (TX GTKSA) distributed to the peer in M3 (initiator) or
     * M4 (responder); installed against the local NDI as the TX group key. */
    uint8_t own_gtk[NAN_ND_GTK_LEN];
    uint8_t own_gtk_len;
    uint8_t own_gtk_keyid;              /* own GTK Key ID (1 or 2) */
    uint8_t own_gtk_rsc[NAN_KEY_RSC_LEN];
    uint8_t own_gtk_set: 1;
    uint8_t gtk_required: 1;            /* GTKSA negotiated for this NDP */
    uint8_t igtk_required: 1;          /* IGTKSA negotiated for this NDP */
    uint8_t bigtk_required: 1;         /* BIGTKSA negotiated for this NDP */
    uint8_t own_group_reserved: 4;

    uint8_t key_rsc[NAN_KEY_RSC_LEN];
#endif
};

/* Forward decl for PASN data, used opaquely when CONFIG_ESP_WIFI_PASN_SUPPORT */
struct nan_pasn_data;

/* NAN context shared between files */
typedef struct {
    uint8_t state;
    uint8_t event;
    struct ndl_info ndl[ESP_WIFI_NAN_DATAPATH_MAX_PEERS];
    struct own_svc_info own_svc[ESP_WIFI_NAN_MAX_SVC_SUPPORTED];
    esp_netif_t *nan_netif;
#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
    uint8_t own_nik[ESP_WIFI_NAN_NIK_LEN];
    bool own_nik_valid;
    /* Device-global IGTKSA/BIGTKSA (one per NMI, §7.1.3.3/§7.1.3.4). Generated
     * once via os_get_random and reused across all secured NDPs; copied into
     * each M3/M4 and installed against the local NMI. BIP-CMAC-128 (16-byte).
     * On ESP the NMI and NDI are the same MAC today. */
    uint8_t own_igtk[NAN_ND_GTK_LEN];
    uint8_t own_igtk_ipn[6];
    uint8_t own_igtk_keyid;             /* 4 or 5 */
    bool    own_igtk_set;
    uint8_t own_bigtk[NAN_ND_GTK_LEN];
    uint8_t own_bigtk_bipn[6];
    uint8_t own_bigtk_keyid;            /* 6 or 7 */
    bool    own_bigtk_set;
    /* Device-global "support + use group management protection (IGTKSA/BIGTKSA)".
     * One setting per NMI, not per service: Beacons are NMI-level and there is
     * exactly one IGTKSA/BIGTKSA per NMI (§7.1.3.3/§7.1.3.4). Sourced from
     * wifi_nan_sync_config_t.group_mgmt_prot at NAN start. When set it: forces
     * GTKSA on every secured service (the CSIA caps field cannot encode IGTK/
     * BIGTK without GTKSA, §9.5.21.2 Table 122), generates own IGTK+BIGTK,
     * advertises caps 0x04, and BIP-protects Beacons + multicast SDFs.
     * Advertising never blocks a non-supporting peer — keys and protection are
     * set up only after capability negotiation (§7.1.3.5). */
    bool    group_mgmt_prot;
    uint8_t cached_nira_nonce[8];
    uint8_t cached_nira_tag[8];
    bool nira_cached;
    /* Peer NIK/NPK credentials loaded from NVS at start and refreshed as peers
     * are paired. Drives NIRA identity resolution; optionally persisted to NVS
     * when @c use_nvs_for_caching is set. */
    wifi_nan_peer_creds_t peer_creds[ESP_WIFI_NAN_MAX_PEER_CREDS];
    uint8_t num_peer_creds;
    bool use_nvs_for_caching;
    uint32_t nik_lifetime;
#endif
#ifdef CONFIG_ESP_WIFI_PASN_SUPPORT
    struct nan_pasn_data *nan_pasn_data;
#endif
#if defined(CONFIG_ESP_WIFI_NAN_SECURITY) && defined(CONFIG_ESP_WIFI_NAN_PAIRING)
    /* Pairing records produced by PASN/Pairing key-install. Sized to the
     * datapath peer cap since paired peers are the population that can
     * actually run an NDP; LRU-evicted on overflow. */
    struct nan_paired_peer paired_peers[ESP_WIFI_NAN_DATAPATH_MAX_PEERS];
#endif
} nan_ctx_t;

extern nan_ctx_t s_nan_ctx;

/* Helpers defined in nan_app.c, used by nan_security.c */
struct own_svc_info *nan_find_own_svc(uint8_t svc_id);
struct own_svc_info *nan_find_own_svc_by_name(const char *svc_name);
struct ndl_info *nan_find_ndl(uint8_t ndp_id, uint8_t peer_nmi[]);
struct ndl_info *nan_find_ndl_by_pub_id_and_peer(uint8_t pub_id, const uint8_t *peer_nmi);
bool nan_compute_service_id(const char *service_name, uint8_t service_id[6]);

#ifdef CONFIG_ESP_WIFI_NAN_SECURITY
/* Security-gated (defined in nan_security.c) */
uint32_t esp_nan_get_csia_len(uint16_t own_csid_bitmap, uint16_t peer_csid_bitmap);
uint32_t esp_nan_get_scia_len(uint8_t num_pmkids);
uint32_t esp_nan_get_shared_key_desc_attr_len(uint16_t key_data_len);
int      esp_nan_ndp_security_install_get_shared_desc_len(uint8_t ndp_id, const uint8_t *peer_nmi);
int      esp_nan_ndp_confirm_get_shared_desc_len(uint8_t ndp_id, const uint8_t *peer_nmi);
uint8_t esp_nan_get_ndp_resp_num_pmkids(uint8_t ndp_id, const uint8_t *peer_nmi);
uint32_t esp_nan_get_ndp_resp_shared_key_desc_len(uint8_t ndp_id, const uint8_t *peer_nmi);

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

/* Generate (once) and TX-install the device-global IGTK/BIGTK so Beacons and
 * group-addressed SDFs are BIP-protected from NAN start (§7.1.3.3/§7.1.3.4).
 * Call once at NAN start; never per-NDP (would reset the blob's BIPN counter). */
void nan_security_install_own_group_integrity_keys(void);

/* Clear the device-global IGTK/BIGTK state on NAN stop so the next NAN start
 * regenerates fresh keys. Prevents re-installing a stale key with IPN/BIPN=0
 * (which resets the blob's monotonic replay counter) and key reuse if the NMI
 * is re-randomized on restart. */
void nan_security_reset_own_group_keys(void);

/*
 * Match subscriber discovery security to a publisher's params.
 * NCS-SK: Compare locally derived ND-PMKID (subscriber passphrase, publisher NMI) to
 * peer discovery security params. Returns true if any local cred's PMKID matches a peer-advertised one.
 * NCS-PASN: Returns true for matching cipher in Publisher's csid_bitmap
 */
bool nan_security_service_match(const struct own_svc_info *own_svc,
                                struct peer_svc_info *peer_svc,
                                const uint8_t *publisher_nmi,
                                const wifi_nan_peer_sdf_security_t *peer_sec);
#endif /* CONFIG_ESP_WIFI_NAN_SECURITY */
#if CONFIG_ESP_WIFI_NAN_PAIRING
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

void nan_app_post_event(int32_t event_id, void *event_data, size_t event_data_size);
struct peer_svc_info *nan_find_peer_svc(uint8_t own_svc_id, uint8_t peer_svc_id, uint8_t peer_nmi[]);
EventGroupHandle_t nan_pairing_get_event_group(void);
uint32_t *nan_pairing_get_fup_context(void);
const uint8_t *nan_pairing_get_null_mac(void);

bool nan_pairing_validate_publish_bootstrapping(uint16_t bootstrapping_methods);
bool nan_pairing_validate_subscribe_bootstrapping(uint16_t bootstrapping_methods);

uint16_t nan_app_parse_npba_from_publish(const struct nan_cb_npba_t *npba);

void nan_app_bootstrap_indication(uint8_t peer_svc_id, uint8_t pub_id,
                                  uint8_t peer_nmi[6], uint16_t selected_method);
void nan_app_bootstrap_completed(uint8_t status, uint8_t peer_svc_id, uint8_t sub_id,
                                 uint8_t peer_nmi[6], uint16_t matched_method,
                                 uint8_t reason_code);
bool nan_app_parse_npba_from_receive(uint8_t own_svc_id, uint8_t peer_svc_id,
                                     uint8_t peer_nmi[6], const struct nan_cb_npba_t *npba);
void nan_pairing_cancel_svc_pending(struct own_svc_info *own);

void nan_app_receive_pairing_followup(uint8_t svc_id, uint8_t peer_svc_id,
                                      const uint8_t *peer_mac,
                                      const uint8_t *shared_key_attr,
                                      size_t shared_key_attr_buf_len);
/*
 * Paired-peer cache API. Called from nan_pairing.c after the PASN install
 * callback fires; consumed by the NDP security layer to source ND-PMK + cipher
 * for paired peers (no per-service credential involved).
 *
 * Caller of nan_app_find_paired_peer() must hold NAN_DATA_LOCK while using the
 * returned pointer -- it points into s_nan_ctx storage. Same convention as
 * nan_find_own_svc() / nan_find_peer_svc().
 */
esp_err_t nan_app_register_paired_peer(const uint8_t *peer_nmi,
                                       uint8_t role,
                                       uint8_t ndp_csid,
                                       const uint8_t *nd_pmk,
                                       size_t nd_pmk_len,
                                       uint32_t lifetime_sec);
const struct nan_paired_peer *nan_app_find_paired_peer(const uint8_t *peer_nmi);
void nan_app_remove_paired_peer(const uint8_t *peer_nmi);
void nan_app_clear_paired_peers(void);

#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */

#ifdef __cplusplus
}
#endif
