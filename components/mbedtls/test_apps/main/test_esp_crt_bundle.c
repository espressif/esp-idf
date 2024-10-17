/* SSL server using plain mbedTLS sockets
 *
 * Adapted from the ssl_server example in mbedtls.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2019-2024 Espressif Systems (Shanghai) CO LTD
 */
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509.h"
#include "mbedtls/ssl.h"
#include "entropy_poll.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/esp_debug.h"

#include "esp_crt_bundle.h"
#include "esp_random.h"

#include "unity.h"
#include "test_utils.h"
#include "unity_test_utils.h"

#define SERVER_ADDRESS "localhost"
#define SERVER_PORT "4433"

extern const uint8_t server_cert_chain_pem_start[] asm("_binary_server_cert_chain_pem_start");
extern const uint8_t server_cert_chain_pem_end[]   asm("_binary_server_cert_chain_pem_end");

extern const uint8_t server_pk_start[] asm("_binary_prvtkey_pem_start");
extern const uint8_t server_pk_end[]   asm("_binary_prvtkey_pem_end");

// `server_cert_bundle_corrupt` is created by generating the cert bundle using `server_root.pem`
extern const uint8_t server_cert_bundle_start[] asm("_binary_server_cert_bundle_start");
extern const uint8_t server_cert_bundle_end[] asm("_binary_server_cert_bundle_end");

extern const uint8_t bad_md_crt_pem_start[] asm("_binary_bad_md_crt_pem_start");
extern const uint8_t bad_md_crt_pem_end[]   asm("_binary_bad_md_crt_pem_end");

extern const uint8_t wrong_sig_crt_pem_start[] asm("_binary_wrong_sig_crt_esp32_com_pem_start");
extern const uint8_t wrong_sig_crt_pem_end[]   asm("_binary_wrong_sig_crt_esp32_com_pem_end");

extern const uint8_t correct_sig_crt_pem_start[] asm("_binary_correct_sig_crt_esp32_com_pem_start");
extern const uint8_t correct_sig_crt_pem_end[]   asm("_binary_correct_sig_crt_esp32_com_pem_end");

#define SEM_TIMEOUT 10000
typedef struct {
    mbedtls_ssl_context ssl;
    mbedtls_net_context listen_fd;
    mbedtls_net_context client_fd;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_ssl_config conf;
    mbedtls_x509_crt cert;
    mbedtls_pk_context pkey;

} mbedtls_endpoint_t;

typedef enum {
    ESP_CRT_VALIDATE_UNKNOWN,
    ESP_CRT_VALIDATE_OK,
    ESP_CRT_VALIDATE_FAIL,
}esp_crt_validate_res_t;

int esp_crt_verify_callback(void *buf, mbedtls_x509_crt *crt, int data, uint32_t *flags);

static const char *TAG = "cert_bundle_test";

static volatile bool exit_flag;

esp_err_t endpoint_teardown(mbedtls_endpoint_t *endpoint);

static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    size_t olen;
    return mbedtls_hardware_poll(rng_state, output, len, &olen);
}

esp_err_t server_setup(mbedtls_endpoint_t *server)
{
    int ret;
    mbedtls_ssl_config_init( &server->conf );
#if CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log( &server->conf, CONFIG_MBEDTLS_DEBUG_LEVEL );
#endif
    mbedtls_net_init( &server->listen_fd );
    mbedtls_net_init( &server->client_fd );
    mbedtls_ssl_init( &server->ssl );
    mbedtls_x509_crt_init( &server->cert );
    mbedtls_pk_init( &server->pkey );
    mbedtls_entropy_init( &server->entropy );
    mbedtls_ctr_drbg_init( &server->ctr_drbg );

    ESP_LOGI(TAG, "Loading the server cert and key");
    ret = mbedtls_x509_crt_parse( &server->cert, server_cert_chain_pem_start,
                                  server_cert_chain_pem_end - server_cert_chain_pem_start);

    if ( ret != 0 ) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned %d", ret );
        return ESP_FAIL;
    }

    ret =  mbedtls_pk_parse_key( &server->pkey, (const unsigned char *)server_pk_start,
                                 server_pk_end - server_pk_start, NULL, 0, myrand, NULL );
    if ( ret != 0 ) {
        ESP_LOGE(TAG, "mbedtls_pk_parse_key returned %d", ret );
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Bind on https://%s:%s/", SERVER_ADDRESS, SERVER_PORT );
    if ( ( ret = mbedtls_net_bind( &server->listen_fd, NULL, SERVER_PORT, MBEDTLS_NET_PROTO_TCP ) ) != 0 ) {
        ESP_LOGE(TAG, "mbedtls_net_bind returned %d", ret );
        return ESP_FAIL;
    }
    mbedtls_net_set_nonblock(&server->listen_fd);

    ESP_LOGI(TAG, "Seeding the random number generator");
    if ( ( ret = mbedtls_ctr_drbg_seed( &server->ctr_drbg, mbedtls_entropy_func, &server->entropy,
                                        NULL, 0) ) != 0 ) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret );
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setting up the SSL data");
    if ( ( ret = mbedtls_ssl_config_defaults( &server->conf,
                 MBEDTLS_SSL_IS_SERVER,
                 MBEDTLS_SSL_TRANSPORT_STREAM,
                 MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 ) {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret );
        return ESP_FAIL;
    }

    mbedtls_ssl_conf_rng( &server->conf, mbedtls_ctr_drbg_random, &server->ctr_drbg );

    if (( ret = mbedtls_ssl_conf_own_cert( &server->conf, &server->cert, &server->pkey ) ) != 0 ) {
        ESP_LOGE(TAG, "mbedtls_ssl_conf_own_cert returned %d", ret );
        return ESP_FAIL;
    }

    if (( ret = mbedtls_ssl_setup( &server->ssl, &server->conf ) ) != 0 ) {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned %d", ret );
        return ESP_FAIL;
    }
    return ESP_OK;
}

void server_task(void *pvParameters)
{
    int ret;
    mbedtls_endpoint_t server;
    SemaphoreHandle_t *sema = (SemaphoreHandle_t *) pvParameters;


    if (server_setup(&server) != ESP_OK) {
        ESP_LOGE(TAG, "SSL server setup failed");
        goto exit;
    }

    /* Signal that server is up and hence client task can start now */
    xSemaphoreGive(*sema);

    bool connected = false;
    while (!exit_flag) {

        ret = mbedtls_net_accept( &server.listen_fd, &server.client_fd, NULL, 0, NULL );

        if (ret == 0) {
            connected = true;
        }

        if (connected) {
            mbedtls_ssl_set_bio( &server.ssl, &server.client_fd, mbedtls_net_send, mbedtls_net_recv, NULL );
            ret = mbedtls_ssl_handshake( &server.ssl );
            mbedtls_ssl_session_reset(&server.ssl);
            connected = false;
        }

        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    ESP_LOGE(TAG, "Server shutdown");
exit:
    endpoint_teardown(&server);
    xSemaphoreGive(*sema);
    vTaskSuspend(NULL);
}


esp_err_t endpoint_teardown(mbedtls_endpoint_t *endpoint)
{
    mbedtls_net_free( &endpoint->client_fd );
    mbedtls_net_free( &endpoint->listen_fd );

    mbedtls_x509_crt_free( &endpoint->cert );
    mbedtls_pk_free( &endpoint->pkey );
    mbedtls_ssl_free( &endpoint->ssl );
    mbedtls_ssl_config_free( &endpoint->conf );

    mbedtls_ctr_drbg_free( &endpoint->ctr_drbg );
    mbedtls_entropy_free( &endpoint->entropy );

    return ESP_OK;
}

esp_err_t client_setup(mbedtls_endpoint_t *client)
{
    int ret;
    mbedtls_ssl_config_init( &client->conf );
#if CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log( &client->conf, CONFIG_MBEDTLS_DEBUG_LEVEL );
#endif
    mbedtls_net_init( &client->client_fd );
    mbedtls_ssl_init( &client->ssl );
    mbedtls_x509_crt_init( &client->cert );
    mbedtls_pk_init( &client->pkey );
    mbedtls_entropy_init( &client->entropy );
    mbedtls_ctr_drbg_init( &client->ctr_drbg );

    ESP_LOGI(TAG, "Seeding the random number generator");
    if ((ret = mbedtls_ctr_drbg_seed(&client->ctr_drbg, mbedtls_entropy_func, &client->entropy,
                                     NULL, 0)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setting hostname for TLS session...");
    /* Hostname set here should match CN in server certificate */
    if ((ret = mbedtls_ssl_set_hostname(&client->ssl, SERVER_ADDRESS)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(&client->conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        return ESP_FAIL;
    }
    mbedtls_ssl_conf_rng(&client->conf, mbedtls_ctr_drbg_random, &client->ctr_drbg);

    if ((ret = mbedtls_ssl_setup(&client->ssl, &client->conf)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x", -ret);
        return ESP_FAIL;
    }

    return ESP_OK;
}

void client_task(void *pvParameters)
{
    SemaphoreHandle_t *client_signal_sem = (SemaphoreHandle_t *) pvParameters;
    int ret = ESP_FAIL;

    mbedtls_endpoint_t client;
    esp_crt_validate_res_t res = ESP_CRT_VALIDATE_UNKNOWN;

    if (client_setup(&client) != ESP_OK) {
        ESP_LOGE(TAG, "SSL client setup failed");
        goto exit;
    }

    /* Test with default crt bundle that does not contain the ca crt */
    ESP_LOGI(TAG, "Connecting to %s:%s...", SERVER_ADDRESS, SERVER_PORT);
    if ((ret = mbedtls_net_connect(&client.client_fd, SERVER_ADDRESS, SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
        ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
        goto exit;
    }

    ESP_LOGI(TAG, "Connected.");
    mbedtls_ssl_set_bio(&client.ssl, &client.client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    ESP_LOGI(TAG, "Performing the SSL/TLS handshake with bundle that is missing the server root certificate");
    while ( ( ret = mbedtls_ssl_handshake( &client.ssl ) ) != 0 ) {
        if ( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE ) {
            printf( "mbedtls_ssl_handshake failed with -0x%x\n", -ret );
            break;
        }
    }

    ESP_LOGI(TAG, "Verifying peer X.509 certificate for bundle ...");
    ret  = mbedtls_ssl_get_verify_result(&client.ssl);

    res = (ret == 0) ? ESP_CRT_VALIDATE_OK : ESP_CRT_VALIDATE_FAIL;

    if (res == ESP_CRT_VALIDATE_OK) {
        ESP_LOGI(TAG, "Certificate verification passed!");
    } else {
        ESP_LOGE(TAG, "Certificate verification failed!");
    }
    TEST_ASSERT_EQUAL(ESP_CRT_VALIDATE_FAIL, res);

    // Reset session before new connection
    mbedtls_ssl_close_notify(&client.ssl);
    mbedtls_ssl_session_reset(&client.ssl);
    mbedtls_net_free( &client.client_fd);

    /* Test with bundle that does contain the CA crt */
    esp_crt_bundle_attach(&client.conf);
    esp_crt_bundle_set(server_cert_bundle_start, server_cert_bundle_end - server_cert_bundle_start);

    ESP_LOGI(TAG, "Connecting to %s:%s...", SERVER_ADDRESS, SERVER_PORT);
    if ((ret = mbedtls_net_connect(&client.client_fd, SERVER_ADDRESS, SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
        ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
        goto exit;
    }

    ESP_LOGI(TAG, "Connected.");
    mbedtls_ssl_set_bio(&client.ssl, &client.client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    ESP_LOGI(TAG, "Performing the SSL/TLS handshake with bundle that is missing the server root certificate");
    while ( ( ret = mbedtls_ssl_handshake( &client.ssl ) ) != 0 ) {
        if ( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE ) {
            printf( "mbedtls_ssl_handshake failed with -0x%x\n", -ret );
            break;
        }
    }

    ESP_LOGI(TAG, "Verifying peer X.509 certificate for bundle ...");
    ret  = mbedtls_ssl_get_verify_result(&client.ssl);

    res = (ret == 0) ? ESP_CRT_VALIDATE_OK : ESP_CRT_VALIDATE_FAIL;

    if (res == ESP_CRT_VALIDATE_OK) {
        ESP_LOGI(TAG, "Certificate verification passed!");
    } else {
        ESP_LOGE(TAG, "Certificate verification failed!");
    }
    TEST_ASSERT_EQUAL(ESP_CRT_VALIDATE_OK, res);

    // Reset session before new connection
    mbedtls_ssl_close_notify(&client.ssl);
    mbedtls_ssl_session_reset(&client.ssl);
    mbedtls_net_free( &client.client_fd);


exit:
    mbedtls_ssl_close_notify(&client.ssl);
    mbedtls_ssl_session_reset(&client.ssl);
    esp_crt_bundle_detach(&client.conf);
    endpoint_teardown(&client);
    xSemaphoreGive(*client_signal_sem);
    vTaskSuspend(NULL);
}


TEST_CASE("custom certificate bundle", "[mbedtls]")
{
   test_case_uses_tcpip();

   SemaphoreHandle_t signal_sem = xSemaphoreCreateBinary();
   TEST_ASSERT_NOT_NULL(signal_sem);

   exit_flag = false;
   TaskHandle_t server_task_handle;
   xTaskCreate(server_task, "server task", 8192, &signal_sem, 10, &server_task_handle);

   // Wait for the server to start up
   if (!xSemaphoreTake(signal_sem, SEM_TIMEOUT / portTICK_PERIOD_MS)) {
       TEST_FAIL_MESSAGE("signal_sem not released, server start failed");
   }

   SemaphoreHandle_t client_signal_sem = xSemaphoreCreateBinary();
   TEST_ASSERT_NOT_NULL(client_signal_sem);

   TaskHandle_t client_task_handle;
   xTaskCreate(client_task, "client task", 8192, &client_signal_sem, 10, &client_task_handle);

   if (!xSemaphoreTake(client_signal_sem, SEM_TIMEOUT / portTICK_PERIOD_MS)) {
       TEST_FAIL_MESSAGE("client_signal_sem not released, client exit failed");
   }
   unity_utils_task_delete(client_task_handle);

   exit_flag = true;

   if (!xSemaphoreTake(signal_sem, SEM_TIMEOUT / portTICK_PERIOD_MS)) {
       TEST_FAIL_MESSAGE("signal_sem not released, server exit failed");
   }
   unity_utils_task_delete(server_task_handle);
   vSemaphoreDelete(client_signal_sem);
   vSemaphoreDelete(signal_sem);
}

TEST_CASE("custom certificate bundle - weak hash", "[mbedtls]")
{
    /* A weak signature hash on the trusted certificate should not stop
       us from verifying the chain, since we already trust it a weak signature hash is
       not a security issue */

    mbedtls_x509_crt crt;
    uint32_t flags = 0;

    esp_crt_bundle_attach(NULL);

    mbedtls_x509_crt_init( &crt );
    mbedtls_x509_crt_parse(&crt, bad_md_crt_pem_start, bad_md_crt_pem_end - bad_md_crt_pem_start);
    TEST_ASSERT_EQUAL(0, mbedtls_x509_crt_verify(&crt, NULL, NULL, NULL, &flags, esp_crt_verify_callback, NULL));

    mbedtls_x509_crt_free(&crt);

    esp_crt_bundle_detach(NULL);
}

TEST_CASE("custom certificate bundle - wrong signature", "[mbedtls]")
{
    /* Check that the bundle will not verify a valid certificate from trusted root where the signature is wrong */

    mbedtls_x509_crt crt;
    uint32_t flags = 0;

    esp_crt_bundle_attach(NULL);

    mbedtls_x509_crt_init( &crt );
    /* esp32.com cert chain where 1 byte in the signature is changed */
    printf("Testing certificate with wrong signature\n");
    mbedtls_x509_crt_parse(&crt, wrong_sig_crt_pem_start, wrong_sig_crt_pem_end - wrong_sig_crt_pem_start);
    TEST_ASSERT_NOT_EQUAL(0, mbedtls_x509_crt_verify(&crt, NULL, NULL, NULL, &flags, esp_crt_verify_callback, NULL));
    mbedtls_x509_crt_free(&crt);

    mbedtls_x509_crt_init( &crt );
    /* the correct esp32.com cert chain*/
    printf("Testing certificate with correct signature\n");
    mbedtls_x509_crt_parse(&crt, correct_sig_crt_pem_start, correct_sig_crt_pem_end - correct_sig_crt_pem_start);
    TEST_ASSERT_EQUAL(0, mbedtls_x509_crt_verify(&crt, NULL, NULL, NULL, &flags, esp_crt_verify_callback, NULL));
    mbedtls_x509_crt_free(&crt);

    esp_crt_bundle_detach(NULL);
}

TEST_CASE("custom certificate bundle init API - bound checking - NULL certificate bundle", "[mbedtls]")
{
    esp_err_t esp_ret;

    /* The API should fail when NULL is passed as the bundle */
    esp_ret = esp_crt_bundle_set(NULL, 0);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ret);
}

TEST_CASE("custom certificate bundle init API - bound checking - Invalid size of certificate bundle", "[mbedtls]")
{
    uint8_t test_bundle[1024] = {0};
    esp_err_t esp_ret;

    /* The API should fail with bundle size given as 1 */
    esp_ret = esp_crt_bundle_set(test_bundle, 1);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ret);
}

TEST_CASE("custom certificate bundle init API - bound checking - Invalid first certificate offset", "[mbedtls]")
{
    uint8_t test_bundle[1024] = {0};
    esp_err_t esp_ret;

    /* Check that the esp_crt_bundle_set API will not accept
     * the first offset to be invalid */

    /* The first certificate must start after N uint32_t offset values,
     * thus, it cannot start from the 0th position */
    test_bundle[0] = 0;
    esp_ret = esp_crt_bundle_set(test_bundle, sizeof(test_bundle));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ret);

    /* The first certificate must start after N uint32_t offset values, thus,
     * the offset from where the it would start should be divisible by sizeof(uint32_t) */
    test_bundle[0] = 1;
    esp_ret = esp_crt_bundle_set(test_bundle, sizeof(test_bundle));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ret);

    /* Check that the esp_crt_bundle_set API will not accept a bundle
     * which in which the first cert starts beyond end of bundle*/
    uint8_t *dummy_test_bundle = test_bundle + sizeof(uint32_t);

    esp_ret = esp_crt_bundle_set(dummy_test_bundle, sizeof(test_bundle));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ret);
}

TEST_CASE("custom certificate bundle init API - bound checking - Certificates count overflow", "[mbedtls]")
{
    uint8_t test_bundle[1024] = {0};
    esp_err_t esp_ret;

    memset(test_bundle, 0, sizeof(test_bundle));

    /* Check that the esp_crt_bundle_set API will not accept a bundle
     * which has more no. of certs than configured in
     * CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_MAX_CERTS */
    *((uint32_t*) test_bundle) = ((CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_MAX_CERTS + 1) * sizeof(uint32_t));

    esp_ret = esp_crt_bundle_set(test_bundle, sizeof(test_bundle));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ret);

    esp_crt_bundle_detach(NULL);
}

TEST_CASE("custom certificate bundle init API - bound checking - Incorrect certificate offset", "[mbedtls]")
{
    uint8_t test_bundle[1024] = {0};
    esp_err_t esp_ret;

    memset(test_bundle, 0, sizeof(test_bundle));

    /* Check that the esp_crt_bundle_set API will not accept a bundle where
       all offsets are not consistent with certificate data */

    /*
    | offset 1 | offset 2 | Cert 1 name len | Cert 1 key len | Cert 1 name | Cert 1 key | Cert 2 name len | ..... |
    | ----- offsets ----- |
                          | ---------------------- Certificate 1 ---------------------- |
                                                                                        | ---- Certificate 2 ---- |
    */

    *((uint32_t*) &test_bundle[0]) = (2 * sizeof(uint32_t));
    *((uint16_t*) &test_bundle[8]) = 2;     // Cert 1 name len
    *((uint16_t*) &test_bundle[10]) = 4;    // Cert 1 key len

    /* Correct offset of certificate 2 should be
        = 2 * sizeof(uint32_t) (Offsets of 2 certs) + 2 * sizeof(uint16_t) (Cert name and len) + 2 (Cert 1 name len) + 4 (Cert 1 key len);
        = 18
    */
    *((uint32_t*) &test_bundle[4]) = 16;    // Incorrect certificate 2 offset

    esp_ret = esp_crt_bundle_set(test_bundle, sizeof(test_bundle));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ret);

    /* Check that the esp_crt_bundle_set API will not accept a bundle where
       all offsets are not consistency with certificate data and the certificate
       offsets exceeds the bundle size */
    *((uint32_t*) &test_bundle[4]) = sizeof(test_bundle) + 1;    // Offset exceeds the test_bundle size

    esp_ret = esp_crt_bundle_set(test_bundle, sizeof(test_bundle));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_ret);
}
