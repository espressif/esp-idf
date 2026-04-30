/*
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_AUDIO_CSIP_API_H_
#define ESP_BLE_AUDIO_CSIP_API_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/audio/audio.h>
#include <zephyr/bluetooth/audio/csip.h>

#include "common/init.h"
#include "common/host.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< Size of the Set Identification Resolving Key (SIRK) */
#define ESP_BLE_AUDIO_CSIP_SIRK_SIZE    BT_CSIP_SIRK_SIZE

/*!< Size of the Resolvable Set Identifier (RSI) */
#define ESP_BLE_AUDIO_CSIP_RSI_SIZE     BT_CSIP_RSI_SIZE

/*!< Accept the request to read the SIRK as plaintext */
#define ESP_BLE_AUDIO_CSIP_READ_SIRK_REQ_RSP_ACCEPT     BT_CSIP_READ_SIRK_REQ_RSP_ACCEPT
/*!< Accept the request to read the SIRK, but return encrypted SIRK */
#define ESP_BLE_AUDIO_CSIP_READ_SIRK_REQ_RSP_ACCEPT_ENC BT_CSIP_READ_SIRK_REQ_RSP_ACCEPT_ENC
/*!< Reject the request to read the SIRK */
#define ESP_BLE_AUDIO_CSIP_READ_SIRK_REQ_RSP_REJECT     BT_CSIP_READ_SIRK_REQ_RSP_REJECT
/*!< SIRK is available only via an OOB procedure */
#define ESP_BLE_AUDIO_CSIP_READ_SIRK_REQ_RSP_OOB_ONLY   BT_CSIP_READ_SIRK_REQ_RSP_OOB_ONLY

/**
 * @brief   Helper to declare bt_data array including RSI.
 *
 * This macro is mainly for creating an array of struct bt_data elements.
 *
 * @param   _rsi    Pointer to the RSI value.
 */
#define ESP_BLE_AUDIO_CSIP_DATA_RSI(_rsi)               BT_CSIP_DATA_RSI(_rsi)

/** Callback structure for the Coordinated Set Identification Service */
typedef struct bt_csip_set_member_cb                esp_ble_audio_csip_set_member_cb_t;

/** Coordinated Set Identification Service instance */
typedef struct bt_csip_set_member_svc_inst          esp_ble_audio_csip_set_member_svc_inst_t;

/** Register structure for Coordinated Set Identification Service */
typedef struct bt_csip_set_member_register_param    esp_ble_audio_csip_set_member_register_param_t;

/** Struct to hold information about a service instance */
typedef struct bt_csip_set_member_set_info          esp_ble_audio_csip_set_member_set_info_t;

/** Struct to hold the Coordinated Set Identification Profile Set Coordinator callbacks */
typedef struct bt_csip_set_coordinator_cb           esp_ble_audio_csip_set_coordinator_cb_t;

/** Information about a specific set */
typedef struct bt_csip_set_coordinator_set_info     esp_ble_audio_csip_set_coordinator_set_info_t;

/** Struct representing a coordinated set instance on a remote device */
typedef struct bt_csip_set_coordinator_csis_inst    esp_ble_audio_csip_set_coordinator_csis_inst_t;

/** Struct representing a remote device as a set member */
typedef struct bt_csip_set_coordinator_set_member   esp_ble_audio_csip_set_coordinator_set_member_t;

/**
 * @brief   Get the service declaration attribute.
 *
 * The first service attribute can be included in any other GATT service.
 *
 * @param   svc_inst    Pointer to the Coordinated Set Identification Service.
 *
 * @return  The first CSIS attribute instance.
 */
void *esp_ble_audio_csip_set_member_svc_decl_get(const esp_ble_audio_csip_set_member_svc_inst_t *svc_inst);

/**
 * @brief   Register a Coordinated Set Identification Service instance.
 *
 * This will register and enable the service and make it discoverable by clients.
 *
 * This shall only be done as a server.
 *
 * @param   param       Coordinated Set Identification Service register parameters.
 * @param   svc_inst    Pointer to the registered Coordinated Set Identification Service.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_member_register(const esp_ble_audio_csip_set_member_register_param_t *param,
                                                 esp_ble_audio_csip_set_member_svc_inst_t **svc_inst);

/**
 * @brief   Unregister a Coordinated Set Identification Service instance.
 *
 * This will unregister and disable the service instance.
 *
 * @param   svc_inst    Pointer to the registered Coordinated Set Identification Service.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_member_unregister(esp_ble_audio_csip_set_member_svc_inst_t *svc_inst);

/**
 * @brief   Set the SIRK of a service instance.
 *
 * @param   svc_inst    Pointer to the registered Coordinated Set Identification Service.
 * @param   sirk        The new SIRK.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_member_sirk(esp_ble_audio_csip_set_member_svc_inst_t *svc_inst,
                                             const uint8_t sirk[ESP_BLE_AUDIO_CSIP_SIRK_SIZE]);

/**
 * @brief   Set a new size and rank for a service instance.
 *
 * This function can be used to dynamically change the size and rank of a service instance.
 * It is important to note that a set cannot have multiple devices with the same rank in a
 * set, and it is up to the caller of this function to ensure that.
 * Similarly, it is important that the size is updated on all devices in the set at the same
 * time.
 *
 * If CONFIG_BT_CSIP_SET_MEMBER_SIZE_NOTIFIABLE is enabled, this will also send a notification
 * to all connected or bonded clients.
 *
 * @param   svc_inst    The service instance.
 * @param   size        The new set size.
 * @param   rank        The new rank. Ignored if the @p svc_inst is not lockable.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_member_set_size_and_rank(esp_ble_audio_csip_set_member_svc_inst_t *svc_inst,
                                                          uint8_t size, uint8_t rank);

/**
 * @brief   Get information about a service instances.
 *
 * @param   svc_inst    The service instance.
 * @param   info        Pointer to a struct to store the information in.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_member_get_info(const esp_ble_audio_csip_set_member_svc_inst_t *svc_inst,
                                                 esp_ble_audio_csip_set_member_set_info_t *info);

/**
 * @brief   Generate the Resolvable Set Identifier (RSI) value.
 *
 * This will generate RSI for given @p svc_inst instance.
 *
 * @param   svc_inst    Pointer to the Coordinated Set Identification Service.
 * @param   rsi         Pointer to the 6-octet newly generated RSI data in little-endian.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_member_generate_rsi(const esp_ble_audio_csip_set_member_svc_inst_t *svc_inst,
                                                     uint8_t rsi[ESP_BLE_AUDIO_CSIP_RSI_SIZE]);

/**
 * @brief   Locks a specific Coordinated Set Identification Service instance on the server.
 *
 * @param   svc_inst    Pointer to the Coordinated Set Identification Service.
 * @param   lock        If true lock the set, if false release the set.
 * @param   force       This argument only have meaning when @p lock is false
 *                      (release) and will force release the lock, regardless of who
 *                      took the lock.

 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_member_lock(esp_ble_audio_csip_set_member_svc_inst_t *svc_inst,
                                             bool lock, bool force);

/**
 * @brief   Initialise the csip_set_coordinator instance for a connection.
 *          This will do a discovery on the device and prepare the instance
 *          for following commands.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_coordinator_discover(uint16_t conn_handle);

/**
 * @brief   Get the set member from a connection pointer.
 *
 * Get the Coordinated Set Identification Profile Set Coordinator pointer
 * from connection handle.
 * Only Set Coordinators that have been initiated can be retrieved.
 *
 * @param   conn_handle Connection handle.
 *
 * @return  Pointer to a Coordinated Set Identification Profile Set Coordinator instance.
 *          NULL if the connection has not been found or has not done discovery yet.
 */
esp_ble_audio_csip_set_coordinator_set_member_t *
esp_ble_audio_csip_set_coordinator_set_member_by_conn(uint16_t conn_handle);

/**
 * @brief   Check if advertising data indicates a set member.
 *
 * @param   sirk        The SIRK of the set to check against.
 * @param   data_type   Type of the advertising data.
 * @param   data        Pointer of the advertising data.
 * @param   data_len    Length of the advertising data.
 *
 * @return  True if the advertising data indicates a set member, false otherwise.
 */
bool esp_ble_audio_csip_set_coordinator_is_set_member(const uint8_t sirk[ESP_BLE_AUDIO_CSIP_SIRK_SIZE],
                                                      uint8_t data_type, const uint8_t *data, uint8_t data_len);

/**
 * @brief   Registers callbacks for csip_set_coordinator.
 *
 * @param   cb  Pointer to the callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_coordinator_register_cb(esp_ble_audio_csip_set_coordinator_cb_t *cb);

/**
 * @brief   Access Coordinated Set devices in an ordered manner as a client.
 *
 * This function will read the lock state of all devices and if all devices are
 * in the unlocked state, then @p cb will be called with the same members as
 * provided by @p members, but where the members are ordered by rank (if present).
 *
 * This procedure only works if all the members have the lock characteristic,
 * and all either has rank = 0 or unique ranks.
 *
 * If any of the members are in the locked state, the procedure will be cancelled.
 *
 * This can only be done on members that are bonded.
 *
 * @param   members     Array of set members to access.
 * @param   count       Number of set members in @p members.
 * @param   set_info    Pointer to the a specific set_info struct, as a member may
 *                      be part of multiple sets.
 * @param   cb          The callback function to be called for each member.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_coordinator_ordered_access(const esp_ble_audio_csip_set_coordinator_set_member_t *members[],
                                                            uint8_t count,
                                                            const esp_ble_audio_csip_set_coordinator_set_info_t *set_info,
                                                            bt_csip_set_coordinator_ordered_access_t cb);

/**
 * @brief   Lock an array of set members.
 *
 * The members will be locked starting from lowest rank going up.
 *
 * @param   members     Array of set members to lock.
 * @param   count       Number of set members in @p members.
 * @param   set_info    Pointer to the a specific set_info struct, as a member
 *                      may be part of multiple sets.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_coordinator_lock(const esp_ble_audio_csip_set_coordinator_set_member_t **members,
                                                  uint8_t count,
                                                  const esp_ble_audio_csip_set_coordinator_set_info_t *set_info);

/**
 * @brief   Release an array of set members.
 *
 * The members will be released starting from highest rank going down.
 *
 * @param   members     Array of set members to lock.
 * @param   count       Number of set members in @p members.
 * @param   set_info    Pointer to the a specific set_info struct, as a member
 *                      may be part of multiple sets.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_audio_csip_set_coordinator_release(const esp_ble_audio_csip_set_coordinator_set_member_t **members,
                                                     uint8_t count,
                                                     const esp_ble_audio_csip_set_coordinator_set_info_t *set_info);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_AUDIO_CSIP_API_H_ */
