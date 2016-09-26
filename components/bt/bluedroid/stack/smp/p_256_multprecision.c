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
#include "bt_target.h"
#include "p_256_ecc_pp.h"
#include "p_256_multprecision.h"

void multiprecision_init(DWORD *c, uint32_t keyLength)
{
    for (uint32_t i = 0; i < keyLength; i++)
        c[i] = 0;
}

void multiprecision_copy(DWORD *c, DWORD *a, uint32_t keyLength)
{
    for (uint32_t i = 0; i < keyLength; i++)
        c[i] = a[i];
}

int multiprecision_compare(DWORD *a, DWORD *b, uint32_t keyLength)
{
    for (int i = keyLength - 1; i >= 0; i--)
    {
        if (a[i] > b[i])
            return 1;
        if (a[i] < b[i])
            return -1;
    }
    return 0;
}

int multiprecision_iszero(DWORD *a, uint32_t keyLength)
{
    for (uint32_t i = 0; i < keyLength; i++)
        if (a[i])
            return 0;

    return 1;
}

UINT32 multiprecision_dword_bits(DWORD a)
{
    uint32_t i;
    for (i = 0; i < DWORD_BITS; i++, a >>= 1)
        if (a == 0)
            break;

    return i;
}

UINT32 multiprecision_most_signdwords(DWORD *a, uint32_t keyLength)
{
    int  i;
    for (i = keyLength - 1; i >= 0; i--)
        if (a[i])
           break;
    return (i + 1);
}

UINT32 multiprecision_most_signbits(DWORD *a, uint32_t keyLength)
{
    int aMostSignDWORDs;

    aMostSignDWORDs = multiprecision_most_signdwords(a, keyLength);
    if (aMostSignDWORDs == 0)
        return 0;

    return (((aMostSignDWORDs-1) << DWORD_BITS_SHIFT) +
            multiprecision_dword_bits(a[aMostSignDWORDs-1]) );
}

DWORD multiprecision_add(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength)
{
    DWORD carrier;
    DWORD temp;

    carrier=0;
    for (uint32_t i = 0; i < keyLength; i++)
    {
        temp = a[i] + carrier;
        carrier = (temp < carrier);
        temp += b[i];
        carrier |= (temp < b[i]);
        c[i]=temp;
    }

    return carrier;
}

//c=a-b
DWORD multiprecision_sub(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength)
{
    DWORD borrow;
    DWORD temp;

    borrow=0;
    for (uint32_t i=0; i < keyLength; i++)
    {
        temp = a[i] - borrow;
        borrow = (temp > a[i]);
        c[i] = temp - b[i];
        borrow |= (c[i] > temp);
    }

    return borrow;
}

// c = a << 1
void multiprecision_lshift_mod(DWORD * c, DWORD * a, uint32_t keyLength)
{
    DWORD carrier;
    DWORD *modp;

    if (keyLength == KEY_LENGTH_DWORDS_P192)
    {
        modp = curve.p;
    }
    else if (keyLength == KEY_LENGTH_DWORDS_P256)
    {
        modp = curve_p256.p;
    }
    else
        return;

    carrier = multiprecision_lshift(c, a, keyLength);
    if (carrier)
    {
        multiprecision_sub(c, c, modp, keyLength);
    }
    else if (multiprecision_compare(c, modp, keyLength)>=0)
    {
        multiprecision_sub(c, c, modp, keyLength);
    }
}

// c=a>>1
void multiprecision_rshift(DWORD * c, DWORD * a, uint32_t keyLength)
{
    int j;
    DWORD b = 1;

    j = DWORD_BITS - b;

    DWORD carrier = 0;
    DWORD temp;
    for (int i = keyLength-1; i >= 0; i--)
    {
        temp = a[i]; // in case of c==a
        c[i] = (temp >> b) | carrier;
        carrier = temp << j;
    }
}

// Curve specific optimization when p is a pseudo-Mersenns prime, p=2^(KEY_LENGTH_BITS)-omega
void multiprecision_mersenns_mult_mod(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength)
{
    DWORD cc[2*KEY_LENGTH_DWORDS_P256];

    multiprecision_mult(cc, a, b, keyLength);
    if (keyLength == 6)
    {
        multiprecision_fast_mod(c, cc);
    }
    else if (keyLength == 8)
    {
        multiprecision_fast_mod_P256(c, cc);
    }
}

// Curve specific optimization when p is a pseudo-Mersenns prime
void multiprecision_mersenns_squa_mod(DWORD *c, DWORD *a, uint32_t keyLength)
{
    multiprecision_mersenns_mult_mod(c, a, a, keyLength);
}

// c=(a+b) mod p, b<p, a<p
void multiprecision_add_mod(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength)
{
    DWORD carrier;
    DWORD *modp;

    if (keyLength == KEY_LENGTH_DWORDS_P192)
    {
        modp = curve.p;
    }
    else if (keyLength == KEY_LENGTH_DWORDS_P256)
    {
        modp = curve_p256.p;
    }
    else
        return;

    carrier = multiprecision_add(c, a, b, keyLength);
    if (carrier)
    {
        multiprecision_sub(c, c, modp, keyLength);
    }
    else if (multiprecision_compare(c, modp, keyLength) >= 0)
    {
        multiprecision_sub(c, c, modp, keyLength);
    }
}

// c=(a-b) mod p, a<p, b<p
void multiprecision_sub_mod(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength)
{
    DWORD borrow;
    DWORD *modp;

    if (keyLength == KEY_LENGTH_DWORDS_P192)
    {
        modp = curve.p;
    }
    else if(keyLength == KEY_LENGTH_DWORDS_P256)
    {
        modp = curve_p256.p;
    }
    else
        return;

    borrow = multiprecision_sub(c, a, b, keyLength);
    if(borrow)
        multiprecision_add(c, c, modp, keyLength);
}

// c=a<<b, b<DWORD_BITS, c has a buffer size of NumDWORDs+1
DWORD multiprecision_lshift(DWORD * c, DWORD * a, uint32_t keyLength)
{
    int j;
    uint32_t b = 1;
    j = DWORD_BITS - b;

    DWORD carrier = 0;
    DWORD temp;

    for (uint32_t i = 0; i < keyLength; i++)
    {
        temp = a[i];  // in case c==a
        c[i] = (temp << b) | carrier;
        carrier = temp >> j;
    }

    return carrier;
}

// c=a*b; c must have a buffer of 2*Key_LENGTH_DWORDS, c != a != b
void multiprecision_mult(DWORD *c, DWORD *a, DWORD *b, uint32_t keyLength)
{
    DWORD W;
    DWORD U;
    DWORD V;

    U = V = W = 0;
    multiprecision_init(c, keyLength);

    //assume little endian right now
    for (uint32_t i = 0; i < keyLength; i++)
    {
        U = 0;
        for (uint32_t j = 0; j < keyLength; j++)
        {
            uint64_t result;
            result = ((UINT64)a[i]) * ((uint64_t) b[j]);
            W = result >> 32;
            V = a[i] * b[j];
            V = V + U;
            U = (V < U);
            U += W;
            V = V + c[i+j];
            U += (V < c[i+j]);
            c[i+j] = V;
        }
        c[i+keyLength] = U;
    }
}

void multiprecision_fast_mod(DWORD *c, DWORD *a)
{
    DWORD U;
    DWORD V;
    DWORD *modp = curve.p;

    c[0] = a[0] + a[6];
    U=c[0] < a[0];
    c[0] += a[10];
    U += c[0] < a[10];

    c[1] = a[1] + U;
    U = c[1] < a[1];
    c[1] += a[7];
    U += c[1] < a[7];
    c[1] += a[11];
    U += c[1]< a[11];

    c[2] = a[2] + U;
    U = c[2] < a[2];
    c[2] += a[6];
    U += c[2] < a[6];
    c[2] += a[8];
    U += c[2] < a[8];
    c[2] += a[10];
    U += c[2] < a[10];

    c[3] = a[3]+U;
    U = c[3] < a[3];
    c[3] += a[7];
    U += c[3] < a[7];
    c[3] += a[9];
    U += c[3] < a[9];
    c[3] += a[11];
    U += c[3] < a[11];

    c[4] = a[4]+U;
    U = c[4] < a[4];
    c[4] += a[8];
    U += c[4] < a[8];
    c[4] += a[10];
    U += c[4] < a[10];

    c[5] = a[5]+U;
    U = c[5] < a[5];
    c[5] += a[9];
    U += c[5] < a[9];
    c[5] += a[11];
    U += c[5] < a[11];

    c[0] += U;
    V = c[0] < U;
    c[1] += V;
    V = c[1] < V;
    c[2] += V;
    V = c[2] < V;
    c[2] += U;
    V = c[2] < U;
    c[3] += V;
    V = c[3] < V;
    c[4] += V;
    V = c[4] < V;
    c[5] += V;
    V = c[5] < V;

    if (V)
    {
        multiprecision_sub(c, c, modp, KEY_LENGTH_DWORDS_P192);
    }
    else if(multiprecision_compare(c, modp, KEY_LENGTH_DWORDS_P192) >= 0)
    {
        multiprecision_sub(c, c, modp, KEY_LENGTH_DWORDS_P192);
    }
}

void multiprecision_fast_mod_P256(DWORD *c, DWORD *a)
{
    DWORD A;
    DWORD B;
    DWORD C;
    DWORD D;
    DWORD E;
    DWORD F;
    DWORD G;
    uint8_t UA;
    uint8_t UB;
    uint8_t UC;
    uint8_t UD;
    uint8_t UE;
    uint8_t UF;
    uint8_t UG;
    DWORD U;
    DWORD *modp = curve_p256.p;

    // C = a[13] + a[14] + a[15];
    C = a[13];
    C += a[14];
    UC = (C < a[14]);
    C += a[15];
    UC += (C < a[15]);

    // E = a[8] + a[9];
    E = a[8];
    E += a[9];
    UE = (E < a[9]);

    // F = a[9] + a[10];
    F = a[9];
    F += a[10];
    UF = (F < a[10]);

    // G = a[10] + a[11]
    G = a[10];
    G += a[11];
    UG = (G < a[11]);

    // B = a[12] + a[13] + a[14] + a[15] == C + a[12]
    B = C;
    UB = UC;
    B += a[12];
    UB += (B < a[12]);

    // A = a[11] + a[12] + a[13] + a[14] == B + a[11] - a[15]
    A = B;
    UA = UB;
    A += a[11];
    UA += (A < a[11]);
    UA -= (A < a[15]);
    A -= a[15];

    // D = a[10] + a[11] + a[12] + a[13] == A + a[10] - a[14]
    D = A;
    UD = UA;
    D += a[10];
    UD += (D < a[10]);
    UD -= (D < a[14]);
    D -= a[14];

    c[0] = a[0];
    c[0] += E;
    U = (c[0] < E);
    U += UE;
    U -= (c[0] < A);
    U -= UA;
    c[0] -= A;

    if (U & 0x80000000)
    {
        DWORD UU;
        UU = 0 - U;
        U = (a[1] < UU);
        c[1] = a[1] - UU;
    }
    else
    {
        c[1] = a[1] + U;
        U = (c[1] < a[1]);
    }

    c[1] += F;
    U += (c[1] < F);
    U += UF;
    U -= (c[1] < B);
    U -= UB;
    c[1] -= B;

    if (U & 0x80000000)
    {
        DWORD UU;
        UU = 0 - U;
        U = (a[2] < UU);
        c[2] = a[2] - UU;
    }
    else
    {
        c[2] = a[2] + U;
        U = (c[2] < a[2]);
    }

    c[2] += G;
    U += (c[2] < G);
    U += UG;
    U -= (c[2] < C);
    U -= UC;
    c[2] -= C;

    if (U & 0x80000000)
    {
        DWORD UU;
        UU = 0 - U;
        U = (a[3] < UU);
        c[3] = a[3] - UU;
    }
    else
    {
        c[3] = a[3] + U;
        U = (c[3] < a[3]);
    }

    c[3] += A;
    U += (c[3] < A);
    U += UA;
    c[3] += a[11];
    U += (c[3] < a[11]);
    c[3] += a[12];
    U += (c[3] < a[12]);
    U -= (c[3] < a[14]);
    c[3] -= a[14];
    U -= (c[3] < a[15]);
    c[3] -= a[15];
    U -= (c[3] < E);
    U -= UE;
    c[3] -= E;

    if (U & 0x80000000)
    {
        DWORD UU;
        UU = 0 - U;
        U = (a[4] < UU);
        c[4] = a[4] - UU;
    }
    else
    {
        c[4] = a[4] + U;
        U = (c[4] < a[4]);
    }

    c[4] += B;
    U += (c[4] < B);
    U += UB;
    U -= (c[4] < a[15]);
    c[4] -= a[15];
    c[4] += a[12];
    U += (c[4] < a[12]);
    c[4] += a[13];
    U += (c[4] < a[13]);
    U -= (c[4] < F);
    U -= UF;
    c[4] -= F;

    if (U & 0x80000000)
    {
        DWORD UU;
        UU = 0 - U;
        U = (a[5] < UU);
        c[5] = a[5] - UU;
    }
    else
    {
        c[5] = a[5] + U;
        U = (c[5] < a[5]);
    }

    c[5] += C;
    U += (c[5] < C);
    U += UC;
    c[5] += a[13];
    U += (c[5] < a[13]);
    c[5] += a[14];
    U += (c[5] < a[14]);
    U -= (c[5] < G);
    U -= UG;
    c[5] -= G;

    if (U & 0x80000000)
    {
        DWORD UU;
        UU = 0 - U;
        U = (a[6] < UU);
        c[6] = a[6] - UU;
    }
    else
    {
        c[6] = a[6] + U;
        U = (c[6] < a[6]);
    }

    c[6] += C;
    U += (c[6] < C);
    U += UC;
    c[6] += a[14];
    U += (c[6] < a[14]);
    c[6] += a[14];
    U += (c[6] < a[14]);
    c[6] += a[15];
    U += (c[6] < a[15]);
    U -= (c[6] < E);
    U -= UE;
    c[6] -= E;

    if (U & 0x80000000)
    {
        DWORD UU;
        UU = 0 - U;
        U = (a[7] < UU);
        c[7] = a[7] - UU;
    }
    else
    {
        c[7] = a[7] + U;
        U = (c[7] < a[7]);
    }

    c[7] += a[15];
    U += (c[7] < a[15]);
    c[7] += a[15];
    U += (c[7] < a[15]);
    c[7] += a[15];
    U += (c[7] < a[15]);
    c[7] += a[8];
    U += (c[7] < a[8]);
    U -= (c[7] < D);
    U -= UD;
    c[7] -= D;

    if (U & 0x80000000)
    {
        while (U)
        {
            multiprecision_add(c, c, modp, KEY_LENGTH_DWORDS_P256);
            U++;
        }
    }
    else if (U)
    {
        while (U)
        {
            multiprecision_sub(c, c, modp, KEY_LENGTH_DWORDS_P256);
            U--;
        }
    }

    if (multiprecision_compare(c, modp, KEY_LENGTH_DWORDS_P256)>=0)
        multiprecision_sub(c, c, modp, KEY_LENGTH_DWORDS_P256);

}

void multiprecision_inv_mod(DWORD *aminus, DWORD *u, uint32_t keyLength)
{
    DWORD v[KEY_LENGTH_DWORDS_P256];
    DWORD A[KEY_LENGTH_DWORDS_P256+1];
    DWORD C[KEY_LENGTH_DWORDS_P256+1];
    DWORD *modp;

    if(keyLength == KEY_LENGTH_DWORDS_P256)
    {
        modp = curve_p256.p;
    }
    else
    {
        modp = curve.p;
    }

    multiprecision_copy(v, modp, keyLength);
    multiprecision_init(A, keyLength);
    multiprecision_init(C, keyLength);
    A[0] = 1;

    while (!multiprecision_iszero(u, keyLength))
    {
        while (!(u[0] & 0x01))  // u is even
        {
            multiprecision_rshift(u, u, keyLength);
            if(!(A[0] & 0x01))  // A is even
                multiprecision_rshift(A, A, keyLength);
            else
            {
                A[keyLength]=multiprecision_add(A, A, modp, keyLength); // A =A+p
                multiprecision_rshift(A, A, keyLength);
                A[keyLength-1] |= (A[keyLength]<<31);
            }
        }

        while (!(v[0] & 0x01))  // v is even
        {
            multiprecision_rshift(v, v, keyLength);
            if (!(C[0] & 0x01))  // C is even
            {
                multiprecision_rshift(C, C, keyLength);
            }
            else
            {
                C[keyLength] = multiprecision_add(C, C, modp, keyLength); // C =C+p
                multiprecision_rshift(C, C, keyLength);
                C[keyLength-1] |= (C[keyLength] << 31);
            }
        }

        if (multiprecision_compare(u, v, keyLength) >= 0)
        {
            multiprecision_sub(u, u, v, keyLength);
            multiprecision_sub_mod(A, A, C, keyLength);
        }
        else
        {
            multiprecision_sub(v, v, u, keyLength);
            multiprecision_sub_mod(C, C, A, keyLength);
        }
    }

    if (multiprecision_compare(C, modp, keyLength) >= 0)
        multiprecision_sub(aminus, C, modp, keyLength);
    else
        multiprecision_copy(aminus, C, keyLength);
}

