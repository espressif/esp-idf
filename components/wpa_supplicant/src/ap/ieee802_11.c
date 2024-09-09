/*
 * hostapd / IEEE 802.11 Management
 * Copyright (c) 2002-2017, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include "common/sae.h"
#include "common/ieee802_11_defs.h"
#include "esp_wifi_driver.h"
#include "ap/wpa_auth.h"
#include "ap/hostapd.h"
#include "ap/sta_info.h"
#include "crypto/sha256.h"
#include "ap/pmksa_cache_auth.h"
#include "ap/comeback_token.h"
#include "crypto/random.h"
#include "esp_wpa3_i.h"
#include "esp_hostap.h"

#ifdef CONFIG_SAE

static void sae_set_state(struct sta_info *sta, enum sae_state state,
                          const char *reason)
{
    wpa_printf(MSG_DEBUG, "SAE: State %s -> %s for peer " MACSTR " (%s)",
               sae_state_txt(sta->sae->state), sae_state_txt(state),
               MAC2STR(sta->addr), reason);
    sta->sae->state = state;
}


static const char * sae_get_password(struct hostapd_data *hapd,
                     struct sta_info *sta,
                     const char *rx_id,
                     struct sae_pt **s_pt)
{
    const char *password = NULL;
    struct sae_pt *pt = NULL;

    if (!password) {
        password = hapd->conf->ssid.wpa_passphrase;
        pt = hapd->conf->ssid.pt;
    }

    if (s_pt) {
        *s_pt = pt;
    }

    return password;
}


static struct wpabuf *auth_build_sae_commit(struct hostapd_data *hapd,
        struct sta_info *sta, int update, int status_code)
{
    struct wpabuf *buf;
    const char *password = NULL;
    const char *rx_id = NULL;
    int use_pt = 0;
    struct sae_pt *pt = NULL;

    if (sta->sae->tmp) {
        rx_id = sta->sae->tmp->pw_id;
        use_pt = sta->sae->h2e;
    }

    if (rx_id && hapd->conf->sae_pwe != SAE_PWE_FORCE_HUNT_AND_PECK)
        use_pt = 1;
    else if (status_code == WLAN_STATUS_SUCCESS)
        use_pt = 0;
    else if (status_code == WLAN_STATUS_SAE_HASH_TO_ELEMENT)
        use_pt = 1;

    password = sae_get_password(hapd, sta, rx_id, &pt);
    if (!password || (use_pt && !pt)) {
        wpa_printf(MSG_DEBUG, "SAE: No password available");
        return NULL;
    }

    if (update && use_pt &&
        sae_prepare_commit_pt(sta->sae, pt, hapd->own_addr, sta->addr,
                  NULL, NULL) < 0) {
        return NULL;
    }

    if (update && !use_pt &&
        sae_prepare_commit(hapd->own_addr, sta->addr,
                (u8 *) password, os_strlen((const char *)password),
                sta->sae) < 0) {
        wpa_printf(MSG_DEBUG, "SAE: Could not pick PWE");
        return NULL;
    }

    buf = wpabuf_alloc(SAE_COMMIT_MAX_LEN +
                       (rx_id ? 3 + os_strlen(rx_id) : 0));

    if (buf &&
        sae_write_commit(sta->sae, buf, sta->sae->tmp ?
                sta->sae->tmp->anti_clogging_token : NULL,
                rx_id) < 0) {
        wpabuf_free(buf);
        buf = NULL;
    }

    return buf;
}


static struct wpabuf *auth_build_sae_confirm(struct hostapd_data *hapd,
        struct sta_info *sta)
{
    struct wpabuf *buf;

    buf = wpabuf_alloc(SAE_CONFIRM_MAX_LEN);
    if (buf == NULL) {
        return NULL;
    }

    if (sae_write_confirm(sta->sae, buf) < 0) {
        wpabuf_free(buf);
        return NULL;
    }

    return buf;
}


static int auth_sae_send_commit(struct hostapd_data *hapd,
                                struct sta_info *sta,
                                const u8 *bssid, int update, int status_code)
{
    struct wpabuf *data;
    int reply_res;
    u16 status;

    data = auth_build_sae_commit(hapd, sta, update, status_code);
    if (!data && sta->sae->tmp && sta->sae->tmp->pw_id) {
        return WLAN_STATUS_UNKNOWN_PASSWORD_IDENTIFIER;
    }
    if (data == NULL) {
        return WLAN_STATUS_UNSPECIFIED_FAILURE;
    }

    if (sta->sae->tmp && sta->sae->h2e) {
        status = WLAN_STATUS_SAE_HASH_TO_ELEMENT;
    } else {
        status = WLAN_STATUS_SUCCESS;
    }

#ifdef ESP_SUPPLICANT
    if (sta->remove_pending) {
        reply_res = -1;
    } else {
        reply_res = esp_send_sae_auth_reply(hapd, sta->addr, bssid, WLAN_AUTH_SAE, 1,
                                        status, wpabuf_head(data),
                                        wpabuf_len(data));
    }
#endif /* ESP_SUPPLICANT */

    wpabuf_free(data);
    return reply_res;
}

static int auth_sae_send_confirm(struct hostapd_data *hapd,
                                 struct sta_info *sta,
                                 const u8 *bssid)
{
    struct wpabuf *data;
    int reply_res;

    data = auth_build_sae_confirm(hapd, sta);
    if (data == NULL) {
        return WLAN_STATUS_UNSPECIFIED_FAILURE;
    }

#ifdef ESP_SUPPLICANT
    if (sta->remove_pending) {
        reply_res = -1;
    } else {
        reply_res = esp_send_sae_auth_reply(hapd, sta->addr, bssid, WLAN_AUTH_SAE, 2,
                                       WLAN_STATUS_SUCCESS, wpabuf_head(data),
                                       wpabuf_len(data));
    }
#endif /* ESP_SUPPLICANT */

    wpabuf_free(data);
    return reply_res;
}


static int use_sae_anti_clogging(struct hostapd_data *hapd)
{
    struct sta_info *sta;
    unsigned int open = 0;

    if (hapd->conf->sae_anti_clogging_threshold == 0) {
        return 1;
    }

    for (sta = hapd->sta_list; sta; sta = sta->next) {
        if (sta->sae &&
            (sta->sae->state == SAE_COMMITTED ||
             sta->sae->state == SAE_CONFIRMED)) {
            open++;
        }
        if (open >= hapd->conf->sae_anti_clogging_threshold) {
            return 1;
        }
    }

    /* In addition to already existing open SAE sessions, check whether
     * there are enough pending commit messages in the processing queue to
     * potentially result in too many open sessions. */
    if (open + dl_list_len(&hapd->sae_commit_queue) >=
        hapd->conf->sae_anti_clogging_threshold) {
        return 1;
    }

    return 0;
}


static int sae_check_big_sync(struct hostapd_data *hapd, struct sta_info *sta)
{
    if (sta->sae->sync > hapd->conf->sae_sync) {
        sae_set_state(sta, SAE_NOTHING, "Sync > dot11RSNASAESync");
        sta->sae->sync = 0;
        return -1;
    }
    return 0;
}


void sae_accept_sta(struct hostapd_data *hapd, struct sta_info *sta)
{
    sta->flags |= WLAN_STA_AUTH;

#ifdef ESP_SUPPLICANT
    sta->sae_commit_processing = false;
#endif /* ESP_SUPPLICANT */

    sta->auth_alg = WLAN_AUTH_SAE;
    sae_set_state(sta, SAE_ACCEPTED, "Accept Confirm");
    wpa_auth_pmksa_add_sae(hapd->wpa_auth, sta->addr,
                           sta->sae->pmk, sta->sae->pmkid, false);
}


static int sae_sm_step(struct hostapd_data *hapd, struct sta_info *sta,
                       const u8 *bssid, u8 auth_transaction, u16 status_code,
                       int allow_reuse, int *sta_removed)
{
    int ret = WLAN_STATUS_SUCCESS;
    *sta_removed = 0;

    if (auth_transaction != 1 && auth_transaction != 2) {
        return WLAN_STATUS_UNSPECIFIED_FAILURE;
    }

    wpa_printf(MSG_DEBUG, "SAE: Peer " MACSTR " state=%s auth_trans=%u",
               MAC2STR(sta->addr), sae_state_txt(sta->sae->state),
               auth_transaction);
    switch (sta->sae->state) {
    case SAE_NOTHING:
        if (auth_transaction == 1) {
            if (sta->sae->tmp) {
                sta->sae->h2e =
                    (status_code ==
                     WLAN_STATUS_SAE_HASH_TO_ELEMENT);
            }
            ret = auth_sae_send_commit(hapd, sta, bssid,
                                       !allow_reuse, status_code);

            if (ret) {
                return ret;
            }

            sae_set_state(sta, SAE_COMMITTED, "Sent Commit");

            if (sae_process_commit(sta->sae) < 0) {
                return WLAN_STATUS_UNSPECIFIED_FAILURE;
            }

            sta->sae->sync = 0;
        } else {
            wpa_printf(MSG_DEBUG, "SAE confirm before commit");
        }
        break;
    case SAE_COMMITTED:
        if (auth_transaction == 1) {
            if (sae_process_commit(sta->sae) < 0) {
                return WLAN_STATUS_UNSPECIFIED_FAILURE;
            }

            ret = auth_sae_send_confirm(hapd, sta, bssid);
            if (ret) {
                return ret;
            }
            sae_set_state(sta, SAE_CONFIRMED, "Sent Confirm");
            sta->sae->sync = 0;
        } else {
            /*
             * For instructure BSS, send the postponed Confirm from
             * Nothing -> Confirmed transition that was reduced to
             * Nothing -> Committed above.
             */
            ret = auth_sae_send_confirm(hapd, sta, bssid);
            if (ret) {
                return ret;
            }

            sae_set_state(sta, SAE_CONFIRMED, "Sent Confirm");

            /*
             * Since this was triggered on Confirm RX, run another
             * step to get to Accepted without waiting for
             * additional events.
             */
            return sae_sm_step(hapd, sta, bssid, auth_transaction,
                               WLAN_STATUS_SUCCESS, 0, sta_removed);
        }
        break;
    case SAE_CONFIRMED:
        if (auth_transaction == 1) {
            if (sae_check_big_sync(hapd, sta)) {
                return WLAN_STATUS_SUCCESS;
            }
            sta->sae->sync++;

            ret = auth_sae_send_commit(hapd, sta, bssid, 1,
                    status_code);

            if (ret) {
                return ret;
            }

            if (sae_process_commit(sta->sae) < 0) {
                return WLAN_STATUS_UNSPECIFIED_FAILURE;
            }

            ret = auth_sae_send_confirm(hapd, sta, bssid);
            if (ret) {
                return ret;
            }

        } else {
            sta->sae->send_confirm = 0xffff;
            sae_accept_sta(hapd, sta);
        }
        break;
    case SAE_ACCEPTED:
        if (auth_transaction == 1) {
            wpa_printf(MSG_DEBUG, "SAE: Start reauthentication");
            ret = auth_sae_send_commit(hapd, sta, bssid, 1,
                    status_code);
            if (ret) {
                return ret;
            }
            sae_set_state(sta, SAE_COMMITTED, "Sent Commit");

            if (sae_process_commit(sta->sae) < 0) {
                return WLAN_STATUS_UNSPECIFIED_FAILURE;
            }

            sta->sae->sync = 0;
        } else {
            if (sae_check_big_sync(hapd, sta)) {
                return WLAN_STATUS_SUCCESS;
            }
            sta->sae->sync++;

            ret = auth_sae_send_confirm(hapd, sta, bssid);
            sae_clear_temp_data(sta->sae);
            if (ret) {
                return ret;
            }
        }
        break;
    default:
        wpa_printf(MSG_ERROR, "SAE: invalid state %d",
                   sta->sae->state);
        return WLAN_STATUS_UNSPECIFIED_FAILURE;
    }
    return WLAN_STATUS_SUCCESS;
}


static int sae_status_success(struct hostapd_data *hapd, u16 status_code)
{
    enum sae_pwe sae_pwe = hapd->conf->sae_pwe;

    return ((sae_pwe == SAE_PWE_HUNT_AND_PECK ||
         sae_pwe == SAE_PWE_FORCE_HUNT_AND_PECK) &&
        status_code == WLAN_STATUS_SUCCESS) ||
        (sae_pwe == SAE_PWE_HASH_TO_ELEMENT &&
         (status_code == WLAN_STATUS_SAE_HASH_TO_ELEMENT)) ||
        (sae_pwe == SAE_PWE_BOTH &&
         (status_code == WLAN_STATUS_SUCCESS ||
          status_code == WLAN_STATUS_SAE_HASH_TO_ELEMENT));
}


static int sae_is_group_enabled(struct hostapd_data *hapd, int group)
{
    int *groups = NULL;
    int default_groups[] = { 19, 0 };
    int i;

    if (!groups) {
        groups = default_groups;
    }

    for (i = 0; groups[i] > 0; i++) {
        if (groups[i] == group)
            return 1;
    }

    return 0;
}


static int check_sae_rejected_groups(struct hostapd_data *hapd,
				     struct sae_data *sae)
{
    const struct wpabuf *groups;
    size_t i, count, len;
    const u8 *pos;

    if (!sae->tmp)
        return 0;
    groups = sae->tmp->peer_rejected_groups;
    if (!groups)
        return 0;

    pos = wpabuf_head(groups);
    len = wpabuf_len(groups);
    if (len & 1) {
        wpa_printf(MSG_DEBUG,
                  "SAE: Invalid length of the Rejected Groups element payload: %zu",
                  len);
        return 1;
    }

    count = len / 2;
    for (i = 0; i < count; i++) {
        int enabled;
        u16 group;

        group = WPA_GET_LE16(pos);
        pos += 2;
        enabled = sae_is_group_enabled(hapd, group);
        wpa_printf(MSG_DEBUG, "SAE: Rejected group %u is %s",
                  group, enabled ? "enabled" : "disabled");
        if (enabled)
            return 1;
    }

    return 0;
}


int handle_auth_sae(struct hostapd_data *hapd, struct sta_info *sta,
                    u8 *buf, size_t len, u8 *bssid,
                    u16 auth_transaction, u16 status)
{
    int resp =  WLAN_STATUS_SUCCESS;
    struct wpabuf *data = NULL;
    int *groups = hapd->conf->sae_groups;
    int default_group[] = { IANA_SECP256R1, 0};
    const u8 *pos, *end;
    int sta_removed = 0;

    if (!groups) {
        groups = default_group;
    }

    if (!sta->sae) {
        if (auth_transaction != 1 ||
            !sae_status_success(hapd, status)) {
            wpa_printf(MSG_DEBUG, "SAE: Unexpected Status Code %u",
                   status);
            resp = WLAN_STATUS_UNSPECIFIED_FAILURE;
            goto reply;
        }
        sta->sae = os_zalloc(sizeof(*sta->sae));
        if (!sta->sae) {
            resp = WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA;
            goto remove_sta;
        }
        sae_set_state(sta, SAE_NOTHING, "Init");
        sta->sae->sync = 0;
    }
    if (auth_transaction == 1) {
        const u8 *token = NULL;
        size_t token_len = 0;
        int allow_reuse = 0;

        if (!sae_status_success(hapd, status)) {
            goto remove_sta;
        }

        if (sta->sae->state == SAE_COMMITTED) {
            /* This is needed in the infrastructure BSS case to
             * address a sequence where a STA entry may remain in
             * hostapd across two attempts to do SAE authentication
             * by the same STA. The second attempt may end up trying
             * to use a different group and that would not be
             * allowed if we remain in Committed state with the
             * previously set parameters. */
            pos = buf;
            end = buf + len;
            if (end - pos >= (int) sizeof(le16) &&
                sae_group_allowed(sta->sae, groups,
                          WPA_GET_LE16(pos)) ==
                WLAN_STATUS_SUCCESS) {
                /* Do not waste resources deriving the same PWE
                 * again since the same group is reused. */
                sae_set_state(sta, SAE_NOTHING,
                              "Allow previous PWE to be reused");
                allow_reuse = 1;
            } else {
                sae_set_state(sta, SAE_NOTHING,
                              "Clear existing state to allow restart");
                sae_clear_data(sta->sae);
            }
        }

        resp = sae_parse_commit(sta->sae, buf, len, &token, &token_len, default_group,
                status == WLAN_STATUS_SAE_HASH_TO_ELEMENT);
        if (resp == SAE_SILENTLY_DISCARD) {
            wpa_printf(MSG_DEBUG,
                       "SAE: Drop commit message from " MACSTR " due to reflection attack",
                       MAC2STR(sta->addr));
            resp = WLAN_STATUS_SUCCESS;
            goto remove_sta;
        }

        if (resp == WLAN_STATUS_UNKNOWN_PASSWORD_IDENTIFIER) {
            sae_set_state(sta, SAE_NOTHING,
                          "Unknown Password Identifier");
            goto remove_sta;
        }

        if (token &&
            check_comeback_token(hapd->comeback_key,
                     hapd->comeback_pending_idx, sta->addr,
                     token, token_len) < 0) {
            wpa_printf(MSG_DEBUG, "SAE: Drop commit message with "
                       "incorrect token from " MACSTR,
                       MAC2STR(sta->addr));
            resp = WLAN_STATUS_UNSPECIFIED_FAILURE;
            goto remove_sta;
        }

        if (check_sae_rejected_groups(hapd, sta->sae)) {
            resp = WLAN_STATUS_UNSPECIFIED_FAILURE;
            goto reply;
        }

        if (resp != WLAN_STATUS_SUCCESS) {
            goto reply;
        }

        if (!token && use_sae_anti_clogging(hapd) && !allow_reuse) {
            int h2e = 0;

            wpa_printf(MSG_DEBUG,
                       "SAE: Request anti-clogging token from "
                       MACSTR, MAC2STR(sta->addr));
            if (sta->sae->tmp)
                h2e = sta->sae->h2e;
            if (status == WLAN_STATUS_SAE_HASH_TO_ELEMENT)
                h2e = 1;
            data = auth_build_token_req(
                &hapd->last_comeback_key_update,
                hapd->comeback_key,
                hapd->comeback_idx,
                hapd->comeback_pending_idx,
                sizeof(hapd->comeback_pending_idx),
                sta->sae->group,
                sta->addr, h2e);
            resp = WLAN_STATUS_ANTI_CLOGGING_TOKEN_REQ;

#ifdef ESP_SUPPLICANT
            sta->sae_commit_processing = false;
#endif /* ESP_SUPPLICANT */

            goto reply;
        }

        resp = sae_sm_step(hapd, sta, bssid, auth_transaction,
                           status, allow_reuse, &sta_removed);
    } else if (auth_transaction == 2) {
        if (status != WLAN_STATUS_SUCCESS) {
            goto remove_sta;
        }

       const u8 *var;
       size_t var_len;
       u16 peer_send_confirm;

       var = buf;
       var_len = len;
       if (var_len < 2) {
           resp = WLAN_STATUS_UNSPECIFIED_FAILURE;
           goto reply;
       }

       peer_send_confirm = WPA_GET_LE16(var);

       if (sta->sae->state == SAE_ACCEPTED &&
               (peer_send_confirm <= sta->sae->rc ||
                peer_send_confirm == 0xffff)) {
           wpa_printf(MSG_DEBUG,
                      "SAE: Silently ignore unexpected Confirm from peer "
                      MACSTR
                      " (peer-send-confirm=%u Rc=%u)",
                      MAC2STR(sta->addr),
                      peer_send_confirm, sta->sae->rc);
           return 0;
       }

       if (sae_check_confirm(sta->sae, buf, len) < 0) {
           resp = WLAN_STATUS_CHALLENGE_FAIL;
           goto reply;
       }
       sta->sae->rc = peer_send_confirm;

        resp = sae_sm_step(hapd, sta, bssid, auth_transaction,
                           status, 0, &sta_removed);
    } else {
        wpa_printf(MSG_ERROR, "unexpected SAE authentication transaction %u (status=%u )", auth_transaction, status);
        if (status != WLAN_STATUS_SUCCESS) {
            goto remove_sta;
        }
        resp = WLAN_STATUS_UNKNOWN_AUTH_TRANSACTION;
    }
reply:
    if (!sta_removed && resp != WLAN_STATUS_SUCCESS) {
        pos = buf;
        end = buf + len;

        /* Copy the Finite Cyclic Group field from the request if we
         * rejected it as unsupported group. */
        if (resp == WLAN_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED &&
                !data && end - pos >= 2) {
            data = wpabuf_alloc_copy(pos, 2);
        }
#ifdef ESP_SUPPLICANT
        if (!sta->remove_pending) {
            esp_send_sae_auth_reply(hapd, bssid, bssid, WLAN_AUTH_SAE,
                                    auth_transaction, resp,
                                    data ? wpabuf_head(data) : (u8 *) "",
                                    data ? wpabuf_len(data) : 0);
        }
#endif /* ESP_SUPPLICANT */

    }
remove_sta:
    wpabuf_free(data);
    return resp;
}


int auth_sae_queue(struct hostapd_data *hapd,
                    u8 *buf, size_t len, u8 *bssid, u16 status, u32 auth_transaction)
{
    struct hostapd_sae_commit_queue *q, *q2;
    unsigned int queue_len;

    queue_len = dl_list_len(&hapd->sae_commit_queue);
    if (queue_len >= 5) {
        wpa_printf(MSG_DEBUG,
                   "SAE: No more room in message queue - drop the new frame from "
                   MACSTR, MAC2STR(bssid));
        return 0;
    }

    wpa_printf(MSG_DEBUG, "SAE: Queue Authentication message from "
               MACSTR " for processing (queue_len %u)", MAC2STR(bssid),
               queue_len);
    q = os_zalloc(sizeof(*q) + len);
    if (!q) {
        return -1;
    }

    q->len = len;
    os_memcpy(q->msg, buf, len);
    os_memcpy(q->bssid, bssid, ETH_ALEN);
    q->auth_transaction = auth_transaction;
    q->status = status;
    /* Check whether there is already a queued Authentication frame from the
     * same station with the same transaction number and if so, replace that
     * queue entry with the new one. This avoids issues with a peer that
     * sends multiple times (e.g., due to frequent SAE retries). There is no
     * point in us trying to process the old attempts after a new one has
     * obsoleted them. */
    dl_list_for_each(q2, &hapd->sae_commit_queue,
                     struct hostapd_sae_commit_queue, list) {
        if (os_memcmp(bssid, q2->bssid, ETH_ALEN) == 0 &&
            auth_transaction == q2->auth_transaction) {
            wpa_printf(MSG_DEBUG,
                       "SAE: Replace queued message from same STA with same transaction number");
            dl_list_add(&q2->list, &q->list);
            dl_list_del(&q2->list);
            os_free(q2);
            goto queued;
        }
    }

    /* No pending identical entry, so add to the end of the queue */
    dl_list_add_tail(&hapd->sae_commit_queue, &q->list);

queued:

#ifdef ESP_SUPPLICANT
    /* posting event to the task to handle commit */
    if (wpa3_hostap_post_evt(SIG_WPA3_RX_COMMIT, 0) != 0) {
        wpa_printf(MSG_ERROR, "failed to queue commit build event");
        return -1;
    }
    return 0;
#endif /* ESP_SUPPLICANT */

}

#endif /* CONFIG_SAE */

u16 wpa_res_to_status_code(enum wpa_validate_result res)
{
    switch (res) {
    case WPA_IE_OK:
        return WLAN_STATUS_SUCCESS;
    case WPA_INVALID_IE:
        return WLAN_STATUS_INVALID_IE;
    case WPA_INVALID_GROUP:
        return WLAN_STATUS_GROUP_CIPHER_NOT_VALID;
    case WPA_INVALID_PAIRWISE:
        return WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID;
    case WPA_INVALID_AKMP:
        return WLAN_STATUS_AKMP_NOT_VALID;
    case WPA_NOT_ENABLED:
        return WLAN_STATUS_INVALID_IE;
    case WPA_ALLOC_FAIL:
        return WLAN_STATUS_UNSPECIFIED_FAILURE;
    case WPA_MGMT_FRAME_PROTECTION_VIOLATION:
        return WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION;
    case WPA_INVALID_MGMT_GROUP_CIPHER:
        return WLAN_STATUS_CIPHER_REJECTED_PER_POLICY;
    case WPA_INVALID_MDIE:
        return WLAN_STATUS_INVALID_MDIE;
    case WPA_INVALID_PROTO:
        return WLAN_STATUS_INVALID_IE;
    case WPA_INVALID_PMKID:
        return WLAN_STATUS_INVALID_PMKID;
    case WPA_DENIED_OTHER_REASON:
        return WLAN_STATUS_ASSOC_DENIED_UNSPEC;
    }
    return WLAN_STATUS_INVALID_IE;
}
