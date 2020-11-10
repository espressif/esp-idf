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
//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "p_256_ecc_pp.h"
#include "p_256_multprecision.h"
#include "common/bt_target.h"

#if SMP_DYNAMIC_MEMORY == FALSE
elliptic_curve_t curve;
elliptic_curve_t curve_p256;
#else
elliptic_curve_t *curve_ptr;
elliptic_curve_t *curve_p256_ptr;
#endif

static void p_256_init_point(Point *q)
{
    memset(q, 0, sizeof(Point));
}

static void p_256_copy_point(Point *q, Point *p)
{
    memcpy(q, p, sizeof(Point));
}

// q=2q
static void ECC_Double(Point *q, Point *p, uint32_t keyLength)
{
    DWORD t1[KEY_LENGTH_DWORDS_P256];
    DWORD t2[KEY_LENGTH_DWORDS_P256];
    DWORD t3[KEY_LENGTH_DWORDS_P256];
    DWORD *x1;
    DWORD *x3;
    DWORD *y1;
    DWORD *y3;
    DWORD *z1;
    DWORD *z3;

    if (multiprecision_iszero(p->z, keyLength)) {
        multiprecision_init(q->z, keyLength);
        return;                                     // return infinity
    }

    x1 = p->x; y1 = p->y; z1 = p->z;
    x3 = q->x; y3 = q->y; z3 = q->z;

    multiprecision_mersenns_squa_mod(t1, z1, keyLength);  // t1=z1^2
    multiprecision_sub_mod(t2, x1, t1, keyLength);  // t2=x1-t1
    multiprecision_add_mod(t1, x1, t1, keyLength);  // t1=x1+t1
    multiprecision_mersenns_mult_mod(t2, t1, t2, keyLength);  // t2=t2*t1
    multiprecision_lshift_mod(t3, t2, keyLength);
    multiprecision_add_mod(t2, t3, t2, keyLength);  // t2=3t2

    multiprecision_mersenns_mult_mod(z3, y1, z1, keyLength);  // z3=y1*z1
    multiprecision_lshift_mod(z3, z3, keyLength);

    multiprecision_mersenns_squa_mod(y3, y1, keyLength);  // y3=y1^2
    multiprecision_lshift_mod(y3, y3, keyLength);
    multiprecision_mersenns_mult_mod(t3, y3, x1, keyLength);  // t3=y3*x1=x1*y1^2
    multiprecision_lshift_mod(t3, t3, keyLength);
    multiprecision_mersenns_squa_mod(y3, y3, keyLength);  // y3=y3^2=y1^4
    multiprecision_lshift_mod(y3, y3, keyLength);

    multiprecision_mersenns_squa_mod(x3, t2, keyLength);  // x3=t2^2
    multiprecision_lshift_mod(t1, t3, keyLength);                // t1=2t3
    multiprecision_sub_mod(x3, x3, t1, keyLength);               // x3=x3-t1
    multiprecision_sub_mod(t1, t3, x3, keyLength);               // t1=t3-x3
    multiprecision_mersenns_mult_mod(t1, t1, t2, keyLength);  // t1=t1*t2
    multiprecision_sub_mod(y3, t1, y3, keyLength);               // y3=t1-y3
}

// q=q+p,     zp must be 1
static void ECC_Add(Point *r, Point *p, Point *q, uint32_t keyLength)
{
    DWORD t1[KEY_LENGTH_DWORDS_P256];
    DWORD t2[KEY_LENGTH_DWORDS_P256];
    DWORD *x1;
    DWORD *x2;
    DWORD *x3;
    DWORD *y1;
    DWORD *y2;
    DWORD *y3;
    DWORD *z1;
    DWORD *z2;
    DWORD *z3;

    x1 = p->x; y1 = p->y; z1 = p->z;
    x2 = q->x; y2 = q->y; z2 = q->z;
    x3 = r->x; y3 = r->y; z3 = r->z;

    // if Q=infinity, return p
    if (multiprecision_iszero(z2, keyLength)) {
        p_256_copy_point(r, p);
        return;
    }

    // if P=infinity, return q
    if (multiprecision_iszero(z1, keyLength)) {
        p_256_copy_point(r, q);
        return;
    }

    multiprecision_mersenns_squa_mod(t1, z1, keyLength);      // t1=z1^2
    multiprecision_mersenns_mult_mod(t2, z1, t1, keyLength);  // t2=t1*z1
    multiprecision_mersenns_mult_mod(t1, x2, t1, keyLength);  // t1=t1*x2
    multiprecision_mersenns_mult_mod(t2, y2, t2, keyLength);  // t2=t2*y2

    multiprecision_sub_mod(t1, t1, x1, keyLength);  // t1=t1-x1
    multiprecision_sub_mod(t2, t2, y1, keyLength);  // t2=t2-y1

    if (multiprecision_iszero(t1, keyLength)) {
        if (multiprecision_iszero(t2, keyLength)) {
            ECC_Double(r, q, keyLength) ;
            return;
        } else {
            multiprecision_init(z3, keyLength);
            return;                             // return infinity
        }
    }

    multiprecision_mersenns_mult_mod(z3, z1, t1, keyLength);  // z3=z1*t1
    multiprecision_mersenns_squa_mod(y3, t1, keyLength);      // t3=t1^2
    multiprecision_mersenns_mult_mod(z1, y3, t1, keyLength);  // t4=t3*t1
    multiprecision_mersenns_mult_mod(y3, y3, x1, keyLength);  // t3=t3*x1
    multiprecision_lshift_mod(t1, y3, keyLength);            // t1=2*t3
    multiprecision_mersenns_squa_mod(x3, t2, keyLength);      // x3=t2^2
    multiprecision_sub_mod(x3, x3, t1, keyLength);           // x3=x3-t1
    multiprecision_sub_mod(x3, x3, z1, keyLength);           // x3=x3-t4
    multiprecision_sub_mod(y3, y3, x3, keyLength);           // t3=t3-x3
    multiprecision_mersenns_mult_mod(y3, y3, t2, keyLength);  // t3=t3*t2
    multiprecision_mersenns_mult_mod(z1, z1, y1, keyLength);  // t4=t4*t1
    multiprecision_sub_mod(y3, y3, z1, keyLength);
}

// Computing the Non-Adjacent Form of a positive integer
static void ECC_NAF(uint8_t *naf, uint32_t *NumNAF, DWORD *k, uint32_t keyLength)
{
    uint32_t sign;
    int i = 0;
    int j;
    uint32_t var;

    while ((var = multiprecision_most_signbits(k, keyLength)) >= 1) {
        if (k[0] & 0x01) { // k is odd
            sign = (k[0] & 0x03);  // 1 or 3

            // k = k-naf[i]
            if (sign == 1) {
                k[0] = k[0] & 0xFFFFFFFE;
            } else {
                k[0] = k[0] + 1;
                if (k[0] == 0) { //overflow
                    j = 1;
                    do {
                        k[j]++;
                    } while (k[j++] == 0); //overflow
                }
            }
        } else {
            sign = 0;
        }

        multiprecision_rshift(k, k, keyLength);
        naf[i / 4] |= (sign) << ((i % 4) * 2);
        i++;
    }

    *NumNAF = i;
}

// Binary Non-Adjacent Form for point multiplication
void ECC_PointMult_Bin_NAF(Point *q, Point *p, DWORD *n, uint32_t keyLength)
{
    uint32_t sign;
    UINT8 naf[256 / 4 + 1];
    uint32_t NumNaf;
    Point minus_p;
    Point r;
    DWORD *modp;

    if (keyLength == KEY_LENGTH_DWORDS_P256) {
        modp = curve_p256.p;
    } else {
        modp = curve.p;
    }

    p_256_init_point(&r);
    multiprecision_init(p->z, keyLength);
    p->z[0] = 1;

    // initialization
    p_256_init_point(q);

    // -p
    multiprecision_copy(minus_p.x, p->x, keyLength);
    multiprecision_sub(minus_p.y, modp, p->y, keyLength);

    multiprecision_init(minus_p.z, keyLength);
    minus_p.z[0] = 1;

    // NAF
    memset(naf, 0, sizeof(naf));
    ECC_NAF(naf, &NumNaf, n, keyLength);

    for (int i = NumNaf - 1; i >= 0; i--) {
        p_256_copy_point(&r, q);
        ECC_Double(q, &r, keyLength);
        sign = (naf[i / 4] >> ((i % 4) * 2)) & 0x03;

        if (sign == 1) {
            p_256_copy_point(&r, q);
            ECC_Add(q, &r, p, keyLength);
        } else if (sign == 3) {
            p_256_copy_point(&r, q);
            ECC_Add(q, &r, &minus_p, keyLength);
        }
    }

    multiprecision_inv_mod(minus_p.x, q->z, keyLength);
    multiprecision_mersenns_squa_mod(q->z, minus_p.x, keyLength);
    multiprecision_mersenns_mult_mod(q->x, q->x, q->z, keyLength);
    multiprecision_mersenns_mult_mod(q->z, q->z, minus_p.x, keyLength);
    multiprecision_mersenns_mult_mod(q->y, q->y, q->z, keyLength);
}

bool ECC_CheckPointIsInElliCur_P256(Point *p)
{
    /* y^2 % q */
    DWORD y_y_q[KEY_LENGTH_DWORDS_P256] = {0x0};
    /* x^2 % q */
    DWORD x_x_q[KEY_LENGTH_DWORDS_P256] = {0x0};
    /* x % q */
    DWORD x_q[KEY_LENGTH_DWORDS_P256] = {0x0};
    /* x^2, To prevent overflow, the length of the x square here needs to
       be expanded to two times the original one. */
    DWORD x_x[2*KEY_LENGTH_DWORDS_P256] = {0x0};
    /* y_y_q =(p->y)^2(mod q) */
    multiprecision_mersenns_squa_mod(y_y_q, p->y, KEY_LENGTH_DWORDS_P256);
    /* Calculate the value of p->x square, x_x = (p->x)^2 */
    multiprecision_mult(x_x, p->x, p->x, KEY_LENGTH_DWORDS_P256);
    /* The function of the elliptic curve is y^2 = x^3 - 3x + b (mod q) ==>
       y^2 = (x^2 - 3)*x + b (mod q),
       so we calculate the x^2 - 3 value here */
    x_x[0] -= 3;
    /* Using math relations. (a*b) % q = ((a%q)*(b%q)) % q ==>
      (x^2 - 3)*x = (((x^2 - 3) % q) * x % q) % q */
    multiprecision_fast_mod_P256(x_x_q, x_x);
    /* x_x = x_x_q * x_q */
    multiprecision_mult(x_x, x_x_q, p->x, KEY_LENGTH_DWORDS_P256);
    /* x_q = x_x % q */
    multiprecision_fast_mod_P256(x_q, x_x);
    /* Save the result in x_x_q */
    multiprecision_add_mod(x_x_q, x_q, curve_p256.b, KEY_LENGTH_DWORDS_P256);
    /* compare the y_y_q and x_x_q, see if they are on a given elliptic curve. */
    if (multiprecision_compare(y_y_q, x_x_q, KEY_LENGTH_DWORDS_P256)) {
        return false;
    } else {
        return true;
    }
}
