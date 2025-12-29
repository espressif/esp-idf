/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_v11_DFD_H__
#define _BLE_MESH_v11_DFD_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_DFD_OP_RECEIVERS_ADD        BLE_MESH_MODEL_OP_2(0x83, 0x11)
#define BLE_MESH_DFD_OP_RECEIVERS_DELETE_ALL BLE_MESH_MODEL_OP_2(0x83, 0x12)
#define BLE_MESH_DFD_OP_RECEIVERS_STATUS     BLE_MESH_MODEL_OP_2(0x83, 0x13)
#define BLE_MESH_DFD_OP_RECEIVERS_GET        BLE_MESH_MODEL_OP_2(0x83, 0x14)
#define BLE_MESH_DFD_OP_RECEIVERS_LIST       BLE_MESH_MODEL_OP_2(0x83, 0x15)
#define BLE_MESH_DFD_OP_CAPABILITIES_GET     BLE_MESH_MODEL_OP_2(0x83, 0x16)
#define BLE_MESH_DFD_OP_CAPABILITIES_STATUS  BLE_MESH_MODEL_OP_2(0x83, 0x17)
#define BLE_MESH_DFD_OP_GET                  BLE_MESH_MODEL_OP_2(0x83, 0x18)
#define BLE_MESH_DFD_OP_START                BLE_MESH_MODEL_OP_2(0x83, 0x19)
#define BLE_MESH_DFD_OP_SUSPEND              BLE_MESH_MODEL_OP_2(0x83, 0x1a)
#define BLE_MESH_DFD_OP_CANCEL               BLE_MESH_MODEL_OP_2(0x83, 0x1b)
#define BLE_MESH_DFD_OP_APPLY                BLE_MESH_MODEL_OP_2(0x83, 0x1c)
#define BLE_MESH_DFD_OP_STATUS               BLE_MESH_MODEL_OP_2(0x83, 0x1d)
#define BLE_MESH_DFD_OP_UPLOAD_GET           BLE_MESH_MODEL_OP_2(0x83, 0x1e)
#define BLE_MESH_DFD_OP_UPLOAD_START         BLE_MESH_MODEL_OP_2(0x83, 0x1f)
#define BLE_MESH_DFD_OP_UPLOAD_START_OOB     BLE_MESH_MODEL_OP_2(0x83, 0x20)
#define BLE_MESH_DFD_OP_UPLOAD_CANCEL        BLE_MESH_MODEL_OP_2(0x83, 0x21)
#define BLE_MESH_DFD_OP_UPLOAD_STATUS        BLE_MESH_MODEL_OP_2(0x83, 0x22)
#define BLE_MESH_DFD_OP_FW_GET               BLE_MESH_MODEL_OP_2(0x83, 0x23)
#define BLE_MESH_DFD_OP_FW_GET_BY_INDEX      BLE_MESH_MODEL_OP_2(0x83, 0x24)
#define BLE_MESH_DFD_OP_FW_DELETE            BLE_MESH_MODEL_OP_2(0x83, 0x25)
#define BLE_MESH_DFD_OP_FW_DELETE_ALL        BLE_MESH_MODEL_OP_2(0x83, 0x26)
#define BLE_MESH_DFD_OP_FW_STATUS            BLE_MESH_MODEL_OP_2(0x83, 0x27)

/**
 * @defgroup bt_mesh_dfd Firmware Distribution models
 * @ingroup bt_mesh
 * @{
 */

/** Firmware distribution status. */
enum bt_mesh_dfd_status {
    /** The message was processed successfully. */
    BLE_MESH_DFD_SUCCESS,

    /** Insufficient resources on the node. */
    BLE_MESH_DFD_ERR_INSUFFICIENT_RESOURCES,

    /** The operation cannot be performed while the Server is in the current
     *  phase.
     */
    BLE_MESH_DFD_ERR_WRONG_PHASE,

    /** An internal error occurred on the node. */
    BLE_MESH_DFD_ERR_INTERNAL,

    /** The requested firmware image is not stored on the Distributor. */
    BLE_MESH_DFD_ERR_FW_NOT_FOUND,

    /** The AppKey identified by the AppKey Index is not known to the node.
     */
    BLE_MESH_DFD_ERR_INVALID_APPKEY_INDEX,

    /** There are no Target nodes in the Distribution Receivers List
     *  state.
     */
    BLE_MESH_DFD_ERR_RECEIVERS_LIST_EMPTY,

    /** Another firmware image distribution is in progress. */
    BLE_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION,

    /** Another upload is in progress. */
    BLE_MESH_DFD_ERR_BUSY_WITH_UPLOAD,

    /** The URI scheme name indicated by the Update URI is not supported. */
    BLE_MESH_DFD_ERR_URI_NOT_SUPPORTED,

    /** The format of the Update URI is invalid. */
    BLE_MESH_DFD_ERR_URI_MALFORMED,

    /** The URI is currently unreachable. */
    BLE_MESH_DFD_ERR_URI_UNREACHABLE,

    /** The Check Firmware OOB procedure did not find any new firmware. */
    BLE_MESH_DFD_ERR_NEW_FW_NOT_AVAILABLE,

    /** The suspension of the Distribute Firmware procedure failed. */
    BLE_MESH_DFD_ERR_SUSPEND_FAILED,
};

/** Firmware distribution phases. */
enum bt_mesh_dfd_phase {
    /** No firmware distribution is in progress. */
    BLE_MESH_DFD_PHASE_IDLE,

    /** Firmware distribution is in progress. */
    BLE_MESH_DFD_PHASE_TRANSFER_ACTIVE,

    /** The Transfer BLOB procedure has completed successfully. */
    BLE_MESH_DFD_PHASE_TRANSFER_SUCCESS,

    /** The Apply Firmware on Target Nodes procedure is being executed. */
    BLE_MESH_DFD_PHASE_APPLYING_UPDATE,

    /** The Distribute Firmware procedure has completed successfully. */
    BLE_MESH_DFD_PHASE_COMPLETED,

    /** The Distribute Firmware procedure has failed. */
    BLE_MESH_DFD_PHASE_FAILED,

    /** The Cancel Firmware Update procedure is being executed. */
    BLE_MESH_DFD_PHASE_CANCELING_UPDATE,

    /** The Transfer BLOB procedure is suspended. */
    BLE_MESH_DFD_PHASE_TRANSFER_SUSPENDED,
};

/** Firmware upload phases. */
enum bt_mesh_dfd_upload_phase {
    /** No firmware upload is in progress. */
    BLE_MESH_DFD_UPLOAD_PHASE_IDLE,

    /** The Store Firmware procedure is being executed. */
    BLE_MESH_DFD_UPLOAD_PHASE_TRANSFER_ACTIVE,

    /** The Store Firmware procedure or Store Firmware OOB procedure failed.
     */
    BLE_MESH_DFD_UPLOAD_PHASE_TRANSFER_ERROR,

    /** The Store Firmware procedure or the Store Firmware OOB procedure
     *  completed successfully.
     */
    BLE_MESH_DFD_UPLOAD_PHASE_TRANSFER_SUCCESS,
};

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_v11_DFD_H__ */
