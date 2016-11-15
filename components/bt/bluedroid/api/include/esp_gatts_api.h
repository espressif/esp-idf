#ifndef __ESP_GATTS_API_H__
#define __ESP_GATTS_API_H__

#include "bt_types.h"
#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "bta_gatt_api.h"
#include "esp_err.h"

/* GATT Server Data Structure */
/* Server callback function events */
#define ESP_GATTS_REG_EVT                 0
#define ESP_GATTS_READ_EVT                1         
#define ESP_GATTS_WRITE_EVT               2         
#define ESP_GATTS_EXEC_WRITE_EVT          3         
#define ESP_GATTS_MTU_EVT                 4         
#define ESP_GATTS_CONF_EVT                5         
#define ESP_GATTS_UNREG_EVT               6
#define ESP_GATTS_CREATE_EVT              7
#define ESP_GATTS_ADD_INCL_SRVC_EVT       8
#define ESP_GATTS_ADD_CHAR_EVT            9
#define ESP_GATTS_ADD_CHAR_DESCR_EVT      10
#define ESP_GATTS_DELELTE_EVT             11
#define ESP_GATTS_START_EVT               12
#define ESP_GATTS_STOP_EVT                13
#define ESP_GATTS_CONNECT_EVT             14
#define ESP_GATTS_DISCONNECT_EVT          15
#define ESP_GATTS_OPEN_EVT                16
#define ESP_GATTS_CANCEL_OPEN_EVT         17
#define ESP_GATTS_CLOSE_EVT               18
#define ESP_GATTS_LISTEN_EVT              19
#define ESP_GATTS_CONGEST_EVT             20
/* following is extra event */
#define ESP_GATTS_RESPONSE_EVT            21

/* esp_ble_gatts_args_t */
typedef struct {
	esp_gatt_if_t gatt_if;	 /* internal is server_if or client_if */
	esp_gatt_srvc_id_t service_uuid;
	esp_gatt_id_t char_uuid;
	esp_gatt_id_t descr_uuid;
	esp_bt_uuid_t uuid;
	esp_gatt_rsp_t rsp;
	esp_gatt_perm_t perm;
	esp_gatt_char_prop_t property;
	esp_bd_addr_t remote_bda;
	esp_gatt_status_t status;
	uint16_t service_handle;
	uint16_t included_service_handle;
	uint16_t attr_handle;
	uint16_t num_handle;
	uint16_t conn_id;
	uint16_t trans_id;
	bool need_confirm;
	bool is_direct;
	uint16_t app_uuid;
	uint16_t data_len;
	uint8_t data[ESP_GATT_MAX_ATTR_LEN];
} esp_ble_gatts_args_t;


/* esp_ble_gatts_cb_param_t */
typedef union {
	//ESP_GATTS_REG_EVT
	struct gatts_reg_evt_param {
		int status;
		uint16_t gatt_if;
		uint16_t app_id;
	} reg;
	// param for ESP_GATTS_READ_EVT
	struct gatts_read_evt_param {
		uint16_t conn_id;
		uint32_t trans_id;
		esp_bd_addr_t bda;
		uint16_t handle;
		uint16_t offset;
		bool is_long;
	} read;
	// param for ESP_GATTS_WRITE_EVT
	struct gatts_write_evt_param {
		uint16_t conn_id;
		uint32_t trans_id;
		esp_bd_addr_t bda;
		uint16_t handle;
		uint16_t offset;
		bool need_rsp;
		bool is_prep;
		uint16_t len;
		uint8_t value[ESP_GATT_MAX_ATTR_LEN];
	} write;
	// param for ESP_GATTS_EXEC_WRITE_EVT
	struct gatts_exec_write_evt_param {
		uint16_t conn_id;
		uint32_t trans_id;
		esp_bd_addr_t bda;
#define ESP_GATT_PREP_WRITE_CANCEL   0x00
#define ESP_GATT_PREP_WRITE_EXEC     0x01
		uint8_t exec_write_flag;
	} exec_write;
	// param for ESP_GATTS_MTU_EVT
	struct gatts_mtu_evt_param {
		uint16_t conn_id;
		uint16_t mtu;
	} mtu;
	// param for ESP_GATTS_CONF_EVT
	struct gatts_conf_evt_param {
		uint16_t conn_id;
		int status;
	} conf;
	// param for ESP_GATTS_DEREG_EVT, NONE
	// param for ESP_GATTS_CREATE_EVT
	struct gatts_create_evt_param {
		int status;
		uint16_t gatt_if;
		uint16_t service_handle; //handle
		esp_gatt_srvc_id_t service_id; //uuid
	} create;
	// param for ESP_GATTS_ADD_INCL_SRVC_EVT
	struct gatts_add_incl_srvc_evt_param {
		int status;
		uint16_t gatt_if;
		uint16_t attr_handle; //handle
		uint16_t service_handle; //handle
	} add_incl_srvc;
	// param for ESP_GATTS_ADD_CHAR_EVT
	struct gatts_add_char_evt_param {
		int status;
		uint16_t gatt_if;
		uint16_t attr_handle; //handle
		uint16_t service_handle; //handle
		esp_bt_uuid_t char_uuid;
	} add_char;
	// param for ESP_GATTS_ADD_CHAR_DESCR_EVT
	struct gatts_add_char_descr_evt_param {
		int status;
		uint16_t gatt_if;
		uint16_t attr_handle; //handle
		uint16_t service_handle; //handle
		esp_bt_uuid_t char_uuid;
	} add_char_descr;
	// param for ESP_GATTS_DELELTE_EVT
	struct gatts_delete_evt_param {
		int status;
		uint16_t gatt_if;
		uint16_t service_handle; //handle
	} del;
	// param for ESP_GATTS_START_EVT
	struct gatts_start_evt_param {
		int status;
		uint16_t gatt_if;
		uint16_t service_handle; //handle
	} start;
	// param for ESP_GATTS_STOP_EVT
	struct gatts_stop_evt_param {
		int status;
		uint16_t gatt_if;
		uint16_t service_handle; //handle
	} stop;
	// param for ESP_GATTS_CONNECT_EVT
	struct gatts_connect_evt_param {
		uint16_t conn_id;
		uint16_t gatt_if;
		esp_bd_addr_t remote_bda;
		bool is_connected;
	} connect;
	// param for ESP_GATTS_DISCONNECT_EVT
	struct gatts_disconnect_evt_param {
		uint16_t conn_id;
		uint16_t gatt_if;
		esp_bd_addr_t remote_bda;
		bool is_connected;
	} disconnect;
	// param for ESP_GATTS_OPEN_EVT none
	// param for ESP_GATTS_CANCEL_OPEN_EVT none
	// param for ESP_GATTS_CLOSE_EVT none
	// param for ESP_GATTS_LISTEN_EVT none
	// param for ESP_GATTS_CONGEST_EVT
	struct gatts_congest_evt_param {
		uint16_t conn_id;
		bool congested;
	} congest;
	// param for  ESP_GATTS_RESPONSE_EVT
	struct gatts_rsp_evt_param {
		int status;			//response status, 0 is success
		uint16_t handle;	//attribute handle which send response
	} rsp;
} esp_ble_gatts_cb_param_t;

/*******************************************************************************
**
** @function        esp_ble_gatts_register_callback
**
** @brief           This function is called to register application callbacks
**                  with BTA GATTS module.
**
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_register_callback(esp_profile_cb_t callback);



/*******************************************************************************
**
** @function        esp_ble_gatts_app_register
**
** @brief           This function is called to register application identity
**
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_app_register(uint16_t app_id);



/*******************************************************************************
**
** @function        esp_ble_gatts_app_unregister
**
** @brief           un-register with GATT Server.
**
** @param[in]       gatt_if: gatt interface id.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_app_unregister(esp_gatt_if_t gatt_if);


/*******************************************************************************
**
** @function        esp_ble_gatts_create_service
**
** @brief      		Create a service. When service creation is done, a callback
**                  event BTA_GATTS_CREATE_SRVC_EVT is called to report status
**                  and service ID to the profile. The service ID obtained in
**                  the callback function needs to be used when adding included
**                  service and characteristics/descriptors into the service.
**
** @param[in]       gatt_if: gatt interface ID
** @param[in]       service_uuid: service UUID.
** @param[in]       num_handle: numble of handle requessted for this service.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_create_service(esp_gatt_if_t gatt_if,
				esp_gatt_srvc_id_t *service_uuid, uint16_t num_handle);



/*******************************************************************************
**
** @function        esp_ble_gatts_add_include_service
**
** @brief      		This function is called to add an included service. After included
**                  service is included, a callback event BTA_GATTS_ADD_INCL_SRVC_EVT
**                  is reported the included service ID.
**
** @param[in]       service_handle: service handle to which this included service is to
**                  be added.
** @param[in]       included_service_handle: the service ID to be included.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_add_include_service(uint16_t service_handle, uint16_t included_service_handle);



/*******************************************************************************
**
** @function        esp_ble_gatts_add_char
**
** @brief           This function is called to add a characteristic into a service.
**
** @param[in]       service_handle: service handle to which this included service is to
**                  be added.
** @param[in]       char_uuid : Characteristic UUID.
** @param[in]       perm      : Characteristic value declaration attribute permission.
** @param[in]       property  : Characteristic Properties
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_add_char(uint16_t service_handle,  esp_bt_uuid_t  *char_uuid,
                                  esp_gatt_perm_t perm, esp_gatt_char_prop_t property);




/*******************************************************************************
**
** @function        esp_ble_gatts_add_char_descr
**
** @brief           This function is called to add characteristic descriptor. When
**                  it's done, a callback event BTA_GATTS_ADD_DESCR_EVT is called
**                  to report the status and an ID number for this descriptor.
**
** @param[in]       service_handle: service handle to which this charatceristic descriptor is to
**                              be added.
** @param[in]       perm: descriptor access permission.
** @param[in]       descr_uuid: descriptor UUID.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_add_char_descr (uint16_t service_handle,
                                  						esp_bt_uuid_t  *descr_uuid,
                                  						esp_gatt_perm_t perm);



/*******************************************************************************
**
** @function        esp_ble_gatts_delete_service
**
** @brief           This function is called to delete a service. When this is done,
**                  a callback event BTA_GATTS_DELETE_EVT is report with the status.
**
** @param[in]       service_handled: service_handle to be deleted.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_delete_service(uint16_t service_handle);



/*******************************************************************************
**
** @function        esp_ble_gatts_start_service
**
** @brief           This function is called to start a service.
**
** @param[in]       service_handle: the service handle to be started.
** @param[in]       sup_transport: supported trasnport.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_start_service(uint16_t service_handle);



/*******************************************************************************
**
** @function        esp_ble_gatts_stop_service
**
** @brief           This function is called to stop a service.
**
** @param[in]       service_handle - service to be topped.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_stop_service(uint16_t service_handle);



/*******************************************************************************
**
** @function        esp_ble_gatts_send_indicate
**
** @brief           This function is called to read a characteristics descriptor.
**
** @param[in]       conn_id - connection id to indicate.
** @param[in]	    attribute_handle - attribute handle to indicate.
** @param[in]       data_len - indicate data length.
** @param[in]       data: data to indicate.
** @param[in]       need_confirm - if this indication expects a confirmation or not.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_send_indicate(uint16_t conn_id, uint16_t attr_handle,
					uint16_t data_len, uint8_t *data, bool need_confirm);


/*******************************************************************************
**
** @function        esp_ble_gatts_send_rsp
**
** @brief           This function is called to send a response to a request.
**
** @param[in]       conn_id - connection identifier.
** @param[in]       trans_id - transfe id
** @param[in]       status - response status
** @param[in]       rsp - response data.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_send_response(uint16_t conn_id, uint32_t trans_id,
                        			esp_gatt_status_t status, esp_gatt_rsp_t *rsp);


/*******************************************************************************
**
** @function        esp_ble_gatts_open
**
** @brief           Open a direct open connection or add a background auto connection
**                  bd address
**
** @param[in]       gatt_if: application ID.
** @param[in]       remote_bda: remote device BD address.
** @param[in]       is_direct: direct connection or background auto connection
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_open(esp_gatt_if_t gatt_if, esp_bd_addr_t remote_bda, bool is_direct);

/*******************************************************************************
**
** @function        esp_ble_gatts_close
**
** @brief           Close a connection  a remote device.
**
** @param[in]       conn_id: connectino ID to be closed.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gatts_close(uint16_t conn_id);


#endif /* __ESP_GATTS_API_H__ */
