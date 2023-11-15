/*
 * Multi-precision integer library
 * ESP-IDF hardware accelerated parts based on mbedTLS implementation
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2023 Espressif Systems (Shanghai) CO LTD
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/param.h>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

#include "esp_private/periph_ctrl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "bignum_impl.h"

#include "mbedtls/bignum.h"

#include "hal/mpi_hal.h"

/* Some implementation notes:
 *
 * - Naming convention x_words, y_words, z_words for number of words (limbs) used in a particular
 *   bignum. This number may be less than the size of the bignum
 *
 * - Naming convention hw_words for the hardware length of the operation. This number maybe be rounded up
 *   for targets that requres this (e.g. ESP32), and may be larger than any of the numbers
 *   involved in the calculation.
 *
 * - Timing behaviour of these functions will depend on the length of the inputs. This is fundamentally
 *   the same constraint as the software mbedTLS implementations, and relies on the same
 *   countermeasures (exponent blinding, etc) which are used in mbedTLS.
 */

static const __attribute__((unused)) char *TAG = "bignum";

#define ciL    (sizeof(mbedtls_mpi_uint))         /* chars in limb  */
#define biL    (ciL << 3)                         /* bits  in limb  */

#if defined(CONFIG_MBEDTLS_MPI_USE_INTERRUPT)
static SemaphoreHandle_t op_complete_sem;
#if defined(CONFIG_PM_ENABLE)
static esp_pm_lock_handle_t s_pm_cpu_lock;
static esp_pm_lock_handle_t s_pm_sleep_lock;
#endif

static IRAM_ATTR void esp_mpi_complete_isr(void *arg)
{
    BaseType_t higher_woken;
    mpi_hal_clear_interrupt();

    xSemaphoreGiveFromISR(op_complete_sem, &higher_woken);
    if (higher_woken) {
        portYIELD_FROM_ISR();
    }
}


static esp_err_t esp_mpi_isr_initialise(void)
{
    mpi_hal_clear_interrupt();
    mpi_hal_interrupt_enable(true);
    if (op_complete_sem == NULL) {
        static StaticSemaphore_t op_sem_buf;
        op_complete_sem = xSemaphoreCreateBinaryStatic(&op_sem_buf);
        if (op_complete_sem == NULL) {
            ESP_LOGE(TAG, "Failed to create intr semaphore");
            return ESP_FAIL;
        }

        const int isr_flags = esp_intr_level_to_flags(CONFIG_MBEDTLS_MPI_INTERRUPT_LEVEL);

        esp_err_t ret;
        ret = esp_intr_alloc(ETS_RSA_INTR_SOURCE, isr_flags, esp_mpi_complete_isr, NULL, NULL);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to allocate RSA interrupt %d", ret);

            // This should be treated as fatal error as this API would mostly
            // be invoked within mbedTLS interface. There is no way for the system
            // to proceed if the MPI interrupt allocation fails here.
            abort();
        }
    }

    /* MPI is clocked proportionally to CPU clock, take power management lock */
#ifdef CONFIG_PM_ENABLE
    if (s_pm_cpu_lock == NULL) {
        if (esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "mpi_sleep", &s_pm_sleep_lock) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create PM sleep lock");
            return ESP_FAIL;
        }
        if (esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "mpi_cpu", &s_pm_cpu_lock) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create PM CPU lock");
            return ESP_FAIL;
        }
    }
    esp_pm_lock_acquire(s_pm_cpu_lock);
    esp_pm_lock_acquire(s_pm_sleep_lock);
#endif

    return ESP_OK;
}

static int esp_mpi_wait_intr(void)
{
    if (!xSemaphoreTake(op_complete_sem, 2000 / portTICK_PERIOD_MS)) {
        ESP_LOGE("MPI", "Timed out waiting for completion of MPI Interrupt");
        return -1;
    }

#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(s_pm_cpu_lock);
    esp_pm_lock_release(s_pm_sleep_lock);
#endif  // CONFIG_PM_ENABLE

    mpi_hal_interrupt_enable(false);

    return 0;
}

#endif // CONFIG_MBEDTLS_MPI_USE_INTERRUPT

/* Convert bit count to word count
 */
static inline size_t bits_to_words(size_t bits)
{
    return (bits + 31) / 32;
}

/* Return the number of words actually used to represent an mpi
   number.
*/
#if defined(MBEDTLS_MPI_EXP_MOD_ALT) || defined(MBEDTLS_MPI_EXP_MOD_ALT_FALLBACK)
static size_t mpi_words(const mbedtls_mpi *mpi)
{
    for (size_t i = mpi->MBEDTLS_PRIVATE(n); i > 0; i--) {
        if (mpi->MBEDTLS_PRIVATE(p[i - 1]) != 0) {
            return i;
        }
    }
    return 0;
}

#endif //(MBEDTLS_MPI_EXP_MOD_ALT || MBEDTLS_MPI_EXP_MOD_ALT_FALLBACK)

/**
 *
 * There is a need for the value of integer N' such that B^-1(B-1)-N^-1N'=1,
 * where B^-1(B-1) mod N=1. Actually, only the least significant part of
 * N' is needed, hence the definition N0'=N' mod b. We reproduce below the
 * simple algorithm from an article by Dusse and Kaliski to efficiently
 * find N0' from N0 and b
 */
static mbedtls_mpi_uint modular_inverse(const mbedtls_mpi *M)
{
    int i;
    uint64_t t = 1;
    uint64_t two_2_i_minus_1 = 2;   /* 2^(i-1) */
    uint64_t two_2_i = 4;           /* 2^i */
    uint64_t N = M->MBEDTLS_PRIVATE(p[0]);

    for (i = 2; i <= 32; i++) {
        if ((mbedtls_mpi_uint) N * t % two_2_i >= two_2_i_minus_1) {
            t += two_2_i_minus_1;
        }

        two_2_i_minus_1 <<= 1;
        two_2_i <<= 1;
    }

    return (mbedtls_mpi_uint)(UINT32_MAX - t + 1);
}

/* Calculate Rinv = RR^2 mod M, where:
 *
 *  R = b^n where b = 2^32, n=num_words,
 *  R = 2^N (where N=num_bits)
 *  RR = R^2 = 2^(2*N) (where N=num_bits=num_words*32)
 *
 * This calculation is computationally expensive (mbedtls_mpi_mod_mpi)
 * so caller should cache the result where possible.
 *
 * DO NOT call this function while holding esp_mpi_enable_hardware_hw_op().
 *
 */
static int calculate_rinv(mbedtls_mpi *Rinv, const mbedtls_mpi *M, int num_words)
{
    int ret;
    size_t num_bits = num_words * 32;
    mbedtls_mpi RR;
    mbedtls_mpi_init(&RR);
    MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(&RR, num_bits * 2, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(Rinv, &RR, M));

cleanup:
    mbedtls_mpi_free(&RR);

    return ret;
}




/* Z = (X * Y) mod M

   Not an mbedTLS function
*/
int esp_mpi_mul_mpi_mod(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M)
{
    int ret = 0;

    size_t x_bits = mbedtls_mpi_bitlen(X);
    size_t y_bits = mbedtls_mpi_bitlen(Y);
    size_t m_bits = mbedtls_mpi_bitlen(M);
    size_t z_bits = MIN(m_bits, x_bits + y_bits);
    size_t x_words = bits_to_words(x_bits);
    size_t y_words = bits_to_words(y_bits);
    size_t m_words = bits_to_words(m_bits);
    size_t z_words = bits_to_words(z_bits);
    size_t hw_words = mpi_hal_calc_hardware_words(MAX(x_words, MAX(y_words, m_words))); /* longest operand */
    mbedtls_mpi Rinv;
    mbedtls_mpi_uint Mprime;

    /* Calculate and load the first stage montgomery multiplication */
    mbedtls_mpi_init(&Rinv);
    MBEDTLS_MPI_CHK(calculate_rinv(&Rinv, M, hw_words));
    Mprime = modular_inverse(M);

    esp_mpi_enable_hardware_hw_op();
    /* Load and start a (X * Y) mod M calculation */
    esp_mpi_mul_mpi_mod_hw_op(X, Y, M, &Rinv, Mprime, hw_words);

    MBEDTLS_MPI_CHK(mbedtls_mpi_grow(Z, z_words));

    /* Read back the result */
    mpi_hal_read_result_hw_op(Z->MBEDTLS_PRIVATE(p), Z->MBEDTLS_PRIVATE(n), z_words);
    Z->MBEDTLS_PRIVATE(s) = X->MBEDTLS_PRIVATE(s) * Y->MBEDTLS_PRIVATE(s);

cleanup:
    mbedtls_mpi_free(&Rinv);
    esp_mpi_disable_hardware_hw_op();

    return ret;
}

#if defined(MBEDTLS_MPI_EXP_MOD_ALT) || defined(MBEDTLS_MPI_EXP_MOD_ALT_FALLBACK)

#ifdef ESP_MPI_USE_MONT_EXP
/*
 * Return the most significant one-bit.
 */
static size_t mbedtls_mpi_msb( const mbedtls_mpi *X )
{
    int i, j;
    if (X != NULL && X->MBEDTLS_PRIVATE(n) != 0) {
        for (i = X->MBEDTLS_PRIVATE(n) - 1; i >= 0; i--) {
            if (X->MBEDTLS_PRIVATE(p[i]) != 0) {
                for (j = biL - 1; j >= 0; j--) {
                    if ((X->MBEDTLS_PRIVATE(p[i]) & (1 << j)) != 0) {
                        return (i * biL) + j;
                    }
                }
            }
        }
    }
    return 0;
}


/*
 * Montgomery exponentiation: Z = X ^ Y mod M  (HAC 14.94)
 */
static int mpi_montgomery_exp_calc( mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M,
                                    mbedtls_mpi *Rinv,
                                    size_t hw_words,
                                    mbedtls_mpi_uint Mprime )
{
    int ret = 0;
    mbedtls_mpi X_, one;

    mbedtls_mpi_init(&X_);
    mbedtls_mpi_init(&one);
    if ( ( ( ret = mbedtls_mpi_grow(&one, hw_words) ) != 0 ) ||
            ( ( ret = mbedtls_mpi_set_bit(&one, 0, 1) )  != 0 ) ) {
        goto cleanup2;
    }

    // Algorithm from HAC 14.94
    {
        // 0 determine t (highest bit set in y)
        int t = mbedtls_mpi_msb(Y);

        esp_mpi_enable_hardware_hw_op();

        // 1.1 x_ = mont(x, R^2 mod m)
        //        = mont(x, rb)
        MBEDTLS_MPI_CHK( esp_mont_hw_op(&X_, X, Rinv, M, Mprime, hw_words, false) );

        // 1.2 z = R mod m
        // now z = R mod m = Mont (R^2 mod m, 1) mod M (as Mont(x) = X&R^-1 mod M)
        MBEDTLS_MPI_CHK( esp_mont_hw_op(Z, Rinv, &one, M, Mprime, hw_words, true) );

        // 2 for i from t down to 0
        for (int i = t; i >= 0; i--) {
            // 2.1 z = mont(z,z)
            if (i != t) { // skip on the first iteration as is still unity
                MBEDTLS_MPI_CHK( esp_mont_hw_op(Z, Z, Z, M, Mprime, hw_words, true) );
            }

            // 2.2 if y[i] = 1 then z = mont(A, x_)
            if (mbedtls_mpi_get_bit(Y, i)) {
                MBEDTLS_MPI_CHK( esp_mont_hw_op(Z, Z, &X_, M, Mprime, hw_words, true) );
            }
        }

        // 3 z = Mont(z, 1)
        MBEDTLS_MPI_CHK( esp_mont_hw_op(Z, Z, &one, M, Mprime, hw_words, true) );
    }

cleanup:
    esp_mpi_disable_hardware_hw_op();

cleanup2:
    mbedtls_mpi_free(&X_);
    mbedtls_mpi_free(&one);
    return ret;
}

#endif //USE_MONT_EXPONENATIATION


/*
 * Z = X ^ Y mod M
 *
 * _Rinv is optional pre-calculated version of Rinv (via calculate_rinv()).
 *
 * (See RSA Accelerator section in Technical Reference for more about Mprime, Rinv)
 *
 */
static int esp_mpi_exp_mod( mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M, mbedtls_mpi *_Rinv )
{
    int ret = 0;

    mbedtls_mpi Rinv_new; /* used if _Rinv == NULL */
    mbedtls_mpi *Rinv;    /* points to _Rinv (if not NULL) othwerwise &RR_new */
    mbedtls_mpi_uint Mprime;

    size_t x_words = mpi_words(X);
    size_t y_words = mpi_words(Y);
    size_t m_words = mpi_words(M);

    /* "all numbers must be the same length", so choose longest number
       as cardinal length of operation...
    */
    size_t num_words = mpi_hal_calc_hardware_words(MAX(m_words, MAX(x_words, y_words)));

    if (num_words * 32 > SOC_RSA_MAX_BIT_LEN) {
        return MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
    }

    if (mbedtls_mpi_cmp_int(M, 0) <= 0 || (M->MBEDTLS_PRIVATE(p[0]) & 1) == 0) {
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }

    if (mbedtls_mpi_cmp_int(Y, 0) < 0) {
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    }

    if (mbedtls_mpi_cmp_int(Y, 0) == 0) {
        return mbedtls_mpi_lset(Z, 1);
    }

    /* Determine RR pointer, either _RR for cached value
       or local RR_new */
    if (_Rinv == NULL) {
        mbedtls_mpi_init(&Rinv_new);
        Rinv = &Rinv_new;
    } else {
        Rinv = _Rinv;
    }
    if (Rinv->MBEDTLS_PRIVATE(p) == NULL) {
        MBEDTLS_MPI_CHK(calculate_rinv(Rinv, M, num_words));
    }

    Mprime = modular_inverse(M);

    // Montgomery exponentiation: Z = X ^ Y mod M  (HAC 14.94)
#ifdef ESP_MPI_USE_MONT_EXP
    ret = mpi_montgomery_exp_calc(Z, X, Y, M, Rinv, num_words, Mprime) ;
    MBEDTLS_MPI_CHK(ret);
#else
    esp_mpi_enable_hardware_hw_op();

#if defined (CONFIG_MBEDTLS_MPI_USE_INTERRUPT)
    if (esp_mpi_isr_initialise() != ESP_OK) {
        ret = -1;
        esp_mpi_disable_hardware_hw_op();
        goto cleanup;
    }
#endif

    esp_mpi_exp_mpi_mod_hw_op(X, Y, M, Rinv, Mprime, num_words);
    ret = mbedtls_mpi_grow(Z, m_words);
    if (ret != 0) {
        esp_mpi_disable_hardware_hw_op();
        goto cleanup;
    }

#if defined(CONFIG_MBEDTLS_MPI_USE_INTERRUPT)
    ret = esp_mpi_wait_intr();
    if (ret != 0) {
        esp_mpi_disable_hardware_hw_op();
        goto cleanup;
    }
#endif //CONFIG_MBEDTLS_MPI_USE_INTERRUPT

    /* Read back the result */
    mpi_hal_read_result_hw_op(Z->MBEDTLS_PRIVATE(p), Z->MBEDTLS_PRIVATE(n), m_words);

    esp_mpi_disable_hardware_hw_op();
#endif

    // Compensate for negative X
    if (X->MBEDTLS_PRIVATE(s) == -1 && (Y->MBEDTLS_PRIVATE(p[0]) & 1) != 0) {
        Z->MBEDTLS_PRIVATE(s) = -1;
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(Z, M, Z));
    } else {
        Z->MBEDTLS_PRIVATE(s) = 1;
    }

cleanup:
    if (_Rinv == NULL) {
        mbedtls_mpi_free(&Rinv_new);
    }
    return ret;
}

#endif /* (MBEDTLS_MPI_EXP_MOD_ALT || MBEDTLS_MPI_EXP_MOD_ALT_FALLBACK) */

/*
 * Sliding-window exponentiation: X = A^E mod N  (HAC 14.85)
 */
int mbedtls_mpi_exp_mod( mbedtls_mpi *X, const mbedtls_mpi *A,
                         const mbedtls_mpi *E, const mbedtls_mpi *N,
                         mbedtls_mpi *_RR )
{
    int ret;
#if defined(MBEDTLS_MPI_EXP_MOD_ALT_FALLBACK)
    /* Try hardware API first and then fallback to software */
    ret = esp_mpi_exp_mod( X, A, E, N, _RR );
    if( ret == MBEDTLS_ERR_MPI_NOT_ACCEPTABLE ) {
        ret = mbedtls_mpi_exp_mod_soft( X, A, E, N, _RR );
    }
#else
    /* Hardware approach */
    ret = esp_mpi_exp_mod( X, A, E, N, _RR );
#endif
    /* Note: For software only approach, it gets handled in mbedTLS library.
    This file is not part of build objects for that case */

    return ret;
}

#if defined(MBEDTLS_MPI_MUL_MPI_ALT) /* MBEDTLS_MPI_MUL_MPI_ALT */

static int mpi_mult_mpi_failover_mod_mult( mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t z_words);
static int mpi_mult_mpi_overlong(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t y_words, size_t z_words);

/* Z = X * Y */
int mbedtls_mpi_mul_mpi( mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y )
{
    int ret = 0;
    size_t x_bits = mbedtls_mpi_bitlen(X);
    size_t y_bits = mbedtls_mpi_bitlen(Y);
    size_t x_words = bits_to_words(x_bits);
    size_t y_words = bits_to_words(y_bits);
    size_t z_words = bits_to_words(x_bits + y_bits);
    size_t hw_words = mpi_hal_calc_hardware_words(MAX(x_words, y_words)); // length of one operand in hardware

    /* Short-circuit eval if either argument is 0 or 1.

       This is needed as the mpi modular division
       argument will sometimes call in here when one
       argument is too large for the hardware unit, but the other
       argument is zero or one.
    */
    if (x_bits == 0 || y_bits == 0) {
        mbedtls_mpi_lset(Z, 0);
        return 0;
    }
    if (x_bits == 1) {
        ret = mbedtls_mpi_copy(Z, Y);
        Z->MBEDTLS_PRIVATE(s) *= X->MBEDTLS_PRIVATE(s);
        return ret;
    }
    if (y_bits == 1) {
        ret = mbedtls_mpi_copy(Z, X);
        Z->MBEDTLS_PRIVATE(s) *= Y->MBEDTLS_PRIVATE(s);
        return ret;
    }

    /* Grow Z to result size early, avoid interim allocations */
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow(Z, z_words) );

    /* If either factor is over 2048 bits, we can't use the standard hardware multiplier
       (it assumes result is double longest factor, and result is max 4096 bits.)

       However, we can fail over to mod_mult for up to 4096 bits of result (modulo
       multiplication doesn't have the same restriction, so result is simply the
       number of bits in X plus number of bits in in Y.)
    */
    if (hw_words * 32 > SOC_RSA_MAX_BIT_LEN/2) {
        if (z_words * 32 <= SOC_RSA_MAX_BIT_LEN) {
            /* Note: it's possible to use mpi_mult_mpi_overlong
               for this case as well, but it's very slightly
               slower and requires a memory allocation.
            */
            return mpi_mult_mpi_failover_mod_mult(Z, X, Y, z_words);
        } else {
            /* Still too long for the hardware unit... */
            if (y_words > x_words) {
                return mpi_mult_mpi_overlong(Z, X, Y, y_words, z_words);
            } else {
                return mpi_mult_mpi_overlong(Z, Y, X, x_words, z_words);
            }
        }
    }

    /* Otherwise, we can use the (faster) multiply hardware unit */
    esp_mpi_enable_hardware_hw_op();

    esp_mpi_mul_mpi_hw_op(X, Y, hw_words);

    /* Read back the result */
    mpi_hal_read_result_hw_op(Z->MBEDTLS_PRIVATE(p), Z->MBEDTLS_PRIVATE(n), z_words);

    esp_mpi_disable_hardware_hw_op();

    Z->MBEDTLS_PRIVATE(s) = X->MBEDTLS_PRIVATE(s) * Y->MBEDTLS_PRIVATE(s);

cleanup:
    return ret;
}

int mbedtls_mpi_mul_int( mbedtls_mpi *X, const mbedtls_mpi *A, mbedtls_mpi_uint b )
{
    mbedtls_mpi _B;
    mbedtls_mpi_uint p[1];

    _B.MBEDTLS_PRIVATE(s) = 1;
    _B.MBEDTLS_PRIVATE(n) = 1;
    _B.MBEDTLS_PRIVATE(p) = p;
    p[0] = b;

    return( mbedtls_mpi_mul_mpi( X, A, &_B ) );
}

/* Deal with the case when X & Y are too long for the hardware unit, by splitting one operand
   into two halves.

   Y must be the longer operand

   Slice Y into Yp, Ypp such that:
   Yp = lower 'b' bits of Y
   Ypp = upper 'b' bits of Y (right shifted)

   Such that
   Z = X * Y
   Z = X * (Yp + Ypp<<b)
   Z = (X * Yp) + (X * Ypp<<b)

   Note that this function may recurse multiple times, if both X & Y
   are too long for the hardware multiplication unit.
*/
static int mpi_mult_mpi_overlong(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t y_words, size_t z_words)
{
    int ret = 0;
    mbedtls_mpi Ztemp;
    /* Rather than slicing in two on bits we slice on limbs (32 bit words) */
    const size_t words_slice = y_words / 2;
    /* Yp holds lower bits of Y (declared to reuse Y's array contents to save on copying) */
    const mbedtls_mpi Yp = {
        .MBEDTLS_PRIVATE(p) = Y->MBEDTLS_PRIVATE(p),
        .MBEDTLS_PRIVATE(n) = words_slice,
        .MBEDTLS_PRIVATE(s) = Y->MBEDTLS_PRIVATE(s)
    };
    /* Ypp holds upper bits of Y, right shifted (also reuses Y's array contents) */
    const mbedtls_mpi Ypp = {
        .MBEDTLS_PRIVATE(p) = Y->MBEDTLS_PRIVATE(p) + words_slice,
        .MBEDTLS_PRIVATE(n) = y_words - words_slice,
        .MBEDTLS_PRIVATE(s) = Y->MBEDTLS_PRIVATE(s)
    };
    mbedtls_mpi_init(&Ztemp);

    /* Get result Ztemp = Yp * X (need temporary variable Ztemp) */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi(&Ztemp, X, &Yp) );

    /* Z = Ypp * Y */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi(Z, X, &Ypp) );

    /* Z = Z << b */
    MBEDTLS_MPI_CHK( mbedtls_mpi_shift_l(Z, words_slice * 32) );

    /* Z += Ztemp */
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_mpi(Z, Z, &Ztemp) );

cleanup:
    mbedtls_mpi_free(&Ztemp);

    return ret;
}


static int mpi_mult_mpi_failover_mod_mult( mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t z_words)
{
    int ret;
    size_t hw_words = mpi_hal_calc_hardware_words(z_words);

    esp_mpi_enable_hardware_hw_op();

    esp_mpi_mult_mpi_failover_mod_mult_hw_op(X, Y, hw_words );
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow(Z, hw_words) );

    /* Read back the result */
    mpi_hal_read_result_hw_op(Z->MBEDTLS_PRIVATE(p), Z->MBEDTLS_PRIVATE(n), hw_words);

    Z->MBEDTLS_PRIVATE(s) = X->MBEDTLS_PRIVATE(s) * Y->MBEDTLS_PRIVATE(s);

    /*
     * Relevant: https://github.com/espressif/esp-idf/issues/11850
     *
     * If z_words < mpi_words(Z) (the actual words taken by the MPI result),
     * the assert fails due to unsigned arithmetic - most likely hardware
     * peripheral has produced an incorrect result for MPI operation.
     * This can happen if data fed to the peripheral register was incorrect.
     *
     * z_words is calculated as the worst-case possible size of the result
     * MPI Z. The difference between z_words and the actual words taken by
     * the MPI result (mpi_words(Z)) can be a maximum of 1 word.
     * The value z_bits (actual bits taken by the MPI result) is calculated
     * as x_bits + y_bits bits, however, in some cases, z_bits can be
     * x_bits + y_bits - 1 bits (see example below).
     * 0b1111 * 0b1111 = 0b11100001 -> 8 bits
     * 0b1000 * 0b1000 = 0b01000000 -> 7 bits.
     * The code rounds up to the nearest word size, so the maximum difference
     * could be of only 1 word. The assert handles this.
     *
     */
    assert(z_words - mpi_words(Z) <= (size_t)1);
cleanup:
    esp_mpi_disable_hardware_hw_op();
    return ret;
}

#endif /* MBEDTLS_MPI_MUL_MPI_ALT */
