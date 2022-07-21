/*
 * EAP server/peer: EAP-NOOB (RFC 9140)
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "eap_peer/eap_i.h"
#include "eap_peer/eap_config.h"
#include "eap_peer/eap_methods.h"
#include "eap_peer/eap_noob.h"
#include "utils/base64.h"

#include "cJSON.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/md.h"

struct eap_noob_state g_wpa_eap_noob_state;

typedef enum {
    EAP_NOOB_ERROR_INVALID_NAI = 1001,
    EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE = 1002,
    EAP_NOOB_ERROR_INVALID_DATA = 1003,
    EAP_NOOB_ERROR_UNEXPECTED_MESSAGE_TYPE = 1004,
    EAP_NOOB_ERROR_INVALID_ECDHE_KEY = 1005,

    EAP_NOOB_ERROR_UNWANTED_PEER = 2001,
    EAP_NOOB_ERROR_STATE_MISMATCH = 2002,
    EAP_NOOB_ERROR_UNRECOGNIZED_OOB_MSG_IDENTIFIER = 2003,
    EAP_NOOB_ERROR_UNEXPECTED_PEER_IDENTIFIER = 2004,

    EAP_NOOB_ERROR_NO_MUTUALLY_SUPPORTED_PROTOCOL_VERSION = 3001,
    EAP_NOOB_ERROR_NO_MUTUALLY_SUPPORTED_CRYPTOSUITE = 3002,
    EAP_NOOB_ERROR_NO_MUTUALLY_SUPPORTED_OOB_DIRECTION = 3003,

    EAP_NOOB_ERROR_HMAC_VERIFICATION_FAILURE = 4001,

    EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR = 5001,
    EAP_NOOB_ERROR_INVALID_SERVER_INFO = 5002,
    EAP_NOOB_ERROR_INVALID_SERVER_URL = 5003,
    EAP_NOOB_ERROR_INVALID_PEER_INFO = 5004,
} eap_noob_error_types_t;

typedef enum {
    EAP_NOOB_KEYINGMODE_COMPLETION = 0,
    EAP_NOOB_KEYINGMODE_RECONNECT_WITHOUT_ECDHE = 1,
    EAP_NOOB_KEYINGMODE_RECONNECT_WITH_ECDHE = 2,
    EAP_NOOB_KEYINGMODE_RECONNECT_CRYPTOSUITE_UPDATE = 3
} eap_noob_keyingmodes_t;

typedef enum {
    EAP_NOOB_STATE_INTERNAL_IDENTITY_SENT,
    EAP_NOOB_STATE_INTERNAL_PEERID_SENT, // Response to Type 1
    EAP_NOOB_STATE_INTERNAL_VERSION_NEGOTIATION_SENT, // Response to Type 2
    EAP_NOOB_STATE_INTERNAL_PUBKEY_SENT, // Response to Type 3
    EAP_NOOB_STATE_INTERNAL_WAITING_SENT, // Response to type 4
    EAP_NOOB_STATE_INTERNAL_NOOBID_DISCOVERY_SENT, // Response to Type 5
    EAP_NOOB_STATE_INTERNAL_MACP_SENT, // Response to Type 6
    EAP_NOOB_STATE_INTERNAL_RECONNECT_VERSION_NEGOTIATION_SENT, // Response to Type 7
    EAP_NOOB_STATE_INTERNAL_RECONNECT_PUBKEY_SENT, // Response to type 8
} eap_noob_internal_state_t;

typedef enum {
    EAP_NOOB_MSG_TYPE_ERROR_NOTIFICATION = 0,
    EAP_NOOB_MSG_TYPE_PEERID_AND_STATE_DISCOVERY = 1,
    EAP_NOOB_MSG_TYPE_INITIAL_VERSION_NEGOTIATION = 2,
    EAP_NOOB_MSG_TYPE_INITIAL_ECDHE_EXCHANGE = 3,
    EAP_NOOB_MSG_TYPE_WAITING = 4,
    EAP_NOOB_MSG_TYPE_COMPLETION_NOOBID_DISCOVERY = 5,
    EAP_NOOB_MSG_TYPE_COMPLETION_AUTHENTICATION = 6,
    EAP_NOOB_MSG_TYPE_RECONNECT_VERSION_NEGOTIATION = 7,
    EAP_NOOB_MSG_TYPE_RECONNECT_ECHDE_EXCHANGE = 8,
    EAP_NOOB_MSG_TYPE_RECONNECT_AUTHENTICATION = 9,
} eap_noob_message_types_t;

struct eap_noob_cryptographic_material {
    u8 kdf_out[320];
    size_t kdf_len;
    u8 macp[32];
    u8 macs[32];
};

struct eap_noob_data {
    eap_noob_internal_state_t internal_state;
    char *nai; // String, 0-byte terminated
    char *peer_id; // String, 0-byte terminated
    u8 ns[32];
    u8 np[32];
    u8 *shared_key;
    size_t shared_key_length;
    eap_noob_oob_msg_t *used_oob_msg;
    struct eap_noob_cryptographic_material *cryptokeys;
    u8 keyingmode;
    bool persistent_state_updated;

    char *vers; // JSON-Array as String, 0-byte terminated
    char *cryptosuites; // JSON-Array as String, 0-byte terminated
    char *dirs; // Number as String, 0-byte terminated
    char *server_info; // JSON-Object as String, 0-byte terminated
    char *verp; // Number as String, 0-byte terminated
    char *cryptosuitep; // Number as String, 0-byte terminated
    char *dirp; // Number as String, 0-byte terminated
    char *peer_info; // JSON-Object as String, 0-byte terminated
    char *pks; // JSON-Object as String, 0-byte terminated
    char *ns_b; // Base64URL encoded string, 0-byte terminated
    char *pkp; // JSON-Object as String, 0-byte terminated
    char *np_b; // Base64URL encoded string, 0-byte terminated
};

/**
 * Cryptographically secure compare two values with static runtime.
 * @param one first array to compare
 * @param two second array to compare
 * @param len length of the arrays
 * @return 0 if the arrays are equal, any other value otherwise.
 */
static u8 cryptographic_secure_compare(u8 *one, u8 *two, size_t len)
{
    u8 mac_check = 0;
    for (size_t i = 0; i < len; i++) {
        mac_check |= one[i] ^ two[i];
    }
    return mac_check;
}

static int eap_noob_calculate_ecdhe_keyexchange(cJSON *pks, cJSON **pkp_out, u8 **shared_key_out, size_t *shared_key_length_out)
{
    cJSON *ret_pkp = cJSON_CreateObject();
    *pkp_out = ret_pkp;

    // Parse the x coordinate of the Server's Public Key
    cJSON *parsed_pks_x = cJSON_GetObjectItemCaseSensitive(pks, "x");
    if (!cJSON_IsString(parsed_pks_x)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: PKs.x was not a string");
        return EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE;
    }

    size_t base64_len;
    u8 *otherpubkey_tmp = base64_url_decode(parsed_pks_x->valuestring, strlen(parsed_pks_x->valuestring), &base64_len);
    if (base64_len != 32) {
        free(otherpubkey_tmp);
        return EAP_NOOB_ERROR_INVALID_ECDHE_KEY;
    }
    u8 otherpubkey[32];
    memcpy(otherpubkey, otherpubkey_tmp, 32);
    free(otherpubkey_tmp);

    // Calculate the X25519 Handshake
    mbedtls_ecdh_context x25519_ctx;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ecdh_init( &x25519_ctx );
    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );

    int ret_val;

    // Calculate the X25519 private and public key
    unsigned char mykey[32];
    const char *mbed_seed = "ecdh";

    mbedtls_ecdh_context_mbed *x25519_ctx_m = &x25519_ctx.MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(mbed_ecdh);
    ret_val = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (unsigned char *)mbed_seed, 4);
    if (ret_val != 0) {
        return EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR;
    }
    ret_val = mbedtls_ecp_group_load( &x25519_ctx_m->MBEDTLS_PRIVATE(grp), MBEDTLS_ECP_DP_CURVE25519);
    if (ret_val != 0) {
        return EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR;
    }
    ret_val = mbedtls_ecdh_gen_public( &x25519_ctx_m->MBEDTLS_PRIVATE(grp), &x25519_ctx_m->MBEDTLS_PRIVATE(d), &x25519_ctx_m->MBEDTLS_PRIVATE(Q), mbedtls_ctr_drbg_random, &ctr_drbg );
    if (ret_val != 0) {
        return EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR;
    }
    ret_val = mbedtls_mpi_write_binary_le( &x25519_ctx_m->MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X), mykey, 32);
    if (ret_val != 0) {
        return EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR;
    }

    // Load public key of the server
    mbedtls_mpi_lset(&x25519_ctx_m->MBEDTLS_PRIVATE(Qp).MBEDTLS_PRIVATE(Z), 1);
    ret_val = mbedtls_mpi_read_binary_le(&x25519_ctx_m->MBEDTLS_PRIVATE(Qp).MBEDTLS_PRIVATE(X), otherpubkey, 32);
    if (ret_val != 0) {
        return EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR;
    }

    // Compute shared secret
    unsigned char sharedkey[32];
    ret_val = mbedtls_ecdh_compute_shared(&x25519_ctx_m->MBEDTLS_PRIVATE(grp), &x25519_ctx_m->MBEDTLS_PRIVATE(z), &x25519_ctx_m->MBEDTLS_PRIVATE(Qp), &x25519_ctx_m->MBEDTLS_PRIVATE(d), mbedtls_ctr_drbg_random, &ctr_drbg);
    if (ret_val != 0) {
        return EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR;
    }
    ret_val = mbedtls_mpi_write_binary_le( &x25519_ctx_m->MBEDTLS_PRIVATE(z), sharedkey, 32);
    if (ret_val != 0) {
        return EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR;
    }
    *shared_key_out = os_zalloc(32);
    memcpy(*shared_key_out, sharedkey, 32);
    *shared_key_length_out = 32;

    // Generate cJSON output for peer's public key
    size_t mykey_b64_len;
    char *mykey_b64 = base64_url_encode(mykey, 32, &mykey_b64_len);
    cJSON_AddItemToObject(ret_pkp, "kty", cJSON_CreateString("OKP"));
    cJSON_AddItemToObject(ret_pkp, "crv", cJSON_CreateString("X25519"));
    cJSON_AddItemToObject(ret_pkp, "x", cJSON_CreateString(mykey_b64));

    *pkp_out = ret_pkp;
    return 0;
}

static int eap_noob_generate_new_nonce(u8 *out, size_t len)
{
    mbedtls_entropy_context entropy;
    mbedtls_entropy_init( &entropy );
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ctr_drbg_init( &ctr_drbg );

    const char *mbed_seed = "npseed";
    int ret_val = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (unsigned char *)mbed_seed, 6);
    if (ret_val != 0) {
        return EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR;
    }
    ret_val = mbedtls_ctr_drbg_random(&ctr_drbg, out, len);
    if (ret_val != 0) {
        return EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR;
    }
    return 0;
}

static u8 *eap_noob_calculate_hoob(eap_noob_oob_msg_t *oobMsg)
{
    struct eap_noob_ephemeral_state_info *eph = g_wpa_eap_noob_state.ephemeral_state;
    cJSON *json_vals;

    size_t b64len;
    char *noob_b = base64_url_encode(oobMsg->noob, 16, &b64len);

    json_vals = cJSON_CreateNumber(1);
    char *dir = cJSON_PrintUnformatted(json_vals);
    cJSON_Delete(json_vals);
    json_vals = cJSON_CreateString(noob_b);
    char *noob = cJSON_PrintUnformatted(json_vals);
    cJSON_Delete(json_vals); // Also frees noob_b

    size_t hoob_src_len = strlen(eph->hash_base_string) + strlen(dir) + strlen(noob) + 2;
    char *hoob_src = os_zalloc(hoob_src_len + 1);
    snprintf(hoob_src, hoob_src_len + 1, "[%s%s%s]",
             dir,
             eph->hash_base_string,
             noob
            );

    wpa_printf(MSG_INFO, "HOOB Input: %s", hoob_src);
    u8 hash_out[32];
    u8 *hash_ret = os_zalloc(16);
    mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (u8 *)hoob_src, strlen(hoob_src), hash_out);
    memcpy(hash_ret, hash_out, 16);
    free(hoob_src);
    return hash_ret;
}

static void eap_noob_calculate_noobid(eap_noob_oob_msg_t *oobMsg)
{

    size_t b64len;
    char *noob_b = base64_url_encode(oobMsg->noob, 16, &b64len);

    size_t noobid_src_len = strlen(noob_b) + 11;
    char *noobid_src = os_zalloc(noobid_src_len + 1);
    snprintf(noobid_src, noobid_src_len + 1, "[\"NoobId\",%s]", noob_b);
    free(noob_b);
    u8 hash_out[32];

    mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (u8 *)noobid_src, strlen(noobid_src), hash_out);
    memcpy(oobMsg->noob_id, hash_out, 16);
    free(noobid_src);
}

bool eap_noob_receive_oob_msg(eap_noob_oob_msg_t *oobMsg)
{
    if (g_wpa_eap_noob_state.ephemeral_state == NULL) {
        return false;
    }

    struct eap_noob_ephemeral_state_info *eph = g_wpa_eap_noob_state.ephemeral_state;

    u8 *hoob = eap_noob_calculate_hoob(oobMsg);

    // TODO: Should this be a cryptographically secure comparison? Not sure.
    if (memcmp(oobMsg->hoob, hoob, 16) != 0) {
        // HOOB did not match, not including the OOB Message in the structure
        return false;
    }

    eap_noob_calculate_noobid(oobMsg);

    eap_noob_oob_msg_node_t *oobMsgNode = os_zalloc(sizeof(eap_noob_oob_msg_node_t));
    oobMsgNode->value = oobMsg;

    if (eph->oobMessages == NULL) {
        eph->oobMessages = oobMsgNode;
    } else {
        eap_noob_oob_msg_node_t *cur = eph->oobMessages;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = oobMsgNode;
    }

    g_wpa_eap_noob_state.noob_state = EAP_NOOB_STATE_OOB_RECEIVED;
    return true;
}

eap_noob_oob_msg_t *eap_noob_generate_oob_msg(void)
{
    if (g_wpa_eap_noob_state.ephemeral_state == NULL) {
        return NULL;
    }

    eap_noob_oob_msg_node_t *oobMsgNode = os_zalloc(sizeof(eap_noob_oob_msg_node_t));
    if (oobMsgNode == NULL) {
        return NULL;
    }
    struct eap_noob_oob_msg *oobMsg = os_zalloc(sizeof(struct eap_noob_oob_msg));
    if (oobMsg == NULL) {
        return NULL;
    }

    oobMsgNode->value = oobMsg;


    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );
    int ret_val;
    const char *mbed_seed = "EAP-NOOB-Nonce";
    ret_val = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (unsigned char *)mbed_seed, 14);
    if (ret_val != 0) {
        return NULL;
    }
    mbedtls_ctr_drbg_random(&ctr_drbg, oobMsg->noob, 16);

    struct eap_noob_ephemeral_state_info *eph = g_wpa_eap_noob_state.ephemeral_state;

    oobMsg->dir = EAP_NOOB_OOB_DIRECTION_PEER_TO_SERVER;

    u8 *hoob = eap_noob_calculate_hoob(oobMsg);

    memcpy(oobMsg->hoob, hoob, 16);

    eap_noob_calculate_noobid(oobMsg);

    if (eph->oobMessages == NULL) {
        eph->oobMessages = oobMsgNode;
    } else {
        eap_noob_oob_msg_node_t *cur = eph->oobMessages;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = oobMsgNode;
    }

    return oobMsg;
}

static void free_ephemeral_state(void)
{
    if (g_wpa_eap_noob_state.ephemeral_state == NULL) {
        return;
    }

    struct eap_noob_ephemeral_state_info *eph = g_wpa_eap_noob_state.ephemeral_state;

    if (eph->hash_base_string != NULL) {
        os_free(eph->hash_base_string);
    }
    if (eph->shared_secret != NULL) {
        os_free(eph->shared_secret);
    }

    eap_noob_oob_msg_node_t *cur = eph->oobMessages;
    eap_noob_oob_msg_node_t *next = NULL;
    while (cur != NULL) {
        next = cur->next;
        free(cur->value);
        free(cur);
        cur = next;
    }
}

static char *generate_hash_base(struct eap_noob_data *data, int keyingmode)
{
    cJSON *keyingmode_j = cJSON_CreateNumber(keyingmode);
    char *keyingmode_str = cJSON_PrintUnformatted(keyingmode_j);
    cJSON_Delete(keyingmode_j);

    // Add empty string for MAC calculation
    cJSON *emptystr_json = cJSON_CreateString("");
    if (data->dirs == NULL) {
        data->dirs = cJSON_PrintUnformatted(emptystr_json);
    }
    if (data->server_info == NULL) {
        data->server_info = cJSON_PrintUnformatted(emptystr_json);
    }
    if (data->dirp == NULL) {
        data->dirp = cJSON_PrintUnformatted(emptystr_json);
    }
    if (data->peer_info == NULL) {
        data->peer_info = cJSON_PrintUnformatted(emptystr_json);
    }
    if (data->pks == NULL) {
        data->pks = cJSON_PrintUnformatted(emptystr_json);
    }
    if (data->pkp == NULL) {
        data->pkp = cJSON_PrintUnformatted(emptystr_json);
    }
    cJSON_Delete(emptystr_json);

    size_t total_length =
        strlen(data->vers) +
        strlen(data->verp) +
        strlen(data->peer_id) +
        strlen(data->cryptosuites) +
        strlen(data->dirs) +
        strlen(data->server_info) +
        strlen(data->cryptosuitep) +
        strlen(data->dirp) +
        strlen(data->nai) +
        strlen(data->peer_info) +
        strlen(keyingmode_str) +
        strlen(data->pks) +
        strlen(data->ns_b) +
        strlen(data->pkp) +
        strlen(data->np_b) +
        20;



    char *to_return = os_zalloc(total_length + 1);

    snprintf(to_return, total_length + 1,
             ",%s,%s,\"%s\",%s,%s,%s,%s,%s,\"%s\",%s,%s,%s,%s,%s,%s,",
             data->vers,
             data->verp,
             data->peer_id,
             data->cryptosuites,
             data->dirs,
             data->server_info,
             data->cryptosuitep,
             data->dirp,
             data->nai,
             data->peer_info,
             keyingmode_str,
             data->pks,
             data->ns_b,
             data->pkp,
             data->np_b
            );
    return to_return;
}

static void eap_noob_save_ephemeral_state(struct eap_noob_data *data, int keyingmode)
{
    free_ephemeral_state();
    g_wpa_eap_noob_state.ephemeral_state = os_zalloc(sizeof(struct eap_noob_ephemeral_state_info));
    struct eap_noob_ephemeral_state_info *eph = g_wpa_eap_noob_state.ephemeral_state;

    if (data->shared_key != NULL) {
        eph->shared_secret = os_zalloc(data->shared_key_length);
        eph->shared_secret_length = data->shared_key_length;
        memcpy(eph->shared_secret, data->shared_key, eph->shared_secret_length);
    }

    memcpy(eph->np, data->np, 32);
    memcpy(eph->ns, data->ns, 32);

    if (g_wpa_eap_noob_state.peer_id != NULL) {
        os_free(g_wpa_eap_noob_state.peer_id);
    }
    size_t len = strlen(data->peer_id);
    g_wpa_eap_noob_state.peer_id = os_zalloc(len + 1);
    memcpy(g_wpa_eap_noob_state.peer_id, data->peer_id, len + 1);

    eph->hash_base_string = generate_hash_base(data, keyingmode);
    g_wpa_eap_noob_state.noob_state = EAP_NOOB_STATE_WAITING_FOR_OOB;
}

static void eap_noob_save_initial_persistent_state(struct eap_noob_data *data, u8 *kz)
{
    size_t len;

    g_wpa_eap_noob_state.active = true;
    g_wpa_eap_noob_state.persistent = true;

    g_wpa_eap_noob_state.noob_state = EAP_NOOB_STATE_REGISTERED;
    g_wpa_eap_noob_state.prev_active = false;

    if (g_wpa_eap_noob_state.peer_id != NULL) {
        os_free(g_wpa_eap_noob_state.peer_id);
    }
    len = strlen(data->peer_id);
    g_wpa_eap_noob_state.peer_id = os_zalloc(len + 1);
    memcpy(g_wpa_eap_noob_state.peer_id, data->peer_id, len + 1);

    g_wpa_eap_noob_state.version = 1;
    g_wpa_eap_noob_state.cryptosuite = 1;
    memcpy(g_wpa_eap_noob_state.kz, kz, 32);

    free_ephemeral_state();
}
static void eap_noob_flush_persistent_kz_prev(void)
{
    g_wpa_eap_noob_state.prev_active = false;
}
static void eap_noob_rollback_persistent_kz_prev(void)
{
    g_wpa_eap_noob_state.prev_active = false;
    memcpy(g_wpa_eap_noob_state.kz, g_wpa_eap_noob_state.kz_prev, 32);
    g_wpa_eap_noob_state.version = g_wpa_eap_noob_state.version_prev;
    g_wpa_eap_noob_state.cryptosuite = g_wpa_eap_noob_state.cryptosuite_prev;
}
static void eap_noob_update_persistent_state(struct eap_noob_data *data, u8 *kz)
{
    g_wpa_eap_noob_state.prev_active = true;
    memcpy(g_wpa_eap_noob_state.kz_prev, g_wpa_eap_noob_state.kz, 32);
    g_wpa_eap_noob_state.cryptosuite_prev = g_wpa_eap_noob_state.cryptosuite;
    g_wpa_eap_noob_state.version_prev = g_wpa_eap_noob_state.version;

    memcpy(g_wpa_eap_noob_state.kz, kz, 32);
    g_wpa_eap_noob_state.version = 1;
    g_wpa_eap_noob_state.cryptosuite = 1;

}

static void eap_noob_calculate_kdf(u8 *out, size_t len, u8 *z, size_t z_len, u8 *party_u_info, u8 *party_v_info, u8 *supp_priv_info, size_t supp_priv_info_len)
{
    char *alg_id = "EAP-NOOB";
    size_t kdf_len = 4 + z_len + 8 + 64 + supp_priv_info_len;
    u8 *kdf_input = os_zalloc(kdf_len);
    memcpy(kdf_input + 4, z, z_len);
    memcpy(kdf_input + 4 + z_len, alg_id, 8);
    memcpy(kdf_input + 4 + z_len + 8, party_u_info, 32);
    memcpy(kdf_input + 4 + z_len + 8 + 32, party_v_info, 32);
    memcpy(kdf_input + 4 + z_len + 8 + 64, supp_priv_info, supp_priv_info_len);

    size_t cur_ptr = 0;
    u32 id = 1;
    u8 kdf_out[32];
    while (cur_ptr < len) {
        kdf_input[0] = id >> 24;
        kdf_input[1] = id >> 16;
        kdf_input[2] = id >> 8;
        kdf_input[3] = id;
        id++;

        mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), kdf_input, kdf_len, kdf_out);

        if (cur_ptr + 32 <= len) {
            memcpy(out + cur_ptr, kdf_out, 32);
            cur_ptr += 32;
        } else {
            memcpy(out + cur_ptr, kdf_out, len - cur_ptr);
            cur_ptr = len;
        }
    }
}

static struct eap_noob_cryptographic_material *eap_noob_calculate_cryptographic_elements(struct eap_noob_data *data, int keyingmode, u8 *kz)
{
    if (keyingmode != EAP_NOOB_KEYINGMODE_COMPLETION && kz == NULL) {
        return NULL;
    }

    struct eap_noob_cryptographic_material *to_return = os_zalloc(sizeof(struct eap_noob_cryptographic_material));
    if (keyingmode == EAP_NOOB_KEYINGMODE_RECONNECT_WITHOUT_ECDHE ||
            keyingmode == EAP_NOOB_KEYINGMODE_RECONNECT_WITH_ECDHE) {
        to_return->kdf_len = 288;
    } else {
        to_return->kdf_len = 320;
    }
    char *noob_j;
    if (keyingmode == EAP_NOOB_KEYINGMODE_COMPLETION) {
        eap_noob_calculate_kdf(to_return->kdf_out, to_return->kdf_len, data->shared_key, data->shared_key_length,
                               data->np, data->ns, data->used_oob_msg->noob, 16);
        size_t b64;
        char *noob_b = base64_url_encode(data->used_oob_msg->noob, 16, &b64);
        cJSON *noob_json = cJSON_CreateString(noob_b);
        noob_j = cJSON_PrintUnformatted(noob_json);
        cJSON_Delete(noob_json);
    } else {
        cJSON *noob_json = cJSON_CreateString("");
        noob_j = cJSON_PrintUnformatted(noob_json);
        cJSON_Delete(noob_json);
        if (keyingmode == EAP_NOOB_KEYINGMODE_RECONNECT_WITHOUT_ECDHE) {
            eap_noob_calculate_kdf(to_return->kdf_out, to_return->kdf_len, kz, 32, data->np, data->ns, NULL, 0);
        } else {
            eap_noob_calculate_kdf(to_return->kdf_out, to_return->kdf_len, data->shared_key, data->shared_key_length,
                                   data->np, data->ns, kz, 32);
        }
    }

    // MSK: 0-63
    // EMSK: 64-127
    // AMSK: 128-191
    // MethodId: 192-223
    // Kms: 224-255
    // Kmp: 256-287
    // Kz: 288-319 (only Completion/reconnect with version negotiation)

    char *hash_base_string;
    if (keyingmode == EAP_NOOB_KEYINGMODE_COMPLETION) {
        hash_base_string = g_wpa_eap_noob_state.ephemeral_state->hash_base_string;
    } else {
        hash_base_string = generate_hash_base(data, keyingmode);
    }

    size_t km_input_len = strlen(hash_base_string) + 3 + strlen(noob_j);
    char *kms_input = os_zalloc(km_input_len + 1);
    char *kmp_input = os_zalloc(km_input_len + 1);

    snprintf(kms_input, km_input_len + 1, "[2%s%s]", hash_base_string, noob_j);
    snprintf(kmp_input, km_input_len + 1, "[1%s%s]", hash_base_string, noob_j);

    if (keyingmode != EAP_NOOB_KEYINGMODE_COMPLETION) {
        os_free(hash_base_string);
    }


    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), to_return->kdf_out + 224, 32, (u8 *)kms_input, km_input_len, to_return->macs);
    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), to_return->kdf_out + 256, 32, (u8 *)kmp_input, km_input_len, to_return->macp);

    return to_return;
}

static struct wpabuf *build_error_msg(const struct wpabuf *reqData, int error_code)
{
    //<editor-fold desc="Build response JSON structure">
    cJSON *ret_json = cJSON_CreateObject();
    cJSON_AddItemToObject(ret_json, "Type", cJSON_CreateNumber(EAP_NOOB_MSG_TYPE_ERROR_NOTIFICATION));
    cJSON_AddItemToObject(ret_json, "ErrorCode", cJSON_CreateNumber(error_code));
    //</editor-fold>

    //<editor-fold desc="Build response wpabuf packet">
    char *return_json = cJSON_PrintUnformatted(ret_json);
    cJSON_Delete(ret_json);
    size_t payload_len = strlen(return_json);
    struct wpabuf *to_return = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, payload_len, EAP_CODE_RESPONSE, eap_get_id(reqData));
    wpabuf_put_data(to_return, return_json, payload_len);
    os_free(return_json);
    //</editor-fold>

    return to_return;
}

static struct wpabuf *eap_noob_handle_type_1(struct eap_sm *sm, struct eap_noob_data *data, struct eap_method_ret *ret, const struct wpabuf *reqData, cJSON *json)
{
    // No parsing of the incoming JSON, since it does not contain anything.

    //<editor-fold desc="Build response JSON structure">
    // Reply with Type 1
    cJSON *ret_json = cJSON_CreateObject();
    cJSON_AddItemToObject(ret_json, "Type", cJSON_CreateNumber(EAP_NOOB_MSG_TYPE_PEERID_AND_STATE_DISCOVERY));
    cJSON_AddItemToObject(ret_json, "PeerState", cJSON_CreateNumber(g_wpa_eap_noob_state.noob_state));
    if (data->peer_id) {
        cJSON_AddItemToObject(ret_json, "PeerId", cJSON_CreateStringReference(data->peer_id));
    }
    //</editor-fold>

    //<editor-fold desc="Build response wpabuf packet">
    char *return_json = cJSON_PrintUnformatted(ret_json);
    cJSON_Delete(ret_json);
    size_t payload_len = strlen(return_json);
    struct wpabuf *to_return = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, payload_len, EAP_CODE_RESPONSE, eap_get_id(reqData));
    wpabuf_put_data(to_return, return_json, payload_len);
    os_free(return_json);
    //</editor-fold>

    data->internal_state = EAP_NOOB_STATE_INTERNAL_PEERID_SENT;
    return to_return;
}

static struct wpabuf *eap_noob_handle_type_2(struct eap_sm *sm, struct eap_noob_data *data, struct eap_method_ret *ret, const struct wpabuf *reqData, cJSON *json)
{
    //<editor-fold desc="Parse incoming packet">
    cJSON *parsed_vers = cJSON_GetObjectItemCaseSensitive(json, "Vers");
    if (!cJSON_IsArray(parsed_vers)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: Vers was not an array");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    cJSON *parsed_vers_item;
    bool compatible = false;
    cJSON_ArrayForEach(parsed_vers_item, parsed_vers) {
        if (!cJSON_IsNumber(parsed_vers_item)) {
            return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
        }
        // TODO Currently this is fixed on protocol version 1
        if (parsed_vers_item->valueint == 1) {
            compatible = true;
        }
    }
    if (!compatible) {
        wpa_printf(MSG_INFO, "EAP-NOOB: No mutually supported version");
        return build_error_msg(reqData, EAP_NOOB_ERROR_NO_MUTUALLY_SUPPORTED_PROTOCOL_VERSION);
    }
    data->vers = cJSON_PrintUnformatted(parsed_vers);


    size_t len;
    cJSON *parsed_peerid = cJSON_GetObjectItemCaseSensitive(json, "PeerId");
    if (!cJSON_IsString(parsed_peerid)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: PeerID was not a string");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    if (data->peer_id == NULL) {
        wpa_printf(MSG_INFO, "EAP-NOOB: New PeerID allocated");
        len = strlen(parsed_peerid->valuestring);
        data->peer_id = os_zalloc(len + 1);
        memcpy(data->peer_id, parsed_peerid->valuestring, len + 1);
    } else {
        // TODO check for identical peer id
    }

    cJSON *parsed_newnai = cJSON_GetObjectItemCaseSensitive(json, "NewNAI");
    if (parsed_newnai == NULL) {
        wpa_printf(MSG_DEBUG, "EAP-NOOB: No NewNAI");
    } else {
        if (!cJSON_IsString(parsed_newnai)) {
            wpa_printf(MSG_INFO, "EAP-NOOB: NewNAI was not a string");
            return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
        }
        wpa_printf(MSG_INFO, "EAP-NOOB: NewNAI is set, updating");
        if (data->nai) {
            os_free(data->nai);
        }
        len = strlen(parsed_newnai->valuestring);
        data->nai = os_zalloc(len + 1);
        strcpy(data->nai, parsed_newnai->valuestring);
    }

    cJSON *parsed_cryptosuites = cJSON_GetObjectItemCaseSensitive(json, "Cryptosuites");
    if (!cJSON_IsArray(parsed_cryptosuites)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: Cryptosuites was not an array");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    cJSON *parsed_cryptosuite_item;
    compatible = false;
    cJSON_ArrayForEach(parsed_cryptosuite_item, parsed_cryptosuites) {
        if (!cJSON_IsNumber(parsed_cryptosuite_item)) {
            return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
        }
        // TODO Currently fixed on cryptosuite value "1" (which is Curve25519 with SHA2)
        if (parsed_cryptosuite_item->valueint == 1) {
            compatible = true;
        }
    }
    if (!compatible) {
        wpa_printf(MSG_INFO, "EAP-NOOB: No mutually supported cryptosuite");
        return build_error_msg(reqData, EAP_NOOB_ERROR_NO_MUTUALLY_SUPPORTED_CRYPTOSUITE);
    }
    data->cryptosuites = cJSON_PrintUnformatted(parsed_cryptosuites);

    cJSON *parsed_dirs = cJSON_GetObjectItemCaseSensitive(json, "Dirs");
    if (!cJSON_IsNumber(parsed_dirs)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: Dirs was not a number");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    if ((parsed_dirs->valueint & g_wpa_eap_noob_state.supported_dir) == 0) {
        wpa_printf(MSG_INFO, "EAP-NOOB: No mutually supported OOB direction");
        return build_error_msg(reqData, EAP_NOOB_ERROR_NO_MUTUALLY_SUPPORTED_OOB_DIRECTION);
    }
    data->dirs = cJSON_PrintUnformatted(parsed_dirs);

    cJSON *parsed_serverinfo = cJSON_GetObjectItemCaseSensitive(json, "ServerInfo");
    if (!cJSON_IsObject(parsed_serverinfo)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: ServerInfo was not a JSON Object");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    data->server_info = cJSON_PrintUnformatted(parsed_serverinfo);
    // TODO: The ServerInfo is not yet parsed or saved into the state.
    //   For a production-ready implementation,send_error the ServerInfo object should be made available to the application.
    //</editor-fold>

    // Build Reply Type 2

    //<editor-fold desc="Build response JSON structure">
    cJSON *ret_json = cJSON_CreateObject();
    cJSON_AddItemToObject(ret_json, "Type", cJSON_CreateNumber(EAP_NOOB_MSG_TYPE_INITIAL_VERSION_NEGOTIATION));

    cJSON *ret_verp = cJSON_CreateNumber(1);
    data->verp = cJSON_PrintUnformatted(ret_verp);
    cJSON_AddItemToObject(ret_json, "Verp", ret_verp);

    cJSON_AddItemToObject(ret_json, "PeerId", cJSON_CreateStringReference(data->peer_id));

    cJSON *ret_cryptosuitep = cJSON_CreateNumber(1);
    data->cryptosuitep = cJSON_PrintUnformatted(ret_cryptosuitep);
    cJSON_AddItemToObject(ret_json, "Cryptosuitep", ret_cryptosuitep);

    cJSON *ret_dirp = cJSON_CreateNumber(g_wpa_eap_noob_state.supported_dir);
    data->dirp = cJSON_PrintUnformatted(ret_dirp);
    cJSON_AddItemToObject(ret_json, "Dirp", ret_dirp);

    cJSON *ret_peerinfo = cJSON_CreateObject();
    data->peer_info = cJSON_PrintUnformatted(ret_peerinfo);
    cJSON_AddItemToObject(ret_json, "PeerInfo", ret_peerinfo);
    //</editor-fold>

    //<editor-fold desc="Build response wpabuf packet">
    char *return_json = cJSON_PrintUnformatted(ret_json);
    cJSON_Delete(ret_json);
    size_t payload_len = strlen(return_json);
    struct wpabuf *to_return = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, payload_len, EAP_CODE_RESPONSE, eap_get_id(reqData));
    wpabuf_put_data(to_return, return_json, payload_len);
    os_free(return_json);
    //</editor-fold>

    data->internal_state = EAP_NOOB_STATE_INTERNAL_VERSION_NEGOTIATION_SENT;
    return to_return;
}

static struct wpabuf *eap_noob_handle_type_3(struct eap_sm *sm, struct eap_noob_data *data, struct eap_method_ret *ret, const struct wpabuf *reqData, cJSON *json)
{
    //<editor-fold desc="Parse incoming JSON">
    cJSON *parsed_peerid = cJSON_GetObjectItemCaseSensitive(json, "PeerId");
    if (!cJSON_IsString(parsed_peerid)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: PeerID was not a string");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    // TODO check for identical peer id

    cJSON *parsed_pks = cJSON_GetObjectItemCaseSensitive(json, "PKs");
    if (!cJSON_IsObject(parsed_pks)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: PKs was not an object");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    data->pks = cJSON_PrintUnformatted(parsed_pks);

    cJSON *parsed_ns = cJSON_GetObjectItemCaseSensitive(json, "Ns");
    if (!cJSON_IsString(parsed_ns)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: Ns was not a string");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    size_t base64_len;
    u8 *ns = base64_url_decode(parsed_ns->valuestring, strlen(parsed_ns->valuestring), &base64_len);
    if (base64_len != 32) {
        wpa_printf(MSG_INFO, "EAP-NOOB: Ns was not 32 bytes long");
        free(ns);
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    data->ns_b = cJSON_PrintUnformatted(parsed_ns);
    memcpy(data->ns, ns, 32);
    free(ns);

    cJSON *parsed_sleeptime = cJSON_GetObjectItemCaseSensitive(json, "SleepTime");
    // TODO: actually parse SleepTime
    //</editor-fold>

    cJSON *ret_pkp;

    int ecdhe_status = eap_noob_calculate_ecdhe_keyexchange(parsed_pks, &ret_pkp, &data->shared_key, &data->shared_key_length);

    if (ecdhe_status != 0) {
        cJSON_Delete(ret_pkp);
        return build_error_msg(reqData, ecdhe_status);
    }
    data->pkp = cJSON_PrintUnformatted(ret_pkp);

    int nonce_status = eap_noob_generate_new_nonce(data->np, 32);
    if (nonce_status != 0) {
        return build_error_msg(reqData, nonce_status);
    }
    char *np_b = base64_url_encode(data->np, 32, &base64_len);

    //<editor-fold desc="Build response JSON structure">
    // Build Reply Type 3
    cJSON *ret_json = cJSON_CreateObject();
    cJSON_AddItemToObject(ret_json, "Type", cJSON_CreateNumber(EAP_NOOB_MSG_TYPE_INITIAL_ECDHE_EXCHANGE));
    cJSON_AddItemToObject(ret_json, "PeerId", cJSON_CreateStringReference(data->peer_id));

    cJSON_AddItemToObject(ret_json, "PKp", ret_pkp);

    cJSON *ret_np = cJSON_CreateString(np_b);
    data->np_b = cJSON_PrintUnformatted(ret_np);
    cJSON_AddItemToObject(ret_json, "Np", ret_np);
    //</editor-fold>


    //<editor-fold desc="Build response wpabuf packet">
    char *return_json = cJSON_PrintUnformatted(ret_json);
    cJSON_Delete(ret_json);
    size_t payload_len = strlen(return_json);
    struct wpabuf *to_return = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, payload_len, EAP_CODE_RESPONSE, eap_get_id(reqData));
    wpabuf_put_data(to_return, return_json, payload_len);
    os_free(return_json);
    //</editor-fold>

    eap_noob_save_ephemeral_state(data, EAP_NOOB_KEYINGMODE_COMPLETION);

    // TODO: Add callback to the notification function for OOB Message generation

    data->internal_state = EAP_NOOB_STATE_INTERNAL_PUBKEY_SENT;
    return to_return;
}

static struct wpabuf *eap_noob_handle_type_4(struct eap_sm *sm, struct eap_noob_data *data, struct eap_method_ret *ret, const struct wpabuf *reqData, cJSON *json)
{
    // TODO Actually parse the waiting exchange

    //<editor-fold desc="Build response JSON structure">
    // Reply with Type 4
    cJSON *ret_json = cJSON_CreateObject();
    cJSON_AddItemToObject(ret_json, "Type", cJSON_CreateNumber(EAP_NOOB_MSG_TYPE_WAITING));
    cJSON_AddItemToObject(ret_json, "PeerId", cJSON_CreateStringReference(data->peer_id));
    //</editor-fold>

    //<editor-fold desc="Build response wpabuf packet">
    char *return_json = cJSON_PrintUnformatted(ret_json);
    cJSON_Delete(ret_json);
    size_t payload_len = strlen(return_json);
    struct wpabuf *to_return = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, payload_len, EAP_CODE_RESPONSE, eap_get_id(reqData));
    wpabuf_put_data(to_return, return_json, payload_len);
    os_free(return_json);
    //</editor-fold>

    data->internal_state = EAP_NOOB_STATE_INTERNAL_WAITING_SENT;

    return to_return;
}

static struct wpabuf *eap_noob_handle_type_5(struct eap_sm *sm, struct eap_noob_data *data, struct eap_method_ret *ret, const struct wpabuf *reqData, cJSON *json)
{
    // TODO: Parse incoming message
    struct eap_noob_ephemeral_state_info *eph = g_wpa_eap_noob_state.ephemeral_state;
    eap_noob_oob_msg_node_t *msgNode = eph->oobMessages;
    while (msgNode != NULL) {
        if (msgNode->value->dir == EAP_NOOB_OOB_DIRECTION_SERVER_TO_PEER) {
            break;
        }
        msgNode = msgNode->next;
    }
    if (msgNode == NULL) {
        // TODO: Send out error, no suitable OOB Message found
        //   This should not happen, since type 5 is only sent by the server if we are in "OOB Received" state,
        //   and we should not be in this state if we don't have a OOB message here.
        return build_error_msg(reqData, EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR);
    }
    size_t b64_len;
    char *noobid = base64_url_encode(msgNode->value->noob_id, 16, &b64_len);
    data->used_oob_msg = msgNode->value;

    //<editor-fold desc="Build response JSON structure">
    cJSON *ret_json = cJSON_CreateObject();
    cJSON_AddItemToObject(ret_json, "Type", cJSON_CreateNumber(EAP_NOOB_MSG_TYPE_COMPLETION_NOOBID_DISCOVERY));
    cJSON_AddItemToObject(ret_json, "PeerId", cJSON_CreateStringReference(data->peer_id));
    cJSON_AddItemToObject(ret_json, "NoobId", cJSON_CreateString(noobid));
    //</editor-fold>

    //<editor-fold desc="Build response wpabuf packet">
    char *return_json = cJSON_PrintUnformatted(ret_json);
    cJSON_Delete(ret_json);
    size_t payload_len = strlen(return_json);
    struct wpabuf *to_return = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, payload_len, EAP_CODE_RESPONSE, eap_get_id(reqData));
    wpabuf_put_data(to_return, return_json, payload_len);
    os_free(return_json);
    //</editor-fold>

    data->internal_state = EAP_NOOB_STATE_INTERNAL_NOOBID_DISCOVERY_SENT;

    return to_return;
}

static struct wpabuf *eap_noob_handle_type_6(struct eap_sm *sm, struct eap_noob_data *data, struct eap_method_ret *ret, const struct wpabuf *reqData, cJSON *json)
{
    //<editor-fold desc="Parse incoming JSON">
    // TODO: Check for correct PeerId
    cJSON *parsed_noobid = cJSON_GetObjectItemCaseSensitive(json, "NoobId");
    if (!cJSON_IsString(parsed_noobid)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: NoobId was not a string");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    size_t b64_len;
    u8 *noobid = base64_url_decode(parsed_noobid->valuestring, strlen(parsed_noobid->valuestring), &b64_len);
    if (b64_len != 16) {
        free(noobid);
        wpa_printf(MSG_INFO, "EAP-NOOB: NoobId was not 16 bytes long");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    if (data->used_oob_msg != NULL) {
        if (memcmp(data->used_oob_msg->noob_id, noobid, 16) != 0) {
            free(noobid);
            wpa_printf(MSG_INFO, "EAP-NOOB: NoobId did not match");
            return build_error_msg(reqData, EAP_NOOB_ERROR_UNRECOGNIZED_OOB_MSG_IDENTIFIER);
        }
    } else {
        eap_noob_oob_msg_node_t *cur = g_wpa_eap_noob_state.ephemeral_state->oobMessages;
        while (cur != NULL) {
            if (memcmp(cur->value->noob_id, noobid, 16) == 0) {
                data->used_oob_msg = cur->value;
                break;
            }
            cur = cur->next;
        }
        free(noobid);
        if (cur == NULL) {
            wpa_printf(MSG_INFO, "EAP-NOOB: no matching NoobId found");
            return build_error_msg(reqData, EAP_NOOB_ERROR_UNRECOGNIZED_OOB_MSG_IDENTIFIER);
        }
    }

    cJSON *parsed_macs = cJSON_GetObjectItemCaseSensitive(json, "MACs");
    if (!cJSON_IsString(parsed_macs)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: MACs was not a string");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    size_t b64_len2;
    u8 *macs = base64_url_decode(parsed_macs->valuestring, strlen(parsed_macs->valuestring), &b64_len2);
    if (b64_len2 != 32) {
        os_free(macs);
        wpa_printf(MSG_INFO, "EAP-NOOB: MACs was not 32 bytes long");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    //</editor-fold>

    data->cryptokeys = eap_noob_calculate_cryptographic_elements(data, EAP_NOOB_KEYINGMODE_COMPLETION, NULL);
    // Cryptographic secure (time constant) check for equal MAC

    u8 mac_check = cryptographic_secure_compare(macs, data->cryptokeys->macs, 32);

    os_free(macs);

    if (mac_check != 0) {
        wpa_printf(MSG_INFO, "EAP-NOOB: MACs did not match.");
        return build_error_msg(reqData, EAP_NOOB_ERROR_HMAC_VERIFICATION_FAILURE);
    }

    char *macp_base64 = base64_url_encode(data->cryptokeys->macp, 32, &b64_len);

    // TODO
    //   At this point the association is finished, so we can write the persistent state.
    //   The problem is: we don't know if the server sees it the same way. To know this, we need to wait
    //   for the EAP-Success from the server. Only then the association is indeed persistent.
    //   If we do it this way, we may end up with a persistent state stored, when the server did not move to the
    //   "Registered" state. For now we just live with that.

    eap_noob_save_initial_persistent_state(data, data->cryptokeys->kdf_out + 288);

    //<editor-fold desc="Build response JSON structure">
    cJSON *ret_json = cJSON_CreateObject();

    cJSON_AddItemToObject(ret_json, "Type", cJSON_CreateNumber(EAP_NOOB_MSG_TYPE_COMPLETION_AUTHENTICATION));
    cJSON_AddItemToObject(ret_json, "PeerId", cJSON_CreateStringReference(data->peer_id));
    cJSON_AddItemToObject(ret_json, "MACp", cJSON_CreateString(macp_base64));
    //</editor-fold>

    //<editor-fold desc="Build response wpabuf packet">
    char *return_json = cJSON_PrintUnformatted(ret_json);
    cJSON_Delete(ret_json);
    size_t payload_len = strlen(return_json);
    struct wpabuf *to_return = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, payload_len, EAP_CODE_RESPONSE, eap_get_id(reqData));
    wpabuf_put_data(to_return, return_json, payload_len);
    os_free(return_json);
    //</editor-fold>

    data->internal_state = EAP_NOOB_STATE_INTERNAL_MACP_SENT;

    return to_return;
}

static struct wpabuf *eap_noob_handle_type_7(struct eap_sm *sm, struct eap_noob_data *data, struct eap_method_ret *ret, const struct wpabuf *reqData, cJSON *json)
{
    //<editor-fold desc="Parse incoming JSON">
    cJSON *parsed_peerid = cJSON_GetObjectItemCaseSensitive(json, "PeerId");
    if (!cJSON_IsString(parsed_peerid)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: PeerID was not a string");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }

    cJSON *parsed_vers = cJSON_GetObjectItemCaseSensitive(json, "Vers");
    if (!cJSON_IsArray(parsed_vers)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: Vers was not an array");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    cJSON *parsed_vers_item;
    bool compatible = false;
    cJSON_ArrayForEach(parsed_vers_item, parsed_vers) {
        if (!cJSON_IsNumber(parsed_vers_item)) {
            return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
        }
        // TODO Currently this is fixed on protocol version 1
        if (parsed_vers_item->valueint == 1) {
            compatible = true;
        }
    }
    if (!compatible) {
        wpa_printf(MSG_INFO, "EAP-NOOB: No mutually supported version");
        return build_error_msg(reqData, EAP_NOOB_ERROR_NO_MUTUALLY_SUPPORTED_PROTOCOL_VERSION);
    }
    data->vers = cJSON_PrintUnformatted(parsed_vers);

    cJSON *parsed_cryptosuites = cJSON_GetObjectItemCaseSensitive(json, "Cryptosuites");
    if (!cJSON_IsArray(parsed_cryptosuites)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: Cryptosuites was not an array");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    cJSON *parsed_cryptosuite_item;
    compatible = false;
    cJSON_ArrayForEach(parsed_cryptosuite_item, parsed_cryptosuites) {
        if (!cJSON_IsNumber(parsed_cryptosuite_item)) {
            return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
        }
        // TODO Currently fixed on cryptosuite value "1" (which is Curve25519 with SHA2)
        if (parsed_cryptosuite_item->valueint == 1) {
            compatible = true;
        }
    }
    if (!compatible) {
        wpa_printf(MSG_INFO, "EAP-NOOB: No mutually supported cryptosuite");
        return build_error_msg(reqData, EAP_NOOB_ERROR_NO_MUTUALLY_SUPPORTED_CRYPTOSUITE);
    }
    data->cryptosuites = cJSON_PrintUnformatted(parsed_cryptosuites);

    cJSON *parsed_newnai = cJSON_GetObjectItemCaseSensitive(json, "NewNAI");
    if (parsed_newnai == NULL) {
        wpa_printf(MSG_DEBUG, "EAP-NOOB: No NewNAI");
    } else {
        if (!cJSON_IsString(parsed_newnai)) {
            wpa_printf(MSG_INFO, "EAP-NOOB: NewNAI was not a string");
            return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
        }
        wpa_printf(MSG_INFO, "EAP-NOOB: NewNAI is set, updating");
        if (data->nai) {
            os_free(data->nai);
        }
        size_t len = strlen(parsed_newnai->valuestring);
        data->nai = os_zalloc(len + 1);
        strcpy(data->nai, parsed_newnai->valuestring);
    }

    cJSON *parsed_serverinfo = cJSON_GetObjectItemCaseSensitive(json, "ServerInfo");
    if (parsed_serverinfo == NULL) {
        data->server_info = NULL;
    } else {
        if (!cJSON_IsObject(parsed_serverinfo)) {
            wpa_printf(MSG_INFO, "EAP-NOOB: ServerInfo was not a JSON Object");
            return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
        }
        data->server_info = cJSON_PrintUnformatted(parsed_serverinfo);
    }
    //</editor-fold>

    //<editor-fold desc="Build response JSON structure">
    cJSON *ret_json = cJSON_CreateObject();

    cJSON_AddItemToObject(ret_json, "Type", cJSON_CreateNumber(EAP_NOOB_MSG_TYPE_RECONNECT_VERSION_NEGOTIATION));

    cJSON *ret_verp = cJSON_CreateNumber(1);
    data->verp = cJSON_PrintUnformatted(ret_verp);
    cJSON_AddItemToObject(ret_json, "Verp", ret_verp);

    cJSON_AddItemToObject(ret_json, "PeerId", cJSON_CreateStringReference(data->peer_id));

    cJSON *ret_cryptosuitep = cJSON_CreateNumber(1);
    data->cryptosuitep = cJSON_PrintUnformatted(ret_cryptosuitep);
    cJSON_AddItemToObject(ret_json, "Cryptosuitep", ret_cryptosuitep);
    //</editor-fold>

    //<editor-fold desc="Build response wpabuf packet">
    char *return_json = cJSON_PrintUnformatted(ret_json);
    cJSON_Delete(ret_json);
    size_t payload_len = strlen(return_json);
    struct wpabuf *to_return = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, payload_len, EAP_CODE_RESPONSE, eap_get_id(reqData));
    wpabuf_put_data(to_return, return_json, payload_len);
    os_free(return_json);
    //</editor-fold>

    data->internal_state = EAP_NOOB_STATE_INTERNAL_RECONNECT_VERSION_NEGOTIATION_SENT;
    return to_return;
}

static struct wpabuf *eap_noob_handle_type_8(struct eap_sm *sm, struct eap_noob_data *data, struct eap_method_ret *ret, const struct wpabuf *reqData, cJSON *json)
{
    //<editor-fold desc="Parse incoming JSON">
    cJSON *parsed_peerid = cJSON_GetObjectItemCaseSensitive(json, "PeerId");
    if (!cJSON_IsString(parsed_peerid)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: PeerID was not a string");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }

    cJSON *parsed_keyingmode = cJSON_GetObjectItemCaseSensitive(json, "KeyingMode");
    if (!cJSON_IsNumber(parsed_keyingmode)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: KeyingMode was not a number");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    data->keyingmode = parsed_keyingmode->valueint;

    cJSON *parsed_pks2 = cJSON_GetObjectItemCaseSensitive(json, "PKs2");
    if (parsed_pks2 == NULL) {
        data->pks = NULL;
    } else {
        if (!cJSON_IsObject(parsed_pks2)) {
            wpa_printf(MSG_INFO, "EAP-NOOB: PKs2 was not an Object");
            return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
        }
        data->pks = cJSON_PrintUnformatted(parsed_pks2);
    }
    cJSON *parsed_ns2 = cJSON_GetObjectItemCaseSensitive(json, "Ns2");
    if (!cJSON_IsString(parsed_ns2)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: Ns2 was not a string");
    }
    size_t base64_len;
    u8 *ns = base64_url_decode(parsed_ns2->valuestring, strlen(parsed_ns2->valuestring), &base64_len);
    if (base64_len != 32) {
        wpa_printf(MSG_INFO, "EAP-NOOB: Ns2 was not 32 bytes long");
        free(ns);
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    data->ns_b = cJSON_PrintUnformatted(parsed_ns2);
    memcpy(data->ns, ns, 32);
    free(ns);
    //</editor-fold>

    // Check for the needed parameters depending on the KeyingMode

    cJSON *ret_json = cJSON_CreateObject();

    if (data->keyingmode == EAP_NOOB_KEYINGMODE_RECONNECT_WITH_ECDHE || data->keyingmode == EAP_NOOB_KEYINGMODE_RECONNECT_CRYPTOSUITE_UPDATE) {
        if (parsed_pks2 == NULL) {
            wpa_printf(MSG_INFO, "EAP-NOOB: Keyingmode with ECDHE, but no server Parameters present");
            return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
        }

        cJSON *ret_pkp;

        int ecdhe_status = eap_noob_calculate_ecdhe_keyexchange(parsed_pks2, &ret_pkp, &data->shared_key, &data->shared_key_length);
        if (ecdhe_status != 0) {
            cJSON_Delete(ret_pkp);
            return build_error_msg(reqData, ecdhe_status);
        }

        cJSON_AddItemToObject(ret_json, "PKp2", ret_pkp);
    }

    int nonce_stats = eap_noob_generate_new_nonce(data->np, 32);
    if (nonce_stats != 0) {
        return build_error_msg(reqData, EAP_NOOB_ERROR_APPLICATION_SPECIFIC_ERROR);
    }

    char *np2_b = base64_url_encode(data->np, 32, &base64_len);

    cJSON *ret_np2 = cJSON_CreateString(np2_b);
    data->np_b = cJSON_PrintUnformatted(ret_np2);
    cJSON_AddItemToObject(ret_json, "Np2", ret_np2);

    cJSON_AddItemToObject(ret_json, "Type", cJSON_CreateNumber(EAP_NOOB_MSG_TYPE_RECONNECT_ECHDE_EXCHANGE));
    cJSON_AddItemToObject(ret_json, "PeerId", cJSON_CreateStringReference(data->peer_id));

    //<editor-fold desc="Build response wpabuf packet">
    char *return_json = cJSON_PrintUnformatted(ret_json);
    cJSON_Delete(ret_json);
    size_t payload_len = strlen(return_json);
    struct wpabuf *to_return = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, payload_len, EAP_CODE_RESPONSE, eap_get_id(reqData));
    wpabuf_put_data(to_return, return_json, payload_len);
    os_free(return_json);
    //</editor-fold>

    data->internal_state = EAP_NOOB_STATE_INTERNAL_RECONNECT_PUBKEY_SENT;
    return to_return;
}

static struct wpabuf *eap_noob_handle_type_9(struct eap_sm *sm, struct eap_noob_data *data, struct eap_method_ret *ret, const struct wpabuf *reqData, cJSON *json)
{
    //<editor-fold desc="Parse incoming JSON">
    cJSON *parsed_peerid = cJSON_GetObjectItemCaseSensitive(json, "PeerId");
    if (!cJSON_IsString(parsed_peerid)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: PeerID was not a string");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }

    cJSON *parsed_macs2 = cJSON_GetObjectItemCaseSensitive(json, "MACs2");
    if (!cJSON_IsString(parsed_macs2)) {
        wpa_printf(MSG_INFO, "EAP-NOOB: MACs2 was not a string");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    size_t b64_len;
    u8 *macs2 = base64_url_decode(parsed_macs2->valuestring, strlen(parsed_macs2->valuestring), &b64_len);
    if (b64_len != 32) {
        os_free(macs2);
        wpa_printf(MSG_INFO, "EAP-NOOB: MACs2 was not 32 bytes long");
        return build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
    }
    //</editor-fold>


    // This is now the tricky part, where we have to find out, if we have to update our persistent state.

    // First, we compute and compare the MACs2 value based on Kz.
    data->cryptokeys = eap_noob_calculate_cryptographic_elements(data, data->keyingmode, g_wpa_eap_noob_state.kz);
    u8 mac_check = cryptographic_secure_compare(macs2, data->cryptokeys->macs, 32);

    // If the MACs2 value computed with Kz we have to flush the KzPrev (if there was any).
    if (mac_check == 0) {
        if (g_wpa_eap_noob_state.prev_active) {
            eap_noob_flush_persistent_kz_prev();
            data->persistent_state_updated = true;
        }
    } else {
        // If the MACs2 value computed with Kz did not match, we have to recompute the MAC values using KzPrev, if it exists.
        // We check for the negative case first, in this case we can send an error immediately
        if (!g_wpa_eap_noob_state.prev_active) {
            wpa_printf(MSG_INFO, "EAP-NOOB: MACs2 did not match");
            return build_error_msg(reqData, EAP_NOOB_ERROR_HMAC_VERIFICATION_FAILURE);
        }

        // Then we recalculate the keying material and compare the MACs2 value against that.
        os_free(data->cryptokeys);
        data->cryptokeys = eap_noob_calculate_cryptographic_elements(data, data->keyingmode, g_wpa_eap_noob_state.kz_prev);
        mac_check = cryptographic_secure_compare(macs2, data->cryptokeys->macs, 32);

        // If it didn't match too, then we can return an error.
        if (mac_check != 0) {
            wpa_printf(MSG_INFO, "EAP-NOOB: MACs2 with Kz and KzPrev did not match");
            return build_error_msg(reqData, EAP_NOOB_ERROR_HMAC_VERIFICATION_FAILURE);
        }

        // In this case we rollback
        eap_noob_rollback_persistent_kz_prev();
        data->persistent_state_updated = true;
    }

    // At this point we can build the response to the server

    char *macp_base64 = base64_url_encode(data->cryptokeys->macp, 32, &b64_len);

    cJSON *ret_json = cJSON_CreateObject();
    cJSON_AddItemToObject(ret_json, "Type", cJSON_CreateNumber(EAP_NOOB_MSG_TYPE_RECONNECT_AUTHENTICATION));
    cJSON_AddItemToObject(ret_json, "PeerId", cJSON_CreateStringReference(data->peer_id));
    cJSON_AddItemToObject(ret_json, "MACp2", cJSON_CreateString(macp_base64));

    //<editor-fold desc="Build response wpabuf packet">
    char *return_json = cJSON_PrintUnformatted(ret_json);
    cJSON_Delete(ret_json);
    size_t payload_len = strlen(return_json);
    struct wpabuf *to_return = eap_msg_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, payload_len, EAP_CODE_RESPONSE, eap_get_id(reqData));
    wpabuf_put_data(to_return, return_json, payload_len);
    os_free(return_json);
    //</editor-fold>

    data->internal_state = EAP_NOOB_STATE_INTERNAL_MACP_SENT;

    // After we have build our response, we have to update the persistent storage, if we are in the cryptosuite upgrade exchange.
    if (data->keyingmode == EAP_NOOB_KEYINGMODE_RECONNECT_CRYPTOSUITE_UPDATE) {
        eap_noob_update_persistent_state(data, data->cryptokeys->kdf_out + 288);
        data->persistent_state_updated = true;
    }

    // TODO At this point we should use a callback function to let the overlaying application know to update the persistent state it saves to persistent storage.

    return to_return;
}

static void eap_noob_deinit(struct eap_sm *sm, void *priv);

static void *eap_noob_init(struct eap_sm *sm)
{
    // IF the global eap_noob state is not in active mode, we won't try to do EAP-NOOB.
    if (!g_wpa_eap_noob_state.active) {
        return NULL;
    }

    struct eap_noob_data *data;
    data = (struct eap_noob_data *)os_zalloc(sizeof(struct eap_noob_data));
    if (data == NULL) {
        return NULL;
    }

    data->keyingmode = 0;
    data->persistent_state_updated = false;
    data->internal_state = EAP_NOOB_STATE_INTERNAL_IDENTITY_SENT;

    // Get the NAI from the anonymous identity
    data->nai = os_zalloc(g_wpa_anonymous_identity_len + 1);
    if (data->nai == NULL) {
        eap_noob_deinit(sm, data);
        return NULL;
    }
    memcpy(data->nai, g_wpa_anonymous_identity, g_wpa_anonymous_identity_len);
    data->nai[g_wpa_anonymous_identity_len] = 0;

    if (g_wpa_eap_noob_state.peer_id) {
        data->peer_id = os_zalloc(strlen(g_wpa_eap_noob_state.peer_id) + 1);
        if (data->peer_id == NULL) {
            eap_noob_deinit(sm, data);
            return NULL;
        }
        strcpy(data->peer_id, g_wpa_eap_noob_state.peer_id);
    }

    if (g_wpa_eap_noob_state.ephemeral_state != NULL) {
        if (g_wpa_eap_noob_state.ephemeral_state->shared_secret != NULL) {
            data->shared_key_length = g_wpa_eap_noob_state.ephemeral_state->shared_secret_length;
            data->shared_key = os_zalloc(data->shared_key_length);
            memcpy(data->shared_key, g_wpa_eap_noob_state.ephemeral_state->shared_secret, data->shared_key_length);
        }
        memcpy(data->np, g_wpa_eap_noob_state.ephemeral_state->np, 32);
        memcpy(data->ns, g_wpa_eap_noob_state.ephemeral_state->ns, 32);
    }

    return data;
}

static void eap_noob_deinit(struct eap_sm *sm, void *priv)
{
    struct eap_noob_data *data = priv;
    if (data == NULL) {
        return;
    }

    if (data->nai != NULL) {
        os_free(data->nai);
    }

    if (data->peer_id != NULL) {
        os_free(data->peer_id);
    }

    if (data->vers != NULL) {
        os_free(data->vers);
    }

    if (data->cryptosuites != NULL) {
        os_free(data->cryptosuites);
    }

    if (data->dirs != NULL) {
        os_free(data->dirs);
    }

    if (data->server_info != NULL) {
        os_free(data->server_info);
    }

    if (data->verp != NULL) {
        os_free(data->verp);
    }

    if (data->cryptosuitep != NULL) {
        os_free(data->cryptosuitep);
    }

    if (data->dirp != NULL) {
        os_free(data->dirp);
    }

    if (data->peer_info != NULL) {
        os_free(data->peer_info);
    }

    if (data->pks != NULL) {
        os_free(data->pks);
    }

    if (data->ns_b != NULL) {
        os_free(data->ns_b);
    }

    if (data->pkp != NULL) {
        os_free(data->pkp);
    }

    if (data->np_b != NULL) {
        os_free(data->np_b);
    }

    if (data->shared_key != NULL) {
        os_free(data->shared_key);
    }

    if (data->cryptokeys != NULL) {
        os_free(data->cryptokeys);
    }

    // Caution: used_oob_msg is not freed, since this is simply a reference

    os_free(data);
}

static struct wpabuf *eap_noob_process(struct eap_sm *sm, void *priv, struct eap_method_ret *ret, const struct wpabuf *reqData)
{
    size_t len;
    struct eap_noob_data *data = priv;
    const u8 *eap_pkt = eap_hdr_validate(EAP_VENDOR_IETF, EAP_TYPE_NOOB, reqData, &len);

    cJSON *recvContent;

    if (eap_pkt == NULL) {
        // No content in the EAP Packet, so ignore it.
        ret->ignore = true;
        return NULL;
    }

    struct wpabuf *to_return;

    recvContent = cJSON_ParseWithLength((char *) eap_pkt, len);
    if (recvContent == NULL) {
        // Invalid JSON
        to_return = build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_MESSAGE_STRUCTURE);
        goto send_wpabuf;
    }

    cJSON *parsed_type = cJSON_GetObjectItemCaseSensitive(recvContent, "Type");
    if (!cJSON_IsNumber(parsed_type)) {
        // Type is not a number
        to_return = build_error_msg(reqData, EAP_NOOB_ERROR_INVALID_DATA);
        goto send_wpabuf;
    }

    int type = parsed_type->valueint;

    switch (type) {
    case EAP_NOOB_MSG_TYPE_ERROR_NOTIFICATION:
        // Error message
        // TODO at this point the EAP communication should be aborted
        goto ignore;
    case EAP_NOOB_MSG_TYPE_PEERID_AND_STATE_DISCOVERY:
        // PeerId and PeerState discovery
        if (data->internal_state != EAP_NOOB_STATE_INTERNAL_IDENTITY_SENT) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_UNEXPECTED_MESSAGE_TYPE);
        } else {
            to_return = eap_noob_handle_type_1(sm, data, ret, reqData, recvContent);
        }
        break;
    case EAP_NOOB_MSG_TYPE_INITIAL_VERSION_NEGOTIATION:
        // Version, cryptosuite, and parameter negotiation
        if (g_wpa_eap_noob_state.noob_state == EAP_NOOB_STATE_REGISTERED ||
                g_wpa_eap_noob_state.noob_state == EAP_NOOB_STATE_RECONNECTING
           ) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_STATE_MISMATCH);
        } else if (data->internal_state != EAP_NOOB_STATE_INTERNAL_PEERID_SENT) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_UNEXPECTED_MESSAGE_TYPE);
        } else {
            to_return = eap_noob_handle_type_2(sm, data, ret, reqData, recvContent);
        }
        break;
    case EAP_NOOB_MSG_TYPE_INITIAL_ECDHE_EXCHANGE:
        // Exchange of ECDHE keys and nonces
        if (data->internal_state != EAP_NOOB_STATE_INTERNAL_VERSION_NEGOTIATION_SENT) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_UNEXPECTED_MESSAGE_TYPE);
        } else {
            to_return = eap_noob_handle_type_3(sm, data, ret, reqData, recvContent);
        }
        break;
    case EAP_NOOB_MSG_TYPE_WAITING:
        // Indication to the peer that the server has not yet received an OOB message
        if (g_wpa_eap_noob_state.noob_state != EAP_NOOB_STATE_WAITING_FOR_OOB &&
                g_wpa_eap_noob_state.noob_state != EAP_NOOB_STATE_OOB_RECEIVED
           ) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_STATE_MISMATCH);
        } else if (data->internal_state != EAP_NOOB_STATE_INTERNAL_PEERID_SENT) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_UNEXPECTED_MESSAGE_TYPE);
        } else {
            to_return = eap_noob_handle_type_4(sm, data, ret, reqData, recvContent);
        }
        break;
    case EAP_NOOB_MSG_TYPE_COMPLETION_NOOBID_DISCOVERY:
        // NoobId discovery
        if (g_wpa_eap_noob_state.noob_state != EAP_NOOB_STATE_WAITING_FOR_OOB &&
                g_wpa_eap_noob_state.noob_state != EAP_NOOB_STATE_OOB_RECEIVED
           ) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_STATE_MISMATCH);
        } else if (data->internal_state != EAP_NOOB_STATE_INTERNAL_PEERID_SENT) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_UNEXPECTED_MESSAGE_TYPE);
        } else {
            to_return = eap_noob_handle_type_5(sm, data, ret, reqData, recvContent);
        }
        break;
    case EAP_NOOB_MSG_TYPE_COMPLETION_AUTHENTICATION:
        // Authentication and key confirmation with HMAC
        if (g_wpa_eap_noob_state.noob_state != EAP_NOOB_STATE_WAITING_FOR_OOB &&
                g_wpa_eap_noob_state.noob_state != EAP_NOOB_STATE_OOB_RECEIVED
           ) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_STATE_MISMATCH);
        } else if (data->internal_state != EAP_NOOB_STATE_INTERNAL_PEERID_SENT &&
                   data->internal_state != EAP_NOOB_STATE_INTERNAL_NOOBID_DISCOVERY_SENT
                  ) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_UNEXPECTED_MESSAGE_TYPE);
        } else {
            to_return = eap_noob_handle_type_6(sm, data, ret, reqData, recvContent);
        }
        break;
    case EAP_NOOB_MSG_TYPE_RECONNECT_VERSION_NEGOTIATION:
        // Version, cryptosuite, and parameter negotiation
        if (g_wpa_eap_noob_state.noob_state != EAP_NOOB_STATE_REGISTERED &&
                g_wpa_eap_noob_state.noob_state != EAP_NOOB_STATE_RECONNECTING
           ) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_STATE_MISMATCH);
        } else if (data->internal_state != EAP_NOOB_STATE_INTERNAL_PEERID_SENT) {
            to_return = build_error_msg(reqData, EAP_NOOB_ERROR_UNEXPECTED_MESSAGE_TYPE);
        } else {
            to_return = eap_noob_handle_type_7(sm, data, ret, reqData, recvContent);
        }
        break;
    case EAP_NOOB_MSG_TYPE_RECONNECT_ECHDE_EXCHANGE:
        // Exchange of ECDHE keys and nonces
        to_return = eap_noob_handle_type_8(sm, data, ret, reqData, recvContent);
        break;
    case EAP_NOOB_MSG_TYPE_RECONNECT_AUTHENTICATION:
        // Authentication and key confirmation with HMAC
        to_return = eap_noob_handle_type_9(sm, data, ret, reqData, recvContent);
        break;
    default:
        // Unknown message type.
        to_return = build_error_msg(reqData, EAP_NOOB_ERROR_UNEXPECTED_MESSAGE_TYPE);
    }
send_wpabuf:
    if (recvContent != NULL) {
        cJSON_Delete(recvContent);
    }
    return to_return;

ignore:
    if (recvContent != NULL) {
        cJSON_Delete(recvContent);
    }
    ret->ignore = true;
    return NULL;

}

static bool eap_noob_isKeyAvailable(struct eap_sm *sm, void *priv)
{
    struct eap_noob_data *data = priv;
    return data->internal_state == EAP_NOOB_STATE_INTERNAL_MACP_SENT;
}

static u8 *eap_noob_getKey(struct eap_sm *sm, void *priv, size_t *len)
{
    if (!eap_noob_isKeyAvailable(sm, priv)) {
        return NULL;
    }

    struct eap_noob_data *data = priv;
    u8 *msk = os_zalloc(64);
    memcpy(msk, data->cryptokeys->kdf_out, 64);
    *len = 64;
    return msk;
}

static u8 *eap_noob_get_emsk(struct eap_sm *sm, void *priv, size_t *len)
{
    if (!eap_noob_isKeyAvailable(sm, priv)) {
        return NULL;
    }

    struct eap_noob_data *data = priv;
    u8 *emsk = os_zalloc(64);
    memcpy(emsk, data->cryptokeys->kdf_out + 64, 64);
    *len = 64;
    return emsk;
}

int eap_peer_noob_register(void)
{
    struct eap_method *eap;
    int ret;

    eap = eap_peer_method_alloc(EAP_VENDOR_IETF, EAP_TYPE_NOOB, "NOOB");
    if (eap == NULL) {
        return -1;
    }

    eap->init = eap_noob_init;
    eap->deinit = eap_noob_deinit;
    eap->process = eap_noob_process;
    eap->isKeyAvailable = eap_noob_isKeyAvailable;
    eap->getKey = eap_noob_getKey;
    eap->get_emsk = eap_noob_get_emsk;

    ret = eap_peer_method_register(eap);
    if (ret) {
        eap_peer_method_free(eap);
    }
    return ret;
}
