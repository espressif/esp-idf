/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

extern "C" {
#include "Mockmock_transport.h"
#include "Mocknetdb.h"
#include "Mockesp_timer.h"
}

struct HostTestCMockReset {
    HostTestCMockReset()
    {
        Mockmock_transport_Init();
        Mocknetdb_Init();
        Mockesp_timer_Init();
        mock_destroy_IgnoreAndReturn(ESP_OK);
    }

    ~HostTestCMockReset()
    {
        Mockmock_transport_Destroy();
        Mocknetdb_Destroy();
        Mockesp_timer_Destroy();
    }
};
