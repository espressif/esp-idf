/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "unity.h"
#include "unity_test_runner.h"

#include "ble_log.h"
#include "ble_log_lbm_v2.h"
#include "test_ble_log_main.h"

bool test_ble_log_walk_frames(const uint8_t *data, size_t len,
                              test_ble_log_frame_observer_t observer, void *ctx)
{
    size_t offset = 0;
    while (len - offset >= BLE_LOG_FRAME_OVERHEAD) {
        ble_log_frame_head_t head;
        memcpy(&head, data + offset, sizeof(head));

        size_t frame_len = BLE_LOG_FRAME_OVERHEAD + head.length;
        if (frame_len > len - offset) {
            return false;
        }

        uint32_t checksum;
        memcpy(&checksum, data + offset + BLE_LOG_FRAME_HEAD_LEN + head.length,
               sizeof(checksum));
        if (checksum != ble_log_fast_checksum(data + offset,
                                              BLE_LOG_FRAME_HEAD_LEN + head.length)) {
            return false;
        }

        if (observer) {
            test_ble_log_frame_t frame = {
                .src = (ble_log_src_t)(head.frame_meta & 0xff),
                .sn = head.frame_meta >> 8,
                .payload = data + offset + BLE_LOG_FRAME_HEAD_LEN,
                .payload_len = head.length,
            };
            observer(&frame, ctx);
        }
        offset += frame_len;
    }
    return offset == len;
}

void setUp(void)
{
    /* Preserve the external test-system contract: every test starts with the
     * optional sync IO disabled and low. Periodic snapshots remain active. */
    (void)ble_log_sync_enable(false);
}

void tearDown(void)
{
    /* Unity aborts a case by longjmp, so an armed test hook would fire in
     * the NEXT case (its time gate was set in a dead stack frame and is
     * long since passed). Disarm every hook the bounded-wait cases arm so
     * a failed assert cannot leak it across cases. The hook state lives
     * in test_ble_log_rt.c next to its cases. */
    extern void test_ble_log_disarm_case_hooks(void);
    test_ble_log_disarm_case_hooks();
}

void app_main(void)
{
    /* The BLE Log module has no automatic system init on this branch; the
     * controller normally calls ble_log_init(). Initialize it explicitly. */
    TEST_ASSERT_TRUE_MESSAGE(ble_log_init(), "BLE Log init failed");
    unity_run_menu();
}
