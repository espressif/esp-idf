// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_types.h"
#include "esp_err.h"
#include "esp_event.h"

typedef struct modem_dte modem_dte_t;
typedef struct modem_dce modem_dce_t;

/**
 * @brief Working mode of Modem
 *
 */
typedef enum {
    MODEM_COMMAND_MODE = 0, /*!< Command Mode */
    MODEM_PPP_MODE          /*!< PPP Mode */
} modem_mode_t;

/**
 * @brief Modem flow control type
 *
 */
typedef enum {
    MODEM_FLOW_CONTROL_NONE = 0,
    MODEM_FLOW_CONTROL_SW,
    MODEM_FLOW_CONTROL_HW
} modem_flow_ctrl_t;

/**
 * @brief DTE(Data Terminal Equipment)
 *
 */
struct modem_dte {
    modem_flow_ctrl_t flow_ctrl;                                                    /*!< Flow control of DTE */
    modem_dce_t *dce;                                                               /*!< DCE which connected to the DTE */
    esp_err_t (*send_cmd)(modem_dte_t *dte, const char *command, uint32_t timeout); /*!< Send command to DCE */
    int (*send_data)(modem_dte_t *dte, const char *data, uint32_t length);          /*!< Send data to DCE */
    esp_err_t (*send_wait)(modem_dte_t *dte, const char *data, uint32_t length,
                           const char *prompt, uint32_t timeout);      /*!< Wait for specific prompt */
    esp_err_t (*change_mode)(modem_dte_t *dte, modem_mode_t new_mode); /*!< Changing working mode */
    esp_err_t (*process_cmd_done)(modem_dte_t *dte);                   /*!< Callback when DCE process command done */
    esp_err_t (*deinit)(modem_dte_t *dte);                             /*!< Deinitialize */
};

#ifdef __cplusplus
}
#endif
