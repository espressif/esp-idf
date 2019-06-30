/******************************************************************************
 *
 *  Copyright (C) 2006-2015 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This file contains simple pairing algorithms
 *
 ******************************************************************************/
#pragma once

#include "stack/bt_types.h"

/* Type definitions */
typedef unsigned long  DWORD;

#define DWORD_BITS      32
#define DWORD_BYTES     4
#define DWORD_BITS_SHIFT 5

#define KEY_LENGTH_DWORDS_P192 6
#define KEY_LENGTH_DWORDS_P256 8
/* Arithmetic Operations */

int multiprecision_compare(DWORD *a, DWORD *b, uint32_t keyLength);
int multiprecision_iszero(DWORD *a, uint32_t keyLength);
void multiprecision_init(DWORD *c, uint32_t keyLength);
void multiprecision_copy(DWORD *c, DWORD *a, uint32_t keyLength);
UINT32 multiprecision_dword_bits (DWORD a);
UINT32 multiprecision_most_signdwords(DWORD *a, uint32_t keyLength);
UINT32 multiprecision_most_signbits(DWORD *a, uint32_t keyLength);
void multiprecision_inv_mod(DWORD *aminus, DWORD *a, uint32_t keyLength);
DWORD multiprecision_add(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength); // c=a+b
void multiprecision_add_mod(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength);
DWORD multiprecision_sub(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength); // c=a-b
void multiprecision_sub_mod(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength);
void multiprecision_rshift(DWORD *c, DWORD *a, uint32_t keyLength);    // c=a>>1, return carrier
void multiprecision_lshift_mod(DWORD *c, DWORD *a, uint32_t keyLength);   // c=a<<b, return carrier
DWORD multiprecision_lshift(DWORD *c, DWORD *a, uint32_t keyLength);    // c=a<<b, return carrier
void multiprecision_mult(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength); // c=a*b
void multiprecision_mersenns_mult_mod(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength);
void multiprecision_mersenns_squa_mod(DWORD *c, DWORD *a, uint32_t keyLength);
DWORD multiprecision_lshift(DWORD *c, DWORD *a, uint32_t keyLength);
void multiprecision_mult(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength);
void multiprecision_fast_mod(DWORD *c, DWORD *a);
void multiprecision_fast_mod_P256(DWORD *c, DWORD *a);


