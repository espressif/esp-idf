/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "crypto.h"
#include "mesh/config.h"
#include "mesh/common.h"
#include "mesh/adapter.h"

#if CONFIG_BLE_MESH_V11_SUPPORT
#include "mesh_v1.1/utils.h"
#endif /* CONFIG_BLE_MESH_V11_SUPPORT */

#define NET_MIC_LEN(pdu)    (((pdu)[1] & 0x80) ? 8 : 4)
#define APP_MIC_LEN(aszmic) ((aszmic) ? 8 : 4)

/* bt_mesh_aes_cmac is now mapped to bt_mesh_aes_cmac_raw_key from common crypto module */

int bt_mesh_k1(const uint8_t *ikm, size_t ikm_len, const uint8_t salt[16],
               const char *info, uint8_t okm[16])
{
    int err = 0;

    err = bt_mesh_aes_cmac_one(salt, ikm, ikm_len, okm);
    if (err < 0) {
        return err;
    }

    return bt_mesh_aes_cmac_one(okm, info, strlen(info), okm);
}

int bt_mesh_k2(const uint8_t n[16], const uint8_t *p, size_t p_len,
               uint8_t net_id[1], uint8_t enc_key[16], uint8_t priv_key[16])
{
    struct bt_mesh_sg sg[3] = {0};
    uint8_t salt[16] = {0};
    uint8_t out[16] = {0};
    uint8_t t[16] = {0};
    uint8_t pad = 0U;
    int err = 0;

    BT_DBG("n %s", bt_hex(n, 16));
    BT_DBG("p %s", bt_hex(p, p_len));

    err = bt_mesh_s1("smk2", salt);
    if (err) {
        return err;
    }

    err = bt_mesh_aes_cmac_one(salt, n, 16, t);
    if (err) {
        return err;
    }

    pad = 0x01;

    sg[0].data = NULL;
    sg[0].len  = 0;
    sg[1].data = p;
    sg[1].len  = p_len;
    sg[2].data = &pad;
    sg[2].len  = sizeof(pad);

    err = bt_mesh_aes_cmac(t, sg, ARRAY_SIZE(sg), out);
    if (err) {
        return err;
    }

    net_id[0] = out[15] & 0x7f;

    sg[0].data = out;
    sg[0].len  = sizeof(out);
    pad = 0x02;

    err = bt_mesh_aes_cmac(t, sg, ARRAY_SIZE(sg), out);
    if (err) {
        return err;
    }

    memcpy(enc_key, out, 16);

    pad = 0x03;

    err = bt_mesh_aes_cmac(t, sg, ARRAY_SIZE(sg), out);
    if (err) {
        return err;
    }

    memcpy(priv_key, out, 16);

    BT_DBG("NID 0x%02x enc_key %s", net_id[0], bt_hex(enc_key, 16));
    BT_DBG("priv_key %s", bt_hex(priv_key, 16));

    return 0;
}

int bt_mesh_k3(const uint8_t n[16], uint8_t out[8])
{
    uint8_t id64[] = { 'i', 'd', '6', '4', 0x01 };
    uint8_t tmp[16] = {0};
    uint8_t t[16] = {0};
    int err = 0;

    err = bt_mesh_s1("smk3", tmp);
    if (err) {
        return err;
    }

    err = bt_mesh_aes_cmac_one(tmp, n, 16, t);
    if (err) {
        return err;
    }

    err = bt_mesh_aes_cmac_one(t, id64, sizeof(id64), tmp);
    if (err) {
        return err;
    }

    memcpy(out, tmp + 8, 8);

    return 0;
}

int bt_mesh_k4(const uint8_t n[16], uint8_t out[1])
{
    uint8_t id6[] = { 'i', 'd', '6', 0x01 };
    uint8_t tmp[16] = {0};
    uint8_t t[16] = {0};
    int err = 0;

    err = bt_mesh_s1("smk4", tmp);
    if (err) {
        return err;
    }

    err = bt_mesh_aes_cmac_one(tmp, n, 16, t);
    if (err) {
        return err;
    }

    err = bt_mesh_aes_cmac_one(t, id6, sizeof(id6), tmp);
    if (err) {
        return err;
    }

    out[0] = tmp[15] & BIT_MASK(6);

    return 0;
}

int bt_mesh_id128(const uint8_t n[16], const char *s, uint8_t out[16])
{
    const char *id128 = "id128\x01";
    uint8_t salt[16] = {0};
    int err = 0;

    err = bt_mesh_s1(s, salt);
    if (err) {
        return err;
    }

    return bt_mesh_k1(n, 16, salt, id128, out);
}

/* Use the unified crypto module for CCM operations */
#define bt_mesh_ccm_decrypt bt_mesh_ccm_decrypt_raw_key
#define bt_mesh_ccm_encrypt bt_mesh_ccm_encrypt_raw_key

#if CONFIG_BLE_MESH_PROXY
static void create_proxy_nonce(uint8_t nonce[13], const uint8_t *pdu,
                               uint32_t iv_index)
{
    /* Nonce Type */
    nonce[0] = 0x03;

    /* Pad */
    nonce[1] = 0x00;

    /* Sequence Number */
    nonce[2] = pdu[2];
    nonce[3] = pdu[3];
    nonce[4] = pdu[4];

    /* Source Address */
    nonce[5] = pdu[5];
    nonce[6] = pdu[6];

    /* Pad */
    nonce[7] = 0U;
    nonce[8] = 0U;

    /* IV Index */
    sys_put_be32(iv_index, &nonce[9]);
}
#endif /* CONFIG_BLE_MESH_PROXY */

static void create_net_nonce(uint8_t nonce[13], const uint8_t *pdu,
                             uint32_t iv_index)
{
    /* Nonce Type */
    nonce[0] = 0x00;

    /* FRND + TTL */
    nonce[1] = pdu[1];

    /* Sequence Number */
    nonce[2] = pdu[2];
    nonce[3] = pdu[3];
    nonce[4] = pdu[4];

    /* Source Address */
    nonce[5] = pdu[5];
    nonce[6] = pdu[6];

    /* Pad */
    nonce[7] = 0U;
    nonce[8] = 0U;

    /* IV Index */
    sys_put_be32(iv_index, &nonce[9]);
}

int bt_mesh_net_obfuscate(uint8_t *pdu, uint32_t iv_index,
                          const uint8_t privacy_key[16])
{
    uint8_t priv_rand[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, };
    uint8_t tmp[16] = {0};
    int err = 0, i;

    BT_DBG("IVIndex %lu PrivacyKey %s", iv_index, bt_hex(privacy_key, 16));

    sys_put_be32(iv_index, &priv_rand[5]);
    memcpy(&priv_rand[9], &pdu[7], 7);

    BT_DBG("PrivacyRandom %s", bt_hex(priv_rand, 16));

    err = bt_mesh_encrypt_be(privacy_key, priv_rand, tmp);
    if (err) {
        BT_ERR("NetObfuscateFailed (%d)", err);
        return err;
    }

    for (i = 0; i < 6; i++) {
        pdu[1 + i] ^= tmp[i];
    }

    return 0;
}

int bt_mesh_net_encrypt(const uint8_t key[16], struct net_buf_simple *buf,
                        uint32_t iv_index, bool proxy, bool proxy_solic)
{
    uint8_t mic_len = NET_MIC_LEN(buf->data);
    uint8_t nonce[13] = {0};
    int err = 0;

    BT_DBG("IVIndex %u EncKey %s MicLen %u proxy %u/%u",
           iv_index, bt_hex(key, 16), mic_len, proxy, proxy_solic);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

#if CONFIG_BLE_MESH_PROXY
    if (proxy) {
#if CONFIG_BLE_MESH_PROXY_SOLIC
        if (proxy_solic) {
            bt_mesh_create_proxy_solic_nonce(nonce, buf->data, iv_index);
        } else
#endif /* CONFIG_BLE_MESH_PROXY_SOLIC */
        {
            create_proxy_nonce(nonce, buf->data, iv_index);
        }
    } else {
        create_net_nonce(nonce, buf->data, iv_index);
    }
#else /* CONFIG_BLE_MESH_PROXY */
    create_net_nonce(nonce, buf->data, iv_index);
#endif /* CONFIG_BLE_MESH_PROXY */

    BT_DBG("Nonce %s", bt_hex(nonce, 13));

    err = bt_mesh_ccm_encrypt(key, nonce, &buf->data[7], buf->len - 7,
                              NULL, 0, &buf->data[7], mic_len);
    if (!err) {
        net_buf_simple_add(buf, mic_len);
    } else {
        BT_ERR("NetEncryptFailed (%d)", err);
    }

    return err;
}

int bt_mesh_net_decrypt(const uint8_t key[16], struct net_buf_simple *buf,
                        uint32_t iv_index, bool proxy, bool proxy_solic)
{
    uint8_t mic_len = NET_MIC_LEN(buf->data);
    uint8_t nonce[13] = {0};
    int err;

    BT_DBG("IVIndex %u EncKey %s MicLen %u proxy %u/%u",
           iv_index, bt_hex(key, 16), mic_len, proxy, proxy_solic);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

#if CONFIG_BLE_MESH_PROXY
    if (proxy) {
#if CONFIG_BLE_MESH_PROXY_SOLIC
        if (proxy_solic) {
            bt_mesh_create_proxy_solic_nonce(nonce, buf->data, iv_index);
        } else
#endif /* CONFIG_BLE_MESH_PROXY_SOLIC */
        {
            create_proxy_nonce(nonce, buf->data, iv_index);
        }
    } else {
        create_net_nonce(nonce, buf->data, iv_index);
    }
#else /* CONFIG_BLE_MESH_PROXY */
    create_net_nonce(nonce, buf->data, iv_index);
#endif /* CONFIG_BLE_MESH_PROXY */

    BT_DBG("Nonce %s", bt_hex(nonce, 13));

    if (buf->len < 8 + mic_len) {
        BT_ERR("TooShortMsgToDecrypt[%u][%u]", buf->len, mic_len);
        return -EINVAL;
    }

    buf->len -= mic_len;

    err = bt_mesh_ccm_decrypt(key, nonce, &buf->data[7], buf->len - 7,
                              NULL, 0, &buf->data[7], mic_len);
    if (err) {
        BT_ERR("NetDecrypt failed (%d)", err);
    }

    return err;
}

static void create_app_nonce(uint8_t nonce[13], bool dev_key, uint8_t aszmic,
                             uint16_t src, uint16_t dst, uint32_t seq_num,
                             uint32_t iv_index)
{
    if (dev_key) {
        nonce[0] = 0x02;
    } else {
        nonce[0] = 0x01;
    }

    sys_put_be32((seq_num | ((uint32_t)aszmic << 31)), &nonce[1]);

    sys_put_be16(src, &nonce[5]);
    sys_put_be16(dst, &nonce[7]);

    sys_put_be32(iv_index, &nonce[9]);
}

int bt_mesh_app_encrypt(const uint8_t key[16], bool dev_key, uint8_t aszmic,
                        struct net_buf_simple *buf, const uint8_t *ad,
                        uint16_t src, uint16_t dst, uint32_t seq_num, uint32_t iv_index)
{
    uint8_t nonce[13] = {0};
    int err = 0;

    BT_DBG("AppEncrypt");
    BT_DBG("AppKey %s", bt_hex(key, 16));
    BT_DBG("DevKey %u Src 0x%04x Dst 0x%04x", dev_key, src, dst);
    BT_DBG("SeqNum 0x%08lx IVIndex 0x%08lx", seq_num, iv_index);
    BT_DBG("Data %u %s", buf->len, bt_hex(buf->data, buf->len));

    create_app_nonce(nonce, dev_key, aszmic, src, dst, seq_num, iv_index);

    BT_DBG("Nonce %s", bt_hex(nonce, 13));

    err = bt_mesh_ccm_encrypt(key, nonce, buf->data, buf->len, ad,
                              ad ? 16 : 0, buf->data, APP_MIC_LEN(aszmic));
    if (!err) {
        net_buf_simple_add(buf, APP_MIC_LEN(aszmic));
        BT_DBG("Encr: %s", bt_hex(buf->data, buf->len));
    }

    return err;
}

int bt_mesh_app_decrypt(const uint8_t key[16], bool dev_key, uint8_t aszmic,
                        struct net_buf_simple *buf, struct net_buf_simple *out,
                        const uint8_t *ad, uint16_t src, uint16_t dst, uint32_t seq_num,
                        uint32_t iv_index)
{
    uint8_t nonce[13] = {0};
    int err = 0;

    BT_DBG("EncData %u %s", buf->len, bt_hex(buf->data, buf->len));

    create_app_nonce(nonce, dev_key, aszmic, src, dst, seq_num, iv_index);

    BT_DBG("AppKey %s", bt_hex(key, 16));
    BT_DBG("Nonce %s", bt_hex(nonce, 13));

    err = bt_mesh_ccm_decrypt(key, nonce, buf->data, buf->len, ad,
                              ad ? 16 : 0, out->data, APP_MIC_LEN(aszmic));
    if (!err) {
        net_buf_simple_add(out, buf->len);
    }

    return err;
}

/* reversed, 8-bit, poly=0x07 */
static const uint8_t crc_table[256] = {
    0x00, 0x91, 0xe3, 0x72, 0x07, 0x96, 0xe4, 0x75,
    0x0e, 0x9f, 0xed, 0x7c, 0x09, 0x98, 0xea, 0x7b,
    0x1c, 0x8d, 0xff, 0x6e, 0x1b, 0x8a, 0xf8, 0x69,
    0x12, 0x83, 0xf1, 0x60, 0x15, 0x84, 0xf6, 0x67,

    0x38, 0xa9, 0xdb, 0x4a, 0x3f, 0xae, 0xdc, 0x4d,
    0x36, 0xa7, 0xd5, 0x44, 0x31, 0xa0, 0xd2, 0x43,
    0x24, 0xb5, 0xc7, 0x56, 0x23, 0xb2, 0xc0, 0x51,
    0x2a, 0xbb, 0xc9, 0x58, 0x2d, 0xbc, 0xce, 0x5f,

    0x70, 0xe1, 0x93, 0x02, 0x77, 0xe6, 0x94, 0x05,
    0x7e, 0xef, 0x9d, 0x0c, 0x79, 0xe8, 0x9a, 0x0b,
    0x6c, 0xfd, 0x8f, 0x1e, 0x6b, 0xfa, 0x88, 0x19,
    0x62, 0xf3, 0x81, 0x10, 0x65, 0xf4, 0x86, 0x17,

    0x48, 0xd9, 0xab, 0x3a, 0x4f, 0xde, 0xac, 0x3d,
    0x46, 0xd7, 0xa5, 0x34, 0x41, 0xd0, 0xa2, 0x33,
    0x54, 0xc5, 0xb7, 0x26, 0x53, 0xc2, 0xb0, 0x21,
    0x5a, 0xcb, 0xb9, 0x28, 0x5d, 0xcc, 0xbe, 0x2f,

    0xe0, 0x71, 0x03, 0x92, 0xe7, 0x76, 0x04, 0x95,
    0xee, 0x7f, 0x0d, 0x9c, 0xe9, 0x78, 0x0a, 0x9b,
    0xfc, 0x6d, 0x1f, 0x8e, 0xfb, 0x6a, 0x18, 0x89,
    0xf2, 0x63, 0x11, 0x80, 0xf5, 0x64, 0x16, 0x87,

    0xd8, 0x49, 0x3b, 0xaa, 0xdf, 0x4e, 0x3c, 0xad,
    0xd6, 0x47, 0x35, 0xa4, 0xd1, 0x40, 0x32, 0xa3,
    0xc4, 0x55, 0x27, 0xb6, 0xc3, 0x52, 0x20, 0xb1,
    0xca, 0x5b, 0x29, 0xb8, 0xcd, 0x5c, 0x2e, 0xbf,

    0x90, 0x01, 0x73, 0xe2, 0x97, 0x06, 0x74, 0xe5,
    0x9e, 0x0f, 0x7d, 0xec, 0x99, 0x08, 0x7a, 0xeb,
    0x8c, 0x1d, 0x6f, 0xfe, 0x8b, 0x1a, 0x68, 0xf9,
    0x82, 0x13, 0x61, 0xf0, 0x85, 0x14, 0x66, 0xf7,

    0xa8, 0x39, 0x4b, 0xda, 0xaf, 0x3e, 0x4c, 0xdd,
    0xa6, 0x37, 0x45, 0xd4, 0xa1, 0x30, 0x42, 0xd3,
    0xb4, 0x25, 0x57, 0xc6, 0xb3, 0x22, 0x50, 0xc1,
    0xba, 0x2b, 0x59, 0xc8, 0xbd, 0x2c, 0x5e, 0xcf
};

uint8_t bt_mesh_fcs_calc(const uint8_t *data, uint8_t data_len)
{
    uint8_t fcs = 0xff;

    while (data_len--) {
        fcs = crc_table[fcs ^ *data++];
    }

    BT_DBG("fcs 0x%02x", 0xff - fcs);

    return 0xff - fcs;
}

bool bt_mesh_fcs_check(struct net_buf_simple *buf, uint8_t received_fcs)
{
    const uint8_t *data = buf->data;
    uint16_t data_len = buf->len;
    uint8_t fcs = 0xff;

    while (data_len--) {
        fcs = crc_table[fcs ^ *data++];
    }

    return crc_table[fcs ^ received_fcs] == 0xcf;
}

int bt_mesh_virtual_addr(const uint8_t virtual_label[16], uint16_t *addr)
{
    uint8_t salt[16] = {0};
    uint8_t tmp[16] = {0};
    int err = 0;

    err = bt_mesh_s1("vtad", salt);
    if (err) {
        return err;
    }

    err = bt_mesh_aes_cmac_one(salt, virtual_label, 16, tmp);
    if (err) {
        return err;
    }

    *addr = (sys_get_be16(&tmp[14]) & 0x3fff) | 0x8000;

    return 0;
}

int bt_mesh_prov_conf_salt(const uint8_t conf_inputs[145], uint8_t salt[16])
{
    const uint8_t conf_salt_key[16] = { 0 };

    return bt_mesh_aes_cmac_one(conf_salt_key, conf_inputs, 145, salt);
}

int bt_mesh_prov_conf_key(const uint8_t dhkey[32], const uint8_t conf_salt[16],
                          uint8_t conf_key[16])
{
    return bt_mesh_k1(dhkey, 32, conf_salt, "prck", conf_key);
}

int bt_mesh_prov_conf(const uint8_t conf_key[16], const uint8_t rand[16],
                      const uint8_t auth[16], uint8_t conf[16])
{
    struct bt_mesh_sg sg[] = { { rand, 16 }, { auth, 16 } };

    BT_DBG("ConfirmationKey %s", bt_hex(conf_key, 16));
    BT_DBG("RandomDevice %s", bt_hex(rand, 16));
    BT_DBG("AuthValue %s", bt_hex(auth, 16));

    return bt_mesh_aes_cmac(conf_key, sg, ARRAY_SIZE(sg), conf);
}

int bt_mesh_prov_decrypt(const uint8_t key[16], uint8_t nonce[13],
                         const uint8_t data[25 + 8], uint8_t out[25])
{
    return bt_mesh_ccm_decrypt(key, nonce, data, 25, NULL, 0, out, 8);
}

#if CONFIG_BLE_MESH_PROVISIONER
int bt_mesh_prov_encrypt(const uint8_t key[16], uint8_t nonce[13],
                         const uint8_t data[25], uint8_t out[33])
{
    return bt_mesh_ccm_encrypt(key, nonce, data, 25, NULL, 0, out, 8);
}
#endif

int bt_mesh_secure_beacon_auth(const uint8_t beacon_key[16], uint8_t flags,
                               const uint8_t net_id[8], uint32_t iv_index,
                               uint8_t auth[8])
{
    uint8_t msg[13] = {0}, tmp[16] = {0};
    int err = 0;

    BT_DBG("BeaconKey %s", bt_hex(beacon_key, 16));
    BT_DBG("NetId %s", bt_hex(net_id, 8));
    BT_DBG("IV Index 0x%08x", iv_index);

    msg[0] = flags;
    memcpy(&msg[1], net_id, 8);
    sys_put_be32(iv_index, &msg[9]);

    BT_DBG("BeaconMsg %s", bt_hex(msg, sizeof(msg)));

    err = bt_mesh_aes_cmac_one(beacon_key, msg, sizeof(msg), tmp);
    if (!err) {
        memcpy(auth, tmp, 8);
    }

    return err;
}
