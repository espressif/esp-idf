/** @file
 *  @brief Attribute Protocol handling.
 */

/*
 * SPDX-FileCopyrightText: 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_ATT_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_ATT_H_

/**
 * @brief Attribute Protocol (ATT)
 * @defgroup bt_att Attribute Protocol (ATT)
 * @ingroup bluetooth
 * @{
 */

#include <stdint.h>
#include <stddef.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/slist.h>
#include <zephyr/sys/util_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes for Error response PDU
 *
 * Defined by The Bluetooth Core Specification, Version 5.4, Vol 3, Part F, Section 3.4.1.1
 */
/** The ATT operation was successful */
#define BT_ATT_ERR_SUCCESS          0x00
/** The attribute handle given was not valid on the server */
#define BT_ATT_ERR_INVALID_HANDLE       0x01
/** The attribute cannot be read */
#define BT_ATT_ERR_READ_NOT_PERMITTED       0x02
/** The attribute cannot be written */
#define BT_ATT_ERR_WRITE_NOT_PERMITTED      0x03
/** The attribute PDU was invalid */
#define BT_ATT_ERR_INVALID_PDU          0x04
/** The attribute requires authentication before it can be read or written */
#define BT_ATT_ERR_AUTHENTICATION       0x05
/** The ATT Server does not support the request received from the client */
#define BT_ATT_ERR_NOT_SUPPORTED        0x06
/** Offset specified was past the end of the attribute */
#define BT_ATT_ERR_INVALID_OFFSET       0x07
/** The attribute requires authorization before it can be read or written */
#define BT_ATT_ERR_AUTHORIZATION        0x08
/** Too many prepare writes have been queued */
#define BT_ATT_ERR_PREPARE_QUEUE_FULL       0x09
/** No attribute found within the given attribute handle range */
#define BT_ATT_ERR_ATTRIBUTE_NOT_FOUND      0x0a
/** The attribute cannot be read using the ATT_READ_BLOB_REQ PDU */
#define BT_ATT_ERR_ATTRIBUTE_NOT_LONG       0x0b
/** The Encryption Key Size used for encrypting this link is too short */
#define BT_ATT_ERR_ENCRYPTION_KEY_SIZE      0x0c
/** The attribute value length is invalid for the operation */
#define BT_ATT_ERR_INVALID_ATTRIBUTE_LEN    0x0d
/**
 * @brief The attribute request that was requested has encountered an error that was unlikely
 *
 * The attribute request could therefore not be completed as requested
 */
#define BT_ATT_ERR_UNLIKELY         0x0e
/** The attribute requires encryption before it can be read or written */
#define BT_ATT_ERR_INSUFFICIENT_ENCRYPTION  0x0f
/**
 * @brief The attribute type is not a supported grouping attribute
 *
 * The attribute type is not a supported grouping attribute as defined by a higher layer
 * specification.
 */
#define BT_ATT_ERR_UNSUPPORTED_GROUP_TYPE   0x10
/** Insufficient Resources to complete the request */
#define BT_ATT_ERR_INSUFFICIENT_RESOURCES   0x11
/** The server requests the client to rediscover the database */
#define BT_ATT_ERR_DB_OUT_OF_SYNC       0x12
/** The attribute parameter value was not allowed */
#define BT_ATT_ERR_VALUE_NOT_ALLOWED        0x13

/* Common Profile Error Codes
 *
 * Defined by the Supplement to the Bluetooth Core Specification (CSS), v11, Part B, Section 1.2.
 */
/** Write Request Rejected */
#define BT_ATT_ERR_WRITE_REQ_REJECTED       0xfc
/** Client Characteristic Configuration Descriptor Improperly Configured */
#define BT_ATT_ERR_CCC_IMPROPER_CONF        0xfd
/** Procedure Already in Progress */
#define BT_ATT_ERR_PROCEDURE_IN_PROGRESS    0xfe
/** Out of Range */
#define BT_ATT_ERR_OUT_OF_RANGE         0xff

/* Version 5.2, Vol 3, Part F, 3.2.9 defines maximum attribute length to 512 */
#define BT_ATT_MAX_ATTRIBUTE_LEN        512

/* Handle 0x0000 is reserved for future use */
#define BT_ATT_FIRST_ATTRIBUTE_HANDLE           0x0001
/* 0xffff is defined as the maximum, and thus last, valid attribute handle */
#define BT_ATT_LAST_ATTRIBUTE_HANDLE            0xffff

/** @brief Get number of EATT channels connected.
 *
 * @param conn The connection to get the number of EATT channels for.
 *
 * @return The number of EATT channels connected.
 * Returns 0 if @p conn is NULL or not connected.
 */
size_t bt_eatt_count(struct bt_conn *conn);

/** @brief ATT channel option bit field values.
 * @note @ref BT_ATT_CHAN_OPT_UNENHANCED_ONLY and @ref BT_ATT_CHAN_OPT_ENHANCED_ONLY are mutually
 * exclusive and both bits may not be set.
 */
enum bt_att_chan_opt {
    /** Both Enhanced and Unenhanced channels can be used  */
    BT_ATT_CHAN_OPT_NONE = 0x0,
    /** Only Unenhanced channels will be used  */
    BT_ATT_CHAN_OPT_UNENHANCED_ONLY = BIT(0),
    /** Only Enhanced channels will be used  */
    BT_ATT_CHAN_OPT_ENHANCED_ONLY = BIT(1),
};

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_ATT_H_ */
