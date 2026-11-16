/**
 * @file
 * @brief Bluetooth Audio Stream Control Service (ASCS) APIs.
 */

/*
 * SPDX-FileCopyrightText: 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_ASCS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_ASCS_H_

/**
 * @brief Audio Stream Control Service (ASCS)
 *
 * @defgroup bt_ascs Audio Stream Control Service (ASCS)
 *
 * @since 4.5
 * @version 0.1.0
 *
 * @ingroup bluetooth
 * @{
 *
 * The Audio Stream Control Service (ASCS) exposes Audio Stream Endpoints (ASEs),
 * which enables clients to control the ASEs and their associated unicast audio streams.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/autoconf.h>
#include <zephyr/bluetooth/assigned_numbers.h>
#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/util_macro.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Response Status Code
 *
 * These are sent by the server to the client when a stream operation is
 * requested.
 */
enum bt_bap_ascs_rsp_code {
    /** Server completed operation successfully */
    BT_BAP_ASCS_RSP_CODE_SUCCESS = 0x00U,
    /** Server did not support operation by client */
    BT_BAP_ASCS_RSP_CODE_NOT_SUPPORTED = 0x01U,
    /** Server rejected due to invalid operation length */
    BT_BAP_ASCS_RSP_CODE_INVALID_LENGTH = 0x02U,
    /** Invalid ASE ID */
    BT_BAP_ASCS_RSP_CODE_INVALID_ASE = 0x03U,
    /** Invalid ASE state */
    BT_BAP_ASCS_RSP_CODE_INVALID_ASE_STATE = 0x04U,
    /** Invalid operation for direction */
    BT_BAP_ASCS_RSP_CODE_INVALID_DIR = 0x05U,
    /** Capabilities not supported by server */
    BT_BAP_ASCS_RSP_CODE_CAP_UNSUPPORTED = 0x06U,
    /** Configuration parameters not supported by server */
    BT_BAP_ASCS_RSP_CODE_CONF_UNSUPPORTED = 0x07U,
    /** Configuration parameters rejected by server */
    BT_BAP_ASCS_RSP_CODE_CONF_REJECTED = 0x08U,
    /** Invalid Configuration parameters */
    BT_BAP_ASCS_RSP_CODE_CONF_INVALID = 0x09U,
    /** Unsupported metadata */
    BT_BAP_ASCS_RSP_CODE_METADATA_UNSUPPORTED = 0x0AU,
    /** Metadata rejected by server */
    BT_BAP_ASCS_RSP_CODE_METADATA_REJECTED = 0x0BU,
    /** Invalid metadata */
    BT_BAP_ASCS_RSP_CODE_METADATA_INVALID = 0x0CU,
    /** Server has insufficient resources */
    BT_BAP_ASCS_RSP_CODE_NO_MEM = 0x0DU,
    /** Unspecified error */
    BT_BAP_ASCS_RSP_CODE_UNSPECIFIED = 0x0EU,
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
    BT_BAP_ASCS_REASON_NONE = 0x00U,
    /** Codec ID */
    BT_BAP_ASCS_REASON_CODEC = 0x01U,
    /** Codec configuration */
    BT_BAP_ASCS_REASON_CODEC_DATA = 0x02U,
    /** SDU interval */
    BT_BAP_ASCS_REASON_INTERVAL = 0x03U,
    /** Framing */
    BT_BAP_ASCS_REASON_FRAMING = 0x04U,
    /** PHY */
    BT_BAP_ASCS_REASON_PHY = 0x05U,
    /** Maximum SDU size*/
    BT_BAP_ASCS_REASON_SDU = 0x06U,
    /** Retransmission number */
    BT_BAP_ASCS_REASON_RTN = 0x07U,
    /** Max transport latency */
    BT_BAP_ASCS_REASON_LATENCY = 0x08U,
    /** Presentation delay */
    BT_BAP_ASCS_REASON_PD = 0x09U,
    /** Invalid CIS mapping */
    BT_BAP_ASCS_REASON_CIS = 0x0AU,
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
#define BT_BAP_ASCS_RSP(c, r)                                                                      \
    (struct bt_bap_ascs_rsp)                                                                   \
    {                                                                                          \
        .code = c, .reason = r                                                             \
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
     * Preferred Transport Latency in milliseconds
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
        .unframed_supported = _unframed_supported,                                         \
        .phy = _phy,                                                                       \
        .rtn = _rtn,                                                                       \
        .latency = _latency,                                                               \
        .pd_min = _pd_min,                                                                 \
        .pd_max = _pd_max,                                                                 \
        .pref_pd_min = _pref_pd_min,                                                       \
        .pref_pd_max = _pref_pd_max,                                                       \
    }

/* Temporary solution to handle circular dependencies between ascs.h and bap.h */
struct bt_bap_stream;
struct bt_bap_ep;
struct bt_bap_qos_cfg;

/** ASCS callback structure */
struct bt_ascs_cb {
    /**
     * @brief Endpoint config request callback
     *
     * Config callback is called whenever an endpoint is requested to be configured
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

/** Parameters for registering the Audio Stream Control Service (ASCS) */
struct bt_ascs_register_param {
    /**
     * @brief Sink Count to register.
     *
     * Should be in range 0 to @kconfig{CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT}
     */
    uint8_t snk_cnt;

    /**
     * @brief Source Count to register.
     *
     * Should be in range 0 to @kconfig{CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT}
     */
    uint8_t src_cnt;

    /**
     * @brief Callback structure
     */
    const struct bt_ascs_cb *cb;
};

/**
 * @brief Register the Audio Stream Control Service (ASCS)
 *
 * This will initialize the service and expose it in the GATT database, as well as the provided
 * number of sink and source ASE characteristics.
 *
 * @param param Parameters for the service
 *
 * @retval 0 Success.
 * @retval -EINVAL @p param is NULL or contains invalid values, or bt_gatt_service_register()
 *                failed.
 * @retval -EALREADY ASCS already registered.
 * @retval -EADDRINUSE ISO server is already registered with bt_iso_server_register().
 * @retval -ENOTSUP Controller does not support ISO.
 */
int bt_ascs_register(const struct bt_ascs_register_param *param);

/**
 * @brief Unregister the Audio Stream Control Service (ASCS)
 *
 * This will release all streams and remove the service from the GATT database.
 *
 * @retval 0 Success.
 * @retval -ENOENT if bt_gatt_service_unregister() failed to unregister the service.
 * @retval -EALREADY ASCS already unregistered.
 */
int bt_ascs_unregister(void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_ASCS_H_ */
