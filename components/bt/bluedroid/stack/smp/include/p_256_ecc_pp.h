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
 *  This file contains simple pairing algorithms using Elliptic Curve Cryptography for private public key
 *
 ******************************************************************************/

#pragma once

#include "p_256_multprecision.h"

typedef unsigned long  DWORD;

typedef struct {
    DWORD x[KEY_LENGTH_DWORDS_P256];
    DWORD y[KEY_LENGTH_DWORDS_P256];
    DWORD z[KEY_LENGTH_DWORDS_P256];
} Point;

typedef struct {
    // curve's coefficients
    DWORD a[KEY_LENGTH_DWORDS_P256];
    DWORD b[KEY_LENGTH_DWORDS_P256];

    //whether a is -3
    int a_minus3;

    // prime modulus
    DWORD p[KEY_LENGTH_DWORDS_P256];

    // Omega, p = 2^m -omega
    DWORD omega[KEY_LENGTH_DWORDS_P256];

    // base point, a point on E of order r
    Point G;

} elliptic_curve_t;

extern elliptic_curve_t curve;
extern elliptic_curve_t curve_p256;

void ECC_PointMult_Bin_NAF(Point *q, Point *p, DWORD *n, uint32_t keyLength);

#define ECC_PointMult(q, p, n, keyLength)  ECC_PointMult_Bin_NAF(q, p, n, keyLength)

void p_256_init_curve(UINT32 keyLength);


