/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "memory_checks.h"
#include "esp_tls.h"
#include "esp_tls_custom_stack.h"
#include "unity.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "sys/socket.h"

const char *test_cert_pem =   "-----BEGIN CERTIFICATE-----\n"\
                              "MIICrDCCAZQCCQD88gCs5AFs/jANBgkqhkiG9w0BAQsFADAYMRYwFAYDVQQDDA1F\n"\
                              "U1AtVExTIFRlc3RzMB4XDTIxMDEwNzAxMTc1OVoXDTMxMDEwNTAxMTc1OVowGDEW\n"\
                              "MBQGA1UEAwwNRVNQLVRMUyBUZXN0czCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n"\
                              "AQoCggEBAMn4KrV9M40PcxDNdmwfFMXI5cydrV3nm1o1QzcDDuYn1YfxoCCcaIc0\n"\
                              "JgHox948u94z32vnJU5qXVgEvwkD3nWGzWqjkwB1NlnIuF10XQFLa8cIas7Db/hd\n"\
                              "KygbIbYwhtwQAQFEt0/MiUbA+m+rjzvffV9pDcPRlCOE6pC+X3iC7Ft3aevkEBtR\n"\
                              "EPPN0sxGx0iFRIsglpDcEKUONYjF46q90LDmjA4EbqvK7XBuXjZ1QRGcQ871/8Ht\n"\
                              "LzHjZKc5IcIxBVoVNJo0sgHHBOaG7J8x2eAJX8DfK6GseGZDbuptiJyZoj4nmOXx\n"\
                              "iToEPG08DPlMHy90TDHXZbVeXNHzFV8CAwEAATANBgkqhkiG9w0BAQsFAAOCAQEA\n"\
                              "n3oD/VONY9WIf0jvlAn62+VheABHFQqMrH02ucDxj0lRlgS4Z6+gRN2ITkzs8eNN\n"\
                              "UglqVZCjU1mstL10tD8mwH8ShelxTWzHavc+GmAiEQx9C9zpMa7Oa+t6xFZCF6mi\n"\
                              "c9h+3oNeql7zugcHjFXnlPd+oFhDTzH8R4Y+drZyR5LZsT5SNeEvLs7DEBMHNdKC\n"\
                              "GOuWb7JNNhZKvNTymfCtM98s+7cbukwPogfWbaI+CjT49v4NOQlHNSN2Ldxpg9eh\n"\
                              "fDgV8LqpUghnYjpe5lHgEqSZxV57z+amv5DRJNsH5+hB4k6HnRjfjQZqHUPlQ7Bk\n"\
                              "pC4dhDzYsU8L1MN9e2i5Ig==\n"\
                              "-----END CERTIFICATE-----\n";

const char *test_key_pem =    "-----BEGIN PRIVATE KEY-----\n"\
                              "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDJ+Cq1fTOND3MQ\n"\
                              "zXZsHxTFyOXMna1d55taNUM3Aw7mJ9WH8aAgnGiHNCYB6MfePLveM99r5yVOal1Y\n"\
                              "BL8JA951hs1qo5MAdTZZyLhddF0BS2vHCGrOw2/4XSsoGyG2MIbcEAEBRLdPzIlG\n"\
                              "wPpvq487331faQ3D0ZQjhOqQvl94guxbd2nr5BAbURDzzdLMRsdIhUSLIJaQ3BCl\n"\
                              "DjWIxeOqvdCw5owOBG6ryu1wbl42dUERnEPO9f/B7S8x42SnOSHCMQVaFTSaNLIB\n"\
                              "xwTmhuyfMdngCV/A3yuhrHhmQ27qbYicmaI+J5jl8Yk6BDxtPAz5TB8vdEwx12W1\n"\
                              "XlzR8xVfAgMBAAECggEAMPHNF9JRjduynN3N1wHpPM0ks2N6wgkAounJcilxkOES\n"\
                              "ZDFwBrcgtbMzir7owLijJoQXnsSTS9+G42+zA/JGFOEoIrGlTDUbiMKVu0AZ0dh/\n"\
                              "M+qcR6MhHWUmsj9w+f/kjqvqMm5+pf4BPn5BspI20eGlNlv1FAFUDw+E8LLR3Bgf\n"\
                              "6Wl1BfuKHvdIRUd8nnJlDd0eeFap7yI/b2RDdvkOl4ounlKQhwpyJue2fbYJ2Z2v\n"\
                              "pCIc1Nt3lZv4KE14nBRNH95nfakA+BmIos/pN78QRtSkSQN/9drDik46+t5Bz1Lm\n"\
                              "kjFaujF73szV9mOx2O1JekziyfSJkVOJGqFSrzeIUQKBgQDvNpade0PlV+4Mh+Ra\n"\
                              "p66OhsZGisl92GZZztK660pixzXoC6cAllxutBHSmu6GM9ISa5GoXMIgmD8d16p0\n"\
                              "eXtBHuKysi/0iLkbfl1EtztB8vcChdzT0JCWD2vSqGq3772GT3TV1+2OZFsnpiON\n"\
                              "3Jy0UbHAclMHZrBknYHEOkpTpQKBgQDYJIBeUya5y0MtvcR+hu+Hku8eJq9Tr8jx\n"\
                              "Yuw5ihlg9rtsEhiWf8O0gnG9e5RrJiOAHcbDcAFx3AbB8ks8CpZ+7Cu9+Wq0VQND\n"\
                              "ySJwuWXHZfgPEIvj5MlSuaaYP52k7tAF1HJhP5H8DyHMEkmUaPvk9cYRIQNsnADD\n"\
                              "a3KDPgjlswKBgQCMTALYCCSRDRojpUvL046wLuF5HAsC0PlucLvtPvRYrKe0Galv\n"\
                              "0cASeW/+ijmc4iWyAf7itMxyW3ihIwzo1Rrx3ZBt583yaIHQWzOOirypeDtg8mvK\n"\
                              "mwdBy3ecjIRSrrwEWvOWO9cq7Tb9MRanLB2IMGXFDggeTh7kU/B4ywsutQKBgEpJ\n"\
                              "uWkyESRYjV1iVO/b9uXVBaSKTUZpK5pzkpdwNKrUArTxP0wy25PmJaunluqA8ECv\n"\
                              "e0pe69AzfIsaeOtJBFLZX2trF1alxXp93f16a1XuYSTkXgBU+TEap/y6GLfAcPUg\n"\
                              "ALQWAlns5YPbYTjGIJWgN5RZkXC56bVA79nV737rAoGBAM9swaysVULI6A9lofIH\n"\
                              "w/AgsIH0SotZt+1d7PJBww4ztrcqt66z56p13mUw0/zsIDxvPrhOS20hv3tymJyM\n"\
                              "v3bwMnszsoZ1Kkj881GmnFtpMb6cwPfkbRJuY5DHfTdRhq+Tik5uYdRthlWM92Sf\n"\
                              "Aogx44Fozd1t2hYcozPuZD4s\n"\
                              "-----END PRIVATE KEY-----\n";

#if CONFIG_ESP_TLS_USING_MBEDTLS
TEST_CASE("esp-tls init deinit", "[esp-tls]")
{
    struct esp_tls *tls = esp_tls_init();
    TEST_ASSERT_NOT_NULL(tls);
    int ret = esp_tls_conn_destroy(tls);
    TEST_ASSERT_EQUAL(0, ret);
}

TEST_CASE("esp-tls global_ca_store set free", "[esp-tls]")
{
    esp_err_t ret = esp_tls_init_global_ca_store();
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    ret = esp_tls_set_global_ca_store((const unsigned char *)test_cert_pem, strlen(test_cert_pem) + 1);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    esp_tls_free_global_ca_store();
}

TEST_CASE("esp_tls_server session create delete", "[esp-tls]")
{
    struct esp_tls *tls = esp_tls_init();
    TEST_ASSERT_NOT_NULL(tls);
    esp_tls_cfg_server_t cfg = {
        .servercert_buf = (const unsigned char *)test_cert_pem,
        .servercert_bytes = strlen(test_cert_pem) + 1,
        .serverkey_buf = (const unsigned char *)test_key_pem,
        .serverkey_bytes = strlen(test_key_pem) + 1,
    };
    // The call is going to fail at the time of TLS handshake (no internet),
    // but the internal memory for the TLS connection will be allocated.
    int ret = esp_tls_server_session_create(&cfg, -1, tls);
    TEST_ASSERT_LESS_THAN_INT(0, ret);
    // free the allocated memory.
    esp_tls_server_session_delete(tls);

}
#endif /* CONFIG_ESP_TLS_USING_MBEDTLS */

#if CONFIG_ESP_TLS_CUSTOM_STACK
/* Stub functions for testing custom stack registration */
static esp_err_t stub_create_ssl_handle(void *user_ctx, const char *hostname, size_t hostlen, const void *cfg, esp_tls_t *tls, void *server_params) { return ESP_OK; }
static int stub_handshake(void *user_ctx, esp_tls_t *tls, const esp_tls_cfg_t *cfg) { return 1; }
static ssize_t stub_read(void *user_ctx, esp_tls_t *tls, char *data, size_t datalen) { return 0; }
static ssize_t stub_write(void *user_ctx, esp_tls_t *tls, const char *data, size_t datalen) { return (ssize_t)datalen; }
static void stub_conn_delete(void *user_ctx, esp_tls_t *tls) { }
static void stub_net_init(void *user_ctx, esp_tls_t *tls) { }
static void *stub_get_ssl_context(void *user_ctx, esp_tls_t *tls) { return NULL; }
static ssize_t stub_get_bytes_avail(void *user_ctx, esp_tls_t *tls) { return 0; }
static esp_err_t stub_init_global_ca_store(void *user_ctx) { return ESP_OK; }
static esp_err_t stub_set_global_ca_store(void *user_ctx, const unsigned char *buf, const unsigned int len) { return ESP_OK; }
static void *stub_get_global_ca_store(void *user_ctx) { return NULL; }
static void stub_free_global_ca_store(void *user_ctx) { }
static const int stub_ciphersuites[] = { 0 };
static const int *stub_get_ciphersuites_list(void *user_ctx) { return stub_ciphersuites; }

static const esp_tls_stack_ops_t s_stub_ops = {
    .create_ssl_handle = stub_create_ssl_handle,
    .handshake = stub_handshake,
    .read = stub_read,
    .write = stub_write,
    .conn_delete = stub_conn_delete,
    .net_init = stub_net_init,
    .get_ssl_context = stub_get_ssl_context,
    .get_bytes_avail = stub_get_bytes_avail,
    .init_global_ca_store = stub_init_global_ca_store,
    .set_global_ca_store = stub_set_global_ca_store,
    .get_global_ca_store = stub_get_global_ca_store,
    .free_global_ca_store = stub_free_global_ca_store,
    .get_ciphersuites_list = stub_get_ciphersuites_list,
};

TEST_CASE("esp_tls custom stack registration", "[esp-tls][custom-stack]")
{
    /* Test: registration with NULL should fail */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_tls_register_stack(NULL, NULL));

    /* Test: no stack registered initially */
    TEST_ASSERT_NULL(esp_tls_get_registered_stack());

    /* Test: unregister when none registered should fail */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_tls_unregister_stack());

    /* Test: registration with valid ops should succeed */
    TEST_ASSERT_EQUAL(ESP_OK, esp_tls_register_stack(&s_stub_ops, NULL));

    /* Test: get registered stack should return the ops */
    TEST_ASSERT_EQUAL_PTR(&s_stub_ops, esp_tls_get_registered_stack());

    /* Test: double registration should fail */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_tls_register_stack(&s_stub_ops, NULL));

    /* Test: unregister should succeed */
    TEST_ASSERT_EQUAL(ESP_OK, esp_tls_unregister_stack());

    /* Test: after unregister, no stack should be registered */
    TEST_ASSERT_NULL(esp_tls_get_registered_stack());
}
#endif /* CONFIG_ESP_TLS_CUSTOM_STACK */
