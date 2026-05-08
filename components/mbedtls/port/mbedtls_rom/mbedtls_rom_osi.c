/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/platform.h"
#include "mbedtls_rom_osi.h"

#ifndef MBEDTLS_ERR_THREADING_BAD_INPUT_DATA
#define MBEDTLS_ERR_THREADING_BAD_INPUT_DATA MBEDTLS_ERR_THREADING_USAGE_ERROR
#endif

#define MBEDTLS_ROM_ECO4_FUNC_COUNT 221
_Static_assert(sizeof(mbedtls_rom_eco4_funcs_t) == MBEDTLS_ROM_ECO4_FUNC_COUNT * sizeof(void (*)(void)),
               "mbedtls_rom_eco4_funcs_t layout must match ROM");

#define ROM_TABLE_FN(table_type, field, fn) ((__typeof__(((table_type *)0)->field))(fn))

void mbedtls_rom_osi_functions_init(void);

extern void mbedtls_internal_sha1_process(void) __attribute__((weak));
extern void mbedtls_internal_sha256_process(void) __attribute__((weak));
extern void mbedtls_internal_sha512_process(void) __attribute__((weak));
extern void mbedtls_internal_aes_encrypt(void) __attribute__((weak));
extern void mbedtls_internal_aes_decrypt(void) __attribute__((weak));
extern int mbedtls_sha1_update(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen) __attribute__((weak));
extern int mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen) __attribute__((weak));
extern void mbedtls_oid_get_cipher_alg(void) __attribute__((weak));
extern void mbedtls_oid_get_ec_grp(void) __attribute__((weak));
extern void mbedtls_oid_get_md_alg(void) __attribute__((weak));
extern void mbedtls_oid_get_md_hmac(void) __attribute__((weak));
extern void mbedtls_oid_get_oid_by_md(void) __attribute__((weak));
extern void mbedtls_oid_get_pk_alg(void) __attribute__((weak));
extern void mbedtls_oid_get_pkcs12_pbe_alg(void) __attribute__((weak));
extern void mbedtls_oid_get_sig_alg(void) __attribute__((weak));
extern void mbedtls_oid_get_x509_ext_type(void) __attribute__((weak));

extern void rom_mbedtls_threading_set_alt(void (*mutex_init)(mbedtls_threading_mutex_t *),
                                          void (*mutex_free)(mbedtls_threading_mutex_t *),
                                          int (*mutex_lock)(mbedtls_threading_mutex_t *),
                                          int (*mutex_unlock)(mbedtls_threading_mutex_t *));

#if defined(MBEDTLS_THREADING_ALT)
static int mbedtls_rom_platform_mutex_init(mbedtls_platform_mutex_t *mutex);
static void mbedtls_rom_platform_mutex_free(mbedtls_platform_mutex_t *mutex);
static int mbedtls_rom_platform_mutex_lock(mbedtls_platform_mutex_t *mutex);
static int mbedtls_rom_platform_mutex_unlock(mbedtls_platform_mutex_t *mutex);
#endif

static void mbedtls_rom_mutex_init(mbedtls_threading_mutex_t *mutex)
{
#if defined(MBEDTLS_THREADING_ALT)
    int ret = mbedtls_rom_platform_mutex_init(&mutex->MBEDTLS_PRIVATE(mutex));
    mutex->MBEDTLS_PRIVATE(initialized) = (ret == 0);
#else
    mbedtls_mutex_init(mutex);
#endif
}

static void mbedtls_rom_mutex_free(mbedtls_threading_mutex_t *mutex)
{
#if defined(MBEDTLS_THREADING_ALT)
    if (!mutex->MBEDTLS_PRIVATE(initialized)) {
        return;
    }
    mbedtls_rom_platform_mutex_free(&mutex->MBEDTLS_PRIVATE(mutex));
    mutex->MBEDTLS_PRIVATE(initialized) = 0;
#else
    mbedtls_mutex_free(mutex);
#endif
}

static int mbedtls_rom_mutex_lock(mbedtls_threading_mutex_t *mutex)
{
#if defined(MBEDTLS_THREADING_ALT)
    if (!mutex->MBEDTLS_PRIVATE(initialized)) {
        return MBEDTLS_ERR_THREADING_USAGE_ERROR;
    }
    return mbedtls_rom_platform_mutex_lock(&mutex->MBEDTLS_PRIVATE(mutex));
#else
    return mbedtls_mutex_lock(mutex);
#endif
}

static int mbedtls_rom_mutex_unlock(mbedtls_threading_mutex_t *mutex)
{
#if defined(MBEDTLS_THREADING_ALT)
    if (!mutex->MBEDTLS_PRIVATE(initialized)) {
        return MBEDTLS_ERR_THREADING_USAGE_ERROR;
    }
    return mbedtls_rom_platform_mutex_unlock(&mutex->MBEDTLS_PRIVATE(mutex));
#else
    return mbedtls_mutex_unlock(mutex);
#endif
}

#if defined(MBEDTLS_THREADING_ALT)
static int mbedtls_rom_platform_mutex_init(mbedtls_platform_mutex_t *mutex)
{
    if (mutex == NULL) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    mutex->mutex = xSemaphoreCreateMutex();
    mutex->is_valid = (mutex->mutex != NULL);
    assert(mutex->is_valid);
    return mutex->is_valid ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR;
}

static void mbedtls_rom_platform_mutex_free(mbedtls_platform_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return;
    }

    vSemaphoreDelete(mutex->mutex);
    mutex->mutex = NULL;
    mutex->is_valid = 0;
}

static int mbedtls_rom_platform_mutex_lock(mbedtls_platform_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    if (xSemaphoreTake(mutex->mutex, portMAX_DELAY) != pdTRUE) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }
    return 0;
}

static int mbedtls_rom_platform_mutex_unlock(mbedtls_platform_mutex_t *mutex)
{
    if (mutex == NULL || !mutex->is_valid) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    if (xSemaphoreGive(mutex->mutex) != pdTRUE) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }
    return 0;
}

typedef struct mbedtls_rom_cond_waiter {
    SemaphoreHandle_t semaphore;
    struct mbedtls_rom_cond_waiter *next;
} mbedtls_rom_cond_waiter_t;

static void mbedtls_rom_cond_remove_waiter(mbedtls_platform_condition_variable_t *cond,
                                           mbedtls_rom_cond_waiter_t *waiter)
{
    mbedtls_rom_cond_waiter_t **current = &cond->waiters;

    while (*current != NULL) {
        if (*current == waiter) {
            *current = waiter->next;
            waiter->next = NULL;
            return;
        }
        current = &(*current)->next;
    }
}

static int mbedtls_rom_cond_init(mbedtls_platform_condition_variable_t *cond)
{
    if (cond == NULL) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    cond->mutex = xSemaphoreCreateMutex();
    cond->waiters = NULL;
    cond->is_valid = (cond->mutex != NULL);
    return cond->is_valid ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR;
}

static void mbedtls_rom_cond_free(mbedtls_platform_condition_variable_t *cond)
{
    if (cond == NULL || !cond->is_valid) {
        return;
    }

    vSemaphoreDelete(cond->mutex);
    cond->mutex = NULL;
    cond->waiters = NULL;
    cond->is_valid = 0;
}

static int mbedtls_rom_cond_signal(mbedtls_platform_condition_variable_t *cond)
{
    mbedtls_rom_cond_waiter_t *waiter;

    if (cond == NULL || !cond->is_valid) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    if (xSemaphoreTake(cond->mutex, portMAX_DELAY) != pdTRUE) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    waiter = cond->waiters;
    if (waiter != NULL) {
        cond->waiters = waiter->next;
        waiter->next = NULL;
        if (xSemaphoreGive(waiter->semaphore) != pdTRUE) {
            (void) xSemaphoreGive(cond->mutex);
            return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
        }
    }

    if (xSemaphoreGive(cond->mutex) != pdTRUE) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    return 0;
}

static int mbedtls_rom_cond_broadcast(mbedtls_platform_condition_variable_t *cond)
{
    mbedtls_rom_cond_waiter_t *waiter;

    if (cond == NULL || !cond->is_valid) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    if (xSemaphoreTake(cond->mutex, portMAX_DELAY) != pdTRUE) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    waiter = cond->waiters;
    cond->waiters = NULL;

    while (waiter != NULL) {
        mbedtls_rom_cond_waiter_t *next = waiter->next;
        waiter->next = NULL;
        if (xSemaphoreGive(waiter->semaphore) != pdTRUE) {
            (void) xSemaphoreGive(cond->mutex);
            return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
        }
        waiter = next;
    }

    return (xSemaphoreGive(cond->mutex) == pdTRUE) ? 0 : MBEDTLS_ERR_THREADING_MUTEX_ERROR;
}

static int mbedtls_rom_cond_wait(mbedtls_platform_condition_variable_t *cond,
                                 mbedtls_platform_mutex_t *mutex)
{
    int ret;
    mbedtls_rom_cond_waiter_t waiter = { 0 };

    if (cond == NULL || mutex == NULL || !cond->is_valid) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    waiter.semaphore = xSemaphoreCreateBinary();
    if (waiter.semaphore == NULL) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    if (xSemaphoreTake(cond->mutex, portMAX_DELAY) != pdTRUE) {
        vSemaphoreDelete(waiter.semaphore);
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    waiter.next = cond->waiters;
    cond->waiters = &waiter;

    if (xSemaphoreGive(cond->mutex) != pdTRUE) {
        cond->waiters = waiter.next;
        waiter.next = NULL;
        vSemaphoreDelete(waiter.semaphore);
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    ret = mbedtls_rom_platform_mutex_unlock(mutex);
    if (ret != 0) {
        if (xSemaphoreTake(cond->mutex, portMAX_DELAY) == pdTRUE) {
            mbedtls_rom_cond_remove_waiter(cond, &waiter);
            (void) xSemaphoreGive(cond->mutex);
        }
        vSemaphoreDelete(waiter.semaphore);
        return ret;
    }

    if (xSemaphoreTake(waiter.semaphore, portMAX_DELAY) != pdTRUE) {
        if (xSemaphoreTake(cond->mutex, portMAX_DELAY) == pdTRUE) {
            mbedtls_rom_cond_remove_waiter(cond, &waiter);
            (void) xSemaphoreGive(cond->mutex);
        }
        vSemaphoreDelete(waiter.semaphore);
        (void) mbedtls_rom_platform_mutex_lock(mutex);
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    vSemaphoreDelete(waiter.semaphore);
    ret = mbedtls_rom_platform_mutex_lock(mutex);
    /* The wakeup has already been consumed, so a re-lock failure is unrecoverable. */
    assert(ret == 0);
    return ret;
}
#endif

/* This structure can be automatically generated by the script with rom.mbedtls.ld. */
static const mbedtls_rom_eco4_funcs_t mbedtls_rom_eco4_funcs_table = {
    /* Fill the ROM functions into mbedtls rom function table. */
    ._rom_mbedtls_aes_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_init, mbedtls_aes_init),
    ._rom_mbedtls_aes_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_free, mbedtls_aes_free),
    ._rom_mbedtls_aes_setkey_enc = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_setkey_enc, mbedtls_aes_setkey_enc),
    ._rom_mbedtls_aes_setkey_dec = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_setkey_dec, mbedtls_aes_setkey_dec),
    ._rom_mbedtls_aes_crypt_ecb = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_crypt_ecb, mbedtls_aes_crypt_ecb),
    ._rom_mbedtls_aes_crypt_cbc = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_crypt_cbc, mbedtls_aes_crypt_cbc),
    ._rom_mbedtls_internal_aes_encrypt = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_internal_aes_encrypt, mbedtls_internal_aes_encrypt),
    ._rom_mbedtls_internal_aes_decrypt = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_internal_aes_decrypt, mbedtls_internal_aes_decrypt),
    ._rom_mbedtls_asn1_get_len = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_len, mbedtls_asn1_get_len),
    ._rom_mbedtls_asn1_get_tag = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_tag, mbedtls_asn1_get_tag),
    ._rom_mbedtls_asn1_get_bool = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_bool, mbedtls_asn1_get_bool),
    ._rom_mbedtls_asn1_get_int = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_int, mbedtls_asn1_get_int),
    ._rom_mbedtls_asn1_get_bitstring = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_bitstring, mbedtls_asn1_get_bitstring),
    ._rom_mbedtls_asn1_get_bitstring_null = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_bitstring_null, mbedtls_asn1_get_bitstring_null),
    ._rom_mbedtls_asn1_get_sequence_of = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_sequence_of, mbedtls_asn1_get_sequence_of),
    ._rom_mbedtls_asn1_get_mpi = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_mpi, mbedtls_asn1_get_mpi),
    ._rom_mbedtls_asn1_get_alg = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_alg, mbedtls_asn1_get_alg),
    ._rom_mbedtls_asn1_get_alg_null = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_alg_null, mbedtls_asn1_get_alg_null),
    ._rom_mbedtls_asn1_write_len = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_len, mbedtls_asn1_write_len),
    ._rom_mbedtls_asn1_write_tag = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_tag, mbedtls_asn1_write_tag),
    ._rom_mbedtls_asn1_write_mpi = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_mpi, mbedtls_asn1_write_mpi),
    ._rom_mbedtls_base64_decode = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_base64_decode, mbedtls_base64_decode),
    ._rom_mbedtls_mpi_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_init, mbedtls_mpi_init),
    ._rom_mbedtls_mpi_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_free, mbedtls_mpi_free),
    ._rom_mbedtls_mpi_grow = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_grow, mbedtls_mpi_grow),
    ._rom_mbedtls_mpi_shrink = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_shrink, mbedtls_mpi_shrink),
    ._rom_mbedtls_mpi_copy = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_copy, mbedtls_mpi_copy),
    ._rom_mbedtls_mpi_safe_cond_assign = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_safe_cond_assign, mbedtls_mpi_safe_cond_assign),
    ._rom_mbedtls_mpi_safe_cond_swap = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_safe_cond_swap, mbedtls_mpi_safe_cond_swap),
    ._rom_mbedtls_mpi_lset = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_lset, mbedtls_mpi_lset),
    ._rom_mbedtls_mpi_get_bit = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_get_bit, mbedtls_mpi_get_bit),
    ._rom_mbedtls_mpi_set_bit = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_set_bit, mbedtls_mpi_set_bit),
    ._rom_mbedtls_mpi_lsb = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_lsb, mbedtls_mpi_lsb),
    ._rom_mbedtls_mpi_bitlen = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_bitlen, mbedtls_mpi_bitlen),
    ._rom_mbedtls_mpi_size = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_size, mbedtls_mpi_size),
    ._rom_mbedtls_mpi_read_binary = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_read_binary, mbedtls_mpi_read_binary),
    ._rom_mbedtls_mpi_write_binary = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_write_binary, mbedtls_mpi_write_binary),
    ._rom_mbedtls_mpi_shift_l = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_shift_l, mbedtls_mpi_shift_l),
    ._rom_mbedtls_mpi_shift_r = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_shift_r, mbedtls_mpi_shift_r),
    ._rom_mbedtls_mpi_cmp_abs = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_cmp_abs, mbedtls_mpi_cmp_abs),
    ._rom_mbedtls_mpi_cmp_mpi = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_cmp_mpi, mbedtls_mpi_cmp_mpi),
    ._rom_mbedtls_mpi_lt_mpi_ct = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_lt_mpi_ct, mbedtls_mpi_lt_mpi_ct),
    ._rom_mbedtls_mpi_cmp_int = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_cmp_int, mbedtls_mpi_cmp_int),
    ._rom_mbedtls_mpi_add_abs = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_add_abs, mbedtls_mpi_add_abs),
    ._rom_mbedtls_mpi_sub_abs = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_sub_abs, mbedtls_mpi_sub_abs),
    ._rom_mbedtls_mpi_add_mpi = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_add_mpi, mbedtls_mpi_add_mpi),
    ._rom_mbedtls_mpi_sub_mpi = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_sub_mpi, mbedtls_mpi_sub_mpi),
    ._rom_mbedtls_mpi_add_int = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_add_int, mbedtls_mpi_add_int),
    ._rom_mbedtls_mpi_sub_int = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_sub_int, mbedtls_mpi_sub_int),
    ._rom_mbedtls_mpi_mul_mpi = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_mul_mpi, mbedtls_mpi_mul_mpi),
    ._rom_mbedtls_mpi_mul_int = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_mul_int, mbedtls_mpi_mul_int),
    ._rom_mbedtls_mpi_div_mpi = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_div_mpi, mbedtls_mpi_div_mpi),
    ._rom_mbedtls_mpi_div_int = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_div_int, mbedtls_mpi_div_int),
    ._rom_mbedtls_mpi_mod_mpi = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_mod_mpi, mbedtls_mpi_mod_mpi),
    ._rom_mbedtls_mpi_mod_int = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_mod_int, mbedtls_mpi_mod_int),
    ._rom_mbedtls_mpi_exp_mod = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_exp_mod, mbedtls_mpi_exp_mod),
    ._rom_mbedtls_mpi_fill_random = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_fill_random, mbedtls_mpi_fill_random),
    ._rom_mbedtls_mpi_gcd = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_gcd, mbedtls_mpi_gcd),
    ._rom_mbedtls_mpi_inv_mod = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_inv_mod, mbedtls_mpi_inv_mod),
    ._rom_mbedtls_mpi_is_prime_ext = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_is_prime_ext, mbedtls_mpi_is_prime_ext),
    ._rom_mbedtls_ccm_star_encrypt_and_tag = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_star_encrypt_and_tag, mbedtls_ccm_star_encrypt_and_tag),
    ._rom_mbedtls_ccm_star_auth_decrypt = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_star_auth_decrypt, mbedtls_ccm_star_auth_decrypt),
    ._rom_mbedtls_cipher_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_init, mbedtls_cipher_init),
    ._rom_mbedtls_cipher_set_padding_mode = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_set_padding_mode, mbedtls_cipher_set_padding_mode),
    ._rom_mbedtls_cipher_reset = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_reset, mbedtls_cipher_reset),
    ._rom_mbedtls_cipher_finish = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_finish, mbedtls_cipher_finish),
    ._rom_mbedtls_cipher_crypt = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_crypt, mbedtls_cipher_crypt),
    ._rom_mbedtls_cipher_cmac_starts = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_cmac_starts, mbedtls_cipher_cmac_starts),
    ._rom_mbedtls_cipher_cmac_update = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_cmac_update, mbedtls_cipher_cmac_update),
    ._rom_mbedtls_cipher_cmac_finish = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_cmac_finish, mbedtls_cipher_cmac_finish),
    ._rom_mbedtls_ctr_drbg_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_init, mbedtls_ctr_drbg_init),
    ._rom_mbedtls_ctr_drbg_seed = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_seed, mbedtls_ctr_drbg_seed),
    ._rom_mbedtls_ctr_drbg_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_free, mbedtls_ctr_drbg_free),
    ._rom_mbedtls_ctr_drbg_reseed = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_reseed, mbedtls_ctr_drbg_reseed),
    ._rom_mbedtls_ctr_drbg_random_with_add = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_random_with_add, mbedtls_ctr_drbg_random_with_add),
    ._rom_mbedtls_ctr_drbg_random = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_random, mbedtls_ctr_drbg_random),
    ._rom_mbedtls_sha1_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha1_init, mbedtls_sha1_init),
    ._rom_mbedtls_sha1_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha1_free, mbedtls_sha1_free),
    ._rom_mbedtls_sha1_clone = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha1_clone, mbedtls_sha1_clone),
    ._rom_mbedtls_sha1_starts = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha1_starts, mbedtls_sha1_starts),
    ._rom_mbedtls_sha1_finish = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha1_finish, mbedtls_sha1_finish),
    ._rom_mbedtls_sha256_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha256_init, mbedtls_sha256_init),
    ._rom_mbedtls_sha256_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha256_free, mbedtls_sha256_free),
    ._rom_mbedtls_sha256_clone = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha256_clone, mbedtls_sha256_clone),
    ._rom_mbedtls_sha256_starts = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha256_starts, mbedtls_sha256_starts),
    ._rom_mbedtls_sha256_finish = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha256_finish, mbedtls_sha256_finish),
    ._rom_mbedtls_sha256 = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha256, mbedtls_sha256),
    ._rom_mbedtls_sha512_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha512_init, mbedtls_sha512_init),
    ._rom_mbedtls_sha512_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha512_free, mbedtls_sha512_free),
    ._rom_mbedtls_sha512_clone = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha512_clone, mbedtls_sha512_clone),
    ._rom_mbedtls_sha512_starts = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha512_starts, mbedtls_sha512_starts),
    ._rom_mbedtls_sha512_update = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha512_update, mbedtls_sha512_update),
    ._rom_mbedtls_sha512_finish = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha512_finish, mbedtls_sha512_finish),
    ._rom_mbedtls_sha512 = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha512, mbedtls_sha512),
    ._rom_mbedtls_aes_xts_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_xts_init, mbedtls_aes_xts_init),
    ._rom_mbedtls_aes_xts_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_xts_free, mbedtls_aes_xts_free),
    ._rom_mbedtls_aes_xts_setkey_enc = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_xts_setkey_enc, mbedtls_aes_xts_setkey_enc),
    ._rom_mbedtls_aes_xts_setkey_dec = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_xts_setkey_dec, mbedtls_aes_xts_setkey_dec),
    ._rom_mbedtls_aes_crypt_xts = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_crypt_xts, mbedtls_aes_crypt_xts),
    ._rom_mbedtls_aes_crypt_cfb128 = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_crypt_cfb128, mbedtls_aes_crypt_cfb128),
    ._rom_mbedtls_aes_crypt_ofb = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_crypt_ofb, mbedtls_aes_crypt_ofb),
    ._rom_mbedtls_aes_crypt_ctr = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_crypt_ctr, mbedtls_aes_crypt_ctr),
    ._rom_mbedtls_ccm_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_init, mbedtls_ccm_init),
    ._rom_mbedtls_ccm_setkey = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_setkey, mbedtls_ccm_setkey),
    ._rom_mbedtls_ccm_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_free, mbedtls_ccm_free),
    ._rom_mbedtls_ccm_encrypt_and_tag = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_encrypt_and_tag, mbedtls_ccm_encrypt_and_tag),
    ._rom_mbedtls_ccm_auth_decrypt = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_auth_decrypt, mbedtls_ccm_auth_decrypt),
    ._rom_mbedtls_md5_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_md5_init, mbedtls_md5_init),
    ._rom_mbedtls_md5_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_md5_free, mbedtls_md5_free),
    ._rom_mbedtls_md5_clone = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_md5_clone, mbedtls_md5_clone),
    ._rom_mbedtls_md5_starts = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_md5_starts, mbedtls_md5_starts),
    ._rom_mbedtls_md5_update = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_md5_update, mbedtls_md5_update),
    ._rom_mbedtls_md5_finish = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_md5_finish, mbedtls_md5_finish),
    ._rom_mbedtls_md5 = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_md5, mbedtls_md5),
    ._rom_mbedtls_sha1 = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha1, mbedtls_sha1),
    ._rom_mbedtls_aes_crypt_cfb8 = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_crypt_cfb8, mbedtls_aes_crypt_cfb8),
    ._rom_mbedtls_mpi_swap = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_swap, mbedtls_mpi_swap),
    ._rom_mbedtls_mpi_read_string = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_read_string, mbedtls_mpi_read_string),
    ._rom_mbedtls_mpi_write_string = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_write_string, mbedtls_mpi_write_string),
    ._rom_mbedtls_mpi_read_binary_le = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_read_binary_le, mbedtls_mpi_read_binary_le),
    ._rom_mbedtls_mpi_write_binary_le = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_write_binary_le, mbedtls_mpi_write_binary_le),
    ._rom_mbedtls_mpi_random = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_random, mbedtls_mpi_random),
    ._rom_mbedtls_mpi_gen_prime = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_mpi_gen_prime, mbedtls_mpi_gen_prime),
    ._rom_mbedtls_ecp_check_budget = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_check_budget, mbedtls_ecp_check_budget),
    ._rom_mbedtls_ecp_set_max_ops = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_set_max_ops, mbedtls_ecp_set_max_ops),
    ._rom_mbedtls_ecp_restart_is_enabled = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_restart_is_enabled, mbedtls_ecp_restart_is_enabled),
    ._rom_mbedtls_ecp_get_type = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_get_type, mbedtls_ecp_get_type),
    ._rom_mbedtls_ecp_curve_list = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_curve_list, mbedtls_ecp_curve_list),
    ._rom_mbedtls_ecp_grp_id_list = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_grp_id_list, mbedtls_ecp_grp_id_list),
    ._rom_mbedtls_ecp_curve_info_from_grp_id = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_curve_info_from_grp_id, mbedtls_ecp_curve_info_from_grp_id),
    ._rom_mbedtls_ecp_curve_info_from_tls_id = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_curve_info_from_tls_id, mbedtls_ecp_curve_info_from_tls_id),
    ._rom_mbedtls_ecp_curve_info_from_name = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_curve_info_from_name, mbedtls_ecp_curve_info_from_name),
    ._rom_mbedtls_ecp_point_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_point_init, mbedtls_ecp_point_init),
    ._rom_mbedtls_ecp_group_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_group_init, mbedtls_ecp_group_init),
    ._rom_mbedtls_ecp_keypair_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_keypair_init, mbedtls_ecp_keypair_init),
    ._rom_mbedtls_ecp_point_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_point_free, mbedtls_ecp_point_free),
    ._rom_mbedtls_ecp_group_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_group_free, mbedtls_ecp_group_free),
    ._rom_mbedtls_ecp_keypair_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_keypair_free, mbedtls_ecp_keypair_free),
    ._rom_mbedtls_ecp_restart_init = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_restart_init, mbedtls_ecp_restart_init),
    ._rom_mbedtls_ecp_restart_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_restart_free, mbedtls_ecp_restart_free),
    ._rom_mbedtls_ecp_copy = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_copy, mbedtls_ecp_copy),
    ._rom_mbedtls_ecp_group_copy = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_group_copy, mbedtls_ecp_group_copy),
    ._rom_mbedtls_ecp_set_zero = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_set_zero, mbedtls_ecp_set_zero),
    ._rom_mbedtls_ecp_is_zero = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_is_zero, mbedtls_ecp_is_zero),
    ._rom_mbedtls_ecp_point_cmp = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_point_cmp, mbedtls_ecp_point_cmp),
    ._rom_mbedtls_ecp_point_read_string = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_point_read_string, mbedtls_ecp_point_read_string),
    ._rom_mbedtls_ecp_point_write_binary = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_point_write_binary, mbedtls_ecp_point_write_binary),
    ._rom_mbedtls_ecp_point_read_binary = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_point_read_binary, mbedtls_ecp_point_read_binary),
    ._rom_mbedtls_ecp_tls_read_point = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_tls_read_point, mbedtls_ecp_tls_read_point),
    ._rom_mbedtls_ecp_tls_write_point = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_tls_write_point, mbedtls_ecp_tls_write_point),
    ._rom_mbedtls_ecp_group_load = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_group_load, mbedtls_ecp_group_load),
    ._rom_mbedtls_ecp_tls_read_group = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_tls_read_group, mbedtls_ecp_tls_read_group),
    ._rom_mbedtls_ecp_tls_read_group_id = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_tls_read_group_id, mbedtls_ecp_tls_read_group_id),
    ._rom_mbedtls_ecp_tls_write_group = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_tls_write_group, mbedtls_ecp_tls_write_group),
    ._rom_mbedtls_ecp_mul = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_mul, mbedtls_ecp_mul),
    ._rom_mbedtls_ecp_mul_restartable = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_mul_restartable, mbedtls_ecp_mul_restartable),
    ._rom_mbedtls_ecp_muladd = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_muladd, mbedtls_ecp_muladd),
    ._rom_mbedtls_ecp_muladd_restartable = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_muladd_restartable, mbedtls_ecp_muladd_restartable),
    ._rom_mbedtls_ecp_check_pubkey = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_check_pubkey, mbedtls_ecp_check_pubkey),
    ._rom_mbedtls_ecp_check_privkey = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_check_privkey, mbedtls_ecp_check_privkey),
    ._rom_mbedtls_ecp_gen_privkey = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_gen_privkey, mbedtls_ecp_gen_privkey),
    ._rom_mbedtls_ecp_gen_keypair_base = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_gen_keypair_base, mbedtls_ecp_gen_keypair_base),
    ._rom_mbedtls_ecp_gen_keypair = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_gen_keypair, mbedtls_ecp_gen_keypair),
    ._rom_mbedtls_ecp_gen_key = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_gen_key, mbedtls_ecp_gen_key),
    ._rom_mbedtls_ecp_read_key = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_read_key, mbedtls_ecp_read_key),
    ._rom_mbedtls_ecp_write_key_ext = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_write_key_ext, mbedtls_ecp_write_key_ext),
    ._rom_mbedtls_ecp_check_pub_priv = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_check_pub_priv, mbedtls_ecp_check_pub_priv),
    ._rom_mbedtls_ecp_export = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ecp_export, mbedtls_ecp_export),
    ._rom_mbedtls_asn1_get_enum = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_get_enum, mbedtls_asn1_get_enum),
    ._rom_mbedtls_asn1_sequence_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_sequence_free, mbedtls_asn1_sequence_free),
    ._rom_mbedtls_asn1_traverse_sequence_of = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_traverse_sequence_of, mbedtls_asn1_traverse_sequence_of),
    ._rom_mbedtls_asn1_find_named_data = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_find_named_data, mbedtls_asn1_find_named_data),
    ._rom_mbedtls_asn1_free_named_data_list = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_free_named_data_list, mbedtls_asn1_free_named_data_list),
    ._rom_mbedtls_asn1_free_named_data_list_shallow = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_free_named_data_list_shallow, mbedtls_asn1_free_named_data_list_shallow),
    ._rom_mbedtls_asn1_write_raw_buffer = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_raw_buffer, mbedtls_asn1_write_raw_buffer),
    ._rom_mbedtls_asn1_write_null = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_null, mbedtls_asn1_write_null),
    ._rom_mbedtls_asn1_write_oid = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_oid, mbedtls_asn1_write_oid),
    ._rom_mbedtls_asn1_write_algorithm_identifier = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_algorithm_identifier, mbedtls_asn1_write_algorithm_identifier),
    ._rom_mbedtls_asn1_write_bool = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_bool, mbedtls_asn1_write_bool),
    ._rom_mbedtls_asn1_write_int = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_int, mbedtls_asn1_write_int),
    ._rom_mbedtls_asn1_write_enum = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_enum, mbedtls_asn1_write_enum),
    ._rom_mbedtls_asn1_write_tagged_string = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_tagged_string, mbedtls_asn1_write_tagged_string),
    ._rom_mbedtls_asn1_write_printable_string = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_printable_string, mbedtls_asn1_write_printable_string),
    ._rom_mbedtls_asn1_write_utf8_string = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_utf8_string, mbedtls_asn1_write_utf8_string),
    ._rom_mbedtls_asn1_write_ia5_string = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_ia5_string, mbedtls_asn1_write_ia5_string),
    ._rom_mbedtls_asn1_write_bitstring = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_bitstring, mbedtls_asn1_write_bitstring),
    ._rom_mbedtls_asn1_write_named_bitstring = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_named_bitstring, mbedtls_asn1_write_named_bitstring),
    ._rom_mbedtls_asn1_write_octet_string = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_write_octet_string, mbedtls_asn1_write_octet_string),
    ._rom_mbedtls_asn1_store_named_data = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_asn1_store_named_data, mbedtls_asn1_store_named_data),
    ._rom_mbedtls_ccm_starts = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_starts, mbedtls_ccm_starts),
    ._rom_mbedtls_ccm_set_lengths = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_set_lengths, mbedtls_ccm_set_lengths),
    ._rom_mbedtls_ccm_update_ad = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_update_ad, mbedtls_ccm_update_ad),
    ._rom_mbedtls_ccm_update = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_update, mbedtls_ccm_update),
    ._rom_mbedtls_ccm_finish = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ccm_finish, mbedtls_ccm_finish),
    ._rom_mbedtls_cipher_list = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_list, mbedtls_cipher_list),
    ._rom_mbedtls_cipher_info_from_string = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_info_from_string, mbedtls_cipher_info_from_string),
    ._rom_mbedtls_cipher_info_from_type = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_info_from_type, mbedtls_cipher_info_from_type),
    ._rom_mbedtls_cipher_info_from_values = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_info_from_values, mbedtls_cipher_info_from_values),
    ._rom_mbedtls_cipher_free = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_free, mbedtls_cipher_free),
    ._rom_mbedtls_cipher_setup = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_setup, mbedtls_cipher_setup),
    ._rom_mbedtls_cipher_setkey = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_setkey, mbedtls_cipher_setkey),
    ._rom_mbedtls_cipher_set_iv = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_set_iv, mbedtls_cipher_set_iv),
    ._rom_mbedtls_cipher_update_ad = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_update_ad, mbedtls_cipher_update_ad),
    ._rom_mbedtls_cipher_update = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_update, mbedtls_cipher_update),
    ._rom_mbedtls_cipher_write_tag = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_write_tag, mbedtls_cipher_write_tag),
    ._rom_mbedtls_cipher_check_tag = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_check_tag, mbedtls_cipher_check_tag),
    ._rom_mbedtls_cipher_auth_encrypt_ext = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_auth_encrypt_ext, mbedtls_cipher_auth_encrypt_ext),
    ._rom_mbedtls_cipher_auth_decrypt_ext = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_auth_decrypt_ext, mbedtls_cipher_auth_decrypt_ext),
    ._rom_mbedtls_cipher_cmac_reset = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_cmac_reset, mbedtls_cipher_cmac_reset),
    ._rom_mbedtls_cipher_cmac = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_cipher_cmac, mbedtls_cipher_cmac),
    ._rom_mbedtls_aes_cmac_prf_128 = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_aes_cmac_prf_128, mbedtls_aes_cmac_prf_128),
    ._rom_mbedtls_ctr_drbg_set_prediction_resistance = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_set_prediction_resistance, mbedtls_ctr_drbg_set_prediction_resistance),
    ._rom_mbedtls_ctr_drbg_set_entropy_len = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_set_entropy_len, mbedtls_ctr_drbg_set_entropy_len),
    ._rom_mbedtls_ctr_drbg_set_nonce_len = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_set_nonce_len, mbedtls_ctr_drbg_set_nonce_len),
    ._rom_mbedtls_ctr_drbg_set_reseed_interval = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_set_reseed_interval, mbedtls_ctr_drbg_set_reseed_interval),
    ._rom_mbedtls_ctr_drbg_update = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_ctr_drbg_update, mbedtls_ctr_drbg_update),
    ._rom_mbedtls_base64_encode = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_base64_encode, mbedtls_base64_encode),
    ._rom_mbedtls_sha1_update = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha1_update, mbedtls_sha1_update),
    ._rom_mbedtls_sha256_update = ROM_TABLE_FN(mbedtls_rom_eco4_funcs_t, _rom_mbedtls_sha256_update, mbedtls_sha256_update),
    ._rom_mbedtls_mem_calloc = MBEDTLS_PLATFORM_STD_CALLOC,
    ._rom_mbedtls_mem_free = MBEDTLS_PLATFORM_STD_FREE,
};

__attribute__((constructor)) void mbedtls_rom_osi_functions_init(void)
{
    /* Export the rom mbedtls functions table pointer */
    extern void *mbedtls_rom_osi_funcs_ptr;

#if defined(MBEDTLS_THREADING_ALT)
    mbedtls_threading_set_alt(mbedtls_rom_platform_mutex_init,
                              mbedtls_rom_platform_mutex_free,
                              mbedtls_rom_platform_mutex_lock,
                              mbedtls_rom_platform_mutex_unlock,
                              mbedtls_rom_cond_init,
                              mbedtls_rom_cond_free,
                              mbedtls_rom_cond_signal,
                              mbedtls_rom_cond_broadcast,
                              mbedtls_rom_cond_wait);
#endif

    /* Initialize the rom function mbedtls_threading_set_alt on chip rev2.0 with rom eco4 */
    rom_mbedtls_threading_set_alt(mbedtls_rom_mutex_init, mbedtls_rom_mutex_free, mbedtls_rom_mutex_lock, mbedtls_rom_mutex_unlock);

    /* Initialize the pointer of rom eco4 mbedtls functions table. */
    mbedtls_rom_osi_funcs_ptr = (mbedtls_rom_eco4_funcs_t *)&mbedtls_rom_eco4_funcs_table;
}
