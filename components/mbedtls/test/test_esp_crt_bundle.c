/* SSL server using plain mbedTLS sockets
 *
 * Adapted from the ssl_server example in mbedtls.
 *
 * Original Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 * Additions Copyright (C) Copyright 2019 Espressif Systems (Shanghai) PTE LTD, Apache 2.0 License.
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"
#include "mbedtls/x509.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"

#include "esp_crt_bundle.h"

#include "unity.h"
#include "test_utils.h"

#define SERVER_ADDRESS "localhost"
#define SERVER_PORT "4433"

extern const uint8_t server_cert_chain_pem_start[] asm("_binary_server_cert_chain_pem_start");
extern const uint8_t server_cert_chain_pem_end[]   asm("_binary_server_cert_chain_pem_end");

extern const uint8_t server_pk_start[] asm("_binary_prvtkey_pem_start");
extern const uint8_t server_pk_end[]   asm("_binary_prvtkey_pem_end");

extern const uint8_t server_cert_bundle_start[] asm("_binary_server_cert_bundle_start");
extern const uint8_t server_cert_bundle_end[] asm("_binary_server_cert_bundle_end");

typedef struct {
    mbedtls_ssl_context ssl;
    mbedtls_net_context listen_fd;
    mbedtls_net_context client_fd;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_ssl_config conf;
    mbedtls_x509_crt cert;
    mbedtls_pk_context pkey;

}mbedtls_endpoint_t;

static const char *TAG = "cert_bundle_test";

static volatile bool exit_flag;

esp_err_t endpoint_teardown(mbedtls_endpoint_t* endpoint);

esp_err_t server_setup(mbedtls_endpoint_t * server)
{
    int ret;
    mbedtls_ssl_config_init( &server->conf );
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

    if( ret != 0 ) {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned %d", ret );
        return ESP_FAIL;
    }

    ret =  mbedtls_pk_parse_key( &server->pkey, (const unsigned char *)server_pk_start ,
                                 server_pk_end - server_pk_start, NULL, 0 );
    if( ret != 0 ) {
        ESP_LOGE(TAG, "mbedtls_pk_parse_key returned %d", ret );
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Bind on https://%s:%s/", SERVER_ADDRESS, SERVER_PORT );
    if( ( ret = mbedtls_net_bind( &server->listen_fd, NULL, SERVER_PORT, MBEDTLS_NET_PROTO_TCP ) ) != 0 ) {
        ESP_LOGE(TAG, "mbedtls_net_bind returned %d", ret );
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Seeding the random number generator");
    if( ( ret = mbedtls_ctr_drbg_seed( &server->ctr_drbg, mbedtls_entropy_func, &server->entropy,
                NULL, 0) ) != 0 ) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret );
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setting up the SSL data");
    if( ( ret = mbedtls_ssl_config_defaults( &server->conf,
                    MBEDTLS_SSL_IS_SERVER,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret );
        return ESP_FAIL;
    }

    mbedtls_ssl_conf_rng( &server->conf, mbedtls_ctr_drbg_random, &server->ctr_drbg );
    mbedtls_ssl_conf_ca_chain( &server->conf, server->cert.next, NULL );

    if (( ret = mbedtls_ssl_conf_own_cert( &server->conf, &server->cert, &server->pkey ) ) != 0 )
    {
        ESP_LOGE(TAG, "mbedtls_ssl_conf_own_cert returned %d", ret );
        return ESP_FAIL;
    }

    if (( ret = mbedtls_ssl_setup( &server->ssl, &server->conf ) ) != 0 )
    {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned %d", ret );
        return ESP_FAIL;
    }
    return ESP_OK;
}

void server_task(void *pvParameters)
{
    int ret;
    mbedtls_endpoint_t server;
    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;


    if (server_setup(&server) != ESP_OK) {
        ESP_LOGE(TAG, "SSL server setup failed");
        goto exit;
    }

    ESP_LOGI(TAG, "Waiting for a remote connection" );
    if( ( ret = mbedtls_net_accept( &server.listen_fd, &server.client_fd,
                                    NULL, 0, NULL ) ) != 0 ) {
        ESP_LOGE(TAG, "mbedtls_net_accept returned %d", ret );
        goto exit;
    }

    mbedtls_ssl_set_bio( &server.ssl, &server.client_fd, mbedtls_net_send, mbedtls_net_recv, NULL );

    while(exit_flag == false) {
        mbedtls_ssl_handshake( &server.ssl );
    }
    ESP_LOGE(TAG, "Server shutdown");
    exit:
        endpoint_teardown(&server);
        xSemaphoreGive(*sema);
        vTaskDelete(NULL);
}


esp_err_t endpoint_teardown(mbedtls_endpoint_t* endpoint)
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

esp_err_t client_setup(mbedtls_endpoint_t* client)
{
    int ret;
    mbedtls_ssl_config_init( &client->conf );
    mbedtls_net_init( &client->client_fd );
    mbedtls_ssl_init( &client->ssl );
    mbedtls_x509_crt_init( &client->cert );
    mbedtls_pk_init( &client->pkey );
    mbedtls_entropy_init( &client->entropy );
    mbedtls_ctr_drbg_init( &client->ctr_drbg );

    ESP_LOGI(TAG, "Seeding the random number generator");
    if((ret = mbedtls_ctr_drbg_seed(&client->ctr_drbg, mbedtls_entropy_func, &client->entropy,
                                    NULL, 0)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setting hostname for TLS session...");
     /* Hostname set here should match CN in server certificate */
    if((ret = mbedtls_ssl_set_hostname(&client->ssl, SERVER_ADDRESS)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setting up the SSL/TLS structure...");
    if((ret = mbedtls_ssl_config_defaults(&client->conf,
                                          MBEDTLS_SSL_IS_CLIENT,
                                          MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        return ESP_FAIL;
    }
    mbedtls_ssl_conf_rng(&client->conf, mbedtls_ctr_drbg_random, &client->ctr_drbg);

    if ((ret = mbedtls_ssl_setup(&client->ssl, &client->conf)) != 0) {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x\n\n", -ret);
        return ESP_FAIL;
    }

    return ESP_OK;
}

void client_task(void *pvParameters)
{
    int ret;

    xSemaphoreHandle *sema = (xSemaphoreHandle *) pvParameters;

    mbedtls_endpoint_t client;

    if(client_setup(&client) != ESP_OK) {
        ESP_LOGE(TAG, "SSL client setup failed");
        goto exit;
    }

    // Set the custom bundle which DOESN'T includes the server's root certificate (default bundle)
    esp_crt_bundle_attach(&client.conf);

    ESP_LOGI(TAG, "Connecting to %s:%s...", SERVER_ADDRESS, SERVER_PORT);
    if ((ret = mbedtls_net_connect(&client.client_fd, SERVER_ADDRESS, SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
        ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
        goto exit;
    }

    ESP_LOGI(TAG, "Connected.");
    mbedtls_ssl_set_bio(&client.ssl, &client.client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    ESP_LOGI(TAG, "Performing the SSL/TLS handshake with bundle that is missing the server root certificate");
    ret = mbedtls_ssl_handshake(&client.ssl);

    ESP_LOGI(TAG, "Verifying peer X.509 certificate for bundle ...");
    TEST_ASSERT(mbedtls_ssl_get_verify_result(&client.ssl) != 0);

    // Reset session before new connection
    mbedtls_ssl_close_notify(&client.ssl);
    mbedtls_ssl_session_reset(&client.ssl);

    // Set the custom bundle which includes the server's root certificate
    esp_crt_bundle_set(server_cert_bundle_start);

    ESP_LOGI(TAG, "Performing the SSL/TLS handshake with bundle containing the server root certificate");
    ret = mbedtls_ssl_handshake(&client.ssl);

    ESP_LOGI(TAG, "Verifying peer X.509 certificate ...");
    ret = mbedtls_ssl_get_verify_result(&client.ssl);
    TEST_ASSERT(ret == 0);

    if(ret == 0) {
        ESP_LOGI(TAG, "Certificate validated");
    }


    exit_flag = true;

    exit:
        mbedtls_ssl_close_notify(&client.ssl);
        mbedtls_ssl_session_reset(&client.ssl);
        esp_crt_bundle_detach(&client.conf);
        endpoint_teardown(&client);
        xSemaphoreGive(*sema);
        vTaskDelete(NULL);
}

TEST_CASE("custom certificate bundle", "[mbedtls]")
{
    test_case_uses_tcpip();

    xSemaphoreHandle exit_sema = xSemaphoreCreateCounting(2, 0);

    xTaskCreate(server_task, "server task", 8192, &exit_sema, 5, NULL);

    // Wait for the server to start up
    vTaskDelay(100 / portTICK_PERIOD_MS);
    xTaskCreate(client_task, "https_get_task", 8192, &exit_sema, 5, NULL);

    for(int i = 0; i < 2; i++) {
        if(!xSemaphoreTake(exit_sema, 10000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("exit_sem not released by test task");
        }
    }
    vSemaphoreDelete(exit_sema);
}