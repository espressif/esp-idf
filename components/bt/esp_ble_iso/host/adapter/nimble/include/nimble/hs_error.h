/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <errno.h>

#include "host/ble_hs.h"

/**
 * Translate a NimBLE host error code (positive BLE_HS_* value from
 * host/ble_hs.h) into a standard negative POSIX errno.
 *
 * Used at the adapter boundary to keep NimBLE-specific error codes out of
 * common/ and lib code, which use standard Zephyr/POSIX errno conventions.
 *
 * Pass-through rules:
 *   - rc == 0:      success, returned as-is
 *   - rc < 0:       already in negative-errno form, returned as-is
 *   - rc >= 0x100:  protocol-level error (ATT / HCI / L2CAP base) whose
 *                   encoded class the caller may extract — returned as-is so
 *                   the semantic information is preserved
 *   - 1..31:        known NimBLE host codes, translated below
 *   - other:        unknown NimBLE code, mapped to -EIO so it is not
 *                   confused with a known POSIX errno
 */
static inline int nimble_err_to_errno(int rc)
{
    if (rc <= 0) {
        return rc;
    }

    if (rc >= 0x100) {
        return rc;
    }

    switch (rc) {
    case BLE_HS_EAGAIN:          return -EAGAIN;
    case BLE_HS_EALREADY:        return -EALREADY;
    case BLE_HS_EINVAL:          return -EINVAL;
    case BLE_HS_EMSGSIZE:        return -EMSGSIZE;
    case BLE_HS_ENOENT:          return -ENOENT;
    case BLE_HS_ENOMEM:          return -ENOMEM;
    case BLE_HS_ENOTCONN:        return -ENOTCONN;
    case BLE_HS_ENOTSUP:         return -ENOTSUP;
    case BLE_HS_EAPP:            return -EPROTO;
    case BLE_HS_EBADDATA:        return -EBADMSG;
    case BLE_HS_EOS:             return -EIO;
    case BLE_HS_ECONTROLLER:     return -EIO;
    case BLE_HS_ETIMEOUT:        return -ETIMEDOUT;
    case BLE_HS_EDONE:           return -EALREADY;
    case BLE_HS_EBUSY:           return -EBUSY;
    case BLE_HS_EREJECT:         return -EACCES;
    case BLE_HS_EUNKNOWN:        return -EIO;
    case BLE_HS_EROLE:           return -EPERM;
    case BLE_HS_ETIMEOUT_HCI:    return -ETIMEDOUT;
    case BLE_HS_ENOMEM_EVT:      return -ENOMEM;
    case BLE_HS_ENOADDR:         return -EADDRNOTAVAIL;
    case BLE_HS_ENOTSYNCED:      return -EAGAIN;
    case BLE_HS_EAUTHEN:         return -EPERM;
    case BLE_HS_EAUTHOR:         return -EACCES;
    case BLE_HS_EENCRYPT:        return -EPERM;
    case BLE_HS_EENCRYPT_KEY_SZ: return -EINVAL;
    case BLE_HS_ESTORE_CAP:      return -ENOSPC;
    case BLE_HS_ESTORE_FAIL:     return -EIO;
    case BLE_HS_EPREEMPTED:      return -EINTR;
    case BLE_HS_EDISABLED:       return -EPERM;
    case BLE_HS_ESTALLED:        return -EAGAIN; /* flow-control stall */
    default:                     return -EIO;
    }
}
