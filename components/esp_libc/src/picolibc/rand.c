/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>

_Thread_local unsigned long long _tls_rand_next = 1;

void srand(unsigned int seed)
{
    _tls_rand_next = seed;
}

int rand(void)
{
    /* This multiplier was obtained from Knuth, D.E., "The Art of
       Computer Programming," Vol 2, Seminumerical Algorithms, Third
       Edition, Addison-Wesley, 1998, p. 106 (line 26) & p. 108 */
    _tls_rand_next = _tls_rand_next * __extension__ 6364136223846793005LL + 1;
    return (int)((_tls_rand_next >> 32) & RAND_MAX);
}
