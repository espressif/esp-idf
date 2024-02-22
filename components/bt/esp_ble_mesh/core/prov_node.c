/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <errno.h>
#include <string.h>

#include "crypto.h"
#include "adv.h"
#include "mesh.h"
#include "access.h"
#include "foundation.h"
#include "mesh/common.h"
#include "mesh/proxy.h"
#include "proxy_common.h"
#include "proxy_server.h"
#include "proxy_client.h"
#include "prov_common.h"
#include "prov_node.h"

#include "mesh_v1.1/utils.h"

#if CONFIG_BLE_MESH_NODE

#if CONFIG_BLE_MESH_PB_ADV && !CONFIG_BLE_MESH_PB_GATT
NET_BUF_SIMPLE_DEFINE_STATIC(rx_buf, PROV_RX_BUF_SIZE);
#endif

static struct bt_mesh_prov_link prov_link;

#if CONFIG_BLE_MESH_PB_ADV
static void reset_adv_link(struct bt_mesh_prov_link *link, uint8_t reason);
extern uint8_t node_next_xact_id(struct bt_mesh_prov_link *link);
#endif

#if CONFIG_BLE_MESH_PB_GATT
static int prov_send_gatt(struct bt_mesh_prov_link *link, struct net_buf_simple *msg);
#endif

static void prov_send_fail_msg(uint8_t err);

struct bt_mesh_prov_link *bt_mesh_prov_node_get_link(void)
{
    return &prov_link;
}

static void close_link(uint8_t reason)
{
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE)) {
        if (prov_link.pb_remote_close) {
            prov_link.pb_remote_close(&prov_link, reason);
        }
        return;
    }

    prov_send_fail_msg(reason);
}

void bt_mesh_prov_node_close_link(uint8_t reason)
{
    close_link(reason);
}

static void reset_state(void)
{
    k_delayed_work_cancel(&prov_link.prot_timer);

    /* Disable Attention Timer if it was set */
    if (prov_link.conf_inputs[0]) {
        bt_mesh_attention(NULL, 0);
    }

#if CONFIG_BLE_MESH_PB_GATT
    if (prov_link.conn) {
        bt_mesh_conn_unref(prov_link.conn);
    }
#endif /* CONFIG_BLE_MESH_PB_GATT */

#if CONFIG_BLE_MESH_PB_ADV
    /* Clear everything except the retransmit and protocol timer
     * delayed work objects.
     */
    (void)memset(&prov_link, 0, offsetof(struct bt_mesh_prov_link, tx.retransmit));

    prov_link.pending_ack = PROV_XACT_NVAL;

    prov_link.rx.prev_id = PROV_XACT_NVAL;
#if CONFIG_BLE_MESH_PB_GATT
    prov_link.rx.buf = bt_mesh_proxy_server_get_buf();
#else
    net_buf_simple_reset(&rx_buf);
    prov_link.rx.buf = &rx_buf;
#endif /* CONFIG_BLE_MESH_PB_GATT */

    prov_link.next_xact_id = node_next_xact_id;
    prov_link.reset_adv_link = reset_adv_link;
    prov_link.retrans_timeout = reset_adv_link;
    prov_link.invite_recv = false;

#if CONFIG_BLE_MESH_FAST_PROV
    prov_link.last_tx_pdu = PROV_COMPLETE;
#endif /* CONFIG_BLE_MESH_FAST_PROV */

#else /* !CONFIG_BLE_MESH_PB_ADV */
    /* Clear everything except the protocol timer (k_delayed_work) */
    (void)memset(&prov_link, 0, offsetof(struct bt_mesh_prov_link, prot_timer));
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
    prov_link.pb_gatt_send = prov_send_gatt;
#endif /* CONFIG_BLE_MESH_PB_GATT */
}

#if CONFIG_BLE_MESH_PB_ADV
static void reset_adv_link(struct bt_mesh_prov_link *link, uint8_t reason)
{
    ARG_UNUSED(link);

    bt_mesh_prov_clear_tx(&prov_link, true);

    if (bt_mesh_prov_get()->link_close) {
        bt_mesh_prov_get()->link_close(BLE_MESH_PROV_ADV, reason);
    }

#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
    /* Remove the link id from exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_REMOVE,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID,
                                    &prov_link.link_id);
#endif

    reset_state();
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
static int prov_send_gatt(struct bt_mesh_prov_link *link, struct net_buf_simple *msg)
{
    int err = 0;

    ARG_UNUSED(link);

    if (!prov_link.conn) {
        BT_ERR("PB-GATT send, not connected");
        return -ENOTCONN;
    }

    /* Changed by Espressif, add provisioning timeout timer operations.
     * When sending a provisioning PDU successfully, restart the 60s timer.
     */
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT && CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE)) {
        err = bt_mesh_proxy_client_send(link->conn, BLE_MESH_PROXY_PROV, msg);

        /* TODO: do we need to check the err here? */
        bt_mesh_rpr_srv_send_outbound_report(link->pb_remote_uuid, true);
    } else
#endif
    {
        err = bt_mesh_proxy_server_send(prov_link.conn, BLE_MESH_PROXY_PROV, msg);
    }

    if (err) {
        BT_ERR("Failed to send provisioning PDU");
        return err;
    }

    k_delayed_work_submit(&prov_link.prot_timer, PROTOCOL_TIMEOUT);

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

static void prov_send_fail_msg(uint8_t err)
{
    PROV_BUF(buf, 2);

    bt_mesh_prov_buf_init(&buf, PROV_FAILED);
    net_buf_simple_add_u8(&buf, err);

    if (bt_mesh_prov_send(&prov_link, &buf)) {
        BT_ERR("Failed to send Provisioning Failed message");
    }

    bt_mesh_atomic_set_bit(prov_link.flags, LINK_INVALID);
}

static void prov_invite(const uint8_t *data)
{
    PROV_BUF(buf, 12);

    BT_DBG("Attention Duration: %u seconds", data[0]);

    if (data[0]) {
        bt_mesh_attention(NULL, data[0]);
    }

#if CONFIG_BLE_MESH_CERT_BASED_PROV
    /* Indicate prov_invite is received */
    prov_link.invite_recv = true;
#endif

    prov_link.conf_inputs[0] = data[0];

    bt_mesh_prov_buf_init(&buf, PROV_CAPABILITIES);

    /* Number of Elements supported */
    net_buf_simple_add_u8(&buf, bt_mesh_elem_count());

    /* NOTE: If bit 1 of the OOB Type field is set to 1, bit 0 of
     * the Algorithms field shall be set to 0.
     */
    /* Supported algorithms - FIPS P-256 Elliptic Curve */
#if CONFIG_BLE_MESH_PROV_EPA
    if (bt_mesh_prov_get()->oob_type & BIT(PROV_ONLY_OOB_AUTH_SUPPORT)) {
        net_buf_simple_add_be16(&buf, BIT(PROV_ALG_P256_HMAC_SHA256));
    } else {
        net_buf_simple_add_be16(&buf, (BIT(PROV_ALG_P256_CMAC_AES128) |
                                       BIT(PROV_ALG_P256_HMAC_SHA256)));
    }
#else
    net_buf_simple_add_be16(&buf, BIT(PROV_ALG_P256_CMAC_AES128));
#endif

    /* Public Key Type */
    net_buf_simple_add_u8(&buf, bt_mesh_prov_get()->oob_pub_key);

    /* OOB Type */
    net_buf_simple_add_u8(&buf, bt_mesh_prov_get()->oob_type);

    /* Output OOB Size */
    net_buf_simple_add_u8(&buf, bt_mesh_prov_get()->output_size);

    /* Output OOB Action */
    net_buf_simple_add_be16(&buf, bt_mesh_prov_get()->output_actions);

    /* Input OOB Size */
    net_buf_simple_add_u8(&buf, bt_mesh_prov_get()->input_size);

    /* Input OOB Action */
    net_buf_simple_add_be16(&buf, bt_mesh_prov_get()->input_actions);

    memcpy(&prov_link.conf_inputs[1], &buf.data[1], 11);

    if (bt_mesh_prov_send(&prov_link, &buf)) {
        BT_ERR("Failed to send capabilities");
        return;
    }

    prov_link.expect = PROV_START;
}

static void prov_capabilities(const uint8_t *data)
{}

static int prov_auth(uint8_t method, uint8_t action, uint8_t size)
{
    bt_mesh_output_action_t output = 0U;
    bt_mesh_input_action_t input = 0U;
    uint8_t auth_size = 0;

    auth_size = PROV_AUTH_SIZE(&prov_link);

    switch (method) {
    case AUTH_METHOD_NO_OOB:
        if (action || size) {
            return -EINVAL;
        }

        (void)memset(prov_link.auth, 0, sizeof(prov_link.auth));
        return 0;

    case AUTH_METHOD_STATIC:
        if (action || size) {
            return -EINVAL;
        }

        if (bt_mesh_prov_get()->static_val_len > auth_size) {
            memcpy(prov_link.auth, bt_mesh_prov_get()->static_val, auth_size);
        } else {
            memcpy(prov_link.auth + auth_size - bt_mesh_prov_get()->static_val_len,
                   bt_mesh_prov_get()->static_val, bt_mesh_prov_get()->static_val_len);
            (void)memset(prov_link.auth, 0,
                         auth_size - bt_mesh_prov_get()->static_val_len);
        }

        return 0;

    case AUTH_METHOD_OUTPUT:
        output = bt_mesh_prov_output_action(action);
        if (!output) {
            return -EINVAL;
        }

        if (!(bt_mesh_prov_get()->output_actions & output)) {
            return -EINVAL;
        }

        if (size > bt_mesh_prov_get()->output_size) {
            return -EINVAL;
        }

        if (output == BLE_MESH_DISPLAY_STRING) {
            unsigned char str[9] = {'\0'};
            uint8_t i = 0U;

            bt_mesh_rand(str, size);

            /* Normalize to '0' .. '9' & 'A' .. 'Z' */
            for (i = 0U; i < size; i++) {
                str[i] %= 36;
                if (str[i] < 10) {
                    str[i] += '0';
                } else {
                    str[i] += 'A' - 10;
                }
            }
            str[size] = '\0';

            memcpy(prov_link.auth, str, size);
            (void)memset(prov_link.auth + size, 0,
                         sizeof(prov_link.auth) - size);

            return bt_mesh_prov_get()->output_string((char *)str);
        } else {
            uint32_t div[8] = { 10, 100, 1000, 10000, 100000,
                                1000000, 10000000, 100000000
                              };
            uint32_t num = 0U;

            bt_mesh_rand(&num, sizeof(num));

            if (output == BLE_MESH_BLINK ||
                output == BLE_MESH_BEEP ||
                output == BLE_MESH_VIBRATE) {
                /** NOTE: According to the Bluetooth Mesh Profile Specification
                 *  Section 5.4.2.4, blink, beep and vibrate should be a random
                 *  integer between 0 and 10^size.
                 */
                num = (num % (div[size - 1] - 1)) + 1;
            } else {
                num %= div[size - 1];
            }

            sys_put_be32(num, &prov_link.auth[auth_size - 4]);
            (void)memset(prov_link.auth, 0, auth_size - 4);

            return bt_mesh_prov_get()->output_number(output, num);
        }

    case AUTH_METHOD_INPUT:
        input = bt_mesh_prov_input_action(action);
        if (!input) {
            return -EINVAL;
        }

        if (!(bt_mesh_prov_get()->input_actions & input)) {
            return -EINVAL;
        }

        if (size > bt_mesh_prov_get()->input_size) {
            return -EINVAL;
        }

        if (input == BLE_MESH_ENTER_STRING) {
            bt_mesh_atomic_set_bit(prov_link.flags, WAIT_STRING);
        } else {
            bt_mesh_atomic_set_bit(prov_link.flags, WAIT_NUMBER);
        }

        return bt_mesh_prov_get()->input(input, size);

    default:
        return -EINVAL;
    }
}

static void prov_start(const uint8_t *data)
{
    BT_INFO("Algorithm:   0x%02x", data[0]);
    BT_INFO("Public Key:  0x%02x", data[1]);
    BT_INFO("Auth Method: 0x%02x", data[2]);
    BT_INFO("Auth Action: 0x%02x", data[3]);
    BT_INFO("Auth Size:   0x%02x", data[4]);

    prov_link.algorithm = data[0];
    if (data[0] >= PROV_ALG_METHOD_MAX_NUM) {
        BT_ERR("Unknown algorithm 0x%02x", data[0]);
        close_link(PROV_ERR_NVAL_FMT);
        return;
    }

    /* If bit 1 of the OOB Type field of the Provisioning Capabilities PDU
     * is set to 1 (Only OOB authenticated provisioning supported) and any
     * of the following conditions is met:
     *  1. the Algorithm field of the Provisioning Start PDU is not set to
     *     BTM_ECDH_P256_CMAC_AES128_AES_CCM and the Provisioning Start PDU
     *     has the Authentication Method field set to 0x00 (Authentication
     *     with No OOB).
     *  2. the Algorithm field is set to BTM_ECDH_P256_CMAC_AES128_AES_CCM.
     * the provisioning protocol shall fail and the message shall be treated
     * by the Provisionee as an error in the provisioning protocol.
     */
#if CONFIG_BLE_MESH_PROV_EPA
    if ((bt_mesh_prov_get()->oob_type & BIT(PROV_ONLY_OOB_AUTH_SUPPORT)) &&
        ((data[0] == PROV_ALG_P256_HMAC_SHA256 && data[2] == AUTH_METHOD_NO_OOB) ||
         data[0] == PROV_ALG_P256_CMAC_AES128)) {
        close_link(PROV_ERR_NVAL_FMT);
        return;
    }
#endif

    if (data[1] != bt_mesh_prov_get()->oob_pub_key) {
        BT_ERR("Invalid public key type: 0x%02x", data[1]);
        close_link(PROV_ERR_NVAL_FMT);
        return;
    }

    prov_link.public_key = data[1];

    memcpy(&prov_link.conf_inputs[12], data, 5);

    prov_link.expect = PROV_PUB_KEY;

    /* If Provisioning Start PDU indicates that provisioner chooses
     * OOB public key, then callback to the application layer to let
     * users input public & private key pair.
     */
    if (prov_link.public_key == PROV_OOB_PUB_KEY) {
        bt_mesh_prov_get()->oob_pub_key_cb();
    }

    if (prov_auth(data[2], data[3], data[4])) {
        BT_ERR("Invalid authentication method: 0x%02x; "
               "action: 0x%02x; size: 0x%02x",
               data[2], data[3], data[4]);
        close_link(PROV_ERR_NVAL_FMT);
    }
}

static void send_confirm(void)
{
    uint8_t *local_conf = NULL;
    uint8_t conf_val_size = 0;
    uint8_t rand_val_size = 0;
    uint8_t conf_salt_size = 0;
    uint8_t conf_key_size = 0;

    conf_val_size = PROV_CONF_SIZE(&prov_link);
    rand_val_size = PROV_RAND_SIZE(&prov_link);
    conf_salt_size = PROV_CONF_SALT_SIZE(&prov_link);
    conf_key_size = PROV_CONF_KEY_SIZE(&prov_link);

    ARG_UNUSED(conf_salt_size);
    ARG_UNUSED(conf_key_size);

    PROV_BUF(cfm, (conf_val_size + 1));

    BT_DBG("ConfInputs[0]   %s", bt_hex(prov_link.conf_inputs, 64));
    BT_DBG("ConfInputs[64]  %s", bt_hex(&prov_link.conf_inputs[64], 64));
    BT_DBG("ConfInputs[128] %s", bt_hex(&prov_link.conf_inputs[128], 17));

    if (prov_link.algorithm == PROV_ALG_P256_CMAC_AES128) {
        if (bt_mesh_prov_conf_salt(prov_link.conf_inputs, prov_link.conf_salt)) {
            BT_ERR("Unable to generate confirmation salt");
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }

        if (bt_mesh_prov_conf_key(prov_link.dhkey, prov_link.conf_salt,
                                  prov_link.conf_key)) {
            BT_ERR("Unable to generate confirmation key");
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }
    }
#if CONFIG_BLE_MESH_PROV_EPA
    else {
        if (bt_mesh_prov_conf_salt_epa(prov_link.conf_inputs, prov_link.conf_salt)) {
            BT_ERR("Unable to generate confirmation salt(epa)");
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }

        if (bt_mesh_prov_conf_key_epa(prov_link.dhkey, prov_link.auth,
                                      prov_link.conf_salt, prov_link.conf_key)) {
            BT_ERR("Unable to generate confirmation key(epa)");
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }
    }
#endif

    if (bt_mesh_rand(prov_link.rand, rand_val_size)) {
        BT_ERR("Unable to generate random number");
        close_link(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("ConfirmationSalt: %s", bt_hex(prov_link.conf_salt, conf_salt_size));
    BT_DBG("ConfirmationKey: %s", bt_hex(prov_link.conf_key, conf_key_size));
    BT_DBG("LocalRandom: %s", bt_hex(prov_link.rand, rand_val_size));

    bt_mesh_prov_buf_init(&cfm, PROV_CONFIRM);

    local_conf = net_buf_simple_add(&cfm, conf_val_size);

    if (prov_link.algorithm == PROV_ALG_P256_CMAC_AES128) {
        if (bt_mesh_prov_conf(prov_link.conf_key, prov_link.rand,
                              prov_link.auth, local_conf)) {
            BT_ERR("Unable to generate confirmation value");
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }
    }
#if CONFIG_BLE_MESH_PROV_EPA
    else {
        if (bt_mesh_prov_conf_epa(prov_link.conf_key, prov_link.rand,
                                  local_conf)) {
            BT_ERR("Unable to generate confirmation value(epa)");
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }
    }
#endif

    if (!memcmp(prov_link.conf, local_conf, conf_val_size)) {
        BT_ERR("Confirmation value is identical to ours, rejecting.");
        prov_send_fail_msg(PROV_ERR_NVAL_FMT);
        return;
    }

    if (bt_mesh_prov_send(&prov_link, &cfm)) {
        BT_ERR("Unable to send Provisioning Confirm");
        return;
    }

    prov_link.expect = PROV_RANDOM;
}

static void send_input_complete(void)
{
    PROV_BUF(buf, 1);

    bt_mesh_prov_buf_init(&buf, PROV_INPUT_COMPLETE);

    if (bt_mesh_prov_send(&prov_link, &buf)) {
        BT_ERR("Failed to send Provisioning Input Complete");
    }
}

int bt_mesh_input_number(uint32_t num)
{
    uint8_t auth_size = 0;

    auth_size = PROV_AUTH_SIZE(&prov_link);

    BT_INFO("%u", num);

    if (!bt_mesh_atomic_test_and_clear_bit(prov_link.flags, WAIT_NUMBER)) {
        return -EINVAL;
    }

    sys_put_be32(num, &prov_link.auth[auth_size - 4]);

    send_input_complete();

    if (!bt_mesh_atomic_test_bit(prov_link.flags, HAVE_DHKEY)) {
        return 0;
    }

    if (bt_mesh_atomic_test_and_clear_bit(prov_link.flags, SEND_CONFIRM)) {
        send_confirm();
    }

    return 0;
}

int bt_mesh_input_string(const char *str)
{
    BT_INFO("%s", str);

    if (!bt_mesh_atomic_test_and_clear_bit(prov_link.flags, WAIT_STRING)) {
        return -EINVAL;
    }

    (void)memcpy(prov_link.auth, str, bt_mesh_prov_get()->input_size);

    send_input_complete();

    if (!bt_mesh_atomic_test_bit(prov_link.flags, HAVE_DHKEY)) {
        return 0;
    }

    if (bt_mesh_atomic_test_and_clear_bit(prov_link.flags, SEND_CONFIRM)) {
        send_confirm();
    }

    return 0;
}

static void send_pub_key(void)
{
    const uint8_t *key = NULL;
    uint8_t dhkey[32] = {0};
    PROV_BUF(buf, 65);

    /* Copy remote key in little-endian for generating DHKey.
     * X and Y halves are swapped independently. Use response
     * buffer as a temporary storage location. The validating
     * of the remote public key is finished when it is received.
     */
    sys_memcpy_swap(buf.data, &prov_link.conf_inputs[17], 32);
    sys_memcpy_swap(&buf.data[32], &prov_link.conf_inputs[49], 32);

    if (bt_mesh_dh_key_gen(buf.data, dhkey)) {
        BT_ERR("Unable to generate DHKey");
        close_link(PROV_ERR_UNEXP_ERR);
        return;
    }

    sys_memcpy_swap(prov_link.dhkey, dhkey, 32);

    BT_DBG("DHkey: %s", bt_hex(prov_link.dhkey, 32));

    bt_mesh_atomic_set_bit(prov_link.flags, HAVE_DHKEY);

    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("No public key available");
        close_link(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("Local Public Key: %s", bt_hex(key, 64));

    bt_mesh_prov_buf_init(&buf, PROV_PUB_KEY);

    /* Swap X and Y halves independently to big-endian */
    sys_memcpy_swap(net_buf_simple_add(&buf, 32), key, 32);
    sys_memcpy_swap(net_buf_simple_add(&buf, 32), &key[32], 32);

    memcpy(&prov_link.conf_inputs[81], &buf.data[1], 64);

    if (bt_mesh_prov_send(&prov_link, &buf)) {
        BT_ERR("Failed to send Public Key");
        return;
    }

    prov_link.expect = PROV_CONFIRM;
}

static int bt_mesh_calc_dh_key(void)
{
    uint8_t pub_key[64] = {0};
    uint8_t dhkey[32] = {0};

    /* Copy remote key in little-endian for generating DHKey.
     * X and Y halves are swapped independently.
     */
    sys_memcpy_swap(&pub_key[0], &prov_link.conf_inputs[17], 32);
    sys_memcpy_swap(&pub_key[32], &prov_link.conf_inputs[49], 32);

    if (bt_mesh_dh_key_gen(pub_key, dhkey)) {
        BT_ERR("Unable to generate DHKey");
        close_link(PROV_ERR_UNEXP_ERR);
        return -EIO;
    }

    sys_memcpy_swap(prov_link.dhkey, dhkey, 32);

    BT_DBG("DHkey: %s", bt_hex(prov_link.dhkey, 32));

    bt_mesh_atomic_set_bit(prov_link.flags, HAVE_DHKEY);

    if (bt_mesh_atomic_test_bit(prov_link.flags, WAIT_NUMBER) ||
        bt_mesh_atomic_test_bit(prov_link.flags, WAIT_STRING)) {
        return 0;
    }

    if (bt_mesh_atomic_test_and_clear_bit(prov_link.flags, SEND_CONFIRM)) {
        send_confirm();
    }

    return 0;
}

int bt_mesh_set_oob_pub_key(const uint8_t pub_key_x[32],
                            const uint8_t pub_key_y[32],
                            const uint8_t pri_key[32])
{
    if (!pub_key_x || !pub_key_y || !pri_key) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Copy OOB public key in big-endian to Provisioning ConfirmationInputs,
     * X and Y halves are swapped independently.
     * And set input private key to mesh_bearer_adapt.c
     */
    sys_memcpy_swap(&prov_link.conf_inputs[81], pub_key_x, 32);
    sys_memcpy_swap(&prov_link.conf_inputs[81] + 32, pub_key_y, 32);
    bt_mesh_set_private_key(pri_key);

    bt_mesh_atomic_set_bit(prov_link.flags, OOB_PUB_KEY);

    /* If remote public key is not got, just return */
    if (!bt_mesh_atomic_test_bit(prov_link.flags, REMOTE_PUB_KEY)) {
        return 0;
    }

    return bt_mesh_calc_dh_key();
}

static void prov_pub_key(const uint8_t *data)
{
    BT_DBG("Remote Public Key: %s", bt_hex(data, 64));

    /* BLE Mesh BQB test case MESH/NODE/PROV/UPD/BI-13-C needs to
     * check the public key using the following rules:
     * (1) X > 0, Y > 0
     * (2) X > 0, Y = 0
     * (3) X = 0, Y = 0
     */
    if (!bt_mesh_check_public_key(data)) {
        BT_ERR("Invalid public key");
        close_link(PROV_ERR_UNEXP_PDU);
        return;
    }

    memcpy(&prov_link.conf_inputs[17], data, 64);
    bt_mesh_atomic_set_bit(prov_link.flags, REMOTE_PUB_KEY);

    if (prov_link.public_key == PROV_NO_OOB_PUB_KEY) {
        send_pub_key();
    } else {
        prov_link.expect = PROV_CONFIRM;
    }
}

static void prov_input_complete(const uint8_t *data)
{}

static void prov_confirm(const uint8_t *data)
{
    uint8_t conf_val_size = 0;

    conf_val_size = PROV_CONF_SIZE(&prov_link);

    BT_DBG("Remote Confirm: %s", bt_hex(data, conf_val_size));

    memcpy(prov_link.conf, data, conf_val_size);

    if (!bt_mesh_atomic_test_bit(prov_link.flags, HAVE_DHKEY)) {
#if CONFIG_BLE_MESH_PB_ADV
        bt_mesh_prov_clear_tx(&prov_link, true);
#endif
        bt_mesh_atomic_set_bit(prov_link.flags, SEND_CONFIRM);
        /* If using OOB public key and it has already got, calculates dhkey */
        if (prov_link.public_key == PROV_OOB_PUB_KEY &&
            bt_mesh_atomic_test_bit(prov_link.flags, OOB_PUB_KEY)) {
            bt_mesh_calc_dh_key();
        }
    } else {
        send_confirm();
    }
}

static void prov_random(const uint8_t *data)
{
    uint8_t conf_verify[32] = {0};
    uint8_t rand_val_size = 0;
    uint8_t conf_val_size = 0;

    rand_val_size = PROV_RAND_SIZE(&prov_link);
    conf_val_size = PROV_CONF_SIZE(&prov_link);
    PROV_BUF(rnd, rand_val_size);

    BT_INFO("Remote Random: %s", bt_hex(data, rand_val_size));

    if (prov_link.algorithm == PROV_ALG_P256_CMAC_AES128) {
        if (bt_mesh_prov_conf(prov_link.conf_key, data, prov_link.auth, conf_verify)) {
            BT_ERR("Unable to calculate confirmation verification");
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }
    }
#if CONFIG_BLE_MESH_PROV_EPA
    else {
        if (bt_mesh_prov_conf_epa(prov_link.conf_key, data, conf_verify)) {
            BT_ERR("Unable to calculate confirmation verification");
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }
    }
#endif

    if (memcmp(conf_verify, prov_link.conf, conf_val_size)) {
        BT_ERR("Invalid confirmation value");
        BT_ERR("Received:   %s", bt_hex(prov_link.conf, conf_val_size));
        BT_ERR("Calculated: %s",  bt_hex(conf_verify, conf_val_size));
        close_link(PROV_ERR_CFM_FAILED);
        return;
    }

    bt_mesh_prov_buf_init(&rnd, PROV_RANDOM);
    net_buf_simple_add_mem(&rnd, prov_link.rand, rand_val_size);

    if (bt_mesh_prov_send(&prov_link, &rnd)) {
        BT_ERR("Failed to send Provisioning Random");
        return;
    }

    if (prov_link.algorithm == PROV_ALG_P256_CMAC_AES128) {
        if (bt_mesh_prov_salt(prov_link.conf_salt, data, prov_link.rand,
                              prov_link.prov_salt)) {
            BT_ERR("Failed to generate provisioning salt");
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }
    }
#if CONFIG_BLE_MESH_PROV_EPA
    else {
        if (bt_mesh_prov_salt_epa(prov_link.conf_salt, data, prov_link.rand,
                                  prov_link.prov_salt)) {
            BT_ERR("Failed to generate provisioning salt");
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }
    }
#endif

    BT_DBG("ProvisioningSalt: %s", bt_hex(prov_link.prov_salt, 16));

    prov_link.expect = PROV_DATA;
}

static inline bool is_pb_gatt(void)
{
#if CONFIG_BLE_MESH_PB_GATT
    return !!prov_link.conn;
#else
    return false;
#endif
}

static void prov_data(const uint8_t *data)
{
    uint8_t session_key[16] = {0};
    bool identity_enable = false;
    uint8_t dev_key[16] = {0};
    uint8_t nonce[13] = {0};
    uint32_t iv_index = 0U;
    uint16_t net_idx = 0U;
    uint8_t pdu[25] = {0};
    uint16_t addr = 0U;
    uint8_t flags = 0U;
    PROV_BUF(msg, 1);
    int err = 0;

    err = bt_mesh_session_key(prov_link.dhkey, prov_link.prov_salt, session_key);
    if (err) {
        BT_ERR("Unable to generate session key");
        close_link(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("SessionKey: %s", bt_hex(session_key, 16));

    err = bt_mesh_prov_nonce(prov_link.dhkey, prov_link.prov_salt, nonce);
    if (err) {
        BT_ERR("Unable to generate session nonce");
        close_link(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("Nonce: %s", bt_hex(nonce, 13));

    err = bt_mesh_prov_decrypt(session_key, nonce, data, pdu);
    if (err) {
        BT_ERR("Unable to decrypt provisioning data");
        close_link(PROV_ERR_DECRYPT);
        return;
    }

    net_idx = sys_get_be16(&pdu[16]);
    flags = pdu[18];
    iv_index = sys_get_be32(&pdu[19]);
    addr = sys_get_be16(&pdu[23]);

    BT_DBG("net_idx %u iv_index 0x%08x, addr 0x%04x",
            net_idx, iv_index, addr);

#if CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_NPPI)) {
        uint8_t reason = 0;
        if (bt_mesh_rpr_srv_nppi_check(prov_link.pb_remote_nppi, pdu, net_idx,
                                       iv_index, addr, &reason) == false) {
            close_link(reason);
            return;
        }
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

    err = bt_mesh_dev_key(prov_link.dhkey, prov_link.prov_salt, dev_key);
    if (err) {
        BT_ERR("Unable to generate device key");
        close_link(PROV_ERR_UNEXP_ERR);
        return;
    }

    BT_DBG("DevKey: %s", bt_hex(dev_key, 16));

#if CONFIG_BLE_MESH_RPR_SRV
    /* Store NPPI data */
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_NPPI)) {
        err = bt_mesh_rpr_srv_store_nppi_data(prov_link.pb_remote_uuid,
                                              pdu, net_idx, flags,
                                              iv_index, addr, dev_key);
        if (err) {
            close_link(PROV_ERR_UNEXP_ERR);
            return;
        }
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

    bt_mesh_prov_buf_init(&msg, PROV_COMPLETE);

    if (bt_mesh_prov_send(&prov_link, &msg)) {
        BT_ERR("Failed to send Provisioning Complete");
        return;
    }

    /* Ignore any further PDUs on this link */
    prov_link.expect = 0U;

#if CONFIG_BLE_MESH_RPR_SRV
    /* For NPPI, no need to perform the following actions */
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_NPPI)) {
        return;
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

    /* Store info, since bt_mesh_provision() will end up clearing it */
    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
        identity_enable = is_pb_gatt();
    } else {
        identity_enable = false;
    }

    err = bt_mesh_provision(pdu, net_idx, flags, iv_index, addr, dev_key);
    if (err) {
        BT_ERR("Failed to provision (err %d)", err);
        return;
    }

    /* After PB-GATT provisioning we should start advertising
     * using Node Identity.
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) && identity_enable) {
        bt_mesh_proxy_identity_enable();
    }
}

static void prov_complete(const uint8_t *data)
{}

static void prov_failed(const uint8_t *data)
{
    BT_WARN("Error: 0x%02x", data[0]);

#if CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE)) {
        prov_link.pb_remote_cbd = true;
        /* In this case, no need to send Link Close */
        prov_link.pb_remote_reset = true;
        close_link(data[0]);
        return;
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */
}

static const struct {
    void (*func)(const uint8_t *data);
    bool pb_remote; /* Indicate if the PDU could be received when PB-Remote is used */
} prov_handlers[] = {
    { prov_invite,               false },
    { prov_capabilities,         true  },
    { prov_start,                false },
    { prov_pub_key,              true  },
    { prov_input_complete,       true  },
    { prov_confirm,              true  },
    { prov_random,               true  },
    { prov_data,                 false },
    { prov_complete,             true  },
    { prov_failed,               true  },
#if CONFIG_BLE_MESH_CERT_BASED_PROV
    { bt_mesh_prov_record_req,   false },
    { bt_mesh_prov_record_rsp,   false },
    { bt_mesh_prov_records_get,  false },
    { bt_mesh_prov_records_list, false },
#endif
};

#if CONFIG_BLE_MESH_PB_ADV
static void link_open(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (buf->len < 16) {
        BT_ERR("Too short bearer open message (len %u)", buf->len);
        return;
    }

    if (bt_mesh_atomic_test_bit(prov_link.flags, LINK_ACTIVE)) {
        /* Send another link ack if the provisioner missed the last */
        if (prov_link.link_id == rx->link_id && prov_link.expect == PROV_INVITE) {
            BT_DBG("Resending link ack");
            bt_mesh_prov_bearer_ctl_send(&prov_link, LINK_ACK, NULL, 0);
        } else {
            BT_INFO("Ignoring bearer open: link already active");
        }
        return;
    }

    if (memcmp(buf->data, bt_mesh_prov_get()->uuid, 16)) {
        BT_DBG("Bearer open message not for us");
        return;
    }

    if (bt_mesh_prov_get()->link_open) {
        bt_mesh_prov_get()->link_open(BLE_MESH_PROV_ADV);
    }

    prov_link.link_id = rx->link_id;

    bt_mesh_atomic_set_bit(prov_link.flags, LINK_ACTIVE);

    net_buf_simple_reset(prov_link.rx.buf);

#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
    /* Add the link id into exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_ADD,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID,
                                    &prov_link.link_id);
#endif

    bt_mesh_prov_bearer_ctl_send(&prov_link, LINK_ACK, NULL, 0);

#if CONFIG_BLE_MESH_CERT_BASED_PROV
    if (PROV_REC_SUPPORT(bt_mesh_prov_get()->oob_info)) {
        prov_link.expect = PROV_REC_EXP;
    } else
#endif
    {
        prov_link.expect = PROV_INVITE;
    }
}

static void link_ack(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

#if CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE)) {
        if (bt_mesh_atomic_test_and_clear_bit(prov_link.flags, PBR_OPENING)) {
            BT_INFO("PB-Remote, receive Link ACK");
            /* Cancel the retransmit timer, in case timeout is caused. */
            bt_mesh_prov_clear_tx(&prov_link, true);
            bt_mesh_rpr_srv_recv_link_ack(prov_link.pb_remote_uuid, true);
        } else {
            BT_INFO("Link ACK for PB-Remote already received");
        }
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */
}

static void link_close(struct prov_rx *rx, struct net_buf_simple *buf)
{
    uint8_t reason = 0;

    BT_DBG("len %u", buf->len);

    if (buf->len != 1) {
        BT_ERR("Invalid Link Close length %d", buf->len);
        return;
    }

    reason = net_buf_simple_pull_u8(buf);

#if CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE)) {
        prov_link.pb_remote_cbd = true;
        /* In this case, no need to send Link Close */
        prov_link.pb_remote_reset = true;
        close_link(reason);
        return;
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

    reset_adv_link(&prov_link, reason);
}

static void gen_prov_ctl(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("op 0x%02x len %u", BEARER_CTL(rx->gpc), buf->len);

    switch (BEARER_CTL(rx->gpc)) {
    case LINK_OPEN:
#if CONFIG_BLE_MESH_RPR_SRV
        if (bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE)) {
            return;
        }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

        link_open(rx, buf);
        break;

    case LINK_ACK:
        if (!bt_mesh_atomic_test_bit(prov_link.flags, LINK_ACTIVE)) {
            return;
        }

        link_ack(rx, buf);
        break;

    case LINK_CLOSE:
        if (!bt_mesh_atomic_test_bit(prov_link.flags, LINK_ACTIVE)) {
            return;
        }

        link_close(rx, buf);
        break;

    default:
        BT_ERR("Unknown bearer opcode: 0x%02x", BEARER_CTL(rx->gpc));
        return;
    }
}

static void prov_msg_recv(void)
{
    uint8_t err_code = 0;
    uint8_t type = 0;

    if (bt_mesh_atomic_test_bit(prov_link.flags, LINK_INVALID)) {
        BT_WARN("Unexpected msg 0x%02x on invalidated link", type);
        close_link(PROV_ERR_UNEXP_PDU);
        return;
    }

    /* When Link Close is being sent, any received Provisioning PDU
     * should be ignored.
     */
    if (bt_mesh_atomic_test_bit(prov_link.flags, LINK_CLOSING)) {
        BT_WARN("Link is closing, unexpected msg 0x%02x", type);
        return;
    }

    if (!bt_mesh_fcs_check(prov_link.rx.buf, prov_link.rx.fcs)) {
        BT_ERR("Incorrect FCS");
        return;
    }

    type = net_buf_simple_pull_u8(prov_link.rx.buf);

    BT_DBG("type 0x%02x len %u", type, prov_link.rx.buf->len);

    /* For case MESH/NODE/PROV/BI-15-C, when the node receive a Provisioning PDU
     * with the Type field set to the lowest unsupported or RFU value, it sends a
     * Provisioning Failed PDU with the Error Code field set to Invalid PDU(0x01).
     */
    if (type >= ARRAY_SIZE(prov_handlers)) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        prov_send_fail_msg(PROV_ERR_NVAL_PDU);
        return;
    }

    if (type != PROV_FAILED && type != prov_link.expect) {
#if CONFIG_BLE_MESH_CERT_BASED_PROV
        if (prov_link.expect == PROV_REC_EXP &&
            (type == PROV_INVITE || type == PROV_REC_GET || type == PROV_REC_REQ)) {
            if (prov_link.invite_recv) {
                BT_ERR("%s, Provisioning invite PDU already received", __func__);
                close_link(PROV_ERR_UNEXP_PDU);
                return;
            }
        } else
#endif
        {
            BT_WARN("Unexpected msg 0x%02x != 0x%02x", type, prov_link.expect);
            close_link(PROV_ERR_UNEXP_PDU);
            return;
        }
    }

    if (!bt_mesh_prov_pdu_check(type, prov_link.rx.buf->len, &err_code)) {
        close_link(err_code);
        return;
    }

    bt_mesh_gen_prov_ack_send(&prov_link, prov_link.rx.id);
    prov_link.rx.prev_id = prov_link.rx.id;
    prov_link.rx.id = 0U;

    k_delayed_work_submit(&prov_link.prot_timer, PROTOCOL_TIMEOUT);

#if CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE)) {
        if (prov_handlers[type].pb_remote) {
            BT_INFO("PB-Remote, receive prov pdu 0x%02x", type);
            /* Note:
             * Provisioning implementation should make sure that
             * no duplicate provisioning pdu is received by the
             * Remote Provisioning Server.
             */
            bt_mesh_rpr_srv_send_pdu_report(prov_link.pb_remote_uuid, type,
                                            prov_link.rx.buf);
        }
        return;
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

    prov_handlers[type].func(prov_link.rx.buf->data);
}

static void gen_prov_cont(struct prov_rx *rx, struct net_buf_simple *buf)
{
    bool close = false;

    if (!bt_mesh_gen_prov_cont(&prov_link, buf, rx, &close)) {
        if (close) {
            close_link(PROV_ERR_NVAL_FMT);
        }
        return;
    }

    prov_msg_recv();
}

static void gen_prov_ack(struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (!prov_link.tx.buf[0]) {
        return;
    }

#if CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE)) {
        if (prov_link.tx.id == 0) {
            return;
        }

        if (rx->xact_id == prov_link.tx.id - 1) {
            bt_mesh_prov_clear_tx(&prov_link, true);

            BT_INFO("PB-Remote, receive Transaction ACK");
            bt_mesh_rpr_srv_send_outbound_report(prov_link.pb_remote_uuid, true);
        }
        return;
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

    if (rx->xact_id == prov_link.tx.id) {
        bt_mesh_prov_clear_tx(&prov_link, true);
    }
}

static void gen_prov_start(struct prov_rx *rx, struct net_buf_simple *buf)
{
    bool close = false;

    if (!bt_mesh_gen_prov_start(&prov_link, buf, rx, &close)) {
        if (close) {
            close_link(PROV_ERR_NVAL_FMT);
        }
        return;
    }

    prov_msg_recv();
}

static const struct {
    void (*func)(struct prov_rx *rx, struct net_buf_simple *buf);
    bool require_link;
    uint8_t min_len;
} gen_prov[] = {
    { gen_prov_start, true,  3 },
    { gen_prov_ack,   true,  0 },
    { gen_prov_cont,  true,  0 },
    { gen_prov_ctl,   false, 0 },
};

static void gen_prov_recv(struct prov_rx *rx, struct net_buf_simple *buf)
{
    if (buf->len < gen_prov[GPCF(rx->gpc)].min_len) {
        BT_ERR("Too short GPC message type %u", GPCF(rx->gpc));
        return;
    }

    if (!bt_mesh_atomic_test_bit(prov_link.flags, LINK_ACTIVE) &&
        gen_prov[GPCF(rx->gpc)].require_link) {
        BT_DBG("Ignoring message that requires active link");
        return;
    }

    gen_prov[GPCF(rx->gpc)].func(rx, buf);
}

void bt_mesh_pb_adv_recv(struct net_buf_simple *buf)
{
    struct prov_rx rx = {0};

    if (!bt_mesh_atomic_test_bit(prov_link.flags, LINK_ACTIVE) &&
        bt_mesh_is_provisioned()) {
        BT_DBG("Ignoring provisioning PDU - already provisioned");
        return;
    }

    if (buf->len < 6) {
        BT_WARN("Too short provisioning packet (len %u)", buf->len);
        return;
    }

    rx.link_id = net_buf_simple_pull_be32(buf);
    rx.xact_id = net_buf_simple_pull_u8(buf);
    rx.gpc = net_buf_simple_pull_u8(buf);

    BT_DBG("link_id 0x%08x xact_id %u", rx.link_id, rx.xact_id);

    if (bt_mesh_atomic_test_bit(prov_link.flags, LINK_ACTIVE) &&
        prov_link.link_id != rx.link_id) {
        BT_DBG("Ignoring mesh beacon for unknown link");
        return;
    }

    gen_prov_recv(&rx, buf);
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
int bt_mesh_pb_gatt_recv(struct bt_mesh_conn *conn, struct net_buf_simple *buf)
{
    uint8_t err_code = 0U;
    uint8_t type = 0U;

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    if (prov_link.conn != conn) {
        BT_WARN("Data for unexpected connection");
        return -ENOTCONN;
    }

    if (buf->len < 1) {
        BT_WARN("Too short provisioning packet (len %u)", buf->len);
        return -EINVAL;
    }

    /* For case MESH/NODE/PROV/BI-03-C, if the link is closed, when the node receive
     * a Provisioning PDU , it will send a Provisioning Failed PDU with the Error Code
     * field set to Unexpected PDU(0x03).
     */
    if (bt_mesh_atomic_test_bit(prov_link.flags, LINK_INVALID)) {
        BT_WARN("Unexpected msg 0x%02x on invalid link", type);
        prov_send_fail_msg(PROV_ERR_UNEXP_PDU);
        return -EINVAL;
    }

    /* For case MESH/NODE/PROV/BI-15-C, when the node receive a Provisioning PDU
     * with the Type field set to the lowest unsupported or RFU value, it sends a
     * Provisioning Failed PDU with the Error Code field set to Invalid PDU(0x01).
     */
    type = net_buf_simple_pull_u8(buf);
    if (type >= ARRAY_SIZE(prov_handlers)) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        prov_send_fail_msg(PROV_ERR_NVAL_PDU);
        return -EINVAL;
    }

    if (type != PROV_FAILED && type != prov_link.expect) {
#if CONFIG_BLE_MESH_CERT_BASED_PROV
        if (prov_link.expect == PROV_REC_EXP &&
            (type == PROV_INVITE || type == PROV_REC_GET || type == PROV_REC_REQ)) {
            if (prov_link.invite_recv) {
                BT_ERR("%s, Provisioning invite PDU already received", __func__);
                close_link(PROV_ERR_UNEXP_PDU);
                return -EINVAL;
            }
        } else
#endif
        {
            BT_WARN("Unexpected msg 0x%02x != 0x%02x", type, prov_link.expect);
            close_link(PROV_ERR_UNEXP_PDU);
            return -EINVAL;
        }
    }

    if (!bt_mesh_prov_pdu_check(type, buf->len, &err_code)) {
        close_link(err_code);
        return -EINVAL;
    }

    k_delayed_work_submit(&prov_link.prot_timer, PROTOCOL_TIMEOUT);

#if CONFIG_BLE_MESH_RPR_SRV
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE)) {
        if (prov_handlers[type].pb_remote) {
            BT_INFO("PB-Remote, receive prov pdu 0x%02x by GATT %s",
                    type, bt_hex(buf->data, buf->len));
            /* Note:
             * Provisioning implementation should make sure that
             * no duplicate provisioning pdu is received by the
             * Remote Provisioning Server.
             */
            bt_mesh_rpr_srv_send_pdu_report(prov_link.pb_remote_uuid, type, buf);
        }
        return 0;
    }
#endif /* CONFIG_BLE_MESH_RPR_SRV */

    prov_handlers[type].func(buf->data);

    return 0;
}

int bt_mesh_pb_gatt_open(struct bt_mesh_conn *conn)
{
    BT_DBG("conn %p", conn);

    if (!bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE) &&
        bt_mesh_atomic_test_and_set_bit(prov_link.flags, LINK_ACTIVE)) {
        BT_ERR("Link is busy");
        return -EBUSY;
    }

    prov_link.conn = bt_mesh_conn_ref(conn);

#if CONFIG_BLE_MESH_CERT_BASED_PROV
    if (PROV_REC_SUPPORT(bt_mesh_prov_get()->oob_info)) {
        prov_link.expect = PROV_REC_EXP;
    } else
#endif
    {
        prov_link.expect = PROV_INVITE;
    }

    if (bt_mesh_prov_get()->link_open) {
        bt_mesh_prov_get()->link_open(BLE_MESH_PROV_GATT);
    }

    return 0;
}

int bt_mesh_pb_gatt_close(struct bt_mesh_conn *conn, uint8_t reason)
{
    BT_DBG("conn %p", conn);

    if (prov_link.conn != conn) {
        BT_ERR("Not connected");
        return -ENOTCONN;
    }

#if CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT
    if (bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE)) {
        prov_link.pb_remote_cbd = true;
        prov_link.pb_remote_reset = true;
        prov_link.pb_remote_close(&prov_link, reason);
    }
#endif

    if (bt_mesh_prov_get()->link_close) {
        bt_mesh_prov_get()->link_close(BLE_MESH_PROV_GATT, reason);
    }

    reset_state();

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

bool bt_mesh_prov_active(void)
{
    return bt_mesh_atomic_test_bit(prov_link.flags, LINK_ACTIVE);
}

static void protocol_timeout(struct k_work *work)
{
    BT_WARN("Protocol timeout");

#if CONFIG_BLE_MESH_PB_GATT
    if (prov_link.conn) {
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT && CONFIG_BLE_MESH_RPR_SRV
        if (bt_mesh_atomic_test_bit(prov_link.flags, PB_REMOTE)) {
            prov_link.pb_remote_reset = true;
            bt_mesh_gattc_disconnect(prov_link.conn);
            return;
        }
#endif
        bt_mesh_pb_gatt_close(prov_link.conn, CLOSE_REASON_TIMEOUT);
        return;
    }
#endif /* CONFIG_BLE_MESH_PB_GATT */

#if CONFIG_BLE_MESH_PB_ADV
    uint8_t reason = CLOSE_REASON_TIMEOUT;
    prov_link.rx.seg = 0U;
    bt_mesh_prov_bearer_ctl_send(&prov_link, LINK_CLOSE, &reason, sizeof(reason));
#endif /* CONFIG_BLE_MESH_PB_ADV */
}

int bt_mesh_prov_init(void)
{
    const uint8_t *key = NULL;

    if (bt_mesh_prov_get() == NULL) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    if (bt_mesh_prov_get()->static_val_len > BLE_MESH_PROV_STATIC_OOB_MAX_LEN ||
        bt_mesh_prov_get()->output_size > BLE_MESH_PROV_OUTPUT_OOB_MAX_LEN ||
        bt_mesh_prov_get()->input_size > BLE_MESH_PROV_INPUT_OOB_MAX_LEN) {
        BT_ERR("Invalid authentication oob length");
        return -EINVAL;
    }

    __ASSERT(bt_mesh_prov_get()->uuid, "Device UUID not initialized");

    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("Failed to generate public key");
        return -EIO;
    }

    k_delayed_work_init(&prov_link.prot_timer, protocol_timeout);

#if CONFIG_BLE_MESH_PB_ADV
    bt_mesh_prov_retransmit_init(&prov_link);
#endif

    reset_state();

#if CONFIG_BLE_MESH_PB_ADV
    bt_mesh_mutex_create(&prov_link.buf_lock);
#endif

#if CONFIG_BLE_MESH_CERT_BASED_PROV
    if (PROV_REC_SUPPORT(bt_mesh_prov_get()->oob_info)) {
        int err = bt_mesh_node_cert_based_prov_init();
        if (err) {
            BT_ERR("Loading provisioning records failed!");
            return err;
        }
    }
#endif /* CONFIG_BLE_MESH_CERT_BASED_PROV */

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
int bt_mesh_prov_deinit(void)
{
    if (bt_mesh_prov_get() == NULL) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    k_delayed_work_free(&prov_link.prot_timer);

#if CONFIG_BLE_MESH_PB_ADV
    bt_mesh_prov_clear_tx(&prov_link, true);
    k_delayed_work_free(&prov_link.tx.retransmit);
#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
    /* Remove the link id from exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_REMOVE,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID,
                                    &prov_link.link_id);
#endif /* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
    bt_mesh_mutex_free(&prov_link.buf_lock);
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_CERT_BASED_PROV
    if (PROV_REC_SUPPORT(bt_mesh_prov_get()->oob_info)) {
        bt_mesh_node_cert_based_prov_deinit();
    }
#endif /* CONFIG_BLE_MESH_CERT_BASED_PROV */

    (void)memset(&prov_link, 0, sizeof(prov_link));

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

void bt_mesh_prov_complete(uint16_t net_idx, const uint8_t net_key[16],
                           uint16_t addr, uint8_t flags, uint32_t iv_index)
{
    if (bt_mesh_prov_get()->complete) {
        bt_mesh_prov_get()->complete(net_idx, net_key, addr, flags, iv_index);
    }
}

void bt_mesh_prov_reset(void)
{
    if (bt_mesh_prov_get()->reset) {
        bt_mesh_prov_get()->reset();
    }
}

#if CONFIG_BLE_MESH_RPR_SRV
void bt_mesh_rpr_srv_reset_prov_link(struct bt_mesh_prov_link *link, uint8_t reason)
{
#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
    /* Remove the link id from exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_REMOVE,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID,
                                    &link->link_id);
#endif

    reset_state();
}

int bt_mesh_rpr_srv_nppi_pdu_recv(uint8_t type, const uint8_t *data)
{
    if (!bt_mesh_atomic_test_bit(prov_link.flags, PB_NPPI)) {
        BT_ERR("Not a NPPI provisioning link");
        return -EINVAL;
    }

    if (type != prov_link.expect) {
        BT_ERR("NPPI, unexpected msg 0x%02x != 0x%02x", type, prov_link.expect);
        return -EINVAL;
    }

    prov_handlers[type].func(data);
    return 0;
}
#endif /* CONFIG_BLE_MESH_RPR_SRV */

#endif /* CONFIG_BLE_MESH_NODE */
