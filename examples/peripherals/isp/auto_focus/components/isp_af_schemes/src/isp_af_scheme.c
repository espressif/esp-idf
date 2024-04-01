/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/cdefs.h>
#include "esp_types.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "isp_af_scheme.h"
#include "isp_af_scheme_interface.h"

const static char *TAG = "AF_SCHEME";

esp_err_t isp_af_process(isp_af_scheme_handle_t scheme, int *out_definition_thresh, int *out_luminance_thresh)
{
    ESP_RETURN_ON_FALSE(scheme, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(scheme->ctx, ESP_ERR_INVALID_STATE, TAG, "no scheme registered, create a scheme first");

    return scheme->af_process(scheme->ctx, out_definition_thresh, out_luminance_thresh);
}
