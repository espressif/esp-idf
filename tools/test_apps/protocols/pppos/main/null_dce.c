// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_modem.h"

/**
 * @brief Dummy DCE to facilitate testing lwip ppp client with ppp server
 *
 */
typedef struct {
    modem_dce_t parent;  /*!< DCE parent class */
} null_modem_dce_t;


static esp_err_t null_dce_set_working_mode(modem_dce_t *dce, modem_mode_t mode)
{
    dce->mode = mode;
    return ESP_OK;
}

static esp_err_t null_dce_define_pdp_context(modem_dce_t *dce, uint32_t cid, const char *type, const char *apn)
{
    return ESP_OK;
}

esp_err_t null_dce_dce_hang_up(modem_dce_t *dce)
{
    return ESP_OK;
}

static esp_err_t null_dce_deinit(modem_dce_t *dce)
{
    null_modem_dce_t *bg96_dce = __containerof(dce, null_modem_dce_t, parent);
    if (dce->dte) {
        dce->dte->dce = NULL;
    }
    free(bg96_dce);
    return ESP_OK;
}

modem_dce_t *null_dce_init(modem_dte_t *dte)
{
    if (!dte) return NULL;
    /* malloc memory for bg96_dce object */
    null_modem_dce_t *null_dce = calloc(1, sizeof(null_modem_dce_t));
    if (!null_dce) return NULL;
    /* Bind DTE with DCE */
    null_dce->parent.dte = dte;
    dte->dce = &(null_dce->parent);
    /* Bind methods */
    null_dce->parent.handle_line = NULL;
    null_dce->parent.define_pdp_context = null_dce_define_pdp_context;
    null_dce->parent.set_working_mode = null_dce_set_working_mode;
    null_dce->parent.deinit = null_dce_deinit;
    null_dce->parent.hang_up = null_dce_dce_hang_up;
    /* Sync between DTE and DCE */
    return &(null_dce->parent);
}
