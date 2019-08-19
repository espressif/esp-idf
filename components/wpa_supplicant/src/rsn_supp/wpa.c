
/*
 * WPA Supplicant - WPA state machine and EAPOL-Key processing
 * Copyright (c) 2003-2010, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */
#include "utils/includes.h"

#include "utils/common.h"
#include "rsn_supp/wpa.h"
#include "rsn_supp/wpa_i.h"
#include "common/eapol_common.h"
#include "common/ieee802_11_defs.h"
#include "rsn_supp/wpa_ie.h"
#include "esp_supplicant/esp_wpas_glue.h"
#include "esp_supplicant/esp_wifi_driver.h"

#include "crypto/crypto.h"
#include "crypto/sha1.h"
#include "crypto/aes_wrap.h"

/**
 * eapol_sm_notify_eap_success - Notification of external EAP success trigger
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @success: %TRUE = set success, %FALSE = clear success
 *
 * Notify the EAPOL state machine that external event has forced EAP state to
 * success (success = %TRUE). This can be cleared by setting success = %FALSE.
 *
 * This function is called to update EAP state when WPA-PSK key handshake has
 * been completed successfully since WPA-PSK does not use EAP state machine.
 */

#define WPA_4_4_HANDSHAKE_BIT   (1<<13)
#define WPA_GROUP_HANDSHAKE_BIT (1<<14)
  struct wpa_sm gWpaSm;
/* fix buf for tx for now */
#define WPA_TX_MSG_BUFF_MAXLEN 200

#define ASSOC_IE_LEN 24
u8 assoc_ie_buf[ASSOC_IE_LEN+2]; 

void set_assoc_ie(u8 * assoc_buf);

int wpa_sm_set_key(struct install_key *sm, enum wpa_alg alg,
        u8 *addr, int key_idx, int set_tx,
        u8 *seq, size_t seq_len,
        u8 *key, size_t key_len,
        int  key_entry_valid);

int wpa_sm_get_key(uint8_t *ifx, int *alg, u8 *addr, int *key_idx, u8 *key, size_t key_len, int key_entry_valid);

void wpa_set_passphrase(char * passphrase, u8 *ssid, size_t ssid_len);

static inline enum wpa_states   wpa_sm_get_state(struct wpa_sm *sm)
{
    return sm->wpa_state;;
}

static inline void   wpa_sm_cancel_auth_timeout(struct wpa_sm *sm)
{

}

void   eapol_sm_notify_eap_success(Boolean success)
{

}
/**
 * get_bssid - Get the current BSSID
 * @priv: private driver interface data
 * @bssid: buffer for BSSID (ETH_ALEN = 6 bytes)
 *
 * Returns: 0 on success, -1 on failure
 *
 * Query kernel driver for the current BSSID and copy it to bssid.
 * Setting bssid to 00:00:00:00:00:00 is recommended if the STA is not
 * associated.
 */
static inline int   wpa_sm_get_bssid(struct wpa_sm *sm, u8 *bssid)
{
    memcpy(bssid, sm->bssid, ETH_ALEN);
    return 0;
}

 /*
 * wpa_ether_send - Send Ethernet frame
 * @wpa_s: Pointer to wpa_supplicant data
 * @dest: Destination MAC address
 * @proto: Ethertype in host byte order
 * @buf: Frame payload starting from IEEE 802.1X header
 * @len: Frame payload length
 * Returns: >=0 on success, <0 on failure
 */
static inline int   wpa_sm_ether_send( struct wpa_sm *sm, const u8 *dest, u16 proto,
        const u8 *data, size_t data_len)
{
    void *buffer = (void *)(data - sizeof(struct l2_ethhdr));
    struct l2_ethhdr *eth = (struct l2_ethhdr *)buffer;

    memcpy(eth->h_dest, dest, ETH_ALEN);
    memcpy(eth->h_source, sm->own_addr, ETH_ALEN);
    eth->h_proto = host_to_be16(proto);
    sm->sendto(buffer, sizeof(struct l2_ethhdr) + data_len);

    return 0;
}

/**
 * wpa_eapol_key_send - Send WPA/RSN EAPOL-Key message
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @kck: Key Confirmation Key (KCK, part of PTK)
 * @ver: Version field from Key Info
 * @dest: Destination address for the frame
 * @proto: Ethertype (usually ETH_P_EAPOL)
 * @msg: EAPOL-Key message
 * @msg_len: Length of message
 * @key_mic: Pointer to the buffer to which the EAPOL-Key MIC is written
 */
void   wpa_eapol_key_send(struct wpa_sm *sm, const u8 *kck,
            int ver, const u8 *dest, u16 proto,
            u8 *msg, size_t msg_len, u8 *key_mic)
{
    if (is_zero_ether_addr(dest) && is_zero_ether_addr(sm->bssid)) {
        /*
         * Association event was not yet received; try to fetch
         * BSSID from the driver.
         */
        if (wpa_sm_get_bssid(sm, sm->bssid) < 0) {
           #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: Failed to read BSSID for "
                   "EAPOL-Key destination address");
           #endif
        } else {
            dest = sm->bssid;
              #ifdef DEBUG_PRINT          
            wpa_printf(MSG_DEBUG, "WPA: Use BSSID (" MACSTR
                   ") as the destination for EAPOL-Key",
                   MAC2STR(dest));
           #endif
        }
    }
    if (key_mic &&
        wpa_eapol_key_mic(kck, ver, msg, msg_len, key_mic)) {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Failed to generate EAPOL-Key "
               "version %d MIC", ver);
         #endif    
        goto out;
    }
    wpa_hexdump(MSG_MSGDUMP, "WPA: TX EAPOL-Key", msg, msg_len);
    wpa_sm_ether_send(sm, dest, proto, msg, msg_len);
out:
    return;
}

/**
 * wpa_sm_key_request - Send EAPOL-Key Request
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @error: Indicate whether this is an Michael MIC error report
 * @pairwise: 1 = error report for pairwise packet, 0 = for group packet
 *
 * Send an EAPOL-Key Request to the current authenticator. This function is
 * used to request rekeying and it is usually called when a local Michael MIC
 * failure is detected.
 */
void   wpa_sm_key_request(struct wpa_sm *sm, int error, int pairwise)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    int key_info, ver;
    u8 bssid[ETH_ALEN], *rbuf;

    if (wpa_key_mgmt_ft(sm->key_mgmt) || wpa_key_mgmt_sha256(sm->key_mgmt))
        ver = WPA_KEY_INFO_TYPE_AES_128_CMAC;
    else if (sm->pairwise_cipher == WPA_CIPHER_CCMP)
        ver = WPA_KEY_INFO_TYPE_HMAC_SHA1_AES;
    else
        ver = WPA_KEY_INFO_TYPE_HMAC_MD5_RC4;

    if (wpa_sm_get_bssid(sm, bssid) < 0) {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "Failed to read BSSID for EAPOL-Key "
               "request");
        #endif    
        return;
    }

    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                  sizeof(*reply), &rlen, (void *) &reply);
    if (rbuf == NULL)
        return;

    reply->type = sm->proto == WPA_PROTO_RSN ?
        EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info = WPA_KEY_INFO_REQUEST | ver;
    if (sm->ptk_set)
        key_info |= WPA_KEY_INFO_MIC;
    if (error)
        key_info |= WPA_KEY_INFO_ERROR|WPA_KEY_INFO_SECURE;
    if (pairwise)
        key_info |= WPA_KEY_INFO_KEY_TYPE;
    WPA_PUT_BE16(reply->key_info, key_info);
    WPA_PUT_BE16(reply->key_length, 0);
    memcpy(reply->replay_counter, sm->request_counter,
          WPA_REPLAY_COUNTER_LEN);
    inc_byte_array(sm->request_counter, WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, 0);

   #ifdef DEBUG_PRINT    
    wpa_printf(MSG_DEBUG, "WPA: Sending EAPOL-Key Request (error=%d "
           "pairwise=%d ptk_set=%d len=%lu)",
           error, pairwise, sm->ptk_set, (unsigned long) rlen);
   #endif
    wpa_eapol_key_send(sm, sm->ptk.kck, ver, bssid, ETH_P_EAPOL,
               rbuf, rlen, key_info & WPA_KEY_INFO_MIC ?
               reply->key_mic : NULL);
    wpa_sm_free_eapol(rbuf);
}

int   wpa_supplicant_get_pmk(struct wpa_sm *sm)
{   
       if(sm->pmk_len >0) {
        return 0;
       } else {
           return 1;
    }     
}

/**
 * wpa_supplicant_send_2_of_4 - Send message 2 of WPA/RSN 4-Way Handshake
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @dst: Destination address for the frame
 * @key: Pointer to the EAPOL-Key frame header
 * @ver: Version bits from EAPOL-Key Key Info
 * @nonce: Nonce value for the EAPOL-Key frame
 * @wpa_ie: WPA/RSN IE
 * @wpa_ie_len: Length of the WPA/RSN IE
 * @ptk: PTK to use for keyed hash and encryption
 * Returns: 0 on success, -1 on failure
 */
int   wpa_supplicant_send_2_of_4(struct wpa_sm *sm, const unsigned char *dst,
                   const struct wpa_eapol_key *key,
                   int ver, const u8 *nonce,
                   const u8 *wpa_ie, size_t wpa_ie_len,
                   struct wpa_ptk *ptk)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    u8 *rbuf;

    if (wpa_ie == NULL) {
       #ifdef DEBUG_PRINT    
        wpa_printf(MSG_ERROR, "WPA: No wpa_ie set - cannot "
               "generate msg 2/4");
       #endif
        return -1;
    }

    wpa_hexdump(MSG_MSGDUMP, "WPA: WPA IE for msg 2/4\n", wpa_ie, wpa_ie_len);

    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY,
                  NULL, sizeof(*reply) + wpa_ie_len,
                  &rlen, (void *) &reply);
    if (rbuf == NULL) {
        return -1;
    }

    reply->type = sm->proto == WPA_PROTO_RSN ?
        EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    WPA_PUT_BE16(reply->key_info,
             ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        memcpy(reply->key_length, key->key_length, 2);

    memcpy(reply->replay_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, wpa_ie_len);
    memcpy(reply + 1, wpa_ie, wpa_ie_len);

    memcpy(reply->key_nonce, nonce, WPA_NONCE_LEN);
    
    wpa_printf(MSG_DEBUG, "WPA Send EAPOL-Key 2/4\n");
    
    wpa_eapol_key_send(sm, ptk->kck, ver, dst, ETH_P_EAPOL,
               rbuf, rlen, reply->key_mic);
    wpa_sm_free_eapol(rbuf);

    return 0;
}

int   wpa_derive_ptk(struct wpa_sm *sm, const unsigned char *src_addr,
              const struct wpa_eapol_key *key,
              struct wpa_ptk *ptk)
{
    size_t ptk_len = sm->pairwise_cipher == WPA_CIPHER_CCMP ? 48 : 64;

    wpa_pmk_to_ptk(sm->pmk, sm->pmk_len, "Pairwise key expansion",
               sm->own_addr, sm->bssid, sm->snonce, key->key_nonce,
               (u8 *) ptk, ptk_len,
               wpa_key_mgmt_sha256(sm->key_mgmt));
    return 0;
}

void   wpa_supplicant_process_1_of_4(struct wpa_sm *sm,
                      const unsigned char *src_addr,
                      const struct wpa_eapol_key *key,
                      u16 ver)
{
    struct wpa_eapol_ie_parse ie;
    struct wpa_ptk *ptk;
    int res;

    wpa_sm_set_state(WPA_FIRST_HALF_4WAY_HANDSHAKE);

    wpa_printf(MSG_DEBUG, "WPA 1/4-Way Handshake\n");

    memset(&ie, 0, sizeof(ie));

#ifndef CONFIG_NO_WPA2
    if (sm->proto == WPA_PROTO_RSN) {
        /* RSN: msg 1/4 should contain PMKID for the selected PMK */
        const u8 *_buf = (const u8 *) (key + 1);
        size_t len = WPA_GET_BE16(key->key_data_length);    
        wpa_hexdump(MSG_MSGDUMP, "RSN: msg 1/4 key data", _buf, len);
        wpa_supplicant_parse_ies(_buf, len, &ie);
        if (ie.pmkid) {
            wpa_hexdump(MSG_DEBUG, "RSN: PMKID from "
                    "Authenticator", ie.pmkid, PMKID_LEN);
        }
    }
#endif /* CONFIG_NO_WPA2 */

       res = wpa_supplicant_get_pmk(sm);
       if (res == -2) {
          #ifdef DEBUG_PRINT    
         wpa_printf(MSG_DEBUG, "RSN: Do not reply to msg 1/4 - "
               "requesting full EAP authentication");
          #endif     
        return;
    }
    if (res)
        goto failed;

    if (sm->renew_snonce) {
        if (os_get_random(sm->snonce, WPA_NONCE_LEN)) {
         #ifdef DEBUG_PRINT            
            wpa_printf(MSG_DEBUG, "WPA: Failed to get random data for SNonce");
         #endif     
            goto failed;
        }
         
        sm->renew_snonce = 0;
        wpa_hexdump(MSG_DEBUG, "WPA: Renewed SNonce",
                sm->snonce, WPA_NONCE_LEN);
    }

    /* Calculate PTK which will be stored as a temporary PTK until it has
     * been verified when processing message 3/4. */
    ptk = &sm->tptk;
    wpa_derive_ptk(sm, src_addr, key, ptk);
    /* Supplicant: swap tx/rx Mic keys */
    sm->tptk_set = 1;
    sm->ptk_set = 0;
    sm->key_install = true;

    if (wpa_supplicant_send_2_of_4(sm, sm->bssid, key, ver, sm->snonce,
                       sm->assoc_wpa_ie, sm->assoc_wpa_ie_len,
                       ptk))
        goto failed;

    memcpy(sm->anonce, key->key_nonce, WPA_NONCE_LEN);
    return;

failed:
    wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
}

  void   wpa_sm_rekey_ptk(void *eloop_ctx, void *timeout_ctx)
{
    struct wpa_sm *sm = eloop_ctx;

   #ifdef DEBUG_PRINT    
    wpa_printf(MSG_DEBUG, "WPA: Request PTK rekeying");
   #endif
    wpa_sm_key_request(sm, 0, 1);
}


int   wpa_supplicant_install_ptk(struct wpa_sm *sm)
{
    int keylen;
    enum wpa_alg alg;

   #ifdef DEBUG_PRINT    
    wpa_printf(MSG_DEBUG, "WPA: Installing PTK to the driver.\n");
   #endif
   
    switch (sm->pairwise_cipher) {
    case WPA_CIPHER_CCMP:
        alg = WPA_ALG_CCMP;
        keylen = 16;
        break;
    case WPA_CIPHER_TKIP:
        alg = WPA_ALG_TKIP;
        keylen = 32;
        break;
    case WPA_CIPHER_NONE:
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Pairwise Cipher Suite: "
               "NONE - do not use pairwise keys");
        #endif    
        return 0;
    default:
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Unsupported pairwise cipher %d",
               sm->pairwise_cipher);
           #endif
        return -1;
    }

    //now only use keyentry 0 for pairwise key
    sm->key_entry_valid = 5;

    if (wpa_sm_set_key(&(sm->install_ptk), alg, sm->bssid, 0, 1, (sm->install_ptk).seq, WPA_KEY_RSC_LEN,
               (u8 *) sm->ptk.tk1, keylen,sm->key_entry_valid) < 0) {
        #ifdef DEBUG_PRINT           
        wpa_printf(MSG_DEBUG, "WPA: Failed to set PTK to the "
               "driver (alg=%d keylen=%d bssid=" MACSTR ")",
               alg, keylen, MAC2STR(sm->bssid));
        #endif    
        return -1;
    }
       
    if (sm->wpa_ptk_rekey) {
        eloop_cancel_timeout(wpa_sm_rekey_ptk, sm, NULL);
        eloop_register_timeout(sm->wpa_ptk_rekey, 0, wpa_sm_rekey_ptk,
                       sm, NULL);
    }

    return 0;
}

int   wpa_supplicant_check_group_cipher(int group_cipher,
                         int keylen, int maxkeylen,
                         int *key_rsc_len,
                         enum wpa_alg *alg)
{
    int ret = 0;

    switch (group_cipher) {
    case WPA_CIPHER_CCMP:
        if (keylen != 16 || maxkeylen < 16) {
            ret = -1;
            break;
        }
        *key_rsc_len = 6;
        *alg = WPA_ALG_CCMP;
        break;
    case WPA_CIPHER_TKIP:
        if (keylen != 32 || maxkeylen < 32) {
            ret = -1;
            break;
        }
        *key_rsc_len = 6;
        *alg = WPA_ALG_TKIP;
        break;
    case WPA_CIPHER_WEP104:
        if (keylen != 13 || maxkeylen < 13) {
            ret = -1;
            break;
        }
        *key_rsc_len = 0;
        *alg = WPA_ALG_WEP104;
        break;
    case WPA_CIPHER_WEP40:
        if (keylen != 5 || maxkeylen < 5) {
            ret = -1;
            break;
        }
        *key_rsc_len = 0;
        *alg = WPA_ALG_WEP40;
        break;
    default:
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Unsupported Group Cipher %d",
               group_cipher);
        #endif
        return -1;
    }

    if (ret < 0 ) {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Unsupported %s Group Cipher key "
               "length %d (%d).",
               wpa_cipher_txt(group_cipher), keylen, maxkeylen);
        #endif    
    }

    return ret;
}

void   wpa_supplicant_key_neg_complete(struct wpa_sm *sm,
                        const u8 *addr, int secure)
{
#ifdef DEBUG_PRINT
    wpa_printf(MSG_DEBUG, "WPA: Key negotiation completed with "
          MACSTR " [PTK=%s GTK=%s]\n", MAC2STR(addr),
          wpa_cipher_txt(sm->pairwise_cipher),
          wpa_cipher_txt(sm->group_cipher));
#endif     
    wpa_sm_cancel_auth_timeout(sm);
    wpa_sm_set_state(WPA_COMPLETED);

    sm->wpa_neg_complete();

    if (secure) {
        wpa_sm_mlme_setprotection(
            sm, addr, MLME_SETPROTECTION_PROTECT_TYPE_RX_TX,
            MLME_SETPROTECTION_KEY_TYPE_PAIRWISE);

        if (wpa_key_mgmt_wpa_psk(sm->key_mgmt))
            eapol_sm_notify_eap_success(TRUE);
        /*
         * Start preauthentication after a short wait to avoid a
         * possible race condition between the data receive and key
         * configuration after the 4-Way Handshake. This increases the
         * likelyhood of the first preauth EAPOL-Start frame getting to
         * the target AP.
         */ 
    }

}


int   wpa_supplicant_install_gtk(struct wpa_sm *sm,
                      struct wpa_gtk_data *gd)
{
    u8 *_gtk = gd->gtk;
    u8 gtk_buf[32];
       u8 *key_rsc=(sm->install_gtk).seq;
       
    wpa_hexdump(MSG_DEBUG, "WPA: Group Key", gd->gtk, gd->gtk_len);

    #ifdef DEBUG_PRINT    
    wpa_printf(MSG_DEBUG, "WPA: Installing GTK to the driver "
           "(keyidx=%d tx=%d len=%d).\n", gd->keyidx, gd->tx,
           gd->gtk_len);
    #endif    
    wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, gd->key_rsc_len);
    if (sm->group_cipher == WPA_CIPHER_TKIP) {
        /* Swap Tx/Rx keys for Michael MIC */
        memcpy(gtk_buf, gd->gtk, 16);
        memcpy(gtk_buf + 16, gd->gtk + 16, 8);
        memcpy(gtk_buf + 24, gd->gtk + 24, 8);
        _gtk = gtk_buf;
    }
    //now only use keycache entry1 for group key
    sm->key_entry_valid = gd->keyidx;
    if (sm->pairwise_cipher == WPA_CIPHER_NONE) {
        if (wpa_sm_set_key(&(sm->install_gtk), gd->alg,
                   sm->bssid, //(u8 *) "\xff\xff\xff\xff\xff\xff",
                   gd->keyidx, 1, key_rsc, gd->key_rsc_len,
                   _gtk, gd->gtk_len,sm->key_entry_valid) < 0) {
            #ifdef DEBUG_PRINT           
            wpa_printf(MSG_DEBUG, "WPA: Failed to set "
                   "GTK to the driver (Group only).");
            #endif    
            return -1;
        }
    } else if (wpa_sm_set_key(&(sm->install_gtk), gd->alg,
                  sm->bssid, //(u8 *) "\xff\xff\xff\xff\xff\xff",
                  gd->keyidx, gd->tx, key_rsc, gd->key_rsc_len,
                  _gtk, gd->gtk_len, sm->key_entry_valid) < 0) {
        #ifdef DEBUG_PRINT          
        wpa_printf(MSG_DEBUG, "WPA: Failed to set GTK to "
               "the driver (alg=%d keylen=%d keyidx=%d)",
               gd->alg, gd->gtk_len, gd->keyidx);
        #endif    
        return -1;
    }

    return 0;
}

bool wpa_supplicant_gtk_in_use(struct wpa_sm *sm, struct wpa_gtk_data *gd)
{
    u8 *_gtk = gd->gtk;
    u8 gtk_buf[32];
    u8 gtk_get[32] = {0};
    u8 ifx;
    int alg;
    u8 bssid[6];
    int keyidx;

    wpa_hexdump(MSG_DEBUG, "WPA: Group Key", gd->gtk, gd->gtk_len);

    #ifdef DEBUG_PRINT
    wpa_printf(MSG_DEBUG, "WPA: Judge GTK: (keyidx=%d len=%d).", gd->keyidx, gd->gtk_len);
    #endif

    if (sm->group_cipher == WPA_CIPHER_TKIP) {
        /* Swap Tx/Rx keys for Michael MIC */
        memcpy(gtk_buf, gd->gtk, 16);
        memcpy(gtk_buf + 16, gd->gtk + 16, 8);
        memcpy(gtk_buf + 24, gd->gtk + 24, 8);
        _gtk = gtk_buf;
    }

    //check if gtk is in use.
    if (wpa_sm_get_key(&ifx, &alg, bssid, &keyidx, gtk_get, gd->gtk_len, gd->keyidx) == 0) {
        if (ifx == 0 && alg == gd->alg && memcmp(bssid, sm->bssid, ETH_ALEN) == 0 &&
        		memcmp(_gtk, gtk_get, gd->gtk_len) == 0) {
            wpa_printf(MSG_DEBUG, "GTK %d is already in use in entry %d, it may be an attack, ignor it.", gd->keyidx, gd->keyidx + 2);
            return true;
        }
    }

    if (wpa_sm_get_key(&ifx, &alg, bssid, &keyidx, gtk_get, gd->gtk_len, (gd->keyidx+1)%2) == 0) {
    	if (ifx == 0 && alg == gd->alg && memcmp(bssid, sm->bssid, ETH_ALEN) == 0 &&
    			memcmp(_gtk, gtk_get, gd->gtk_len) == 0) {
            wpa_printf(MSG_DEBUG, "GTK %d is already in use in entry %d, it may be an attack, ignor it.", gd->keyidx, (gd->keyidx+1)%2 + 2);
            return true;
        }
    }

    return false;
}

int wpa_supplicant_gtk_tx_bit_workaround(const struct wpa_sm *sm,
                        int tx)
{
    if (tx && sm->pairwise_cipher != WPA_CIPHER_NONE) {
        /* Ignore Tx bit for GTK if a pairwise key is used. One AP
         * seemed to set this bit (incorrectly, since Tx is only when
         * doing Group Key only APs) and without this workaround, the
         * data connection does not work because wpa_supplicant
         * configured non-zero keyidx to be used for unicast. */
        #ifdef DEBUG_PRINT     
        wpa_printf(MSG_DEBUG, "WPA: Tx bit set for GTK, but pairwise "
               "keys are used - ignore Tx bit");
        #endif    
        return 0;
    }
    return tx;
}

int wpa_supplicant_pairwise_gtk(struct wpa_sm *sm,
                       const u8 *gtk, size_t gtk_len,
                       int key_info)
{
#ifndef CONFIG_NO_WPA2
    struct wpa_gtk_data *gd=&(sm->gd);

    /*
     * IEEE Std 802.11i-2004 - 8.5.2 EAPOL-Key frames - Figure 43x
     * GTK KDE format:
     * KeyID[bits 0-1], Tx [bit 2], Reserved [bits 3-7]
     * Reserved [bits 0-7]
     * GTK
     */

    memset(gd, 0, sizeof(struct wpa_gtk_data));
    wpa_hexdump(MSG_DEBUG, "RSN: received GTK in pairwise handshake",
            gtk, gtk_len);

    if (gtk_len < 2 || gtk_len - 2 > sizeof(gd->gtk))
        return -1;

    gd->keyidx = gtk[0] & 0x3;
    gd->tx = wpa_supplicant_gtk_tx_bit_workaround(sm,
                             !!(gtk[0] & BIT(2)));
    gtk += 2;
    gtk_len -= 2;

    memcpy(gd->gtk, gtk, gtk_len);
    gd->gtk_len = gtk_len;

    if (wpa_supplicant_check_group_cipher(sm->group_cipher,
                          gtk_len, gtk_len,
                          &(gd->key_rsc_len), &(gd->alg))) {
            #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "RSN: Failed to install GTK");
        #endif        
        return -1;
    }
    return 0;
#else /* CONFIG_NO_WPA2 */
    return -1;
#endif /* CONFIG_NO_WPA2 */
}

#ifdef DEBUG_PRINT
void wpa_report_ie_mismatch(struct wpa_sm *sm,
                   const char *reason, const u8 *src_addr,
                   const u8 *wpa_ie, size_t wpa_ie_len,
                   const u8 *rsn_ie, size_t rsn_ie_len)
#else
void wpa_report_ie_mismatch(struct wpa_sm *sm, const u8 *src_addr,
                   const u8 *wpa_ie, size_t wpa_ie_len,
                   const u8 *rsn_ie, size_t rsn_ie_len)
#endif
{
    #ifdef DEBUG_PRINT    
    wpa_printf(MSG_DEBUG, "WPA: %s (src=" MACSTR ")",
        reason, MAC2STR(src_addr));
    #endif
    if (sm->ap_wpa_ie) {
        wpa_hexdump(MSG_INFO, "WPA: WPA IE in Beacon/ProbeResp",
                sm->ap_wpa_ie, sm->ap_wpa_ie_len);
    }
    if (wpa_ie) {
        if (!sm->ap_wpa_ie) {
            #ifdef DEBUG_PRINT
            wpa_printf(MSG_DEBUG, "WPA: No WPA IE in "
                   "Beacon/ProbeResp");
            #endif    
        }
        wpa_hexdump(MSG_INFO, "WPA: WPA IE in 3/4 msg",
                wpa_ie, wpa_ie_len);
    }

    if (sm->ap_rsn_ie) {
        wpa_hexdump(MSG_INFO, "WPA: RSN IE in Beacon/ProbeResp",
                sm->ap_rsn_ie, sm->ap_rsn_ie_len);
    }
    if (rsn_ie) {
        if (!sm->ap_rsn_ie) {
            #ifdef DEBUG_PRINT
            wpa_printf(MSG_DEBUG, "WPA: No RSN IE in "
                   "Beacon/ProbeResp");
            #endif    
        }
        wpa_hexdump(MSG_INFO, "WPA: RSN IE in 3/4 msg",
                rsn_ie, rsn_ie_len);
    }

    wpa_sm_disassociate(sm, WLAN_REASON_IE_IN_4WAY_DIFFERS);
}

int   ieee80211w_set_keys(struct wpa_sm *sm,
                   struct wpa_eapol_ie_parse *ie)
{
     return 0;
}

  int   wpa_supplicant_validate_ie(struct wpa_sm *sm,
                      const unsigned char *src_addr,
                      struct wpa_eapol_ie_parse *ie)
{
    if (sm->ap_wpa_ie == NULL && sm->ap_rsn_ie == NULL) {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: No WPA/RSN IE for this AP known. "
               "Trying to get from scan results\n");
        #endif    
        if (wpa_sm_get_beacon_ie(sm) < 0) {
            #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: Could not find AP from "
                   "the scan results");
            #endif    
        } else {
            #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: Found the current AP from "
                   "updated scan results\n");
            #endif    
        }
    }

    if (ie->wpa_ie == NULL && ie->rsn_ie == NULL &&
        (sm->ap_wpa_ie || sm->ap_rsn_ie)) {
#ifdef DEBUG_PRINT
        wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                       "with IE in Beacon/ProbeResp (no IE?)",
                       src_addr, ie->wpa_ie, ie->wpa_ie_len,
                       ie->rsn_ie, ie->rsn_ie_len);
#else
        wpa_report_ie_mismatch(sm,
                       src_addr, ie->wpa_ie, ie->wpa_ie_len,
                       ie->rsn_ie, ie->rsn_ie_len);
#endif
        return -1;
    }

    if ((ie->wpa_ie && sm->ap_wpa_ie &&
         (ie->wpa_ie_len != sm->ap_wpa_ie_len ||
         memcmp(ie->wpa_ie, sm->ap_wpa_ie, ie->wpa_ie_len) != 0)) ||
        (ie->rsn_ie && sm->ap_rsn_ie &&
         wpa_compare_rsn_ie(wpa_key_mgmt_ft(sm->key_mgmt),
                sm->ap_rsn_ie, sm->ap_rsn_ie_len,
                ie->rsn_ie, ie->rsn_ie_len))) {
#ifdef DEBUG_PRINT
        wpa_report_ie_mismatch(sm, "IE in 3/4 msg does not match "
                       "with IE in Beacon/ProbeResp",
                       src_addr, ie->wpa_ie, ie->wpa_ie_len,
                       ie->rsn_ie, ie->rsn_ie_len);
#else
        wpa_report_ie_mismatch(sm,
                       src_addr, ie->wpa_ie, ie->wpa_ie_len,
                       ie->rsn_ie, ie->rsn_ie_len);
#endif
        return -1;
    }

    if (sm->proto == WPA_PROTO_WPA &&
        ie->rsn_ie && sm->ap_rsn_ie == NULL && sm->rsn_enabled) {
#ifdef DEBUG_PRINT
        wpa_report_ie_mismatch(sm, "Possible downgrade attack "
                       "detected - RSN was enabled and RSN IE "
                       "was in msg 3/4, but not in "
                       "Beacon/ProbeResp",
                       src_addr, ie->wpa_ie, ie->wpa_ie_len,
                       ie->rsn_ie, ie->rsn_ie_len);
#else
        wpa_report_ie_mismatch(sm,
                               src_addr, ie->wpa_ie, ie->wpa_ie_len,
                               ie->rsn_ie, ie->rsn_ie_len);
#endif
        return -1;
    }

    return 0;
}

/**
 * wpa_supplicant_send_4_of_4 - Send message 4 of WPA/RSN 4-Way Handshake
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @dst: Destination address for the frame
 * @key: Pointer to the EAPOL-Key frame header
 * @ver: Version bits from EAPOL-Key Key Info
 * @key_info: Key Info
 * @kde: KDEs to include the EAPOL-Key frame
 * @kde_len: Length of KDEs
 * @ptk: PTK to use for keyed hash and encryption
 * Returns: 0 on success, -1 on failure
 */
  int   wpa_supplicant_send_4_of_4(struct wpa_sm *sm, const unsigned char *dst,
                   const struct wpa_eapol_key *key,
                   u16 ver, u16 key_info,
                   const u8 *kde, size_t kde_len,
                   struct wpa_ptk *ptk)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    u8 *rbuf;

    if (kde)
        wpa_hexdump(MSG_DEBUG, "WPA: KDE for msg 4/4", kde, kde_len);

    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                  sizeof(*reply) + kde_len,
                  &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;

    sm->txcb_flags |= WPA_4_4_HANDSHAKE_BIT;
    wpa_printf(MSG_DEBUG, "tx 4/4 txcb_flags=%d\n", sm->txcb_flags);

    reply->type = sm->proto == WPA_PROTO_RSN ?
        EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info &= WPA_KEY_INFO_SECURE;
    key_info |= ver | WPA_KEY_INFO_KEY_TYPE | WPA_KEY_INFO_MIC;
    WPA_PUT_BE16(reply->key_info, key_info);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        memcpy(reply->key_length, key->key_length, 2);
    memcpy(reply->replay_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, kde_len);
    if (kde)
        memcpy(reply + 1, kde, kde_len);

    wpa_printf(MSG_DEBUG, "WPA Send EAPOL-Key 4/4\n");
    wpa_eapol_key_send(sm, ptk->kck, ver, dst, ETH_P_EAPOL,
               rbuf, rlen, reply->key_mic);
    wpa_sm_free_eapol(rbuf);

    return 0;
}

  void   wpa_sm_set_seq(struct wpa_sm *sm, struct wpa_eapol_key *key, u8 isptk)
{    
    u8 *key_rsc, *seq;
    u8 null_rsc[WPA_KEY_RSC_LEN];
    
    os_bzero(null_rsc, WPA_KEY_RSC_LEN);

    if (sm->proto == WPA_PROTO_RSN) {
        key_rsc = null_rsc;
    } else {
        key_rsc = key->key_rsc;
        wpa_hexdump(MSG_DEBUG, "WPA: RSC", key_rsc, WPA_KEY_RSC_LEN);
    }

    seq=(isptk) ? (sm->install_ptk).seq : (sm->install_gtk).seq;  
    memcpy(seq, key_rsc, WPA_KEY_RSC_LEN);
}

  void   wpa_supplicant_process_3_of_4(struct wpa_sm *sm,
                      struct wpa_eapol_key *key,
                      u16 ver)
{
    u16 key_info, keylen, len;
    const u8 *pos;
    struct wpa_eapol_ie_parse ie;

    wpa_sm_set_state(WPA_LAST_HALF_4WAY_HANDSHAKE);
    wpa_printf(MSG_DEBUG, "WPA 3/4-Way Handshake\n");

    key_info = WPA_GET_BE16(key->key_info);

    pos = (const u8 *) (key + 1);
    len = WPA_GET_BE16(key->key_data_length);
    wpa_hexdump(MSG_DEBUG, "WPA: IE KeyData", pos, len);
    wpa_supplicant_parse_ies(pos, len, &ie);
    if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: GTK IE in unencrypted key data");
        #endif    
        goto failed;
    }

    if (wpa_supplicant_validate_ie(sm, sm->bssid, &ie) < 0)
        goto failed;

    if (memcmp(sm->anonce, key->key_nonce, WPA_NONCE_LEN) != 0) {
       #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: ANonce from message 1 of 4-Way "
               "Handshake differs from 3 of 4-Way Handshake - drop"
               " packet (src=" MACSTR ")", MAC2STR(sm->bssid));
     #endif  
        goto failed;
    }

    keylen = WPA_GET_BE16(key->key_length);
    switch (sm->pairwise_cipher) {
    case WPA_CIPHER_CCMP:
        if (keylen != 16) {
            #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: Invalid CCMP key length "
                   "%d (src=" MACSTR ")",
                   keylen, MAC2STR(sm->bssid));
            #endif    
            goto failed;
        }
        break;
    case WPA_CIPHER_TKIP:
        if (keylen != 32) {
            #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: Invalid TKIP key length "
                   "%d (src=" MACSTR ")",
                   keylen, MAC2STR(sm->bssid));
            #endif    
            goto failed;
        }
        break;
    }

    
    /* SNonce was successfully used in msg 3/4, so mark it to be renewed
     * for the next 4-Way Handshake. If msg 3 is received again, the old
     * SNonce will still be used to avoid changing PTK. */
    sm->renew_snonce = 1;

    /*ready for txcallback , set seq and set txcallback param*/
    wpa_sm_set_seq(sm, key, 1);
    sm->key_info=key_info;
    (sm->gd).gtk_len=0; //used as flag if gtk is installed in callback
    if (ie.gtk) {
        wpa_sm_set_seq(sm, key, 0);
           if (wpa_supplicant_pairwise_gtk(sm, 
                    ie.gtk, ie.gtk_len, key_info) < 0) {
        #ifdef DEBUG_PRINT            
        wpa_printf(MSG_DEBUG, "RSN: Failed to configure GTK");
        #endif    
        goto failed;
        }
    }
    
    if (ieee80211w_set_keys(sm, &ie) < 0) {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "RSN: Failed to configure IGTK");
        #endif    
        goto failed;
    }

    /*after txover, callback will continue run remain task*/
    if (wpa_supplicant_send_4_of_4(sm, sm->bssid, key, ver, key_info,
                       NULL, 0, &sm->ptk)) {
        goto failed;
    }

    return;

failed:
    wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
}


  int   wpa_supplicant_send_4_of_4_txcallback(struct wpa_sm *sm)
{
       u16 key_info=sm->key_info;

    if (sm->key_install && key_info & WPA_KEY_INFO_INSTALL) {
        if (wpa_supplicant_install_ptk(sm))
            goto failed;
    }
    else if (sm->key_install == false) {
        wpa_printf(MSG_DEBUG, "PTK has been installed, it may be an attack, ignor it.");
    }

    wpa_sm_set_state(WPA_GROUP_HANDSHAKE);
    
    if((sm->gd).gtk_len) {
    	if (sm->key_install) {
          if (wpa_supplicant_install_gtk(sm, &(sm->gd))) 
            goto failed;
    	}
    	else {
    	    wpa_printf(MSG_DEBUG, "GTK has been installed, it may be an attack, ignor it.");
    	}
        wpa_supplicant_key_neg_complete(sm, sm->bssid,
                    key_info & WPA_KEY_INFO_SECURE);
    }


    if (key_info & WPA_KEY_INFO_SECURE) {
        wpa_sm_mlme_setprotection(
            sm, sm->bssid, MLME_SETPROTECTION_PROTECT_TYPE_RX,
            MLME_SETPROTECTION_KEY_TYPE_PAIRWISE);
    }

    sm->key_install = false;

    return 0;

failed:
       return WLAN_REASON_UNSPECIFIED; 
}


  int   wpa_supplicant_process_1_of_2_rsn(struct wpa_sm *sm,
                         const u8 *keydata,
                         size_t keydatalen,
                         u16 key_info,
                         struct wpa_gtk_data *gd)
{
    int maxkeylen;
    struct wpa_eapol_ie_parse ie;

    wpa_hexdump(MSG_DEBUG, "RSN: msg 1/2 key data", keydata, keydatalen);
    wpa_supplicant_parse_ies(keydata, keydatalen, &ie);
    if (ie.gtk && !(key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: GTK IE in unencrypted key data");
      #endif    
        return -1;
    }
    if (ie.gtk == NULL) {
         #ifdef DEBUG_PRINT        
        wpa_printf(MSG_DEBUG, "WPA: No GTK IE in Group Key msg 1/2");
       #endif     
        return -1;
    }
    maxkeylen = gd->gtk_len = ie.gtk_len - 2;

    if (wpa_supplicant_check_group_cipher(sm->group_cipher,
                          gd->gtk_len, maxkeylen,
                          &gd->key_rsc_len, &gd->alg))
        return -1;

    wpa_hexdump(MSG_DEBUG, "RSN: received GTK in group key handshake",
            ie.gtk, ie.gtk_len);
    gd->keyidx = ie.gtk[0] & 0x3;
    gd->tx = wpa_supplicant_gtk_tx_bit_workaround(sm,
                              !!(ie.gtk[0] & BIT(2)));
    if (ie.gtk_len - 2 > sizeof(gd->gtk)) {
         #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "RSN: Too long GTK in GTK IE "
               "(len=%lu)", (unsigned long) ie.gtk_len - 2);
      #endif     
        return -1;
    }
    memcpy(gd->gtk, ie.gtk + 2, ie.gtk_len - 2);

    if (ieee80211w_set_keys(sm, &ie) < 0)
    {   
        #ifdef DEBUG_PRINT
        wpa_printf(MSG_DEBUG, "RSN: Failed to configure IGTK");
            #endif
    }       
    return 0;
}

  int   wpa_supplicant_process_1_of_2_wpa(struct wpa_sm *sm,
                         const struct wpa_eapol_key *key,
                         size_t keydatalen, int key_info,
                         size_t extra_len, u16 ver,
                         struct wpa_gtk_data *gd)
{
    size_t maxkeylen;
    u8 ek[32];

    gd->gtk_len = WPA_GET_BE16(key->key_length);
    maxkeylen = keydatalen;
    if (keydatalen > extra_len) {
         #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Truncated EAPOL-Key packet:"
               " key_data_length=%lu > extra_len=%lu",
               (unsigned long) keydatalen,
               (unsigned long) extra_len);
      #endif     
        return -1;
    }
    if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
        if (maxkeylen < 8) {
              #ifdef DEBUG_PRINT            
            wpa_printf(MSG_DEBUG, "WPA: Too short maxkeylen (%lu)",
                   (unsigned long) maxkeylen);
        #endif      
            return -1;
        }
        maxkeylen -= 8;
    }

    if (wpa_supplicant_check_group_cipher(sm->group_cipher,
                          gd->gtk_len, maxkeylen,
                          &gd->key_rsc_len, &gd->alg))
        return -1;

    gd->keyidx = (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) >>
        WPA_KEY_INFO_KEY_INDEX_SHIFT;
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4) {
        memcpy(ek, key->key_iv, 16);
        memcpy(ek + 16, sm->ptk.kek, 16);
        if (keydatalen > sizeof(gd->gtk)) {
            #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: RC4 key data "
                   "too long (%lu)",
                   (unsigned long) keydatalen);
            #endif    
            return -1;
        }
        memcpy(gd->gtk, key + 1, keydatalen);
        if (rc4_skip(ek, 32, 256, gd->gtk, keydatalen)) {
            #ifdef DEBUG_PRINT
            wpa_printf(MSG_DEBUG, "WPA: RC4 failed");
            #endif
            return -1;
        }
    } else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
        if (keydatalen % 8) {
            #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: Unsupported AES-WRAP "
                   "len %lu", (unsigned long) keydatalen);
             #endif    
            return -1;
        }
        if (maxkeylen > sizeof(gd->gtk)) {
                 #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: AES-WRAP key data "
                   "too long (keydatalen=%lu maxkeylen=%lu)",
                   (unsigned long) keydatalen,
                   (unsigned long) maxkeylen);
            #endif      
            return -1;
        }
        if (aes_unwrap(sm->ptk.kek, maxkeylen / 8,
        				    (const u8 *) (key + 1), gd->gtk)) {
            #ifdef DEBUG_PRINT                       
        	wpa_printf(MSG_DEBUG, "WPA: AES unwrap "
                "failed - could not decrypt GTK");
            #endif
        	return -1;
        }
    } else {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Unsupported key_info type %d",
               ver);
         #endif    
        return -1;
    }
    gd->tx = wpa_supplicant_gtk_tx_bit_workaround(
        sm, !!(key_info & WPA_KEY_INFO_TXRX));
    return 0;
}

  int   wpa_supplicant_send_2_of_2(struct wpa_sm *sm,
                      const struct wpa_eapol_key *key,
                      int ver, u16 key_info)
{
    size_t rlen;
    struct wpa_eapol_key *reply;
    u8 *rbuf;

    rbuf = wpa_sm_alloc_eapol(sm, IEEE802_1X_TYPE_EAPOL_KEY, NULL,
                  sizeof(*reply), &rlen, (void *) &reply);
    if (rbuf == NULL)
        return -1;

    sm->txcb_flags |= WPA_GROUP_HANDSHAKE_BIT;
    wpa_printf(MSG_DEBUG, "2/2 txcb_flags=%d\n", sm->txcb_flags);

    reply->type = sm->proto == WPA_PROTO_RSN ?
        EAPOL_KEY_TYPE_RSN : EAPOL_KEY_TYPE_WPA;
    key_info &= WPA_KEY_INFO_KEY_INDEX_MASK;
    key_info |= ver | WPA_KEY_INFO_MIC | WPA_KEY_INFO_SECURE;
    WPA_PUT_BE16(reply->key_info, key_info);
    if (sm->proto == WPA_PROTO_RSN)
        WPA_PUT_BE16(reply->key_length, 0);
    else
        memcpy(reply->key_length, key->key_length, 2);
    memcpy(reply->replay_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);

    WPA_PUT_BE16(reply->key_data_length, 0);
    
    wpa_printf(MSG_DEBUG, "WPA Send 2/2 Group key\n");

    wpa_eapol_key_send(sm, sm->ptk.kck, ver, sm->bssid, ETH_P_EAPOL,
               rbuf, rlen, reply->key_mic);
    wpa_sm_free_eapol(rbuf);

    return 0;
}

  void   wpa_supplicant_process_1_of_2(struct wpa_sm *sm,
                      const unsigned char *src_addr,
                      struct wpa_eapol_key *key,
                      int extra_len, u16 ver)
{
    u16 key_info, keydatalen;
    int  ret;
    struct wpa_gtk_data *gd=&(sm->gd);

    memset(gd, 0, sizeof(struct wpa_gtk_data));

    wpa_printf(MSG_DEBUG, "WPA 1/2 Group Key Handshake\n");
    
    key_info = WPA_GET_BE16(key->key_info);
    keydatalen = WPA_GET_BE16(key->key_data_length);

    if (sm->proto == WPA_PROTO_RSN) {
        ret = wpa_supplicant_process_1_of_2_rsn(sm,
                            (const u8 *) (key + 1),
                            keydatalen, key_info,
                            gd);
    } else {
        ret = wpa_supplicant_process_1_of_2_wpa(sm, key, keydatalen,
                            key_info, extra_len,
                            ver, gd);
    }

    wpa_sm_set_state(WPA_GROUP_HANDSHAKE);

    if (ret)
        goto failed;

    /*before callback, set seq for add param difficult in callback*/ 
    wpa_sm_set_seq(sm, key, 0);
    sm->key_info=key_info;

    if (wpa_supplicant_send_2_of_2(sm, key, ver, key_info))
        goto failed;

    return;

failed:
    wpa_sm_deauthenticate(sm, WLAN_REASON_UNSPECIFIED);
}

  int   wpa_supplicant_send_2_of_2_txcallback(struct wpa_sm *sm)
{
    u16 key_info=sm->key_info;
    u16 rekey= (WPA_SM_STATE(sm) == WPA_COMPLETED);

    if((sm->gd).gtk_len) {
    	if (wpa_supplicant_gtk_in_use(sm, &(sm->gd)) == false) {
            if (wpa_supplicant_install_gtk(sm, &(sm->gd)))
                goto failed;
    	}
    } else {
        goto failed;
    }

    if (rekey) {
#ifdef MSG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Group rekeying "
            "completed with " MACSTR " [GTK=%s]",
            MAC2STR(sm->bssid), wpa_cipher_txt(sm->group_cipher));
#endif    
        wpa_sm_cancel_auth_timeout(sm);
        wpa_sm_set_state(WPA_COMPLETED);
    } else 
        wpa_supplicant_key_neg_complete(sm, sm->bssid,
                        key_info &WPA_KEY_INFO_SECURE);
    return 0;

failed:
    return WLAN_REASON_UNSPECIFIED;
}

   int   wpa_supplicant_verify_eapol_key_mic(struct wpa_sm *sm,
                           struct wpa_eapol_key *key,
                           u16 ver,
                           const u8 *buf, size_t len)
{
    u8 mic[16];
    int ok = 0;

    memcpy(mic, key->key_mic, 16);
    if (sm->tptk_set) {
        memset(key->key_mic, 0, 16);
        wpa_eapol_key_mic(sm->tptk.kck, ver, buf, len,
                  key->key_mic);
        if (memcmp(mic, key->key_mic, 16) != 0) {
           #ifdef DEBUG_PRINT        
            wpa_printf(MSG_DEBUG, "WPA: Invalid EAPOL-Key MIC "
                   "when using TPTK - ignoring TPTK");
           #endif
        } else {
            ok = 1;
            sm->tptk_set = 0;
            sm->ptk_set = 1;
            memcpy(&sm->ptk, &sm->tptk, sizeof(sm->ptk));
        }
    }

    if (!ok && sm->ptk_set) {
        memset(key->key_mic, 0, 16);
        wpa_eapol_key_mic(sm->ptk.kck, ver, buf, len,
                  key->key_mic);
        if (memcmp(mic, key->key_mic, 16) != 0) {
            #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: Invalid EAPOL-Key MIC "
                   "- dropping packet");
             #endif    
            return -1;
        }
        ok = 1;
    }

    if (!ok) {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Could not verify EAPOL-Key MIC "
               "- dropping packet");
         #endif    
        return -1;
    }

    memcpy(sm->rx_replay_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);
    sm->rx_replay_counter_set = 1;
    /*update request_counter for mic failure report*/
    memcpy(sm->request_counter, key->replay_counter,
          WPA_REPLAY_COUNTER_LEN);    
    return 0;
}


/* Decrypt RSN EAPOL-Key key data (RC4 or AES-WRAP) */
  int   wpa_supplicant_decrypt_key_data(struct wpa_sm *sm,
                       struct wpa_eapol_key *key, u16 ver)
{
    u16 keydatalen = WPA_GET_BE16(key->key_data_length);

    wpa_hexdump(MSG_DEBUG, "RSN: encrypted key data",
            (u8 *) (key + 1), keydatalen);
    if (!sm->ptk_set) {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: PTK not available, "
               "cannot decrypt EAPOL-Key key data.");
        #endif    
        return -1;
    }

    /* Decrypt key data here so that this operation does not need
     * to be implemented separately for each message type. */
    if (ver == WPA_KEY_INFO_TYPE_HMAC_MD5_RC4) {
        u8 ek[32];
        memcpy(ek, key->key_iv, 16);
        memcpy(ek + 16, sm->ptk.kek, 16);
        if (rc4_skip(ek, 32, 256, (u8 *) (key + 1), keydatalen)) {
            #ifdef DEBUG_PRINT
            wpa_printf(MSG_DEBUG, "WPA: RC4 failed");
            #endif
            return -1;
        }
    } else if (ver == WPA_KEY_INFO_TYPE_HMAC_SHA1_AES ||
           ver == WPA_KEY_INFO_TYPE_AES_128_CMAC) {
        u8 *buf;
        if (keydatalen % 8) {
            #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: Unsupported "
                   "AES-WRAP len %d", keydatalen);
            #endif    
            return -1;
        }
        keydatalen -= 8; /* AES-WRAP adds 8 bytes */
        
        /*replaced by xxx to remove malloc*/
        buf = ((u8 *) (key+1))+ 8;
             /*
        buf = os_wifi_malloc(keydatalen);
        if (buf == NULL) {
            #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: No memory for "
                   "AES-UNWRAP buffer");
             #endif    
            return -1;
        }
        */
        if (aes_unwrap(sm->ptk.kek, keydatalen / 8,
        				    (u8 *) (key + 1), buf)) {
            #ifdef DEBUG_PRINT    
        	wpa_printf(MSG_DEBUG, "WPA: AES unwrap failed - "
        			"could not decrypt EAPOL-Key key data");
            #endif    
        	return -1;
        }
        memcpy(key + 1, buf, keydatalen);
        WPA_PUT_BE16(key->key_data_length, keydatalen);
    } else {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Unsupported key_info type %d",
               ver);
         #endif    
        return -1;
    }
    wpa_hexdump(MSG_DEBUG, "WPA: decrypted EAPOL-Key key data",
            (u8 *) (key + 1), keydatalen);
    return 0;
}


  void   wpa_eapol_key_dump(int level, const struct wpa_eapol_key *key)
{
#ifdef DEBUG_PRINT
    if (level < MSG_MSGDUMP)
        return;

    u16 key_info = WPA_GET_BE16(key->key_info);

    wpa_printf(MSG_DEBUG, "  EAPOL-Key type=%d\n", key->type);
    wpa_printf(MSG_DEBUG, "  key_info 0x%x (ver=%d keyidx=%d rsvd=%d %s"
           "%s%s%s%s%s%s%s)\n",
           key_info, (u32)(key_info & WPA_KEY_INFO_TYPE_MASK),
           (u32)((key_info & WPA_KEY_INFO_KEY_INDEX_MASK) >>
           WPA_KEY_INFO_KEY_INDEX_SHIFT),
           (u32)((key_info & (BIT(13) | BIT(14) | BIT(15))) >> 13),
           key_info & WPA_KEY_INFO_KEY_TYPE ? "Pairwise" : "Group",
           key_info & WPA_KEY_INFO_INSTALL ? " Install" : "",
           key_info & WPA_KEY_INFO_ACK ? " Ack" : "",
           key_info & WPA_KEY_INFO_MIC ? " MIC" : "",
           key_info & WPA_KEY_INFO_SECURE ? " Secure" : "",
           key_info & WPA_KEY_INFO_ERROR ? " Error" : "",
           key_info & WPA_KEY_INFO_REQUEST ? " Request" : "",
           key_info & WPA_KEY_INFO_ENCR_KEY_DATA ? " Encr" : "");
    wpa_printf(MSG_DEBUG, "  key_length=%u key_data_length=%u\n",
           WPA_GET_BE16(key->key_length),
           WPA_GET_BE16(key->key_data_length));
#endif    
}

/**
 * wpa_sm_rx_eapol - Process received WPA EAPOL frames
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @src_addr: Source MAC address of the EAPOL packet
 * @buf: Pointer to the beginning of the EAPOL data (EAPOL header)
 * @len: Length of the EAPOL frame
 * Returns: 1 = WPA EAPOL-Key processed, 0 = not a WPA EAPOL-Key, -1 failure
 *
 * This function is called for each received EAPOL frame. Other than EAPOL-Key
 * frames can be skipped if filtering is done elsewhere. wpa_sm_rx_eapol() is
 * only processing WPA and WPA2 EAPOL-Key frames.
 *
 * The received EAPOL-Key packets are validated and valid packets are replied
 * to. In addition, key material (PTK, GTK) is configured at the end of a
 * successful key handshake.
 * buf begin from version, so remove mac header ,snap header and ether_type 
 */
int   wpa_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len)
{
    struct wpa_sm *sm = &gWpaSm;
    u32 plen, data_len, extra_len;
    struct ieee802_1x_hdr *hdr;
    struct wpa_eapol_key *key;
    u16 key_info, ver;
    u8 *tmp;
    int ret = -1;

    if (len < sizeof(*hdr) + sizeof(*key)) {       
#ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame too short to be a WPA "
               "EAPOL-Key (len %lu, expecting at least %lu)",
               (unsigned long) len,
               (unsigned long) sizeof(*hdr) + sizeof(*key));
#endif
        return 0;
    }
    
       tmp = buf;
    
    hdr = (struct ieee802_1x_hdr *) tmp;
    key = (struct wpa_eapol_key *) (hdr + 1);
    plen = be_to_host16(hdr->length);
    data_len = plen + sizeof(*hdr);

#ifdef DEBUG_PRINT    
    wpa_printf(MSG_DEBUG, "IEEE 802.1X RX: version=%d type=%d length=%d\n",
           hdr->version, hdr->type, plen);
#endif
    
    if (hdr->version < EAPOL_VERSION) {
        /* TODO: backwards compatibility */
    }
    if (hdr->type != IEEE802_1X_TYPE_EAPOL_KEY) {
#ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame (type %u) discarded, "
            "not a Key frame", hdr->type);
#endif
        ret = 0;
        goto out;
    }
    if (plen > len - sizeof(*hdr) || plen < sizeof(*key)) {
#ifdef DEBUG_PRINT        
        wpa_printf(MSG_DEBUG, "WPA: EAPOL frame payload size %lu "
               "invalid (frame size %lu)",
               (unsigned long) plen, (unsigned long) len);
#endif    
        ret = 0;
        goto out;
    }

    if (key->type != EAPOL_KEY_TYPE_WPA && key->type != EAPOL_KEY_TYPE_RSN)
    {
#ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key type (%d) unknown, "
               "discarded", key->type);
#endif
        ret = 0;
        goto out;
    }

    wpa_eapol_key_dump(MSG_MSGDUMP, key);

    wpa_hexdump(MSG_MSGDUMP, "WPA: RX EAPOL-Key", tmp, len);
    
    if (data_len < len) {
#ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: ignoring %lu bytes after the IEEE "
               "802.1X data\n", (unsigned long) len - data_len);
#endif    
    }
    key_info = WPA_GET_BE16(key->key_info);
    ver = key_info & WPA_KEY_INFO_TYPE_MASK;
    if (ver != WPA_KEY_INFO_TYPE_HMAC_MD5_RC4 &&
        ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
#ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: Unsupported EAPOL-Key descriptor "
               "version %d.", ver);
#endif        
        goto out;
    }

    if (sm->pairwise_cipher == WPA_CIPHER_CCMP &&
        ver != WPA_KEY_INFO_TYPE_HMAC_SHA1_AES) {
#ifdef DEBUG_PRINT    
           wpa_printf(MSG_DEBUG, "WPA: CCMP is used, but EAPOL-Key "
               "descriptor version (%d) is not 2.", ver);
#endif    
        if (sm->group_cipher != WPA_CIPHER_CCMP &&
            !(key_info & WPA_KEY_INFO_KEY_TYPE)) {
            /* Earlier versions of IEEE 802.11i did not explicitly
             * require version 2 descriptor for all EAPOL-Key
             * packets, so allow group keys to use version 1 if
             * CCMP is not used for them. */
            #ifdef DEBUG_PRINT         
              wpa_printf(MSG_DEBUG, "WPA: Backwards compatibility: "
                   "allow invalid version for non-CCMP group "
                   "keys");
            #endif    
        } else
            goto out;
    }


    if ( sm->rx_replay_counter_set &&
    	memcmp(key->replay_counter, sm->rx_replay_counter,
              WPA_REPLAY_COUNTER_LEN) <= 0) {
        #ifdef DEBUG_PRINT              
        wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key Replay Counter did not"
               " increase - dropping packet");
        #endif    
        goto out;
    }

    if (!(key_info & (WPA_KEY_INFO_ACK | WPA_KEY_INFO_SMK_MESSAGE))) {
        #ifdef DEBUG_PRINT    
        wpa_printf(MSG_DEBUG, "WPA: No Ack bit in key_info");
         #endif    
        goto out;
    }

    if (key_info & WPA_KEY_INFO_REQUEST) {
        #ifdef DEBUG_PRINT        
        wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key with Request bit - dropped");
        #endif    
        goto out;
    }

    if ((key_info & WPA_KEY_INFO_MIC) && 
        wpa_supplicant_verify_eapol_key_mic(sm, key, ver, tmp, data_len)) 
        goto out;
    
    extra_len = data_len - sizeof(*hdr) - sizeof(*key);

    if (WPA_GET_BE16(key->key_data_length) > extra_len) {
            #ifdef DEBUG_PRINT        
        wpa_printf(MSG_DEBUG, "WPA: Invalid EAPOL-Key "
            "frame - key_data overflow (%d > %lu)",
            WPA_GET_BE16(key->key_data_length),
            (unsigned long) extra_len);
        #endif        
        goto out;
    }
    extra_len = WPA_GET_BE16(key->key_data_length);

    if (sm->proto == WPA_PROTO_RSN &&
        (key_info & WPA_KEY_INFO_ENCR_KEY_DATA)) {
        if (wpa_supplicant_decrypt_key_data(sm, key, ver))
            goto out;
        extra_len = WPA_GET_BE16(key->key_data_length);
    }

    if (key_info & WPA_KEY_INFO_KEY_TYPE) {
        if (key_info & WPA_KEY_INFO_KEY_INDEX_MASK) {
             #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: Ignored EAPOL-Key "
                   "(Pairwise) with non-zero key index");
          #endif  
            goto out;
        }

        if (key_info & WPA_KEY_INFO_MIC) {
            /* 3/4 4-Way Handshake */
            wpa_supplicant_process_3_of_4(sm, key, ver);
        } else {
            /* 1/4 4-Way Handshake */
            wpa_supplicant_process_1_of_4(sm, src_addr, key,
                              ver);
        }
    } else {
        if (key_info & WPA_KEY_INFO_MIC) {
            /* 1/2 Group Key Handshake */
            wpa_supplicant_process_1_of_2(sm, src_addr, key,
                              extra_len, ver);
        } else {
         #ifdef DEBUG_PRINT    
            wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key (Group) "
                   "without Mic bit - dropped");
         #endif    
        }
    }

    ret = 1;

out:

    return ret;
}

/**
 * wpa_supplicant_set_state - Set current connection state
 * @wpa_s: Pointer to wpa_supplicant data
 * @state: The new connection state
 *
 * This function is called whenever the connection state changes, e.g.,
 * association is completed for WPA/WPA2 4-Way Handshake is started.
 */
void wpa_sm_set_state(enum wpa_states state)
{
       struct wpa_sm *sm = &gWpaSm;
    if(WPA_MIC_FAILURE==WPA_SM_STATE(sm))
        ets_timer_disarm(&(sm->cm_timer));
    sm->wpa_state= state;
}

#ifdef ESP_SUPPLICANT
void   wpa_register(char * payload, WPA_SEND_FUNC snd_func,
                   WPA_SET_ASSOC_IE set_assoc_ie_func, WPA_INSTALL_KEY ppinstallkey, WPA_GET_KEY ppgetkey, WPA_DEAUTH_FUNC wpa_deauth,
                   WPA_NEG_COMPLETE wpa_neg_complete)
{
    struct wpa_sm *sm = &gWpaSm;

    sm->eapol_version = 0x1;   /* DEFAULT_EAPOL_VERSION */    
    sm->sendto = snd_func;
    sm->config_assoc_ie = set_assoc_ie_func;
    sm->install_ppkey = ppinstallkey;
    sm->get_ppkey = ppgetkey;
    sm->wpa_deauthenticate = wpa_deauth;
    sm->wpa_neg_complete = wpa_neg_complete;
    sm->key_entry_valid = 0;
    sm->key_install = false;
    wpa_sm_set_state(WPA_INACTIVE);
}

void wpa_set_profile(u32 wpa_proto, u8 auth_mode)
{
    struct wpa_sm *sm = &gWpaSm;

    sm->proto = wpa_proto;
    if (auth_mode == WPA2_AUTH_ENT)
        sm->key_mgmt = WPA_KEY_MGMT_IEEE8021X; /* for wpa2 enterprise */
    else
        sm->key_mgmt = WPA_KEY_MGMT_PSK;  /* fixed to PSK for now */
}

void wpa_set_pmk(uint8_t *pmk)
{
    struct wpa_sm *sm = &gWpaSm;
    
    memcpy(sm->pmk, pmk, PMK_LEN);
    sm->pmk_len = PMK_LEN;
}

void  
wpa_set_bss(char *macddr, char * bssid, u8 pairwise_cipher, u8 group_cipher, char *passphrase, u8 *ssid, size_t ssid_len)
{
    struct wpa_sm *sm = &gWpaSm;
    
    sm->pairwise_cipher = BIT(pairwise_cipher);
    sm->group_cipher = BIT(group_cipher);
    sm->rx_replay_counter_set = 0;  //init state not intall replay counter value
    memset(sm->rx_replay_counter, 0, WPA_REPLAY_COUNTER_LEN);
    sm->wpa_ptk_rekey = 0;
    sm->renew_snonce = 1;
    memcpy(sm->own_addr, macddr, ETH_ALEN);
    memcpy(sm->bssid, bssid, ETH_ALEN);
    sm->ap_notify_completed_rsne = esp_wifi_sta_is_ap_notify_completed_rsne_internal();
    
    set_assoc_ie(assoc_ie_buf); /* use static buffer */
    wpa_gen_wpa_ie(sm, sm->assoc_wpa_ie, sm->assoc_wpa_ie_len); //TODO: NEED TO DEBUG!!
    wpa_set_passphrase(passphrase, ssid, ssid_len);
}

/*
 *  Call after set ssid since we calc pmk inside this routine directly
 */
  void  
wpa_set_passphrase(char * passphrase, u8 *ssid, size_t ssid_len)
{
    struct wifi_ssid *sta_ssid = esp_wifi_sta_get_prof_ssid_internal();
    struct wpa_sm *sm = &gWpaSm;
   
    if (passphrase == NULL) return;
    
    /*
     *  Here only handle passphrase string.  Need extra step to handle 32B, 64Hex raw
     *    PMK.
     */

    /* This is really SLOW, so just re cacl while reset param */
    if (esp_wifi_sta_get_reset_param_internal() != 0) {
        // check it's psk
        if (strlen((char *)esp_wifi_sta_get_prof_password_internal()) == 64) {
            hexstr2bin((char *)esp_wifi_sta_get_prof_password_internal(), esp_wifi_sta_get_ap_info_prof_pmk_internal(), PMK_LEN);
        } else {
        pbkdf2_sha1((char *)esp_wifi_sta_get_prof_password_internal(), (char *)sta_ssid->ssid, (size_t)sta_ssid->len,
            4096, esp_wifi_sta_get_ap_info_prof_pmk_internal(), PMK_LEN);
        }
        esp_wifi_sta_update_ap_info_internal();
        esp_wifi_sta_set_reset_param_internal(0);
    }

    if (sm->key_mgmt == WPA_KEY_MGMT_IEEE8021X) {
    /* TODO nothing */
    } else {
        memcpy(sm->pmk, esp_wifi_sta_get_ap_info_prof_pmk_internal(), PMK_LEN);
    }
    sm->pmk_len = PMK_LEN;
}

  void  
set_assoc_ie(u8 * assoc_buf)
{
    struct wpa_sm *sm = &gWpaSm;
    
    sm->assoc_wpa_ie = assoc_buf + 2;
    //wpa_ie insert OUI 4 byte before ver, but RSN have 2 bytes of RSN capability,
    // so wpa_ie have two more bytes than rsn_ie
    if ( sm->proto == WPA_PROTO_WPA) 
         sm->assoc_wpa_ie_len = ASSOC_IE_LEN;
    else 
         sm->assoc_wpa_ie_len = ASSOC_IE_LEN - 2;    
    
    sm->config_assoc_ie(sm->proto, assoc_buf, sm->assoc_wpa_ie_len);    
}

  int  
wpa_sm_set_key(struct install_key *key_sm, enum wpa_alg alg,
        u8 *addr, int key_idx, int set_tx,
        u8 *seq, size_t seq_len,
        u8 *key, size_t key_len,
        int  key_entry_valid)
{
    struct wpa_sm *sm = &gWpaSm;

    /*gtk or ptk both need check countermeasures*/
    if (alg == WPA_ALG_TKIP && key_len == 32) {
        /* Clear the MIC error counter when setting a new PTK. */
        key_sm->mic_errors_seen = 0;
    }

    key_sm->keys_cleared = 0;
    key_sm->alg = alg;
    memcpy(key_sm->addr, addr, ETH_ALEN);
    key_sm->key_idx = key_idx;
    key_sm->set_tx = set_tx; 
    memcpy(key_sm->key, key, key_len);
    
    sm->install_ppkey(alg, addr, key_idx, set_tx, seq, seq_len, key, key_len, key_entry_valid);
    return 0;
}

  int  
wpa_sm_get_key(uint8_t *ifx, int *alg, u8 *addr, int *key_idx, u8 *key, size_t key_len, int key_entry_valid)
{
    struct wpa_sm *sm = &gWpaSm;
    return sm->get_ppkey(ifx, alg, addr, key_idx, key, key_len, key_entry_valid);
}

void wpa_supplicant_clr_countermeasures(u16 *pisunicast)
{
       struct wpa_sm *sm = &gWpaSm;
       (sm->install_ptk).mic_errors_seen=0;
    (sm->install_gtk).mic_errors_seen=0;
    ets_timer_done(&(sm->cm_timer));
    wpa_printf(MSG_DEBUG, "WPA: TKIP countermeasures clean\n");
}

/*recovery from countermeasures state, countermeasures state is period that stop connection with ap
  also used in wpa_init after connecting with ap 
*/
void wpa_supplicant_stop_countermeasures(u16 *pisunicast)
{
       struct wpa_sm *sm = &gWpaSm;

       ets_timer_done(&(sm->cm_timer));
    if (sm->countermeasures) {
        sm->countermeasures = 0;
              wpa_supplicant_clr_countermeasures(NULL);

        wpa_printf(MSG_DEBUG, "WPA: TKIP countermeasures stopped\n");
              /*renew scan preocess, this isn't done now*/
    }
       wpa_sm_set_state(WPA_DISCONNECTED);
}

int wpa_michael_mic_failure(u16 isunicast)
{
       struct wpa_sm *sm = &gWpaSm;
       int32_t *pmic_errors_seen=(isunicast)? &((sm->install_ptk).mic_errors_seen) : &((sm->install_gtk).mic_errors_seen);
    
    wpa_printf(MSG_DEBUG, "\nTKIP MIC failure occur\n");
    
       /*both unicast and multicast mic_errors_seen need statistics*/
    if ((sm->install_ptk).mic_errors_seen + (sm->install_gtk).mic_errors_seen) {
        /* Send the new MIC error report immediately since we are going
         * to start countermeasures and AP better do the same.
         */
        wpa_sm_set_state(WPA_TKIP_COUNTERMEASURES);
        wpa_sm_key_request(sm, 1, 0);

        /* initialize countermeasures */
        sm->countermeasures = 1;
        wpa_printf(MSG_DEBUG, "TKIP countermeasures started\n");

        /*
         * Need to wait for completion of request frame. We do not get
         * any callback for the message completion, so just wait a
         * short while and hope for the best. */
         ets_delay_us(10000);

        /*deauthenticate AP*/ 
        
        /*stop monitor next mic_failure timer,disconnect for 60sec, then stop contermeasures*/ 
        ets_timer_disarm(&(sm->cm_timer));
        ets_timer_done(&(sm->cm_timer));
        ets_timer_setfn(&(sm->cm_timer), (ETSTimerFunc *)wpa_supplicant_stop_countermeasures, NULL);
        ets_timer_arm(&(sm->cm_timer), 60*1000, false);
        
        /* TODO: mark the AP rejected for 60 second. STA is
         * allowed to associate with another AP.. */
    } else {
        *pmic_errors_seen=(*pmic_errors_seen)+1;
        wpa_sm_set_state(WPA_MIC_FAILURE);
        wpa_sm_key_request(sm, 1, 0);
        /*start 60sec counter to monitor whether next mic_failure occur in this period, or clear mic_errors_seen*/
        ets_timer_disarm(&(sm->cm_timer));
        ets_timer_done(&(sm->cm_timer));
        ets_timer_setfn(&(sm->cm_timer), (ETSTimerFunc *)wpa_supplicant_clr_countermeasures, NULL);
        ets_timer_arm(&(sm->cm_timer), 60*1000, false);
    }

    return 0;
}

/*
   eapol tx callback function to make sure new key 
    install after 4-way handoff
*/
void eapol_txcb(void *eb)
{    
    struct wpa_sm *sm = &gWpaSm;
    u8 isdeauth = 0;  //no_zero value is the reason for deauth

    if (false == esp_wifi_sta_is_running_internal()){
        return;
    }

    switch(WPA_SM_STATE(sm)) {      
        case WPA_FIRST_HALF_4WAY_HANDSHAKE: 
            break;
        case WPA_LAST_HALF_4WAY_HANDSHAKE:
            if (sm->txcb_flags & WPA_4_4_HANDSHAKE_BIT) {
                sm->txcb_flags &= ~WPA_4_4_HANDSHAKE_BIT;
                isdeauth = wpa_supplicant_send_4_of_4_txcallback(sm);
            } else {
                wpa_printf(MSG_DEBUG, "4/4 txcb, flags=%d\n", sm->txcb_flags);
            }
            break;
        case WPA_GROUP_HANDSHAKE:
            if (sm->txcb_flags & WPA_GROUP_HANDSHAKE_BIT) {
                sm->txcb_flags &= ~WPA_GROUP_HANDSHAKE_BIT;
                isdeauth = wpa_supplicant_send_2_of_2_txcallback(sm);
            } else {
                wpa_printf(MSG_DEBUG, "2/2 txcb, flags=%d\n", sm->txcb_flags);
            }
            break;
        case WPA_TKIP_COUNTERMEASURES: isdeauth=WLAN_REASON_MICHAEL_MIC_FAILURE;
            break;           
        default: break;    
    }

    if(isdeauth) {
        wpa_sm_deauthenticate(sm, isdeauth);    
    }
}

bool wpa_sta_in_4way_handshake(void)
{
    struct wpa_sm *sm = &gWpaSm;
    if ( WPA_SM_STATE(sm) == WPA_MIC_FAILURE || WPA_SM_STATE(sm) == WPA_FIRST_HALF_4WAY_HANDSHAKE
        || WPA_SM_STATE(sm) == WPA_LAST_HALF_4WAY_HANDSHAKE) {
    	return true;
    }
    return false;
}

#endif // ESP_SUPPLICANT

