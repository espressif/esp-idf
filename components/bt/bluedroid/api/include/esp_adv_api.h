#ifndef __ESP_ADV_API_H__
#define __ESP_ADV_API_H__

#include "bt_types.h"
#include "esp_bt_defs.h"
#include "bta_gatt_api.h"
#include "bt_prf_sys.h"


/*******************************************************************************
**
** @function        esp_ble_config_adv_data
**
** @brief           This function is called to override the BTA default ADV parameters.
**
** @param[in]       adv_data: Pointer to User defined ADV data structure. This
**                  memory space can not be freed until p_adv_data_cback
**                  is received.
** @param[in|out]   adv_data_cback: set adv data complete callback.
**
** @return          None
**
*******************************************************************************/
void esp_ble_config_adv_data (esp_ble_adv_data_cfg_t *adv_data,
												     esp_ble_set_adv_data_cmpl_cb_t *adv_data_cback);


/*******************************************************************************
**
** Function         esp_ble_set_scan_rsp
**
** Description      This function is called to override the app scan response.
**
** Parameters       Pointer to User defined ADV data structure
**
** Returns          None
**
*******************************************************************************/
void esp_ble_set_scan_rsp(esp_ble_adv_data_cfg_t *scan_rsp_data,
												 esp_ble_set_adv_data_cmpl_cb_t *p_scan_rsp_data_cback);


/*******************************************************************************
**
** @function        esp_ble_set_scan_params
**
** @brief           This function is called to set scan parameters
**
** @param[in]       esp_ble_scan_params: Pointer to User defined scan_params data structure. This
**                  memory space can not be freed until scan_param_setup_cback
** @param[in]       client_if: Client interface ID
** @param[in|out]   scan_param_setup_status_cback - Set scan param status callback
**
** @return          None
**
*******************************************************************************/
void esp_ble_set_scan_params (esp_ble_scan_params *scan_params, esp_gattc_if_t client_if,
                            		            esp_scan_param_setup_cb_t scan_param_setup_cback);


/*******************************************************************************
**
** @function        esp_ble_start_scanning
**
** @brief           This procedure keep the device scanning the peer device whith advertising on the air 
**
** @param[in]       duration: Keeping the scaning time, the unit is second.
** @param[in|out]	results_cb: The scanning function callback when the peer device has been scan 
**					which advertising on the air
**
** @return          None
**
*******************************************************************************/
void esp_ble_start_scanning (UINT8 duration, esp_dm_search_cb_t *results_cb);



/*******************************************************************************
**
** @function        esp_ble_start_advertising
**
** @brief           This function is called to start advertising.
**
** @param[in]       esp_ble_adv_params_all_t: ointer to User defined adv_params data structure.
**
** @return          None
**
*******************************************************************************/
void esp_ble_start_advertising (esp_ble_adv_params_all_t *ble_adv_params);



/*******************************************************************************
**
** @function        esp_ble_stop_advertising
**
** @brief           This function is called to stop advertising.
**
** @param       	None
**
** @return          None
**
*******************************************************************************/
void esp_ble_stop_advertising (void);

#endif /* __ESP_ADV_API_H__ */
