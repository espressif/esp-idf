/** @file
 *  @brief Bluetooth UUID handling
 */

/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_UUID_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_UUID_H_

/**
 * @brief UUIDs
 * @defgroup bt_uuid UUIDs
 * @ingroup bluetooth
 * @{
 */

#include <stddef.h>
#include <stdint.h>

#include <zephyr/sys/util.h>
#include <zephyr/bluetooth/byteorder.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Bluetooth UUID types */
enum {
    /** UUID type 16-bit. */
    BT_UUID_TYPE_16,
    /** UUID type 32-bit. */
    BT_UUID_TYPE_32,
    /** UUID type 128-bit. */
    BT_UUID_TYPE_128,
};

/** Size in octets of a 16-bit UUID */
#define BT_UUID_SIZE_16                  2

/** Size in octets of a 32-bit UUID */
#define BT_UUID_SIZE_32                  4

/** Size in octets of a 128-bit UUID */
#define BT_UUID_SIZE_128                 16

/** @brief This is a 'tentative' type and should be used as a pointer only */
struct bt_uuid {
    uint8_t type;
};

struct bt_uuid_16 {
    /** UUID generic type. */
    struct bt_uuid uuid;
    /** UUID value, 16-bit in host endianness. */
    uint16_t val;
};

struct bt_uuid_32 {
    /** UUID generic type. */
    struct bt_uuid uuid;
    /** UUID value, 32-bit in host endianness. */
    uint32_t val;
};

struct bt_uuid_128 {
    /** UUID generic type. */
    struct bt_uuid uuid;
    /** UUID value, 128-bit in little-endian format. */
    uint8_t val[BT_UUID_SIZE_128];
};

/** @brief Initialize a 16-bit UUID.
 *
 *  @param value 16-bit UUID value in host endianness.
 */
#define BT_UUID_INIT_16(value)      \
{                   \
    .uuid = { BT_UUID_TYPE_16 },    \
    .val = (value),         \
}

/** @brief Initialize a 32-bit UUID.
 *
 *  @param value 32-bit UUID value in host endianness.
 */
#define BT_UUID_INIT_32(value)      \
{                   \
    .uuid = { BT_UUID_TYPE_32 },    \
    .val = (value),         \
}

/** @brief Initialize a 128-bit UUID.
 *
 *  @param value 128-bit UUID array values in little-endian format.
 *               Can be combined with @ref BT_UUID_128_ENCODE to initialize a
 *               UUID from the readable form of UUIDs.
 */
#define BT_UUID_INIT_128(value...)  \
{                   \
    .uuid = { BT_UUID_TYPE_128 },   \
    .val = { value },       \
}

/** @brief Helper to declare a 16-bit UUID inline.
 *
 *  @param value 16-bit UUID value in host endianness.
 *
 *  @return Pointer to a generic UUID.
 */
#define BT_UUID_DECLARE_16(value) \
    ((const struct bt_uuid *) ((const struct bt_uuid_16[]) {BT_UUID_INIT_16(value)}))

/** @brief Helper to declare a 32-bit UUID inline.
 *
 *  @param value 32-bit UUID value in host endianness.
 *
 *  @return Pointer to a generic UUID.
 */
#define BT_UUID_DECLARE_32(value) \
    ((const struct bt_uuid *) ((const struct bt_uuid_32[]) {BT_UUID_INIT_32(value)}))

/** @brief Helper to declare a 128-bit UUID inline.
 *
 *  @param value 128-bit UUID array values in little-endian format.
 *               Can be combined with @ref BT_UUID_128_ENCODE to declare a
 *               UUID from the readable form of UUIDs.
 *
 *  @return Pointer to a generic UUID.
 */
#define BT_UUID_DECLARE_128(value...) \
    ((const struct bt_uuid *) ((const struct bt_uuid_128[]) {BT_UUID_INIT_128(value)}))

/** Helper macro to access the 16-bit UUID from a generic UUID. */
#define BT_UUID_16(__u) CONTAINER_OF(__u, struct bt_uuid_16, uuid)

/** Helper macro to access the 32-bit UUID from a generic UUID. */
#define BT_UUID_32(__u) CONTAINER_OF(__u, struct bt_uuid_32, uuid)

/** Helper macro to access the 128-bit UUID from a generic UUID. */
#define BT_UUID_128(__u) CONTAINER_OF(__u, struct bt_uuid_128, uuid)

/** @brief Encode 128 bit UUID into array values in little-endian format.
 *
 *  Helper macro to initialize a 128-bit UUID array value from the readable form
 *  of UUIDs, or encode 128-bit UUID values into advertising data
 *  Can be combined with BT_UUID_DECLARE_128 to declare a 128-bit UUID.
 *
 *  Example of how to declare the UUID `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
 *
 *  @code
 *  BT_UUID_DECLARE_128(
 *       BT_UUID_128_ENCODE(0x6E400001, 0xB5A3, 0xF393, 0xE0A9, 0xE50E24DCCA9E))
 *  @endcode
 *
 *  Example of how to encode the UUID `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
 *  into advertising data.
 *
 *  @code
 *  BT_DATA_BYTES(BT_DATA_UUID128_ALL,
 *       BT_UUID_128_ENCODE(0x6E400001, 0xB5A3, 0xF393, 0xE0A9, 0xE50E24DCCA9E))
 *  @endcode
 *
 *  Just replace the hyphen by the comma and add `0x` prefixes.
 *
 *  @param w32 First part of the UUID (32 bits)
 *  @param w1  Second part of the UUID (16 bits)
 *  @param w2  Third part of the UUID (16 bits)
 *  @param w3  Fourth part of the UUID (16 bits)
 *  @param w48 Fifth part of the UUID (48 bits)
 *
 *  @return The comma separated values for UUID 128 initializer that
 *          may be used directly as an argument for
 *          @ref BT_UUID_INIT_128 or @ref BT_UUID_DECLARE_128
 */
#define BT_UUID_128_ENCODE(w32, w1, w2, w3, w48) \
    BT_BYTES_LIST_LE48(w48),\
    BT_BYTES_LIST_LE16(w3), \
    BT_BYTES_LIST_LE16(w2), \
    BT_BYTES_LIST_LE16(w1), \
    BT_BYTES_LIST_LE32(w32)

/** @brief Encode 16-bit UUID into array values in little-endian format.
 *
 *  Helper macro to encode 16-bit UUID values into advertising data.
 *
 *  Example of how to encode the UUID `0x180a` into advertising data.
 *
 *  @code
 *  BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(0x180a))
 *  @endcode
 *
 * @param w16 UUID value (16-bits)
 *
 * @return The comma separated values for UUID 16 value that
 *         may be used directly as an argument for @ref BT_DATA_BYTES.
 */
#define BT_UUID_16_ENCODE(w16) BT_BYTES_LIST_LE16(w16)

/** @brief Encode 32-bit UUID into array values in little-endian format.
 *
 *  Helper macro to encode 32-bit UUID values into advertising data.
 *
 *  Example of how to encode the UUID `0x180a01af` into advertising data.
 *
 *  @code
 *  BT_DATA_BYTES(BT_DATA_UUID32_ALL, BT_UUID_32_ENCODE(0x180a01af))
 *  @endcode
 *
 * @param w32 UUID value (32-bits)
 *
 * @return The comma separated values for UUID 32 value that
 *         may be used directly as an argument for @ref BT_DATA_BYTES.
 */
#define BT_UUID_32_ENCODE(w32) BT_BYTES_LIST_LE32(w32)

/**
 *  @brief Recommended length of user string buffer for Bluetooth UUID.
 *
 *  @details The recommended length guarantee the output of UUID
 *  conversion will not lose valuable information about the UUID being
 *  processed. If the length of the UUID is known the string can be shorter.
 */
#define BT_UUID_STR_LEN 37

/**
 *  @brief Generic Access UUID value
 */
#define BT_UUID_GAP_VAL 0x1800
/**
 *  @brief Generic Access
 */
#define BT_UUID_GAP \
    BT_UUID_DECLARE_16(BT_UUID_GAP_VAL)
/**
 *  @brief Generic attribute UUID value
 */
#define BT_UUID_GATT_VAL 0x1801
/**
 *  @brief Generic Attribute
 */
#define BT_UUID_GATT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_VAL)
/**
 *  @brief Immediate Alert Service UUID value
 */
#define BT_UUID_IAS_VAL 0x1802
/**
 *  @brief Immediate Alert Service
 */
#define BT_UUID_IAS \
    BT_UUID_DECLARE_16(BT_UUID_IAS_VAL)
/**
 *  @brief Link Loss Service UUID value
 */
#define BT_UUID_LLS_VAL 0x1803
/**
 *  @brief Link Loss Service
 */
#define BT_UUID_LLS \
    BT_UUID_DECLARE_16(BT_UUID_LLS_VAL)
/**
 *  @brief Tx Power Service UUID value
 */
#define BT_UUID_TPS_VAL 0x1804
/**
 *  @brief Tx Power Service
 */
#define BT_UUID_TPS \
    BT_UUID_DECLARE_16(BT_UUID_TPS_VAL)
/**
 *  @brief Current Time Service UUID value
 */
#define BT_UUID_CTS_VAL 0x1805
/**
 *  @brief Current Time Service
 */
#define BT_UUID_CTS \
    BT_UUID_DECLARE_16(BT_UUID_CTS_VAL)
/**
 *  @brief Reference Time Update Service UUID value
 */
#define BT_UUID_RTUS_VAL 0x1806
/**
 *  @brief Reference Time Update Service
 */
#define BT_UUID_RTUS \
    BT_UUID_DECLARE_16(BT_UUID_RTUS_VAL)
/**
 *  @brief Next DST Change Service UUID value
 */
#define BT_UUID_NDSTS_VAL 0x1807
/**
 *  @brief Next DST Change Service
 */
#define BT_UUID_NDSTS \
    BT_UUID_DECLARE_16(BT_UUID_NDSTS_VAL)
/**
 *  @brief Glucose Service UUID value
 */
#define BT_UUID_GS_VAL 0x1808
/**
 *  @brief Glucose Service
 */
#define BT_UUID_GS \
    BT_UUID_DECLARE_16(BT_UUID_GS_VAL)
/**
 *  @brief Health Thermometer Service UUID value
 */
#define BT_UUID_HTS_VAL 0x1809
/**
 *  @brief Health Thermometer Service
 */
#define BT_UUID_HTS \
    BT_UUID_DECLARE_16(BT_UUID_HTS_VAL)
/**
 *  @brief Device Information Service UUID value
 */
#define BT_UUID_DIS_VAL 0x180a
/**
 *  @brief Device Information Service
 */
#define BT_UUID_DIS \
    BT_UUID_DECLARE_16(BT_UUID_DIS_VAL)
/**
 *  @brief Network Availability Service UUID value
 */
#define BT_UUID_NAS_VAL 0x180b
/**
 *  @brief Network Availability Service
 */
#define BT_UUID_NAS \
    BT_UUID_DECLARE_16(BT_UUID_NAS_VAL)
/**
 *  @brief Watchdog Service UUID value
 */
#define BT_UUID_WDS_VAL 0x180c
/**
 *  @brief Watchdog Service
 */
#define BT_UUID_WDS \
    BT_UUID_DECLARE_16(BT_UUID_WDS_VAL)
/**
 *  @brief Heart Rate Service UUID value
 */
#define BT_UUID_HRS_VAL 0x180d
/**
 *  @brief Heart Rate Service
 */
#define BT_UUID_HRS \
    BT_UUID_DECLARE_16(BT_UUID_HRS_VAL)
/**
 *  @brief Phone Alert Service UUID value
 */
#define BT_UUID_PAS_VAL 0x180e
/**
 *  @brief Phone Alert Service
 */
#define BT_UUID_PAS \
    BT_UUID_DECLARE_16(BT_UUID_PAS_VAL)
/**
 *  @brief Battery Service UUID value
 */
#define BT_UUID_BAS_VAL 0x180f
/**
 *  @brief Battery Service
 */
#define BT_UUID_BAS \
    BT_UUID_DECLARE_16(BT_UUID_BAS_VAL)
/**
 *  @brief Blood Pressure Service UUID value
 */
#define BT_UUID_BPS_VAL 0x1810
/**
 *  @brief Blood Pressure Service
 */
#define BT_UUID_BPS \
    BT_UUID_DECLARE_16(BT_UUID_BPS_VAL)
/**
 *  @brief Alert Notification Service UUID value
 */
#define BT_UUID_ANS_VAL 0x1811
/**
 *  @brief Alert Notification Service
 */
#define BT_UUID_ANS \
    BT_UUID_DECLARE_16(BT_UUID_ANS_VAL)
/**
 *  @brief HID Service UUID value
 */
#define BT_UUID_HIDS_VAL 0x1812
/**
 *  @brief HID Service
 */
#define BT_UUID_HIDS \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_VAL)
/**
 *  @brief Scan Parameters Service UUID value
 */
#define BT_UUID_SPS_VAL 0x1813
/**
 *  @brief Scan Parameters Service
 */
#define BT_UUID_SPS \
    BT_UUID_DECLARE_16(BT_UUID_SPS_VAL)
/**
 *  @brief Running Speed and Cadence Service UUID value
 */
#define BT_UUID_RSCS_VAL 0x1814
/**
 *  @brief Running Speed and Cadence Service
 */
#define BT_UUID_RSCS \
    BT_UUID_DECLARE_16(BT_UUID_RSCS_VAL)
/**
 *  @brief Automation IO Service UUID value
 */
#define BT_UUID_AIOS_VAL 0x1815
/**
 *  @brief Automation IO Service
 */
#define BT_UUID_AIOS \
    BT_UUID_DECLARE_16(BT_UUID_AIOS_VAL)
/**
 *  @brief Cycling Speed and Cadence Service UUID value
 */
#define BT_UUID_CSC_VAL 0x1816
/**
 *  @brief Cycling Speed and Cadence Service
 */
#define BT_UUID_CSC \
    BT_UUID_DECLARE_16(BT_UUID_CSC_VAL)
/**
 *  @brief Cycling Power Service UUID value
 */
#define BT_UUID_CPS_VAL 0x1818
/**
 *  @brief Cycling Power Service
 */
#define BT_UUID_CPS \
    BT_UUID_DECLARE_16(BT_UUID_CPS_VAL)
/**
 *  @brief Location and Navigation Service UUID value
 */
#define BT_UUID_LNS_VAL 0x1819
/**
 *  @brief Location and Navigation Service
 */
#define BT_UUID_LNS \
    BT_UUID_DECLARE_16(BT_UUID_LNS_VAL)
/**
 *  @brief Environmental Sensing Service UUID value
 */
#define BT_UUID_ESS_VAL 0x181a
/**
 *  @brief Environmental Sensing Service
 */
#define BT_UUID_ESS \
    BT_UUID_DECLARE_16(BT_UUID_ESS_VAL)
/**
 *  @brief Body Composition Service UUID value
 */
#define BT_UUID_BCS_VAL 0x181b
/**
 *  @brief Body Composition Service
 */
#define BT_UUID_BCS \
    BT_UUID_DECLARE_16(BT_UUID_BCS_VAL)
/**
 *  @brief User Data Service UUID value
 */
#define BT_UUID_UDS_VAL 0x181c
/**
 *  @brief User Data Service
 */
#define BT_UUID_UDS \
    BT_UUID_DECLARE_16(BT_UUID_UDS_VAL)
/**
 *  @brief Weight Scale Service UUID value
 */
#define BT_UUID_WSS_VAL 0x181d
/**
 *  @brief Weight Scale Service
 */
#define BT_UUID_WSS \
    BT_UUID_DECLARE_16(BT_UUID_WSS_VAL)
/**
 *  @brief Bond Management Service UUID value
 */
#define BT_UUID_BMS_VAL 0x181e
/**
 *  @brief Bond Management Service
 */
#define BT_UUID_BMS \
    BT_UUID_DECLARE_16(BT_UUID_BMS_VAL)
/**
 *  @brief Continuous Glucose Monitoring Service UUID value
 */
#define BT_UUID_CGMS_VAL 0x181f
/**
 *  @brief Continuous Glucose Monitoring Service
 */
#define BT_UUID_CGMS \
    BT_UUID_DECLARE_16(BT_UUID_CGMS_VAL)
/**
 *  @brief IP Support Service UUID value
 */
#define BT_UUID_IPSS_VAL 0x1820
/**
 *  @brief IP Support Service
 */
#define BT_UUID_IPSS \
    BT_UUID_DECLARE_16(BT_UUID_IPSS_VAL)
/**
 *  @brief Indoor Positioning Service UUID value
 */
#define BT_UUID_IPS_VAL 0x1821
/**
 *  @brief Indoor Positioning Service
 */
#define BT_UUID_IPS \
    BT_UUID_DECLARE_16(BT_UUID_IPS_VAL)
/**
 *  @brief Pulse Oximeter Service UUID value
 */
#define BT_UUID_POS_VAL 0x1822
/**
 *  @brief Pulse Oximeter Service
 */
#define BT_UUID_POS \
    BT_UUID_DECLARE_16(BT_UUID_POS_VAL)
/**
 *  @brief HTTP Proxy Service UUID value
 */
#define BT_UUID_HPS_VAL 0x1823
/**
 *  @brief HTTP Proxy Service
 */
#define BT_UUID_HPS \
    BT_UUID_DECLARE_16(BT_UUID_HPS_VAL)
/**
 *  @brief Transport Discovery Service UUID value
 */
#define BT_UUID_TDS_VAL 0x1824
/**
 *  @brief Transport Discovery Service
 */
#define BT_UUID_TDS \
    BT_UUID_DECLARE_16(BT_UUID_TDS_VAL)
/**
 *  @brief Object Transfer Service UUID value
 */
#define BT_UUID_OTS_VAL 0x1825
/**
 *  @brief Object Transfer Service
 */
#define BT_UUID_OTS \
    BT_UUID_DECLARE_16(BT_UUID_OTS_VAL)
/**
 *  @brief Fitness Machine Service UUID value
 */
#define BT_UUID_FMS_VAL 0x1826
/**
 *  @brief Fitness Machine Service
 */
#define BT_UUID_FMS \
    BT_UUID_DECLARE_16(BT_UUID_FMS_VAL)
/**
 *  @brief Mesh Provisioning Service UUID value
 */
#define BT_UUID_MESH_PROV_VAL 0x1827
/**
 *  @brief Mesh Provisioning Service
 */
#define BT_UUID_MESH_PROV \
    BT_UUID_DECLARE_16(BT_UUID_MESH_PROV_VAL)
/**
 *  @brief Mesh Proxy Service UUID value
 */
#define BT_UUID_MESH_PROXY_VAL 0x1828
/**
 *  @brief Mesh Proxy Service
 */
#define BT_UUID_MESH_PROXY \
    BT_UUID_DECLARE_16(BT_UUID_MESH_PROXY_VAL)
/**
 *  @brief Proxy Solicitation UUID value
 */
#define BT_UUID_MESH_PROXY_SOLICITATION_VAL 0x1859
/**
 *  @brief Reconnection Configuration Service UUID value
 */
#define BT_UUID_RCSRV_VAL 0x1829
/**
 *  @brief Reconnection Configuration Service
 */
#define BT_UUID_RCSRV \
    BT_UUID_DECLARE_16(BT_UUID_RCSRV_VAL)
/**
 *  @brief Insulin Delivery Service UUID value
 */
#define BT_UUID_IDS_VAL 0x183a
/**
 *  @brief Insulin Delivery Service
 */
#define BT_UUID_IDS \
    BT_UUID_DECLARE_16(BT_UUID_IDS_VAL)
/**
 *  @brief Binary Sensor Service UUID value
 */
#define BT_UUID_BSS_VAL 0x183b
/**
 *  @brief Binary Sensor Service
 */
#define BT_UUID_BSS \
    BT_UUID_DECLARE_16(BT_UUID_BSS_VAL)
/**
 *  @brief Emergency Configuration Service UUID value
 */
#define BT_UUID_ECS_VAL 0x183c
/**
 *  @brief Emergency Configuration Service
 */
#define BT_UUID_ECS \
    BT_UUID_DECLARE_16(BT_UUID_ECS_VAL)
/**
 *  @brief Authorization Control Service UUID value
 */
#define BT_UUID_ACLS_VAL 0x183d
/**
 *  @brief Authorization Control Service
 */
#define BT_UUID_ACLS \
    BT_UUID_DECLARE_16(BT_UUID_ACLS_VAL)
/**
 *  @brief Physical Activity Monitor Service UUID value
 */
#define BT_UUID_PAMS_VAL 0x183e
/**
 *  @brief Physical Activity Monitor Service
 */
#define BT_UUID_PAMS \
    BT_UUID_DECLARE_16(BT_UUID_PAMS_VAL)
/**
 *  @brief Audio Input Control Service UUID value
 */
#define BT_UUID_AICS_VAL 0x1843
/**
 *  @brief Audio Input Control Service
 */
#define BT_UUID_AICS \
    BT_UUID_DECLARE_16(BT_UUID_AICS_VAL)
/**
 *  @brief Volume Control Service UUID value
 */
#define BT_UUID_VCS_VAL 0x1844
/**
 *  @brief Volume Control Service
 */
#define BT_UUID_VCS \
    BT_UUID_DECLARE_16(BT_UUID_VCS_VAL)
/**
 *  @brief Volume Offset Control Service UUID value
 */
#define BT_UUID_VOCS_VAL 0x1845
/**
 *  @brief Volume Offset Control Service
 */
#define BT_UUID_VOCS \
    BT_UUID_DECLARE_16(BT_UUID_VOCS_VAL)
/**
 *  @brief Coordinated Set Identification Service UUID value
 */
#define BT_UUID_CSIS_VAL 0x1846
/**
 *  @brief Coordinated Set Identification Service
 */
#define BT_UUID_CSIS \
    BT_UUID_DECLARE_16(BT_UUID_CSIS_VAL)
/**
 *  @brief Device Time Service UUID value
 */
#define BT_UUID_DTS_VAL 0x1847
/**
 *  @brief Device Time Service
 */
#define BT_UUID_DTS \
    BT_UUID_DECLARE_16(BT_UUID_DTS_VAL)
/**
 *  @brief Media Control Service UUID value
 */
#define BT_UUID_MCS_VAL 0x1848
/**
 *  @brief Media Control Service
 */
#define BT_UUID_MCS \
    BT_UUID_DECLARE_16(BT_UUID_MCS_VAL)
/**
 *  @brief Generic Media Control Service UUID value
 */
#define BT_UUID_GMCS_VAL 0x1849
/**
 *  @brief Generic Media Control Service
 */
#define BT_UUID_GMCS \
    BT_UUID_DECLARE_16(BT_UUID_GMCS_VAL)
/**
 *  @brief Constant Tone Extension Service UUID value
 */
#define BT_UUID_CTES_VAL 0x184a
/**
 *  @brief Constant Tone Extension Service
 */
#define BT_UUID_CTES \
    BT_UUID_DECLARE_16(BT_UUID_CTES_VAL)
/**
 *  @brief Telephone Bearer Service UUID value
 */
#define BT_UUID_TBS_VAL 0x184b
/**
 *  @brief Telephone Bearer Service
 */
#define BT_UUID_TBS \
    BT_UUID_DECLARE_16(BT_UUID_TBS_VAL)
/**
 *  @brief Generic Telephone Bearer Service UUID value
 */
#define BT_UUID_GTBS_VAL 0x184c
/**
 *  @brief Generic Telephone Bearer Service
 */
#define BT_UUID_GTBS \
    BT_UUID_DECLARE_16(BT_UUID_GTBS_VAL)
/**
 *  @brief Microphone Control Service UUID value
 */
#define BT_UUID_MICS_VAL 0x184d
/**
 *  @brief Microphone Control Service
 */
#define BT_UUID_MICS \
    BT_UUID_DECLARE_16(BT_UUID_MICS_VAL)
/**
 *  @brief Audio Stream Control Service UUID value
 */
#define BT_UUID_ASCS_VAL 0x184e
/**
 *  @brief Audio Stream Control Service
 */
#define BT_UUID_ASCS \
    BT_UUID_DECLARE_16(BT_UUID_ASCS_VAL)
/**
 *  @brief Broadcast Audio Scan Service UUID value
 */
#define BT_UUID_BASS_VAL 0x184f
/**
 *  @brief Broadcast Audio Scan Service
 */
#define BT_UUID_BASS \
    BT_UUID_DECLARE_16(BT_UUID_BASS_VAL)
/**
 *  @brief Published Audio Capabilities Service UUID value
 */
#define BT_UUID_PACS_VAL 0x1850
/**
 *  @brief Published Audio Capabilities Service
 */
#define BT_UUID_PACS \
    BT_UUID_DECLARE_16(BT_UUID_PACS_VAL)
/**
 *  @brief Basic Audio Announcement Service UUID value
 */
#define BT_UUID_BASIC_AUDIO_VAL 0x1851
/**
 *  @brief Basic Audio Announcement Service
 */
#define BT_UUID_BASIC_AUDIO \
    BT_UUID_DECLARE_16(BT_UUID_BASIC_AUDIO_VAL)
/**
 *  @brief Broadcast Audio Announcement Service UUID value
 */
#define BT_UUID_BROADCAST_AUDIO_VAL 0x1852
/**
 *  @brief Broadcast Audio Announcement Service
 */
#define BT_UUID_BROADCAST_AUDIO \
    BT_UUID_DECLARE_16(BT_UUID_BROADCAST_AUDIO_VAL)
/**
 *  @brief Common Audio Service UUID value
 */
#define BT_UUID_CAS_VAL 0x1853
/**
 *  @brief Common Audio Service
 */
#define BT_UUID_CAS \
    BT_UUID_DECLARE_16(BT_UUID_CAS_VAL)
/**
 *  @brief Hearing Access Service UUID value
 */
#define BT_UUID_HAS_VAL 0x1854
/**
 *  @brief Hearing Access Service
 */
#define BT_UUID_HAS \
    BT_UUID_DECLARE_16(BT_UUID_HAS_VAL)
/**
 *  @brief Telephony and Media Audio Service UUID value
 */
#define BT_UUID_TMAS_VAL 0x1855
/**
 *  @brief Telephony and Media Audio Service
 */
#define BT_UUID_TMAS \
    BT_UUID_DECLARE_16(BT_UUID_TMAS_VAL)
/**
 *  @brief Public Broadcast Announcement Service UUID value
 */
#define BT_UUID_PBA_VAL 0x1856
/**
 *  @brief Public Broadcast Announcement Service
 */
#define BT_UUID_PBA \
    BT_UUID_DECLARE_16(BT_UUID_PBA_VAL)
/**
 *  @brief GATT Primary Service UUID value
 */
#define BT_UUID_GATT_PRIMARY_VAL 0x2800
/**
 *  @brief GATT Primary Service
 */
#define BT_UUID_GATT_PRIMARY \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PRIMARY_VAL)
/**
 *  @brief GATT Secondary Service UUID value
 */
#define BT_UUID_GATT_SECONDARY_VAL 0x2801
/**
 *  @brief GATT Secondary Service
 */
#define BT_UUID_GATT_SECONDARY \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SECONDARY_VAL)
/**
 *  @brief GATT Include Service UUID value
 */
#define BT_UUID_GATT_INCLUDE_VAL 0x2802
/**
 *  @brief GATT Include Service
 */
#define BT_UUID_GATT_INCLUDE \
    BT_UUID_DECLARE_16(BT_UUID_GATT_INCLUDE_VAL)
/**
 *  @brief GATT Characteristic UUID value
 */
#define BT_UUID_GATT_CHRC_VAL 0x2803
/**
 *  @brief GATT Characteristic
 */
#define BT_UUID_GATT_CHRC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CHRC_VAL)
/**
 *  @brief GATT Characteristic Extended Properties UUID value
 */
#define BT_UUID_GATT_CEP_VAL 0x2900
/**
 *  @brief GATT Characteristic Extended Properties
 */
#define BT_UUID_GATT_CEP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CEP_VAL)
/**
 *  @brief GATT Characteristic User Description UUID value
 */
#define BT_UUID_GATT_CUD_VAL 0x2901
/**
 *  @brief GATT Characteristic User Description
 */
#define BT_UUID_GATT_CUD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CUD_VAL)
/**
 *  @brief GATT Client Characteristic Configuration UUID value
 */
#define BT_UUID_GATT_CCC_VAL 0x2902
/**
 *  @brief GATT Client Characteristic Configuration
 */
#define BT_UUID_GATT_CCC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CCC_VAL)
/**
 *  @brief GATT Server Characteristic Configuration UUID value
 */
#define BT_UUID_GATT_SCC_VAL 0x2903
/**
 *  @brief GATT Server Characteristic Configuration
 */
#define BT_UUID_GATT_SCC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SCC_VAL)
/**
 *  @brief GATT Characteristic Presentation Format UUID value
 */
#define BT_UUID_GATT_CPF_VAL 0x2904
/**
 *  @brief GATT Characteristic Presentation Format
 */
#define BT_UUID_GATT_CPF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CPF_VAL)
/**
 *  @brief GATT Characteristic Aggregated Format UUID value
 */
#define BT_UUID_GATT_CAF_VAL 0x2905
/**
 *  @brief GATT Characteristic Aggregated Format
 */
#define BT_UUID_GATT_CAF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CAF_VAL)
/**
 *  @brief Valid Range Descriptor UUID value
 */
#define BT_UUID_VALID_RANGE_VAL 0x2906
/**
 *  @brief Valid Range Descriptor
 */
#define BT_UUID_VALID_RANGE \
    BT_UUID_DECLARE_16(BT_UUID_VALID_RANGE_VAL)
/**
 *  @brief HID External Report Descriptor UUID value
 */
#define BT_UUID_HIDS_EXT_REPORT_VAL 0x2907
/**
 *  @brief HID External Report Descriptor
 */
#define BT_UUID_HIDS_EXT_REPORT \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_EXT_REPORT_VAL)
/**
 *  @brief HID Report Reference Descriptor UUID value
 */
#define BT_UUID_HIDS_REPORT_REF_VAL 0x2908
/**
 *  @brief HID Report Reference Descriptor
 */
#define BT_UUID_HIDS_REPORT_REF \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_REPORT_REF_VAL)
/**
 *  @brief Value Trigger Setting Descriptor UUID value
 */
#define BT_UUID_VAL_TRIGGER_SETTING_VAL 0x290a
/**
 *  @brief Value Trigger Setting Descriptor
 */
#define BT_UUID_VAL_TRIGGER_SETTING \
    BT_UUID_DECLARE_16(BT_UUID_VAL_TRIGGER_SETTING_VAL)
/**
 *  @brief Environmental Sensing Configuration Descriptor UUID value
 */
#define BT_UUID_ES_CONFIGURATION_VAL 0x290b
/**
 *  @brief Environmental Sensing Configuration Descriptor
 */
#define BT_UUID_ES_CONFIGURATION \
    BT_UUID_DECLARE_16(BT_UUID_ES_CONFIGURATION_VAL)
/**
 *  @brief Environmental Sensing Measurement Descriptor UUID value
 */
#define BT_UUID_ES_MEASUREMENT_VAL 0x290c
/**
 *  @brief Environmental Sensing Measurement Descriptor
 */
#define BT_UUID_ES_MEASUREMENT \
    BT_UUID_DECLARE_16(BT_UUID_ES_MEASUREMENT_VAL)
/**
 *  @brief Environmental Sensing Trigger Setting Descriptor UUID value
 */
#define BT_UUID_ES_TRIGGER_SETTING_VAL 0x290d
/**
 *  @brief Environmental Sensing Trigger Setting Descriptor
 */
#define BT_UUID_ES_TRIGGER_SETTING \
    BT_UUID_DECLARE_16(BT_UUID_ES_TRIGGER_SETTING_VAL)
/**
 *  @brief Time Trigger Setting Descriptor UUID value
 */
#define BT_UUID_TM_TRIGGER_SETTING_VAL 0x290e
/**
 *  @brief Time Trigger Setting Descriptor
 */
#define BT_UUID_TM_TRIGGER_SETTING \
    BT_UUID_DECLARE_16(BT_UUID_TM_TRIGGER_SETTING_VAL)
/**
 *  @brief GAP Characteristic Device Name UUID value
 */
#define BT_UUID_GAP_DEVICE_NAME_VAL 0x2a00
/**
 *  @brief GAP Characteristic Device Name
 */
#define BT_UUID_GAP_DEVICE_NAME \
    BT_UUID_DECLARE_16(BT_UUID_GAP_DEVICE_NAME_VAL)
/**
 *  @brief GAP Characteristic Appearance UUID value
 */
#define BT_UUID_GAP_APPEARANCE_VAL 0x2a01
/**
 *  @brief GAP Characteristic Appearance
 */
#define BT_UUID_GAP_APPEARANCE \
    BT_UUID_DECLARE_16(BT_UUID_GAP_APPEARANCE_VAL)
/**
 *  @brief GAP Characteristic Peripheral Privacy Flag UUID value
 */
#define BT_UUID_GAP_PPF_VAL 0x2a02
/**
 *  @brief GAP Characteristic Peripheral Privacy Flag
 */
#define BT_UUID_GAP_PPF \
    BT_UUID_DECLARE_16(BT_UUID_GAP_PPF_VAL)
/**
 *  @brief GAP Characteristic Reconnection Address UUID value
 */
#define BT_UUID_GAP_RA_VAL 0x2a03
/**
 *  @brief GAP Characteristic Reconnection Address
 */
#define BT_UUID_GAP_RA \
    BT_UUID_DECLARE_16(BT_UUID_GAP_RA_VAL)
/**
 *  @brief GAP Characteristic Peripheral Preferred Connection Parameters UUID
 *         value
 */
#define BT_UUID_GAP_PPCP_VAL 0x2a04
/**
 *  @brief GAP Characteristic Peripheral Preferred Connection Parameters
 */
#define BT_UUID_GAP_PPCP \
    BT_UUID_DECLARE_16(BT_UUID_GAP_PPCP_VAL)
/**
 *  @brief GATT Characteristic Service Changed UUID value
 */
#define BT_UUID_GATT_SC_VAL 0x2a05
/**
 *  @brief GATT Characteristic Service Changed
 */
#define BT_UUID_GATT_SC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SC_VAL)
/**
 *  @brief GATT Characteristic Alert Level UUID value
 */
#define BT_UUID_ALERT_LEVEL_VAL  0x2a06
/**
 *  @brief GATT Characteristic Alert Level
 */
#define BT_UUID_ALERT_LEVEL \
    BT_UUID_DECLARE_16(BT_UUID_ALERT_LEVEL_VAL)
/**
 *  @brief TPS Characteristic Tx Power Level UUID value
 */
#define BT_UUID_TPS_TX_POWER_LEVEL_VAL 0x2a07
/**
 *  @brief TPS Characteristic Tx Power Level
 */
#define BT_UUID_TPS_TX_POWER_LEVEL \
    BT_UUID_DECLARE_16(BT_UUID_TPS_TX_POWER_LEVEL_VAL)
/**
 *  @brief GATT Characteristic Date Time UUID value
 */
#define BT_UUID_GATT_DT_VAL 0x2a08
/**
 *  @brief GATT Characteristic Date Time
 */
#define BT_UUID_GATT_DT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DT_VAL)
/**
 *  @brief GATT Characteristic Day of Week UUID value
 */
#define BT_UUID_GATT_DW_VAL 0x2a09
/**
 *  @brief GATT Characteristic Day of Week
 */
#define BT_UUID_GATT_DW \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DW_VAL)
/**
 *  @brief GATT Characteristic Day Date Time UUID value
 */
#define BT_UUID_GATT_DDT_VAL 0x2a0a
/**
 *  @brief GATT Characteristic Day Date Time
 */
#define BT_UUID_GATT_DDT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DDT_VAL)
/**
 *  @brief GATT Characteristic Exact Time 256 UUID value
 */
#define BT_UUID_GATT_ET256_VAL 0x2a0c
/**
 *  @brief GATT Characteristic Exact Time 256
 */
#define BT_UUID_GATT_ET256 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ET256_VAL)
/**
 *  @brief GATT Characteristic DST Offset UUID value
 */
#define BT_UUID_GATT_DST_VAL 0x2a0d
/**
 *  @brief GATT Characteristic DST Offset
 */
#define BT_UUID_GATT_DST \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DST_VAL)
/**
 *  @brief GATT Characteristic Time Zone UUID value
 */
#define BT_UUID_GATT_TZ_VAL 0x2a0e
/**
 *  @brief GATT Characteristic Time Zone
 */
#define BT_UUID_GATT_TZ \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TZ_VAL)
/**
 *  @brief GATT Characteristic Local Time Information UUID value
 */
#define BT_UUID_GATT_LTI_VAL 0x2a0f
/**
 *  @brief GATT Characteristic Local Time Information
 */
#define BT_UUID_GATT_LTI \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LTI_VAL)
/**
 *  @brief GATT Characteristic Time with DST UUID value
 */
#define BT_UUID_GATT_TDST_VAL 0x2a11
/**
 *  @brief GATT Characteristic Time with DST
 */
#define BT_UUID_GATT_TDST \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TDST_VAL)
/**
 *  @brief GATT Characteristic Time Accuracy UUID value
 */
#define BT_UUID_GATT_TA_VAL 0x2a12
/**
 *  @brief GATT Characteristic Time Accuracy
 */
#define BT_UUID_GATT_TA \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TA_VAL)
/**
 *  @brief GATT Characteristic Time Source UUID value
 */
#define BT_UUID_GATT_TS_VAL 0x2a13
/**
 *  @brief GATT Characteristic Time Source
 */
#define BT_UUID_GATT_TS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TS_VAL)
/**
 *  @brief GATT Characteristic Reference Time Information UUID value
 */
#define BT_UUID_GATT_RTI_VAL 0x2a14
/**
 *  @brief GATT Characteristic Reference Time Information
 */
#define BT_UUID_GATT_RTI \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RTI_VAL)
/**
 *  @brief GATT Characteristic Time Update Control Point UUID value
 */
#define BT_UUID_GATT_TUCP_VAL 0x2a16
/**
 *  @brief GATT Characteristic Time Update Control Point
 */
#define BT_UUID_GATT_TUCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TUCP_VAL)
/**
 *  @brief GATT Characteristic Time Update State UUID value
 */
#define BT_UUID_GATT_TUS_VAL 0x2a17
/**
 *  @brief GATT Characteristic Time Update State
 */
#define BT_UUID_GATT_TUS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TUS_VAL)
/**
 *  @brief GATT Characteristic Glucose Measurement UUID value
 */
#define BT_UUID_GATT_GM_VAL 0x2a18
/**
 *  @brief GATT Characteristic Glucose Measurement
 */
#define BT_UUID_GATT_GM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_GM_VAL)
/**
 *  @brief BAS Characteristic Battery Level UUID value
 */
#define BT_UUID_BAS_BATTERY_LEVEL_VAL 0x2a19
/**
 *  @brief BAS Characteristic Battery Level
 */
#define BT_UUID_BAS_BATTERY_LEVEL \
    BT_UUID_DECLARE_16(BT_UUID_BAS_BATTERY_LEVEL_VAL)
/**
 *  @brief BAS Characteristic Battery Power State UUID value
 */
#define BT_UUID_BAS_BATTERY_POWER_STATE_VAL 0x2a1a
/**
 *  @brief BAS Characteristic Battery Power State
 */
#define BT_UUID_BAS_BATTERY_POWER_STATE \
    BT_UUID_DECLARE_16(BT_UUID_BAS_BATTERY_POWER_STATE_VAL)
/**
 *  @brief BAS Characteristic Battery Level StateUUID value
 */
#define BT_UUID_BAS_BATTERY_LEVEL_STATE_VAL 0x2a1b
/**
 *  @brief BAS Characteristic Battery Level State
 */
#define BT_UUID_BAS_BATTERY_LEVEL_STATE \
    BT_UUID_DECLARE_16(BT_UUID_BAS_BATTERY_LEVEL_STATE_VAL)
/**
 *  @brief HTS Characteristic Temperature Measurement UUID value
 */
#define BT_UUID_HTS_MEASUREMENT_VAL 0x2a1c
/**
 *  @brief HTS Characteristic Temperature Measurement Value
 */
#define BT_UUID_HTS_MEASUREMENT \
    BT_UUID_DECLARE_16(BT_UUID_HTS_MEASUREMENT_VAL)
/**
 *  @brief HTS Characteristic Temperature Type UUID value
 */
#define BT_UUID_HTS_TEMP_TYP_VAL 0x2a1d
/**
 *  @brief HTS Characteristic Temperature Type
 */
#define BT_UUID_HTS_TEMP_TYP \
    BT_UUID_DECLARE_16(BT_UUID_HTS_TEMP_TYP_VAL)
/**
 *  @brief HTS Characteristic Intermediate Temperature UUID value
 */
#define BT_UUID_HTS_TEMP_INT_VAL 0x2a1e
/**
 *  @brief HTS Characteristic Intermediate Temperature
 */
#define BT_UUID_HTS_TEMP_INT \
    BT_UUID_DECLARE_16(BT_UUID_HTS_TEMP_INT_VAL)
/**
 *  @brief HTS Characteristic Temperature Celsius UUID value
 */
#define BT_UUID_HTS_TEMP_C_VAL 0x2a1f
/**
 *  @brief HTS Characteristic Temperature Celsius
 */
#define BT_UUID_HTS_TEMP_C \
    BT_UUID_DECLARE_16(BT_UUID_HTS_TEMP_C_VAL)
/**
 *  @brief HTS Characteristic Temperature Fahrenheit UUID value
 */
#define BT_UUID_HTS_TEMP_F_VAL 0x2a20
/**
 *  @brief HTS Characteristic Temperature Fahrenheit
 */
#define BT_UUID_HTS_TEMP_F \
    BT_UUID_DECLARE_16(BT_UUID_HTS_TEMP_F_VAL)
/**
 *  @brief HTS Characteristic Measurement Interval UUID value
 */
#define BT_UUID_HTS_INTERVAL_VAL 0x2a21
/**
 *  @brief HTS Characteristic Measurement Interval
 */
#define BT_UUID_HTS_INTERVAL \
    BT_UUID_DECLARE_16(BT_UUID_HTS_INTERVAL_VAL)
/**
 *  @brief HID Characteristic Boot Keyboard Input Report UUID value
 */
#define BT_UUID_HIDS_BOOT_KB_IN_REPORT_VAL 0x2a22
/**
 *  @brief HID Characteristic Boot Keyboard Input Report
 */
#define BT_UUID_HIDS_BOOT_KB_IN_REPORT \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_BOOT_KB_IN_REPORT_VAL)
/**
 *  @brief DIS Characteristic System ID UUID value
 */
#define BT_UUID_DIS_SYSTEM_ID_VAL 0x2a23
/**
 *  @brief DIS Characteristic System ID
 */
#define BT_UUID_DIS_SYSTEM_ID \
    BT_UUID_DECLARE_16(BT_UUID_DIS_SYSTEM_ID_VAL)
/**
 *  @brief DIS Characteristic Model Number String UUID value
 */
#define BT_UUID_DIS_MODEL_NUMBER_VAL 0x2a24
/**
 *  @brief DIS Characteristic Model Number String
 */
#define BT_UUID_DIS_MODEL_NUMBER \
    BT_UUID_DECLARE_16(BT_UUID_DIS_MODEL_NUMBER_VAL)
/**
 *  @brief DIS Characteristic Serial Number String UUID value
 */
#define BT_UUID_DIS_SERIAL_NUMBER_VAL 0x2a25
/**
 *  @brief DIS Characteristic Serial Number String
 */
#define BT_UUID_DIS_SERIAL_NUMBER \
    BT_UUID_DECLARE_16(BT_UUID_DIS_SERIAL_NUMBER_VAL)
/**
 *  @brief DIS Characteristic Firmware Revision String UUID value
 */
#define BT_UUID_DIS_FIRMWARE_REVISION_VAL 0x2a26
/**
 *  @brief DIS Characteristic Firmware Revision String
 */
#define BT_UUID_DIS_FIRMWARE_REVISION \
    BT_UUID_DECLARE_16(BT_UUID_DIS_FIRMWARE_REVISION_VAL)
/**
 *  @brief DIS Characteristic Hardware Revision String UUID value
 */
#define BT_UUID_DIS_HARDWARE_REVISION_VAL 0x2a27
/**
 *  @brief DIS Characteristic Hardware Revision String
 */
#define BT_UUID_DIS_HARDWARE_REVISION \
    BT_UUID_DECLARE_16(BT_UUID_DIS_HARDWARE_REVISION_VAL)
/**
 *  @brief DIS Characteristic Software Revision String UUID value
 */
#define BT_UUID_DIS_SOFTWARE_REVISION_VAL 0x2a28
/**
 *  @brief DIS Characteristic Software Revision String
 */
#define BT_UUID_DIS_SOFTWARE_REVISION \
    BT_UUID_DECLARE_16(BT_UUID_DIS_SOFTWARE_REVISION_VAL)
/**
 *  @brief DIS Characteristic Manufacturer Name String UUID Value
 */
#define BT_UUID_DIS_MANUFACTURER_NAME_VAL 0x2a29
/**
 *  @brief DIS Characteristic Manufacturer Name String
 */
#define BT_UUID_DIS_MANUFACTURER_NAME \
    BT_UUID_DECLARE_16(BT_UUID_DIS_MANUFACTURER_NAME_VAL)
/**
 *  @brief GATT Characteristic IEEE Regulatory Certification Data List UUID Value
 */
#define BT_UUID_GATT_IEEE_RCDL_VAL 0x2a2a
/**
 *  @brief GATT Characteristic IEEE Regulatory Certification Data List
 */
#define BT_UUID_GATT_IEEE_RCDL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IEEE_RCDL_VAL)
/**
 *  @brief CTS Characteristic Current Time UUID value
 */
#define BT_UUID_CTS_CURRENT_TIME_VAL 0x2a2b
/**
 *  @brief CTS Characteristic Current Time
 */
#define BT_UUID_CTS_CURRENT_TIME \
    BT_UUID_DECLARE_16(BT_UUID_CTS_CURRENT_TIME_VAL)
/**
 *  @brief Magnetic Declination Characteristic UUID value
 */
#define BT_UUID_MAGN_DECLINATION_VAL 0x2a2c
/**
 *  @brief Magnetic Declination Characteristic
 */
#define BT_UUID_MAGN_DECLINATION \
    BT_UUID_DECLARE_16(BT_UUID_MAGN_DECLINATION_VAL)
/**
 *  @brief GATT Characteristic Legacy Latitude UUID Value
 */
#define BT_UUID_GATT_LLAT_VAL 0x2a2d
/**
 *  @brief GATT Characteristic Legacy Latitude
 */
#define BT_UUID_GATT_LLAT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LLAT_VAL)
/**
 *  @brief GATT Characteristic Legacy Longitude UUID Value
 */
#define BT_UUID_GATT_LLON_VAL 0x2a2e
/**
 *  @brief GATT Characteristic Legacy Longitude
 */
#define BT_UUID_GATT_LLON \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LLON_VAL)
/**
 *  @brief GATT Characteristic Position 2D UUID Value
 */
#define BT_UUID_GATT_POS_2D_VAL 0x2a2f
/**
 *  @brief GATT Characteristic Position 2D
 */
#define BT_UUID_GATT_POS_2D \
    BT_UUID_DECLARE_16(BT_UUID_GATT_POS_2D_VAL)
/**
 *  @brief GATT Characteristic Position 3D UUID Value
 */
#define BT_UUID_GATT_POS_3D_VAL 0x2a30
/**
 *  @brief GATT Characteristic Position 3D
 */
#define BT_UUID_GATT_POS_3D \
    BT_UUID_DECLARE_16(BT_UUID_GATT_POS_3D_VAL)
/**
 *  @brief GATT Characteristic Scan Refresh UUID Value
 */
#define BT_UUID_GATT_SR_VAL 0x2a31
/**
 *  @brief GATT Characteristic Scan Refresh
 */
#define BT_UUID_GATT_SR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SR_VAL)
/**
 *  @brief HID Boot Keyboard Output Report Characteristic UUID value
 */
#define BT_UUID_HIDS_BOOT_KB_OUT_REPORT_VAL 0x2a32
/**
 *  @brief HID Boot Keyboard Output Report Characteristic
 */
#define BT_UUID_HIDS_BOOT_KB_OUT_REPORT \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_BOOT_KB_OUT_REPORT_VAL)
/**
 *  @brief HID Boot Mouse Input Report Characteristic UUID value
 */
#define BT_UUID_HIDS_BOOT_MOUSE_IN_REPORT_VAL 0x2a33
/**
 *  @brief HID Boot Mouse Input Report Characteristic
 */
#define BT_UUID_HIDS_BOOT_MOUSE_IN_REPORT \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_BOOT_MOUSE_IN_REPORT_VAL)
/**
 *  @brief GATT Characteristic Glucose Measurement Context UUID Value
 */
#define BT_UUID_GATT_GMC_VAL 0x2a34
/**
 *  @brief GATT Characteristic Glucose Measurement Context
 */
#define BT_UUID_GATT_GMC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_GMC_VAL)
/**
 *  @brief GATT Characteristic Blood Pressure Measurement UUID Value
 */
#define BT_UUID_GATT_BPM_VAL 0x2a35
/**
 *  @brief GATT Characteristic Blood Pressure Measurement
 */
#define BT_UUID_GATT_BPM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_BPM_VAL)
/**
 *  @brief GATT Characteristic Intermediate Cuff Pressure UUID Value
 */
#define BT_UUID_GATT_ICP_VAL 0x2a36
/**
 *  @brief GATT Characteristic Intermediate Cuff Pressure
 */
#define BT_UUID_GATT_ICP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ICP_VAL)
/**
 *  @brief HRS Characteristic Measurement Interval UUID value
 */
#define BT_UUID_HRS_MEASUREMENT_VAL 0x2a37
/**
 *  @brief HRS Characteristic Measurement Interval
 */
#define BT_UUID_HRS_MEASUREMENT \
    BT_UUID_DECLARE_16(BT_UUID_HRS_MEASUREMENT_VAL)
/**
 *  @brief HRS Characteristic Body Sensor Location
 */
#define BT_UUID_HRS_BODY_SENSOR_VAL 0x2a38
/**
 *  @brief HRS Characteristic Control Point
 */
#define BT_UUID_HRS_BODY_SENSOR \
    BT_UUID_DECLARE_16(BT_UUID_HRS_BODY_SENSOR_VAL)
/**
 *  @brief HRS Characteristic Control Point UUID value
 */
#define BT_UUID_HRS_CONTROL_POINT_VAL 0x2a39
/**
 *  @brief HRS Characteristic Control Point
 */
#define BT_UUID_HRS_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_HRS_CONTROL_POINT_VAL)
/**
 *  @brief GATT Characteristic Removable UUID Value
 */
#define BT_UUID_GATT_REM_VAL 0x2a3a
/**
 *  @brief GATT Characteristic Removable
 */
#define BT_UUID_GATT_REM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_REM_VAL)
/**
 *  @brief GATT Characteristic Service Required UUID Value
 */
#define BT_UUID_GATT_SRVREQ_VAL 0x2a3b
/**
 *  @brief GATT Characteristic Service Required
 */
#define BT_UUID_GATT_SRVREQ \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SRVREQ_VAL)
/**
 *  @brief GATT Characteristic Scientific Temperature in Celsius UUID Value
 */
#define BT_UUID_GATT_SC_TEMP_C_VAL 0x2a3c
/**
 *  @brief GATT Characteristic Scientific Temperature in Celsius
 */
#define BT_UUID_GATT_SC_TEMP_C \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SC_TEMP_C_VAL)
/**
 *  @brief GATT Characteristic String UUID Value
 */
#define BT_UUID_GATT_STRING_VAL 0x2a3d
/**
 *  @brief GATT Characteristic String
 */
#define BT_UUID_GATT_STRING \
    BT_UUID_DECLARE_16(BT_UUID_GATT_STRING_VAL)
/**
 *  @brief GATT Characteristic Network Availability UUID Value
 */
#define BT_UUID_GATT_NETA_VAL 0x2a3e
/**
 *  @brief GATT Characteristic Network Availability
 */
#define BT_UUID_GATT_NETA \
    BT_UUID_DECLARE_16(BT_UUID_GATT_NETA_VAL)
/**
 *  @brief GATT Characteristic Alert Status UUID Value
 */
#define BT_UUID_GATT_ALRTS_VAL 0x2a3f
/**
 *  @brief GATT Characteristic Alert Status
 */
#define BT_UUID_GATT_ALRTS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ALRTS_VAL)
/**
 *  @brief GATT Characteristic Ringer Control Point UUID Value
 */
#define BT_UUID_GATT_RCP_VAL 0x2a40
/**
 *  @brief GATT Characteristic Ringer Control Point
 */
#define BT_UUID_GATT_RCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RCP_VAL)
/**
 *  @brief GATT Characteristic Ringer Setting UUID Value
 */
#define BT_UUID_GATT_RS_VAL 0x2a41
/**
 *  @brief GATT Characteristic Ringer Setting
 */
#define BT_UUID_GATT_RS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RS_VAL)
/**
 *  @brief GATT Characteristic Alert Category ID Bit Mask UUID Value
 */
#define BT_UUID_GATT_ALRTCID_MASK_VAL 0x2a42
/**
 *  @brief GATT Characteristic Alert Category ID Bit Mask
 */
#define BT_UUID_GATT_ALRTCID_MASK \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ALRTCID_MASK_VAL)
/**
 *  @brief GATT Characteristic Alert Category ID UUID Value
 */
#define BT_UUID_GATT_ALRTCID_VAL 0x2a43
/**
 *  @brief GATT Characteristic Alert Category ID
 */
#define BT_UUID_GATT_ALRTCID \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ALRTCID_VAL)
/**
 *  @brief GATT Characteristic Alert Notification Control Point Value
 */
#define BT_UUID_GATT_ALRTNCP_VAL 0x2a44
/**
 *  @brief GATT Characteristic Alert Notification Control Point
 */
#define BT_UUID_GATT_ALRTNCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ALRTNCP_VAL)
/**
 *  @brief GATT Characteristic Unread Alert Status UUID Value
 */
#define BT_UUID_GATT_UALRTS_VAL 0x2a45
/**
 *  @brief GATT Characteristic Unread Alert Status
 */
#define BT_UUID_GATT_UALRTS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_UALRTS_VAL)
/**
 *  @brief GATT Characteristic New Alert UUID Value
 */
#define BT_UUID_GATT_NALRT_VAL 0x2a46
/**
 *  @brief GATT Characteristic New Alert
 */
#define BT_UUID_GATT_NALRT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_NALRT_VAL)
/**
 *  @brief GATT Characteristic Supported New Alert Category UUID Value
 */
#define BT_UUID_GATT_SNALRTC_VAL 0x2a47
/**
 *  @brief GATT Characteristic Supported New Alert Category
 */
#define BT_UUID_GATT_SNALRTC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SNALRTC_VAL)
/**
 *  @brief GATT Characteristic Supported Unread Alert Category UUID Value
 */
#define BT_UUID_GATT_SUALRTC_VAL 0x2a48
/**
 *  @brief GATT Characteristic Supported Unread Alert Category
 */
#define BT_UUID_GATT_SUALRTC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SUALRTC_VAL)
/**
 *  @brief GATT Characteristic Blood Pressure Feature UUID Value
 */
#define BT_UUID_GATT_BPF_VAL 0x2a49
/**
 *  @brief GATT Characteristic Blood Pressure Feature
 */
#define BT_UUID_GATT_BPF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_BPF_VAL)
/**
 *  @brief HID Information Characteristic UUID value
 */
#define BT_UUID_HIDS_INFO_VAL 0x2a4a
/**
 *  @brief HID Information Characteristic
 */
#define BT_UUID_HIDS_INFO \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_INFO_VAL)
/**
 *  @brief HID Report Map Characteristic UUID value
 */
#define BT_UUID_HIDS_REPORT_MAP_VAL 0x2a4b
/**
 *  @brief HID Report Map Characteristic
 */
#define BT_UUID_HIDS_REPORT_MAP \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_REPORT_MAP_VAL)
/**
 *  @brief HID Control Point Characteristic UUID value
 */
#define BT_UUID_HIDS_CTRL_POINT_VAL 0x2a4c
/**
 *  @brief HID Control Point Characteristic
 */
#define BT_UUID_HIDS_CTRL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_CTRL_POINT_VAL)
/**
 *  @brief HID Report Characteristic UUID value
 */
#define BT_UUID_HIDS_REPORT_VAL 0x2a4d
/**
 *  @brief HID Report Characteristic
 */
#define BT_UUID_HIDS_REPORT \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_REPORT_VAL)
/**
 *  @brief HID Protocol Mode Characteristic UUID value
 */
#define BT_UUID_HIDS_PROTOCOL_MODE_VAL 0x2a4e
/**
 *  @brief HID Protocol Mode Characteristic
 */
#define BT_UUID_HIDS_PROTOCOL_MODE \
    BT_UUID_DECLARE_16(BT_UUID_HIDS_PROTOCOL_MODE_VAL)
/**
 *  @brief GATT Characteristic Scan Interval Windows UUID Value
 */
#define BT_UUID_GATT_SIW_VAL 0x2a4f
/**
 *  @brief GATT Characteristic Scan Interval Windows
 */
#define BT_UUID_GATT_SIW \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SIW_VAL)
/**
 *  @brief DIS Characteristic PnP ID UUID value
 */
#define BT_UUID_DIS_PNP_ID_VAL 0x2a50
/**
 *  @brief DIS Characteristic PnP ID
 */
#define BT_UUID_DIS_PNP_ID \
    BT_UUID_DECLARE_16(BT_UUID_DIS_PNP_ID_VAL)
/**
 *  @brief GATT Characteristic Glucose Feature UUID Value
 */
#define BT_UUID_GATT_GF_VAL 0x2a51
/**
 *  @brief GATT Characteristic Glucose Feature
 */
#define BT_UUID_GATT_GF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_GF_VAL)
/**
 *  @brief Record Access Control Point Characteristic value
 */
#define BT_UUID_RECORD_ACCESS_CONTROL_POINT_VAL 0x2a52
/**
 *  @brief Record Access Control Point
 */
#define BT_UUID_RECORD_ACCESS_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_RECORD_ACCESS_CONTROL_POINT_VAL)
/**
 *  @brief RSC Measurement Characteristic UUID value
 */
#define BT_UUID_RSC_MEASUREMENT_VAL 0x2a53
/**
 *  @brief RSC Measurement Characteristic
 */
#define BT_UUID_RSC_MEASUREMENT \
    BT_UUID_DECLARE_16(BT_UUID_RSC_MEASUREMENT_VAL)
/**
 *  @brief RSC Feature Characteristic UUID value
 */
#define BT_UUID_RSC_FEATURE_VAL 0x2a54
/**
 *  @brief RSC Feature Characteristic
 */
#define BT_UUID_RSC_FEATURE \
    BT_UUID_DECLARE_16(BT_UUID_RSC_FEATURE_VAL)
/**
 *  @brief SC Control Point Characteristic UUID value
 */
#define BT_UUID_SC_CONTROL_POINT_VAL 0x2a55
/**
 *  @brief SC Control Point Characteristic
 */
#define BT_UUID_SC_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_SC_CONTROL_POINT_VAL)
/**
 *  @brief GATT Characteristic Digital Input UUID Value
 */
#define BT_UUID_GATT_DI_VAL 0x2a56
/**
 *  @brief GATT Characteristic Digital Input
 */
#define BT_UUID_GATT_DI \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DI_VAL)
/**
 *  @brief GATT Characteristic Digital Output UUID Value
 */
#define BT_UUID_GATT_DO_VAL 0x2a57
/**
 *  @brief GATT Characteristic Digital Output
 */
#define BT_UUID_GATT_DO \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DO_VAL)
/**
 *  @brief GATT Characteristic Analog Input UUID Value
 */
#define BT_UUID_GATT_AI_VAL 0x2a58
/**
 *  @brief GATT Characteristic Analog Input
 */
#define BT_UUID_GATT_AI \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AI_VAL)
/**
 *  @brief GATT Characteristic Analog Output UUID Value
 */
#define BT_UUID_GATT_AO_VAL 0x2a59
/**
 *  @brief GATT Characteristic Analog Output
 */
#define BT_UUID_GATT_AO \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AO_VAL)
/**
 *  @brief GATT Characteristic Aggregate UUID Value
 */
#define BT_UUID_GATT_AGGR_VAL 0x2a5a
/**
 *  @brief GATT Characteristic Aggregate
 */
#define BT_UUID_GATT_AGGR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AGGR_VAL)
/**
 *  @brief CSC Measurement Characteristic UUID value
 */
#define BT_UUID_CSC_MEASUREMENT_VAL 0x2a5b
/**
 *  @brief CSC Measurement Characteristic
 */
#define BT_UUID_CSC_MEASUREMENT \
    BT_UUID_DECLARE_16(BT_UUID_CSC_MEASUREMENT_VAL)
/**
 *  @brief CSC Feature Characteristic UUID value
 */
#define BT_UUID_CSC_FEATURE_VAL 0x2a5c
/**
 *  @brief CSC Feature Characteristic
 */
#define BT_UUID_CSC_FEATURE \
    BT_UUID_DECLARE_16(BT_UUID_CSC_FEATURE_VAL)
/**
 *  @brief Sensor Location Characteristic UUID value
 */
#define BT_UUID_SENSOR_LOCATION_VAL 0x2a5d
/**
 *  @brief Sensor Location Characteristic
 */
#define BT_UUID_SENSOR_LOCATION \
    BT_UUID_DECLARE_16(BT_UUID_SENSOR_LOCATION_VAL)
/**
 *  @brief GATT Characteristic PLX Spot-Check Measurement UUID Value
 */
#define BT_UUID_GATT_PLX_SCM_VAL 0x2a5e
/**
 *  @brief GATT Characteristic PLX Spot-Check Measurement
 */
#define BT_UUID_GATT_PLX_SCM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PLX_SCM_VAL)
/**
 *  @brief GATT Characteristic PLX Continuous Measurement UUID Value
 */
#define BT_UUID_GATT_PLX_CM_VAL 0x2a5f
/**
 *  @brief GATT Characteristic PLX Continuous Measurement
 */
#define BT_UUID_GATT_PLX_CM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PLX_CM_VAL)
/**
 *  @brief GATT Characteristic PLX Features UUID Value
 */
#define BT_UUID_GATT_PLX_F_VAL 0x2a60
/**
 *  @brief GATT Characteristic PLX Features
 */
#define BT_UUID_GATT_PLX_F \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PLX_F_VAL)
/**
 *  @brief GATT Characteristic Pulse Oximetry Pulastile Event UUID Value
 */
#define BT_UUID_GATT_POPE_VAL 0x2a61
/**
 *  @brief GATT Characteristic Pulse Oximetry Pulsatile Event
 */
#define BT_UUID_GATT_POPE \
    BT_UUID_DECLARE_16(BT_UUID_GATT_POPE_VAL)
/**
 *  @brief GATT Characteristic Pulse Oximetry Control Point UUID Value
 */
#define BT_UUID_GATT_POCP_VAL 0x2a62
/**
 *  @brief GATT Characteristic Pulse Oximetry Control Point
 */
#define BT_UUID_GATT_POCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_POCP_VAL)
/**
 *  @brief GATT Characteristic Cycling Power Measurement UUID Value
 */
#define BT_UUID_GATT_CPS_CPM_VAL 0x2a63
/**
 *  @brief GATT Characteristic Cycling Power Measurement
 */
#define BT_UUID_GATT_CPS_CPM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CPS_CPM_VAL)
/**
 *  @brief GATT Characteristic Cycling Power Vector UUID Value
 */
#define BT_UUID_GATT_CPS_CPV_VAL 0x2a64
/**
 *  @brief GATT Characteristic Cycling Power Vector
 */
#define BT_UUID_GATT_CPS_CPV \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CPS_CPV_VAL)
/**
 *  @brief GATT Characteristic Cycling Power Feature UUID Value
 */
#define BT_UUID_GATT_CPS_CPF_VAL 0x2a65
/**
 *  @brief GATT Characteristic Cycling Power Feature
 */
#define BT_UUID_GATT_CPS_CPF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CPS_CPF_VAL)
/**
 *  @brief GATT Characteristic Cycling Power Control Point UUID Value
 */
#define BT_UUID_GATT_CPS_CPCP_VAL 0x2a66
/**
 *  @brief GATT Characteristic Cycling Power Control Point
 */
#define BT_UUID_GATT_CPS_CPCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CPS_CPCP_VAL)
/**
 *  @brief GATT Characteristic Location and Speed UUID Value
 */
#define BT_UUID_GATT_LOC_SPD_VAL 0x2a67
/**
 *  @brief GATT Characteristic Location and Speed
 */
#define BT_UUID_GATT_LOC_SPD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LOC_SPD_VAL)
/**
 *  @brief GATT Characteristic Navigation UUID Value
 */
#define BT_UUID_GATT_NAV_VAL 0x2a68
/**
 *  @brief GATT Characteristic Navigation
 */
#define BT_UUID_GATT_NAV \
    BT_UUID_DECLARE_16(BT_UUID_GATT_NAV_VAL)
/**
 *  @brief GATT Characteristic Position Quality UUID Value
 */
#define BT_UUID_GATT_PQ_VAL 0x2a69
/**
 *  @brief GATT Characteristic Position Quality
 */
#define BT_UUID_GATT_PQ \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PQ_VAL)
/**
 *  @brief GATT Characteristic LN Feature UUID Value
 */
#define BT_UUID_GATT_LNF_VAL 0x2a6a
/**
 *  @brief GATT Characteristic LN Feature
 */
#define BT_UUID_GATT_LNF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LNF_VAL)
/**
 *  @brief GATT Characteristic LN Control Point UUID Value
 */
#define BT_UUID_GATT_LNCP_VAL 0x2a6b
/**
 *  @brief GATT Characteristic LN Control Point
 */
#define BT_UUID_GATT_LNCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LNCP_VAL)
/**
 *  @brief Elevation Characteristic UUID value
 */
#define BT_UUID_ELEVATION_VAL 0x2a6c
/**
 *  @brief Elevation Characteristic
 */
#define BT_UUID_ELEVATION \
    BT_UUID_DECLARE_16(BT_UUID_ELEVATION_VAL)
/**
 *  @brief Pressure Characteristic UUID value
 */
#define BT_UUID_PRESSURE_VAL 0x2a6d
/**
 *  @brief Pressure Characteristic
 */
#define BT_UUID_PRESSURE \
    BT_UUID_DECLARE_16(BT_UUID_PRESSURE_VAL)
/**
 *  @brief Temperature Characteristic UUID value
 */
#define BT_UUID_TEMPERATURE_VAL 0x2a6e
/**
 *  @brief Temperature Characteristic
 */
#define BT_UUID_TEMPERATURE \
    BT_UUID_DECLARE_16(BT_UUID_TEMPERATURE_VAL)
/**
 *  @brief Humidity Characteristic UUID value
 */
#define BT_UUID_HUMIDITY_VAL 0x2a6f
/**
 *  @brief Humidity Characteristic
 */
#define BT_UUID_HUMIDITY \
    BT_UUID_DECLARE_16(BT_UUID_HUMIDITY_VAL)
/**
 *  @brief True Wind Speed Characteristic UUID value
 */
#define BT_UUID_TRUE_WIND_SPEED_VAL 0x2a70
/**
 *  @brief True Wind Speed Characteristic
 */
#define BT_UUID_TRUE_WIND_SPEED \
    BT_UUID_DECLARE_16(BT_UUID_TRUE_WIND_SPEED_VAL)
/**
 *  @brief True Wind Direction Characteristic UUID value
 */
#define BT_UUID_TRUE_WIND_DIR_VAL 0x2a71
/**
 *  @brief True Wind Direction Characteristic
 */
#define BT_UUID_TRUE_WIND_DIR \
    BT_UUID_DECLARE_16(BT_UUID_TRUE_WIND_DIR_VAL)
/**
 *  @brief Apparent Wind Speed Characteristic UUID value
 */
#define BT_UUID_APPARENT_WIND_SPEED_VAL 0x2a72
/**
 *  @brief Apparent Wind Speed Characteristic
 */
#define BT_UUID_APPARENT_WIND_SPEED \
    BT_UUID_DECLARE_16(BT_UUID_APPARENT_WIND_SPEED_VAL)
/**
 *  @brief Apparent Wind Direction Characteristic UUID value
 */
#define BT_UUID_APPARENT_WIND_DIR_VAL 0x2a73
/**
 *  @brief Apparent Wind Direction Characteristic
 */
#define BT_UUID_APPARENT_WIND_DIR \
    BT_UUID_DECLARE_16(BT_UUID_APPARENT_WIND_DIR_VAL)
/**
 *  @brief Gust Factor Characteristic UUID value
 */
#define BT_UUID_GUST_FACTOR_VAL 0x2a74
/**
 *  @brief Gust Factor Characteristic
 */
#define BT_UUID_GUST_FACTOR \
    BT_UUID_DECLARE_16(BT_UUID_GUST_FACTOR_VAL)
/**
 *  @brief Pollen Concentration Characteristic UUID value
 */
#define BT_UUID_POLLEN_CONCENTRATION_VAL 0x2a75
/**
 *  @brief Pollen Concentration Characteristic
 */
#define BT_UUID_POLLEN_CONCENTRATION \
    BT_UUID_DECLARE_16(BT_UUID_POLLEN_CONCENTRATION_VAL)
/**
 *  @brief UV Index Characteristic UUID value
 */
#define BT_UUID_UV_INDEX_VAL 0x2a76
/**
 *  @brief UV Index Characteristic
 */
#define BT_UUID_UV_INDEX \
    BT_UUID_DECLARE_16(BT_UUID_UV_INDEX_VAL)
/**
 *  @brief Irradiance Characteristic UUID value
 */
#define BT_UUID_IRRADIANCE_VAL 0x2a77
/**
 *  @brief Irradiance Characteristic
 */
#define BT_UUID_IRRADIANCE \
    BT_UUID_DECLARE_16(BT_UUID_IRRADIANCE_VAL)
/**
 *  @brief Rainfall Characteristic UUID value
 */
#define BT_UUID_RAINFALL_VAL 0x2a78
/**
 *  @brief Rainfall Characteristic
 */
#define BT_UUID_RAINFALL \
    BT_UUID_DECLARE_16(BT_UUID_RAINFALL_VAL)
/**
 *  @brief Wind Chill Characteristic UUID value
 */
#define BT_UUID_WIND_CHILL_VAL 0x2a79
/**
 *  @brief Wind Chill Characteristic
 */
#define BT_UUID_WIND_CHILL \
    BT_UUID_DECLARE_16(BT_UUID_WIND_CHILL_VAL)
/**
 *  @brief Heat Index Characteristic UUID value
 */
#define BT_UUID_HEAT_INDEX_VAL 0x2a7a
/**
 *  @brief Heat Index Characteristic
 */
#define BT_UUID_HEAT_INDEX \
    BT_UUID_DECLARE_16(BT_UUID_HEAT_INDEX_VAL)
/**
 *  @brief Dew Point Characteristic UUID value
 */
#define BT_UUID_DEW_POINT_VAL 0x2a7b
/**
 *  @brief Dew Point Characteristic
 */
#define BT_UUID_DEW_POINT \
    BT_UUID_DECLARE_16(BT_UUID_DEW_POINT_VAL)
/**
 *  @brief GATT Characteristic Trend UUID Value
 */
#define BT_UUID_GATT_TREND_VAL 0x2a7c
/**
 *  @brief GATT Characteristic Trend
 */
#define BT_UUID_GATT_TREND \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TREND_VAL)
/**
 *  @brief Descriptor Value Changed Characteristic UUID value
 */
#define BT_UUID_DESC_VALUE_CHANGED_VAL 0x2a7d
/**
 *  @brief Descriptor Value Changed Characteristic
 */
#define BT_UUID_DESC_VALUE_CHANGED \
    BT_UUID_DECLARE_16(BT_UUID_DESC_VALUE_CHANGED_VAL)
/**
 *  @brief GATT Characteristic Aerobic Heart Rate Low Limit UUID Value
 */
#define BT_UUID_GATT_AEHRLL_VAL 0x2a7e
/**
 *  @brief GATT Characteristic Aerobic Heart Rate Lower Limit
 */
#define BT_UUID_GATT_AEHRLL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AEHRLL_VAL)
/**
 *  @brief GATT Characteristic Aerobic Threshold UUID Value
 */
#define BT_UUID_GATT_AETHR_VAL 0x2a7f
/**
 *  @brief GATT Characteristic Aerobic Threshold
 */
#define BT_UUID_GATT_AETHR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AETHR_VAL)
/**
 *  @brief GATT Characteristic Age UUID Value
 */
#define BT_UUID_GATT_AGE_VAL 0x2a80
/**
 *  @brief GATT Characteristic Age
 */
#define BT_UUID_GATT_AGE \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AGE_VAL)
/**
 *  @brief GATT Characteristic Anaerobic Heart Rate Lower Limit UUID Value
 */
#define BT_UUID_GATT_ANHRLL_VAL 0x2a81
/**
 *  @brief GATT Characteristic Anaerobic Heart Rate Lower Limit
 */
#define BT_UUID_GATT_ANHRLL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ANHRLL_VAL)
/**
 *  @brief GATT Characteristic Anaerobic Heart Rate Upper Limit UUID Value
 */
#define BT_UUID_GATT_ANHRUL_VAL 0x2a82
/**
 *  @brief GATT Characteristic Anaerobic Heart Rate Upper Limit
 */
#define BT_UUID_GATT_ANHRUL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ANHRUL_VAL)
/**
 *  @brief GATT Characteristic Anaerobic Threshold UUID Value
 */
#define BT_UUID_GATT_ANTHR_VAL 0x2a83
/**
 *  @brief GATT Characteristic Anaerobic Threshold
 */
#define BT_UUID_GATT_ANTHR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ANTHR_VAL)
/**
 *  @brief GATT Characteristic Aerobic Heart Rate Upper Limit UUID Value
 */
#define BT_UUID_GATT_AEHRUL_VAL 0x2a84
/**
 *  @brief GATT Characteristic Aerobic Heart Rate Upper Limit
 */
#define BT_UUID_GATT_AEHRUL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AEHRUL_VAL)
/**
 *  @brief GATT Characteristic Date of Birth UUID Value
 */
#define BT_UUID_GATT_DATE_BIRTH_VAL 0x2a85
/**
 *  @brief GATT Characteristic Date of Birth
 */
#define BT_UUID_GATT_DATE_BIRTH \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DATE_BIRTH_VAL)
/**
 *  @brief GATT Characteristic Date of Threshold Assessment UUID Value
 */
#define BT_UUID_GATT_DATE_THRASS_VAL 0x2a86
/**
 *  @brief GATT Characteristic Date of Threshold Assessment
 */
#define BT_UUID_GATT_DATE_THRASS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DATE_THRASS_VAL)
/**
 *  @brief GATT Characteristic Email Address UUID Value
 */
#define BT_UUID_GATT_EMAIL_VAL 0x2a87
/**
 *  @brief GATT Characteristic Email Address
 */
#define BT_UUID_GATT_EMAIL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_EMAIL_VAL)
/**
 *  @brief GATT Characteristic Fat Burn Heart Rate Lower Limit UUID Value
 */
#define BT_UUID_GATT_FBHRLL_VAL 0x2a88
/**
 *  @brief GATT Characteristic Fat Burn Heart Rate Lower Limit
 */
#define BT_UUID_GATT_FBHRLL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FBHRLL_VAL)
/**
 *  @brief GATT Characteristic Fat Burn Heart Rate Upper Limit UUID Value
 */
#define BT_UUID_GATT_FBHRUL_VAL 0x2a89
/**
 *  @brief GATT Characteristic Fat Burn Heart Rate Upper Limit
 */
#define BT_UUID_GATT_FBHRUL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FBHRUL_VAL)
/**
 *  @brief GATT Characteristic First Name UUID Value
 */
#define BT_UUID_GATT_FIRST_NAME_VAL 0x2a8a
/**
 *  @brief GATT Characteristic First Name
 */
#define BT_UUID_GATT_FIRST_NAME \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FIRST_NAME_VAL)
/**
 *  @brief GATT Characteristic Five Zone Heart Rate Limits UUID Value
 */
#define BT_UUID_GATT_5ZHRL_VAL 0x2a8b
/**
 *  @brief GATT Characteristic Five Zone Heart Rate Limits
 */
#define BT_UUID_GATT_5ZHRL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_5ZHRL_VAL)
/**
 *  @brief GATT Characteristic Gender UUID Value
 */
#define BT_UUID_GATT_GENDER_VAL 0x2a8c
/**
 *  @brief GATT Characteristic Gender
 */
#define BT_UUID_GATT_GENDER \
    BT_UUID_DECLARE_16(BT_UUID_GATT_GENDER_VAL)
/**
 *  @brief GATT Characteristic Heart Rate Max UUID Value
 */
#define BT_UUID_GATT_HR_MAX_VAL 0x2a8d
/**
 *  @brief GATT Characteristic Heart Rate Max
 */
#define BT_UUID_GATT_HR_MAX \
    BT_UUID_DECLARE_16(BT_UUID_GATT_HR_MAX_VAL)
/**
 *  @brief GATT Characteristic Height UUID Value
 */
#define BT_UUID_GATT_HEIGHT_VAL 0x2a8e
/**
 *  @brief GATT Characteristic Height
 */
#define BT_UUID_GATT_HEIGHT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_HEIGHT_VAL)
/**
 *  @brief GATT Characteristic Hip Circumference UUID Value
 */
#define BT_UUID_GATT_HC_VAL 0x2a8f
/**
 *  @brief GATT Characteristic Hip Circumference
 */
#define BT_UUID_GATT_HC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_HC_VAL)
/**
 *  @brief GATT Characteristic Last Name UUID Value
 */
#define BT_UUID_GATT_LAST_NAME_VAL 0x2a90
/**
 *  @brief GATT Characteristic Last Name
 */
#define BT_UUID_GATT_LAST_NAME \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LAST_NAME_VAL)
/**
 *  @brief GATT Characteristic Maximum Recommended Heart Rate> UUID Value
 */
#define BT_UUID_GATT_MRHR_VAL 0x2a91
/**
 *  @brief GATT Characteristic Maximum Recommended Heart Rate
 */
#define BT_UUID_GATT_MRHR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_MRHR_VAL)
/**
 *  @brief GATT Characteristic Resting Heart Rate UUID Value
 */
#define BT_UUID_GATT_RHR_VAL 0x2a92
/**
 *  @brief GATT Characteristic Resting Heart Rate
 */
#define BT_UUID_GATT_RHR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RHR_VAL)
/**
 *  @brief GATT Characteristic Sport Type for Aerobic and Anaerobic Thresholds UUID Value
 */
#define BT_UUID_GATT_AEANTHR_VAL 0x2a93
/**
 *  @brief GATT Characteristic Sport Type for Aerobic and Anaerobic Threshold
 */
#define BT_UUID_GATT_AEANTHR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AEANTHR_VAL)
/**
 *  @brief GATT Characteristic Three Zone Heart Rate Limits UUID Value
 */
#define BT_UUID_GATT_3ZHRL_VAL 0x2a94
/**
 *  @brief GATT Characteristic Three Zone Heart Rate Limits
 */
#define BT_UUID_GATT_3ZHRL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_3ZHRL_VAL)
/**
 *  @brief GATT Characteristic Two Zone Heart Rate Limits UUID Value
 */
#define BT_UUID_GATT_2ZHRL_VAL 0x2a95
/**
 *  @brief GATT Characteristic Two Zone Heart Rate Limits
 */
#define BT_UUID_GATT_2ZHRL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_2ZHRL_VAL)
/**
 *  @brief GATT Characteristic VO2 Max UUID Value
 */
#define BT_UUID_GATT_VO2_MAX_VAL 0x2a96
/**
 *  @brief GATT Characteristic VO2 Max
 */
#define BT_UUID_GATT_VO2_MAX \
    BT_UUID_DECLARE_16(BT_UUID_GATT_VO2_MAX_VAL)
/**
 *  @brief GATT Characteristic Waist Circumference UUID Value
 */
#define BT_UUID_GATT_WC_VAL 0x2a97
/**
 *  @brief GATT Characteristic Waist Circumference
 */
#define BT_UUID_GATT_WC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_WC_VAL)
/**
 *  @brief GATT Characteristic Weight UUID Value
 */
#define BT_UUID_GATT_WEIGHT_VAL 0x2a98
/**
 *  @brief GATT Characteristic Weight
 */
#define BT_UUID_GATT_WEIGHT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_WEIGHT_VAL)
/**
 *  @brief GATT Characteristic Database Change Increment UUID Value
 */
#define BT_UUID_GATT_DBCHINC_VAL 0x2a99
/**
 *  @brief GATT Characteristic Database Change Increment
 */
#define BT_UUID_GATT_DBCHINC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DBCHINC_VAL)
/**
 *  @brief GATT Characteristic User Index UUID Value
 */
#define BT_UUID_GATT_USRIDX_VAL 0x2a9a
/**
 *  @brief GATT Characteristic User Index
 */
#define BT_UUID_GATT_USRIDX \
    BT_UUID_DECLARE_16(BT_UUID_GATT_USRIDX_VAL)
/**
 *  @brief GATT Characteristic Body Composition Feature UUID Value
 */
#define BT_UUID_GATT_BCF_VAL 0x2a9b
/**
 *  @brief GATT Characteristic Body Composition Feature
 */
#define BT_UUID_GATT_BCF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_BCF_VAL)
/**
 *  @brief GATT Characteristic Body Composition Measurement UUID Value
 */
#define BT_UUID_GATT_BCM_VAL 0x2a9c
/**
 *  @brief GATT Characteristic Body Composition Measurement
 */
#define BT_UUID_GATT_BCM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_BCM_VAL)
/**
 *  @brief GATT Characteristic Weight Measurement UUID Value
 */
#define BT_UUID_GATT_WM_VAL 0x2a9d
/**
 *  @brief GATT Characteristic Weight Measurement
 */
#define BT_UUID_GATT_WM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_WM_VAL)
/**
 *  @brief GATT Characteristic Weight Scale Feature UUID Value
 */
#define BT_UUID_GATT_WSF_VAL 0x2a9e
/**
 *  @brief GATT Characteristic Weight Scale Feature
 */
#define BT_UUID_GATT_WSF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_WSF_VAL)
/**
 *  @brief GATT Characteristic User Control Point UUID Value
 */
#define BT_UUID_GATT_USRCP_VAL 0x2a9f
/**
 *  @brief GATT Characteristic User Control Point
 */
#define BT_UUID_GATT_USRCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_USRCP_VAL)
/**
 *  @brief Magnetic Flux Density - 2D Characteristic UUID value
 */
#define BT_UUID_MAGN_FLUX_DENSITY_2D_VAL 0x2aa0
/**
 *  @brief Magnetic Flux Density - 2D Characteristic
 */
#define BT_UUID_MAGN_FLUX_DENSITY_2D \
    BT_UUID_DECLARE_16(BT_UUID_MAGN_FLUX_DENSITY_2D_VAL)
/**
 *  @brief Magnetic Flux Density - 3D Characteristic UUID value
 */
#define BT_UUID_MAGN_FLUX_DENSITY_3D_VAL 0x2aa1
/**
 *  @brief Magnetic Flux Density - 3D Characteristic
 */
#define BT_UUID_MAGN_FLUX_DENSITY_3D \
    BT_UUID_DECLARE_16(BT_UUID_MAGN_FLUX_DENSITY_3D_VAL)
/**
 *  @brief GATT Characteristic Language UUID Value
 */
#define BT_UUID_GATT_LANG_VAL 0x2aa2
/**
 *  @brief GATT Characteristic Language
 */
#define BT_UUID_GATT_LANG \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LANG_VAL)
/**
 *  @brief Barometric Pressure Trend Characteristic UUID value
 */
#define BT_UUID_BAR_PRESSURE_TREND_VAL 0x2aa3
/**
 *  @brief Barometric Pressure Trend Characteristic
 */
#define BT_UUID_BAR_PRESSURE_TREND \
    BT_UUID_DECLARE_16(BT_UUID_BAR_PRESSURE_TREND_VAL)
/**
 *  @brief Bond Management Control Point UUID value
 */
#define BT_UUID_BMS_CONTROL_POINT_VAL 0x2aa4
/**
 *  @brief Bond Management Control Point
 */
#define BT_UUID_BMS_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_BMS_CONTROL_POINT_VAL)
/**
 *  @brief Bond Management Feature UUID value
 */
#define BT_UUID_BMS_FEATURE_VAL 0x2aa5
/**
 *  @brief Bond Management Feature
 */
#define BT_UUID_BMS_FEATURE \
    BT_UUID_DECLARE_16(BT_UUID_BMS_FEATURE_VAL)
/**
 *  @brief Central Address Resolution Characteristic UUID value
 */
#define BT_UUID_CENTRAL_ADDR_RES_VAL 0x2aa6
/**
 *  @brief Central Address Resolution Characteristic
 */
#define BT_UUID_CENTRAL_ADDR_RES \
    BT_UUID_DECLARE_16(BT_UUID_CENTRAL_ADDR_RES_VAL)
/**
 *  @brief CGM Measurement Characteristic value
 */
#define BT_UUID_CGM_MEASUREMENT_VAL 0x2aa7
/**
 *  @brief CGM Measurement Characteristic
 */
#define BT_UUID_CGM_MEASUREMENT \
    BT_UUID_DECLARE_16(BT_UUID_CGM_MEASUREMENT_VAL)
/**
 *  @brief CGM Feature Characteristic value
 */
#define BT_UUID_CGM_FEATURE_VAL 0x2aa8
/**
 *  @brief CGM Feature Characteristic
 */
#define BT_UUID_CGM_FEATURE \
    BT_UUID_DECLARE_16(BT_UUID_CGM_FEATURE_VAL)
/**
 *  @brief CGM Status Characteristic value
 */
#define BT_UUID_CGM_STATUS_VAL 0x2aa9
/**
 *  @brief CGM Status Characteristic
 */
#define BT_UUID_CGM_STATUS \
    BT_UUID_DECLARE_16(BT_UUID_CGM_STATUS_VAL)
/**
 *  @brief CGM Session Start Time Characteristic value
 */
#define BT_UUID_CGM_SESSION_START_TIME_VAL 0x2aaa
/**
 *  @brief CGM Session Start Time
 */
#define BT_UUID_CGM_SESSION_START_TIME \
    BT_UUID_DECLARE_16(BT_UUID_CGM_SESSION_START_TIME_VAL)
/**
 *  @brief CGM Session Run Time Characteristic value
 */
#define BT_UUID_CGM_SESSION_RUN_TIME_VAL 0x2aab
/**
 *  @brief CGM Session Run Time
 */
#define BT_UUID_CGM_SESSION_RUN_TIME \
    BT_UUID_DECLARE_16(BT_UUID_CGM_SESSION_RUN_TIME_VAL)
/**
 *  @brief CGM Specific Ops Control Point Characteristic value
 */
#define BT_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT_VAL 0x2aac
/**
 *  @brief CGM Specific Ops Control Point
 */
#define BT_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT_VAL)
/**
 *  @brief GATT Characteristic Indoor Positioning Configuration UUID Value
 */
#define BT_UUID_GATT_IPC_VAL 0x2aad
/**
 *  @brief GATT Characteristic Indoor Positioning Configuration
 */
#define BT_UUID_GATT_IPC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IPC_VAL)
/**
 *  @brief GATT Characteristic Latitude UUID Value
 */
#define BT_UUID_GATT_LAT_VAL 0x2aae
/**
 *  @brief GATT Characteristic Latitude
 */
#define BT_UUID_GATT_LAT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LAT_VAL)
/**
 *  @brief GATT Characteristic Longitude UUID Value
 */
#define BT_UUID_GATT_LON_VAL 0x2aaf
/**
 *  @brief GATT Characteristic Longitude
 */
#define BT_UUID_GATT_LON \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LON_VAL)
/**
 *  @brief GATT Characteristic Local North Coordinate UUID Value
 */
#define BT_UUID_GATT_LNCOORD_VAL 0x2ab0
/**
 *  @brief GATT Characteristic Local North Coordinate
 */
#define BT_UUID_GATT_LNCOORD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LNCOORD_VAL)
/**
 *  @brief GATT Characteristic Local East Coordinate UUID Value
 */
#define BT_UUID_GATT_LECOORD_VAL 0x2ab1
/**
 *  @brief GATT Characteristic Local East Coordinate
 */
#define BT_UUID_GATT_LECOORD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LECOORD_VAL)
/**
 *  @brief GATT Characteristic Floor Number UUID Value
 */
#define BT_UUID_GATT_FN_VAL 0x2ab2
/**
 *  @brief GATT Characteristic Floor Number
 */
#define BT_UUID_GATT_FN \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FN_VAL)
/**
 *  @brief GATT Characteristic Altitude UUID Value
 */
#define BT_UUID_GATT_ALT_VAL 0x2ab3
/**
 *  @brief GATT Characteristic Altitude
 */
#define BT_UUID_GATT_ALT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ALT_VAL)
/**
 *  @brief GATT Characteristic Uncertainty UUID Value
 */
#define BT_UUID_GATT_UNCERTAINTY_VAL 0x2ab4
/**
 *  @brief GATT Characteristic Uncertainty
 */
#define BT_UUID_GATT_UNCERTAINTY \
    BT_UUID_DECLARE_16(BT_UUID_GATT_UNCERTAINTY_VAL)
/**
 *  @brief GATT Characteristic Location Name UUID Value
 */
#define BT_UUID_GATT_LOC_NAME_VAL 0x2ab5
/**
 *  @brief GATT Characteristic Location Name
 */
#define BT_UUID_GATT_LOC_NAME \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LOC_NAME_VAL)
/**
 *  @brief URI UUID value
 */
#define BT_UUID_URI_VAL 0x2ab6
/**
 *  @brief URI
 */
#define BT_UUID_URI \
    BT_UUID_DECLARE_16(BT_UUID_URI_VAL)
/**
 *  @brief HTTP Headers UUID value
 */
#define BT_UUID_HTTP_HEADERS_VAL 0x2ab7
/**
 *  @brief HTTP Headers
 */
#define BT_UUID_HTTP_HEADERS \
    BT_UUID_DECLARE_16(BT_UUID_HTTP_HEADERS_VAL)
/**
 *  @brief HTTP Status Code UUID value
 */
#define BT_UUID_HTTP_STATUS_CODE_VAL 0x2ab8
/**
 *  @brief HTTP Status Code
 */
#define BT_UUID_HTTP_STATUS_CODE \
    BT_UUID_DECLARE_16(BT_UUID_HTTP_STATUS_CODE_VAL)
/**
 *  @brief HTTP Entity Body UUID value
 */
#define BT_UUID_HTTP_ENTITY_BODY_VAL 0x2ab9
/**
 *  @brief HTTP Entity Body
 */
#define BT_UUID_HTTP_ENTITY_BODY \
    BT_UUID_DECLARE_16(BT_UUID_HTTP_ENTITY_BODY_VAL)
/**
 *  @brief HTTP Control Point UUID value
 */
#define BT_UUID_HTTP_CONTROL_POINT_VAL 0x2aba
/**
 *  @brief HTTP Control Point
 */
#define BT_UUID_HTTP_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_HTTP_CONTROL_POINT_VAL)
/**
 *  @brief HTTPS Security UUID value
 */
#define BT_UUID_HTTPS_SECURITY_VAL 0x2abb
/**
 *  @brief HTTPS Security
 */
#define BT_UUID_HTTPS_SECURITY \
    BT_UUID_DECLARE_16(BT_UUID_HTTPS_SECURITY_VAL)
/**
 *  @brief GATT Characteristic TDS Control Point UUID Value
 */
#define BT_UUID_GATT_TDS_CP_VAL 0x2abc
/**
 *  @brief GATT Characteristic TDS Control Point
 */
#define BT_UUID_GATT_TDS_CP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TDS_CP_VAL)
/**
 *  @brief OTS Feature Characteristic UUID value
 */
#define BT_UUID_OTS_FEATURE_VAL 0x2abd
/**
 *  @brief OTS Feature Characteristic
 */
#define BT_UUID_OTS_FEATURE \
    BT_UUID_DECLARE_16(BT_UUID_OTS_FEATURE_VAL)
/**
 *  @brief OTS Object Name Characteristic UUID value
 */
#define BT_UUID_OTS_NAME_VAL 0x2abe
/**
 *  @brief OTS Object Name Characteristic
 */
#define BT_UUID_OTS_NAME \
    BT_UUID_DECLARE_16(BT_UUID_OTS_NAME_VAL)
/**
 *  @brief OTS Object Type Characteristic UUID value
 */
#define BT_UUID_OTS_TYPE_VAL 0x2abf
/**
 *  @brief OTS Object Type Characteristic
 */
#define BT_UUID_OTS_TYPE \
    BT_UUID_DECLARE_16(BT_UUID_OTS_TYPE_VAL)
/**
 *  @brief OTS Object Size Characteristic UUID value
 */
#define BT_UUID_OTS_SIZE_VAL 0x2ac0
/**
 *  @brief OTS Object Size Characteristic
 */
#define BT_UUID_OTS_SIZE \
    BT_UUID_DECLARE_16(BT_UUID_OTS_SIZE_VAL)
/**
 *  @brief OTS Object First-Created Characteristic UUID value
 */
#define BT_UUID_OTS_FIRST_CREATED_VAL 0x2ac1
/**
 *  @brief OTS Object First-Created Characteristic
 */
#define BT_UUID_OTS_FIRST_CREATED \
    BT_UUID_DECLARE_16(BT_UUID_OTS_FIRST_CREATED_VAL)
/**
 *  @brief OTS Object Last-Modified Characteristic UUI value
 */
#define BT_UUID_OTS_LAST_MODIFIED_VAL 0x2ac2
/**
 *  @brief OTS Object Last-Modified Characteristic
 */
#define BT_UUID_OTS_LAST_MODIFIED \
    BT_UUID_DECLARE_16(BT_UUID_OTS_LAST_MODIFIED_VAL)
/**
 *  @brief OTS Object ID Characteristic UUID value
 */
#define BT_UUID_OTS_ID_VAL 0x2ac3
/**
 *  @brief OTS Object ID Characteristic
 */
#define BT_UUID_OTS_ID \
    BT_UUID_DECLARE_16(BT_UUID_OTS_ID_VAL)
/**
 *  @brief OTS Object Properties Characteristic UUID value
 */
#define BT_UUID_OTS_PROPERTIES_VAL 0x2ac4
/**
 *  @brief OTS Object Properties Characteristic
 */
#define BT_UUID_OTS_PROPERTIES \
    BT_UUID_DECLARE_16(BT_UUID_OTS_PROPERTIES_VAL)
/**
 *  @brief OTS Object Action Control Point Characteristic UUID value
 */
#define BT_UUID_OTS_ACTION_CP_VAL 0x2ac5
/**
 *  @brief OTS Object Action Control Point Characteristic
 */
#define BT_UUID_OTS_ACTION_CP \
    BT_UUID_DECLARE_16(BT_UUID_OTS_ACTION_CP_VAL)
/**
 *  @brief OTS Object List Control Point Characteristic UUID value
 */
#define BT_UUID_OTS_LIST_CP_VAL 0x2ac6
/**
 *  @brief OTS Object List Control Point Characteristic
 */
#define BT_UUID_OTS_LIST_CP \
    BT_UUID_DECLARE_16(BT_UUID_OTS_LIST_CP_VAL)
/**
 *  @brief OTS Object List Filter Characteristic UUID value
 */
#define BT_UUID_OTS_LIST_FILTER_VAL 0x2ac7
/**
 *  @brief OTS Object List Filter Characteristic
 */
#define BT_UUID_OTS_LIST_FILTER \
    BT_UUID_DECLARE_16(BT_UUID_OTS_LIST_FILTER_VAL)
/**
 *  @brief OTS Object Changed Characteristic UUID value
 */
#define BT_UUID_OTS_CHANGED_VAL 0x2ac8
/**
 *  @brief OTS Object Changed Characteristic
 */
#define BT_UUID_OTS_CHANGED \
    BT_UUID_DECLARE_16(BT_UUID_OTS_CHANGED_VAL)
/**
 *  @brief GATT Characteristic Resolvable Private Address Only UUID Value
 */
#define BT_UUID_GATT_RPAO_VAL 0x2ac9
/**
 *  @brief GATT Characteristic Resolvable Private Address Only
 */
#define BT_UUID_GATT_RPAO \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RPAO_VAL)
/**
 *  @brief OTS Unspecified Object Type UUID value
 */
#define BT_UUID_OTS_TYPE_UNSPECIFIED_VAL 0x2aca
/**
 *  @brief OTS Unspecified Object Type
 */
#define BT_UUID_OTS_TYPE_UNSPECIFIED \
    BT_UUID_DECLARE_16(BT_UUID_OTS_TYPE_UNSPECIFIED_VAL)
/**
 *  @brief OTS Directory Listing UUID value
 */
#define BT_UUID_OTS_DIRECTORY_LISTING_VAL 0x2acb
/**
 *  @brief OTS Directory Listing
 */
#define BT_UUID_OTS_DIRECTORY_LISTING \
    BT_UUID_DECLARE_16(BT_UUID_OTS_DIRECTORY_LISTING_VAL)
/**
 *  @brief GATT Characteristic Fitness Machine Feature UUID Value
 */
#define BT_UUID_GATT_FMF_VAL 0x2acc
/**
 *  @brief GATT Characteristic Fitness Machine Feature
 */
#define BT_UUID_GATT_FMF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FMF_VAL)
/**
 *  @brief GATT Characteristic Treadmill Data UUID Value
 */
#define BT_UUID_GATT_TD_VAL 0x2acd
/**
 *  @brief GATT Characteristic Treadmill Data
 */
#define BT_UUID_GATT_TD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TD_VAL)
/**
 *  @brief GATT Characteristic Cross Trainer Data UUID Value
 */
#define BT_UUID_GATT_CTD_VAL 0x2ace
/**
 *  @brief GATT Characteristic Cross Trainer Data
 */
#define BT_UUID_GATT_CTD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CTD_VAL)
/**
 *  @brief GATT Characteristic Step Climber Data UUID Value
 */
#define BT_UUID_GATT_STPCD_VAL 0x2acf
/**
 *  @brief GATT Characteristic Step Climber Data
 */
#define BT_UUID_GATT_STPCD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_STPCD_VAL)
/**
 *  @brief GATT Characteristic Stair Climber Data UUID Value
 */
#define BT_UUID_GATT_STRCD_VAL 0x2ad0
/**
 *  @brief GATT Characteristic Stair Climber Data
 */
#define BT_UUID_GATT_STRCD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_STRCD_VAL)
/**
 *  @brief GATT Characteristic Rower Data UUID Value
 */
#define BT_UUID_GATT_RD_VAL 0x2ad1
/**
 *  @brief GATT Characteristic Rower Data
 */
#define BT_UUID_GATT_RD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RD_VAL)
/**
 *  @brief GATT Characteristic Indoor Bike Data UUID Value
 */
#define BT_UUID_GATT_IBD_VAL 0x2ad2
/**
 *  @brief GATT Characteristic Indoor Bike Data
 */
#define BT_UUID_GATT_IBD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IBD_VAL)
/**
 *  @brief GATT Characteristic Training Status UUID Value
 */
#define BT_UUID_GATT_TRSTAT_VAL 0x2ad3
/**
 *  @brief GATT Characteristic Training Status
 */
#define BT_UUID_GATT_TRSTAT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TRSTAT_VAL)
/**
 *  @brief GATT Characteristic Supported Speed Range UUID Value
 */
#define BT_UUID_GATT_SSR_VAL 0x2ad4
/**
 *  @brief GATT Characteristic Supported Speed Range
 */
#define BT_UUID_GATT_SSR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SSR_VAL)
/**
 *  @brief GATT Characteristic Supported Inclination Range UUID Value
 */
#define BT_UUID_GATT_SIR_VAL 0x2ad5
/**
 *  @brief GATT Characteristic Supported Inclination Range
 */
#define BT_UUID_GATT_SIR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SIR_VAL)
/**
 *  @brief GATT Characteristic Supported Resistance Level Range UUID Value
 */
#define BT_UUID_GATT_SRLR_VAL 0x2ad6
/**
 *  @brief GATT Characteristic Supported Resistance Level Range
 */
#define BT_UUID_GATT_SRLR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SRLR_VAL)
/**
 *  @brief GATT Characteristic Supported Heart Rate Range UUID Value
 */
#define BT_UUID_GATT_SHRR_VAL 0x2ad7
/**
 *  @brief GATT Characteristic Supported Heart Rate Range
 */
#define BT_UUID_GATT_SHRR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SHRR_VAL)
/**
 *  @brief GATT Characteristic Supported Power Range UUID Value
 */
#define BT_UUID_GATT_SPR_VAL 0x2ad8
/**
 *  @brief GATT Characteristic Supported Power Range
 */
#define BT_UUID_GATT_SPR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SPR_VAL)
/**
 *  @brief GATT Characteristic Fitness Machine Control Point UUID Value
 */
#define BT_UUID_GATT_FMCP_VAL 0x2ad9
/**
 *  @brief GATT Characteristic Fitness Machine Control Point
 */
#define BT_UUID_GATT_FMCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FMCP_VAL)
/**
 *  @brief GATT Characteristic Fitness Machine Status UUID Value
 */
#define BT_UUID_GATT_FMS_VAL 0x2ada
/**
 *  @brief GATT Characteristic Fitness Machine Status
 */
#define BT_UUID_GATT_FMS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FMS_VAL)
/**
 *  @brief Mesh Provisioning Data In UUID value
 */
#define BT_UUID_MESH_PROV_DATA_IN_VAL 0x2adb
/**
 *  @brief Mesh Provisioning Data In
 */
#define BT_UUID_MESH_PROV_DATA_IN \
    BT_UUID_DECLARE_16(BT_UUID_MESH_PROV_DATA_IN_VAL)
/**
 *  @brief Mesh Provisioning Data Out UUID value
 */
#define BT_UUID_MESH_PROV_DATA_OUT_VAL 0x2adc
/**
 *  @brief Mesh Provisioning Data Out
 */
#define BT_UUID_MESH_PROV_DATA_OUT \
    BT_UUID_DECLARE_16(BT_UUID_MESH_PROV_DATA_OUT_VAL)
/**
 *  @brief Mesh Proxy Data In UUID value
 */
#define BT_UUID_MESH_PROXY_DATA_IN_VAL 0x2add
/**
 *  @brief Mesh Proxy Data In
 */
#define BT_UUID_MESH_PROXY_DATA_IN \
    BT_UUID_DECLARE_16(BT_UUID_MESH_PROXY_DATA_IN_VAL)
/**
 *  @brief Mesh Proxy Data Out UUID value
 */
#define BT_UUID_MESH_PROXY_DATA_OUT_VAL 0x2ade
/**
 *  @brief Mesh Proxy Data Out
 */
#define BT_UUID_MESH_PROXY_DATA_OUT \
    BT_UUID_DECLARE_16(BT_UUID_MESH_PROXY_DATA_OUT_VAL)
/**
 *  @brief GATT Characteristic New Number Needed UUID Value
 */
#define BT_UUID_GATT_NNN_VAL 0x2adf
/**
 *  @brief GATT Characteristic New Number Needed
 */
#define BT_UUID_GATT_NNN \
    BT_UUID_DECLARE_16(BT_UUID_GATT_NNN_VAL)
/**
 *  @brief GATT Characteristic Average Current UUID Value
 */
#define BT_UUID_GATT_AC_VAL 0x2ae0
/**
 *  @brief GATT Characteristic Average Current
 */
#define BT_UUID_GATT_AC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AC_VAL)
/**
 *  @brief GATT Characteristic Average Voltage UUID Value
 */
#define BT_UUID_GATT_AV_VAL 0x2ae1
/**
 *  @brief GATT Characteristic Average Voltage
 */
#define BT_UUID_GATT_AV \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AV_VAL)
/**
 *  @brief GATT Characteristic Boolean UUID Value
 */
#define BT_UUID_GATT_BOOLEAN_VAL 0x2ae2
/**
 *  @brief GATT Characteristic Boolean
 */
#define BT_UUID_GATT_BOOLEAN \
    BT_UUID_DECLARE_16(BT_UUID_GATT_BOOLEAN_VAL)
/**
 *  @brief GATT Characteristic Chromatic Distance From Planckian UUID Value
 */
#define BT_UUID_GATT_CRDFP_VAL 0x2ae3
/**
 *  @brief GATT Characteristic Chromatic Distance From Planckian
 */
#define BT_UUID_GATT_CRDFP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CRDFP_VAL)
/**
 *  @brief GATT Characteristic Chromaticity Coordinates UUID Value
 */
#define BT_UUID_GATT_CRCOORDS_VAL 0x2ae4
/**
 *  @brief GATT Characteristic Chromaticity Coordinates
 */
#define BT_UUID_GATT_CRCOORDS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CRCOORDS_VAL)
/**
 *  @brief GATT Characteristic Chromaticity In CCT And Duv Values UUID Value
 */
#define BT_UUID_GATT_CRCCT_VAL 0x2ae5
/**
 *  @brief GATT Characteristic Chromaticity In CCT And Duv Values
 */
#define BT_UUID_GATT_CRCCT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CRCCT_VAL)
/**
 *  @brief GATT Characteristic Chromaticity Tolerance UUID Value
 */
#define BT_UUID_GATT_CRT_VAL 0x2ae6
/**
 *  @brief GATT Characteristic Chromaticity Tolerance
 */
#define BT_UUID_GATT_CRT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CRT_VAL)
/**
 *  @brief GATT Characteristic CIE 13.3-1995 Color Rendering Index UUID Value
 */
#define BT_UUID_GATT_CIEIDX_VAL 0x2ae7
/**
 *  @brief GATT Characteristic CIE 13.3-1995 Color Rendering Index
 */
#define BT_UUID_GATT_CIEIDX \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CIEIDX_VAL)
/**
 *  @brief GATT Characteristic Coefficient UUID Value
 */
#define BT_UUID_GATT_COEFFICIENT_VAL 0x2ae8
/**
 *  @brief GATT Characteristic Coefficient
 */
#define BT_UUID_GATT_COEFFICIENT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_COEFFICIENT_VAL)
/**
 *  @brief GATT Characteristic Correlated Color Temperature UUID Value
 */
#define BT_UUID_GATT_CCTEMP_VAL 0x2ae9
/**
 *  @brief GATT Characteristic Correlated Color Temperature
 */
#define BT_UUID_GATT_CCTEMP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CCTEMP_VAL)
/**
 *  @brief GATT Characteristic Count 16 UUID Value
 */
#define BT_UUID_GATT_COUNT16_VAL 0x2aea
/**
 *  @brief GATT Characteristic Count 16
 */
#define BT_UUID_GATT_COUNT16 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_COUNT16_VAL)
/**
 *  @brief GATT Characteristic Count 24 UUID Value
 */
#define BT_UUID_GATT_COUNT24_VAL 0x2aeb
/**
 *  @brief GATT Characteristic Count 24
 */
#define BT_UUID_GATT_COUNT24 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_COUNT24_VAL)
/**
 *  @brief GATT Characteristic Country Code UUID Value
 */
#define BT_UUID_GATT_CNTRCODE_VAL 0x2aec
/**
 *  @brief GATT Characteristic Country Code
 */
#define BT_UUID_GATT_CNTRCODE \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CNTRCODE_VAL)
/**
 *  @brief GATT Characteristic Date UTC UUID Value
 */
#define BT_UUID_GATT_DATEUTC_VAL 0x2aed
/**
 *  @brief GATT Characteristic Date UTC
 */
#define BT_UUID_GATT_DATEUTC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DATEUTC_VAL)
/**
 *  @brief GATT Characteristic Electric Current UUID Value
 */
#define BT_UUID_GATT_EC_VAL 0x2aee
/**
 *  @brief GATT Characteristic Electric Current
 */
#define BT_UUID_GATT_EC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_EC_VAL)
/**
 *  @brief GATT Characteristic Electric Current Range UUID Value
 */
#define BT_UUID_GATT_ECR_VAL 0x2aef
/**
 *  @brief GATT Characteristic Electric Current Range
 */
#define BT_UUID_GATT_ECR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ECR_VAL)
/**
 *  @brief GATT Characteristic Electric Current Specification UUID Value
 */
#define BT_UUID_GATT_ECSPEC_VAL 0x2af0
/**
 *  @brief GATT Characteristic Electric Current Specification
 */
#define BT_UUID_GATT_ECSPEC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ECSPEC_VAL)
/**
 *  @brief GATT Characteristic Electric Current Statistics UUID Value
 */
#define BT_UUID_GATT_ECSTAT_VAL 0x2af1
/**
 *  @brief GATT Characteristic Electric Current Statistics
 */
#define BT_UUID_GATT_ECSTAT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ECSTAT_VAL)
/**
 *  @brief GATT Characteristic Energy UUID Value
 */
#define BT_UUID_GATT_ENERGY_VAL 0x2af2
/**
 *  @brief GATT Characteristic Energy
 */
#define BT_UUID_GATT_ENERGY \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ENERGY_VAL)
/**
 *  @brief GATT Characteristic Energy In A Period Of Day UUID Value
 */
#define BT_UUID_GATT_EPOD_VAL 0x2af3
/**
 *  @brief GATT Characteristic Energy In A Period Of Day
 */
#define BT_UUID_GATT_EPOD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_EPOD_VAL)
/**
 *  @brief GATT Characteristic Event Statistics UUID Value
 */
#define BT_UUID_GATT_EVTSTAT_VAL 0x2af4
/**
 *  @brief GATT Characteristic Event Statistics
 */
#define BT_UUID_GATT_EVTSTAT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_EVTSTAT_VAL)
/**
 *  @brief GATT Characteristic Fixed String 16 UUID Value
 */
#define BT_UUID_GATT_FSTR16_VAL 0x2af5
/**
 *  @brief GATT Characteristic Fixed String 16
 */
#define BT_UUID_GATT_FSTR16 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FSTR16_VAL)
/**
 *  @brief GATT Characteristic Fixed String 24 UUID Value
 */
#define BT_UUID_GATT_FSTR24_VAL 0x2af6
/**
 *  @brief GATT Characteristic Fixed String 24
 */
#define BT_UUID_GATT_FSTR24 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FSTR24_VAL)
/**
 *  @brief GATT Characteristic Fixed String 36 UUID Value
 */
#define BT_UUID_GATT_FSTR36_VAL 0x2af7
/**
 *  @brief GATT Characteristic Fixed String 36
 */
#define BT_UUID_GATT_FSTR36 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FSTR36_VAL)
/**
 *  @brief GATT Characteristic Fixed String 8 UUID Value
 */
#define BT_UUID_GATT_FSTR8_VAL 0x2af8
/**
 *  @brief GATT Characteristic Fixed String 8
 */
#define BT_UUID_GATT_FSTR8 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FSTR8_VAL)
/**
 *  @brief GATT Characteristic Generic Level UUID Value
 */
#define BT_UUID_GATT_GENLVL_VAL 0x2af9
/**
 *  @brief GATT Characteristic Generic Level
 */
#define BT_UUID_GATT_GENLVL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_GENLVL_VAL)
/**
 *  @brief GATT Characteristic Global Trade Item Number UUID Value
 */
#define BT_UUID_GATT_GTIN_VAL 0x2afa
/**
 *  @brief GATT Characteristic Global Trade Item Number
 */
#define BT_UUID_GATT_GTIN \
    BT_UUID_DECLARE_16(BT_UUID_GATT_GTIN_VAL)
/**
 *  @brief GATT Characteristic Illuminance UUID Value
 */
#define BT_UUID_GATT_ILLUM_VAL 0x2afb
/**
 *  @brief GATT Characteristic Illuminance
 */
#define BT_UUID_GATT_ILLUM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ILLUM_VAL)
/**
 *  @brief GATT Characteristic Luminous Efficacy UUID Value
 */
#define BT_UUID_GATT_LUMEFF_VAL 0x2afc
/**
 *  @brief GATT Characteristic Luminous Efficacy
 */
#define BT_UUID_GATT_LUMEFF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LUMEFF_VAL)
/**
 *  @brief GATT Characteristic Luminous Energy UUID Value
 */
#define BT_UUID_GATT_LUMNRG_VAL 0x2afd
/**
 *  @brief GATT Characteristic Luminous Energy
 */
#define BT_UUID_GATT_LUMNRG \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LUMNRG_VAL)
/**
 *  @brief GATT Characteristic Luminous Exposure UUID Value
 */
#define BT_UUID_GATT_LUMEXP_VAL 0x2afe
/**
 *  @brief GATT Characteristic Luminous Exposure
 */
#define BT_UUID_GATT_LUMEXP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LUMEXP_VAL)
/**
 *  @brief GATT Characteristic Luminous Flux UUID Value
 */
#define BT_UUID_GATT_LUMFLX_VAL 0x2aff
/**
 *  @brief GATT Characteristic Luminous Flux
 */
#define BT_UUID_GATT_LUMFLX \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LUMFLX_VAL)
/**
 *  @brief GATT Characteristic Luminous Flux Range UUID Value
 */
#define BT_UUID_GATT_LUMFLXR_VAL 0x2b00
/**
 *  @brief GATT Characteristic Luminous Flux Range
 */
#define BT_UUID_GATT_LUMFLXR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LUMFLXR_VAL)
/**
 *  @brief GATT Characteristic Luminous Intensity UUID Value
 */
#define BT_UUID_GATT_LUMINT_VAL 0x2b01
/**
 *  @brief GATT Characteristic Luminous Intensity
 */
#define BT_UUID_GATT_LUMINT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LUMINT_VAL)
/**
 *  @brief GATT Characteristic Mass Flow UUID Value
 */
#define BT_UUID_GATT_MASSFLOW_VAL 0x2b02
/**
 *  @brief GATT Characteristic Mass Flow
 */
#define BT_UUID_GATT_MASSFLOW \
    BT_UUID_DECLARE_16(BT_UUID_GATT_MASSFLOW_VAL)
/**
 *  @brief GATT Characteristic Perceived Lightness UUID Value
 */
#define BT_UUID_GATT_PERLGHT_VAL 0x2b03
/**
 *  @brief GATT Characteristic Perceived Lightness
 */
#define BT_UUID_GATT_PERLGHT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PERLGHT_VAL)
/**
 *  @brief GATT Characteristic Percentage 8 UUID Value
 */
#define BT_UUID_GATT_PER8_VAL 0x2b04
/**
 *  @brief GATT Characteristic Percentage 8
 */
#define BT_UUID_GATT_PER8 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PER8_VAL)
/**
 *  @brief GATT Characteristic Power UUID Value
 */
#define BT_UUID_GATT_PWR_VAL 0x2b05
/**
 *  @brief GATT Characteristic Power
 */
#define BT_UUID_GATT_PWR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PWR_VAL)
/**
 *  @brief GATT Characteristic Power Specification UUID Value
 */
#define BT_UUID_GATT_PWRSPEC_VAL 0x2b06
/**
 *  @brief GATT Characteristic Power Specification
 */
#define BT_UUID_GATT_PWRSPEC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PWRSPEC_VAL)
/**
 *  @brief GATT Characteristic Relative Runtime In A Current Range UUID Value
 */
#define BT_UUID_GATT_RRICR_VAL 0x2b07
/**
 *  @brief GATT Characteristic Relative Runtime In A Current Range
 */
#define BT_UUID_GATT_RRICR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RRICR_VAL)
/**
 *  @brief GATT Characteristic Relative Runtime In A Generic Level Range UUID Value
 */
#define BT_UUID_GATT_RRIGLR_VAL 0x2b08
/**
 *  @brief GATT Characteristic Relative Runtime In A Generic Level Range
 */
#define BT_UUID_GATT_RRIGLR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RRIGLR_VAL)
/**
 *  @brief GATT Characteristic Relative Value In A Voltage Range UUID Value
 */
#define BT_UUID_GATT_RVIVR_VAL 0x2b09
/**
 *  @brief GATT Characteristic Relative Value In A Voltage Range
 */
#define BT_UUID_GATT_RVIVR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RVIVR_VAL)
/**
 *  @brief GATT Characteristic Relative Value In A Illuminance Range UUID Value
 */
#define BT_UUID_GATT_RVIIR_VAL 0x2b0a
/**
 *  @brief GATT Characteristic Relative Value In A Illuminance Range
 */
#define BT_UUID_GATT_RVIIR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RVIIR_VAL)
/**
 *  @brief GATT Characteristic Relative Value In A Period Of Day UUID Value
 */
#define BT_UUID_GATT_RVIPOD_VAL 0x2b0b
/**
 *  @brief GATT Characteristic Relative Value In A Period Of Day
 */
#define BT_UUID_GATT_RVIPOD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RVIPOD_VAL)
/**
 *  @brief GATT Characteristic Relative Value In A Temperature Range UUID Value
 */
#define BT_UUID_GATT_RVITR_VAL 0x2b0c
/**
 *  @brief GATT Characteristic Relative Value In A Temperature Range
 */
#define BT_UUID_GATT_RVITR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RVITR_VAL)
/**
 *  @brief GATT Characteristic Temperature 8 UUID Value
 */
#define BT_UUID_GATT_TEMP8_VAL 0x2b0d
/**
 *  @brief GATT Characteristic Temperature 8
 */
#define BT_UUID_GATT_TEMP8 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TEMP8_VAL)
/**
 *  @brief GATT Characteristic Temperature 8 In A Period Of Day UUID Value
 */
#define BT_UUID_GATT_TEMP8_IPOD_VAL 0x2b0e
/**
 *  @brief GATT Characteristic Temperature 8 In A Period Of Day
 */
#define BT_UUID_GATT_TEMP8_IPOD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TEMP8_IPOD_VAL)
/**
 *  @brief GATT Characteristic Temperature 8 Statistics UUID Value
 */
#define BT_UUID_GATT_TEMP8_STAT_VAL 0x2b0f
/**
 *  @brief GATT Characteristic Temperature 8 Statistics
 */
#define BT_UUID_GATT_TEMP8_STAT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TEMP8_STAT_VAL)
/**
 *  @brief GATT Characteristic Temperature Range UUID Value
 */
#define BT_UUID_GATT_TEMP_RNG_VAL 0x2b10
/**
 *  @brief GATT Characteristic Temperature Range
 */
#define BT_UUID_GATT_TEMP_RNG \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TEMP_RNG_VAL)
/**
 *  @brief GATT Characteristic Temperature Statistics UUID Value
 */
#define BT_UUID_GATT_TEMP_STAT_VAL 0x2b11
/**
 *  @brief GATT Characteristic Temperature Statistics
 */
#define BT_UUID_GATT_TEMP_STAT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TEMP_STAT_VAL)
/**
 *  @brief GATT Characteristic Time Decihour 8 UUID Value
 */
#define BT_UUID_GATT_TIM_DC8_VAL 0x2b12
/**
 *  @brief GATT Characteristic Time Decihour 8
 */
#define BT_UUID_GATT_TIM_DC8 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TIM_DC8_VAL)
/**
 *  @brief GATT Characteristic Time Exponential 8 UUID Value
 */
#define BT_UUID_GATT_TIM_EXP8_VAL 0x2b13
/**
 *  @brief GATT Characteristic Time Exponential 8
 */
#define BT_UUID_GATT_TIM_EXP8 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TIM_EXP8_VAL)
/**
 *  @brief GATT Characteristic Time Hour 24 UUID Value
 */
#define BT_UUID_GATT_TIM_H24_VAL 0x2b14
/**
 *  @brief GATT Characteristic Time Hour 24
 */
#define BT_UUID_GATT_TIM_H24 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TIM_H24_VAL)
/**
 *  @brief GATT Characteristic Time Millisecond 24 UUID Value
 */
#define BT_UUID_GATT_TIM_MS24_VAL 0x2b15
/**
 *  @brief GATT Characteristic Time Millisecond 24
 */
#define BT_UUID_GATT_TIM_MS24 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TIM_MS24_VAL)
/**
 *  @brief GATT Characteristic Time Second 16 UUID Value
 */
#define BT_UUID_GATT_TIM_S16_VAL 0x2b16
/**
 *  @brief GATT Characteristic Time Second 16
 */
#define BT_UUID_GATT_TIM_S16 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TIM_S16_VAL)
/**
 *  @brief GATT Characteristic Time Second 8 UUID Value
 */
#define BT_UUID_GATT_TIM_S8_VAL 0x2b17
/**
 *  @brief GATT Characteristic Time Second 8
 */
#define BT_UUID_GATT_TIM_S8 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TIM_S8_VAL)
/**
 *  @brief GATT Characteristic Voltage UUID Value
 */
#define BT_UUID_GATT_V_VAL 0x2b18
/**
 *  @brief GATT Characteristic Voltage
 */
#define BT_UUID_GATT_V \
    BT_UUID_DECLARE_16(BT_UUID_GATT_V_VAL)
/**
 *  @brief GATT Characteristic Voltage Specification UUID Value
 */
#define BT_UUID_GATT_V_SPEC_VAL 0x2b19
/**
 *  @brief GATT Characteristic Voltage Specification
 */
#define BT_UUID_GATT_V_SPEC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_V_SPEC_VAL)
/**
 *  @brief GATT Characteristic Voltage Statistics UUID Value
 */
#define BT_UUID_GATT_V_STAT_VAL 0x2b1a
/**
 *  @brief GATT Characteristic Voltage Statistics
 */
#define BT_UUID_GATT_V_STAT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_V_STAT_VAL)
/**
 *  @brief GATT Characteristic Volume Flow UUID Value
 */
#define BT_UUID_GATT_VOLF_VAL 0x2b1b
/**
 *  @brief GATT Characteristic Volume Flow
 */
#define BT_UUID_GATT_VOLF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_VOLF_VAL)
/**
 *  @brief GATT Characteristic Chromaticity Coordinate (not Coordinates) UUID Value
 */
#define BT_UUID_GATT_CRCOORD_VAL 0x2b1c
/**
 *  @brief GATT Characteristic Chromaticity Coordinate (not Coordinates)
 */
#define BT_UUID_GATT_CRCOORD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CRCOORD_VAL)
/**
 *  @brief GATT Characteristic RC Feature UUID Value
 */
#define BT_UUID_GATT_RCF_VAL 0x2b1d
/**
 *  @brief GATT Characteristic RC Feature
 */
#define BT_UUID_GATT_RCF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RCF_VAL)
/**
 *  @brief GATT Characteristic RC Settings UUID Value
 */
#define BT_UUID_GATT_RCSET_VAL 0x2b1e
/**
 *  @brief GATT Characteristic RC Settings
 */
#define BT_UUID_GATT_RCSET \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RCSET_VAL)
/**
 *  @brief GATT Characteristic Reconnection Configuration Control Point UUID Value
 */
#define BT_UUID_GATT_RCCP_VAL 0x2b1f
/**
 *  @brief GATT Characteristic Reconnection Configuration Control Point
 */
#define BT_UUID_GATT_RCCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RCCP_VAL)
/**
 *  @brief GATT Characteristic IDD Status Changed UUID Value
 */
#define BT_UUID_GATT_IDD_SC_VAL 0x2b20
/**
 *  @brief GATT Characteristic IDD Status Changed
 */
#define BT_UUID_GATT_IDD_SC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IDD_SC_VAL)
/**
 *  @brief GATT Characteristic IDD Status UUID Value
 */
#define BT_UUID_GATT_IDD_S_VAL 0x2b21
/**
 *  @brief GATT Characteristic IDD Status
 */
#define BT_UUID_GATT_IDD_S \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IDD_S_VAL)
/**
 *  @brief GATT Characteristic IDD Annunciation Status UUID Value
 */
#define BT_UUID_GATT_IDD_AS_VAL 0x2b22
/**
 *  @brief GATT Characteristic IDD Annunciation Status
 */
#define BT_UUID_GATT_IDD_AS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IDD_AS_VAL)
/**
 *  @brief GATT Characteristic IDD Features UUID Value
 */
#define BT_UUID_GATT_IDD_F_VAL 0x2b23
/**
 *  @brief GATT Characteristic IDD Features
 */
#define BT_UUID_GATT_IDD_F \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IDD_F_VAL)
/**
 *  @brief GATT Characteristic IDD Status Reader Control Point UUID Value
 */
#define BT_UUID_GATT_IDD_SRCP_VAL 0x2b24
/**
 *  @brief GATT Characteristic IDD Status Reader Control Point
 */
#define BT_UUID_GATT_IDD_SRCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IDD_SRCP_VAL)
/**
 *  @brief GATT Characteristic IDD Command Control Point UUID Value
 */
#define BT_UUID_GATT_IDD_CCP_VAL 0x2b25
/**
 *  @brief GATT Characteristic IDD Command Control Point
 */
#define BT_UUID_GATT_IDD_CCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IDD_CCP_VAL)
/**
 *  @brief GATT Characteristic IDD Command Data UUID Value
 */
#define BT_UUID_GATT_IDD_CD_VAL 0x2b26
/**
 *  @brief GATT Characteristic IDD Command Data
 */
#define BT_UUID_GATT_IDD_CD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IDD_CD_VAL)
/**
 *  @brief GATT Characteristic IDD Record Access Control Point UUID Value
 */
#define BT_UUID_GATT_IDD_RACP_VAL 0x2b27
/**
 *  @brief GATT Characteristic IDD Record Access Control Point
 */
#define BT_UUID_GATT_IDD_RACP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IDD_RACP_VAL)
/**
 *  @brief GATT Characteristic IDD History Data UUID Value
 */
#define BT_UUID_GATT_IDD_HD_VAL 0x2b28
/**
 *  @brief GATT Characteristic IDD History Data
 */
#define BT_UUID_GATT_IDD_HD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_IDD_HD_VAL)
/**
 *  @brief GATT Characteristic Client Supported Features UUID value
 */
#define BT_UUID_GATT_CLIENT_FEATURES_VAL 0x2b29
/**
 *  @brief GATT Characteristic Client Supported Features
 */
#define BT_UUID_GATT_CLIENT_FEATURES \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CLIENT_FEATURES_VAL)
/**
 *  @brief GATT Characteristic Database Hash UUID value
 */
#define BT_UUID_GATT_DB_HASH_VAL 0x2b2a
/**
 *  @brief GATT Characteristic Database Hash
 */
#define BT_UUID_GATT_DB_HASH \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DB_HASH_VAL)
/**
 *  @brief GATT Characteristic BSS Control Point UUID Value
 */
#define BT_UUID_GATT_BSS_CP_VAL 0x2b2b
/**
 *  @brief GATT Characteristic BSS Control Point
 */
#define BT_UUID_GATT_BSS_CP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_BSS_CP_VAL)
/**
 *  @brief GATT Characteristic BSS Response UUID Value
 */
#define BT_UUID_GATT_BSS_R_VAL 0x2b2c
/**
 *  @brief GATT Characteristic BSS Response
 */
#define BT_UUID_GATT_BSS_R \
    BT_UUID_DECLARE_16(BT_UUID_GATT_BSS_R_VAL)
/**
 *  @brief GATT Characteristic Emergency ID UUID Value
 */
#define BT_UUID_GATT_EMG_ID_VAL 0x2b2d
/**
 *  @brief GATT Characteristic Emergency ID
 */
#define BT_UUID_GATT_EMG_ID \
    BT_UUID_DECLARE_16(BT_UUID_GATT_EMG_ID_VAL)
/**
 *  @brief GATT Characteristic Emergency Text UUID Value
 */
#define BT_UUID_GATT_EMG_TXT_VAL 0x2b2e
/**
 *  @brief GATT Characteristic Emergency Text
 */
#define BT_UUID_GATT_EMG_TXT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_EMG_TXT_VAL)
/**
 *  @brief GATT Characteristic ACS Status UUID Value
 */
#define BT_UUID_GATT_ACS_S_VAL 0x2b2f
/**
 *  @brief GATT Characteristic ACS Status
 */
#define BT_UUID_GATT_ACS_S \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACS_S_VAL)
/**
 *  @brief GATT Characteristic ACS Data In UUID Value
 */
#define BT_UUID_GATT_ACS_DI_VAL 0x2b30
/**
 *  @brief GATT Characteristic ACS Data In
 */
#define BT_UUID_GATT_ACS_DI \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACS_DI_VAL)
/**
 *  @brief GATT Characteristic ACS Data Out Notify UUID Value
 */
#define BT_UUID_GATT_ACS_DON_VAL 0x2b31
/**
 *  @brief GATT Characteristic ACS Data Out Notify
 */
#define BT_UUID_GATT_ACS_DON \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACS_DON_VAL)
/**
 *  @brief GATT Characteristic ACS Data Out Indicate UUID Value
 */
#define BT_UUID_GATT_ACS_DOI_VAL 0x2b32
/**
 *  @brief GATT Characteristic ACS Data Out Indicate
 */
#define BT_UUID_GATT_ACS_DOI \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACS_DOI_VAL)
/**
 *  @brief GATT Characteristic ACS Control Point UUID Value
 */
#define BT_UUID_GATT_ACS_CP_VAL 0x2b33
/**
 *  @brief GATT Characteristic ACS Control Point
 */
#define BT_UUID_GATT_ACS_CP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACS_CP_VAL)
/**
 *  @brief GATT Characteristic Enhanced Blood Pressure Measurement UUID Value
 */
#define BT_UUID_GATT_EBPM_VAL 0x2b34
/**
 *  @brief GATT Characteristic Enhanced Blood Pressure Measurement
 */
#define BT_UUID_GATT_EBPM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_EBPM_VAL)
/**
 *  @brief GATT Characteristic Enhanced Intermediate Cuff Pressure UUID Value
 */
#define BT_UUID_GATT_EICP_VAL 0x2b35
/**
 *  @brief GATT Characteristic Enhanced Intermediate Cuff Pressure
 */
#define BT_UUID_GATT_EICP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_EICP_VAL)
/**
 *  @brief GATT Characteristic Blood Pressure Record UUID Value
 */
#define BT_UUID_GATT_BPR_VAL 0x2b36
/**
 *  @brief GATT Characteristic Blood Pressure Record
 */
#define BT_UUID_GATT_BPR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_BPR_VAL)
/**
 *  @brief GATT Characteristic Registered User UUID Value
 */
#define BT_UUID_GATT_RU_VAL 0x2b37
/**
 *  @brief GATT Characteristic Registered User
 */
#define BT_UUID_GATT_RU \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RU_VAL)
/**
 *  @brief GATT Characteristic BR-EDR Handover Data UUID Value
 */
#define BT_UUID_GATT_BR_EDR_HD_VAL 0x2b38
/**
 *  @brief GATT Characteristic BR-EDR Handover Data
 */
#define BT_UUID_GATT_BR_EDR_HD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_BR_EDR_HD_VAL)
/**
 *  @brief GATT Characteristic Bluetooth SIG Data UUID Value
 */
#define BT_UUID_GATT_BT_SIG_D_VAL 0x2b39
/**
 *  @brief GATT Characteristic Bluetooth SIG Data
 */
#define BT_UUID_GATT_BT_SIG_D \
    BT_UUID_DECLARE_16(BT_UUID_GATT_BT_SIG_D_VAL)
/**
 *  @brief GATT Characteristic Server Supported Features UUID value
 */
#define BT_UUID_GATT_SERVER_FEATURES_VAL  0x2b3a
/**
 *  @brief GATT Characteristic Server Supported Features
 */
#define BT_UUID_GATT_SERVER_FEATURES      \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SERVER_FEATURES_VAL)
/**
 *  @brief GATT Characteristic Physical Activity Monitor Features UUID Value
 */
#define BT_UUID_GATT_PHY_AMF_VAL 0x2b3b
/**
 *  @brief GATT Characteristic Physical Activity Monitor Features
 */
#define BT_UUID_GATT_PHY_AMF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PHY_AMF_VAL)
/**
 *  @brief GATT Characteristic General Activity Instantaneous Data UUID Value
 */
#define BT_UUID_GATT_GEN_AID_VAL 0x2b3c
/**
 *  @brief GATT Characteristic General Activity Instantaneous Data
 */
#define BT_UUID_GATT_GEN_AID \
    BT_UUID_DECLARE_16(BT_UUID_GATT_GEN_AID_VAL)
/**
 *  @brief GATT Characteristic General Activity Summary Data UUID Value
 */
#define BT_UUID_GATT_GEN_ASD_VAL 0x2b3d
/**
 *  @brief GATT Characteristic General Activity Summary Data
 */
#define BT_UUID_GATT_GEN_ASD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_GEN_ASD_VAL)
/**
 *  @brief GATT Characteristic CardioRespiratory Activity Instantaneous Data UUID Value
 */
#define BT_UUID_GATT_CR_AID_VAL 0x2b3e
/**
 *  @brief GATT Characteristic CardioRespiratory Activity Instantaneous Data
 */
#define BT_UUID_GATT_CR_AID \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CR_AID_VAL)
/**
 *  @brief GATT Characteristic CardioRespiratory Activity Summary Data UUID Value
 */
#define BT_UUID_GATT_CR_ASD_VAL 0x2b3f
/**
 *  @brief GATT Characteristic CardioRespiratory Activity Summary Data
 */
#define BT_UUID_GATT_CR_ASD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CR_ASD_VAL)
/**
 *  @brief GATT Characteristic Step Counter Activity Summary Data UUID Value
 */
#define BT_UUID_GATT_SC_ASD_VAL 0x2b40
/**
 *  @brief GATT Characteristic Step Counter Activity Summary Data
 */
#define BT_UUID_GATT_SC_ASD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SC_ASD_VAL)
/**
 *  @brief GATT Characteristic Sleep Activity Instantaneous Data UUID Value
 */
#define BT_UUID_GATT_SLP_AID_VAL 0x2b41
/**
 *  @brief GATT Characteristic Sleep Activity Instantaneous Data
 */
#define BT_UUID_GATT_SLP_AID \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SLP_AID_VAL)
/**
 *  @brief GATT Characteristic Sleep Activity Summary Data UUID Value
 */
#define BT_UUID_GATT_SLP_ASD_VAL 0x2b42
/**
 *  @brief GATT Characteristic Sleep Activity Summary Data
 */
#define BT_UUID_GATT_SLP_ASD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SLP_ASD_VAL)
/**
 *  @brief GATT Characteristic Physical Activity Monitor Control Point UUID Value
 */
#define BT_UUID_GATT_PHY_AMCP_VAL 0x2b43
/**
 *  @brief GATT Characteristic Physical Activity Monitor Control Point
 */
#define BT_UUID_GATT_PHY_AMCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PHY_AMCP_VAL)
/**
 *  @brief GATT Characteristic Activity Current Session UUID Value
 */
#define BT_UUID_GATT_ACS_VAL 0x2b44
/**
 *  @brief GATT Characteristic Activity Current Session
 */
#define BT_UUID_GATT_ACS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACS_VAL)
/**
 *  @brief GATT Characteristic Physical Activity Session Descriptor UUID Value
 */
#define BT_UUID_GATT_PHY_ASDESC_VAL 0x2b45
/**
 *  @brief GATT Characteristic Physical Activity Session Descriptor
 */
#define BT_UUID_GATT_PHY_ASDESC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PHY_ASDESC_VAL)
/**
 *  @brief GATT Characteristic Preferred Units UUID Value
 */
#define BT_UUID_GATT_PREF_U_VAL 0x2b46
/**
 *  @brief GATT Characteristic Preferred Units
 */
#define BT_UUID_GATT_PREF_U \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PREF_U_VAL)
/**
 *  @brief GATT Characteristic High Resolution Height UUID Value
 */
#define BT_UUID_GATT_HRES_H_VAL 0x2b47
/**
 *  @brief GATT Characteristic High Resolution Height
 */
#define BT_UUID_GATT_HRES_H \
    BT_UUID_DECLARE_16(BT_UUID_GATT_HRES_H_VAL)
/**
 *  @brief GATT Characteristic Middle Name UUID Value
 */
#define BT_UUID_GATT_MID_NAME_VAL 0x2b48
/**
 *  @brief GATT Characteristic Middle Name
 */
#define BT_UUID_GATT_MID_NAME \
    BT_UUID_DECLARE_16(BT_UUID_GATT_MID_NAME_VAL)
/**
 *  @brief GATT Characteristic Stride Length UUID Value
 */
#define BT_UUID_GATT_STRDLEN_VAL 0x2b49
/**
 *  @brief GATT Characteristic Stride Length
 */
#define BT_UUID_GATT_STRDLEN \
    BT_UUID_DECLARE_16(BT_UUID_GATT_STRDLEN_VAL)
/**
 *  @brief GATT Characteristic Handedness UUID Value
 */
#define BT_UUID_GATT_HANDEDNESS_VAL 0x2b4a
/**
 *  @brief GATT Characteristic Handedness
 */
#define BT_UUID_GATT_HANDEDNESS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_HANDEDNESS_VAL)
/**
 *  @brief GATT Characteristic Device Wearing Position UUID Value
 */
#define BT_UUID_GATT_DEVICE_WP_VAL 0x2b4b
/**
 *  @brief GATT Characteristic Device Wearing Position
 */
#define BT_UUID_GATT_DEVICE_WP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DEVICE_WP_VAL)
/**
 *  @brief GATT Characteristic Four Zone Heart Rate Limit UUID Value
 */
#define BT_UUID_GATT_4ZHRL_VAL 0x2b4c
/**
 *  @brief GATT Characteristic Four Zone Heart Rate Limit
 */
#define BT_UUID_GATT_4ZHRL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_4ZHRL_VAL)
/**
 *  @brief GATT Characteristic High Intensity Exercise Threshold UUID Value
 */
#define BT_UUID_GATT_HIET_VAL 0x2b4d
/**
 *  @brief GATT Characteristic High Intensity Exercise Threshold
 */
#define BT_UUID_GATT_HIET \
    BT_UUID_DECLARE_16(BT_UUID_GATT_HIET_VAL)
/**
 *  @brief GATT Characteristic Activity Goal UUID Value
 */
#define BT_UUID_GATT_AG_VAL 0x2b4e
/**
 *  @brief GATT Characteristic Activity Goal
 */
#define BT_UUID_GATT_AG \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AG_VAL)
/**
 *  @brief GATT Characteristic Sedentary Interval Notification UUID Value
 */
#define BT_UUID_GATT_SIN_VAL 0x2b4f
/**
 *  @brief GATT Characteristic Sedentary Interval Notification
 */
#define BT_UUID_GATT_SIN \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SIN_VAL)
/**
 *  @brief GATT Characteristic Caloric Intake UUID Value
 */
#define BT_UUID_GATT_CI_VAL 0x2b50
/**
 *  @brief GATT Characteristic Caloric Intake
 */
#define BT_UUID_GATT_CI \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CI_VAL)
/**
 *  @brief GATT Characteristic TMAP Role UUID Value
 */
#define BT_UUID_GATT_TMAPR_VAL 0x2b51
/**
 *  @brief GATT Characteristic TMAP Role
 */
#define BT_UUID_GATT_TMAPR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TMAPR_VAL)
/**
 *  @brief Audio Input Control Service State value
 */
#define BT_UUID_AICS_STATE_VAL 0x2b77
/**
 *  @brief Audio Input Control Service State
 */
#define BT_UUID_AICS_STATE \
    BT_UUID_DECLARE_16(BT_UUID_AICS_STATE_VAL)
/**
 *  @brief Audio Input Control Service Gain Settings Properties value
 */
#define BT_UUID_AICS_GAIN_SETTINGS_VAL 0x2b78
/**
 *  @brief Audio Input Control Service Gain Settings Properties
 */
#define BT_UUID_AICS_GAIN_SETTINGS \
    BT_UUID_DECLARE_16(BT_UUID_AICS_GAIN_SETTINGS_VAL)
/**
 *  @brief Audio Input Control Service Input Type value
 */
#define BT_UUID_AICS_INPUT_TYPE_VAL 0x2b79
/**
 *  @brief Audio Input Control Service Input Type
 */
#define BT_UUID_AICS_INPUT_TYPE \
    BT_UUID_DECLARE_16(BT_UUID_AICS_INPUT_TYPE_VAL)
/**
 *  @brief Audio Input Control Service Input Status value
 */
#define BT_UUID_AICS_INPUT_STATUS_VAL 0x2b7a
/**
 *  @brief Audio Input Control Service Input Status
 */
#define BT_UUID_AICS_INPUT_STATUS \
    BT_UUID_DECLARE_16(BT_UUID_AICS_INPUT_STATUS_VAL)
/**
 *  @brief Audio Input Control Service Control Point value
 */
#define BT_UUID_AICS_CONTROL_VAL 0x2b7b
/**
 *  @brief Audio Input Control Service Control Point
 */
#define BT_UUID_AICS_CONTROL \
    BT_UUID_DECLARE_16(BT_UUID_AICS_CONTROL_VAL)
/**
 *  @brief Audio Input Control Service Input Description value
 */
#define BT_UUID_AICS_DESCRIPTION_VAL 0x2b7c
/**
 *  @brief Audio Input Control Service Input Description
 */
#define BT_UUID_AICS_DESCRIPTION \
    BT_UUID_DECLARE_16(BT_UUID_AICS_DESCRIPTION_VAL)
/**
 *  @brief Volume Control Setting value
 */
#define BT_UUID_VCS_STATE_VAL 0x2b7d
/**
 *  @brief Volume Control Setting
 */
#define BT_UUID_VCS_STATE \
    BT_UUID_DECLARE_16(BT_UUID_VCS_STATE_VAL)
/**
 *  @brief Volume Control Control point value
 */
#define BT_UUID_VCS_CONTROL_VAL 0x2b7e
/**
 *  @brief Volume Control Control point
 */
#define BT_UUID_VCS_CONTROL \
    BT_UUID_DECLARE_16(BT_UUID_VCS_CONTROL_VAL)
/**
 *  @brief Volume Control Flags value
 */
#define BT_UUID_VCS_FLAGS_VAL 0x2b7f
/**
 *  @brief Volume Control Flags
 */
#define BT_UUID_VCS_FLAGS \
    BT_UUID_DECLARE_16(BT_UUID_VCS_FLAGS_VAL)
/**
 *  @brief Volume Offset State value
 */
#define BT_UUID_VOCS_STATE_VAL 0x2b80
/**
 *  @brief Volume Offset State
 */
#define BT_UUID_VOCS_STATE \
    BT_UUID_DECLARE_16(BT_UUID_VOCS_STATE_VAL)
/**
 *  @brief Audio Location value
 */
#define BT_UUID_VOCS_LOCATION_VAL 0x2b81
/**
 *  @brief Audio Location
 */
#define BT_UUID_VOCS_LOCATION \
    BT_UUID_DECLARE_16(BT_UUID_VOCS_LOCATION_VAL)
/**
 *  @brief Volume Offset Control Point value
 */
#define BT_UUID_VOCS_CONTROL_VAL 0x2b82
/**
 *  @brief Volume Offset Control Point
 */
#define BT_UUID_VOCS_CONTROL \
    BT_UUID_DECLARE_16(BT_UUID_VOCS_CONTROL_VAL)
/**
 *  @brief Volume Offset Audio Output Description value
 */
#define BT_UUID_VOCS_DESCRIPTION_VAL 0x2b83
/**
 *  @brief Volume Offset Audio Output Description
 */
#define BT_UUID_VOCS_DESCRIPTION \
    BT_UUID_DECLARE_16(BT_UUID_VOCS_DESCRIPTION_VAL)
/**
 *  @brief Set Identity Resolving Key value
 */
#define BT_UUID_CSIS_SIRK_VAL 0x2b84
/**
 *  @brief Set Identity Resolving Key
 */
#define BT_UUID_CSIS_SIRK         BT_UUID_DECLARE_16(BT_UUID_CSIS_SIRK_VAL)
/**
 *  @brief Set size value
 */
#define BT_UUID_CSIS_SET_SIZE_VAL 0x2b85
/**
 *  @brief Set size
 */
#define BT_UUID_CSIS_SET_SIZE \
    BT_UUID_DECLARE_16(BT_UUID_CSIS_SET_SIZE_VAL)
/**
 *  @brief Set lock value
 */
#define BT_UUID_CSIS_SET_LOCK_VAL 0x2b86
/**
 *  @brief Set lock
 */
#define BT_UUID_CSIS_SET_LOCK \
    BT_UUID_DECLARE_16(BT_UUID_CSIS_SET_LOCK_VAL)
/**
 *  @brief Rank value
 */
#define BT_UUID_CSIS_RANK_VAL 0x2b87
/**
 *  @brief Rank
 */
#define BT_UUID_CSIS_RANK \
    BT_UUID_DECLARE_16(BT_UUID_CSIS_RANK_VAL)
/**
 *  @brief GATT Characteristic Encrypted Data Key Material UUID Value
 */
#define BT_UUID_GATT_EDKM_VAL 0x2b88
/**
 *  @brief GATT Characteristic Encrypted Data Key Material
 */
#define BT_UUID_GATT_EDKM \
    BT_UUID_DECLARE_16(BT_UUID_GATT_EDKM_VAL)
/**
 *  @brief GATT Characteristic Apparent Energy 32 UUID Value
 */
#define BT_UUID_GATT_AE32_VAL 0x2b89
/**
 *  @brief GATT Characteristic Apparent Energy 32
 */
#define BT_UUID_GATT_AE32 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AE32_VAL)
/**
 *  @brief GATT Characteristic Apparent Power UUID Value
 */
#define BT_UUID_GATT_AP_VAL 0x2b8a
/**
 *  @brief GATT Characteristic Apparent Power
 */
#define BT_UUID_GATT_AP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_AP_VAL)
/**
 *  @brief GATT Characteristic CO2 Concentration UUID Value
 */
#define BT_UUID_GATT_CO2CONC_VAL 0x2b8c
/**
 *  @brief GATT Characteristic CO2 Concentration
 */
#define BT_UUID_GATT_CO2CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CO2CONC_VAL)
/**
 *  @brief GATT Characteristic Cosine of the Angle UUID Value
 */
#define BT_UUID_GATT_COS_VAL 0x2b8d
/**
 *  @brief GATT Characteristic Cosine of the Angle
 */
#define BT_UUID_GATT_COS \
    BT_UUID_DECLARE_16(BT_UUID_GATT_COS_VAL)
/**
 *  @brief GATT Characteristic Device Time Feature UUID Value
 */
#define BT_UUID_GATT_DEVTF_VAL 0x2b8e
/**
 *  @brief GATT Characteristic Device Time Feature
 */
#define BT_UUID_GATT_DEVTF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DEVTF_VAL)
/**
 *  @brief GATT Characteristic Device Time Parameters UUID Value
 */
#define BT_UUID_GATT_DEVTP_VAL 0x2b8f
/**
 *  @brief GATT Characteristic Device Time Parameters
 */
#define BT_UUID_GATT_DEVTP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DEVTP_VAL)
/**
 *  @brief GATT Characteristic Device Time UUID Value
 */
#define BT_UUID_GATT_DEVT_VAL 0x2b90
/**
 *  @brief GATT Characteristic Device Time
 */
#define BT_UUID_GATT_DEVT \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DEVT_VAL)
/**
 *  @brief GATT Characteristic Device Time Control Point UUID Value
 */
#define BT_UUID_GATT_DEVTCP_VAL 0x2b91
/**
 *  @brief GATT Characteristic Device Time Control Point
 */
#define BT_UUID_GATT_DEVTCP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_DEVTCP_VAL)
/**
 *  @brief GATT Characteristic Time Change Log Data UUID Value
 */
#define BT_UUID_GATT_TCLD_VAL 0x2b92
/**
 *  @brief GATT Characteristic Time Change Log Data
 */
#define BT_UUID_GATT_TCLD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TCLD_VAL)
/**
 *  @brief Media player name value
 */
#define BT_UUID_MCS_PLAYER_NAME_VAL 0x2b93
/**
 *  @brief Media player name
 */
#define BT_UUID_MCS_PLAYER_NAME \
    BT_UUID_DECLARE_16(BT_UUID_MCS_PLAYER_NAME_VAL)
/**
 *  @brief Media Icon Object ID value
 */
#define BT_UUID_MCS_ICON_OBJ_ID_VAL 0x2b94
/**
 *  @brief Media Icon Object ID
 */
#define BT_UUID_MCS_ICON_OBJ_ID \
    BT_UUID_DECLARE_16(BT_UUID_MCS_ICON_OBJ_ID_VAL)
/**
 *  @brief Media Icon URL value
 */
#define BT_UUID_MCS_ICON_URL_VAL 0x2b95
/**
 *  @brief Media Icon URL
 */
#define BT_UUID_MCS_ICON_URL \
    BT_UUID_DECLARE_16(BT_UUID_MCS_ICON_URL_VAL)
/**
 *  @brief Track Changed value
 */
#define BT_UUID_MCS_TRACK_CHANGED_VAL 0x2b96
/**
 *  @brief Track Changed
 */
#define BT_UUID_MCS_TRACK_CHANGED \
    BT_UUID_DECLARE_16(BT_UUID_MCS_TRACK_CHANGED_VAL)
/**
 *  @brief Track Title value
 */
#define BT_UUID_MCS_TRACK_TITLE_VAL 0x2b97
/**
 *  @brief Track Title
 */
#define BT_UUID_MCS_TRACK_TITLE \
    BT_UUID_DECLARE_16(BT_UUID_MCS_TRACK_TITLE_VAL)
/**
 *  @brief Track Duration value
 */
#define BT_UUID_MCS_TRACK_DURATION_VAL 0x2b98
/**
 *  @brief Track Duration
 */
#define BT_UUID_MCS_TRACK_DURATION \
    BT_UUID_DECLARE_16(BT_UUID_MCS_TRACK_DURATION_VAL)
/**
 *  @brief Track Position value
 */
#define BT_UUID_MCS_TRACK_POSITION_VAL 0x2b99
/**
 *  @brief Track Position
 */
#define BT_UUID_MCS_TRACK_POSITION \
    BT_UUID_DECLARE_16(BT_UUID_MCS_TRACK_POSITION_VAL)
/**
 *  @brief Playback Speed value
 */
#define BT_UUID_MCS_PLAYBACK_SPEED_VAL 0x2b9a
/**
 *  @brief Playback Speed
 */
#define BT_UUID_MCS_PLAYBACK_SPEED \
    BT_UUID_DECLARE_16(BT_UUID_MCS_PLAYBACK_SPEED_VAL)
/**
 *  @brief Seeking Speed value
 */
#define BT_UUID_MCS_SEEKING_SPEED_VAL 0x2b9b
/**
 *  @brief Seeking Speed
 */
#define BT_UUID_MCS_SEEKING_SPEED \
    BT_UUID_DECLARE_16(BT_UUID_MCS_SEEKING_SPEED_VAL)
/**
 *  @brief Track Segments Object ID value
 */
#define BT_UUID_MCS_TRACK_SEGMENTS_OBJ_ID_VAL 0x2b9c
/**
 *  @brief Track Segments Object ID
 */
#define BT_UUID_MCS_TRACK_SEGMENTS_OBJ_ID \
    BT_UUID_DECLARE_16(BT_UUID_MCS_TRACK_SEGMENTS_OBJ_ID_VAL)
/**
 *  @brief Current Track Object ID value
 */
#define BT_UUID_MCS_CURRENT_TRACK_OBJ_ID_VAL 0x2b9d
/**
 *  @brief Current Track Object ID
 */
#define BT_UUID_MCS_CURRENT_TRACK_OBJ_ID \
    BT_UUID_DECLARE_16(BT_UUID_MCS_CURRENT_TRACK_OBJ_ID_VAL)
/**
 *  @brief Next Track Object ID value
 */
#define BT_UUID_MCS_NEXT_TRACK_OBJ_ID_VAL 0x2b9e
/**
 *  @brief Next Track Object ID
 */
#define BT_UUID_MCS_NEXT_TRACK_OBJ_ID \
    BT_UUID_DECLARE_16(BT_UUID_MCS_NEXT_TRACK_OBJ_ID_VAL)
/**
 *  @brief Parent Group Object ID value
 */
#define BT_UUID_MCS_PARENT_GROUP_OBJ_ID_VAL 0x2b9f
/**
 *  @brief Parent Group Object ID
 */
#define BT_UUID_MCS_PARENT_GROUP_OBJ_ID \
    BT_UUID_DECLARE_16(BT_UUID_MCS_PARENT_GROUP_OBJ_ID_VAL)
/**
 *  @brief Group Object ID value
 */
#define BT_UUID_MCS_CURRENT_GROUP_OBJ_ID_VAL 0x2ba0
/**
 *  @brief Group Object ID
 */
#define BT_UUID_MCS_CURRENT_GROUP_OBJ_ID \
    BT_UUID_DECLARE_16(BT_UUID_MCS_CURRENT_GROUP_OBJ_ID_VAL)
/**
 *  @brief Playing Order value
 */
#define BT_UUID_MCS_PLAYING_ORDER_VAL 0x2ba1
/**
 *  @brief Playing Order
 */
#define BT_UUID_MCS_PLAYING_ORDER \
    BT_UUID_DECLARE_16(BT_UUID_MCS_PLAYING_ORDER_VAL)
/**
 *  @brief Playing Orders supported value
 */
#define BT_UUID_MCS_PLAYING_ORDERS_VAL 0x2ba2
/**
 *  @brief Playing Orders supported
 */
#define BT_UUID_MCS_PLAYING_ORDERS \
    BT_UUID_DECLARE_16(BT_UUID_MCS_PLAYING_ORDERS_VAL)
/**
 *  @brief Media State value
 */
#define BT_UUID_MCS_MEDIA_STATE_VAL 0x2ba3
/**
 *  @brief Media State
 */
#define BT_UUID_MCS_MEDIA_STATE \
    BT_UUID_DECLARE_16(BT_UUID_MCS_MEDIA_STATE_VAL)
/**
 *  @brief Media Control Point value
 */
#define BT_UUID_MCS_MEDIA_CONTROL_POINT_VAL 0x2ba4
/**
 *  @brief Media Control Point
 */
#define BT_UUID_MCS_MEDIA_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_MCS_MEDIA_CONTROL_POINT_VAL)
/**
 *  @brief Media control opcodes supported value
 */
#define BT_UUID_MCS_MEDIA_CONTROL_OPCODES_VAL 0x2ba5
/**
 *  @brief Media control opcodes supported
 */
#define BT_UUID_MCS_MEDIA_CONTROL_OPCODES \
    BT_UUID_DECLARE_16(BT_UUID_MCS_MEDIA_CONTROL_OPCODES_VAL)
/**
 *  @brief Search result object ID value
 */
#define BT_UUID_MCS_SEARCH_RESULTS_OBJ_ID_VAL 0x2ba6
/**
 *  @brief Search result object ID
 */
#define BT_UUID_MCS_SEARCH_RESULTS_OBJ_ID \
    BT_UUID_DECLARE_16(BT_UUID_MCS_SEARCH_RESULTS_OBJ_ID_VAL)
/**
 *  @brief Search control point value
 */
#define BT_UUID_MCS_SEARCH_CONTROL_POINT_VAL 0x2ba7
/**
 *  @brief Search control point
 */
#define BT_UUID_MCS_SEARCH_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_MCS_SEARCH_CONTROL_POINT_VAL)
/**
 *  @brief GATT Characteristic Energy 32 UUID Value
 */
#define BT_UUID_GATT_E32_VAL 0x2ba8
/**
 *  @brief GATT Characteristic Energy 32
 */
#define BT_UUID_GATT_E32 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_E32_VAL)

/**
 *  @brief Media Player Icon Object Type value
 */
#define BT_UUID_OTS_TYPE_MPL_ICON_VAL 0x2ba9
/**
 *  @brief Media Player Icon Object Type
 */
#define BT_UUID_OTS_TYPE_MPL_ICON \
    BT_UUID_DECLARE_16(BT_UUID_OTS_TYPE_MPL_ICON_VAL)
/**
 *  @brief Track Segments Object Type value
 */
#define BT_UUID_OTS_TYPE_TRACK_SEGMENT_VAL 0x2baa
/**
 *  @brief Track Segments Object Type
 */
#define BT_UUID_OTS_TYPE_TRACK_SEGMENT \
    BT_UUID_DECLARE_16(BT_UUID_OTS_TYPE_TRACK_SEGMENT_VAL)
/**
 *  @brief Track Object Type value
 */
#define BT_UUID_OTS_TYPE_TRACK_VAL 0x2bab
/**
 *  @brief Track Object Type
 */
#define BT_UUID_OTS_TYPE_TRACK \
    BT_UUID_DECLARE_16(BT_UUID_OTS_TYPE_TRACK_VAL)
/**
 *  @brief Group Object Type value
 */
#define BT_UUID_OTS_TYPE_GROUP_VAL 0x2bac
/**
 *  @brief Group Object Type
 */
#define BT_UUID_OTS_TYPE_GROUP \
    BT_UUID_DECLARE_16(BT_UUID_OTS_TYPE_GROUP_VAL)
/**
 *  @brief GATT Characteristic Constant Tone Extension Enable UUID Value
 */
#define BT_UUID_GATT_CTEE_VAL 0x2bad
/**
 *  @brief GATT Characteristic Constant Tone Extension Enable
 */
#define BT_UUID_GATT_CTEE \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CTEE_VAL)
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Minimum Length UUID Value
 */
#define BT_UUID_GATT_ACTEML_VAL 0x2bae
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Minimum Length
 */
#define BT_UUID_GATT_ACTEML \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACTEML_VAL)
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Minimum Transmit Count UUID Value
 */
#define BT_UUID_GATT_ACTEMTC_VAL 0x2baf
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Minimum Transmit Count
 */
#define BT_UUID_GATT_ACTEMTC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACTEMTC_VAL)
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Transmit Duration UUID Value
 */
#define BT_UUID_GATT_ACTETD_VAL 0x2bb0
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Transmit Duration
 */
#define BT_UUID_GATT_ACTETD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACTETD_VAL)
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Interval UUID Value
 */
#define BT_UUID_GATT_ACTEI_VAL 0x2bb1
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension Interval
 */
#define BT_UUID_GATT_ACTEI \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACTEI_VAL)
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension PHY UUID Value
 */
#define BT_UUID_GATT_ACTEP_VAL 0x2bb2
/**
 *  @brief GATT Characteristic Advertising Constant Tone Extension PHY
 */
#define BT_UUID_GATT_ACTEP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ACTEP_VAL)
/**
 *  @brief Bearer Provider Name value
 */
#define BT_UUID_TBS_PROVIDER_NAME_VAL 0x2bb3
/**
 *  @brief Bearer Provider Name
 */
#define BT_UUID_TBS_PROVIDER_NAME \
    BT_UUID_DECLARE_16(BT_UUID_TBS_PROVIDER_NAME_VAL)
/**
 *  @brief Bearer UCI value
 */
#define BT_UUID_TBS_UCI_VAL 0x2bb4
/**
 *  @brief Bearer UCI
 */
#define BT_UUID_TBS_UCI \
    BT_UUID_DECLARE_16(BT_UUID_TBS_UCI_VAL)
/**
 *  @brief Bearer Technology value
 */
#define BT_UUID_TBS_TECHNOLOGY_VAL 0x2bb5
/**
 *  @brief Bearer Technology
 */
#define BT_UUID_TBS_TECHNOLOGY \
    BT_UUID_DECLARE_16(BT_UUID_TBS_TECHNOLOGY_VAL)
/**
 *  @brief Bearer URI Prefixes Supported List value
 */
#define BT_UUID_TBS_URI_LIST_VAL 0x2bb6
/**
 *  @brief Bearer URI Prefixes Supported List
 */
#define BT_UUID_TBS_URI_LIST \
    BT_UUID_DECLARE_16(BT_UUID_TBS_URI_LIST_VAL)
/**
 *  @brief Bearer Signal Strength value
 */
#define BT_UUID_TBS_SIGNAL_STRENGTH_VAL 0x2bb7
/**
 *  @brief Bearer Signal Strength
 */
#define BT_UUID_TBS_SIGNAL_STRENGTH \
    BT_UUID_DECLARE_16(BT_UUID_TBS_SIGNAL_STRENGTH_VAL)
/**
 *  @brief Bearer Signal Strength Reporting Interval value
 */
#define BT_UUID_TBS_SIGNAL_INTERVAL_VAL 0x2bb8
/**
 *  @brief Bearer Signal Strength Reporting Interval
 */
#define BT_UUID_TBS_SIGNAL_INTERVAL \
    BT_UUID_DECLARE_16(BT_UUID_TBS_SIGNAL_INTERVAL_VAL)
/**
 *  @brief Bearer List Current Calls value
 */
#define BT_UUID_TBS_LIST_CURRENT_CALLS_VAL 0x2bb9
/**
 *  @brief Bearer List Current Calls
 */
#define BT_UUID_TBS_LIST_CURRENT_CALLS \
    BT_UUID_DECLARE_16(BT_UUID_TBS_LIST_CURRENT_CALLS_VAL)
/**
 *  @brief Content Control ID value
 */
#define BT_UUID_CCID_VAL 0x2bba
/**
 *  @brief Content Control ID
 */
#define BT_UUID_CCID \
    BT_UUID_DECLARE_16(BT_UUID_CCID_VAL)
/**
 *  @brief Status flags value
 */
#define BT_UUID_TBS_STATUS_FLAGS_VAL 0x2bbb
/**
 *  @brief Status flags
 */
#define BT_UUID_TBS_STATUS_FLAGS \
    BT_UUID_DECLARE_16(BT_UUID_TBS_STATUS_FLAGS_VAL)
/**
 *  @brief Incoming Call Target Caller ID value
 */
#define BT_UUID_TBS_INCOMING_URI_VAL 0x2bbc
/**
 *  @brief Incoming Call Target Caller ID
 */
#define BT_UUID_TBS_INCOMING_URI \
    BT_UUID_DECLARE_16(BT_UUID_TBS_INCOMING_URI_VAL)
/**
 *  @brief Call State value
 */
#define BT_UUID_TBS_CALL_STATE_VAL 0x2bbd
/**
 *  @brief Call State
 */
#define BT_UUID_TBS_CALL_STATE \
    BT_UUID_DECLARE_16(BT_UUID_TBS_CALL_STATE_VAL)
/**
 *  @brief Call Control Point value
 */
#define BT_UUID_TBS_CALL_CONTROL_POINT_VAL 0x2bbe
/**
 *  @brief Call Control Point
 */
#define BT_UUID_TBS_CALL_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_TBS_CALL_CONTROL_POINT_VAL)
/**
 *  @brief Optional Opcodes value
 */
#define BT_UUID_TBS_OPTIONAL_OPCODES_VAL 0x2bbf
/**
 *  @brief Optional Opcodes
 */
#define BT_UUID_TBS_OPTIONAL_OPCODES \
    BT_UUID_DECLARE_16(BT_UUID_TBS_OPTIONAL_OPCODES_VAL)
/** BT_UUID_TBS_TERMINATE_REASON_VAL
 *  @brief Terminate reason value
 */
#define BT_UUID_TBS_TERMINATE_REASON_VAL 0x2bc0
/** BT_UUID_TBS_TERMINATE_REASON
 *  @brief Terminate reason
 */
#define BT_UUID_TBS_TERMINATE_REASON \
    BT_UUID_DECLARE_16(BT_UUID_TBS_TERMINATE_REASON_VAL)
/**
 *  @brief Incoming Call value
 */
#define BT_UUID_TBS_INCOMING_CALL_VAL 0x2bc1
/**
 *  @brief Incoming Call
 */
#define BT_UUID_TBS_INCOMING_CALL \
    BT_UUID_DECLARE_16(BT_UUID_TBS_INCOMING_CALL_VAL)
/**
 *  @brief Incoming Call Friendly name value
 */
#define BT_UUID_TBS_FRIENDLY_NAME_VAL 0x2bc2
/**
 *  @brief Incoming Call Friendly name
 */
#define BT_UUID_TBS_FRIENDLY_NAME \
    BT_UUID_DECLARE_16(BT_UUID_TBS_FRIENDLY_NAME_VAL)
/**
 *  @brief Microphone Control Service Mute value
 */
#define BT_UUID_MICS_MUTE_VAL 0x2bc3
/**
 *  @brief Microphone Control Service Mute
 */
#define BT_UUID_MICS_MUTE \
    BT_UUID_DECLARE_16(BT_UUID_MICS_MUTE_VAL)
/**
 *  @brief Audio Stream Endpoint Sink Characteristic value
 */
#define BT_UUID_ASCS_ASE_SNK_VAL 0x2bc4
/**
 *  @brief Audio Stream Endpoint Sink Characteristic
 */
#define BT_UUID_ASCS_ASE_SNK \
    BT_UUID_DECLARE_16(BT_UUID_ASCS_ASE_SNK_VAL)
/**
 *  @brief Audio Stream Endpoint Source Characteristic value
 */
#define BT_UUID_ASCS_ASE_SRC_VAL 0x2bc5
/**
 *  @brief Audio Stream Endpoint Source Characteristic
 */
#define BT_UUID_ASCS_ASE_SRC \
    BT_UUID_DECLARE_16(BT_UUID_ASCS_ASE_SRC_VAL)
/**
 *  @brief Audio Stream Endpoint Control Point Characteristic value
 */
#define BT_UUID_ASCS_ASE_CP_VAL 0x2bc6
/**
 *  @brief Audio Stream Endpoint Control Point Characteristic
 */
#define BT_UUID_ASCS_ASE_CP \
    BT_UUID_DECLARE_16(BT_UUID_ASCS_ASE_CP_VAL)
/**
 *  @brief Broadcast Audio Scan Service Scan State value
 */
#define BT_UUID_BASS_CONTROL_POINT_VAL 0x2bc7
/**
 *  @brief Broadcast Audio Scan Service Scan State
 */
#define BT_UUID_BASS_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_BASS_CONTROL_POINT_VAL)
/**
 *  @brief Broadcast Audio Scan Service Receive State value
 */
#define BT_UUID_BASS_RECV_STATE_VAL 0x2bc8
/**
 *  @brief Broadcast Audio Scan Service Receive State
 */
#define BT_UUID_BASS_RECV_STATE \
    BT_UUID_DECLARE_16(BT_UUID_BASS_RECV_STATE_VAL)
/**
 *  @brief Sink PAC Characteristic value
 */
#define BT_UUID_PACS_SNK_VAL 0x2bc9
/**
 *  @brief Sink PAC Characteristic
 */
#define BT_UUID_PACS_SNK \
    BT_UUID_DECLARE_16(BT_UUID_PACS_SNK_VAL)
/**
 *  @brief Sink PAC Locations Characteristic value
 */
#define BT_UUID_PACS_SNK_LOC_VAL 0x2bca
/**
 *  @brief Sink PAC Locations Characteristic
 */
#define BT_UUID_PACS_SNK_LOC \
    BT_UUID_DECLARE_16(BT_UUID_PACS_SNK_LOC_VAL)
/**
 *  @brief Source PAC Characteristic value
 */
#define BT_UUID_PACS_SRC_VAL 0x2bcb
/**
 *  @brief Source PAC Characteristic
 */
#define BT_UUID_PACS_SRC \
    BT_UUID_DECLARE_16(BT_UUID_PACS_SRC_VAL)
/**
 *  @brief Source PAC Locations Characteristic value
 */
#define BT_UUID_PACS_SRC_LOC_VAL 0x2bcc
/**
 *  @brief Source PAC Locations Characteristic
 */
#define BT_UUID_PACS_SRC_LOC \
    BT_UUID_DECLARE_16(BT_UUID_PACS_SRC_LOC_VAL)
/**
 *  @brief Available Audio Contexts Characteristic value
 */
#define BT_UUID_PACS_AVAILABLE_CONTEXT_VAL 0x2bcd
/**
 *  @brief Available Audio Contexts Characteristic
 */
#define BT_UUID_PACS_AVAILABLE_CONTEXT \
    BT_UUID_DECLARE_16(BT_UUID_PACS_AVAILABLE_CONTEXT_VAL)
/**
 *  @brief Supported Audio Context Characteristic value
 */
#define BT_UUID_PACS_SUPPORTED_CONTEXT_VAL 0x2bce
/**
 *  @brief Supported Audio Context Characteristic
 */
#define BT_UUID_PACS_SUPPORTED_CONTEXT \
    BT_UUID_DECLARE_16(BT_UUID_PACS_SUPPORTED_CONTEXT_VAL)
/**
 *  @brief GATT Characteristic Ammonia Concentration UUID Value
 */
#define BT_UUID_GATT_NH4CONC_VAL 0x2bcf
/**
 *  @brief GATT Characteristic Ammonia Concentration
 */
#define BT_UUID_GATT_NH4CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_NH4CONC_VAL)
/**
 *  @brief GATT Characteristic Carbon Monoxide Concentration UUID Value
 */
#define BT_UUID_GATT_COCONC_VAL 0x2bd0
/**
 *  @brief GATT Characteristic Carbon Monoxide Concentration
 */
#define BT_UUID_GATT_COCONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_COCONC_VAL)
/**
 *  @brief GATT Characteristic Methane Concentration UUID Value
 */
#define BT_UUID_GATT_CH4CONC_VAL 0x2bd1
/**
 *  @brief GATT Characteristic Methane Concentration
 */
#define BT_UUID_GATT_CH4CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_CH4CONC_VAL)
/**
 *  @brief GATT Characteristic Nitrogen Dioxide Concentration UUID Value
 */
#define BT_UUID_GATT_NO2CONC_VAL 0x2bd2
/**
 *  @brief GATT Characteristic Nitrogen Dioxide Concentration
 */
#define BT_UUID_GATT_NO2CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_NO2CONC_VAL)
/**
 *  @brief GATT Characteristic Non-Methane Volatile Organic Compounds Concentration UUID Value
 */
#define BT_UUID_GATT_NONCH4CONC_VAL 0x2bd3
/**
 *  @brief GATT Characteristic Non-Methane Volatile Organic Compounds Concentration
 */
#define BT_UUID_GATT_NONCH4CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_NONCH4CONC_VAL)
/**
 *  @brief GATT Characteristic Ozone Concentration UUID Value
 */
#define BT_UUID_GATT_O3CONC_VAL 0x2bd4
/**
 *  @brief GATT Characteristic Ozone Concentration
 */
#define BT_UUID_GATT_O3CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_O3CONC_VAL)
/**
 *  @brief GATT Characteristic Particulate Matter - PM1 Concentration UUID Value
 */
#define BT_UUID_GATT_PM1CONC_VAL 0x2bd5
/**
 *  @brief GATT Characteristic Particulate Matter - PM1 Concentration
 */
#define BT_UUID_GATT_PM1CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PM1CONC_VAL)
/**
 *  @brief GATT Characteristic Particulate Matter - PM2.5 Concentration UUID Value
 */
#define BT_UUID_GATT_PM25CONC_VAL 0x2bd6
/**
 *  @brief GATT Characteristic Particulate Matter - PM2.5 Concentration
 */
#define BT_UUID_GATT_PM25CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PM25CONC_VAL)
/**
 *  @brief GATT Characteristic Particulate Matter - PM10 Concentration UUID Value
 */
#define BT_UUID_GATT_PM10CONC_VAL 0x2bd7
/**
 *  @brief GATT Characteristic Particulate Matter - PM10 Concentration
 */
#define BT_UUID_GATT_PM10CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_PM10CONC_VAL)
/**
 *  @brief GATT Characteristic Sulfur Dioxide Concentration UUID Value
 */
#define BT_UUID_GATT_SO2CONC_VAL 0x2bd8
/**
 *  @brief GATT Characteristic Sulfur Dioxide Concentration
 */
#define BT_UUID_GATT_SO2CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SO2CONC_VAL)
/**
 *  @brief GATT Characteristic Sulfur Hexafluoride Concentration UUID Value
 */
#define BT_UUID_GATT_SF6CONC_VAL 0x2bd9
/**
 *  @brief GATT Characteristic Sulfur Hexafluoride Concentration
 */
#define BT_UUID_GATT_SF6CONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SF6CONC_VAL)
/**
 *  @brief Hearing Aid Features Characteristic value
 */
#define BT_UUID_HAS_HEARING_AID_FEATURES_VAL 0x2bda
/**
 *  @brief Hearing Aid Features Characteristic
 */
#define BT_UUID_HAS_HEARING_AID_FEATURES \
    BT_UUID_DECLARE_16(BT_UUID_HAS_HEARING_AID_FEATURES_VAL)
/**
 *  @brief Hearing Aid Preset Control Point Characteristic value
 */
#define BT_UUID_HAS_PRESET_CONTROL_POINT_VAL 0x2bdb
/**
 *  @brief Hearing Aid Preset Control Point Characteristic
 */
#define BT_UUID_HAS_PRESET_CONTROL_POINT \
    BT_UUID_DECLARE_16(BT_UUID_HAS_PRESET_CONTROL_POINT_VAL)
/**
 *  @brief Active Preset Index Characteristic value
 */
#define BT_UUID_HAS_ACTIVE_PRESET_INDEX_VAL 0x2bdc
/**
 *  @brief Active Preset Index Characteristic
 */
#define BT_UUID_HAS_ACTIVE_PRESET_INDEX \
    BT_UUID_DECLARE_16(BT_UUID_HAS_ACTIVE_PRESET_INDEX_VAL)
/**
 *  @brief GATT Characteristic Fixed String 64 UUID Value
 */
#define BT_UUID_GATT_FSTR64_VAL 0x2bde
/**
 *  @brief GATT Characteristic Fixed String 64
 */
#define BT_UUID_GATT_FSTR64 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_FSTR64_VAL)
/**
 *  @brief GATT Characteristic High Temperature UUID Value
 */
#define BT_UUID_GATT_HITEMP_VAL 0x2bdf
/**
 *  @brief GATT Characteristic High Temperature
 */
#define BT_UUID_GATT_HITEMP \
    BT_UUID_DECLARE_16(BT_UUID_GATT_HITEMP_VAL)
/**
 *  @brief GATT Characteristic High Voltage UUID Value
 */
#define BT_UUID_GATT_HV_VAL 0x2be0
/**
 *  @brief GATT Characteristic High Voltage
 */
#define BT_UUID_GATT_HV \
    BT_UUID_DECLARE_16(BT_UUID_GATT_HV_VAL)
/**
 *  @brief GATT Characteristic Light Distribution UUID Value
 */
#define BT_UUID_GATT_LD_VAL 0x2be1
/**
 *  @brief GATT Characteristic Light Distribution
 */
#define BT_UUID_GATT_LD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LD_VAL)
/**
 *  @brief GATT Characteristic Light Output UUID Value
 */
#define BT_UUID_GATT_LO_VAL 0x2be2
/**
 *  @brief GATT Characteristic Light Output
 */
#define BT_UUID_GATT_LO \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LO_VAL)
/**
 *  @brief GATT Characteristic Light Source Type UUID Value
 */
#define BT_UUID_GATT_LST_VAL 0x2be3
/**
 *  @brief GATT Characteristic Light Source Type
 */
#define BT_UUID_GATT_LST \
    BT_UUID_DECLARE_16(BT_UUID_GATT_LST_VAL)
/**
 *  @brief GATT Characteristic Noise UUID Value
 */
#define BT_UUID_GATT_NOISE_VAL 0x2be4
/**
 *  @brief GATT Characteristic Noise
 */
#define BT_UUID_GATT_NOISE \
    BT_UUID_DECLARE_16(BT_UUID_GATT_NOISE_VAL)
/**
 *  @brief GATT Characteristic Relative Runtime in a Correlated Color Temperature Range UUID Value
 */
#define BT_UUID_GATT_RRCCTP_VAL 0x2be5
/**
 *  @brief GATT Characteristic Relative Runtime in a Correlated Color Temperature Range
 */
#define BT_UUID_GATT_RRCCTR \
    BT_UUID_DECLARE_16(BT_UUID_GATT_RRCCTR_VAL)
/**
 *  @brief GATT Characteristic Time Second 32 UUID Value
 */
#define BT_UUID_GATT_TIM_S32_VAL 0x2be6
/**
 *  @brief GATT Characteristic Time Second 32
 */
#define BT_UUID_GATT_TIM_S32 \
    BT_UUID_DECLARE_16(BT_UUID_GATT_TIM_S32_VAL)
/**
 *  @brief GATT Characteristic VOC Concentration UUID Value
 */
#define BT_UUID_GATT_VOCCONC_VAL 0x2be7
/**
 *  @brief GATT Characteristic VOC Concentration
 */
#define BT_UUID_GATT_VOCCONC \
    BT_UUID_DECLARE_16(BT_UUID_GATT_VOCCONC_VAL)
/**
 *  @brief GATT Characteristic Voltage Frequency UUID Value
 */
#define BT_UUID_GATT_VF_VAL 0x2be8
/**
 *  @brief GATT Characteristic Voltage Frequency
 */
#define BT_UUID_GATT_VF \
    BT_UUID_DECLARE_16(BT_UUID_GATT_VF_VAL)
/**
 *  @brief BAS Characteristic Battery Critical Status UUID Value
 */
#define BT_UUID_BAS_BATTERY_CRIT_STATUS_VAL 0x2be9
/**
 *  @brief BAS Characteristic Battery Critical Status
 */
#define BT_UUID_BAS_BATTERY_CRIT_STATUS \
    BT_UUID_DECLARE_16(BT_UUID_BAS_BATTERY_CRIT_STATUS_VAL)
/**
 *  @brief BAS Characteristic Battery Health Status UUID Value
 */
#define BT_UUID_BAS_BATTERY_HEALTH_STATUS_VAL 0x2bea
/**
 *  @brief BAS Characteristic Battery Health Status
 */
#define BT_UUID_BAS_BATTERY_HEALTH_STATUS \
    BT_UUID_DECLARE_16(BT_UUID_BAS_BATTERY_HEALTH_STATUS_VAL)
/**
 *  @brief BAS Characteristic Battery Health Information UUID Value
 */
#define BT_UUID_BAS_BATTERY_HEALTH_INF_VAL 0x2beb
/**
 *  @brief BAS Characteristic Battery Health Information
 */
#define BT_UUID_BAS_BATTERY_HEALTH_INF \
    BT_UUID_DECLARE_16(BT_UUID_BAS_BATTERY_HEALTH_INF_VAL)
/**
 *  @brief BAS Characteristic Battery Information UUID Value
 */
#define BT_UUID_BAS_BATTERY_INF_VAL 0x2bec
/**
 *  @brief BAS Characteristic Battery Information
 */
#define BT_UUID_BAS_BATTERY_INF \
    BT_UUID_DECLARE_16(BT_UUID_BAS_BATTERY_INF_VAL)
/**
 *  @brief BAS Characteristic Battery Level Status UUID Value
 */
#define BT_UUID_BAS_BATTERY_LEVEL_STATUS_VAL 0x2bed
/**
 *  @brief BAS Characteristic Battery Level Status
 */
#define BT_UUID_BAS_BATTERY_LEVEL_STATUS \
    BT_UUID_DECLARE_16(BT_UUID_BAS_BATTERY_LEVEL_STATUS_VAL)
/**
 *  @brief BAS Characteristic Battery Time Status UUID Value
 */
#define BT_UUID_BAS_BATTERY_TIME_STATUS_VAL 0x2bee
/**
 *  @brief BAS Characteristic Battery Time Status
 */
#define BT_UUID_BAS_BATTERY_TIME_STATUS \
    BT_UUID_DECLARE_16(BT_UUID_BAS_BATTERY_TIME_STATUS_VAL)
/**
 *  @brief GATT Characteristic Estimated Service Date UUID Value
 */
#define BT_UUID_GATT_ESD_VAL 0x2bef
/**
 *  @brief GATT Characteristic Estimated Service Date
 */
#define BT_UUID_GATT_ESD \
    BT_UUID_DECLARE_16(BT_UUID_GATT_ESD_VAL)
/**
 *  @brief BAS Characteristic Battery Energy Status UUID Value
 */
#define BT_UUID_BAS_BATTERY_ENERGY_STATUS_VAL 0x2bf0
/**
 *  @brief BAS Characteristic Battery Energy Status
 */
#define BT_UUID_BAS_BATTERY_ENERGY_STATUS \
    BT_UUID_DECLARE_16(BT_UUID_BAS_BATTERY_ENERGY_STATUS_VAL)
/**
 *  @brief GATT Characteristic LE GATT Security Levels UUID Value
 */
#define BT_UUID_GATT_SL_VAL 0x2bf5
/**
 *  @brief GATT Characteristic LE GATT Security Levels
 */
#define BT_UUID_GATT_SL \
    BT_UUID_DECLARE_16(BT_UUID_GATT_SL_VAL)

/**
 *  @brief GATT Characteristic UDI for Medical Devices UUID Value
 */
#define BT_UUID_UDI_FOR_MEDICAL_DEVICES_VAL 0x2bff
/**
 *  @brief GATT Characteristic UDI for Medical Devices
 */
#define BT_UUID_UDI_FOR_MEDICAL_DEVICES \
    BT_UUID_DECLARE_16(BT_UUID_UDI_FOR_MEDICAL_DEVICES_VAL)

/**
 *  @brief Gaming Service UUID value
 */
#define BT_UUID_GMAS_VAL 0x1858
/**
 *  @brief Common Audio Service
 */
#define BT_UUID_GMAS     BT_UUID_DECLARE_16(BT_UUID_GMAS_VAL)

/**
 *  @brief Gaming Audio Profile Role UUID value
 */
#define BT_UUID_GMAP_ROLE_VAL 0x2C00
/**
 *  @brief Gaming Audio Profile Role
 */
#define BT_UUID_GMAP_ROLE     BT_UUID_DECLARE_16(BT_UUID_GMAP_ROLE_VAL)

/**
 *  @brief Gaming Audio Profile Unicast Game Gateway Features UUID value
 */
#define BT_UUID_GMAP_UGG_FEAT_VAL 0x2C01
/**
 *  @brief Gaming Audio Profile Unicast Game Gateway Features
 */
#define BT_UUID_GMAP_UGG_FEAT     BT_UUID_DECLARE_16(BT_UUID_GMAP_UGG_FEAT_VAL)

/**
 *  @brief Gaming Audio Profile Unicast Game Terminal Features UUID value
 */
#define BT_UUID_GMAP_UGT_FEAT_VAL 0x2C02
/**
 *  @brief Gaming Audio Profile Unicast Game Terminal Features
 */
#define BT_UUID_GMAP_UGT_FEAT     BT_UUID_DECLARE_16(BT_UUID_GMAP_UGT_FEAT_VAL)

/**
 *  @brief Gaming Audio Profile Broadcast Game Sender Features UUID value
 */
#define BT_UUID_GMAP_BGS_FEAT_VAL 0x2C03
/**
 *  @brief Gaming Audio Profile Broadcast Game Sender Features
 */
#define BT_UUID_GMAP_BGS_FEAT     BT_UUID_DECLARE_16(BT_UUID_GMAP_BGS_FEAT_VAL)

/**
 *  @brief Gaming Audio Profile Broadcast Game Receiver Features UUID value
 */
#define BT_UUID_GMAP_BGR_FEAT_VAL 0x2C04
/**
 *  @brief Gaming Audio Profile Broadcast Game Receiver Features
 */
#define BT_UUID_GMAP_BGR_FEAT     BT_UUID_DECLARE_16(BT_UUID_GMAP_BGR_FEAT_VAL)

/*
 * Protocol UUIDs
 */
#define BT_UUID_SDP_VAL               0x0001
#define BT_UUID_SDP                   BT_UUID_DECLARE_16(BT_UUID_SDP_VAL)
#define BT_UUID_UDP_VAL               0x0002
#define BT_UUID_UDP                   BT_UUID_DECLARE_16(BT_UUID_UDP_VAL)
#define BT_UUID_RFCOMM_VAL            0x0003
#define BT_UUID_RFCOMM                BT_UUID_DECLARE_16(BT_UUID_RFCOMM_VAL)
#define BT_UUID_TCP_VAL               0x0004
#define BT_UUID_TCP                   BT_UUID_DECLARE_16(BT_UUID_TCP_VAL)
#define BT_UUID_TCS_BIN_VAL           0x0005
#define BT_UUID_TCS_BIN               BT_UUID_DECLARE_16(BT_UUID_TCS_BIN_VAL)
#define BT_UUID_TCS_AT_VAL            0x0006
#define BT_UUID_TCS_AT                BT_UUID_DECLARE_16(BT_UUID_TCS_AT_VAL)
#define BT_UUID_ATT_VAL               0x0007
#define BT_UUID_ATT                   BT_UUID_DECLARE_16(BT_UUID_ATT_VAL)
#define BT_UUID_OBEX_VAL              0x0008
#define BT_UUID_OBEX                  BT_UUID_DECLARE_16(BT_UUID_OBEX_VAL)
#define BT_UUID_IP_VAL                0x0009
#define BT_UUID_IP                    BT_UUID_DECLARE_16(BT_UUID_IP_VAL)
#define BT_UUID_FTP_VAL               0x000a
#define BT_UUID_FTP                   BT_UUID_DECLARE_16(BT_UUID_FTP_VAL)
#define BT_UUID_HTTP_VAL              0x000c
#define BT_UUID_HTTP                  BT_UUID_DECLARE_16(BT_UUID_HTTP_VAL)
#define BT_UUID_WSP_VAL               0x000e
#define BT_UUID_WSP                   BT_UUID_DECLARE_16(BT_UUID_WSP_VAL)
#define BT_UUID_BNEP_VAL              0x000f
#define BT_UUID_BNEP                  BT_UUID_DECLARE_16(BT_UUID_BNEP_VAL)
#define BT_UUID_UPNP_VAL              0x0010
#define BT_UUID_UPNP                  BT_UUID_DECLARE_16(BT_UUID_UPNP_VAL)
#define BT_UUID_HIDP_VAL              0x0011
#define BT_UUID_HIDP                  BT_UUID_DECLARE_16(BT_UUID_HIDP_VAL)
#define BT_UUID_HCRP_CTRL_VAL         0x0012
#define BT_UUID_HCRP_CTRL             BT_UUID_DECLARE_16(BT_UUID_HCRP_CTRL_VAL)
#define BT_UUID_HCRP_DATA_VAL         0x0014
#define BT_UUID_HCRP_DATA             BT_UUID_DECLARE_16(BT_UUID_HCRP_DATA_VAL)
#define BT_UUID_HCRP_NOTE_VAL         0x0016
#define BT_UUID_HCRP_NOTE             BT_UUID_DECLARE_16(BT_UUID_HCRP_NOTE_VAL)
#define BT_UUID_AVCTP_VAL             0x0017
#define BT_UUID_AVCTP                 BT_UUID_DECLARE_16(BT_UUID_AVCTP_VAL)
#define BT_UUID_AVCTP_BROWSING_VAL    0x0018
#define BT_UUID_AVCTP_BROWSING        BT_UUID_DECLARE_16(BT_UUID_AVCTP_BROWSING_VAL)
#define BT_UUID_AVDTP_VAL             0x0019
#define BT_UUID_AVDTP                 BT_UUID_DECLARE_16(BT_UUID_AVDTP_VAL)
#define BT_UUID_CMTP_VAL              0x001b
#define BT_UUID_CMTP                  BT_UUID_DECLARE_16(BT_UUID_CMTP_VAL)
#define BT_UUID_UDI_VAL               0x001d
#define BT_UUID_UDI                   BT_UUID_DECLARE_16(BT_UUID_UDI_VAL)
#define BT_UUID_MCAP_CTRL_VAL         0x001e
#define BT_UUID_MCAP_CTRL             BT_UUID_DECLARE_16(BT_UUID_MCAP_CTRL_VAL)
#define BT_UUID_MCAP_DATA_VAL         0x001f
#define BT_UUID_MCAP_DATA             BT_UUID_DECLARE_16(BT_UUID_MCAP_DATA_VAL)
#define BT_UUID_L2CAP_VAL             0x0100
#define BT_UUID_L2CAP                 BT_UUID_DECLARE_16(BT_UUID_L2CAP_VAL)

/** @brief Compare Bluetooth UUIDs.
 *
 *  Compares 2 Bluetooth UUIDs, if the types are different both UUIDs are
 *  first converted to 128 bits format before comparing.
 *
 *  @param u1 First Bluetooth UUID to compare
 *  @param u2 Second Bluetooth UUID to compare
 *
 *  @return negative value if @a u1 < @a u2, 0 if @a u1 == @a u2, else positive
 */
int bt_uuid_cmp(const struct bt_uuid *u1, const struct bt_uuid *u2);

/** @brief Create a bt_uuid from a little-endian data buffer.
 *
 *  Create a bt_uuid from a little-endian data buffer. The data_len parameter
 *  is used to determine whether the UUID is in 16, 32 or 128 bit format
 *  (length 2, 4 or 16). Note: 32 bit format is not allowed over the air.
 *
 *  @param uuid Pointer to the bt_uuid variable
 *  @param data pointer to UUID stored in little-endian data buffer
 *  @param data_len length of the UUID in the data buffer
 *
 *  @return true if the data was valid and the UUID was successfully created.
 */
bool bt_uuid_create(struct bt_uuid *uuid, const uint8_t *data, uint8_t data_len);

/** @brief Convert Bluetooth UUID to string.
 *
 *  Converts Bluetooth UUID to string.
 *  UUID can be in any format, 16-bit, 32-bit or 128-bit.
 *
 *  @param uuid Bluetooth UUID
 *  @param str pointer where to put converted string
 *  @param len length of str
 */
void bt_uuid_to_str(const struct bt_uuid *uuid, char *str, size_t len);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_UUID_H_ */
