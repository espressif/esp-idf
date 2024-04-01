/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct isp_af_scheme_t isp_af_scheme_t;

/**
 * @brief ISP AF Scheme Interface
 */
struct isp_af_scheme_t {
    /**
     * @brief Do AF
     *
     * @param[in]   arg                ISP AF scheme specific context
     * @param[out]  definition_thresh  Definition thresh that is updated according to the current definition, this can be used to set to the ISP AF Env detector
     * @param[out]  luminance_thresh   Luminance thresh that is updated according to the current luminance, this can be used to set to the ISP AF Env detector
     *
     * @return
     */
    esp_err_t (*af_process)(void *arg, int *out_definition_thresh, int *out_luminance_thresh);

    /**
     * @brief ISP AF scheme specific contexts
     * Can be customized to difference AF schemes
     */
    void *ctx;
};

#ifdef __cplusplus
}
#endif
