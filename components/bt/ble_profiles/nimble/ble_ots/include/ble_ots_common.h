/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BLE_OTS_COMMON_H
#define BLE_OTS_COMMON_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * OTS Service and Characteristic UUIDs (16-bit)
 *****************************************************************************/
#define BLE_OTS_UUID_OTS_SERVICE                0x1825
#define BLE_OTS_UUID_OTS_FEATURE                0x2ABD
#define BLE_OTS_UUID_OBJECT_NAME                0x2ABE
#define BLE_OTS_UUID_OBJECT_TYPE                0x2ABF
#define BLE_OTS_UUID_OBJECT_SIZE                0x2AC0
#define BLE_OTS_UUID_OBJECT_FIRST_CREATED       0x2AC1
#define BLE_OTS_UUID_OBJECT_LAST_MODIFIED       0x2AC2
#define BLE_OTS_UUID_OBJECT_ID                  0x2AC3
#define BLE_OTS_UUID_OBJECT_PROPERTIES          0x2AC4
#define BLE_OTS_UUID_OACP                       0x2AC5
#define BLE_OTS_UUID_OLCP                       0x2AC6
#define BLE_OTS_UUID_OBJECT_LIST_FILTER         0x2AC7
#define BLE_OTS_UUID_OBJECT_CHANGED             0x2AC8
#define BLE_OTS_UUID_DIRECTORY_LISTING          0x2ACB

/*****************************************************************************
 * PSM_OTS — L2CAP PSM / SPSM for Object Transfer Channel
 *****************************************************************************/
#define BLE_OTS_PSM_OTS                         0x0025

/*****************************************************************************
 * Object Name Constraints
 *****************************************************************************/
#define BLE_OTS_OBJECT_NAME_MAX_LEN             120

/*****************************************************************************
 * Object ID Constraints (UINT48 stored as uint64_t)
 *****************************************************************************/
typedef uint64_t ble_ots_obj_id_t;

#define BLE_OTS_OBJ_ID_DIRECTORY_LISTING        0x000000000000ULL
#define BLE_OTS_OBJ_ID_MIN_USER                 0x000000000100ULL
#define BLE_OTS_OBJ_ID_MAX_USER                 0xFFFFFFFFFFFFULL

/*****************************************************************************
 * OACP Op Codes
 *****************************************************************************/
typedef enum {
    BLE_OTS_OACP_OPCODE_CREATE              = 0x01,
    BLE_OTS_OACP_OPCODE_DELETE              = 0x02,
    BLE_OTS_OACP_OPCODE_CALCULATE_CHECKSUM  = 0x03,
    BLE_OTS_OACP_OPCODE_EXECUTE             = 0x04,
    BLE_OTS_OACP_OPCODE_READ                = 0x05,
    BLE_OTS_OACP_OPCODE_WRITE               = 0x06,
    BLE_OTS_OACP_OPCODE_ABORT               = 0x07,
    BLE_OTS_OACP_OPCODE_RESPONSE            = 0x60,
} ble_ots_oacp_opcode_t;

/*****************************************************************************
 * OACP Write Mode Bit-Field (8-bit)
 *****************************************************************************/
#define BLE_OTS_OACP_WRITE_MODE_TRUNCATE        (1 << 1)

/*****************************************************************************
 * OACP Result Codes
 *****************************************************************************/
typedef enum {
    BLE_OTS_OACP_RESULT_SUCCESS                 = 0x01,
    BLE_OTS_OACP_RESULT_OP_CODE_NOT_SUPPORTED   = 0x02,
    BLE_OTS_OACP_RESULT_INVALID_PARAMETER       = 0x03,
    BLE_OTS_OACP_RESULT_INSUFFICIENT_RESOURCES  = 0x04,
    BLE_OTS_OACP_RESULT_INVALID_OBJECT          = 0x05,
    BLE_OTS_OACP_RESULT_CHANNEL_UNAVAILABLE     = 0x06,
    BLE_OTS_OACP_RESULT_UNSUPPORTED_TYPE        = 0x07,
    BLE_OTS_OACP_RESULT_PROCEDURE_NOT_PERMITTED = 0x08,
    BLE_OTS_OACP_RESULT_OBJECT_LOCKED           = 0x09,
    BLE_OTS_OACP_RESULT_OPERATION_FAILED        = 0x0A,
} ble_ots_oacp_result_code_t;

/*****************************************************************************
 * OACP Response Value
 *****************************************************************************/
typedef struct {
    uint8_t  request_op_code;
    uint8_t  result_code;
    uint8_t  response_parameter[4]; /**< Variable length; up to 4 bytes (e.g., checksum UINT32) */
} ble_ots_oacp_response_value_t;

/*****************************************************************************
 * OLCP Op Codes
 *****************************************************************************/
typedef enum {
    BLE_OTS_OLCP_OPCODE_FIRST                   = 0x01,
    BLE_OTS_OLCP_OPCODE_LAST                    = 0x02,
    BLE_OTS_OLCP_OPCODE_PREVIOUS                = 0x03,
    BLE_OTS_OLCP_OPCODE_NEXT                    = 0x04,
    BLE_OTS_OLCP_OPCODE_GO_TO                   = 0x05,
    BLE_OTS_OLCP_OPCODE_ORDER                   = 0x06,
    BLE_OTS_OLCP_OPCODE_REQUEST_NUM_OF_OBJECTS  = 0x07,
    BLE_OTS_OLCP_OPCODE_CLEAR_MARKING           = 0x08,
    BLE_OTS_OLCP_OPCODE_RESPONSE                = 0x70,
} ble_ots_olcp_opcode_t;

/*****************************************************************************
 * OLCP Result Codes
 *****************************************************************************/
typedef enum {
    BLE_OTS_OLCP_RESULT_SUCCESS                 = 0x01,
    BLE_OTS_OLCP_RESULT_OP_CODE_NOT_SUPPORTED   = 0x02,
    BLE_OTS_OLCP_RESULT_INVALID_PARAMETER       = 0x03,
    BLE_OTS_OLCP_RESULT_OPERATION_FAILED        = 0x04,
    BLE_OTS_OLCP_RESULT_OUT_OF_BOUNDS           = 0x05,
    BLE_OTS_OLCP_RESULT_TOO_MANY_OBJECTS        = 0x06,
    BLE_OTS_OLCP_RESULT_NO_OBJECT               = 0x07,
    BLE_OTS_OLCP_RESULT_OBJECT_ID_NOT_FOUND     = 0x08,
} ble_ots_olcp_result_code_t;

/*****************************************************************************
 * OLCP Response Value
 *****************************************************************************/
typedef struct {
    uint8_t  request_op_code;
    uint8_t  result_code;
    uint32_t total_num_of_objects; /**< Present only for Request Number of Objects on Success */
} ble_ots_olcp_response_value_t;

/*****************************************************************************
 * OACP Features Bit-Field (32-bit)
 *****************************************************************************/
#define BLE_OTS_OACP_FEAT_CREATE                (1UL << 0)
#define BLE_OTS_OACP_FEAT_DELETE                (1UL << 1)
#define BLE_OTS_OACP_FEAT_CALCULATE_CHECKSUM    (1UL << 2)
#define BLE_OTS_OACP_FEAT_EXECUTE               (1UL << 3)
#define BLE_OTS_OACP_FEAT_READ                  (1UL << 4)
#define BLE_OTS_OACP_FEAT_WRITE                 (1UL << 5)
#define BLE_OTS_OACP_FEAT_APPEND                (1UL << 6)
#define BLE_OTS_OACP_FEAT_TRUNCATE              (1UL << 7)
#define BLE_OTS_OACP_FEAT_PATCH                 (1UL << 8)
#define BLE_OTS_OACP_FEAT_ABORT                 (1UL << 9)

/*****************************************************************************
 * OLCP Features Bit-Field (32-bit)
 *****************************************************************************/
#define BLE_OTS_OLCP_FEAT_GO_TO                 (1UL << 0)
#define BLE_OTS_OLCP_FEAT_ORDER                 (1UL << 1)
#define BLE_OTS_OLCP_FEAT_REQUEST_NUM_OF_OBJECTS (1UL << 2)
#define BLE_OTS_OLCP_FEAT_CLEAR_MARKING         (1UL << 3)

/*****************************************************************************
 * OTS Feature Characteristic Structure (8 octets)
 *****************************************************************************/
typedef struct {
    uint32_t oacp_features; /**< OACP Features bit-field */
    uint32_t olcp_features; /**< OLCP Features bit-field */
} ble_ots_feature_t;

/*****************************************************************************
 * Object Properties Bit-Field (32-bit)
 *****************************************************************************/
#define BLE_OTS_OBJ_PROP_DELETE                 (1UL << 0)
#define BLE_OTS_OBJ_PROP_EXECUTE                (1UL << 1)
#define BLE_OTS_OBJ_PROP_READ                   (1UL << 2)
#define BLE_OTS_OBJ_PROP_WRITE                  (1UL << 3)
#define BLE_OTS_OBJ_PROP_APPEND                 (1UL << 4)
#define BLE_OTS_OBJ_PROP_TRUNCATE               (1UL << 5)
#define BLE_OTS_OBJ_PROP_PATCH                  (1UL << 6)
#define BLE_OTS_OBJ_PROP_MARK                   (1UL << 7)

/*****************************************************************************
 * Object Size Characteristic Structure (8 octets)
 *****************************************************************************/
typedef struct {
    uint32_t current_size;   /**< Actual size of the object in octets */
    uint32_t allocated_size; /**< Allocated size in octets (>= current_size) */
} ble_ots_obj_size_t;

/*****************************************************************************
 * Date-Time Structure (7 octets)
 *****************************************************************************/
typedef struct {
    uint16_t year;    /**< Gregorian year (1582–9999; 0 = unknown) */
    uint8_t  month;   /**< Month (1–12; 0 = unknown) */
    uint8_t  day;     /**< Day (1–31; 0 = unknown) */
    uint8_t  hours;   /**< Hours (0–23) */
    uint8_t  minutes; /**< Minutes (0–59) */
    uint8_t  seconds; /**< Seconds (0–59) */
} __attribute__((packed)) ble_ots_date_time_t;

/*****************************************************************************
 * Object List Filter Types
 *****************************************************************************/
typedef enum {
    BLE_OTS_FILTER_NO_FILTER            = 0x00,
    BLE_OTS_FILTER_NAME_STARTS_WITH     = 0x01,
    BLE_OTS_FILTER_NAME_ENDS_WITH       = 0x02,
    BLE_OTS_FILTER_NAME_CONTAINS        = 0x03,
    BLE_OTS_FILTER_NAME_IS_EXACTLY      = 0x04,
    BLE_OTS_FILTER_OBJECT_TYPE          = 0x05,
    BLE_OTS_FILTER_CREATED_BETWEEN      = 0x06,
    BLE_OTS_FILTER_MODIFIED_BETWEEN     = 0x07,
    BLE_OTS_FILTER_CURRENT_SIZE_BETWEEN = 0x08,
    BLE_OTS_FILTER_ALLOCATED_SIZE_BETWEEN = 0x09,
    BLE_OTS_FILTER_MARKED_OBJECTS       = 0x0A,
} ble_ots_list_filter_type_t;

/*****************************************************************************
 * Object List Filter Characteristic Structure
 *****************************************************************************/
typedef struct {
    uint8_t filter_type; /**< One of ble_ots_list_filter_type_t */
    union {
        /** For Name Starts With / Ends With / Contains / Is Exactly */
        struct {
            uint8_t  name[BLE_OTS_OBJECT_NAME_MAX_LEN];
            uint8_t  name_len;
        } name;
        /** For Object Type filter */
        struct {
            uint8_t  uuid[16]; /**< 2 or 16 octets */
            uint8_t  uuid_len; /**< 2 or 16 */
        } obj_type;
        /** For Created Between / Modified Between */
        struct {
            ble_ots_date_time_t timestamp1;
            ble_ots_date_time_t timestamp2;
        } timestamp_range;
        /** For Current Size Between / Allocated Size Between */
        struct {
            uint32_t size1;
            uint32_t size2;
        } size_range;
    } param;
} ble_ots_list_filter_t;

/*****************************************************************************
 * Object Changed Flags Bit-Field (8-bit)
 *****************************************************************************/
#define BLE_OTS_OBJ_CHANGED_FLAG_SOURCE         (1 << 0) /**< 0 = Server, 1 = Client */
#define BLE_OTS_OBJ_CHANGED_FLAG_CONTENT        (1 << 1) /**< Object contents changed */
#define BLE_OTS_OBJ_CHANGED_FLAG_METADATA       (1 << 2) /**< Object metadata changed */
#define BLE_OTS_OBJ_CHANGED_FLAG_CREATION       (1 << 3) /**< Object creation */
#define BLE_OTS_OBJ_CHANGED_FLAG_DELETION       (1 << 4) /**< Object deletion */

/*****************************************************************************
 * Object Changed Characteristic Structure (7 octets)
 *****************************************************************************/
typedef struct {
    uint8_t         flags;     /**< Object Changed flags bit-field */
    ble_ots_obj_id_t object_id; /**< Object ID of the changed object */
} ble_ots_obj_changed_t;

/*****************************************************************************
 * List Sort Order Values
 *****************************************************************************/
typedef enum {
    BLE_OTS_SORT_ORDER_NAME_ASC             = 0x01,
    BLE_OTS_SORT_ORDER_TYPE_ASC             = 0x02,
    BLE_OTS_SORT_ORDER_CURRENT_SIZE_ASC     = 0x03,
    BLE_OTS_SORT_ORDER_FIRST_CREATED_ASC    = 0x04,
    BLE_OTS_SORT_ORDER_LAST_MODIFIED_ASC    = 0x05,
    BLE_OTS_SORT_ORDER_NAME_DESC            = 0x11,
    BLE_OTS_SORT_ORDER_TYPE_DESC            = 0x12,
    BLE_OTS_SORT_ORDER_CURRENT_SIZE_DESC    = 0x13,
    BLE_OTS_SORT_ORDER_FIRST_CREATED_DESC   = 0x14,
    BLE_OTS_SORT_ORDER_LAST_MODIFIED_DESC   = 0x15,
} ble_ots_list_sort_order_t;

/*****************************************************************************
 * Application Error Codes
 *****************************************************************************/
typedef enum {
    BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED      = 0x80,
    BLE_OTS_APP_ERR_OBJECT_NOT_SELECTED         = 0x81,
    BLE_OTS_APP_ERR_CONCURRENCY_LIMIT_EXCEEDED  = 0x82,
    BLE_OTS_APP_ERR_OBJECT_NAME_ALREADY_EXISTS  = 0x83,
} ble_ots_app_error_code_t;

/*****************************************************************************
 * Directory Listing Object Record Flags (8-bit)
 *****************************************************************************/
#define BLE_OTS_DLO_FLAG_TYPE_UUID_128          (1 << 0) /**< 0 = 16-bit, 1 = 128-bit UUID */
#define BLE_OTS_DLO_FLAG_CURRENT_SIZE_PRESENT   (1 << 1)
#define BLE_OTS_DLO_FLAG_ALLOCATED_SIZE_PRESENT (1 << 2)
#define BLE_OTS_DLO_FLAG_FIRST_CREATED_PRESENT  (1 << 3)
#define BLE_OTS_DLO_FLAG_LAST_MODIFIED_PRESENT  (1 << 4)
#define BLE_OTS_DLO_FLAG_PROPERTIES_PRESENT     (1 << 5)
#define BLE_OTS_DLO_FLAG_EXTENDED_FLAGS_PRESENT (1 << 7)

/*****************************************************************************
 * Object Type Entry (for storing 16-bit or 128-bit UUIDs)
 *****************************************************************************/
typedef struct {
    uint8_t uuid_len;  /**< 2 for 16-bit UUID, 16 for 128-bit UUID */
    uint8_t uuid[16];  /**< UUID value (little-endian) */
} ble_ots_obj_type_entry_t;

/*****************************************************************************
 * CRC-32 Checksum Calculation
 *****************************************************************************/

/**
 * @brief Compute ISO/IEC 3309 CRC-32 over a byte range of object data.
 *
 * Uses the Ethernet/HDLC CRC-32 algorithm:
 *   - Polynomial: 0x04C11DB7
 *   - Initial value: 0xFFFFFFFF
 *   - Final XOR: 0xFFFFFFFF
 *   - Input/output reflection: enabled
 *
 * @param data   Pointer to the object data buffer.
 * @param offset Byte offset from the start of data at which to begin CRC calculation.
 * @param length Number of octets over which to compute the CRC, starting from offset.
 * @return       The computed 32-bit CRC value.
 */
uint32_t ble_ots_checksum_calculate(const uint8_t *data, uint32_t offset, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* BLE_OTS_COMMON_H */
