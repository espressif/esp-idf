/**
 * @file
 * @brief Header for Bluetooth BAP.
 *
 * SPDX-FileCopyrightText: 2020 Bose Corporation
 * SPDX-FileCopyrightText: 2021-2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_BAP_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_BAP_

/**
 * @brief Bluetooth Basic Audio Profile (BAP)
 * @defgroup bt_bap Bluetooth Basic Audio Profile
 *
 * @since 3.0
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * The Basic Audio Profile (BAP) allows for both unicast and broadcast Audio Stream control.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/autoconf.h>
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/iso.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/net_buf.h>
#include <zephyr/sys/slist.h>
#include <zephyr/sys/util_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum size of BASE excluding service data header */
#define BT_BASE_MAX_SIZE (UINT8_MAX - 1 /* type */ - BT_UUID_SIZE_16)

/** An invalid Broadcast ID */
#define BT_BAP_INVALID_BROADCAST_ID 0xFFFFFFFFU

/** Value that represents an unset presentation delay value */
#define BT_BAP_PD_UNSET 0xFFFFFFFFU

/**
 * @brief Recommended connectable advertising parameters
 *
 * If connection has not been established after 30 seconds, the device should switch to
 * @ref BT_BAP_ADV_PARAM_CONN_REDUCED
 *
 * Defined by Table 8.1 in BAP 1.0.2
 */
#define BT_BAP_ADV_PARAM_CONN_QUICK                                                                \
    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONN | BT_LE_ADV_OPT_EXT_ADV, BT_GAP_MS_TO_ADV_INTERVAL(20), \
            BT_GAP_MS_TO_ADV_INTERVAL(30), NULL)

/**
 * @brief Reduced connectable advertising parameters
 *
 * Defined by Table 8.1 in BAP 1.0.2
 */
#define BT_BAP_ADV_PARAM_CONN_REDUCED                                                              \
    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONN | BT_LE_ADV_OPT_EXT_ADV,                                \
            BT_GAP_MS_TO_ADV_INTERVAL(150), BT_GAP_MS_TO_ADV_INTERVAL(150), NULL)

/**
 * @brief Recommended connection parameters for initial connection request for 7.5 ms SDU interval
 *
 * Once service discovery has completed and a stream is setup, it is recommended to switch to
 * @ref BT_BAP_CONN_PARAM_RELAXED
 *
 * Defined by Table 8.3 in BAP 1.0.2
 */
#define BT_BAP_CONN_PARAM_SHORT_7_5                                                                \
    BT_LE_CONN_PARAM(BT_GAP_US_TO_CONN_INTERVAL(7500), BT_GAP_MS_TO_CONN_INTERVAL(30), 0,      \
             BT_GAP_MS_TO_CONN_TIMEOUT(4000))

/**
 * @brief Recommended connection parameters for initial connection request for 10 ms SDU interval
 *
 * Once service discovery has completed and a stream is setup, it is recommended to switch to
 * @ref BT_BAP_CONN_PARAM_RELAXED
 *
 * Defined by Table 8.3 in BAP 1.0.2
 */
#define BT_BAP_CONN_PARAM_SHORT_10                                                                 \
    BT_LE_CONN_PARAM(BT_GAP_MS_TO_CONN_INTERVAL(10), BT_GAP_MS_TO_CONN_INTERVAL(30), 0,        \
             BT_GAP_MS_TO_CONN_TIMEOUT(4000))

/**
 * @brief Recommended connection parameters for coexistence of ACL and ISO
 *
 * Defined by Table 8.3 in BAP 1.0.2
 */
#define BT_BAP_CONN_PARAM_RELAXED                                                                  \
    BT_LE_CONN_PARAM(BT_GAP_MS_TO_CONN_INTERVAL(50), BT_GAP_MS_TO_CONN_INTERVAL(70), 0,        \
             BT_GAP_MS_TO_CONN_TIMEOUT(4000))

/**
 * @brief Fast advertising parameters for broadcast audio
 *
 * This is suitable for both 7.5 ms and 10 ms SDU intervals, but prioritizes lower time to
 * synchronize over coexistence with ISO and power consumption.
 */
#define BT_BAP_ADV_PARAM_BROADCAST_FAST                                                            \
    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_EXT_ADV, BT_GAP_MS_TO_ADV_INTERVAL(60),                      \
            BT_GAP_MS_TO_ADV_INTERVAL(60), NULL)

/**
 * @brief Slow advertising parameters for broadcast audio
 *
 * This is suitable for both 7.5 ms and 10 ms SDU intervals, but prioritizes coexistence with ISO
 * and power consumption over lower time to synchronize.
 */
#define BT_BAP_ADV_PARAM_BROADCAST_SLOW                                                            \
    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_EXT_ADV, BT_GAP_MS_TO_ADV_INTERVAL(150),                     \
            BT_GAP_MS_TO_ADV_INTERVAL(150), NULL)

/**
 * @brief Fast advertising parameters for broadcast audio
 *
 * This is suitable for both 7.5 ms and 10 ms SDU intervals, but prioritizes lower time to
 * synchronize over coexistence with ISO and power consumption.
 */
#define BT_BAP_PER_ADV_PARAM_BROADCAST_FAST                                                        \
    BT_LE_PER_ADV_PARAM(BT_GAP_MS_TO_PER_ADV_INTERVAL(60), BT_GAP_MS_TO_PER_ADV_INTERVAL(60),  \
                BT_LE_PER_ADV_OPT_NONE)

/**
 * @brief Slow advertising parameters for broadcast audio
 *
 * This is suitable for both 7.5 ms and 10 ms SDU intervals, but prioritizes coexistence with ISO
 * and power consumption over lower time to synchronize.
 */
#define BT_BAP_PER_ADV_PARAM_BROADCAST_SLOW                                                        \
    BT_LE_PER_ADV_PARAM(BT_GAP_MS_TO_PER_ADV_INTERVAL(150),                                    \
                BT_GAP_MS_TO_PER_ADV_INTERVAL(150), BT_LE_PER_ADV_OPT_NONE)

/**
 * @brief Check if a BAP BASS BIS_Sync bitfield is valid
 *
 * Valid options are either a bitmask of valid BIS indices, including none (0x00000000)
 * or @ref BT_BAP_BIS_SYNC_NO_PREF (0xFFFFFFFF).
 *
 * @param _bis_bitfield BIS_Sync bitfield (uint32)
 */
#define BT_BAP_BASS_VALID_BIT_BITFIELD(_bis_bitfield)                                              \
    ((_bis_bitfield) == 0U || (_bis_bitfield) == BT_BAP_BIS_SYNC_NO_PREF ||                    \
     BT_ISO_VALID_BIS_BITFIELD(_bis_bitfield))

/**
 * @brief Helper to declare elements of bt_bap_qos_cfg
 *
 * @param _interval SDU interval (usec)
 * @param _framing Framing
 * @param _phy Target PHY
 * @param _sdu Maximum SDU Size
 * @param _rtn Retransmission number
 * @param _latency Maximum Transport Latency (msec)
 * @param _pd Presentation Delay (usec)
 */
#define BT_BAP_QOS_CFG(_interval, _framing, _phy, _sdu, _rtn, _latency, _pd)                       \
    ((struct bt_bap_qos_cfg){                                                                  \
        .interval = _interval,                                                             \
        .framing = _framing,                                                               \
        .phy = _phy,                                                                       \
        .sdu = _sdu,                                                                       \
        .rtn = _rtn,                                                                       \
        IF_ENABLED(UTIL_OR(IS_ENABLED(CONFIG_BT_BAP_BROADCAST_SOURCE),                     \
                   IS_ENABLED(CONFIG_BT_BAP_UNICAST)),                             \
               (.latency = _latency,))                                                 \
        .pd = _pd,                                                                         \
    })

/** @brief QoS Framing */
enum bt_bap_qos_cfg_framing {
    /** Packets may be framed or unframed */
    BT_BAP_QOS_CFG_FRAMING_UNFRAMED = 0x00,
    /** Packets are always framed */
    BT_BAP_QOS_CFG_FRAMING_FRAMED = 0x01,
};

/** @brief QoS Preferred PHY */
enum {
    /** LE 1M PHY */
    BT_BAP_QOS_CFG_1M = BIT(0),
    /** LE 2M PHY */
    BT_BAP_QOS_CFG_2M = BIT(1),
    /** LE Coded PHY */
    BT_BAP_QOS_CFG_CODED = BIT(2),
};

/**
 * @brief Helper to declare Input Unframed bt_bap_qos_cfg
 *
 * @param _interval SDU interval (usec)
 * @param _sdu Maximum SDU Size
 * @param _rtn Retransmission number
 * @param _latency Maximum Transport Latency (msec)
 * @param _pd Presentation Delay (usec)
 */
#define BT_BAP_QOS_CFG_UNFRAMED(_interval, _sdu, _rtn, _latency, _pd)                              \
    BT_BAP_QOS_CFG(_interval, BT_BAP_QOS_CFG_FRAMING_UNFRAMED, BT_BAP_QOS_CFG_2M, _sdu, _rtn,  \
               _latency, _pd)

/**
 * @brief Helper to declare Input Framed bt_bap_qos_cfg
 *
 * @param _interval SDU interval (usec)
 * @param _sdu Maximum SDU Size
 * @param _rtn Retransmission number
 * @param _latency Maximum Transport Latency (msec)
 * @param _pd Presentation Delay (usec)
 */
#define BT_BAP_QOS_CFG_FRAMED(_interval, _sdu, _rtn, _latency, _pd)                                \
    BT_BAP_QOS_CFG(_interval, BT_BAP_QOS_CFG_FRAMING_FRAMED, BT_BAP_QOS_CFG_2M, _sdu, _rtn,    \
               _latency, _pd)

/** @brief QoS configuration structure. */
struct bt_bap_qos_cfg {
    /**
     * @brief Presentation Delay in microseconds
     *
     * This value can be changed up and until bt_bap_stream_qos() has been called.
     * Once a stream has been QoS configured, modifying this field does not modify the value.
     * It is however possible to modify this field and call bt_bap_stream_qos() again to update
     * the value, assuming that the stream is in the correct state.
     *
     * Value range 0 to @ref BT_AUDIO_PD_MAX.
     */
    uint32_t pd;

    /**
     * @brief Connected Isochronous Group (CIG) parameters
     *
     * The fields in this struct affect the value sent to the controller via HCI
     * when creating the CIG. Once the group has been created with
     * bt_bap_unicast_group_create(), modifying these fields will not affect the group.
     */
    struct {
        /** QoS Framing */
        enum bt_bap_qos_cfg_framing framing;

        /**
         * @brief PHY
         *
         * Allowed values are @ref BT_BAP_QOS_CFG_1M, @ref BT_BAP_QOS_CFG_2M and
         * @ref BT_BAP_QOS_CFG_CODED.
         */
        uint8_t phy;

        /**
         * @brief Retransmission Number
         *
         * This a recommendation to the controller, and the actual retransmission number
         * may be different than this.
         */
        uint8_t rtn;

        /**
         * @brief Maximum SDU size
         *
         * Value range @ref BT_ISO_MIN_SDU to @ref BT_ISO_MAX_SDU.
         */
        uint16_t sdu;

        /**
         * @brief Maximum Transport Latency
         *
         * Not used for the @kconfig{CONFIG_BT_BAP_BROADCAST_SINK} role.
         */
        uint16_t latency;

        /**
         * @brief SDU Interval
         *
         * Value range @ref BT_ISO_SDU_INTERVAL_MIN to @ref BT_ISO_SDU_INTERVAL_MAX
         */
        uint32_t interval;

        /**
         * @brief Maximum PDU size
         *
         * Maximum size, in octets, of the payload from link layer to link layer.
         *
         *  Value range @ref BT_ISO_CONNECTED_PDU_MIN to @ref BT_ISO_PDU_MAX for
         *  connected ISO.
         *
         *  Value range @ref BT_ISO_BROADCAST_PDU_MIN to @ref BT_ISO_PDU_MAX for
         *  broadcast ISO.
         */
        uint16_t max_pdu;

        /**
         * @brief Burst number
         *
         * Value range @ref BT_ISO_BN_MIN to @ref BT_ISO_BN_MAX.
         */
        uint8_t burst_number;

        /**
         * @brief Number of subevents
         *
         * Maximum number of subevents in each CIS or BIS event.
         *
         * Value range @ref BT_ISO_NSE_MIN to @ref BT_ISO_NSE_MAX.
         */
        uint8_t num_subevents;
    };
};

/**
 * @brief Helper to declare elements of @ref bt_bap_qos_cfg_pref
 *
 * @param _unframed_supported Unframed PDUs supported
 * @param _phy Preferred Target PHY
 * @param _rtn Preferred Retransmission number
 * @param _latency Preferred Maximum Transport Latency (msec)
 * @param _pd_min Minimum Presentation Delay (usec)
 * @param _pd_max Maximum Presentation Delay (usec)
 * @param _pref_pd_min Preferred Minimum Presentation Delay (usec)
 * @param _pref_pd_max Preferred Maximum Presentation Delay (usec)
 */
#define BT_BAP_QOS_CFG_PREF(_unframed_supported, _phy, _rtn, _latency, _pd_min, _pd_max,           \
                _pref_pd_min, _pref_pd_max)                                            \
    {                                                                                          \
        .unframed_supported = _unframed_supported, .phy = _phy, .rtn = _rtn,               \
        .latency = _latency, .pd_min = _pd_min, .pd_max = _pd_max,                         \
        .pref_pd_min = _pref_pd_min, .pref_pd_max = _pref_pd_max,                          \
    }

/** @brief Audio Stream Quality of Service Preference structure. */
struct bt_bap_qos_cfg_pref {
    /**
     * @brief Unframed PDUs supported
     *
     *  Unlike the other fields, this is not a preference but whether
     *  the codec supports unframed ISOAL PDUs.
     */
    bool unframed_supported;

    /**
     * @brief Preferred PHY bitfield
     *
     * Bitfield consisting of one or more of @ref BT_GAP_LE_PHY_1M, @ref BT_GAP_LE_PHY_2M and
     * @ref BT_GAP_LE_PHY_CODED.
     */
    uint8_t phy;

    /**
     * @brief Preferred Retransmission Number
     *
     * @ref BT_AUDIO_RTN_PREF_NONE indicates no preference.
     */
    uint8_t rtn;

    /**
     * Preferred Transport Latency
     *
     * Value range @ref BT_ISO_LATENCY_MIN to @ref BT_ISO_LATENCY_MAX
     */
    uint16_t latency;

    /**
     * @brief Minimum Presentation Delay in microseconds
     *
     * Unlike the other fields, this is not a preference but a minimum requirement.
     *
     * Value range 0 to @ref BT_AUDIO_PD_MAX
     */
    uint32_t pd_min;

    /**
     * @brief Maximum Presentation Delay in microseconds
     *
     * Unlike the other fields, this is not a preference but a maximum requirement.
     *
     * Value range @ref bt_bap_qos_cfg_pref.pd_min to @ref BT_AUDIO_PD_MAX
     */
    uint32_t pd_max;

    /**
     * @brief Preferred minimum Presentation Delay in microseconds
     *
     * Value range @ref bt_bap_qos_cfg_pref.pd_min to @ref bt_bap_qos_cfg_pref.pd_max, or
     * @ref BT_AUDIO_PD_PREF_NONE to indicate no preference.
     */
    uint32_t pref_pd_min;

    /**
     * @brief Preferred maximum Presentation Delay in microseconds
     *
     * Value range @ref bt_bap_qos_cfg_pref.pd_min to @ref bt_bap_qos_cfg_pref.pd_max,
     * and higher than or equal to @ref bt_bap_qos_cfg_pref.pref_pd_min, or
     * @ref BT_AUDIO_PD_PREF_NONE to indicate no preference.
     */
    uint32_t pref_pd_max;
};

/** Periodic advertising state reported by the Scan Delegator */
enum bt_bap_pa_state {
    /** The periodic advertising has not been synchronized */
    BT_BAP_PA_STATE_NOT_SYNCED = 0x00,

    /** Waiting for SyncInfo from Broadcast Assistant */
    BT_BAP_PA_STATE_INFO_REQ = 0x01,

    /** Synchronized to periodic advertising */
    BT_BAP_PA_STATE_SYNCED = 0x02,

    /** Failed to synchronized to periodic advertising */
    BT_BAP_PA_STATE_FAILED = 0x03,

    /** No periodic advertising sync transfer receiver from Broadcast Assistant */
    BT_BAP_PA_STATE_NO_PAST = 0x04,
};

/** Broadcast Isochronous Group encryption state reported by the Scan Delegator */
enum bt_bap_big_enc_state {
    /** The Broadcast Isochronous Group not encrypted */
    BT_BAP_BIG_ENC_STATE_NO_ENC = 0x00,

    /** The Broadcast Isochronous Group broadcast code requested */
    BT_BAP_BIG_ENC_STATE_BCODE_REQ = 0x01,

    /** The Broadcast Isochronous Group decrypted */
    BT_BAP_BIG_ENC_STATE_DEC = 0x02,

    /** The Broadcast Isochronous Group bad broadcast code */
    BT_BAP_BIG_ENC_STATE_BAD_CODE = 0x03,
};

/** Broadcast Audio Scan Service (BASS) specific ATT error codes */
enum bt_bap_bass_att_err {
    /** Opcode not supported */
    BT_BAP_BASS_ERR_OPCODE_NOT_SUPPORTED = 0x80,

    /** Invalid source ID supplied */
    BT_BAP_BASS_ERR_INVALID_SRC_ID = 0x81,
};

/** Value indicating that the periodic advertising interval is unknown */
#define BT_BAP_PA_INTERVAL_UNKNOWN             0xFFFF

/**
 * @brief Broadcast Assistant no BIS sync preference
 *
 * Value indicating that the Broadcast Assistant has no preference to which BIS
 * the Scan Delegator syncs to
 */
#define BT_BAP_BIS_SYNC_NO_PREF 0xFFFFFFFF
/** BIS sync value indicating that the BIG sync has failed for any reason */
#define BT_BAP_BIS_SYNC_FAILED  0xFFFFFFFF

/** Endpoint states */
enum bt_bap_ep_state {
    /** Audio Stream Endpoint Idle state */
    BT_BAP_EP_STATE_IDLE = 0x00,

    /** Audio Stream Endpoint Codec Configured state */
    BT_BAP_EP_STATE_CODEC_CONFIGURED = 0x01,

    /** Audio Stream Endpoint QoS Configured state */
    BT_BAP_EP_STATE_QOS_CONFIGURED = 0x02,

    /** Audio Stream Endpoint Enabling state */
    BT_BAP_EP_STATE_ENABLING = 0x03,

    /** Audio Stream Endpoint Streaming state */
    BT_BAP_EP_STATE_STREAMING = 0x04,

    /** Audio Stream Endpoint Disabling state */
    BT_BAP_EP_STATE_DISABLING = 0x05,

    /** Audio Stream Endpoint Streaming state */
    BT_BAP_EP_STATE_RELEASING = 0x06,
};

/**
 * @brief Response Status Code
 *
 * These are sent by the server to the client when a stream operation is
 * requested.
 */
enum bt_bap_ascs_rsp_code {
    /** Server completed operation successfully */
    BT_BAP_ASCS_RSP_CODE_SUCCESS = 0x00,
    /** Server did not support operation by client */
    BT_BAP_ASCS_RSP_CODE_NOT_SUPPORTED = 0x01,
    /** Server rejected due to invalid operation length */
    BT_BAP_ASCS_RSP_CODE_INVALID_LENGTH = 0x02,
    /** Invalid ASE ID */
    BT_BAP_ASCS_RSP_CODE_INVALID_ASE = 0x03,
    /** Invalid ASE state */
    BT_BAP_ASCS_RSP_CODE_INVALID_ASE_STATE = 0x04,
    /** Invalid operation for direction */
    BT_BAP_ASCS_RSP_CODE_INVALID_DIR = 0x05,
    /** Capabilities not supported by server */
    BT_BAP_ASCS_RSP_CODE_CAP_UNSUPPORTED = 0x06,
    /** Configuration parameters not supported by server */
    BT_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED = 0x07,
    /** Configuration parameters rejected by server */
    BT_BAP_ASCS_RSP_CODE_CONF_REJECTED = 0x08,
    /** Invalid Configuration parameters */
    BT_BAP_ASCS_RSP_CODE_CONF_INVALID = 0x09,
    /** Unsupported metadata */
    BT_BAP_ASCS_RSP_CODE_METADATA_UNSUPPORTED = 0x0a,
    /** Metadata rejected by server */
    BT_BAP_ASCS_RSP_CODE_METADATA_REJECTED = 0x0b,
    /** Invalid metadata */
    BT_BAP_ASCS_RSP_CODE_METADATA_INVALID = 0x0c,
    /** Server has insufficient resources */
    BT_BAP_ASCS_RSP_CODE_NO_MEM = 0x0d,
    /** Unspecified error */
    BT_BAP_ASCS_RSP_CODE_UNSPECIFIED = 0x0e,
};

/**
 * @brief Response Reasons
 *
 * These are used if the @ref bt_bap_ascs_rsp_code value is
 * @ref BT_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED, @ref BT_BAP_ASCS_RSP_CODE_CONF_REJECTED or
 * @ref BT_BAP_ASCS_RSP_CODE_CONF_INVALID.
 */
enum bt_bap_ascs_reason {
    /** No reason */
    BT_BAP_ASCS_REASON_NONE = 0x00,
    /** Codec ID */
    BT_BAP_ASCS_REASON_CODEC = 0x01,
    /** Codec configuration */
    BT_BAP_ASCS_REASON_CODEC_DATA = 0x02,
    /** SDU interval */
    BT_BAP_ASCS_REASON_INTERVAL = 0x03,
    /** Framing */
    BT_BAP_ASCS_REASON_FRAMING = 0x04,
    /** PHY */
    BT_BAP_ASCS_REASON_PHY = 0x05,
    /** Maximum SDU size*/
    BT_BAP_ASCS_REASON_SDU = 0x06,
    /** RTN */
    BT_BAP_ASCS_REASON_RTN = 0x07,
    /** Max transport latency */
    BT_BAP_ASCS_REASON_LATENCY = 0x08,
    /** Presendation delay */
    BT_BAP_ASCS_REASON_PD = 0x09,
    /** Invalid CIS mapping */
    BT_BAP_ASCS_REASON_CIS = 0x0a,
};

/** @brief Structure storing values of fields of ASE Control Point notification. */
struct bt_bap_ascs_rsp {
    /**
     * @brief Value of the Response Code field.
     *
     * The following response codes are accepted:
     * - @ref BT_BAP_ASCS_RSP_CODE_SUCCESS
     * - @ref BT_BAP_ASCS_RSP_CODE_CAP_UNSUPPORTED
     * - @ref BT_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED
     * - @ref BT_BAP_ASCS_RSP_CODE_CONF_REJECTED
     * - @ref BT_BAP_ASCS_RSP_CODE_METADATA_UNSUPPORTED
     * - @ref BT_BAP_ASCS_RSP_CODE_METADATA_REJECTED
     * - @ref BT_BAP_ASCS_RSP_CODE_NO_MEM
     * - @ref BT_BAP_ASCS_RSP_CODE_UNSPECIFIED
     */
    enum bt_bap_ascs_rsp_code code;

    /**
     * @brief Value of the Reason field.
     *
     * The meaning of this value depend on the Response Code field.
     */
    union {
        /**
         * @brief Response reason
         *
         * If the Response Code is one of the following:
         * - @ref BT_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED
         * - @ref BT_BAP_ASCS_RSP_CODE_CONF_REJECTED
         * all values from @ref bt_bap_ascs_reason can be used.
         *
         * If the Response Code is one of the following:
         * - @ref BT_BAP_ASCS_RSP_CODE_SUCCESS
         * - @ref BT_BAP_ASCS_RSP_CODE_CAP_UNSUPPORTED
         * - @ref BT_BAP_ASCS_RSP_CODE_NO_MEM
         * - @ref BT_BAP_ASCS_RSP_CODE_UNSPECIFIED
         * only value @ref BT_BAP_ASCS_REASON_NONE shall be used.
         */
        enum bt_bap_ascs_reason reason;

        /**
         * @brief Response metadata type
         *
         * If the Response Code is one of the following:
         * - @ref BT_BAP_ASCS_RSP_CODE_METADATA_UNSUPPORTED
         * - @ref BT_BAP_ASCS_RSP_CODE_METADATA_REJECTED
         * the value of the Metadata Type shall be used.
         */
        enum bt_audio_metadata_type metadata_type;
    };
};

/**
 * @brief Macro used to initialise the object storing values of ASE Control Point notification.
 *
 * @param c Response Code field
 * @param r Reason field - @ref bt_bap_ascs_reason or @ref bt_audio_metadata_type (see notes in
 *          @ref bt_bap_ascs_rsp).
 */
#define BT_BAP_ASCS_RSP(c, r) (struct bt_bap_ascs_rsp) { .code = c, .reason = r }

/** @brief Abstract Audio Broadcast Source structure. */
struct bt_bap_broadcast_source;

/** @brief Abstract Audio Broadcast Sink structure. */
struct bt_bap_broadcast_sink;

/** @brief Abstract Audio Unicast Group structure. */
struct bt_bap_unicast_group;

/** @brief Abstract Audio Endpoint structure. */
struct bt_bap_ep;

/** Struct to hold subgroup specific information for the receive state */
struct bt_bap_bass_subgroup {
    /** BIS synced bitfield */
    uint32_t bis_sync;

    /** Length of the metadata */
    uint8_t metadata_len;

    /** The metadata */
    uint8_t *metadata;
};

/** Represents the Broadcast Audio Scan Service receive state */
struct bt_bap_scan_delegator_recv_state {
    /** The source ID  */
    uint8_t src_id;

    /** The Bluetooth address */
    bt_addr_le_t addr;

    /** The advertising set ID*/
    uint8_t adv_sid;

    /** The periodic adverting sync state */
    enum bt_bap_pa_state pa_sync_state;

    /** The broadcast isochronous group encryption state */
    enum bt_bap_big_enc_state encrypt_state;

    /** The 24-bit broadcast ID */
    uint32_t broadcast_id;

    /**
     * @brief The bad broadcast code
     *
     * Only valid if encrypt_state is @ref BT_BAP_BIG_ENC_STATE_BCODE_REQ
     */
    uint8_t bad_code[BT_ISO_BROADCAST_CODE_SIZE];

    /** Number of subgroups */
    uint8_t num_subgroups;

    /** Subgroup specific information
     *
     * If the @ref bt_bap_bass_subgroup.bis_sync value is @ref BT_BAP_BIS_SYNC_FAILED then it
     * indicates that the BIG sync failed.
     */
    struct bt_bap_bass_subgroup *subgroups;
};

/**
 * @brief Struct to hold the Basic Audio Profile Scan Delegator callbacks
 *
 * These can be registered for usage with bt_bap_scan_delegator_register().
 */
struct bt_bap_scan_delegator_cb {
    /**
     * @brief Receive state updated
     *
     * @param conn       Pointer to the connection to a remote device if
     *                   the change was caused by it, otherwise NULL.
     * @param recv_state Pointer to the receive state that was updated.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    void (*recv_state_updated)(struct bt_conn *conn,
                               const struct bt_bap_scan_delegator_recv_state *recv_state);

    /**
     * @brief Periodic advertising sync request
     *
     * Request from peer device to synchronize with the periodic advertiser
     * denoted by the @p recv_state. To notify the Broadcast Assistant about
     * any pending sync
     *
     * @param conn        Pointer to the connection requesting the
     *                    periodic advertising sync.
     * @param recv_state  Pointer to the receive state that is being
     *                    requested for periodic advertising sync.
     * @param past_avail  True if periodic advertising sync transfer is available.
     * @param pa_interval The periodic advertising interval.
     *
     * @return 0 in case of accept, or other value to reject.
     */
    int (*pa_sync_req)(struct bt_conn *conn,
                       const struct bt_bap_scan_delegator_recv_state *recv_state,
                       bool past_avail, uint16_t pa_interval);

    /**
     * @brief Periodic advertising sync termination request
     *
     * Request from peer device to terminate the periodic advertiser sync
     * denoted by the @p recv_state.
     *
     * @param conn        Pointer to the connection requesting the periodic
     *                    advertising sync termination.
     * @param recv_state  Pointer to the receive state that is being
     *                    requested for periodic advertising sync.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    int (*pa_sync_term_req)(struct bt_conn *conn,
                            const struct bt_bap_scan_delegator_recv_state *recv_state);

    /**
     * @brief Broadcast code received
     *
     * Broadcast code received from a broadcast assistant
     *
     * @param conn           Pointer to the connection providing the
     *                       broadcast code.
     * @param recv_state     Pointer to the receive state the broadcast code
     *                       is being provided for.
     * @param broadcast_code The 16-octet broadcast code
     */
    void (*broadcast_code)(struct bt_conn *conn,
                           const struct bt_bap_scan_delegator_recv_state *recv_state,
                           const uint8_t broadcast_code[BT_ISO_BROADCAST_CODE_SIZE]);
    /**
     * @brief Broadcast Isochronous Stream synchronize request
     *
     * Request from Broadcast Assistant device to modify the Broadcast
     * Isochronous Stream states. The request shall be fulfilled with
     * accordance to the @p bis_sync_req within reasonable time. The
     * Broadcast Assistant may also request fewer, or none, indexes to
     * be synchronized.
     *
     * @param[in]  conn          Pointer to the connection of the
     *                           Broadcast Assistant requesting the sync.
     * @param[in]  recv_state    Pointer to the receive state that is being
     *                           requested for the sync.
     * @param[in]  bis_sync_req  Array of bitfields of which BIS indexes
     *                           that is requested to sync for each subgroup
     *                           by the Broadcast Assistant. A value of 0
     *                           indicates a request to terminate the BIG
     *                           sync.
     *
     * @return 0 in case of accept, or other value to reject.
     */
    int (*bis_sync_req)(struct bt_conn *conn,
                        const struct bt_bap_scan_delegator_recv_state *recv_state,
                        const uint32_t bis_sync_req[CONFIG_BT_BAP_BASS_MAX_SUBGROUPS]);
    /**
     * @brief Broadcast Assistant scanning state callback
     *
     * Callback triggered when a Broadcast Assistant notifies the Scan Delegator about the
     * assistants scanning state.
     *
     * @param conn Pointer to the connection that initiated the scan.
     * @param is_scanning true if scanning started, false if scanning stopped.
     */
    void (*scanning_state)(struct bt_conn *conn, bool is_scanning);
    /**
     * @brief Add Source operation callback
     *
     * These callbacks notify the application when a request comes
     * in to add a source. The application can return 0 to
     * accept or any other value to reject the request.
     *
     * @param conn       Pointer to the connection that initiated the request,
     *                   or NULL if locally triggered.
     * @param recv_state Pointer to the requested receive state to be added.
     *
     * @return 0 in case of accept, or other value to reject.
     */
    int (*add_source)(struct bt_conn *conn,
                      const struct bt_bap_scan_delegator_recv_state *recv_state);

    /**
     * @brief Modify Source operation callback
     *
     * These callbacks notify the application when a request comes
     * in to modify a source. The application can return 0 to
     * accept or any other value to reject the request.
     *
     * @param conn       Pointer to the connection that initiated the request,
     *                   or NULL if locally triggered.
     * @param recv_state Pointer to the requested receive state to be modified.
     *
     * @return 0 in case of accept, or other value to reject.
     */
    int (*modify_source)(struct bt_conn *conn,
                         const struct bt_bap_scan_delegator_recv_state *recv_state);

    /**
     * @brief Remove Source operation callback
     *
     * These callbacks notify the application when a request comes
     * in to remove a source. The application can return 0 to
     * accept or any other value to reject the request.
     *
     * @param conn   Pointer to the connection that initiated the request,
     *               or NULL if locally triggered.
     * @param src_id The Source ID that is requested to be removed.
     *
     * @return 0 in case of accept, or other value to reject.
     */
    int (*remove_source)(struct bt_conn *conn, uint8_t src_id);
};

/** Structure holding information of audio stream endpoint */
struct bt_bap_ep_info {
    /** The ID of the endpoint */
    uint8_t id;

    /** The state of the endpoint */
    enum bt_bap_ep_state state;

    /** Capabilities type */
    enum bt_audio_dir dir;

    /** The isochronous channel associated with the endpoint. */
    struct bt_iso_chan *iso_chan;

    /** @brief True if the stream associated with the endpoint is able to send data */
    bool can_send;

    /** @brief True if the stream associated with the endpoint is able to receive data */
    bool can_recv;

    /** Pointer to paired endpoint if the endpoint is part of a bidirectional CIS,
     *  otherwise NULL
     */
    struct bt_bap_ep *paired_ep;

    /** Pointer to the preferred QoS settings associated with the endpoint */
    const struct bt_bap_qos_cfg_pref *qos_pref;
};

/**
 * @brief Return structure holding information of audio stream endpoint
 *
 * @param ep   The audio stream endpoint object.
 * @param info The structure object to be filled with the info.
 *
 * @retval 0 in case of success
 * @retval -EINVAL if @p ep or @p info are NULL
 */
int bt_bap_ep_get_info_safe(const struct bt_bap_ep *ep, struct bt_bap_ep_info *info);

/**
 * @brief Basic Audio Profile stream structure.
 *
 * Streams represents a stream configuration of a Remote Endpoint and a Local Capability.
 *
 * @note Streams are unidirectional but can be paired with other streams to use a bidirectional
 * connected isochronous stream.
 */
struct bt_bap_stream {
    /** Connection reference */
    struct bt_conn *conn;

    /** Endpoint reference */
    struct bt_bap_ep *ep;

    /** Codec Configuration */
    struct bt_audio_codec_cfg *codec_cfg;

    /** QoS Configuration */
    struct bt_bap_qos_cfg *qos;

    /** Audio stream operations */
    struct bt_bap_stream_ops *ops;

    /** Stream user data */
    void *user_data;

    /** ISO channel reference
     *
     * This will become valid once the stream is added to a group (bt_bap_unicast_group,
     * bt_bap_broadcast_source or bt_bap_broadcast_sink).
     */
    struct bt_iso_chan *iso;

    /** Unicast or Broadcast group - Used internally */
    void *group;

    /** Previously sent sequence number */
    uint16_t _prev_seq_num;

    /** @cond INTERNAL_HIDDEN */
    /** Internally used list node */
    sys_snode_t _node;
    /** @endcond */
};

/** @brief Stream operation. */
struct bt_bap_stream_ops {
    /**
     * @brief Stream configured callback
     *
     * Configured callback is called whenever an Audio Stream has been configured.
     *
     * @param stream Stream object that has been configured.
     * @param pref   Remote QoS preferences.
     */
    void (*configured)(struct bt_bap_stream *stream, const struct bt_bap_qos_cfg_pref *pref);

    /**
     * @brief Stream QoS set callback
     *
     * QoS set callback is called whenever an Audio Stream Quality of Service has been set or
     * updated.
     *
     * @param stream Stream object that had its QoS updated.
     */
    void (*qos_set)(struct bt_bap_stream *stream);

    /**
     * @brief Stream enabled callback
     *
     * Enabled callback is called whenever an Audio Stream has been enabled.
     *
     * @param stream Stream object that has been enabled.
     */
    void (*enabled)(struct bt_bap_stream *stream);

    /**
     * @brief Stream metadata updated callback
     *
     * Metadata Updated callback is called whenever an Audio Stream's metadata has been
     * updated.
     *
     * @param stream Stream object that had its metadata updated.
     */
    void (*metadata_updated)(struct bt_bap_stream *stream);

    /**
     * @brief Stream disabled callback
     *
     * Disabled callback is called whenever an Audio Stream has been disabled.
     *
     * @param stream Stream object that has been disabled.
     */
    void (*disabled)(struct bt_bap_stream *stream);

    /**
     * @brief Stream released callback
     *
     * Released callback is called whenever a Audio Stream has been released and can be
     * deallocated.
     *
     * @param stream Stream object that has been released.
     */
    void (*released)(struct bt_bap_stream *stream);

    /**
     * @brief Stream started callback
     *
     * Started callback is called whenever an Audio Stream has been started
     * and will be usable for streaming.
     *
     * @param stream Stream object that has been started.
     */
    void (*started)(struct bt_bap_stream *stream);

    /**
     * @brief Stream stopped callback
     *
     * Stopped callback is called whenever an Audio Stream has been stopped.
     *
     * @param stream Stream object that has been stopped.
     * @param reason BT_HCI_ERR_* reason for the disconnection.
     */
    void (*stopped)(struct bt_bap_stream *stream, uint8_t reason);

    /**
     * @brief Stream audio HCI receive callback.
     *
     * This callback is only used if the ISO data path is HCI.
     *
     * @param stream Stream object.
     * @param info   Pointer to the metadata for the buffer. The lifetime of the pointer is
     *               linked to the lifetime of the net_buf. Metadata such as sequence number and
     *               timestamp can be provided by the bluetooth controller.
     * @param buf    Buffer containing incoming audio data.
     */
    void (*recv)(struct bt_bap_stream *stream, const struct bt_iso_recv_info *info,
                 const uint8_t *data, uint16_t len);

    /**
     * @brief Stream audio HCI sent callback
     *
     * This callback will be called once the controller marks the SDU
     * as completed. When the controller does so is implementation
     * dependent. It could be after the SDU is enqueued for transmission,
     * or after it is sent on air or flushed.
     *
     * This callback is only used if the ISO data path is HCI.
     *
     * @param stream Stream object.
     */
    void (*sent)(struct bt_bap_stream *stream, void *user_data);

    /**
     * @brief Isochronous channel connected callback
     *
     * If this callback is provided it will be called whenever the isochronous channel for the
     * stream has been connected. This does not mean that the stream is ready to be used, which
     * is indicated by the @ref bt_bap_stream_ops.started callback.
     *
     * If the stream shares an isochronous channel with another stream, then this callback may
     * still be called, without the stream going into the started state.
     *
     * @param stream Stream object.
     */
    void (*connected)(struct bt_bap_stream *stream);

    /**
     * @brief Isochronous channel disconnected callback
     *
     * If this callback is provided it will be called whenever the isochronous channel is
     * disconnected, including when a connection gets rejected.
     *
     * If the stream shares an isochronous channel with another stream, then this callback may
     * not be called, even if the stream is leaving the streaming state.
     *
     * @param stream Stream object.
     * @param reason BT_HCI_ERR_* reason for the disconnection.
     */
    void (*disconnected)(struct bt_bap_stream *stream, uint8_t reason);
};

/** Structure for registering Unicast Server */
struct bt_bap_unicast_server_register_param {
    /**
     * @brief Sink Count to register.
     *
     * Should be in range 0 to @kconfig{CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT}
     */
    uint8_t snk_cnt;

    /** @brief Source Count to register.
     *
     * Should be in range 0 to @kconfig{CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT}
     */
    uint8_t src_cnt;
};

/**
 * @brief Register Audio callbacks for a stream.
 *
 * Register Audio callbacks for a stream.
 *
 * @param stream Stream object.
 * @param ops    Stream operations structure.
 */
void bt_bap_stream_cb_register_safe(struct bt_bap_stream *stream, struct bt_bap_stream_ops *ops);

/**
 * @brief Configure Audio Stream
 *
 * This procedure is used by a client to configure a new stream using the
 * remote endpoint, local capability and codec configuration.
 *
 * @param conn Connection object
 * @param stream Stream object being configured
 * @param ep Remote Audio Endpoint being configured
 * @param codec_cfg Codec configuration
 *
 * @return Allocated Audio Stream object or NULL in case of error.
 */
int bt_bap_stream_config(struct bt_conn *conn, struct bt_bap_stream *stream, struct bt_bap_ep *ep,
                         struct bt_audio_codec_cfg *codec_cfg);
int bt_bap_stream_config_safe(struct bt_conn *conn, struct bt_bap_stream *stream, struct bt_bap_ep *ep,
                              struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Reconfigure Audio Stream
 *
 * This procedure is used by a unicast client or unicast server to reconfigure
 * a stream to use a different local codec configuration.
 *
 * This can only be done for unicast streams.
 *
 * @param stream Stream object being reconfigured
 * @param codec_cfg Codec configuration
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_stream_reconfig_safe(struct bt_bap_stream *stream, struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Configure Audio Stream QoS
 *
 * This procedure is used by a client to configure the Quality of Service of streams in a unicast
 * group. All streams in the group for the specified @p conn will have the Quality of Service
 * configured. This shall only be used to configure unicast streams.
 *
 * @param conn  Connection object
 * @param group Unicast group object
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_stream_qos(struct bt_conn *conn, struct bt_bap_unicast_group *group);
int bt_bap_stream_qos_safe(struct bt_conn *conn, struct bt_bap_unicast_group *group);

/**
 * @brief Enable Audio Stream
 *
 * This procedure is used by a client to enable a stream.
 *
 * This shall only be called for unicast streams, as broadcast streams will always be enabled once
 * created.
 *
 * @param stream Stream object
 * @param meta Metadata
 * @param meta_len Metadata length
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_stream_enable_safe(struct bt_bap_stream *stream, const uint8_t meta[], size_t meta_len);

/**
 * @brief Change Audio Stream Metadata
 *
 * This procedure is used by a unicast client or unicast server to change the metadata of a stream.
 *
 * @param stream Stream object
 * @param meta Metadata
 * @param meta_len Metadata length
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_stream_metadata_safe(struct bt_bap_stream *stream, const uint8_t meta[], size_t meta_len);

/**
 * @brief Disable Audio Stream
 *
 * This procedure is used by a unicast client or unicast server to disable a stream.
 *
 * This shall only be called for unicast streams, as broadcast streams will
 * always be enabled once created.
 *
 * @param stream Stream object
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_stream_disable_safe(struct bt_bap_stream *stream);

/**
 * @brief Connect unicast audio stream
 *
 * This procedure is used by a unicast client to connect the connected isochronous stream (CIS)
 * associated with the audio stream. If two audio streams share a CIS, then this only needs to be
 * done once for those streams. This can only be done for streams in the QoS configured or enabled
 * states.
 *
 * The bt_bap_stream_ops.connected() callback will be called on the streams once this has finished.
 *
 * This shall only be called for unicast streams, and only as the unicast client
 * (@kconfig{CONFIG_BT_BAP_UNICAST_CLIENT}).
 *
 * @param stream Stream object
 *
 * @retval 0 in case of success
 * @retval -EINVAL if the stream, endpoint, ISO channel or connection is NULL
 * @retval -EBADMSG if the stream or ISO channel is in an invalid state for connection
 * @retval -EOPNOTSUPP if the role of the stream is not @ref BT_HCI_ROLE_CENTRAL
 * @retval -EALREADY if the ISO channel is already connecting or connected
 * @retval -EBUSY if another ISO channel is connecting
 * @retval -ENOEXEC if otherwise rejected by the ISO layer
 */
int bt_bap_stream_connect_safe(struct bt_bap_stream *stream);

/**
 * @brief Start Audio Stream
 *
 * This procedure is used by a unicast client or unicast server to make a stream start streaming.
 *
 * For the unicast client, this will send the receiver start ready command to the unicast server for
 * @ref BT_AUDIO_DIR_SOURCE ASEs. The CIS is required to be connected first by
 * bt_bap_stream_connect() before the command can be sent.
 *
 * For the unicast server, this will execute the receiver start ready command on the unicast server
 * for @ref BT_AUDIO_DIR_SINK ASEs. If the CIS is not connected yet, the stream will go into the
 * streaming state as soon as the CIS is connected.
 *
 * This shall only be called for unicast streams.
 *
 * Broadcast sinks will always be started once synchronized, and broadcast
 * source streams shall be started with bt_bap_broadcast_source_start().
 *
 * @param stream Stream object
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_stream_start_safe(struct bt_bap_stream *stream);

/**
 * @brief Stop Audio Stream
 *
 * This procedure is used by a client to make a stream stop streaming.
 *
 * This shall only be called for unicast streams.
 * Broadcast sinks cannot be stopped.
 * Broadcast sources shall be stopped with bt_bap_broadcast_source_stop().
 *
 * @param stream Stream object
 *
 * @retval 0 Success
 * @retval -EINVAL The @p stream does not have an endpoint or a connection, of the stream's
 *                 connection's role is not @p BT_HCI_ROLE_CENTRAL
 * @retval -EBADMSG The state of the @p stream endpoint is not @ref BT_BAP_EP_STATE_DISABLING
 * @retval -EALREADY The CIS state of the @p is not in a connected state, and thus is already
 *                   stopping
 * @retval -EBUSY The @p stream is busy with another operation
 * @retval -ENOTCONN The @p stream ACL connection is not connected
 * @retval -ENOMEM No memory to send request
 * @retval -ENOEXEC The request was rejected by GATT
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_stream_stop_safe(struct bt_bap_stream *stream);

/**
 * @brief Release Audio Stream
 *
 * This procedure is used by a unicast client or unicast server to release a unicast stream.
 *
 * Broadcast sink streams cannot be released, but can be deleted by bt_bap_broadcast_sink_delete().
 * Broadcast source streams cannot be released, but can be deleted by
 * bt_bap_broadcast_source_delete().
 *
 * @param stream Stream object
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_stream_release_safe(struct bt_bap_stream *stream);

/**
 * @brief Send data to Audio stream without timestamp
 *
 * Send data from buffer to the stream.
 *
 * @note Support for sending must be supported, determined by @kconfig{CONFIG_BT_AUDIO_TX}.
 *
 * @param stream   Stream object.
 * @param buf      Buffer containing data to be sent.
 * @param seq_num  Packet Sequence number. This value shall be incremented for each call to this
 *                 function and at least once per SDU interval for a specific channel.
 *
 * @return Bytes sent in case of success or negative value in case of error.
 */
int bt_bap_stream_send_safe(struct bt_bap_stream *stream, struct net_buf *buf, uint16_t seq_num);

/**
 * @brief Send data to Audio stream with timestamp
 *
 * Send data from buffer to the stream.
 *
 * @note Support for sending must be supported, determined by @kconfig{CONFIG_BT_AUDIO_TX}.
 *
 * @param stream   Stream object.
 * @param buf      Buffer containing data to be sent.
 * @param seq_num  Packet Sequence number. This value shall be incremented for each call to this
 *                 function and at least once per SDU interval for a specific channel.
 * @param ts       Timestamp of the SDU in microseconds (us). This value can be used to transmit
 *                 multiple SDUs in the same SDU interval in a CIG or BIG.
 *
 * @return Bytes sent in case of success or negative value in case of error.
 */
int bt_bap_stream_send_ts_safe(struct bt_bap_stream *stream, struct net_buf *buf, uint16_t seq_num,
                               uint32_t ts);

/**
 * @brief Get ISO transmission timing info for a Basic Audio Profile stream
 *
 * Reads timing information for transmitted ISO packet on an ISO channel.
 * The HCI_LE_Read_ISO_TX_Sync HCI command is used to retrieve this information from the controller.
 *
 * @note An SDU must have already been successfully transmitted on the ISO channel
 * for this function to return successfully.
 * Support for sending must be supported, determined by @kconfig{CONFIG_BT_AUDIO_TX}.
 *
 * @param[in]  stream Stream object.
 * @param[out] info   Transmit info object.
 *
 * @retval 0 on success
 * @retval -EINVAL if the stream is invalid, if the stream is not configured for sending or if it is
 *         not connected with a isochronous stream
 * @retval Any return value from bt_iso_chan_get_tx_sync()
 */
int bt_bap_stream_get_tx_sync_safe(struct bt_bap_stream *stream, struct bt_iso_tx_info *info);

/**
 * @defgroup bt_bap_unicast_server BAP Unicast Server APIs
 * @ingroup bt_bap
 * @{
 */

/** Unicast Server callback structure */
struct bt_bap_unicast_server_cb {
    /**
     * @brief Endpoint config request callback
     *
     * Config callback is called whenever an endpoint is requested to be
     * configured
     *
     * @param[in]  conn      Connection object.
     * @param[in]  ep        Local Audio Endpoint being configured.
     * @param[in]  dir       Direction of the endpoint.
     * @param[in]  codec_cfg Codec configuration.
     * @param[out] stream    Pointer to stream that will be configured for the endpoint.
     * @param[out] pref      Pointer to a QoS preference object that shall be populated with
     *                       values. Invalid values will reject the codec configuration request.
     * @param[out] rsp       Object for the ASE operation response. Only used if the return
     *                       value is non-zero.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    int (*config)(struct bt_conn *conn, const struct bt_bap_ep *ep, enum bt_audio_dir dir,
                  const struct bt_audio_codec_cfg *codec_cfg, struct bt_bap_stream **stream,
                  struct bt_bap_qos_cfg_pref *const pref, struct bt_bap_ascs_rsp *rsp);

    /**
     * @brief Stream reconfig request callback
     *
     * Reconfig callback is called whenever an Audio Stream needs to be
     * reconfigured with different codec configuration.
     *
     * @param[in]  stream    Stream object being reconfigured.
     * @param[in]  dir       Direction of the endpoint.
     * @param[in]  codec_cfg Codec configuration.
     * @param[out] pref      Pointer to a QoS preference object that shall be populated with
     *                       values. Invalid values will reject the codec configuration request.
     * @param[out] rsp       Object for the ASE operation response. Only used if the return
     *                       value is non-zero.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    int (*reconfig)(struct bt_bap_stream *stream, enum bt_audio_dir dir,
                    const struct bt_audio_codec_cfg *codec_cfg,
                    struct bt_bap_qos_cfg_pref *const pref, struct bt_bap_ascs_rsp *rsp);

    /**
     * @brief Stream QoS request callback
     *
     * QoS callback is called whenever an Audio Stream Quality of
     * Service needs to be configured.
     *
     * @param[in]  stream  Stream object being reconfigured.
     * @param[in]  qos     Quality of Service configuration.
     * @param[out] rsp     Object for the ASE operation response. Only used if the return
     *                     value is non-zero.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    int (*qos)(struct bt_bap_stream *stream, const struct bt_bap_qos_cfg *qos,
               struct bt_bap_ascs_rsp *rsp);

    /**
     * @brief Stream Enable request callback
     *
     * Enable callback is called whenever an Audio Stream is requested to be enabled to stream.
     *
     * @param[in]  stream      Stream object being enabled.
     * @param[in]  meta        Metadata entries.
     * @param[in]  meta_len    Length of metadata.
     * @param[out] rsp         Object for the ASE operation response. Only used if the return
     *                         value is non-zero.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    int (*enable)(struct bt_bap_stream *stream, const uint8_t meta[], size_t meta_len,
                  struct bt_bap_ascs_rsp *rsp);

    /**
     * @brief Stream Start request callback
     *
     * Start callback is called whenever an Audio Stream is requested to start streaming.
     *
     * @param[in]  stream Stream object.
     * @param[out] rsp    Object for the ASE operation response. Only used if the return
     *                    value is non-zero.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    int (*start)(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp);

    /**
     * @brief Stream Metadata update request callback
     *
     * Metadata callback is called whenever an Audio Stream is requested to update its metadata.
     *
     * @param[in]  stream       Stream object.
     * @param[in]  meta         Metadata entries.
     * @param[in]  meta_len     Length of metadata.
     * @param[out] rsp          Object for the ASE operation response. Only used if the return
     *                          value is non-zero.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    int (*metadata)(struct bt_bap_stream *stream, const uint8_t meta[], size_t meta_len,
                    struct bt_bap_ascs_rsp *rsp);

    /**
     * @brief Stream Disable request callback
     *
     * Disable callback is called whenever an Audio Stream is requested to disable the stream.
     *
     * @param[in]  stream Stream object being disabled.
     * @param[out] rsp    Object for the ASE operation response. Only used if the return
     *                    value is non-zero.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    int (*disable)(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp);

    /**
     * @brief Stream Stop callback
     *
     * Stop callback is called whenever an Audio Stream is requested to stop streaming.
     *
     * @param[in]  stream Stream object.
     * @param[out] rsp    Object for the ASE operation response. Only used if the return
     *                    value is non-zero.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    int (*stop)(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp);

    /**
     * @brief Stream release callback
     *
     * Release callback is called whenever a new Audio Stream needs to be released and thus
     * deallocated.
     *
     * @param[in]  stream Stream object.
     * @param[out] rsp    Object for the ASE operation response. Only used if the return
     *                    value is non-zero.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    int (*release)(struct bt_bap_stream *stream, struct bt_bap_ascs_rsp *rsp);
};

/**
 * @brief Register the Unicast Server.
 *
 * Register the Unicast Server. Only a single Unicast Server can be registered at any one time.
 * This will register ASCS in the GATT database.
 *
 * @param param  Registration parameters for ascs.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int bt_bap_unicast_server_register_safe(const struct bt_bap_unicast_server_register_param *param);

/**
 * @brief Unregister the Unicast Server.
 *
 * Unregister the Unicast Server.
 * This will unregister ASCS in the GATT database.
 * Before calling this function, any callbacks registered through
 * bt_bap_unicast_server_register_cb() needs to be unregistered with
 * bt_bap_unicast_server_unregister_cb().
 *
 * Calling this function will issue an release operation on any ASE
 * in a non-idle state.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int bt_bap_unicast_server_unregister_safe(void);

/**
 * @brief Register unicast server callbacks.
 *
 * Only one callback structure can be registered, and attempting to
 * registering more than one will result in an error.
 * Prior to calling this function the Unicast Server needs to be
 * registered with bt_bap_unicast_server_register().
 *
 * @param cb  Unicast server callback structure.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_unicast_server_register_cb_safe(const struct bt_bap_unicast_server_cb *cb);

/**
 * @brief Unregister unicast server callbacks.
 *
 * May only unregister a callback structure that has previously been
 * registered by bt_bap_unicast_server_register_cb().
 *
 * Calling this function will issue an release operation on any ASE
 * in a non-idle state.
 *
 * @param cb  Unicast server callback structure.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_unicast_server_unregister_cb_safe(const struct bt_bap_unicast_server_cb *cb);

/**
 * @typedef bt_bap_ep_func_t
 * @brief The callback function called for each endpoint.
 *
 * @param ep The structure object with endpoint info.
 * @param user_data Data to pass to the function.
 */
typedef void (*bt_bap_ep_func_t)(struct bt_bap_ep *ep, void *user_data);

/**
 * @brief Iterate through all endpoints of the given connection.
 *
 * @param conn Connection object
 * @param func Function to call for each endpoint.
 * @param user_data Data to pass to the callback function.
 */
void bt_bap_unicast_server_foreach_ep(struct bt_conn *conn, bt_bap_ep_func_t func, void *user_data);

/**
 * @brief Initialize and configure a new ASE.
 *
 * @param conn Connection object
 * @param stream Configured stream object to be attached to the ASE
 * @param codec_cfg Codec configuration
 * @param qos_pref Audio Stream Quality of Service Preference
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_unicast_server_config_ase(struct bt_conn *conn, struct bt_bap_stream *stream,
                                     struct bt_audio_codec_cfg *codec_cfg,
                                     const struct bt_bap_qos_cfg_pref *qos_pref);
int bt_bap_unicast_server_config_ase_safe(struct bt_conn *conn, struct bt_bap_stream *stream,
                                          struct bt_audio_codec_cfg *codec_cfg,
                                          const struct bt_bap_qos_cfg_pref *qos_pref);

/** @} */ /* End of group bt_bap_unicast_server */

/**
 * @defgroup bt_bap_unicast_client BAP Unicast Client APIs
 * @ingroup bt_bap
 * @{
 */

/** Parameter struct for each stream in the unicast group */
struct bt_bap_unicast_group_stream_param {
    /** Pointer to a stream object. */
    struct bt_bap_stream *stream;

    /** The QoS settings for the stream object. */
    struct bt_bap_qos_cfg *qos;
};

/**
 * @brief Parameter struct for the unicast group functions
 *
 * Parameter struct for the bt_bap_unicast_group_create() and
 * bt_bap_unicast_group_add_streams() functions.
 */
struct bt_bap_unicast_group_stream_pair_param {
    /** Pointer to a receiving stream parameters. */
    struct bt_bap_unicast_group_stream_param *rx_param;

    /** Pointer to a transmitting stream parameters. */
    struct bt_bap_unicast_group_stream_param *tx_param;
};

/** Parameters for the creating unicast groups with bt_bap_unicast_group_create() */
struct bt_bap_unicast_group_param {
    /** The number of parameters in @p params */
    size_t params_count;

    /** Array of stream parameters */
    struct bt_bap_unicast_group_stream_pair_param *params;

    /**
     * @brief Unicast Group packing mode.
     *
     * @ref BT_ISO_PACKING_SEQUENTIAL or @ref BT_ISO_PACKING_INTERLEAVED.
     *
     * @note This is a recommendation to the controller, which the controller may ignore.
     */
    uint8_t packing;

    /**
     * @brief Central to Peripheral flush timeout
     *
     * The flush timeout in multiples of ISO_Interval for each payload sent
     * from the Central to Peripheral.
     *
     * Value range from @ref BT_ISO_FT_MIN to @ref BT_ISO_FT_MAX
     */
    uint8_t c_to_p_ft;

    /**
     * @brief Peripheral to Central flush timeout
     *
     * The flush timeout in multiples of ISO_Interval for each payload sent
     * from the Peripheral to Central.
     *
     * Value range from @ref BT_ISO_FT_MIN to @ref BT_ISO_FT_MAX.
     */
    uint8_t p_to_c_ft;

    /**
     * @brief ISO interval
     *
     * Time between consecutive CIS anchor points.
     *
     * Value range from @ref BT_ISO_ISO_INTERVAL_MIN to @ref BT_ISO_ISO_INTERVAL_MAX.
     */
    uint16_t iso_interval;
};

/**
 * @brief Create unicast group.
 *
 * Create a new audio unicast group with one or more audio streams as a unicast client.
 * All streams shall share the same framing.
 * All streams in the same direction shall share the same interval and latency (see
 * @ref bt_bap_qos_cfg).
 *
 * @param[in]  param          The unicast group create parameters.
 * @param[out] unicast_group  Pointer to the unicast group created.
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_unicast_group_create_safe(struct bt_bap_unicast_group_param *param,
                                     struct bt_bap_unicast_group **unicast_group);

/**
 * @brief Reconfigure unicast group.
 *
 * Reconfigure a unicast group with one or more audio streams as a unicast client.
 * All streams shall share the same framing.
 * All streams in the same direction shall share the same interval and latency (see
 * @ref bt_bap_qos_cfg).
 * All streams in @p param shall already belong to @p unicast_group.
 * Use bt_bap_unicast_group_add_streams() to add additional streams.
 *
 * @param unicast_group  Pointer to the unicast group created.
 * @param param          The unicast group reconfigure parameters.
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_unicast_group_reconfig_safe(struct bt_bap_unicast_group *unicast_group,
                                       const struct bt_bap_unicast_group_param *param);

/**
 * @brief Add streams to a unicast group as a unicast client
 *
 * This function can be used to add additional streams to a  bt_bap_unicast_group.
 *
 * This can be called at any time before any of the streams in the group has been started
 * (see bt_bap_stream_ops.started()).
 * This can also be called after the streams have been stopped (see bt_bap_stream_ops.stopped()).
 *
 * Once a stream has been added to a unicast group, it cannot be removed. To remove a stream from a
 * group, the group must be deleted with bt_bap_unicast_group_delete(), but this will require all
 * streams in the group to be released first.
 *
 * @param unicast_group  Pointer to the unicast group
 * @param params         Array of stream parameters with streams being added to the group.
 * @param num_param      Number of parameters in @p params.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_unicast_group_add_streams_safe(struct bt_bap_unicast_group *unicast_group,
                                          struct bt_bap_unicast_group_stream_pair_param params[],
                                          size_t num_param);

/**
 * @brief Delete audio unicast group.
 *
 * Delete a audio unicast group as a client. All streams in the group shall
 * be in the idle or configured state.
 *
 * @param unicast_group  Pointer to the unicast group to delete
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_unicast_group_delete_safe(struct bt_bap_unicast_group *unicast_group);

/** Callback function for bt_bap_unicast_group_foreach_stream()
 *
 * @param stream     The audio stream
 * @param user_data  User data
 *
 * @retval true Stop iterating.
 * @retval false Continue iterating.
 */
typedef bool (*bt_bap_unicast_group_foreach_stream_func_t)(struct bt_bap_stream *stream,
                                                           void *user_data);

/**
 * @brief Iterate through all streams in a unicast group
 *
 * @param unicast_group  The unicast group
 * @param func           The callback function
 * @param user_data      User specified data that sent to the callback function
 *
 * @retval 0 Success (even if no streams exists in the group).
 * @retval -ECANCELED Iteration was stopped by the callback function before complete.
 * @retval -EINVAL @p unicast_group or @p func were NULL.
 */
int bt_bap_unicast_group_foreach_stream_safe(struct bt_bap_unicast_group *unicast_group,
                                             bt_bap_unicast_group_foreach_stream_func_t func,
                                             void *user_data);

/** Structure holding information of audio stream endpoint */
struct bt_bap_unicast_group_info {
    /** Presentation delay for sink ASEs
     *
     * Will be @ref BT_BAP_PD_UNSET if no sink ASEs have been QoS configured
     */
    uint32_t sink_pd;

    /** Presentation delay for source ASEs
     *
     * Will be @ref BT_BAP_PD_UNSET if no source ASEs have been QoS configured
     */
    uint32_t source_pd;
};

/**
 * @brief Return structure holding information of unicast group
 *
 * @param unicast_group The unicast group object.
 * @param info          The structure object to be filled with the info.
 *
 * @retval 0 Success
 * @retval -EINVAL  @p unicast_group or @p info are NULL
 */
int bt_bap_unicast_group_get_info_safe(const struct bt_bap_unicast_group *unicast_group,
                                       struct bt_bap_unicast_group_info *info);

/** Unicast Client callback structure */
struct bt_bap_unicast_client_cb {
    /**
     * @brief Remote Unicast Server Audio Locations
     *
     * This callback is called whenever the audio locations is read from
     * the server or otherwise notified to the client.
     *
     * @param conn  Connection to the remote unicast server.
     * @param dir   Direction of the location.
     * @param loc   The location bitfield value.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    void (*location)(struct bt_conn *conn, enum bt_audio_dir dir, enum bt_audio_location loc);

    /**
     * @brief Remote Unicast Server Available Contexts
     *
     * This callback is called whenever the available contexts are read
     * from the server or otherwise notified to the client.
     *
     * @param conn     Connection to the remote unicast server.
     * @param snk_ctx  The sink context bitfield value.
     * @param src_ctx  The source context bitfield value.
     *
     * @return 0 in case of success or negative value in case of error.
     */
    void (*available_contexts)(struct bt_conn *conn, enum bt_audio_context snk_ctx,
                               enum bt_audio_context src_ctx);

    /**
     * @brief Callback function for bt_bap_stream_config() and bt_bap_stream_reconfig().
     *
     * Called when the codec configure operation is completed on the server.
     *
     * @param stream   Stream the operation was performed on.
     * @param rsp_code Response code.
     * @param reason   Reason code.
     */
    void (*config)(struct bt_bap_stream *stream, enum bt_bap_ascs_rsp_code rsp_code,
                   enum bt_bap_ascs_reason reason);

    /**
     * @brief Callback function for bt_bap_stream_qos().
     *
     * Called when the QoS configure operation is completed on the server.
     * This will be called for each stream in the group that was being QoS
     * configured.
     *
     * @param stream   Stream the operation was performed on. May be NULL if there is no stream
     *                 associated with the ASE ID sent by the server.
     * @param rsp_code Response code.
     * @param reason   Reason code.
     */
    void (*qos)(struct bt_bap_stream *stream, enum bt_bap_ascs_rsp_code rsp_code,
                enum bt_bap_ascs_reason reason);

    /**
     * @brief Callback function for bt_bap_stream_enable().
     *
     * Called when the enable operation is completed on the server.
     *
     * @param stream   Stream the operation was performed on. May be NULL if there is no stream
     *                 associated with the ASE ID sent by the server.
     * @param rsp_code Response code.
     * @param reason   Reason code.
     */
    void (*enable)(struct bt_bap_stream *stream, enum bt_bap_ascs_rsp_code rsp_code,
                   enum bt_bap_ascs_reason reason);

    /**
     * @brief Callback function for bt_bap_stream_start().
     *
     * Called when the start operation is completed on the server. This will
     * only be called if the stream supplied to bt_bap_stream_start() is
     * for a @ref BT_AUDIO_DIR_SOURCE endpoint.
     *
     * @param stream   Stream the operation was performed on. May be NULL if there is no stream
     *                 associated with the ASE ID sent by the server.
     * @param rsp_code Response code.
     * @param reason   Reason code.
     */
    void (*start)(struct bt_bap_stream *stream, enum bt_bap_ascs_rsp_code rsp_code,
                  enum bt_bap_ascs_reason reason);

    /**
     * @brief Callback function for bt_bap_stream_stop().
     *
     * Called when the stop operation is completed on the server. This will
     * only be called if the stream supplied to bt_bap_stream_stop() is
     * for a @ref BT_AUDIO_DIR_SOURCE endpoint.
     *
     * @param stream   Stream the operation was performed on. May be NULL if there is no stream
     *                 associated with the ASE ID sent by the server.
     * @param rsp_code Response code.
     * @param reason   Reason code.
     */
    void (*stop)(struct bt_bap_stream *stream, enum bt_bap_ascs_rsp_code rsp_code,
                 enum bt_bap_ascs_reason reason);

    /**
     * @brief Callback function for bt_bap_stream_disable().
     *
     * Called when the disable operation is completed on the server.
     *
     * @param stream   Stream the operation was performed on. May be NULL if there is no stream
     *                 associated with the ASE ID sent by the server.
     * @param rsp_code Response code.
     * @param reason   Reason code.
     */
    void (*disable)(struct bt_bap_stream *stream, enum bt_bap_ascs_rsp_code rsp_code,
                    enum bt_bap_ascs_reason reason);

    /**
     * @brief Callback function for bt_bap_stream_metadata().
     *
     * Called when the metadata operation is completed on the server.
     *
     * @param stream   Stream the operation was performed on. May be NULL if there is no stream
     *                 associated with the ASE ID sent by the server.
     * @param rsp_code Response code.
     * @param reason   Reason code.
     */
    void (*metadata)(struct bt_bap_stream *stream, enum bt_bap_ascs_rsp_code rsp_code,
                     enum bt_bap_ascs_reason reason);

    /**
     * @brief Callback function for bt_bap_stream_release().
     *
     * Called when the release operation is completed on the server.
     *
     * @param stream   Stream the operation was performed on. May be NULL if there is no stream
     *                 associated with the ASE ID sent by the server.
     * @param rsp_code Response code.
     * @param reason   Reason code.
     */
    void (*release)(struct bt_bap_stream *stream, enum bt_bap_ascs_rsp_code rsp_code,
                    enum bt_bap_ascs_reason reason);

    /**
     * @brief Remote Published Audio Capability (PAC) record discovered
     *
     * Called when a PAC record has been discovered as part of the discovery procedure.
     *
     * The @p codec is only valid while in the callback, so the values must be stored by the
     * receiver if future use is wanted.
     *
     * @param conn      Connection to the remote unicast server.
     * @param dir       The type of remote endpoints and capabilities discovered.
     * @param codec_cap Remote capabilities.
     *
     * If discovery procedure has complete both @p codec and @p ep are set to NULL.
     */
    void (*pac_record)(struct bt_conn *conn, enum bt_audio_dir dir,
                       const struct bt_audio_codec_cap *codec_cap);

    /**
     * @brief Remote Audio Stream Endpoint (ASE) discovered
     *
     * Called when an ASE has been discovered as part of the discovery procedure.
     *
     * @param conn     Connection to the remote unicast server.
     * @param dir      The type of remote endpoints and capabilities discovered.
     * @param ep       Remote endpoint.
     *
     * If discovery procedure has complete both @p codec and @p ep are set to NULL.
     */
    void (*endpoint)(struct bt_conn *conn, enum bt_audio_dir dir, struct bt_bap_ep *ep);

    /**
     * @brief BAP discovery callback function.
     *
     * If discovery procedure has completed @p ep is set to NULL and @p err is 0.
     *
     * @param conn     Connection to the remote unicast server.
     * @param err      Error value. 0 on success, GATT error on positive value or errno on
     *                 negative value.
     * @param dir      The type of remote endpoints and capabilities discovered.
     *
     * If discovery procedure has complete both @p codec and @p ep are set to NULL.
     */
    void (*discover)(struct bt_conn *conn, int err, enum bt_audio_dir dir);

    /** @cond INTERNAL_HIDDEN */
    /** Internally used field for list handling */
    sys_snode_t _node;
    /** @endcond */
};

/**
 * @brief Register unicast client callbacks.
 *
 * @param cb  Unicast client callback structure to register.
 *
 * @retval 0 Success
 * @retval -EINVAL @p cb is NULL.
 * @retval -EEXIST @p cb is already registered.
 */
int bt_bap_unicast_client_register_cb_safe(struct bt_bap_unicast_client_cb *cb);

/**
 * @brief Unregister unicast client callbacks.
 *
 * @param cb  Unicast client callback structure to unregister.
 *
 * @retval 0 Success
 * @retval -EINVAL @p cb is NULL or @p cb was not registered
 */
int bt_bap_unicast_client_unregister_cb_safe(struct bt_bap_unicast_client_cb *cb);

/**
 * @brief Discover remote capabilities and endpoints
 *
 * This procedure is used by a client to discover remote capabilities and
 * endpoints and notifies via params callback.
 *
 * @param conn   Connection object
 * @param dir    The type of remote endpoints and capabilities to discover.
 */
int bt_bap_unicast_client_discover(struct bt_conn *conn, enum bt_audio_dir dir);
int bt_bap_unicast_client_discover_safe(struct bt_conn *conn, enum bt_audio_dir dir);

/** @} */ /* End of group bt_bap_unicast_client */
/**
 * @brief BAP Broadcast APIs
 * @defgroup bt_bap_broadcast BAP Broadcast  APIs
 * @ingroup bt_bap
 * @{
 */

/** @brief Abstract Broadcast Audio Source Endpoint (BASE) subgroup structure. */
struct bt_bap_base_subgroup;
/** @brief Abstract Broadcast Audio Source Endpoint (BASE) structure. */
struct bt_bap_base;

/** Codec ID structure for a Broadcast Audio Source Endpoint (BASE) */
struct bt_bap_base_codec_id {
    /** Codec ID */
    uint8_t id;
    /** Codec Company ID */
    uint16_t cid;
    /** Codec Company Vendor ID */
    uint16_t vid;
};

/** BIS structure for each BIS in a Broadcast Audio Source Endpoint (BASE) subgroup */
struct bt_bap_base_subgroup_bis {
    /** Unique index of the BIS */
    uint8_t index;
    /** Codec Specific Data length. */
    uint8_t data_len;
    /** Codec Specific Data */
    uint8_t *data;
};

/**
 * @brief Generate a pointer to a BASE from periodic advertising data
 *
 * @param ad The periodic advertising data
 *
 * @retval NULL if the data does not contain a BASE
 * @retval Pointer to a bt_bap_base structure
 */
const struct bt_bap_base *bt_bap_base_get_base_from_ad(const struct bt_data *ad);

/**
 * @brief Get the size of a BASE
 *
 * @param base The BASE pointer
 *
 * @retval -EINVAL if arguments are invalid
 * @retval The size of the BASE
 */
int bt_bap_base_get_size(const struct bt_bap_base *base);

/**
 * @brief Get the presentation delay value of a BASE
 *
 * @param base The BASE pointer
 *
 * @retval -EINVAL if arguments are invalid
 * @retval The 24-bit presentation delay value
 */
int bt_bap_base_get_pres_delay(const struct bt_bap_base *base);

/**
 * @brief Get the subgroup count of a BASE
 *
 * @param base The BASE pointer
 *
 * @retval -EINVAL if arguments are invalid
 * @retval The 8-bit subgroup count value
 */
int bt_bap_base_get_subgroup_count(const struct bt_bap_base *base);

/**
 * @brief Get all BIS indexes of a BASE
 *
 * @param[in]  base        The BASE pointer
 * @param[out] bis_indexes 32-bit BIS index bitfield that will be populated
 *
 * @retval -EINVAL if arguments are invalid
 * @retval 0 on success
 */
int bt_bap_base_get_bis_indexes(const struct bt_bap_base *base, uint32_t *bis_indexes);

/**
 * @brief Iterate on all subgroups in the BASE
 *
 * @param base      The BASE pointer
 * @param func      Callback function. Return true to continue iterating, or false to stop.
 * @param user_data Userdata supplied to @p func
 *
 * @retval -EINVAL if arguments are invalid
 * @retval -ECANCELED if iterating over the subgroups stopped prematurely by @p func
 * @retval 0 if all subgroups were iterated
 */
int bt_bap_base_foreach_subgroup(const struct bt_bap_base *base,
                                 bool (*func)(const struct bt_bap_base_subgroup *subgroup,
                                              void *user_data),
                                 void *user_data);

/**
 * @brief Get the codec ID of a subgroup
 *
 * @param[in]  subgroup The subgroup pointer
 * @param[out] codec_id Pointer to the struct where the results are placed
 *
 * @retval -EINVAL if arguments are invalid
 * @retval 0 on success
 */
int bt_bap_base_get_subgroup_codec_id(const struct bt_bap_base_subgroup *subgroup,
                                      struct bt_bap_base_codec_id *codec_id);

/**
 * @brief Get the codec configuration data of a subgroup
 *
 * @param[in]  subgroup The subgroup pointer
 * @param[out] data     Pointer that will point to the resulting codec configuration data
 *
 * @retval -EINVAL if arguments are invalid
 * @retval 0 on success
 */
int bt_bap_base_get_subgroup_codec_data(const struct bt_bap_base_subgroup *subgroup,
                                        uint8_t **data);

/**
 * @brief Get the codec metadata of a subgroup
 *
 * @param[in]  subgroup The subgroup pointer
 * @param[out] meta     Pointer that will point to the resulting codec metadata
 *
 * @retval -EINVAL if arguments are invalid
 * @retval 0 on success
 */
int bt_bap_base_get_subgroup_codec_meta(const struct bt_bap_base_subgroup *subgroup,
                                        uint8_t **meta);

/**
 * @brief Store subgroup codec data in a @ref bt_audio_codec_cfg
 *
 * @param[in]  subgroup  The subgroup pointer
 * @param[out] codec_cfg Pointer to the struct where the results are placed
 *
 * @retval -EINVAL if arguments are invalid
 * @retval -ENOMEM if the @p codec_cfg cannot store the @p subgroup codec data
 * @retval 0 on success
 */
int bt_bap_base_subgroup_codec_to_codec_cfg(const struct bt_bap_base_subgroup *subgroup,
                                            struct bt_audio_codec_cfg *codec_cfg);

/**
 * @brief Get the BIS count of a subgroup
 *
 * @param subgroup The subgroup pointer
 *
 * @retval -EINVAL if arguments are invalid
 * @retval The 8-bit BIS count value
 */
int bt_bap_base_get_subgroup_bis_count(const struct bt_bap_base_subgroup *subgroup);

/**
 * @brief Get all BIS indexes of a subgroup
 *
 * @param[in]  subgroup    The subgroup pointer
 * @param[out] bis_indexes 32-bit BIS index bitfield that will be populated
 *
 * @retval -EINVAL if arguments are invalid
 * @retval 0 on success
 */
int bt_bap_base_subgroup_get_bis_indexes(const struct bt_bap_base_subgroup *subgroup,
                                         uint32_t *bis_indexes);

/**
 * @brief Iterate on all BIS in the subgroup
 *
 * @param subgroup  The subgroup pointer
 * @param func      Callback function. Return true to continue iterating, or false to stop.
 * @param user_data Userdata supplied to @p func
 *
 * @retval -EINVAL if arguments are invalid
 * @retval -ECANCELED if iterating over the subgroups stopped prematurely by @p func
 * @retval 0 if all BIS were iterated
 */
int bt_bap_base_subgroup_foreach_bis(const struct bt_bap_base_subgroup *subgroup,
                                     bool (*func)(const struct bt_bap_base_subgroup_bis *bis,
                                                  void *user_data),
                                     void *user_data);

/**
 * @brief Store BIS codec configuration data in a @ref bt_audio_codec_cfg
 *
 * This only sets the @ref bt_audio_codec_cfg data and @ref bt_audio_codec_cfg data_len, but is
 * useful to use the BIS codec configuration data with the bt_audio_codec_cfg_* functions.
 *
 * @param[in]  bis       The BIS pointer
 * @param[out] codec_cfg Pointer to the struct where the results are placed
 *
 * @retval -EINVAL if arguments are invalid
 * @retval -ENOMEM if the @p codec_cfg cannot store the @p subgroup codec data
 * @retval 0 on success
 */
int bt_bap_base_subgroup_bis_codec_to_codec_cfg(const struct bt_bap_base_subgroup_bis *bis,
                                                struct bt_audio_codec_cfg *codec_cfg);

/** @} */ /* End of group bt_bap_broadcast */

/**
 * @brief BAP Broadcast Source APIs
 * @defgroup bt_bap_broadcast_source BAP Broadcast Source APIs
 * @ingroup bt_bap_broadcast
 * @{
 */

/**
 * @brief Struct to hold the Broadcast Source callbacks
 *
 * These can be registered for usage with bt_bap_broadcast_source_register_cb().
 */
struct bt_bap_broadcast_source_cb {
    /**
     * @brief The Broadcast Source has started and all of the streams are ready for audio data
     *
     * @param source The started Broadcast Source
     */
    void (*started)(struct bt_bap_broadcast_source *source);

    /**
     * @brief The Broadcast Source has stopped and none of the streams are ready for audio data
     *
     * @param source The stopped Broadcast Source
     * @param reason The reason why the Broadcast Source stopped (see the BT_HCI_ERR_* values)
     */
    void (*stopped)(struct bt_bap_broadcast_source *source, uint8_t reason);

    /** @cond INTERNAL_HIDDEN */
    /** Internally used field for list handling */
    sys_snode_t _node;
    /** @endcond */
};

/**
 * @brief Registers callbacks for Broadcast Sources
 *
 * @param cb Pointer to the callback structure.
 *
 * @retval 0 on success
 * @retval -EINVAL if @p cb is NULL
 * @retval -EEXIST if @p cb is already registered
 */
int bt_bap_broadcast_source_register_cb_safe(struct bt_bap_broadcast_source_cb *cb);

/**
 * @brief Unregisters callbacks for Broadcast Sources
 *
 * @param cb Pointer to the callback structure.
 *
 * @retval 0 on success
 * @retval -EINVAL if @p cb is NULL
 * @retval -ENOENT if @p cb is not registered
 */
int bt_bap_broadcast_source_unregister_cb_safe(struct bt_bap_broadcast_source_cb *cb);

/** Broadcast Source stream parameters */
struct bt_bap_broadcast_source_stream_param {
    /** Audio stream */
    struct bt_bap_stream *stream;

    /**
     * @brief The number of elements in the @p data array.
     *
     * The BIS specific data may be omitted and this set to 0.
     */
    size_t data_len;

    /** BIS Codec Specific Configuration */
    uint8_t *data;
};

/** Broadcast Source subgroup parameters*/
struct bt_bap_broadcast_source_subgroup_param {
    /** The number of parameters in @p stream_params */
    size_t params_count;

    /** Array of stream parameters */
    struct bt_bap_broadcast_source_stream_param *params;

    /** Subgroup Codec configuration. */
    struct bt_audio_codec_cfg *codec_cfg;
};

/** Broadcast Source create parameters */
struct bt_bap_broadcast_source_param {
    /** The number of parameters in @p subgroup_params */
    size_t params_count;

    /** Array of stream parameters */
    struct bt_bap_broadcast_source_subgroup_param *params;

    /** Quality of Service configuration. */
    struct bt_bap_qos_cfg *qos;

    /**
     * @brief Broadcast Source packing mode.
     *
     * @ref BT_ISO_PACKING_SEQUENTIAL or @ref BT_ISO_PACKING_INTERLEAVED.
     *
     * @note This is a recommendation to the controller, which the controller may ignore.
     */
    uint8_t packing;

    /** Whether or not to encrypt the streams. */
    bool encryption;

    /**
     * @brief Broadcast code
     *
     * If the value is a string or a the value is less than 16 octets,
     * the remaining octets shall be 0.
     *
     * Example:
     *   The string "Broadcast Code" shall be
     *   [42 72 6F 61 64 63 61 73 74 20 43 6F 64 65 00 00]
     */
    uint8_t broadcast_code[BT_ISO_BROADCAST_CODE_SIZE];

    /**
     * @brief Immediate Repetition Count
     *
     * The number of times the scheduled payloads are transmitted in a given event.
     *
     * Value range from @ref BT_ISO_IRC_MIN to @ref BT_ISO_IRC_MAX.
     */
    uint8_t irc;

    /**
     * @brief Pre-transmission offset
     *
     * Offset used for pre-transmissions.
     *
     * Value range from @ref BT_ISO_PTO_MIN to @ref BT_ISO_PTO_MAX.
     */
    uint8_t pto;

    /**
     * @brief ISO interval
     *
     * Time between consecutive BIS anchor points.
     *
     * Value range from @ref BT_ISO_ISO_INTERVAL_MIN to @ref BT_ISO_ISO_INTERVAL_MAX.
     */
    uint16_t iso_interval;
};

/**
 * @brief Create audio broadcast source.
 *
 * Create a new audio broadcast source with one or more audio streams.
 *
 * The broadcast source will be visible for scanners once this has been called,
 * and the device will advertise audio announcements.
 *
 * No audio data can be sent until bt_bap_broadcast_source_start() has been called and no audio
 * information (BIGInfo) will be visible to scanners (see @ref bt_le_per_adv_sync_cb).
 *
 * @param[in]  param       Pointer to parameters used to create the broadcast source.
 * @param[out] source      Pointer to the broadcast source created
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_broadcast_source_create_safe(struct bt_bap_broadcast_source_param *param,
                                        struct bt_bap_broadcast_source **source);

/**
 * @brief Reconfigure audio broadcast source.
 *
 * Reconfigure an audio broadcast source with a new codec and codec quality of
 * service parameters. This can only be done when the source is stopped.
 *
 * Since this may modify the Broadcast Audio Source Endpoint (BASE),
 * bt_bap_broadcast_source_get_base() should be called after this to get the new BASE information.
 *
 * If the @p param.params_count is smaller than the number of subgroups that have been created in
 * the Broadcast Source, only the first @p param.params_count subgroups are updated. If a stream
 * exist in a subgroup not part of @p param, then that stream is left as is (i.e. it is not removed;
 * the only way to remove a stream from a Broadcast Source is to recreate the Broadcast Source).
 *
 * @param source      Pointer to the broadcast source
 * @param param       Pointer to parameters used to reconfigure the broadcast source.
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_broadcast_source_reconfig_safe(struct bt_bap_broadcast_source *source,
                                          struct bt_bap_broadcast_source_param *param);

/**
 * @brief Modify the metadata of an audio broadcast source.
 *
 * Modify the metadata an audio broadcast source. This can only be done when the source is started.
 * To update the metadata in the stopped state, use bt_bap_broadcast_source_reconfig().
 *
 * @param source      Pointer to the broadcast source.
 * @param meta        Metadata.
 * @param meta_len    Length of metadata.
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_broadcast_source_update_metadata_safe(struct bt_bap_broadcast_source *source,
                                                 const uint8_t meta[], size_t meta_len);

/**
 * @brief Start audio broadcast source.
 *
 * Start an audio broadcast source with one or more audio streams.
 * The broadcast source will start advertising BIGInfo, and audio data can be streamed.
 *
 * @param source      Pointer to the broadcast source
 * @param adv         Pointer to an extended advertising set with periodic advertising configured.
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_broadcast_source_start(struct bt_bap_broadcast_source *source,
                                  struct bt_le_ext_adv *adv);
int bt_bap_broadcast_source_start_safe(struct bt_bap_broadcast_source *source,
                                       struct bt_le_ext_adv *adv);

/**
 * @brief Stop audio broadcast source.
 *
 * Stop an audio broadcast source.
 * The broadcast source will stop advertising BIGInfo, and audio data can no longer be streamed.
 *
 * @param source      Pointer to the broadcast source
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_broadcast_source_stop_safe(struct bt_bap_broadcast_source *source);

/**
 * @brief Delete audio broadcast source.
 *
 * Delete an audio broadcast source.
 * The broadcast source will stop advertising entirely, and the source can no longer be used.
 *
 * @param source      Pointer to the broadcast source
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_broadcast_source_delete_safe(struct bt_bap_broadcast_source *source);

/**
 * @brief Get the Broadcast Audio Stream Endpoint of a broadcast source
 *
 * This will encode the BASE of a broadcast source into a buffer, that can be used for
 * advertisement. The encoded BASE will thus be encoded as little-endian. The BASE shall be put into
 * the periodic advertising data (see bt_le_per_adv_set_data()).
 *
 * See table 3.15 in the Basic Audio Profile v1.0.1 for the structure.
 *
 * @param source        Pointer to the broadcast source.
 * @param base_buf      Pointer to a buffer where the BASE will be inserted.
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_broadcast_source_get_base_safe(struct bt_bap_broadcast_source *source,
                                          struct net_buf_simple *base_buf);

/**
 * @brief Callback function for bt_bap_broadcast_source_foreach_stream()
 *
 * @param stream     The audio stream
 * @param user_data  User data
 *
 * @retval true  Stop iterating.
 * @retval false Continue iterating.
 */
typedef bool (*bt_bap_broadcast_source_foreach_stream_func_t)(struct bt_bap_stream *stream,
                                                              void *user_data);

/**
 * @brief Iterate through all streams in a broadcast source
 *
 * @param source         The broadcast source
 * @param func           The callback function
 * @param user_data      User specified data that is sent to the callback function
 *
 * @retval 0          Success (even if no streams exists in the broadcast source).
 * @retval -ECANCELED The @p func returned true.
 * @retval -EINVAL    @p source or @p func were NULL.
 */
int bt_bap_broadcast_source_foreach_stream_safe(struct bt_bap_broadcast_source *source,
                                                bt_bap_broadcast_source_foreach_stream_func_t func,
                                                void *user_data);
/** @} */ /* End of bt_bap_broadcast_source */

/**
 * @brief BAP Broadcast Sink APIs
 * @defgroup bt_bap_broadcast_sink BAP Broadcast Sink APIs
 * @ingroup bt_bap_broadcast
 * @{
 */

/** Broadcast Audio Sink callback structure */
struct bt_bap_broadcast_sink_cb {
    /**
     * @brief Broadcast Audio Source Endpoint (BASE) received
     *
     * Callback for when we receive a BASE from a broadcaster after
     * syncing to the broadcaster's periodic advertising.
     *
     * @param sink          Pointer to the sink structure.
     * @param base          Broadcast Audio Source Endpoint (BASE).
     * @param base_size     Size of the @p base
     */
    void (*base_recv)(struct bt_bap_broadcast_sink *sink, const struct bt_bap_base *base,
                      size_t base_size);

    /**
     * @brief Broadcast sink is syncable
     *
     * Called whenever a broadcast sink is not synchronized to audio, but the audio is
     * synchronizable. This is inferred when a BIGInfo report is received.
     *
     * Once this callback has been called, it is possible to call
     * bt_bap_broadcast_sink_sync() to synchronize to the audio stream(s).
     *
     * @param sink          Pointer to the sink structure.
     * @param biginfo       The BIGInfo report.
     */
    void (*syncable)(struct bt_bap_broadcast_sink *sink, const struct bt_iso_biginfo *biginfo);

    /**
     * @brief The Broadcast Sink has started and audio data may be received from all of the
     * streams
     *
     * @param sink The started Broadcast Sink
     */
    void (*started)(struct bt_bap_broadcast_sink *sink);

    /**
     * @brief The Broadcast Sink has stopped and none of the streams will receive audio data
     *
     * @param sink The stopped Broadcast Sink
     * @param reason The reason why the Broadcast Sink stopped (see the BT_HCI_ERR_* values)
     */
    void (*stopped)(struct bt_bap_broadcast_sink *sink, uint8_t reason);

    /** @cond INTERNAL_HIDDEN */
    /** Internally used list node */
    sys_snode_t _node;
    /** @endcond */
};

/**
 * @brief Register Broadcast sink callbacks
 *
 * It is possible to register multiple struct of callbacks, but a single struct can only be
 * registered once.
 * Registering the same callback multiple times is undefined behavior and may break the stack.
 *
 * @param cb  Broadcast sink callback structure.
 *
 * @retval 0 on success
 * @retval -EINVAL if @p cb is NULL
 * @retval -EALREADY if @p cb was already registered
 */
int bt_bap_broadcast_sink_register_cb_safe(struct bt_bap_broadcast_sink_cb *cb);

/**
 * @brief Create a Broadcast Sink from a periodic advertising sync
 *
 * This should only be done after verifying that the periodic advertising sync
 * is from a Broadcast Source.
 *
 * The created Broadcast Sink will need to be supplied to
 * bt_bap_broadcast_sink_sync() in order to synchronize to the broadcast audio.
 *
 * bt_bap_broadcast_sink_cb.pa_synced() will be called with the Broadcast
 * Sink object created if this is successful.
 *
 * @param      pa_sync       Pointer to the periodic advertising sync object.
 * @param      broadcast_id  24-bit broadcast ID.
 * @param[out] sink          Pointer to the Broadcast Sink created.
 *
 * @return 0 in case of success or errno value in case of error.
 */
int bt_bap_broadcast_sink_create_safe(struct bt_le_per_adv_sync *pa_sync, uint32_t broadcast_id,
                                      struct bt_bap_broadcast_sink **sink);

/**
 * @brief Sync to a broadcaster's audio
 *
 * @param sink               Pointer to the sink object from the base_recv callback.
 * @param indexes_bitfield   Bitfield of the BIS index to sync to. To sync to e.g. BIS index 1 and
 *                           2, this should have the value of BIT(1) | BIT(2).
 * @param streams            Stream object pointers to be used for the receiver. If multiple BIS
 *                           indexes shall be synchronized, multiple streams shall be provided.
 * @param broadcast_code     The 16-octet broadcast code. Shall be supplied if the broadcast is
 *                           encrypted (see @ref bt_bap_broadcast_sink_cb.syncable).
 *                           If the value is a string or a the value is less
 *                           than 16 octets, the remaining octets shall be 0.
 *
 *                           Example:
 *                           The string "Broadcast Code" shall be
 *                           [42 72 6F 61 64 63 61 73 74 20 43 6F 64 65 00 00]
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_broadcast_sink_sync_safe(struct bt_bap_broadcast_sink *sink, uint32_t indexes_bitfield,
                                    struct bt_bap_stream *streams[],
                                    const uint8_t broadcast_code[BT_ISO_BROADCAST_CODE_SIZE]);

/**
 * @brief Stop audio broadcast sink.
 *
 * Stop an audio broadcast sink.
 * The broadcast sink will stop receiving BIGInfo, and audio data can no longer be streamed.
 *
 * @param sink      Pointer to the broadcast sink
 *
 * @return Zero on success or (negative) error code otherwise.
 */
int bt_bap_broadcast_sink_stop_safe(struct bt_bap_broadcast_sink *sink);

/**
 * @brief Release a broadcast sink
 *
 * Once a broadcast sink has been allocated after the pa_synced callback, it can be deleted using
 * this function. If the sink has synchronized to any broadcast audio streams, these must first be
 * stopped using bt_bap_stream_stop.
 *
 * @param sink Pointer to the sink object to delete.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_broadcast_sink_delete_safe(struct bt_bap_broadcast_sink *sink);

/** @} */ /* End of group bt_bap_broadcast_sink */

/**
 * @brief Register the Basic Audio Profile Scan Delegator and BASS.
 *
 * Register the scan deligator and Broadcast Audio Scan Service (BASS)
 * dynamically at runtime.
 *
 * Only one set of callbacks can be registered at any one time, and calling this function multiple
 * times will override any previously registered callbacks.
 *
 * @param cb Pointer to the callback struct
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_scan_delegator_register_safe(struct bt_bap_scan_delegator_cb *cb);

/**
 * @brief unregister the Basic Audio Profile Scan Delegator and BASS.
 *
 * Unregister the scan deligator and Broadcast Audio Scan Service (BASS)
 * dynamically at runtime.
 *
 * @return 0 in case of success or negative value in case of error.
 */
int bt_bap_scan_delegator_unregister_safe(void);

/**
 * @brief Set the periodic advertising sync state to syncing
 *
 * Set the periodic advertising sync state for a receive state to syncing,
 * notifying Broadcast Assistants.
 *
 * @param src_id    The source id used to identify the receive state.
 * @param pa_state  The Periodic Advertising sync state to set.
 *
 * @return int    Error value. 0 on success, errno on fail.
 */
int bt_bap_scan_delegator_set_pa_state_safe(uint8_t src_id,
                                            enum bt_bap_pa_state pa_state);

/**
 * @brief Set the sync state of a receive state in the server
 *
 * @param src_id         The source id used to identify the receive state.
 * @param bis_synced     Array of bitfields to set the BIS sync state for each
 *                       subgroup.
 * @return int           Error value. 0 on success, ERRNO on fail.
 */
int bt_bap_scan_delegator_set_bis_sync_state_safe(uint8_t src_id,
                                                  uint32_t bis_synced[CONFIG_BT_BAP_BASS_MAX_SUBGROUPS]);

/** Parameters for bt_bap_scan_delegator_add_src() */
struct bt_bap_scan_delegator_add_src_param {
    /** Periodic Advertiser Address */
    bt_addr_le_t addr;

    /** Advertiser SID */
    uint8_t sid;

    /**
     * @brief Periodic Advertising sync state
     *
     * This will typically be either @ref BT_BAP_PA_STATE_NOT_SYNCED or
     * @ref BT_BAP_PA_STATE_SYNCED.
     */
    enum bt_bap_pa_state pa_state;

    /** The broadcast isochronous group encryption state */
    enum bt_bap_big_enc_state encrypt_state;

    /** The 24-bit broadcast ID */
    uint32_t broadcast_id;

    /** Number of subgroups */
    uint8_t num_subgroups;

    /** Subgroup specific information */
    struct bt_bap_bass_subgroup *subgroups;
};

/**
 * @brief Add a receive state source locally
 *
 * This will notify any connected clients about the new source. This allows them
 * to modify and even remove it.
 *
 * If @kconfig{CONFIG_BT_BAP_BROADCAST_SINK} is enabled, any Broadcast Sink
 * sources are autonomously added.
 *
 * @param param The parameters for adding the new source
 *
 * @return int  errno on failure, or source ID on success.
 */
int bt_bap_scan_delegator_add_src_safe(const struct bt_bap_scan_delegator_add_src_param *param);

/** Parameters for bt_bap_scan_delegator_mod_src() */
struct bt_bap_scan_delegator_mod_src_param {
    /** The periodic adverting sync */
    uint8_t src_id;

    /** The broadcast isochronous group encryption state */
    enum bt_bap_big_enc_state encrypt_state;

    /** The 24-bit broadcast ID */
    uint32_t broadcast_id;

    /** Number of subgroups */
    uint8_t num_subgroups;

    /**
     * @brief Subgroup specific information
     *
     * If a subgroup's metadata_len is set to 0, the existing metadata
     * for the subgroup will remain unchanged
     */
    struct bt_bap_bass_subgroup *subgroups;
};

/**
 * @brief Add a receive state source locally
 *
 * This will notify any connected clients about the new source. This allows them
 * to modify and even remove it.
 *
 * If @kconfig{CONFIG_BT_BAP_BROADCAST_SINK} is enabled, any Broadcast Sink
 * sources are autonomously modified.
 *
 * @param param The parameters for adding the new source
 *
 * @return int  errno on failure, or source ID on success.
 */
int bt_bap_scan_delegator_mod_src_safe(const struct bt_bap_scan_delegator_mod_src_param *param);

/**
 * @brief Remove a receive state source
 *
 * This will remove the receive state. If the receive state periodic advertising
 * is synced, bt_bap_scan_delegator_cb.pa_sync_term_req() will be called.
 *
 * If @kconfig{CONFIG_BT_BAP_BROADCAST_SINK} is enabled, any Broadcast Sink
 * sources are autonomously removed.
 *
 * @param src_id The source ID to remove
 *
 * @return int   Error value. 0 on success, errno on fail.
 */
int bt_bap_scan_delegator_rem_src_safe(uint8_t src_id);

/** Callback function for Scan Delegator receive state search functions
 *
 * @param recv_state The receive state.
 * @param user_data  User data.
 *
 * @retval true to stop iterating. If this is used in the context of
 *         bt_bap_scan_delegator_find_state(), the recv_state will be returned by
 *         bt_bap_scan_delegator_find_state()
 * @retval false to continue iterating
 */
typedef bool (*bt_bap_scan_delegator_state_func_t)(
    const struct bt_bap_scan_delegator_recv_state *recv_state, void *user_data);

/**
 * @brief Iterate through all existing receive states
 *
 * @param func      The callback function
 * @param user_data User specified data that sent to the callback function
 */
void bt_bap_scan_delegator_foreach_state_safe(bt_bap_scan_delegator_state_func_t func,
                                              void *user_data);

/**
 * @brief Find and return a receive state based on a compare function
 *
 * @param func      The compare callback function
 * @param user_data User specified data that sent to the callback function
 *
 * @return The first receive state where the @p func returned true, or NULL
 */
const struct bt_bap_scan_delegator_recv_state *bt_bap_scan_delegator_find_state_safe(
    bt_bap_scan_delegator_state_func_t func, void *user_data);

/******************************** CLIENT API ********************************/

/**
 * @brief Callback function for writes.
 *
 * @param conn    The connection to the peer device.
 * @param err     Error value. 0 on success, GATT error on fail.
 */
typedef void (*bt_bap_broadcast_assistant_write_cb)(struct bt_conn *conn,
                                                    int err);

/**
 * @brief Struct to hold the Basic Audio Profile Broadcast Assistant callbacks
 *
 * These can be registered for usage with bt_bap_broadcast_assistant_register_cb().
 */
struct bt_bap_broadcast_assistant_cb {
    /**
     * @brief Callback function for bt_bap_broadcast_assistant_discover.
     *
     * @param conn              The connection that was used to discover
     *                          Broadcast Audio Scan Service.
     * @param err               Error value. 0 on success,
     *                          GATT error or ERRNO on fail.
     * @param recv_state_count  Number of receive states on the server.
     */
    void (*discover)(struct bt_conn *conn, int err,
                     uint8_t recv_state_count);

    /**
     * @brief Callback function for Broadcast Audio Scan Service client scan results
     *
     * Called when the scanner finds an advertiser that advertises the
     * BT_UUID_BROADCAST_AUDIO UUID.
     *
     * @param info          Advertiser information.
     * @param broadcast_id  24-bit broadcast ID.
     */
    void (*scan)(const struct bt_le_scan_recv_info *info,
                 uint32_t broadcast_id);

    /**
     * @brief Callback function for when a receive state is read or updated
     *
     * Called whenever a receive state is read or updated.
     *
     * @param conn     The connection to the Broadcast Audio Scan Service server.
     * @param err      Error value. 0 on success, GATT error on fail.
     * @param state    The receive state or NULL if the receive state is empty.
     */
    void (*recv_state)(struct bt_conn *conn, int err,
                       const struct bt_bap_scan_delegator_recv_state *state);

    /**
     * @brief Callback function for when a receive state is removed.
     *
     * @param conn     The connection to the Broadcast Audio Scan Service server.
     * @param src_id   The receive state.
     */
    void (*recv_state_removed)(struct bt_conn *conn, uint8_t src_id);

    /**
     * @brief Callback function for bt_bap_broadcast_assistant_scan_start().
     *
     * @param conn    The connection to the peer device.
     * @param err     Error value. 0 on success, GATT error on fail.
     */
    void (*scan_start)(struct bt_conn *conn, int err);

    /**
     * @brief Callback function for bt_bap_broadcast_assistant_scan_stop().
     *
     * @param conn    The connection to the peer device.
     * @param err     Error value. 0 on success, GATT error on fail.
     */
    void (*scan_stop)(struct bt_conn *conn, int err);

    /**
     * @brief Callback function for bt_bap_broadcast_assistant_add_src().
     *
     * @param conn    The connection to the peer device.
     * @param err     Error value. 0 on success, GATT error on fail.
     */
    void (*add_src)(struct bt_conn *conn, int err);

    /**
     * @brief Callback function for bt_bap_broadcast_assistant_mod_src().
     *
     * @param conn    The connection to the peer device.
     * @param err     Error value. 0 on success, GATT error on fail.
     */
    void (*mod_src)(struct bt_conn *conn, int err);

    /**
     * @brief Callback function for bt_bap_broadcast_assistant_set_broadcast_code().
     *
     * @param conn    The connection to the peer device.
     * @param err     Error value. 0 on success, GATT error on fail.
     */
    void (*broadcast_code)(struct bt_conn *conn, int err);

    /**
     * @brief Callback function for bt_bap_broadcast_assistant_rem_src().
     *
     * @param conn    The connection to the peer device.
     * @param err     Error value. 0 on success, GATT error on fail.
     */
    void (*rem_src)(struct bt_conn *conn, int err);

    /** @cond INTERNAL_HIDDEN */
    /** Internally used list node */
    sys_snode_t _node;
    /** @endcond */
};

/**
 * @brief Discover Broadcast Audio Scan Service on the server.
 *
 * Warning: Only one connection can be active at any time; discovering for a
 * new connection, will delete all previous data.
 *
 * @param conn  The connection
 *
 * @retval 0 Success
 * @retval -EINVAL @p conn is NULL
 * @retval -EBUSY Another operation is already in progress for this @p conn
 * @retval -ENOTCONN @p conn is not connected
 * @retval -ENOMEM Could not allocated memory for the request
 * @retval -ENOEXEC Unexpected GATT error
 */
int bt_bap_broadcast_assistant_discover(struct bt_conn *conn);
int bt_bap_broadcast_assistant_discover_safe(struct bt_conn *conn);

/**
 * @brief Scan start for BISes for a remote server.
 *
 * This will let the Broadcast Audio Scan Service server know that this device
 * is actively scanning for broadcast sources.
 * The function can optionally also start scanning, if the caller does not want
 * to start scanning itself.
 *
 * Scan results, if @p start_scan is true, is sent to the
 * bt_bap_broadcast_assistant_scan_cb callback.
 *
 * @param conn          Connection to the Broadcast Audio Scan Service server.
 *                      Used to let the server know that we are scanning.
 * @param start_scan    Start scanning if true. If false, the application should
 *                      enable scan itself.

 * @retval 0 Success
 * @retval -EINVAL @p conn is NULL of if @p conn has not done discovery
 * @retval -EBUSY Another operation is already in progress for this @p conn
 * @retval -EAGAIN Bluetooth has not been enabled.
 * @retval -ENOTCONN @p conn is not connected
 * @retval -ENOMEM Could not allocated memory for the request
 * @retval -ENOEXEC Unexpected scan or GATT error
 */
int bt_bap_broadcast_assistant_scan_start(struct bt_conn *conn, bool start_scan);
int bt_bap_broadcast_assistant_scan_start_safe(struct bt_conn *conn,
                                               bool start_scan);

/**
 * @brief Stop remote scanning for BISes for a server.
 *
 * @param conn   Connection to the server.

 * @retval 0 Success
 * @retval -EINVAL @p conn is NULL of if @p conn has not done discovery
 * @retval -EBUSY Another operation is already in progress for this @p conn
 * @retval -EAGAIN Bluetooth has not been enabled.
 * @retval -ENOTCONN @p conn is not connected
 * @retval -ENOMEM Could not allocated memory for the request
 * @retval -ENOEXEC Unexpected scan or GATT error
 */
int bt_bap_broadcast_assistant_scan_stop(struct bt_conn *conn);
int bt_bap_broadcast_assistant_scan_stop_safe(struct bt_conn *conn);

/**
 * @brief Registers the callbacks used by Broadcast Audio Scan Service client.
 *
 * @param cb    The callback structure.
 *
 * @retval 0 on success
 * @retval -EINVAL if @p cb is NULL
 * @retval -EALREADY if @p cb was already registered
 */
int bt_bap_broadcast_assistant_register_cb_safe(struct bt_bap_broadcast_assistant_cb *cb);

/**
 * @brief Unregisters the callbacks used by the Broadcast Audio Scan Service client.
 *
 * @param cb   The callback structure.
 *
 * @retval 0 on success
 * @retval -EINVAL if @p cb is NULL
 * @retval -EALREADY if @p cb was not registered
 */
int bt_bap_broadcast_assistant_unregister_cb_safe(struct bt_bap_broadcast_assistant_cb *cb);

/** Parameters for adding a source to a Broadcast Audio Scan Service server */
struct bt_bap_broadcast_assistant_add_src_param {
    /** Address of the advertiser. */
    bt_addr_le_t addr;

    /** SID of the advertising set. */
    uint8_t adv_sid;

    /** Whether to sync to periodic advertisements. */
    bool pa_sync;

    /** 24-bit broadcast ID */
    uint32_t broadcast_id;

    /**
     * @brief Periodic advertising interval in milliseconds.
     *
     * BT_BAP_PA_INTERVAL_UNKNOWN if unknown.
     */
    uint16_t pa_interval;

    /** Number of subgroups */
    uint8_t num_subgroups;

    /** Pointer to array of subgroups
     *
     * The @ref bt_bap_bass_subgroup.bis_sync value can be set to BT_BAP_BIS_SYNC_NO_PREF to
     * let the broadcast sink decide which BIS to synchronize to.
     */
    struct bt_bap_bass_subgroup *subgroups;
};

/**
 * @brief Add a source on the server.
 *
 * @param conn          Connection to the server.
 * @param param         Parameter struct.
 *
 * @retval 0 Success
 * @retval -EINVAL @p conn is NULL or %p conn has not done discovery or if @p param is invalid
 * @retval -EBUSY Another operation is already in progress for this @p conn
 * @retval -ENOTCONN @p conn is not connected
 * @retval -ENOMEM Could not allocated memory for the request
 * @retval -ENOEXEC Unexpected scan or GATT error
 */
int bt_bap_broadcast_assistant_add_src(
    struct bt_conn *conn, const struct bt_bap_broadcast_assistant_add_src_param *param);
int bt_bap_broadcast_assistant_add_src_safe(
    struct bt_conn *conn, const struct bt_bap_broadcast_assistant_add_src_param *param);

/** Parameters for modifying a source */
struct bt_bap_broadcast_assistant_mod_src_param {
    /** Source ID of the receive state. */
    uint8_t src_id;

    /** Whether to sync to periodic advertisements. */
    bool pa_sync;

    /**
     * @brief Periodic advertising interval.
     *
     * BT_BAP_PA_INTERVAL_UNKNOWN if unknown.
     */
    uint16_t pa_interval;

    /** Number of subgroups */
    uint8_t num_subgroups;

    /** Pointer to array of subgroups */
    struct bt_bap_bass_subgroup *subgroups;
};

/**
 * @brief Modify a source on the server.
 *
 * @param conn          Connection to the server.
 * @param param         Parameter struct.
 *
 * @retval 0 Success
 * @retval -EINVAL @p conn is NULL or %p conn has not done discovery or if @p param is invalid
 * @retval -EBUSY Another operation is already in progress for this @p conn
 * @retval -ENOTCONN @p conn is not connected
 * @retval -ENOMEM Could not allocated memory for the request
 * @retval -ENOEXEC Unexpected scan or GATT error
 */
int bt_bap_broadcast_assistant_mod_src(
    struct bt_conn *conn, const struct bt_bap_broadcast_assistant_mod_src_param *param);
int bt_bap_broadcast_assistant_mod_src_safe(
    struct bt_conn *conn, const struct bt_bap_broadcast_assistant_mod_src_param *param);

/**
 * @brief Set a broadcast code to the specified receive state.
 *
 * @param conn            Connection to the server.
 * @param src_id          Source ID of the receive state.
 * @param broadcast_code  The broadcast code.
 *
 * @retval 0 Success
 * @retval -EINVAL @p conn is NULL or %p conn has not done discovery or @p src_id is invalid
 * @retval -EBUSY Another operation is already in progress for this @p conn
 * @retval -ENOTCONN @p conn is not connected
 * @retval -ENOMEM Could not allocated memory for the request
 * @retval -ENOEXEC Unexpected scan or GATT error
 */
int bt_bap_broadcast_assistant_set_broadcast_code(
    struct bt_conn *conn, uint8_t src_id,
    const uint8_t broadcast_code[BT_ISO_BROADCAST_CODE_SIZE]);
int bt_bap_broadcast_assistant_set_broadcast_code_safe(
    struct bt_conn *conn, uint8_t src_id,
    const uint8_t broadcast_code[BT_ISO_BROADCAST_CODE_SIZE]);

/**
 * @brief Remove a source from the server.
 *
 * @param conn            Connection to the server.
 * @param src_id          Source ID of the receive state.
 *
 * @retval 0 Success
 * @retval -EINVAL @p conn is NULL or %p conn has not done discovery or @p src_id is invalid
 * @retval -EBUSY Another operation is already in progress for this @p conn
 * @retval -ENOTCONN @p conn is not connected
 * @retval -ENOMEM Could not allocated memory for the request
 * @retval -ENOEXEC Unexpected scan or GATT error
 */
int bt_bap_broadcast_assistant_rem_src(struct bt_conn *conn, uint8_t src_id);
int bt_bap_broadcast_assistant_rem_src_safe(struct bt_conn *conn, uint8_t src_id);

/**
 * @brief Read the specified receive state from the server.
 *
 * @param conn     Connection to the server.
 * @param idx      The index of the receive start (0 up to the value from
 *                 bt_bap_broadcast_assistant_discover_cb)
 *
 * @retval 0 Success
 * @retval -EINVAL @p conn is NULL or %p conn has not done discovery or @p src_id is invalid
 * @retval -EBUSY Another operation is already in progress for this @p conn
 * @retval -ENOTCONN @p conn is not connected
 * @retval -ENOMEM Could not allocated memory for the request
 * @retval -ENOEXEC Unexpected scan or GATT error
 */
int bt_bap_broadcast_assistant_read_recv_state(struct bt_conn *conn, uint8_t idx);
int bt_bap_broadcast_assistant_read_recv_state_safe(struct bt_conn *conn, uint8_t idx);

/** @} */ /* end of bt_bap */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_BAP_ */
