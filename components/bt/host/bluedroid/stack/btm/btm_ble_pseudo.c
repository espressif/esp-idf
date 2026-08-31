/******************************************************************************
 *
 *  Copyright (C) 2026 Espressif Systems (Shanghai) CO LTD
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#include <string.h>
#include "common/bt_target.h"
#include "common/bt_trace.h"
#include "stack/bt_types.h"
#include "btm_int.h"
#include "btm_ble_pseudo.h"
#include "osi/mutex.h"

#if (BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE && BLE_PERIPH_PSEUDO_ADDR_BOND == TRUE)

/* The pseudo is derived with AES-CMAC, the same SMP crypto primitive used by
 * c1/f5/f6. aes_cipher_msg_auth_code() is provided by stack/smp/smp_cmac.c for
 * ALL three configurable crypto backends (mbedtls/PSA, tinycrypt, stack-native,
 * selected by SMP_CRYPTO_MBEDTLS / SMP_CRYPTO_TINYCRYPT / SMP_CRYPTO_STACK_NATIVE),
 * so the derivation automatically follows the configured crypto library and
 * this module carries no library-specific code. */
extern BOOLEAN aes_cipher_msg_auth_code(BT_OCTET16 key, UINT8 *input, UINT16 length,
                                        UINT16 tlen, UINT8 *p_signature);

/* Fixed 16-byte domain-separation key for the pseudo CMAC (not secret; the
 * pseudo is a Host-internal index, never sent on air). Do not change after
 * deployment without bumping BLE_PSEUDO_SCHEME_VER and migrating bonds. */
static const UINT8 btm_ble_pseudo_cmac_key[16] = {
    'E', 'S', 'P', '_', 'B', 'L', 'E', '_', 'P', 'S', 'E', 'U', 'D', 'O', 'v', BLE_PSEUDO_SCHEME_VER
};

/* The side table holds one entry per concurrent LE link. BTU updates it from
 * HCI/SMP paths; BTC and the public esp_ble_gap_* API read it. All accessors
 * take btm_ble_pseudo_mutex and copy data out before returning. */
#define BTM_BLE_PSEUDO_MAX_CONN     MAX_ACL_CONNECTIONS

static osi_mutex_t btm_ble_pseudo_mutex;
static tBTM_BLE_CONN_IDENTITY btm_ble_conn_id_tab[BTM_BLE_PSEUDO_MAX_CONN];

static tBTM_BLE_CONN_IDENTITY *conn_identity_by_handle_locked(UINT16 handle)
{
    for (int i = 0; i < BTM_BLE_PSEUDO_MAX_CONN; i++) {
        if (btm_ble_conn_id_tab[i].in_use && btm_ble_conn_id_tab[i].handle == handle) {
            return &btm_ble_conn_id_tab[i];
        }
    }
    return NULL;
}

static tBTM_BLE_CONN_IDENTITY *conn_identity_by_pseudo_locked(const BD_ADDR pseudo)
{
    for (int i = 0; i < BTM_BLE_PSEUDO_MAX_CONN; i++) {
        if (btm_ble_conn_id_tab[i].in_use &&
            memcmp(btm_ble_conn_id_tab[i].pseudo, pseudo, BD_ADDR_LEN) == 0) {
            return &btm_ble_conn_id_tab[i];
        }
    }
    return NULL;
}

/*******************************************************************************
** Function         btm_ble_identity_to_pseudo
*******************************************************************************/
void btm_ble_identity_to_pseudo(const tBLE_CONN_IDENTITY *p_id, BD_ADDR pseudo)
{
    uint8_t  in[1 + BD_ADDR_LEN + BD_ADDR_LEN];
    uint8_t  cmac[16];
    BT_OCTET16 key;
    BOOLEAN  hashed;

    if (p_id == NULL || pseudo == NULL) {
        return;
    }

    in[0] = BLE_PSEUDO_SCHEME_VER;
    memcpy(&in[1], p_id->local, BD_ADDR_LEN);
    memcpy(&in[1 + BD_ADDR_LEN], p_id->peer, BD_ADDR_LEN);

    /* AES-CMAC(key, version || local || peer); follows the configured SMP
     * crypto backend (mbedtls/tinycrypt/native). */
    memcpy(key, btm_ble_pseudo_cmac_key, sizeof(key));
    hashed = aes_cipher_msg_auth_code(key, in, sizeof(in), sizeof(cmac), cmac);

    if (!hashed) {
        /* Fall back to a deterministic non-crypto mix so we never emit a
         * zero / unstable pseudo; bring-up only, should not happen. */
        for (int i = 0; i < BD_ADDR_LEN; i++) {
            cmac[i] = in[1 + i] ^ in[1 + BD_ADDR_LEN + i] ^ BLE_PSEUDO_SCHEME_VER;
        }
    }

    memcpy(pseudo, cmac, BD_ADDR_LEN);

    /* Force Static-Random format (top two bits = 11). This is functionally
     * required: it keeps the pseudo out of the resolvable-RPA space so that
     * btm_find_dev()/btm_ble_addr_resolvable() can never misresolve it. */
    pseudo[0] |= 0xC0;

    /* Avoid the all-ones broadcast pattern. */
    if (pseudo[0] == 0xFF && pseudo[1] == 0xFF && pseudo[2] == 0xFF &&
        pseudo[3] == 0xFF && pseudo[4] == 0xFF && pseudo[5] == 0xFF) {
        pseudo[0] = 0xC1;
    }

    BLE_PSEUDO_DBG("derive: local(t%u) " BLE_PSEUDO_BDA_FMT " + peer(t%u) " BLE_PSEUDO_BDA_FMT
                   " -> pseudo " BLE_PSEUDO_BDA_FMT " (hashed=%d)",
                   p_id->local_type, BLE_PSEUDO_BDA(p_id->local),
                   p_id->peer_type, BLE_PSEUDO_BDA(p_id->peer),
                   BLE_PSEUDO_BDA(pseudo), hashed);
}

/*******************************************************************************
** Function         btm_ble_pseudo_init / deinit
*******************************************************************************/
void btm_ble_pseudo_init(void)
{
    /* Bluedroid host init runs during stack bring-up where heap exhaustion is
     * not a tolerated / recoverable condition: if this single fixed-size mutex
     * cannot be created the whole host cannot come up, so there is nothing to
     * gracefully fall back to. The return value is intentionally not checked
     * here, matching the rest of the host init path (e.g. btm_ble_init()), and
     * this is not a bug. */
    osi_mutex_new(&btm_ble_pseudo_mutex);
    osi_mutex_lock(&btm_ble_pseudo_mutex, OSI_MUTEX_MAX_TIMEOUT);
    memset(btm_ble_conn_id_tab, 0, sizeof(btm_ble_conn_id_tab));
    osi_mutex_unlock(&btm_ble_pseudo_mutex);
}

void btm_ble_pseudo_deinit(void)
{
    if (btm_ble_pseudo_mutex == NULL) {
        return;
    }
    osi_mutex_lock(&btm_ble_pseudo_mutex, OSI_MUTEX_MAX_TIMEOUT);
    memset(btm_ble_conn_id_tab, 0, sizeof(btm_ble_conn_id_tab));
    osi_mutex_unlock(&btm_ble_pseudo_mutex);
    osi_mutex_free(&btm_ble_pseudo_mutex);
}

/*******************************************************************************
** Function         btm_ble_conn_identity_get_by_handle
*******************************************************************************/
BOOLEAN btm_ble_conn_identity_get_by_handle(UINT16 handle, tBTM_BLE_CONN_IDENTITY *p_out)
{
    BOOLEAN found = FALSE;

    if (p_out == NULL) {
        return FALSE;
    }

    osi_mutex_lock(&btm_ble_pseudo_mutex, OSI_MUTEX_MAX_TIMEOUT);
    tBTM_BLE_CONN_IDENTITY *p_ent = conn_identity_by_handle_locked(handle);
    if (p_ent) {
        memcpy(p_out, p_ent, sizeof(tBTM_BLE_CONN_IDENTITY));
        found = TRUE;
    }
    osi_mutex_unlock(&btm_ble_pseudo_mutex);
    return found;
}

/*******************************************************************************
** Function         btm_ble_conn_identity_get_by_pseudo
*******************************************************************************/
BOOLEAN btm_ble_conn_identity_get_by_pseudo(const BD_ADDR pseudo, tBTM_BLE_CONN_IDENTITY *p_out)
{
    BOOLEAN found = FALSE;

    if (pseudo == NULL || p_out == NULL) {
        return FALSE;
    }

    osi_mutex_lock(&btm_ble_pseudo_mutex, OSI_MUTEX_MAX_TIMEOUT);
    tBTM_BLE_CONN_IDENTITY *p_ent = conn_identity_by_pseudo_locked(pseudo);
    if (p_ent) {
        memcpy(p_out, p_ent, sizeof(tBTM_BLE_CONN_IDENTITY));
        found = TRUE;
    }
    osi_mutex_unlock(&btm_ble_pseudo_mutex);
    return found;
}

/*******************************************************************************
** Function         btm_ble_conn_identity_exists_by_handle
*******************************************************************************/
BOOLEAN btm_ble_conn_identity_exists_by_handle(UINT16 handle)
{
    BOOLEAN found = FALSE;

    osi_mutex_lock(&btm_ble_pseudo_mutex, OSI_MUTEX_MAX_TIMEOUT);
    found = (conn_identity_by_handle_locked(handle) != NULL);
    osi_mutex_unlock(&btm_ble_pseudo_mutex);
    return found;
}

/*******************************************************************************
** Function         btm_ble_conn_identity_register
*******************************************************************************/
BOOLEAN btm_ble_conn_identity_register(UINT16 handle,
                                       const tBLE_CONN_IDENTITY *p_id,
                                       const BD_ADDR pseudo,
                                       BOOLEAN local_ready)
{
    BOOLEAN ok = FALSE;

    osi_mutex_lock(&btm_ble_pseudo_mutex, OSI_MUTEX_MAX_TIMEOUT);

    tBTM_BLE_CONN_IDENTITY *p_ent = conn_identity_by_handle_locked(handle);
    if (p_ent == NULL) {
        for (int i = 0; i < BTM_BLE_PSEUDO_MAX_CONN; i++) {
            if (!btm_ble_conn_id_tab[i].in_use) {
                p_ent = &btm_ble_conn_id_tab[i];
                break;
            }
        }
    }

    if (p_ent == NULL) {
        BTM_TRACE_ERROR("%s no free slot for handle 0x%x", __func__, handle);
        BLE_PSEUDO_DBG("register FAIL: no free slot, handle=0x%x", handle);
    } else {
        p_ent->handle = handle;
        p_ent->in_use = TRUE;
        p_ent->local_ready = local_ready;
        if (p_id) {
            memcpy(&p_ent->id, p_id, sizeof(tBLE_CONN_IDENTITY));
        }
        if (pseudo) {
            memcpy(p_ent->pseudo, pseudo, BD_ADDR_LEN);
        }
        BLE_PSEUDO_DBG("register: handle=0x%x slot=%d pseudo=" BLE_PSEUDO_BDA_FMT " local_ready=%d",
                       handle, (int)(p_ent - btm_ble_conn_id_tab),
                       BLE_PSEUDO_BDA(p_ent->pseudo), local_ready);
        ok = TRUE;
    }

    osi_mutex_unlock(&btm_ble_pseudo_mutex);
    return ok;
}

/*******************************************************************************
** Function         btm_ble_conn_identity_unregister
*******************************************************************************/
void btm_ble_conn_identity_unregister(UINT16 handle)
{
    osi_mutex_lock(&btm_ble_pseudo_mutex, OSI_MUTEX_MAX_TIMEOUT);
    tBTM_BLE_CONN_IDENTITY *p_ent = conn_identity_by_handle_locked(handle);
    if (p_ent) {
        BLE_PSEUDO_DBG("unregister: handle=0x%x pseudo=" BLE_PSEUDO_BDA_FMT,
                       handle, BLE_PSEUDO_BDA(p_ent->pseudo));
        memset(p_ent, 0, sizeof(tBTM_BLE_CONN_IDENTITY));
    }
    osi_mutex_unlock(&btm_ble_pseudo_mutex);
}

/*******************************************************************************
** Function         btm_ble_pseudo_to_real_peer
*******************************************************************************/
BOOLEAN btm_ble_pseudo_to_real_peer(const BD_ADDR pseudo, BD_ADDR real_peer)
{
    BOOLEAN found = FALSE;

    if (pseudo == NULL || real_peer == NULL) {
        return FALSE;
    }

    osi_mutex_lock(&btm_ble_pseudo_mutex, OSI_MUTEX_MAX_TIMEOUT);
    tBTM_BLE_CONN_IDENTITY *p_ent = conn_identity_by_pseudo_locked(pseudo);
    if (p_ent) {
        memcpy(real_peer, p_ent->id.peer, BD_ADDR_LEN);
        found = TRUE;
    }
    osi_mutex_unlock(&btm_ble_pseudo_mutex);
    return found;
}

#endif /* BLE_INCLUDED && SMP_INCLUDED && BLE_PERIPH_PSEUDO_ADDR_BOND */
