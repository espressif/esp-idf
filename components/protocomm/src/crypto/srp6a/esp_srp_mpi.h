/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include "mbedtls/bignum.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "esp_random.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef mbedtls_mpi esp_mpi_t;
typedef esp_mpi_t esp_mpi_ctx_t;

esp_mpi_t *esp_mpi_new(void);

esp_mpi_t *esp_mpi_new_from_hex(const char *hex);

esp_mpi_t *esp_mpi_new_from_bin(const char *str, int str_len);

void esp_mpi_free(esp_mpi_t *bn);

esp_mpi_ctx_t *esp_mpi_ctx_new(void);

void esp_mpi_ctx_free(esp_mpi_ctx_t *ctx);

unsigned int esp_mpi_sizeof(esp_mpi_t *bn);

char *esp_mpi_to_bin(esp_mpi_t *bn, int *len);

int esp_get_random(void *ctx, unsigned char *data, size_t len);

int esp_mpi_get_rand(esp_mpi_t *bn, int bits, int top, int bottom);

int esp_mpi_a_exp_b_mod_c(esp_mpi_t *result, esp_mpi_t *a, esp_mpi_t *b, esp_mpi_t *c, esp_mpi_ctx_t *ctx);

int esp_mpi_a_mul_b_mod_c(esp_mpi_t *result, esp_mpi_t *a, esp_mpi_t *b, esp_mpi_t *c, esp_mpi_ctx_t *ctx);

int esp_mpi_a_add_b_mod_c(esp_mpi_t *result, esp_mpi_t *a, esp_mpi_t *b, esp_mpi_t *c, esp_mpi_ctx_t *ctx);

#ifdef __cplusplus
}
#endif
