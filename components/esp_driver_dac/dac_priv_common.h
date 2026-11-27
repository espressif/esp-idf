/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#if CONFIG_DAC_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for DAC driver
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "driver/dac_types.h"
#include "hal/dac_types.h"
#include "hal/dac_types_private.h"
#include "hal/dac_ll.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

ESP_LOG_ATTR_TAG(TAG, "dac");

/**
 * DAC driver spinlock
 *
 * - ESP32 / ESP32-S2: use the global rtc_spinlock
 * - ESP32-S31: dedicated DAC register block, use dac_priv_spinlock
 */
#if SOC_IS(ESP32) || SOC_IS(ESP32S2)
extern portMUX_TYPE rtc_spinlock;

#define DAC_ENTER_CRITICAL()         portENTER_CRITICAL(&rtc_spinlock)
#define DAC_EXIT_CRITICAL()          portEXIT_CRITICAL(&rtc_spinlock)
#define DAC_ENTER_CRITICAL_SAFE()    portENTER_CRITICAL_SAFE(&rtc_spinlock)
#define DAC_EXIT_CRITICAL_SAFE()     portEXIT_CRITICAL_SAFE(&rtc_spinlock)
#else
extern portMUX_TYPE dac_priv_spinlock;

#define DAC_ENTER_CRITICAL()         portENTER_CRITICAL(&dac_priv_spinlock)
#define DAC_EXIT_CRITICAL()          portEXIT_CRITICAL(&dac_priv_spinlock)
#define DAC_ENTER_CRITICAL_SAFE()    portENTER_CRITICAL_SAFE(&dac_priv_spinlock)
#define DAC_EXIT_CRITICAL_SAFE()     portEXIT_CRITICAL_SAFE(&dac_priv_spinlock)
#endif // SOC_IS(ESP32) || SOC_IS(ESP32S2)

#define DAC_NULL_POINTER_CHECK(p)     ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '"#p"' is NULL")
#define DAC_NULL_POINTER_CHECK_ISR(p) ESP_RETURN_ON_FALSE_ISR((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '"#p"' is NULL")

#if CONFIG_DAC_ISR_IRAM_SAFE || CONFIG_DAC_CTRL_FUNC_IN_IRAM
#define DAC_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define DAC_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

#define DAC_DIV_ROUND(n, d) (((n) + (d) / 2) / (d))
#define DAC_DIV_CEIL(n, d) (((n) + (d) - 1) / (d))
#define DAC_MAX(a, b) ((a) > (b) ? (a) : (b))
#define DAC_CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define DAC_ABS_DIFF(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))

#define IS_VALID_DAC_CLK_SRC(clk_src, clk_src_list) ({                          \
    bool _dac_valid = false;                                                    \
    const soc_module_clk_t _dac_clk_src = (soc_module_clk_t)(clk_src);          \
    const soc_module_clk_t _dac_clk_src_list[] = clk_src_list;                  \
    for (size_t _dac_i = 0;                                                     \
         _dac_i < sizeof(_dac_clk_src_list) / sizeof(_dac_clk_src_list[0]);     \
         _dac_i++) {                                                            \
        if (_dac_clk_src == _dac_clk_src_list[_dac_i]) {                        \
            _dac_valid = true;                                                  \
            break;                                                              \
        }                                                                       \
    }                                                                           \
    _dac_valid;                                                                 \
})

static inline bool dac_priv_is_valid_channel_config(const dac_channel_config_t *cfg)
{
    bool valid_output = cfg->output_mode == DAC_CHANNEL_OUTPUT_VOLTAGE;
    bool valid_buffer = cfg->buffer_mode == DAC_CHANNEL_BUFFER_FULL_SWING;
#if SOC_DAC_HS_RESOLUTION
    valid_output |= cfg->output_mode == DAC_CHANNEL_OUTPUT_CURRENT;
#endif
#if SOC_DAC_SUPPORT_BUFFER_CFG
    valid_buffer |= cfg->buffer_mode == DAC_CHANNEL_BUFFER_REDUCED_SWING || cfg->buffer_mode == DAC_CHANNEL_BUFFER_BYPASS;
#endif
    return valid_output && valid_buffer;
}

/**
 * @brief Get the DAC code bit width from a channel (analog pad) configuration
 */
FORCE_INLINE_ATTR uint8_t dac_priv_chan_cfg_get_bitwidth(const dac_channel_config_t *cfg)
{
#if SOC_DAC_HS_RESOLUTION
    return (cfg->output_mode == DAC_CHANNEL_OUTPUT_CURRENT) ? SOC_DAC_HS_RESOLUTION : SOC_DAC_RESOLUTION;
#else
    (void)cfg;
    return SOC_DAC_RESOLUTION;
#endif
}

/*---------------------------------------------------------------
                  Channel (analog pad) management
---------------------------------------------------------------*/

/**
 * @brief Register dac channel in the driver, in case a same channel is reused by different modes
 *
 * @param[in] chan_id   DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has been occupied
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Register the channel success
 */
esp_err_t dac_priv_channel_register(dac_channel_t chan_id);

/**
 * @brief Deregister dac channel in the driver
 *
 * @param[in] chan_id   DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has been freed or not disabled
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Deregister the channel success
 */
esp_err_t dac_priv_channel_deregister(dac_channel_t chan_id);

/**
 * @brief Enable the DAC channel and turn on its power
 *
 * @param chan_id       DAC channel id
 * @param chan_cfg      Channel (analog pad) configuration
 * @param source        Channel data source
 * @return
 *      - ESP_ERR_INVALID_STATE The channel has not been registered or already enabled
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Enable the channel success
 */
esp_err_t dac_priv_channel_enable(dac_channel_t chan_id, dac_channel_config_t chan_cfg, dac_data_source_t source);

/**
 * @brief Disable the DAC channel and turn off its power
 *
 * @param chan_id       DAC channel id
 * @return
 *      - ESP_ERR_INVALID_STATE The channel is not enabled
 *      - ESP_ERR_INVALID_ARG   The channel id is incorrect
 *      - ESP_OK                Disable the channel success
 */
esp_err_t dac_priv_channel_disable(dac_channel_t chan_id);

/*---------------------------------------------------------------
                      DAC clock management
---------------------------------------------------------------*/

/**
 * @brief LS clock frequency constraint held by a DAC clock user
 */
typedef enum {
    DAC_LS_CLK_CONSTRAINT_DEFAULT,       /*!< LS clock constraints under default conditions */
    DAC_LS_CLK_CONSTRAINT_CALIBRATION,   /*!< LS clock constraints during calibration */
    DAC_LS_CLK_CONSTRAINT_SAMPLE_HOLD,   /*!< LS clock constraints in sample-hold mode */
    DAC_LS_CLK_CONSTRAINT_COUNT,
} dac_ls_clk_constraint_t;

/**
 * @brief Acquire a DAC clock reference with the requested LS constraint
 *
 * @param[in] source Clock source validated by the caller
 * @param[in] constraint LS clock constraint, ignored on targets without HS/LS clocks.
 * @param[out] freq_hz DAC HS frequency on targets with HS/LS clocks, otherwise the source frequency.
 *                     Pass NULL when the frequency is not needed.
 * @note On failure, the caller holds no new clock reference.
 */
esp_err_t dac_priv_clk_acquire_unified(soc_module_clk_t source, dac_ls_clk_constraint_t constraint, uint32_t *freq_hz);

/**
 * @brief Release a DAC clock reference using the source and constraint from its successful acquire
 *
 * @note The constraint is ignored on targets without HS/LS clocks.
 */
esp_err_t dac_priv_clk_release_unified(soc_module_clk_t source, dac_ls_clk_constraint_t constraint);

#if DAC_LL_SHARED_HS_LS_CLK
/**
 * @brief Acquire the shared DAC clocks and one LS clock constraint reference
 *
 * @note The first user selects the clock source. Further users must request the same source.
 * @note HS is undivided and LS is dynamically adjusted to satisfy the most restrictive constraint.
 * @note Sample-hold timing must be configured after acquiring its clock constraint. The LS frequency
 *       remains fixed while sample-hold references exist, since no other constraint is stricter.
 *
 * @param[in] source Requested clock source
 * @param[in] constraint LS frequency constraint to acquire
 * @return
 *      - ESP_ERR_INVALID_ARG   Invalid source or constraint
 *      - ESP_ERR_INVALID_STATE Conflicting source
 *      - ESP_ERR_NOT_SUPPORTED Required divider is outside the hardware range
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_clk_acquire(soc_module_clk_t source, dac_ls_clk_constraint_t constraint);

/**
 * @brief Get the configured DAC HS clock frequency in Hz
 *
 * @param[out] freq_hz Actual HS clock frequency
 * @return
 *      - ESP_ERR_INVALID_STATE No clock reference is held
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_clk_get_hs_freq_hz(uint32_t *freq_hz);

/**
 * @brief Get the configured DAC LS clock frequency in Hz
 *
 * @param[out] freq_hz Actual LS clock frequency
 * @return
 *      - ESP_ERR_INVALID_STATE No clock reference is held
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_clk_get_ls_freq_hz(uint32_t *freq_hz);

/**
 * @brief Release one DAC clock reference with its matching LS constraint
 *
 * @note The last release disables the DAC clocks and releases the source clock. Otherwise, LS is
 *       updated to satisfy the remaining constraints.
 *
 * @param[in] constraint Constraint used for the corresponding successful acquire
 * @return
 *      - ESP_ERR_INVALID_ARG   Invalid constraint
 *      - ESP_ERR_INVALID_STATE No reference exists for this constraint
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_clk_release(dac_ls_clk_constraint_t constraint);

#endif // DAC_LL_SHARED_HS_LS_CLK

/*---------------------------------------------------------------
                Cosine generator (Sintx) management
---------------------------------------------------------------*/

typedef enum {
    DAC_SINTX_MODE_TONE,       /*!< Cosine wave output */
#if DAC_LL_DC_VIA_SINTX
    DAC_SINTX_MODE_DC,         /*!< Direct (DC) software output */
#endif
} dac_sintx_mode_t;

/**
 * @brief Declare use of the shared Sintx generator in the specified mode (reference counted)
 *
 * @note  This function only claims a mode reference; it does not configure or start the hardware.
 *        Tone and DC modes are mutually exclusive.
 *
 * @param[in] mode Sintx operating mode to declare
 * @return
 *      - ESP_ERR_INVALID_STATE The generator is declared in a conflicting mode
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_sintx_acquire(dac_sintx_mode_t mode);

/**
 * @brief Start one declared Sintx user (reference counted)
 *
 * @note  The first running user starts the hardware in the currently declared tone or DC mode.
 *
 * @return
 *      - ESP_ERR_INVALID_STATE No unused declared reference is available
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_sintx_start(void);

/**
 * @brief Stop one running Sintx user (reference counted)
 *
 * @note  The last running user stops the hardware.
 *
 * @return
 *      - ESP_ERR_INVALID_STATE The generator has no running user
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_sintx_stop(void);

/**
 * @brief Release one declared Sintx reference
 *
 * @note  Releases one reference acquired by `dac_priv_sintx_acquire()`. A running reference must be
 *        stopped before it is released.
 *
 * @return
 *      - ESP_ERR_INVALID_STATE No stopped declared reference is available
 *      - ESP_OK                Success
 */
esp_err_t dac_priv_sintx_release(void);

/**
 * @brief Sintx hardware frequency parameters
 */
typedef struct {
#if DAC_LL_SINTX_HAS_TIMER_TARGET
    uint32_t timer_target;
#endif
    uint32_t fstep;
} dac_sintx_freq_hw_t;

/**
 * @brief Set sintx frequency parameter
 *
 * @note The DAC lock is not acquired within this function and must be acquired by the caller.
 */
void dac_priv_sintx_set_freq_unlocked(dac_sintx_freq_hw_t freq_hw);

#ifdef __cplusplus
}
#endif
