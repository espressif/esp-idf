/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "tusb.h"
#include "tinyusb_types.h"

/* CDC classification
   ********************************************************************* */
typedef enum {
    TINYUSB_CDC_DATA =          0x00,
} cdc_data_sublcass_type_t; // CDC120 specification

/* Note:other classification is represented in the file components\tinyusb\tinyusb\src\class\cdc\cdc.h */

/*********************************************************************** CDC classification*/
/* Structs
   ********************************************************************* */
typedef struct {
    tinyusb_usbdev_t usb_dev;                           /*!< USB device to set up */
    tusb_class_code_t cdc_class;                 /*!< CDC device class : Communications or Data device */
    union {
        cdc_comm_sublcass_type_t comm_subclass; /*!< Communications device subclasses: ACM, ECM, etc. */
        cdc_data_sublcass_type_t data_subclass; /*!< Data device has only one subclass.*/
    } cdc_subclass;                                     /*!< CDC device subclass according to Class Definitions for Communications Devices the CDC v.1.20 */
} tinyusb_config_cdc_t;                                 /*!< Main configuration structure of a CDC device */

typedef struct {
    tinyusb_usbdev_t usb_dev;                           /*!< USB device used for the instance */
    tusb_class_code_t type;
    union {
        cdc_comm_sublcass_type_t comm_subclass; /*!< Communications device subclasses: ACM, ECM, etc. */
        cdc_data_sublcass_type_t data_subclass; /*!< Data device has only one subclass.*/
    } cdc_subclass;                                     /*!< CDC device subclass according to Class Definitions for Communications Devices the CDC v.1.20 */
    void *subclass_obj;  /*!< Dynamically allocated subclass specific object */
} esp_tusb_cdc_t;
/*********************************************************************** Structs*/
/* Functions
   ********************************************************************* */
/**
 * @brief Initializing CDC basic object
 * @param itf - number of a CDC object
 * @param cfg - CDC configuration structure
 *
 * @return esp_err_t ESP_OK or ESP_FAIL
 */
esp_err_t tinyusb_cdc_init(int itf, const tinyusb_config_cdc_t *cfg);


/**
 * @brief De-initializing CDC. Clean its objects
 * @param itf - number of a CDC object
 * @return esp_err_t ESP_OK, ESP_ERR_INVALID_ARG, ESP_ERR_INVALID_STATE
 *
 */
esp_err_t tinyusb_cdc_deinit(int itf);


/**
 * @brief Return interface of a CDC device
 *
 * @param itf_num
 * @return esp_tusb_cdc_t* pointer to the interface or (NULL) on error
 */
esp_tusb_cdc_t *tinyusb_cdc_get_intf(int itf_num);
/*********************************************************************** Functions*/

#ifdef __cplusplus
}
#endif
