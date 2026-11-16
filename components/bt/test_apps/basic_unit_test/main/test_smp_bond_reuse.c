/*
 * SPDX-FileCopyrightText: 2026 rndm2 <random@random.net.ua>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <string.h>
#include "unity.h"
#include "common/bt_target.h"

#if (BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
#include "btm_int.h"
#include "smp_int.h"

/* Exercise the real action, without a controller or a running SMP procedure.
 * Wrappers intercept only this fixture. Other tests use the real stack calls.
 */
static tSMP_CB s_cb;
static tBTM_SEC_DEV_REC s_record;
static bool s_active;
static bool s_record_present;
static unsigned s_clear_count;
static unsigned s_event_count;
static tSMP_EVENT s_event;
static UINT8 s_reason;

extern tBTM_SEC_DEV_REC *__real_btm_find_dev(BD_ADDR bd_addr);
extern void __real_btm_sec_clear_ble_keys(tBTM_SEC_DEV_REC *p_dev_rec);
extern void __real_smp_sm_event(tSMP_CB *p_cb, tSMP_EVENT event, void *p_data);

tBTM_SEC_DEV_REC *__wrap_btm_find_dev(BD_ADDR bd_addr)
{
    if (s_active && memcmp(bd_addr, s_cb.pairing_bda, BD_ADDR_LEN) == 0) {
        return s_record_present ? &s_record : NULL;
    }
    return __real_btm_find_dev(bd_addr);
}

void __wrap_btm_sec_clear_ble_keys(tBTM_SEC_DEV_REC *p_dev_rec)
{
    if (s_active && p_dev_rec == &s_record) {
        ++s_clear_count;
        p_dev_rec->ble.key_type = BTM_LE_KEY_NONE;
        memset(&p_dev_rec->ble.keys, 0, sizeof(p_dev_rec->ble.keys));
        return;
    }
    __real_btm_sec_clear_ble_keys(p_dev_rec);
}

void __wrap_smp_sm_event(tSMP_CB *p_cb, tSMP_EVENT event, void *p_data)
{
    if (s_active && p_cb == &s_cb) {
        ++s_event_count;
        s_event = event;
        if (p_data != NULL) {
            s_reason = *(UINT8 *)p_data;
        }
        return;
    }
    __real_smp_sm_event(p_cb, event, p_data);
}

static void reset_fixture(UINT8 role, UINT8 flags)
{
    const BD_ADDR address = {0xc0, 0x00, 0x00, 0x00, 0x00, 0x01};
    memset(&s_cb, 0, sizeof(s_cb));
    memset(&s_record, 0, sizeof(s_record));
    memcpy(s_cb.pairing_bda, address, BD_ADDR_LEN);
    s_cb.role = role;
    s_cb.flags = flags;
    s_record.ble.key_type = BTM_LE_KEY_PENC | BTM_LE_KEY_LENC;
    memset(&s_record.ble.keys, 0xa5, sizeof(s_record.ble.keys));
    s_record_present = true;
    s_clear_count = 0;
    s_event_count = 0;
    s_event = 0;
    s_reason = 0xff;
}

static void run_action(UINT8 encrypted)
{
    tSMP_INT_DATA data = {0};
    *(UINT8 *)&data = encrypted;
    /* Disable interception before assertions, even if a test later fails. */
    s_active = true;
    smp_check_auth_req(&s_cb, &data);
    s_active = false;
}

TEST_CASE("SMP retains reused bond keys for both roles", "[ble_smp][bond_reuse]")
{
    for (UINT8 role = HCI_ROLE_MASTER; role <= HCI_ROLE_SLAVE; ++role) {
        reset_fixture(role, SMP_PAIR_FLAGS_WE_STARTED_DD);
        tBTM_SEC_BLE_KEYS saved_keys = s_record.ble.keys;
        run_action(1);
        TEST_ASSERT_EQUAL_UINT32(0, s_clear_count);
        TEST_ASSERT_EQUAL_HEX8(BTM_LE_KEY_PENC | BTM_LE_KEY_LENC, s_record.ble.key_type);
        TEST_ASSERT_EQUAL_MEMORY(&saved_keys, &s_record.ble.keys, sizeof(saved_keys));
        TEST_ASSERT_EQUAL_UINT32(1, s_event_count);
        TEST_ASSERT_EQUAL_UINT8(SMP_AUTH_CMPL_EVT, s_event);
        TEST_ASSERT_EQUAL_UINT8(SMP_SUCCESS, s_reason);
    }
}

TEST_CASE("SMP replaces old keys only after successful new pairing", "[ble_smp][bond_reuse]")
{
    for (UINT8 role = HCI_ROLE_MASTER; role <= HCI_ROLE_SLAVE; ++role) {
        for (UINT8 secure_connections = 0; secure_connections <= 1; ++secure_connections) {
            reset_fixture(role, SMP_PAIR_FLAG_ENC_AFTER_PAIR);
            s_cb.le_secure_connections_mode_is_used = secure_connections;
            s_cb.local_i_key = SMP_SEC_KEY_TYPE_ENC;
            s_cb.local_r_key = SMP_SEC_KEY_TYPE_ENC;
            run_action(1);
            TEST_ASSERT_EQUAL_UINT32(1, s_clear_count);
            TEST_ASSERT_EQUAL_UINT8(BTM_LE_KEY_NONE, s_record.ble.key_type);
            TEST_ASSERT_EQUAL_UINT32(1, s_event_count);
            TEST_ASSERT_EQUAL_UINT8(SMP_BOND_REQ_EVT, s_event);
        }
    }
}

TEST_CASE("SMP retains old keys when encryption fails", "[ble_smp][bond_reuse]")
{
    for (UINT8 role = HCI_ROLE_MASTER; role <= HCI_ROLE_SLAVE; ++role) {
        for (UINT8 pairing = 0; pairing <= 1; ++pairing) {
            reset_fixture(role, pairing ? SMP_PAIR_FLAG_ENC_AFTER_PAIR : 0);
            tBTM_SEC_BLE_KEYS saved_keys = s_record.ble.keys;
            run_action(0);
            TEST_ASSERT_EQUAL_UINT32(0, s_clear_count);
            TEST_ASSERT_EQUAL_HEX8(BTM_LE_KEY_PENC | BTM_LE_KEY_LENC, s_record.ble.key_type);
            TEST_ASSERT_EQUAL_MEMORY(&saved_keys, &s_record.ble.keys, sizeof(saved_keys));
            if (pairing || role == HCI_ROLE_MASTER) {
                TEST_ASSERT_EQUAL_UINT32(1, s_event_count);
                TEST_ASSERT_EQUAL_UINT8(SMP_AUTH_CMPL_EVT, s_event);
                TEST_ASSERT_EQUAL_UINT8(SMP_ENC_FAIL, s_reason);
            } else {
                TEST_ASSERT_EQUAL_UINT32(0, s_event_count);
            }
        }
    }
}

TEST_CASE("SMP handles new pairing without an existing device record", "[ble_smp][bond_reuse]")
{
    reset_fixture(HCI_ROLE_MASTER, SMP_PAIR_FLAG_ENC_AFTER_PAIR);
    s_record_present = false;
    s_cb.local_i_key = SMP_SEC_KEY_TYPE_ENC;
    run_action(1);
    TEST_ASSERT_EQUAL_UINT32(0, s_clear_count);
    TEST_ASSERT_EQUAL_UINT32(1, s_event_count);
    TEST_ASSERT_EQUAL_UINT8(SMP_BOND_REQ_EVT, s_event);
}
#endif /* BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE */
