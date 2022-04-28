/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_LOCAL_DATA_OPERATION_API_H_
#define _ESP_BLE_MESH_LOCAL_DATA_OPERATION_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief         Get the model publish period, the unit is ms.
 *
 * @param[in]     model: Model instance pointer.
 *
 * @return        Publish period value on success, 0 or (negative) error code from errno.h on failure.
 *
 */
int32_t esp_ble_mesh_get_model_publish_period(esp_ble_mesh_model_t *model);

/**
 * @brief         Get the address of the primary element.
 *
 * @return        Address of the primary element on success, or
 *                ESP_BLE_MESH_ADDR_UNASSIGNED on failure which means the device has not been provisioned.
 *
 */
uint16_t esp_ble_mesh_get_primary_element_address(void);

/**
 * @brief         Check if the model has subscribed to the given group address.
 *                Note: E.g., once a status message is received and the destination address
 *                      is a group address, the model uses this API to check if it is successfully subscribed
 *                      to the given group address.
 *
 * @param[in]     model: Pointer to the model.
 * @param[in]     group_addr: Group address.
 *
 * @return        Pointer to the group address within the Subscription List of the model on success, or
 *                NULL on failure which means the model has not subscribed to the given group address.
 *                Note: With the pointer to the group address returned, you can reset the group address
 *                      to 0x0000 in order to unsubscribe the model from the group.
 *
 */
uint16_t *esp_ble_mesh_is_model_subscribed_to_group(esp_ble_mesh_model_t *model,
                                                    uint16_t group_addr);

/**
 * @brief         Find the BLE Mesh element pointer via the element address.
 *
 * @param[in]     element_addr: Element address.
 *
 * @return        Pointer to the element on success, or NULL on failure.
 *
 */
esp_ble_mesh_elem_t *esp_ble_mesh_find_element(uint16_t element_addr);

/**
 * @brief         Get the number of elements that have been registered.
 *
 * @return        Number of elements.
 *
 */
uint8_t esp_ble_mesh_get_element_count(void);

/**
 * @brief        Find the Vendor specific model with the given element,
 *               the company ID and the Vendor Model ID.
 *
 * @param[in]    element: Element to which the model belongs.
 * @param[in]    company_id: A 16-bit company identifier assigned by the Bluetooth SIG.
 * @param[in]    model_id: A 16-bit vendor-assigned model identifier.
 *
 * @return       Pointer to the Vendor Model on success, or NULL on failure which means the Vendor Model is not found.
 *
 */
esp_ble_mesh_model_t *esp_ble_mesh_find_vendor_model(const esp_ble_mesh_elem_t *element,
                                                     uint16_t company_id, uint16_t model_id);

/**
 * @brief        Find the SIG model with the given element and Model id.
 *
 * @param[in]    element: Element to which the model belongs.
 * @param[in]    model_id: SIG model identifier.
 *
 * @return       Pointer to the SIG Model on success, or NULL on failure which means the SIG Model is not found.
 *
 */
esp_ble_mesh_model_t *esp_ble_mesh_find_sig_model(const esp_ble_mesh_elem_t *element,
                                                  uint16_t model_id);

/**
 * @brief        Get the Composition data which has been registered.
 *
 * @return       Pointer to the Composition data on success, or NULL on failure which means the Composition data is not initialized.
 *
 */
const esp_ble_mesh_comp_t *esp_ble_mesh_get_composition_data(void);

/**
 * @brief        A local model of node or Provisioner subscribes a group address.
 *
 * @note         This function shall not be invoked before node is provisioned or Provisioner is enabled.
 *
 * @param[in]    element_addr: Unicast address of the element to which the model belongs.
 * @param[in]    company_id: A 16-bit company identifier.
 * @param[in]    model_id: A 16-bit model identifier.
 * @param[in]    group_addr: The group address to be subscribed.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_model_subscribe_group_addr(uint16_t element_addr, uint16_t company_id,
                                                  uint16_t model_id, uint16_t group_addr);

/**
 * @brief        A local model of node or Provisioner unsubscribes a group address.
 *
 * @note         This function shall not be invoked before node is provisioned or Provisioner is enabled.
 *
 * @param[in]    element_addr: Unicast address of the element to which the model belongs.
 * @param[in]    company_id: A 16-bit company identifier.
 * @param[in]    model_id: A 16-bit model identifier.
 * @param[in]    group_addr: The subscribed group address.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_model_unsubscribe_group_addr(uint16_t element_addr, uint16_t company_id,
                                                    uint16_t model_id, uint16_t group_addr);

/**
 * @brief         This function is called by Node to get the local NetKey.
 *
 * @param[in]     net_idx: NetKey index.
 *
 * @return        NetKey on success, or NULL on failure.
 *
 */
const uint8_t *esp_ble_mesh_node_get_local_net_key(uint16_t net_idx);

/**
 * @brief         This function is called by Node to get the local AppKey.
 *
 * @param[in]     app_idx: AppKey index.
 *
 * @return        AppKey on success, or NULL on failure.
 *
 */
const uint8_t *esp_ble_mesh_node_get_local_app_key(uint16_t app_idx);

/**
 * @brief         This function is called by Node to add a local NetKey.
 *
 * @param[in]     net_key: NetKey to be added.
 * @param[in]     net_idx: NetKey Index.
 *
 * @note          This function can only be called after the device is provisioned.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_node_add_local_net_key(const uint8_t net_key[16], uint16_t net_idx);

/**
 * @brief         This function is called by Node to add a local AppKey.
 *
 * @param[in]     app_key: AppKey to be added.
 * @param[in]     net_idx: NetKey Index.
 * @param[in]     app_idx: AppKey Index.
 *
 * @note          The net_idx must be an existing one.
 *                This function can only be called after the device is provisioned.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_node_add_local_app_key(const uint8_t app_key[16], uint16_t net_idx, uint16_t app_idx);

/**
 * @brief         This function is called by Node to bind AppKey to model locally.
 *
 * @param[in]     element_addr: Node local element address
 * @param[in]     company_id: Node local company id
 * @param[in]     model_id: Node local model id
 * @param[in]     app_idx: Node local appkey index
 *
 * @note          If going to bind app_key with local vendor model, the company_id
 *                shall be set to 0xFFFF.
 *                This function can only be called after the device is provisioned.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_node_bind_app_key_to_local_model(uint16_t element_addr, uint16_t company_id,
                                                        uint16_t model_id, uint16_t app_idx);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_LOCAL_DATA_OPERATION_API_H_ */
