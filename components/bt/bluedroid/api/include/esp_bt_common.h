#ifndef __ESP_BT_COMMON_H__
#define __ESP_BT_COMMON_H__

#include <stdint.h>

#include "bt_types.h"
#include "bta_api.h"
#include "esp_err.h"

typedef tBT_UUID esp_bt_uuid_t;   /* tBT_UUID in "bt_types.h" */

typedef BD_ADDR esp_bd_addr_t;    /* BD_ADDR in bt_types.h */

typedef tBTA_DM_SEC_CBACK esp_bt_sec_cb_t;

typedef void (*bluetooth_init_cb_t)(void);

/*******************************************************************************
**
** @function        esp_enable_bluetooth
**
** @brief           This function is called to enable bluetooth host. This 
**                  function must be called before any other functions in the
**                  API (except esp_bluetooth_init) are called.
**
** @param[in]       p_cback: 
**                  security call back function
** @param[out]      None
**
** @return          ESP_OK - Success; Other - Failed
**
*******************************************************************************/
esp_err_t esp_enable_bluetooth(esp_bt_sec_cb_t p_cback);


/*******************************************************************************
**
** @function        esp_disable_bluetooth
**
** @brief           This function is called to disable bluetooth host
**
** @param[in]       None 
** 
** @param[out]      None
**
** @return          ESP_OK - Success; Other - Failed
**
*******************************************************************************/
esp_err_t esp_disable_bluetooth(void);

/*******************************************************************************
**
** @function        esp_init_bluetooth
**
** @brief           This function is called to init bluetooth host and alloc the
**                  resource. This function must be called before all othor API
**                  are called.
**
** @param[in]       cb: When this function called success, the callback will be called
**               
** @param[out]      None
**
** @return          ESP_OK - Success; Other - Failed
**
*******************************************************************************/
esp_err_t esp_init_bluetooth(bluetooth_init_cb_t cb);

/*******************************************************************************
**
** @function        esp_deinit_bluetooth
**
** @brief           This function is called to deinit bluetooth host and free the
**                  resource.
**
** @param[in]       None 
** 
** @param[out]      None
**
** @return          ESP_OK - Success; Other - Failed
**
*******************************************************************************/
void esp_deinit_bluetooth(void);

#endif /* __ESP_BT_COMMON_H__ */
