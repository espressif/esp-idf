/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_srp_mpi.h"

esp_mpi_t *esp_mpi_new(void)
{
    esp_mpi_t *a = (esp_mpi_t *)malloc(sizeof (esp_mpi_t));
    if (!a) {
        return NULL;
    }
    mbedtls_mpi_init(a);
    return a;
}

esp_mpi_t *esp_mpi_new_from_hex(const char *hex)
{
    esp_mpi_t *a = esp_mpi_new();
    if (!a) {
        return NULL;
    }

    int ret = mbedtls_mpi_read_string(a, 16, hex);
    if (ret != 0) {
        printf("mbedtls_mpi_read_string() failed, returned %x\n", ret);
        esp_mpi_free(a);
        return NULL;
    }
    return a;
}

esp_mpi_t *esp_mpi_new_from_bin(const char *str, int str_len)
{
    esp_mpi_t *a = esp_mpi_new();
    if (!a) {
        return NULL;
    }

    int ret = mbedtls_mpi_read_binary(a, (unsigned char *)str, str_len);
    if (ret != 0) {
        printf("mbedtls_mpi_read_binary() failed, returned %x\n", ret);
        esp_mpi_free(a);
        return NULL;
    }
    return a;
}

void esp_mpi_free(esp_mpi_t *bn)
{
    if (bn) {
        mbedtls_mpi_free(bn);
        free(bn);
    }
}

esp_mpi_ctx_t *esp_mpi_ctx_new(void)
{
    esp_mpi_t *bn = esp_mpi_new();
    return ( esp_mpi_ctx_t *)bn;
}

void esp_mpi_ctx_free(esp_mpi_ctx_t *ctx)
{
    esp_mpi_free((esp_mpi_t *)ctx);
}

unsigned int esp_mpi_sizeof(esp_mpi_t *bn)
{
    return mbedtls_mpi_size(bn);
}

char *esp_mpi_to_bin(esp_mpi_t *bn, int *len)
{
    *len = esp_mpi_sizeof(bn);
    char *p = malloc(*len);
    if (!p) {
        return NULL;
    }

    int ret = mbedtls_mpi_write_binary(bn, (unsigned char *)p, *len);
    if (ret != 0) {
        printf("mbedtls_mpi_read_string() failed, returned %x\n", ret);
        free(p);
        return NULL;
    }
    return p;
}
int esp_get_random(void *ctx, unsigned char *data, size_t len)
{
    (void) ctx;
    esp_fill_random(data, len);
    return 0;
}

int esp_mpi_get_rand(esp_mpi_t *bn, int bits, int top, int bottom)
{
    (void) top;
    (void) bottom;
    return mbedtls_mpi_fill_random(bn, bits / 8,  esp_get_random, NULL);
}

int esp_mpi_a_exp_b_mod_c(esp_mpi_t *result, esp_mpi_t *a, esp_mpi_t *b, esp_mpi_t *c, esp_mpi_ctx_t *ctx)
{
    return mbedtls_mpi_exp_mod(result, a, b, c, (esp_mpi_t *) ctx);
}

int esp_mpi_a_mul_b_mod_c(esp_mpi_t *result, esp_mpi_t *a, esp_mpi_t *b, esp_mpi_t *c, esp_mpi_ctx_t *ctx)
{
    (void) ctx;
    int res;
    mbedtls_mpi t;
    mbedtls_mpi_init(&t);
    res = mbedtls_mpi_mul_mpi(&t, a, b);
    if (res != 0) {
        printf("mbedtls_mpi_mul_mpi(), returned %x\n", res);
        return res;
    }
    res = mbedtls_mpi_mod_mpi(result, &t, c);
    if (res != 0) {
        printf("mbedtls_mpi_mod_mpi() failed, returned %x\n", res);
        return res;
    }
    mbedtls_mpi_free(&t);

    return res;
}

int esp_mpi_a_add_b_mod_c(esp_mpi_t *result, esp_mpi_t *a, esp_mpi_t *b, esp_mpi_t *c, esp_mpi_ctx_t *ctx)
{
    (void) ctx;
    int res;
    mbedtls_mpi t;

    mbedtls_mpi_init(&t);
    res = mbedtls_mpi_add_mpi(&t, a, b);
    if (res != 0) {
        printf("mbedtls_mpi_add_mpi() failed, returned %x\n", res);
        return res;
    }
    res = mbedtls_mpi_mod_mpi(result, &t, c);
    if (res != 0) {
        printf("mbedtls_mpi_mod_mpi(), returned %x\n", res);
        return res;
    }
    mbedtls_mpi_free(&t);

    return res;
}
