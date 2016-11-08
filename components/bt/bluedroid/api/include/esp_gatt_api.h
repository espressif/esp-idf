#ifndef __ESP_GATT_API_H__
#define __ESP_GATT_API_H__

#include "bt_types.h"
#include "esp_bt_defs.h"
#include "bta_gatt_api.h"
#include "bt_prf_sys.h"


/*******************************************************************************
**
** @function         esp_ble_update_conn_params
**
** @brief           Update connection parameters, can only be used when connection is up.
**
** @param[in]      bd_addr   - BD address of the peer
** @param[in]      min_int   -     minimum connection interval, [0x0004~ 0x4000]
** @param[in]      max_int   -     maximum connection interval, [0x0004~ 0x4000]
** @param[in]      latency   -     slave latency [0 ~ 500]
** @param[in]      timeout   -     supervision timeout [0x000a ~ 0xc80]
**
** @return          None
**
*******************************************************************************/
void esp_ble_update_conn_params (BD_ADDR bd_addr, uint16_t min_int, 
												uint16_t max_int, uint16_t latency, uint16_t timeout);


/*******************************************************************************
**
** @function        esp_ble_set_pkt_data_len
**
** @brief           This function is to set maximum LE data packet size
**
** @return          None
**
**
*******************************************************************************/
void esp_ble_set_pkt_data_len (BD_ADDR remote_device, uint16_t tx_data_length);



/*******************************************************************************
**
** @function        esp_ble_set_rand_addr
**
** @brief           This function set the random address for the appliction
**
** @param[in]       rand_addr: the random address whith should be setting
** 
** @return          None
**
**
*******************************************************************************/
void esp_ble_set_rand_addr (BD_ADDR rand_addr);



/*******************************************************************************
**
** @function        esp_ble_config_local_privacy
**
** @brief           Enable/disable privacy on the local device
**
** @param[in]       privacy_enable   - enable/disabe privacy on remote device.
**
** @return          None
**
*******************************************************************************/
void esp_ble_config_local_privacy (BOOLEAN privacy_enable);	



/*******************************************************************************
** @function		esp_gattc_disable
** @brief			This function is called by application to disable GATTC module
**
** @param			NULL      	
**
** @return       	None.
**
*******************************************************************************/
void esp_gattc_disable(void);


/*******************************************************************************
**
** @function        esp_ble_gattc_app_register
**
** @brief	      	This function is called to register application callbacks
**                  with GATTC module.
**
** @param[in]       app_uuid  - applicaiton UUID
** @param[in]       client_cb - pointer to the application callback function.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_app_register(esp_bt_uuid_t *app_uuid, esp_gattc_cb_t *client_cb);


/*******************************************************************************
**
** @function        esp_ble_gattc_app_unregister
**
** @brief      		This function is called to unregister an application
**                  from GATTC module.
**
** @param[in]       client_if - client interface identifier.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_app_unregister(esp_gattc_if_t client_if);


/*******************************************************************************
**
** @function        esp_ble_gattc_conn
**
** @brief      		Open a direct connection or add a background auto connection
**                  bd address
**
** @param[in]       client_if: server interface.
** @param[in]       remote_bda: remote device BD address.
** @param[in]       is_direct: direct connection or background auto connection
**                  
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_conn(esp_gattc_if_t client_if, BD_ADDR remote_bda, BOOLEAN is_direct);



/*******************************************************************************
**
** @function        esp_ble_gattc_cancel_conn
**
** @brief      		Cancel a direct open connection or remove a background auto connection
**                  bd address
**
** @param[in]       client_if: server interface.
** @param[in]       remote_bda: remote device BD address.
** @param[in]       is_direct: direct connection or background auto connection
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_cancel_conn (esp_gattc_if_t client_if, BD_ADDR remote_bda, BOOLEAN is_direct);


/*******************************************************************************
**
** @function        esp_ble_gattc_close
**
** @brief      		Close a connection to a GATT server.
**
** @param[in]       conn_id: connectino ID to be closed.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_close (uint16_t conn_id);



/*******************************************************************************
**
** @function        esp_ble_gattc_config_mtu
**
** @brief      		Configure the MTU size in the GATT channel. This can be done
**                  only once per connection.
**
** @param[in]       conn_id: connection ID.
**                  mtu: desired MTU size to use.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the mtu value invalid
**
*******************************************************************************/
esp_gatt_status_t esp_ble_gattc_config_mtu (uint16_t conn_id, uint16_t mtu);


/*******************************************************************************
**
** @function        esp_ble_gattc_svc_search_req
**
** @brief      		This function is called to request a GATT service discovery
**                  on a GATT server. This function report service search result
**                  by a callback event, and followed by a service search complete
**                  event.
**
** @param[in]       conn_id: connection ID.
** @param[in]       srvc_uuid: a UUID of the service application is interested in.
**                  If Null, discover for all services.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_svc_search_req (uint16_t conn_id, esp_bt_uuid_t *srvc_uuid);


/****************************************************************************************************
**
** @function        esp_ble_gattc_get_first_char
**
** @brief      		This function is called to find the first characteristic of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       srvc_id: the service ID of which the characteristic is belonged to.
** @param[in]       char_uuid_cond: Characteristic UUID, if NULL find the first available
**                  characteristic.
** @param[in]       char_result: output parameter which will store the GATT
**                  characteristic ID.
** @param[in]       property: output parameter to carry the characteristic property.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the srvc_id or char_result parameter is NULL.
**
*****************************************************************************************************/
esp_gatt_status_t  esp_ble_gattc_get_first_char (uint16_t conn_id, esp_gatt_srvc_id_t *srvc_id,
                                          						   esp_bt_uuid_t *char_uuid_cond,
                                          						   esp_gattc_char_id_t *char_result,
                                          						   esp_gatt_char_prop_t *property);


/*******************************************************************************
**
** @function        esp_ble_gattc_get_first_char_descr
**
** @brief      		This function is called to find the first characteristic descriptor of the
**                  characteristic on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       char_id: the characteristic ID of which the descriptor is belonged to.
** @param[in] 		descr_uuid_cond: Characteristic Descr UUID, if NULL find the first available
**                  characteristic.
** @param[in]       descr_result: output parameter which will store the GATT
**                 	characteristic descriptor ID.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the char_id or descr_result parameter is NULL.
**
*******************************************************************************/
esp_gatt_status_t  esp_ble_gattc_get_first_char_descr (uint16_t conn_id, esp_gattc_char_id_t *char_id,
                                                esp_bt_uuid_t *descr_uuid_cond,
                                                esp_gattc_char_descr_id_t *descr_result);


/*******************************************************************************
**
** @function        esp_ble_gattc_get_next_inclu_srvc
**
** @brief	      	This function is called to find the next included service of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       start_id: start the search from the next record
**                  after the one identified by p_start_id.
** @param[in]       uuid_cond: Included service UUID, if NULL find the first available
**                  included service.
** @param[in]       result: output parameter which will store the GATT ID
**                  of the included service found.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the p_char_id or p_descr_result parameter is NULL.

*******************************************************************************/
esp_gatt_status_t  esp_ble_gattc_get_next_inclu_srvc (uint16_t conn_id,
                                                   esp_gattc_incl_srvc_id_t *start_id,
                                                   esp_bt_uuid_t            *uuid_cond,
                                                   esp_gattc_incl_srvc_id_t *result);


/*******************************************************************************
**
** @function        esp_ble_gattc_get_next_char
**
** @brief      		This function is called to find the next characteristic of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       start_char_id: start the characteristic search from the next record
**                  after the one identified by char_id.
** @param[in]       char_uuid_cond: Characteristic UUID, if NULL find the first available
**                  characteristic.
** @param[in]       char_result: output parameter which will store the GATT
**                  characteristic ID.
** @param[in]       property: output parameter to carry the characteristic property.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the start_char_id or char_result parameter is NULL.
** 
*******************************************************************************/
esp_gatt_status_t  esp_ble_gattc_get_next_char (uint16_t conn_id,
                                         esp_gattc_char_id_t *start_char_id,
                                         esp_bt_uuid_t       *char_uuid_cond,
                                         esp_gattc_char_id_t *char_result,
                                         esp_gatt_char_prop_t    *property);


/*******************************************************************************
**
** @function        esp_ble_gattc_get_next_char_descr
**
** @brief      		This function is called to find the next characteristic descriptor
**                  of the characterisctic.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]       start_descr_id: start the descriptor search from the next record
**                  after the one identified by p_start_descr_id.
** @param[in]       descr_uuid_cond: Characteristic descriptor UUID, if NULL find
**                               the first available characteristic descriptor.
** @param[in]       descr_result: output parameter which will store the GATT
**                                  characteristic descriptor ID.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the start_descr_id or descr_result parameter is NULL.
**
*******************************************************************************/
esp_gatt_status_t  esp_ble_gattc_get_next_char_descr (uint16_t conn_id,
                                             esp_gattc_char_descr_id_t *start_descr_id,
                                             esp_bt_uuid_t             *descr_uuid_cond,
                                             esp_gattc_char_descr_id_t *descr_result);


/*******************************************************************************
**
** @function        esp_ble_gattc_get_first_inclu_srvc
**
** @brief      		This function is called to find the first included service of the
**                  service on the given server.
**
** @param[in]       conn_id: connection ID which identify the server.
** @param[in]      	srvc_id: the service ID of which the characteristic is belonged to.
** @param[in]       uuid_cond: Characteristic UUID, if NULL find the first available
**                  characteristic.
** @param[in]       result: output parameter which will store the GATT ID
**                  of the included service found.
**
** @return          Command status code:
**  - @ref ESP_GATT_OK: If request succeeds
**  - @ref ESP_GATT_ILLEGAL_PARAMETER: If the srvc_id or result parameter is NULL.
**
*******************************************************************************/
esp_gatt_status_t  esp_ble_gattc_get_first_inclu_srvc (uint16_t conn_id, esp_gatt_srvc_id_t *srvc_id,
                                                    esp_bt_uuid_t *uuid_cond, esp_gattc_incl_srvc_id_t *result);


/*******************************************************************************
**
** @function        esp_ble_gattc_read_char
**
** @brief      	    This function is called to read a service's characteristics of
**                  the given characteritisc ID.
**
** @param[in]       conn_id - connectino ID.
** @param[in]       char_id - characteritic ID to read.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_read_char (uint16_t conn_id, esp_gattc_char_id_t *char_id,
                                  esp_gatt_auth_req_t auth_req);



/*******************************************************************************
**
** @function        esp_ble_gattc_read_char_descr
**
** @brief	        This function is called to read a characteristics descriptor.
**
** @param[in]       conn_id - connection ID.
** @param[in]       descr_id - characteritic descriptor ID to read.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_read_char_descr (uint16_t conn_id,
                             							 esp_gattc_char_descr_id_t  *descr_id,
                              							 esp_gatt_auth_req_t auth_req);



/*******************************************************************************
**
** @function        esp_ble_gattc_read_multi
**
** @brief      		This function is called to read multiple characteristic or
**                  characteristic descriptors.
**
** @param[in]       conn_id - connectino ID.
** @param[in]       read_multi - pointer to the read multiple parameter.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_read_multi (uint16_t conn_id, esp_gattc_multi_t *read_multi,
                            esp_gatt_auth_req_t auth_req);



/*******************************************************************************
**
** @function        esp_ble_gattc_write_char_val
**
** @brief      		This function is called to write characteristic value.
**
** @param[in]       conn_id - connection ID.
** @param[in]       char_id - characteristic ID to write.
** @param[in]       write_type - type of write.
** @param[in]       len: length of the data to be written.
** @param[in]       value - the value to be written.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_write_char_val ( uint16_t conn_id,
                               							 esp_gattc_char_id_t *char_id,
                                						 esp_gattc_write_type_t  write_type,
                                						 uint16_t len,
                                						 uint8_t *value,
                                						 esp_gatt_auth_req_t auth_req);


/*******************************************************************************
**
** @function        esp_ble_gattc_write_char_descr
**
** @brief	      	This function is called to write characteristic descriptor value.
**
** @param[in]       conn_id - connection ID
** @param[in]       char_descr_id - characteristic descriptor ID to write.
** @param[in]       write_type - write type.
** @param[in]       data - the value to be written.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_write_char_descr (uint16_t conn_id,
                               esp_gattc_char_descr_id_t *char_descr_id,
                               esp_gattc_write_type_t  write_type,
                               esp_gatt_unfmt_t      *data,
                               esp_gatt_auth_req_t auth_req);



/*******************************************************************************
**
** @function        esp_ble_gattc_prepa_write
**
** @brief      		This function is called to prepare write a characteristic value.
**
** @param[in]       conn_id - connection ID.
** @param[in]       char_id - GATT characteritic ID of the service.
** @param[in]       offset - offset of the write value.
** @param[in]       len: length of the data to be written.
** @param[in]       value - the value to be written.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_prepa_write (uint16_t conn_id, esp_gattc_char_id_t *char_id,
                              						uint16_t offset, uint16_t len, uint8_t *value,
                              						esp_gatt_auth_req_t auth_req);



/*******************************************************************************
**
** @function        esp_ble_gattc_execu_write
**
** @brief           This function is called to execute write a prepare write sequence.
**
** @param[in]       conn_id - connection ID.
** @param[in]       is_execute - execute or cancel.
**
** @return          None
**
*******************************************************************************/
void  esp_ble_gattc_execu_write (uint16_t conn_id, BOOLEAN is_execute);



/*******************************************************************************
**
** @function        esp_ble_gattc_send_ind_cfm
**
** @brief           This function is called to send handle value confirmation.
**
** @param[in]       conn_id - connection ID.
** @param[in]       char_id - characteristic ID to confirm.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gattc_send_ind_cfm (uint16_t conn_id, esp_gattc_char_id_t *char_id);




/*******************************************************************************
**
** @function        esp_ble_gattc_register_ntf
**
** @brief           This function is called to register for notification of a service.
**
** @param[in]       client_if - client interface.
** @param[in]       bda - target GATT server.
** @param[in]       char_id - pointer to GATT characteristic ID.
**
** @return          OK if registration succeed, otherwise failed.
**
*******************************************************************************/
esp_gatt_status_t esp_ble_gattc_register_ntf (esp_gattc_if_t client_if,
                                                     BD_ADDR bda,
                                                     esp_gattc_char_id_t *char_id);



/*******************************************************************************
**
** @function        esp_ble_gattc_unregister_ntf
**
** @brief           This function is called to de-register for notification of a service.
**
** @param[in]       client_if - client interface.
** @param[in]       bda - target GATT server.
** @param[in]       char_id - pointer to GATT characteristic ID.
**
** @return          OK if deregistration succeed, otherwise failed.
**
*******************************************************************************/
tBTA_GATT_STATUS esp_ble_gattc_unregister_ntf (esp_gattc_if_t client_if,
                                                       BD_ADDR bda,
                                                       esp_gattc_char_id_t *char_id);


/*******************************************************************************
**
** @function        esp_ble_gatts_disale
**
** @brief           This function is called to disable GATTS module
**
** @param[in]       None.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_disale(void);


 

/*******************************************************************************
**
** @function        esp_ble_gatts_app_register
**
** @brief           This function is called to register application callbacks
**                  with BTA GATTS module.
**
** @param[in]       app_uuid - applicaiton UUID
** @param[in]       cback - pointer to the application callback function.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_app_register(esp_bt_uuid_t *app_uuid, esp_gatts_cb_t *cback);



/*******************************************************************************
**
** @function        esp_ble_gatts_app_unregister
**
** @brief           un-register with GATT Server.
**
** @param[in]       server_if: service interface id.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_app_unregister(esp_gatts_if_t server_if);



/*******************************************************************************
**
** @function        esp_ble_gatts_create_srvc
**
** @brief      		Create a service. When service creation is done, a callback
**                  event BTA_GATTS_CREATE_SRVC_EVT is called to report status
**                  and service ID to the profile. The service ID obtained in
**                  the callback function needs to be used when adding included
**                  service and characteristics/descriptors into the service.
**
** @param[in]       server_if: Profile ID this service is belonged to.
** @param[in]       service_uuid: service UUID.
** @param[in]       inst: instance ID number of this service.
** @param[in]       num_handle: numble of handle requessted for this service.
** @param[in]       is_primary: is this service a primary one or not.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_create_srvc(esp_gatts_if_t server_if, esp_bt_uuid_t *service_uuid, uint8_t inst,
                             uint16_t num_handle, BOOLEAN is_primary);



/*******************************************************************************
**
** @function        esp_ble_gatts_add_inclu_srvc
**
** @brief      		This function is called to add an included service. After included
**                  service is included, a callback event BTA_GATTS_ADD_INCL_SRVC_EVT
**                  is reported the included service ID.
**
** @param[in]       service_id: service ID to which this included service is to
**                  be added.
** @param[in]       included_service_id: the service ID to be included.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_add_inclu_srvc (uint16_t service_id, uint16_t included_service_id);



/*******************************************************************************
**
** @function        esp_ble_gatts_add_char
**
** @brief           This function is called to add a characteristic into a service.
**
** @param[in]       service_id: service ID to which this included service is to
**                  be added.
** @param[in]       char_uuid : Characteristic UUID.
** @param[in]       perm      : Characteristic value declaration attribute permission.
** @param[in]       property  : Characteristic Properties
**
** @return             None
**
*******************************************************************************/
void esp_ble_gatts_add_char (uint16_t service_id,  esp_bt_uuid_t  *char_uuid,
                                  esp_gatt_perm_t perm, esp_gatt_char_prop_t property);




/*******************************************************************************
**
** @function        esp_ble_gatts_add_char_descr
**
** @brief           This function is called to add characteristic descriptor. When
**                  it's done, a callback event BTA_GATTS_ADD_DESCR_EVT is called
**                  to report the status and an ID number for this descriptor.
**
** @param[in]       service_id: service ID to which this charatceristic descriptor is to
**                              be added.
** @param[in]       perm: descriptor access permission.
** @param[in]       descr_uuid: descriptor UUID.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_add_char_descr (uint16_t service_id,
                                  						esp_gatt_perm_t perm,
                                  						esp_bt_uuid_t  * descr_uuid);



/*******************************************************************************
**
** @function        esp_ble_gatts_dele_srvc
**
** @brief           This function is called to delete a service. When this is done,
**                  a callback event BTA_GATTS_DELETE_EVT is report with the status.
**
** @param[in]       service_id: service_id to be deleted.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_dele_srvc (uint16_t service_id);



/*******************************************************************************
**
** @function        esp_ble_gatts_start_srvc
**
** @brief           This function is called to start a service.
**
** @param[in]       service_id: the service ID to be started.
** @param[in]       sup_transport: supported trasnport.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_start_srvc(uint16_t service_id);



/*******************************************************************************
**
** @function        esp_ble_gatts_stop_srvc
**
** @brief           This function is called to stop a service.
**
** @param[in]       service_id - service to be topped.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_stop_srvc(uint16_t service_id);



/*******************************************************************************
**
** @function        esp_ble_gatts_hdl_val_indica
**
** @brief           This function is called to read a characteristics descriptor.
**
** @param[in]       bda - remote device bd address to indicate.
** @param[in]	    attr_id - attribute ID to indicate.
** @param[in]       data_len - indicate data length.
** @param[in]       data: data to indicate.
** @param[in]       need_confirm - if this indication expects a confirmation or not.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_hdl_val_indica (uint16_t conn_id, uint16_t attr_id, uint16_t data_len,
                                      								uint8_t *data, BOOLEAN need_confirm);


/*******************************************************************************
**
** @function        esp_ble_gatts_send_rsp
**
** @brief           This function is called to send a response to a request.
**
** @param[in]       conn_id - connection identifier.
** @param[in]       trans_id - transaction ID.
** @param[in]       status - response status
** @param[in]       msg - response data.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_send_rsp (uint16_t conn_id, uint32_t trans_id,
                        			esp_gatt_status_t status, esp_gatts_rsp_t *msg);


/*******************************************************************************
**
** @function        esp_ble_gatts_conn
**
** @brief           Open a direct open connection or add a background auto connection
**                  bd address
**
** @param[in]       server_if: server interface.
** @param[in]       remote_bda: remote device BD address.
** @param[in]       is_direct: direct connection or background auto connection
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_conn (esp_gatts_if_t server_if, BD_ADDR remote_bda, BOOLEAN is_direct);


/*******************************************************************************
**
** @function         esp_ble_gatts_cancel_conn
**
** @brief           Cancel a direct open connection or remove a background auto connection
**                  bd address
**
** @param[in]       server_if: server interface.
** @param[in]       remote_bda: remote device BD address.
** @param[in]       is_direct: direct connection or background auto connection
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_cancel_conn (esp_gatts_if_t server_if, BD_ADDR remote_bda, BOOLEAN is_direct);


/*******************************************************************************
**
** @function        esp_ble_gatts_close
**
** @brief           Close a connection  a remote device.
**
** @param[in]       conn_id: connectino ID to be closed.
**
** @return          None
**
*******************************************************************************/
void esp_ble_gatts_close(uint16_t conn_id);


/*******************************************************************************
**
** @function        esp_prf_app_register
**
** @brief           This function is called to register application callbacks
**                  with BTA GATTS module.
**
** @param[in]       prf_id - the profile identification
** @param[in]       p_cback - pointer to the application callback function.
**
** @return          None
**
*******************************************************************************/
void esp_prf_app_register(uint8_t prf_id, void *p_cback);


#endif /* __ESP_GATT_API_H__ */
