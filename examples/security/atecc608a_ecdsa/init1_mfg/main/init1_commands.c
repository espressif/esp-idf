/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/uart.h>

#include "mbedtls/error.h"
#include "esp_console.h"
#include "esp_log.h"

#include "init1_commands.h"
#include "handlers.h"

#ifdef ESPWROOM32SE
#define CRYPT_BUF_LEN 800
#else
#define CRYPT_BUF_LEN 4096
#endif /* ESPWROOM32SE */

static unsigned char crypt_buf_private_key[CRYPT_BUF_LEN];
static unsigned char crypt_buf_csr[CRYPT_BUF_LEN];
static unsigned char crypt_buf_device_cert[CRYPT_BUF_LEN];
static unsigned char crypt_buf_ca_cert[CRYPT_BUF_LEN];

static esp_err_t register_generate_key_pair();
static esp_err_t register_generate_csr();
static esp_err_t register_generate_pub_key();
static esp_err_t register_finalise();
static esp_err_t register_program_device_cert();
static esp_err_t register_program_ca_cert();
static esp_err_t register_status();

esp_err_t register_command_handler()
{
    esp_err_t ret = register_generate_key_pair();
    ret |= register_generate_csr();
    ret |= register_generate_pub_key();
    ret |= register_program_device_cert();
    ret |= register_program_ca_cert();
    //ret |= register_finalise();
    //ret |= register_status();
    return ret;
}

static esp_err_t generate_key_pair(int argc, char** argv)
{
    int ret = ESP_ERR_INVALID_ARG;
    char error_buf[1024];
    status_object = BEGIN;
    if (argc == 3 && !strcmp(argv[1], "RSA")) {
        ret = mfg_rsa_keygen(atoi(argv[2]), crypt_buf_private_key, CRYPT_BUF_LEN); 
    }
    printf("Status: %s\n", ret ? "Failure": "Success");
    status_object = ret ? KEY_PAIR_GEN_FAIL : KEY_PAIR_GEN_SUCCESS;

    if (ret == ESP_ERR_INVALID_ARG)
        printf("Reason: Invalid Usage\n");
    else if (ret == ESP_FAIL)
        printf("Reason: Flash Operations Error\n");
    else if (ret != ESP_OK) {
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        printf("Reason: %s\n", error_buf);
    }

    fflush(stdout);
    return ESP_OK;
}

static esp_err_t register_generate_key_pair()
{
    const esp_console_cmd_t cmd = {
        .command = "generate-keys",
        .help = "Generates a Key-Pair with given params\n" 
        "  Usage:generate-keys <algo> <key-size>\n"
        "  Example:generate-keys RSA 2048",
        .func = &generate_key_pair,
    };
    return esp_console_cmd_register(&cmd);
}


static esp_err_t generate_csr(int argc, char** argv)
{
    int ret = ESP_ERR_INVALID_ARG;
    char error_buf[1024];
    if (status_object >= KEY_PAIR_GEN_SUCCESS) {
        if (argc == 1) {
            ret = mfg_csr_gen(NULL, crypt_buf_csr, CRYPT_BUF_LEN);
        } else if (argc == 2) {
            ret = mfg_csr_gen(argv[1], crypt_buf_csr, CRYPT_BUF_LEN);
        }
    }

    printf("Status: %s\n", ret ? "Failure": "Success");
    status_object = ret ? CSR_GEN_FAIL : CSR_GEN_SUCCESS;

    if (status_object < KEY_PAIR_GEN_SUCCESS)
        printf("Generate the key-pair before calling this function.\n");
    else if (ret == ESP_ERR_INVALID_ARG)
        printf("Reason: Invalid Usage\n");
    else if (ret != ESP_OK) {
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        printf("Reason: %s\n", error_buf);
    } else {
        printf("\n%s", crypt_buf_csr);
    }
    fflush(stdout);
    return ESP_OK;
}

static esp_err_t register_generate_csr()
{
    const esp_console_cmd_t cmd = {
        .command = "generate-csr",
        .help = "Generates a CSR with given params.\n" 
        "  Usage:generate-csr <subject-string>\n"
        "  Example:\n"
        "  generate-csr \"CN=Cert,O=mbed TLS,C=UK\"",
        .func = &generate_csr,
    };
    return esp_console_cmd_register(&cmd);
}

static esp_err_t generate_pub_key(int argc, char** argv)
{
    printf("Not yet implemented.\n");
    if (status_object >= KEY_PAIR_GEN_SUCCESS) {
    } else {
        printf("Generate the key-pair before calling this function.\n");
    }
    return ESP_OK;
}

static esp_err_t register_generate_pub_key()
{
    const esp_console_cmd_t cmd = {
        .command = "generate-pub-key",
        .help = "Generates a public key from the present key-pair\n" 
        "  Usage:generate-pub-key\n",
        .func = &generate_pub_key,
    };
    return esp_console_cmd_register(&cmd);
}

static esp_err_t program_device_cert(int argc, char** argv)
{
    int ret = ESP_ERR_INVALID_ARG;
    if (status_object >= CSR_GEN_SUCCESS) {
        if (argc == 1) {
            ret = mfg_input_cert(crypt_buf_device_cert, CRYPT_BUF_LEN
#ifdef ESPWROOM32SE
, CERT_TYPE_DEVICE 
#endif
);
        }
    }
    printf("Status: %s\n", ret ? "Failure": "Success");
    status_object = ret ? PROGRAM_CERT_FAIL : PROGRAM_CERT_SUCCESS;

    if (status_object < CSR_GEN_SUCCESS)
        printf("Generate the CSR before calling this function.\n");
    else if (ret == ESP_ERR_INVALID_ARG)
        printf("Reason: Invalid Usage\n");
    else if (ret != ESP_OK)
        printf("Reason: %d\n", ret);

    fflush(stdout);
    return ESP_OK;
}

static esp_err_t register_program_device_cert()
{
    const esp_console_cmd_t cmd = {
        .command = "program-dev-cert",
        .help = "Sets the UART command handler to input the device certificate.\n" 
        "  Usage:program-dev-cert\n",
        .func = &program_device_cert,
    };
    return esp_console_cmd_register(&cmd);
}

static esp_err_t program_ca_cert(int argc, char** argv)
{
    int ret = ESP_ERR_INVALID_ARG;
    if (status_object >= CSR_GEN_SUCCESS) {
        if (argc == 1) {
            ret = mfg_input_cert(crypt_buf_ca_cert, CRYPT_BUF_LEN
#ifdef ESPWROOM32SE
, CERT_TYPE_DEVICE 
#endif
);
        }
    }
    printf("Status: %s\n", ret ? "Failure": "Success");
    status_object = ret ? PROGRAM_CERT_FAIL : PROGRAM_CERT_SUCCESS;

    if (status_object < CSR_GEN_SUCCESS)
        printf("Generate the CSR before calling this function.\n");
    else if (ret == ESP_ERR_INVALID_ARG)
        printf("Reason: Invalid Usage\n");
    else if (ret != ESP_OK)
        printf("Reason: %d\n", ret);

    fflush(stdout);
    return ESP_OK;
}

static esp_err_t register_program_ca_cert()
{
    const esp_console_cmd_t cmd = {
        .command = "program-ca-cert",
        .help = "Sets the UART command handler to input the ca certificate.\n" 
        "  Usage:program-ca-cert\n",
        .func = &program_ca_cert,
    };
    return esp_console_cmd_register(&cmd);
}

static esp_err_t finalise(int argc, char** argv)
{
    int ret = ESP_ERR_INVALID_ARG;
    if (status_object == PROGRAM_CERT_SUCCESS) {
        if (argc == 1) {
            ret = mfg_overwrite_bootloader(crypt_buf_private_key, crypt_buf_device_cert, crypt_buf_ca_cert);
        }
    }
    printf("Status: %s\n", ret ? "Failure": "Success");
    status_object = ret ? FINALISE_BEGIN : FINALISE_DONE;

    if (status_object < PROGRAM_CERT_SUCCESS)
        printf("Program the certs before calling this function.\n");
    else if (ret == ESP_ERR_INVALID_ARG)
        printf("Reason: Invalid Usage\n");
    else if (ret != ESP_OK)
        printf("Reason: %d\n", ret);
    
    fflush(stdout);  
    return ESP_OK;
}

static esp_err_t register_finalise()
{
    const esp_console_cmd_t cmd = {
        .command = "finalise",
        .help = "Final step to stop the console & begin the bootloader overwriting process.\n" 
        "  Usage:finalise\n",
        .func = &finalise,
    };
    return esp_console_cmd_register(&cmd);
}

static esp_err_t status(int argc, char** argv)
{
    if (argc == 1) {
        printf("Status: Success\n");
        printf("Current Status: %d\n", status_object);
    } else {
        printf("Status: Failure\n");
        printf("Reason: Invalid Usage\n");
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static esp_err_t register_status()
{
    const esp_console_cmd_t cmd = {
        .command = "status",
        .help = "Get the current state of the process.\n" 
        "  Usage:status\n",
        .func = &status,
    };
    return esp_console_cmd_register(&cmd);
}
