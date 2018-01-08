/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "mesh_trace.h"

#include "mesh.h"
#include "test.h"

#if defined(CONFIG_BT_MESH_SELF_TEST)

int bt_mesh_test(void)
{
    return 0;
}
#endif /* #if defined(CONFIG_BT_MESH_SELF_TEST) */
