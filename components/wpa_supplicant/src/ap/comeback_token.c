/*
 * hostapd / Comeback token mechanism for SAE
 * Copyright (c) 2002-2017, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "hostapd.h"
#include "crypto/sha256.h"
#include "crypto/random.h"
#include "common/ieee802_11_defs.h"
#include "comeback_token.h"

#ifdef CONFIG_SAE

int comeback_token_hash(const u8 *comeback_key, const u8 *addr, u8 *idx)
{
    u8 hash[SHA256_MAC_LEN];

    if (hmac_sha256(comeback_key, COMEBACK_KEY_SIZE,
            addr, ETH_ALEN, hash) < 0)
        return -1;
    *idx = hash[0];
    return 0;
}


int check_comeback_token(const u8 *comeback_key,
                    u16 *comeback_pending_idx, const u8 *addr,
                    const u8 *token, size_t token_len)
{
    u8 mac[SHA256_MAC_LEN];
    const u8 *addrs[2];
    size_t len[2];
    u16 token_idx;
    u8 idx;

    if (token_len != SHA256_MAC_LEN ||
        comeback_token_hash(comeback_key, addr, &idx) < 0) {
        return -1;
    }
    token_idx = comeback_pending_idx[idx];
    if (token_idx == 0 || token_idx != WPA_GET_BE16(token)) {
        wpa_printf(MSG_DEBUG,
               "Comeback: Invalid anti-clogging token from "
               MACSTR " - token_idx 0x%04x, expected 0x%04x",
               MAC2STR(addr), WPA_GET_BE16(token), token_idx);
        return -1;
    }

    addrs[0] = addr;
    len[0] = ETH_ALEN;
    addrs[1] = token;
    len[1] = 2;
    if (hmac_sha256_vector(comeback_key, COMEBACK_KEY_SIZE,
                           2, addrs, len, mac) < 0 ||
        os_memcmp_const(token + 2, &mac[2], SHA256_MAC_LEN - 2) != 0) {
        return -1;
    }

    comeback_pending_idx[idx] = 0; /* invalidate used token */

    return 0;
}


struct wpabuf *
auth_build_token_req(struct os_reltime *last_comeback_key_update,
             u8 *comeback_key, u16 comeback_idx,
             u16 *comeback_pending_idx, size_t idx_len,
             int group, const u8 *addr, int h2e)
{
    struct wpabuf *buf;
    u8 *token;
    struct os_reltime now;
    u8 idx[2];
    const u8 *addrs[2];
    size_t len[2];
    u8 p_idx;
    u16 token_idx;

    os_get_time(&now);
    if (!os_reltime_initialized(last_comeback_key_update) ||
        os_reltime_expired(&now, last_comeback_key_update, 60) ||
        comeback_idx == 0xffff) {
        if (random_get_bytes(comeback_key, COMEBACK_KEY_SIZE) < 0) {
            return NULL;
        }
        wpa_hexdump(MSG_DEBUG, "Comeback: Updated token key",
                comeback_key, COMEBACK_KEY_SIZE);
        *last_comeback_key_update = now;
        comeback_idx = 0;
        os_memset(comeback_pending_idx, 0, idx_len);
    }

    buf = wpabuf_alloc(sizeof(le16) + 3 + SHA256_MAC_LEN);
    if (buf == NULL) {
        return NULL;
    }

    if (group)
        wpabuf_put_le16(buf, group); /* Finite Cyclic Group */

    if (h2e) {
        /* Encapsulate Anti-clogging Token field in a container IE */
        wpabuf_put_u8(buf, WLAN_EID_EXTENSION);
        wpabuf_put_u8(buf, 1 + SHA256_MAC_LEN);
        wpabuf_put_u8(buf, WLAN_EID_EXT_ANTI_CLOGGING_TOKEN);
    }

    if (comeback_token_hash(comeback_key, addr, &p_idx) < 0) {
        wpabuf_free(buf);
        return NULL;
    }

    token_idx = comeback_pending_idx[p_idx];
    if (!token_idx) {
        comeback_idx++;
        token_idx = comeback_idx;
        comeback_pending_idx[p_idx] = token_idx;
    }
    WPA_PUT_BE16(idx, token_idx);
    token = wpabuf_put(buf, SHA256_MAC_LEN);
    addrs[0] = addr;
    len[0] = ETH_ALEN;
    addrs[1] = idx;
    len[1] = sizeof(idx);
    if (hmac_sha256_vector(comeback_key, COMEBACK_KEY_SIZE,
                   2, addrs, len, token) < 0) {
        wpabuf_free(buf);
        return NULL;
    }
    WPA_PUT_BE16(token, token_idx);

    return buf;
}

#endif /* CONFIG_SAE */
