/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IR_TOOLS_FLAGS_PROTO_EXT (1 << 0) /*!< Enable Extended IR protocol */
#define IR_TOOLS_FLAGS_INVERSE (1 << 1)   /*!< Inverse the IR signal, i.e. take high level as low, and vice versa */

/**
* @brief IR device type
*
*/
typedef void *ir_dev_t;

/**
* @brief IR builder type
*
*/
typedef struct ir_builder_s ir_builder_t;

/**
* @brief IR parser type
*
*/
typedef struct ir_parser_s ir_parser_t;

/**
* @brief Type definition of IR builder
*
*/
struct ir_builder_s {
    /**
    * @brief Period time of sending repeat code
    *
    */
    uint32_t repeat_period_ms;

    /**
    * @brief Build frame header
    *
    * @param[in] builder: Handle of IR builder
    *
    * @return
    *      - ESP_OK: Build frame header successfully
    *      - ESP_FAIL: Build frame header failed because some error occurred
    */
    esp_err_t (*make_head)(ir_builder_t *builder);

    /**
    * @brief Build logic bit zero
    *
    * @param[in] builder: Handle of IR builder
    *
    * @return
    *      - ESP_OK: Build logic bit zero successfully
    *      - ESP_FAIL: Build logic bit zero failed because some error occurred
    */
    esp_err_t (*make_logic0)(ir_builder_t *builder);

    /**
    * @brief Build logic bit one
    *
    * @param[in] builder: Handle of IR builder
    *
    * @return
    *      ESP_OK: Build logic bit one successfully
    *      ESP_FAIL: Build logic bit one failed because some error occurred
    */
    esp_err_t (*make_logic1)(ir_builder_t *builder);

    /**
    * @brief Build frame tail
    *
    * @param[in] builder: Handle of IR builder
    *
    * @return
    *      - ESP_OK: Build frame tail successfully
    *      - ESP_FAIL: Build frame tail failed because some error occurred
    */
    esp_err_t (*make_end)(ir_builder_t *builder);

    /**
    * @brief Build a complete frame
    *
    * @param[in] builder: Handle of IR builder
    *
    * @return
    *      - ESP_OK: Build a complete frame successfully
    *      - ESP_FAIL: Build a complete frame failed because some error occurred
    */
    esp_err_t (*build_frame)(ir_builder_t *builder, uint32_t address, uint32_t command);

    /**
    * @brief Build a repeat frame
    *
    * @param[in] builder: Handle of IR builder
    *
    * @return
    *      - ESP_OK: Build a repeat frame successfully
    *      - ESP_FAIL: Build a repeat frame failed because some error occurred
    */
    esp_err_t (*build_repeat_frame)(ir_builder_t *builder);

    /**
    * @brief Get the result frame after a series of building steps
    *
    * @param[in] builder: Handle of IR builder
    * @param[out] result: Result of frame building, which contains all of the raw data that could be send directly
    * @param[out] length: Length of result data
    *
    * @return
    *      - ESP_OK: Get result data successfully
    *      - ESP_ERR_INVALID_ARG: Get result data failed because of invalid arguments
    *      - ESP_FAIL: Get result data failed because some other errors occurred
    */
    esp_err_t (*get_result)(ir_builder_t *builder, void *result, size_t *length);

    /**
    * @brief Free resources used by IR builder
    *
    * @param[in] builder: Handle of IR builder
    *
    * @return
    *      - ESP_OK: Free resources successfully
    *      - ESP_FAIL: Free resources failed because some error occurred
    */
    esp_err_t (*del)(ir_builder_t *builder);
};

/**
* @brief Type definition of IR parser
*
*/
struct ir_parser_s {
    /**
    * @brief Input raw data to IR parser
    *
    * @param[in] parser: Handle of IR parser
    * @param[in] raw_data: Raw data which need decoding by IR parser
    * @param[in] length: Length of raw data
    *
    * @return
    *      - ESP_OK: Input raw data successfully
    *      - ESP_ERR_INVALID_ARG: Input raw data failed because of invalid argument
    *      - ESP_FAIL: Input raw data failed because some other error occurred
    */
    esp_err_t (*input)(ir_parser_t *parser, void *raw_data, uint32_t length);

    /**
    * @brief Get the scan code after decoding of raw data
    *
    * @param[in] parser: Handle of IR parser
    * @param[out] address: Address of the scan code
    * @param[out] command: Command of the scan code
    * @param[out] repeat: Indicate if it's a repeat code
    *
    * @return
    *      - ESP_OK: Get scan code successfully
    *      - ESP_ERR_INVALID_ARG: Get scan code failed because of invalid arguments
    *      - ESP_FAIL: Get scan code failed because some error occurred
    */
    esp_err_t (*get_scan_code)(ir_parser_t *parser, uint32_t *address, uint32_t *command, bool *repeat);

    /**
    * @brief Free resources used by IR parser
    *
    * @param[in] parser: Handle of IR parser
    *
    * @return
    *      - ESP_OK: Free resource successfully
    *      - ESP_FAIL: Free resources fail failed because some error occurred
    */
    esp_err_t (*del)(ir_parser_t *parser);
};

/**
* @brief Configuration type of IR builder
*
*/
typedef struct {
    uint32_t buffer_size; /*!< Size of the internal buffer used by IR builder */
    ir_dev_t dev_hdl;     /*!< IR device handle */
    uint32_t flags;       /*!< Flags for IR builder, different flags will enable different features */
} ir_builder_config_t;

/**
* @brief Configuration type of IR parser
*
*/
typedef struct {
    ir_dev_t dev_hdl;   /*!< IR device handle */
    uint32_t flags;     /*!< Flags for IR parser, different flags will enable different features */
    uint32_t margin_us; /*!< Timing parameter, indicating the tolerance to environment noise */
} ir_parser_config_t;

/**
 * @brief Default configuration for IR builder
 *
 */
#define IR_BUILDER_DEFAULT_CONFIG(dev) \
    {                                  \
        .buffer_size = 64,             \
        .dev_hdl = dev,                \
        .flags = 0,                    \
    }

/**
 * @brief Default configuration for IR parser
 *
 */
#define IR_PARSER_DEFAULT_CONFIG(dev) \
    {                                 \
        .dev_hdl = dev,               \
        .flags = 0,                   \
        .margin_us = 200,             \
    }

/**
* @brief Creat a NEC protocol builder
*
* @param config: configuration of NEC builder
* @return
*      Handle of NEC builder or NULL
*/
ir_builder_t *ir_builder_rmt_new_nec(const ir_builder_config_t *config);

/**
* @brief Creat a RC5 protocol builder
*
* @param config: configuration of RC5 builder
* @return
*      Handle of RC5 builder or NULL
*/
ir_builder_t *ir_builder_rmt_new_rc5(const ir_builder_config_t *config);

/**
* @brief Creat a NEC protocol parser
*
* @param config: configuration of NEC parser
* @return
*      Handle of NEC parser or NULL
*/
ir_parser_t *ir_parser_rmt_new_nec(const ir_parser_config_t *config);

/**
* @brief Creat a RC5 protocol parser
*
* @param config: configuration of RC5 parser
* @return
*      Handle of RC5 parser or NULL
*/
ir_parser_t *ir_parser_rmt_new_rc5(const ir_parser_config_t *config);
#ifdef __cplusplus
}
#endif
