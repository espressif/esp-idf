/* @file
 * @brief Object Transfer client internal header file
 *
 * For use with the Object Transfer Service Client (OTC)
 *
 * SPDX-FileCopyrightText: 2020-2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BT_GATT_OTC_INTERNAL_H_
#define BT_GATT_OTC_INTERNAL_H_

#include "ots_oacp_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Translate OTS OLCP result code to "normal" error
 *
 * Replace the OTS SUCCESS value with the value 0.
 *
 * The OLCP result code has the value "1" for success.
 * Elsewhere in the le-audio host stack, "0" is used for no error.
 *
 * TODO: Update to also convert the non-SUCCESS values
 * TODO: The macro is called OLCP, but uses OACP success value
 * TODO: See https://github.com/zephyrproject-rtos/zephyr/issues/41184
 * OTS does not use the value "0".
 */
#define OLCP_RESULT_TO_ERROR(RESULT) \
    (((RESULT) == BT_GATT_OTS_OACP_RES_SUCCESS) ? 0 : RESULT)

#define OTS_FEATURE_LEN        (uint32_t)(2 * sizeof(uint32_t))
#define OTS_SIZE_LEN           (uint32_t)(2 * sizeof(uint32_t))
#define OTS_PROPERTIES_LEN     (uint32_t)(sizeof(uint32_t))
#define OTS_TYPE_MAX_LEN       (uint32_t)(sizeof(struct bt_ots_obj_type))

#ifdef __cplusplus
}
#endif

#endif /* BT_GATT_OTC_INTERNAL_H_ */
