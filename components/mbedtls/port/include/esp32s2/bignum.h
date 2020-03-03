#ifndef _ESP_BIGNUM_H_
#define _ESP_BIGNUM_H_

#include <mbedtls/bignum.h>

/* Use montgomery exponentiation (HAC 14.94) for calculating X ^ Y mod M,
   this may be faster for targets without acceleration options (e.g. ESP32)
*/
#define USE_MONT_EXPONENATIATION 1

size_t hardware_words(size_t words);

void esp_mpi_mul_mpi_mod_ll(const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M, const mbedtls_mpi *Rinv, mbedtls_mpi_uint Mprime, size_t hw_words);
void esp_mpi_exp_mpi_mod_ll(const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M, const mbedtls_mpi *Rinv, mbedtls_mpi_uint Mprime, size_t hw_words);

void esp_mpi_mul_mpi_ll(const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words);

void esp_mpi_read_result_ll(mbedtls_mpi *Z, size_t z_words);

void esp_mpi_enable_hardware_ll( void );
void esp_mpi_disable_hardware_ll( void );

//inline size_t hardware_words(size_t words);

void esp_mpi_mult_mpi_failover_mod_mult_ll(const mbedtls_mpi *X, const mbedtls_mpi *Y, size_t num_words);
int esp_mont_ll(mbedtls_mpi* Z, const mbedtls_mpi* X, const mbedtls_mpi* Y, const mbedtls_mpi* M,
                mbedtls_mpi_uint Mprime,
                size_t hw_words,
                bool again);

#endif