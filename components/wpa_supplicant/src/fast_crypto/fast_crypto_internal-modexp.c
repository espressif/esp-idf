// Hardware crypto support Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "crypto/includes.h"

#include "crypto/common.h"
#include "crypto/crypto.h"
#include "mbedtls/bignum.h"

int 
fast_crypto_mod_exp(const uint8_t *base, size_t base_len,
		    const uint8_t *power, size_t power_len,
		    const uint8_t *modulus, size_t modulus_len,
		    uint8_t *result, size_t *result_len)
{
    mbedtls_mpi bn_base, bn_exp, bn_modulus, bn_result, bn_rinv;
    int32_t ret = 0;
    mbedtls_mpi_init(&bn_base);
    mbedtls_mpi_init(&bn_exp);
    mbedtls_mpi_init(&bn_modulus);
    mbedtls_mpi_init(&bn_result);
    mbedtls_mpi_init(&bn_rinv);

    mbedtls_mpi_read_binary(&bn_base, base, base_len);
    mbedtls_mpi_read_binary(&bn_exp, power, power_len);
    mbedtls_mpi_read_binary(&bn_modulus, modulus, modulus_len);

    ret = mbedtls_mpi_exp_mod(&bn_result, &bn_base, &bn_exp, &bn_modulus, &bn_rinv);
    if (ret < 0) {
        mbedtls_mpi_free(&bn_base);
        mbedtls_mpi_free(&bn_exp);
        mbedtls_mpi_free(&bn_modulus);
        mbedtls_mpi_free(&bn_result);
        mbedtls_mpi_free(&bn_rinv);
        return ret;       
    }
    
    ret = mbedtls_mpi_write_binary(&bn_result, result, *result_len);


    mbedtls_mpi_free(&bn_base);
    mbedtls_mpi_free(&bn_exp);
    mbedtls_mpi_free(&bn_modulus);
    mbedtls_mpi_free(&bn_result);
    mbedtls_mpi_free(&bn_rinv);

    return ret;
}
