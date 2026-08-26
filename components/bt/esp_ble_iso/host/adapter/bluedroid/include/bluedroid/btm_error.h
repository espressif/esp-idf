/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <errno.h>

#include "stack/btm_api.h"

/**
 * Translate a Bluedroid BTM status (tBTM_STATUS, see stack/btm_api.h) into
 * a standard negative POSIX errno.
 *
 * Counterpart to nimble_err_to_errno() — both adapters return errno values
 * to common/ and lib code so the audio layer can branch on -EAGAIN /
 * -EBUSY / -ENOMEM etc. without knowing which host it ran on.
 *
 * Unknown codes map to -EIO so caller logic doesn't accidentally match a
 * known errno.
 */
static inline int bluedroid_err_to_errno(tBTM_STATUS status)
{
    switch (status) {
    case BTM_SUCCESS:                       return 0;
    case BTM_CMD_STARTED:                   return 0;
    case BTM_CMD_STORED:                    return 0;
    case BTM_SUCCESS_NO_SECURITY:           return 0;
    case BTM_BUSY:                          return -EBUSY;
    case BTM_NO_RESOURCES:                  return -ENOMEM;
    case BTM_MODE_UNSUPPORTED:              return -ENOTSUP;
    case BTM_ILLEGAL_VALUE:                 return -EINVAL;
    case BTM_WRONG_MODE:                    return -EPERM;
    case BTM_UNKNOWN_ADDR:                  return -ENOENT;
    case BTM_DEVICE_TIMEOUT:                return -ETIMEDOUT;
    case BTM_BAD_VALUE_RET:                 return -EBADMSG;
    case BTM_ERR_PROCESSING:                return -EIO;
    case BTM_NOT_AUTHORIZED:                return -EACCES;
    case BTM_DEV_RESET:                     return -ECONNRESET;
    case BTM_ILLEGAL_ACTION:                return -EPERM;
    case BTM_FAILED_ON_SECURITY:            return -EPERM;
    case BTM_REPEATED_ATTEMPTS:             return -EAGAIN;
    default:                                return -EIO;
    }
}
