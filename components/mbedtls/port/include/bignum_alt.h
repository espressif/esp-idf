/**
 * \file bignum_alt.h
 *
 * \brief  Multi-precision integer library
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
 
#ifndef BIGNUM_ALT_H
#define BIGNUM_ALT_H

#include "bignum.h"

#if defined(MBEDTLS_BIGNUM_ALT)

typedef MPI_CTX	mbedtls_mpi;
	
#define mbedtls_mpi_init 				esp_mpi_init	
#define mbedtls_mpi_free				esp_mpi_free	
#define mbedtls_mpi_grow				esp_mpi_grow	
#define mbedtls_mpi_shrink				esp_mpi_shrink	
#define mbedtls_mpi_copy				esp_mpi_copy	
#define mbedtls_mpi_swap				esp_mpi_swap	
#define mbedtls_mpi_safe_cond_assign	esp_mpi_safe_cond_assign				
#define mbedtls_mpi_safe_cond_swap		esp_mpi_safe_cond_swap			
#define mbedtls_mpi_lset				esp_mpi_lset	
#define mbedtls_mpi_get_bit				esp_mpi_get_bit	
#define mbedtls_mpi_set_bit				esp_mpi_set_bit	
#define mbedtls_mpi_lsb					esp_mpi_lsb
#define mbedtls_mpi_bitlen				esp_mpi_bitlen	
#define mbedtls_mpi_size				esp_mpi_size	
#define mbedtls_mpi_read_string			esp_mpi_read_string		
#define mbedtls_mpi_write_string		esp_mpi_write_string			
#define mbedtls_mpi_read_binary			esp_mpi_read_binary		
#define mbedtls_mpi_write_binary		esp_mpi_write_binary			
#define mbedtls_mpi_shift_l				esp_mpi_shift_l	
#define mbedtls_mpi_shift_r				esp_mpi_shift_r	
#define mbedtls_mpi_cmp_abs				esp_mpi_cmp_abs	
#define mbedtls_mpi_cmp_mpi				esp_mpi_cmp_mpi	
#define mbedtls_mpi_cmp_int				esp_mpi_cmp_int	
#define mbedtls_mpi_add_abs				esp_mpi_add_abs	
#define mbedtls_mpi_sub_abs				esp_mpi_sub_abs	
#define mbedtls_mpi_add_mpi				esp_mpi_add_mpi	
#define mbedtls_mpi_sub_mpi				esp_mpi_sub_mpi	
#define mbedtls_mpi_add_int				esp_mpi_add_int	
#define mbedtls_mpi_sub_int				esp_mpi_sub_int	
#define mbedtls_mpi_mul_mpi				esp_mpi_mul_mpi	
#define mbedtls_mpi_mul_int				esp_mpi_mul_int	
#define mbedtls_mpi_div_mpi				esp_mpi_div_mpi	
#define mbedtls_mpi_div_int				esp_mpi_div_int	
#define mbedtls_mpi_mod_mpi				esp_mpi_mod_mpi	
#define mbedtls_mpi_mod_int				esp_mpi_mod_int	
#define mbedtls_mpi_exp_mod				esp_mpi_exp_mod	
#define mbedtls_mpi_fill_random			esp_mpi_fill_random		
#define mbedtls_mpi_gcd					esp_mpi_gcd	
#define mbedtls_mpi_inv_mod				esp_mpi_inv_mod	
#define mbedtls_mpi_is_prime 			esp_mpi_is_prime 		
#define mbedtls_mpi_gen_prime 			esp_mpi_gen_prime		
	
#endif

#endif

