/*
 * SPDX-FileCopyrightText: 2018 Vikrant More
 * SPDX-FileContributor: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <math.h>
#include "mesh/types.h"

#define MINDIFF (2.25e-308)

float bt_mesh_sqrt(float square)
{
    float root = 0.0, last = 0.0, diff = 0.0;

    root = square / 3.0;
    diff = 1;

    if (square <= 0) {
        return 0;
    }

    do {
        last = root;
        root = (root + square / root) / 2.0;
        diff = root - last;
    } while (diff > MINDIFF || diff < -MINDIFF);

    return root;
}

int32_t bt_mesh_ceil(float num)
{
    int32_t inum = (int32_t)num;

    if (num == (float)inum) {
        return inum;
    }

    return inum + 1;
}

float bt_mesh_log2(float num)
{
    return log2f(num);
}
