/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include "unity.h"
#include <string.h>
#include "utils/common.h"
#include "utils/includes.h"
#include "crypto/crypto.h"
#include "common/defs.h"
#include "common/dpp.h"
#include "sdkconfig.h"
#include "test_wpa_supplicant_common.h"

#ifdef CONFIG_ESP_WIFI_TESTING_OPTIONS
struct dpp_global {
    void *msg_ctx;
    struct dl_list bootstrap; /* struct dpp_bootstrap_info */
    struct dl_list configurator; /* struct dpp_configurator */
};

extern u8 dpp_protocol_key_override[600];
extern size_t dpp_protocol_key_override_len;
extern u8 dpp_nonce_override[DPP_MAX_NONCE_LEN];
extern size_t dpp_nonce_override_len;
#define MAX_FRAME_SIZE 1200

TEST_CASE("Test vectors DPP responder p256", "[wpa_dpp]")
{
    set_leak_threshold(120);
    /* Global variables */
    char command[1200] = {0};
    const u8 *frame;
    int len = 0;
    struct dpp_authentication *auth_instance = NULL;
    u8 auth[MAX_FRAME_SIZE] = {0};
    char prefix[] = "30310201010420";
    char postfix[] = "a00a06082a8648ce3d030107";
    size_t hex_len;
    int ret = 0;
    int id;

    /* DPP global config initialization */
    struct dpp_global_config dpp_conf;
    memset(&dpp_conf, 0, sizeof(dpp_conf));
    struct dpp_global *dpp = dpp_global_init(&dpp_conf);

    /* bootstrap generation test */
    ESP_LOGI("DPP Test", "bootstrap generation test");
    {
        char key[1000] = {0};
        const char *uri;

        char private_bootstrap_key[] = "54ce181a98525f217216f59b245f60e9df30ac7f6b26c939418cfc3c42d1afa0";
        char bootstrap_info[] = "DPP:K:MDkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDIgACCcWFqRtN+f0loEUgGIXDnMXPrjl92u2pV97Ff6DjUD8=;;";

        sprintf(key, "%s%s%s", prefix, private_bootstrap_key, postfix);

        sprintf(command, "type=qrcode key=%s", key);
        id = dpp_bootstrap_gen(dpp, command);
        uri =  dpp_bootstrap_get_uri(dpp, id);
        printf("uri is =%s\n", uri);
        printf("is  be =%s\n", bootstrap_info);
        TEST_ASSERT((strcmp(uri, bootstrap_info) == 0));
    }
    ESP_LOGI("DPP Test", "bootstap generation passed");
    ESP_LOGI("DPP Test", "Overwrite Protocol key, responder nounce");
    {
        char protocol_key[] = "f798ed2e19286f6a6efe210b1863badb99af2a14b497634dbfd2a97394fb5aa5";
        char nounce[] = "3d0cfb011ca916d796f7029ff0b43393";

        /* Overwrite protocol key */
        memset(command, 0, 1200);
        sprintf(command, "%s%s%s", prefix, protocol_key, postfix);

        hex_len = os_strlen(command);
        ret = 0;

        if (hex_len > 2 * sizeof(dpp_protocol_key_override)) {
            ret = -1;
        } else if (hexstr2bin(command, dpp_protocol_key_override,
                              hex_len / 2)) {
            ret = -1;
        } else {
            dpp_protocol_key_override_len = hex_len / 2;
        }

        TEST_ASSERT(ret == 0);

        /* Overwrite nounce */
        hex_len = os_strlen(nounce);

        if (hex_len > 2 * sizeof(dpp_nonce_override)) {
            ret = -1;
        } else if (hexstr2bin(nounce, dpp_nonce_override, hex_len / 2)) {
            ret = -1;
        } else {
            dpp_nonce_override_len = hex_len / 2;
        }

        TEST_ASSERT(ret == 0);

    }
    ESP_LOGI("DPP Test", "Overwritten Protocol key, responder nounce.. ");
    ESP_LOGI("DPP Test", "Enqueue Auth request");
    {
        char auth_req[] = "d00012001ac459c40d649f8664c1b8771ac459c40d6400120409506f9a1a010002102000922ddd7a3ed69f46125d772bbe6017cd4e03870dc014509e38b54628e157a87d011020005d467a09760292fc15d31792b0a5b050db8bf6ad807d71b2d93f4d1c2e65d8810310400050a532ae2a07207276418d2fa630295d45569be425aa634f02014d00a7d1f61ae14f35a5a858bccad90d126c46594c49ef82655e78888e15a32d916ac217249118100200510104102900868f478fc599ac3fa8152b975eff8be4e71b189dbefbc3185b1d7f3864e896f913cba3d9601326f278";

        char auth_resp[] = "d00012349f8664c1b8771ac459c40d649f8664c1b87712340409506f9a1a0101001001000002102000922ddd7a3ed69f46125d772bbe6017cd4e03870dc014509e38b54628e157a87d091040005e3fb3576884887f17c3203d8a3a6c2fac722ef0e2201b61ac73bc655c709a902d4b030669fb9eff8b0a79fa7c1a172ac2a92c626256963f9274dc90682c81e504107500da553cdf80da3e27054c5e1f809ac303c63948b9bb5690ad12f357d75dfbc362cbae89e472dd6851925534024310aff5ae403831e98a7efc7deb9516164329c227039ae73c509147d156ae085f56c242bf7decc1f3b68d81697c6197453cb6faff7b062f7861073148052db539895bc6583d08b4aa";
        u8 *tmp;

        hex_len = os_strlen(auth_req);
        if (hex_len > 2 * MAX_FRAME_SIZE) {
            ret = -1;
        } else if (hexstr2bin(auth_req, auth, hex_len / 2)) {
            ret = -1;
        } else {
            len = hex_len / 2;
        }
        frame = auth;
        frame += 26;
        len -= 26;
        auth_instance = dpp_auth_req_rx(NULL, 1, 0, NULL,
                                        dpp_bootstrap_get_id(dpp, id), 2412, frame, frame + 6, len - 6);

        /* auth response u8 */
        hex_len = os_strlen(auth_resp);
        if (hex_len > 2 * MAX_FRAME_SIZE) {
            ret = -1;
        } else if (hexstr2bin(auth_resp, auth, hex_len / 2)) {
            ret = -1;
        } else {
            len = hex_len / 2;
        }
        tmp = auth;
        tmp += 26;
        len -= 26;

        frame = wpabuf_head_u8(auth_instance->resp_msg);
        len = wpabuf_len(auth_instance->resp_msg);

        TEST_ASSERT(memcmp(frame + 28, tmp  + 26, len - 26) == 0);
    }
    ESP_LOGI("DPP Test", "Auth request parsing passed");
    ESP_LOGI("DPP Test", "Enqueue Auth confirm parsing passed");
    {
        char auth_confirm[] = "d00012341ac459c40d649f8664c1b8771ac459c40d6412340409506f9a1a0102001001000002102000922ddd7a3ed69f46125d772bbe6017cd4e03870dc014509e38b54628e157a87d0410340054e07e62c74526dfd97e029dc781e0771e573ebc73c94227b5de8350fc6a1974b40f54c9fe1a1c9288a91fce4ee6c1f2ff069741";
        hex_len = os_strlen(auth_confirm);
        os_memset(auth, 0, 1200);
        if (hex_len > 2 * MAX_FRAME_SIZE) {
            ret = -1;
        } else if (hexstr2bin(auth_confirm, auth, hex_len / 2)) {
            ret = -1;
        } else {
            len = hex_len / 2;
        }
        frame = auth;
        frame = auth + 26;
        len = len - 26;
        dpp_auth_conf_rx(auth_instance, frame, frame + 6, len - 6);
        TEST_ASSERT(auth_instance->auth_success == 1);
    }
    ESP_LOGI("DPP Test", "Auth confirm parsing passed");
    /* deinit for memory passing */
    {
        dpp_auth_deinit(auth_instance);
        dpp_global_deinit(dpp);
    }
    ESP_LOGI("DPP Test", "Test case passed");
}
#endif
