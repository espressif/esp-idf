/*
 * SPDX-FileCopyrightText: 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Helper for printk parameters to convert from binary to hex.
 * We declare multiple buffers so the helper can be used multiple times
 * in a single printk call.
 */

#include <stddef.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/types.h>

const char *bt_hex(const void *buf, size_t len)
{
    static const char hex[] = "0123456789abcdef";
    static char str[129];
    const uint8_t *b = buf;
    size_t i;

    len = MIN(len, (sizeof(str) - 1) / 2);

    for (i = 0; i < len; i++) {
        str[i * 2] = hex[b[i] >> 4];
        str[i * 2 + 1] = hex[b[i] & 0xf];
    }

    str[i * 2] = '\0';

    return str;
}

const char *bt_addr_str(const bt_addr_t *addr)
{
    static char str[BT_ADDR_STR_LEN];

    bt_addr_to_str(addr, str, sizeof(str));

    return str;
}

const char *bt_addr_le_str(const bt_addr_le_t *addr)
{
    static char str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(addr, str, sizeof(str));

    return str;
}

const char *bt_uuid_str(const struct bt_uuid *uuid)
{
    static char str[BT_UUID_STR_LEN];

    bt_uuid_to_str(uuid, str, sizeof(str));

    return str;
}

#if defined(CONFIG_BT_HCI_ERR_TO_STR)
const char *bt_hci_err_to_str(uint8_t hci_err)
{
#define HCI_ERR(err) [err] = #err

    const char * const mapping_table[] = {
        HCI_ERR(BT_HCI_ERR_SUCCESS),
        HCI_ERR(BT_HCI_ERR_UNKNOWN_CMD),
        HCI_ERR(BT_HCI_ERR_UNKNOWN_CONN_ID),
        HCI_ERR(BT_HCI_ERR_HW_FAILURE),
        HCI_ERR(BT_HCI_ERR_PAGE_TIMEOUT),
        HCI_ERR(BT_HCI_ERR_AUTH_FAIL),
        HCI_ERR(BT_HCI_ERR_PIN_OR_KEY_MISSING),
        HCI_ERR(BT_HCI_ERR_MEM_CAPACITY_EXCEEDED),
        HCI_ERR(BT_HCI_ERR_CONN_TIMEOUT),
        HCI_ERR(BT_HCI_ERR_CONN_LIMIT_EXCEEDED),
        HCI_ERR(BT_HCI_ERR_SYNC_CONN_LIMIT_EXCEEDED),
        HCI_ERR(BT_HCI_ERR_CONN_ALREADY_EXISTS),
        HCI_ERR(BT_HCI_ERR_CMD_DISALLOWED),
        HCI_ERR(BT_HCI_ERR_INSUFFICIENT_RESOURCES),
        HCI_ERR(BT_HCI_ERR_INSUFFICIENT_SECURITY),
        HCI_ERR(BT_HCI_ERR_BD_ADDR_UNACCEPTABLE),
        HCI_ERR(BT_HCI_ERR_CONN_ACCEPT_TIMEOUT),
        HCI_ERR(BT_HCI_ERR_UNSUPP_FEATURE_PARAM_VAL),
        HCI_ERR(BT_HCI_ERR_INVALID_PARAM),
        HCI_ERR(BT_HCI_ERR_REMOTE_USER_TERM_CONN),
        HCI_ERR(BT_HCI_ERR_REMOTE_LOW_RESOURCES),
        HCI_ERR(BT_HCI_ERR_REMOTE_POWER_OFF),
        HCI_ERR(BT_HCI_ERR_LOCALHOST_TERM_CONN),
        HCI_ERR(BT_HCI_ERR_REPEATED_ATTEMPTS),
        HCI_ERR(BT_HCI_ERR_PAIRING_NOT_ALLOWED),
        HCI_ERR(BT_HCI_ERR_UNKNOWN_LMP_PDU),
        HCI_ERR(BT_HCI_ERR_UNSUPP_REMOTE_FEATURE),
        HCI_ERR(BT_HCI_ERR_SCO_OFFSET_REJECTED),
        HCI_ERR(BT_HCI_ERR_SCO_INTERVAL_REJECTED),
        HCI_ERR(BT_HCI_ERR_SCO_AIR_MODE_REJECTED),
        HCI_ERR(BT_HCI_ERR_INVALID_LL_PARAM),
        HCI_ERR(BT_HCI_ERR_UNSPECIFIED),
        HCI_ERR(BT_HCI_ERR_UNSUPP_LL_PARAM_VAL),
        HCI_ERR(BT_HCI_ERR_ROLE_CHANGE_NOT_ALLOWED),
        HCI_ERR(BT_HCI_ERR_LL_RESP_TIMEOUT),
        HCI_ERR(BT_HCI_ERR_LL_PROC_COLLISION),
        HCI_ERR(BT_HCI_ERR_LMP_PDU_NOT_ALLOWED),
        HCI_ERR(BT_HCI_ERR_ENC_MODE_NOT_ACCEPTABLE),
        HCI_ERR(BT_HCI_ERR_LINK_KEY_CANNOT_BE_CHANGED),
        HCI_ERR(BT_HCI_ERR_REQUESTED_QOS_NOT_SUPPORTED),
        HCI_ERR(BT_HCI_ERR_INSTANT_PASSED),
        HCI_ERR(BT_HCI_ERR_PAIRING_NOT_SUPPORTED),
        HCI_ERR(BT_HCI_ERR_DIFF_TRANS_COLLISION),
        HCI_ERR(BT_HCI_ERR_QOS_UNACCEPTABLE_PARAM),
        HCI_ERR(BT_HCI_ERR_QOS_REJECTED),
        HCI_ERR(BT_HCI_ERR_CHAN_ASSESS_NOT_SUPPORTED),
        HCI_ERR(BT_HCI_ERR_INSUFF_SECURITY),
        HCI_ERR(BT_HCI_ERR_PARAM_OUT_OF_MANDATORY_RANGE),
        HCI_ERR(BT_HCI_ERR_ROLE_SWITCH_PENDING),
        HCI_ERR(BT_HCI_ERR_RESERVED_SLOT_VIOLATION),
        HCI_ERR(BT_HCI_ERR_ROLE_SWITCH_FAILED),
        HCI_ERR(BT_HCI_ERR_EXT_INQ_RESP_TOO_LARGE),
        HCI_ERR(BT_HCI_ERR_SIMPLE_PAIR_NOT_SUPP_BY_HOST),
        HCI_ERR(BT_HCI_ERR_HOST_BUSY_PAIRING),
        HCI_ERR(BT_HCI_ERR_CONN_REJECTED_DUE_TO_NO_CHAN),
        HCI_ERR(BT_HCI_ERR_CONTROLLER_BUSY),
        HCI_ERR(BT_HCI_ERR_UNACCEPT_CONN_PARAM),
        HCI_ERR(BT_HCI_ERR_ADV_TIMEOUT),
        HCI_ERR(BT_HCI_ERR_TERM_DUE_TO_MIC_FAIL),
        HCI_ERR(BT_HCI_ERR_CONN_FAIL_TO_ESTAB),
        HCI_ERR(BT_HCI_ERR_MAC_CONN_FAILED),
        HCI_ERR(BT_HCI_ERR_CLOCK_ADJUST_REJECTED),
        HCI_ERR(BT_HCI_ERR_SUBMAP_NOT_DEFINED),
        HCI_ERR(BT_HCI_ERR_UNKNOWN_ADV_IDENTIFIER),
        HCI_ERR(BT_HCI_ERR_LIMIT_REACHED),
        HCI_ERR(BT_HCI_ERR_OP_CANCELLED_BY_HOST),
        HCI_ERR(BT_HCI_ERR_PACKET_TOO_LONG),
        HCI_ERR(BT_HCI_ERR_TOO_LATE),
        HCI_ERR(BT_HCI_ERR_TOO_EARLY),
    };

    if (hci_err < ARRAY_SIZE(mapping_table) && mapping_table[hci_err]) {
        return mapping_table[hci_err];
    } else {
        return "(unknown)";
    }

#undef HCI_ERR
}
#endif /* CONFIG_BT_HCI_ERR_TO_STR */
