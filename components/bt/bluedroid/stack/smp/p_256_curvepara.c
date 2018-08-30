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

#include <string.h>
#include "p_256_ecc_pp.h"

void p_256_init_curve(UINT32 keyLength)
{
    elliptic_curve_t *ec;

    if (keyLength == KEY_LENGTH_DWORDS_P256) {
        ec = &curve_p256;

        ec->p[7] = 0xFFFFFFFF;
        ec->p[6] = 0x00000001;
        ec->p[5] = 0x0;
        ec->p[4] = 0x0;
        ec->p[3] = 0x0;
        ec->p[2] = 0xFFFFFFFF;
        ec->p[1] = 0xFFFFFFFF;
        ec->p[0] = 0xFFFFFFFF;

        memset(ec->omega, 0, KEY_LENGTH_DWORDS_P256 * sizeof(ec->omega[0]));
        memset(ec->a, 0, KEY_LENGTH_DWORDS_P256 * sizeof(ec->a[0]));

        ec->a_minus3 = TRUE;

        //b
        ec->b[7] =  0x5ac635d8;
        ec->b[6] =  0xaa3a93e7;
        ec->b[5] =  0xb3ebbd55;
        ec->b[4] =  0x769886bc;
        ec->b[3] =  0x651d06b0;
        ec->b[2] =  0xcc53b0f6;
        ec->b[1] =  0x3bce3c3e;
        ec->b[0] =  0x27d2604b;

        //base point
        ec->G.x[7] =  0x6b17d1f2;
        ec->G.x[6] =  0xe12c4247;
        ec->G.x[5] =  0xf8bce6e5;
        ec->G.x[4] =  0x63a440f2;
        ec->G.x[3] =  0x77037d81;
        ec->G.x[2] =  0x2deb33a0;
        ec->G.x[1] =  0xf4a13945;
        ec->G.x[0] =  0xd898c296;

        ec->G.y[7] =  0x4fe342e2;
        ec->G.y[6] =  0xfe1a7f9b;
        ec->G.y[5] =  0x8ee7eb4a;
        ec->G.y[4] =  0x7c0f9e16;
        ec->G.y[3] =  0x2bce3357;
        ec->G.y[2] =  0x6b315ece;
        ec->G.y[1] =  0xcbb64068;
        ec->G.y[0] =  0x37bf51f5;
    }
}

