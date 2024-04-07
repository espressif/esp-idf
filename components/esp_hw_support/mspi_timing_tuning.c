/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_log.h"
#include "soc/io_mux_reg.h"
#include "soc/soc.h"
#include "hal/spi_flash_hal.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "esp_private/mspi_timing_tuning.h"
#include "esp_private/mspi_timing_config.h"
#include "mspi_timing_by_mspi_delay.h"
#include "mspi_timing_by_dqs.h"
#if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY || SOC_MEMSPI_TIMING_TUNING_BY_DQS
#include "mspi_timing_tuning_configs.h"
#include "hal/mspi_timing_tuning_ll.h"
#endif
#if SOC_MEMSPI_CLK_SRC_IS_INDEPENDENT
#include "hal/spimem_flash_ll.h"
#endif

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "esp_ipc_isr.h"
#endif

#if CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define FLASH_FREQUENCY_MHZ 120
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define FLASH_FREQUENCY_MHZ 80
#elif CONFIG_ESPTOOLPY_FLASHFREQ_64M
#define FLASH_FREQUENCY_MHZ 64
#elif CONFIG_ESPTOOLPY_FLASHFREQ_60M
#define FLASH_FREQUENCY_MHZ 60
#elif CONFIG_ESPTOOLPY_FLASHFREQ_48M
#define FLASH_FREQUENCY_MHZ 48
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
#define FLASH_FREQUENCY_MHZ 40
#elif CONFIG_ESPTOOLPY_FLASHFREQ_32M
#define FLASH_FREQUENCY_MHZ 32
#elif CONFIG_ESPTOOLPY_FLASHFREQ_30M
#define FLASH_FREQUENCY_MHZ 30
#elif CONFIG_ESPTOOLPY_FLASHFREQ_26M
#define FLASH_FREQUENCY_MHZ 26
#elif CONFIG_ESPTOOLPY_FLASHFREQ_24M
#define FLASH_FREQUENCY_MHZ 24
#elif CONFIG_ESPTOOLPY_FLASHFREQ_20M
#define FLASH_FREQUENCY_MHZ 20
#elif CONFIG_ESPTOOLPY_FLASHFREQ_16M
#define FLASH_FREQUENCY_MHZ 16
#elif CONFIG_ESPTOOLPY_FLASHFREQ_15M
#define FLASH_FREQUENCY_MHZ 15
#endif

/**
 * @brief MSPI timing tuning type
 */
typedef enum {
    MSPI_TIMING_TUNING_MSPI_DIN_DUMMY,    //tune by mspi din and dummy
    MSPI_TIMING_TUNING_MSPI_DQS_PHASE,    //tune by mspi dqs phase
    MSPI_TIMING_TUNING_MSPI_DELAYLINE,    //tune by mspi delayline
} mspi_timing_tuning_t;

typedef struct mspi_tuning_cfg_drv_s mspi_tuning_cfg_drv_t;
__attribute__((unused)) const static char *TAG = "MSPI Timing";

struct mspi_tuning_cfg_drv_s {

    /**
     * @brief Flash tuning scheme type
     */
    mspi_timing_tuning_t flash_tuning_type;

    /**
     * @brief How many test times during sweep
     */
    uint32_t sweep_test_nums;

    /**
     * @brief Init MSPI for Flash timing tuning
     *
     * @param[in] flash_freq_mhz  Flash frequency in MHz
     */
    void (*flash_init_mspi)(uint32_t flash_freq_mhz);

    /**
     * @brief Configure MSPI for Flash timing tuning
     *
     * @param[in] configs Timing tuning configs
     * @param[in] id      Config ID
     */
    void (*flash_tune_mspi)(const void *configs, uint8_t id);

    /**
     * @brief Flash read
     *
     * @param[in] buf   Read buffer
     * @param[in] addr  Read address
     * @param[in] len   Read length
     */
    void (*flash_read)(uint8_t *buf, uint32_t addr, uint32_t len);

    /**
     * @brief Select best tuning configs for Flash
     *
     * @param[in] configs             Timing tuning configurations
     * @param[in] consecutive_length  Length of the consecutive successful sample results
     * @param[in] end                 End of the consecutive successful sample results
     * @param[in] reference_data      Reference data
     * @param[in] is_ddr              DDR or SDR
     *
     * @return  Best config ID
     */
    uint32_t (*flash_select_best_tuning_config)(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr);

    /**
     * @brief Set best Flash tuning configs.
     *        After this, calling `mspi_timing_enter_high_speed_mode` will set these configs correctly
     *
     * @param[in] configs  Timing tuning configs
     * @param[in] best_id  Best config ID
     */
    void (*flash_set_best_tuning_config)(const void *configs, uint8_t best_id);

    /**
     * @brief PSRAM tuning scheme type
     */
    mspi_timing_tuning_t psram_tuning_type;

    /**
     * @brief Init MSPI for PSRAM timing tuning
     *
     * @param[in] flash_freq_mhz  PSRAM frequency in MHz
     */
    void (*psram_init_mspi)(uint32_t psram_freq_mhz);

    /**
     * @brief Configure MSPI for PSRAM timing tuning
     *
     * @param[in] configs Timing tuning configs
     * @param[in] id      Config ID
     */
    void (*psram_tune_mspi)(const void *configs, uint8_t id);

    /**
     * @brief PSRAM read
     *
     * @param[in] buf   Read buffer
     * @param[in] addr  Read address
     * @param[in] len   Read length
     */
    void (*psram_read)(uint8_t *buf, uint32_t addr, uint32_t len);

    /**
     * @brief Select best tuning configs for PSRAM
     *
     * @param[in] configs             Timing tuning configurations
     * @param[in] consecutive_length  Length of the consecutive successful sample results
     * @param[in] end                 End of the consecutive successful sample results
     * @param[in] reference_data      Reference data
     * @param[in] is_ddr              DDR or SDR
     *
     * @return  Best config ID
     */
    uint32_t (*psram_select_best_tuning_config)(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr);

    /**
     * @brief Set best PSRAM tuning configs.
     *        After this, calling `mspi_timing_enter_high_speed_mode` will set these configs correctly
     *
     * @param[in] configs  Timing tuning config
     * @param[in] best_id  Best config ID
     */
    void (*psram_set_best_tuning_config)(const void *configs, uint8_t best_id);
};

static mspi_tuning_cfg_drv_t s_tuning_cfg_drv = {};

void s_register_config_driver(mspi_tuning_cfg_drv_t *cfg_drv, bool is_flash)
{
    if (is_flash) {
        s_tuning_cfg_drv.flash_tuning_type = cfg_drv->flash_tuning_type;
        s_tuning_cfg_drv.flash_init_mspi = cfg_drv->flash_init_mspi;
        s_tuning_cfg_drv.flash_tune_mspi = cfg_drv->flash_tune_mspi;
        s_tuning_cfg_drv.flash_read = cfg_drv->flash_read;
        s_tuning_cfg_drv.flash_select_best_tuning_config = cfg_drv->flash_select_best_tuning_config;
        s_tuning_cfg_drv.flash_set_best_tuning_config = cfg_drv->flash_set_best_tuning_config;
    } else {
        s_tuning_cfg_drv.psram_tuning_type = cfg_drv->psram_tuning_type;
        s_tuning_cfg_drv.psram_init_mspi = cfg_drv->psram_init_mspi;
        s_tuning_cfg_drv.psram_tune_mspi = cfg_drv->psram_tune_mspi;
        s_tuning_cfg_drv.psram_read = cfg_drv->psram_read;
        s_tuning_cfg_drv.psram_select_best_tuning_config = cfg_drv->psram_select_best_tuning_config;
        s_tuning_cfg_drv.psram_set_best_tuning_config = cfg_drv->psram_set_best_tuning_config;
    }
    s_tuning_cfg_drv.sweep_test_nums = cfg_drv->sweep_test_nums;
}

#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
/**
 * We use different MSPI timing tuning config to read data to see if current MSPI sampling is successful.
 * The sampling result will be stored in an array. In this array, successful item will be 1, failed item will be 0.
 */
static void s_sweep_for_success_sample_points(uint8_t *reference_data, void *config, bool is_flash, uint32_t *out_array)
{
    const mspi_timing_config_t *timing_config = (const mspi_timing_config_t *)config;
    uint32_t config_idx = 0;
    uint8_t read_data[MSPI_TIMING_TEST_DATA_LEN] = {0};

    for (config_idx = 0; config_idx < timing_config->available_config_num; config_idx++) {
        out_array[config_idx] = 0;
        for (int i = 0; i < s_tuning_cfg_drv.sweep_test_nums; i++) {
            memset(read_data, 0, MSPI_TIMING_TEST_DATA_LEN);
#if MSPI_TIMING_FLASH_NEEDS_TUNING
            if (is_flash) {
                s_tuning_cfg_drv.flash_tune_mspi(timing_config, config_idx);
                s_tuning_cfg_drv.flash_read(read_data, MSPI_TIMING_FLASH_TEST_DATA_ADDR, sizeof(read_data));
            }
#endif
#if MSPI_TIMING_PSRAM_NEEDS_TUNING
            if (!is_flash) {
                s_tuning_cfg_drv.psram_tune_mspi(timing_config, config_idx);
                s_tuning_cfg_drv.psram_read(read_data, MSPI_TIMING_PSRAM_TEST_DATA_ADDR, MSPI_TIMING_TEST_DATA_LEN);
            }
#endif
            if (memcmp(reference_data, read_data, sizeof(read_data)) == 0) {
                out_array[config_idx] += 1;
            }
        }
    }

    ESP_EARLY_LOGD(TAG, "test nums: %" PRIu32 ", test result: [id][good/bad][good_times]:", s_tuning_cfg_drv.sweep_test_nums);
    for (config_idx = 0; config_idx < timing_config->available_config_num; config_idx++) {
        ESP_EARLY_LOGD(TAG, "[%"PRIu32"][%s][%" PRIu32 "] ", config_idx, out_array[config_idx] == s_tuning_cfg_drv.sweep_test_nums ? "good" : "bad", out_array[config_idx]);
    }
}

/**
 * Find consecutive successful sampling points.
 * e.g. array: {1, 1, 0, 0, 1, 1, 1, 0}
 * out_length: 3
 * outout_end_index: 6
 */
static void s_find_max_consecutive_success_points(uint32_t *array, uint32_t size, uint32_t *out_length, uint32_t *out_end_index)
{
    uint32_t max = 0;
    uint32_t match_num = 0;
    uint32_t i = 0;
    uint32_t end = 0;

    while (i < size) {
        if (array[i] == s_tuning_cfg_drv.sweep_test_nums) {
            match_num++;
        } else {
            if (match_num > max) {
                max = match_num;
                end = i - 1;
            }
            match_num = 0;
        }
        i++;
    }

    *out_length = match_num > max ? match_num : max;
    *out_end_index = match_num == size ? size : end;
}

static void s_select_best_tuning_config(mspi_timing_config_t *config, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_flash)
{
    const mspi_timing_config_t *timing_config = (const mspi_timing_config_t *)config;
    uint32_t best_point = 0;
    if (is_flash) {
#if MSPI_TIMING_FLASH_DTR_MODE
        best_point = s_tuning_cfg_drv.flash_select_best_tuning_config(timing_config, consecutive_length, end, reference_data, IS_DDR);
#elif MSPI_TIMING_FLASH_STR_MODE
        best_point = s_tuning_cfg_drv.flash_select_best_tuning_config(timing_config, consecutive_length, end, NULL, IS_SDR);
#endif
        s_tuning_cfg_drv.flash_set_best_tuning_config(timing_config, best_point);
    } else {
#if MSPI_TIMING_PSRAM_DTR_MODE
        best_point = s_tuning_cfg_drv.psram_select_best_tuning_config(timing_config, consecutive_length, end, reference_data, IS_DDR);
#elif MSPI_TIMING_PSRAM_STR_MODE
        best_point = s_tuning_cfg_drv.psram_select_best_tuning_config(timing_config, consecutive_length, end, NULL, IS_SDR);
#endif
        s_tuning_cfg_drv.psram_set_best_tuning_config(timing_config, best_point);
    }
}

static void s_do_tuning(uint8_t *reference_data, mspi_timing_config_t *config, bool is_flash)
{
    /**
     * We use MSPI to tune the timing:
     * 1. Get all MSPI sampling results.
     * 2. Find the longest consecutive successful sampling points from the result above.
     * 3. The middle one will be the best sampling point.
     */
    uint32_t consecutive_length = 0;
    uint32_t last_success_point = 0;
    uint32_t sample_result[MSPI_TIMING_CONFIG_NUM_MAX] = {0};

#if MSPI_TIMING_FLASH_NEEDS_TUNING
    if (is_flash) {
        s_tuning_cfg_drv.flash_init_mspi(FLASH_FREQUENCY_MHZ);
    }
#endif
#if MSPI_TIMING_PSRAM_NEEDS_TUNING
    if (!is_flash) {
        s_tuning_cfg_drv.psram_init_mspi(CONFIG_SPIRAM_SPEED);
    }
#endif

    s_sweep_for_success_sample_points(reference_data, config, is_flash, sample_result);
    s_find_max_consecutive_success_points(sample_result, config->available_config_num, &consecutive_length, &last_success_point);
    s_select_best_tuning_config(config, consecutive_length, last_success_point, reference_data, is_flash);
}
#endif  //#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING


/*------------------------------------------------------------------------------
 * FLASH Timing Tuning
 *----------------------------------------------------------------------------*/
#if MSPI_TIMING_FLASH_NEEDS_TUNING
void mspi_timing_flash_tuning(void)
{
    /**
     * set MSPI related regs to 20mhz configuration, to get reference data from FLASH
     * see detailed comments in this function (`mspi_timing_enter_low_speed_mode`)
     */
    mspi_timing_enter_low_speed_mode(true);

#if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY
    mspi_tuning_cfg_drv_t drv = {
        .flash_tuning_type = MSPI_TIMING_TUNING_MSPI_DIN_DUMMY,
        .sweep_test_nums = 1,
        .flash_init_mspi = mspi_timing_flash_init,
        .flash_tune_mspi = mspi_timing_config_flash_set_tuning_regs,
        .flash_read = mspi_timing_config_flash_read_data,
        .flash_select_best_tuning_config = mspi_timing_flash_select_best_tuning_config,
        .flash_set_best_tuning_config = mspi_timing_flash_set_best_tuning_config,
    };
    bool is_flash = true;
    s_register_config_driver(&drv, is_flash);

    //Disable the variable dummy mode when doing timing tuning
    mspi_timing_ll_enable_flash_variable_dummy(1, false);    //GD flash will read error in variable mode with 20MHz

    uint8_t reference_data[MSPI_TIMING_TEST_DATA_LEN] = {0};
    s_tuning_cfg_drv.flash_read(reference_data, MSPI_TIMING_FLASH_TEST_DATA_ADDR, sizeof(reference_data));

    mspi_timing_config_t timing_configs = {0};
    mspi_timing_get_flash_tuning_configs(&timing_configs);

#endif  //SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY

    s_do_tuning(reference_data, &timing_configs, true);

    mspi_timing_enter_high_speed_mode(true);
}
#else
void mspi_timing_flash_tuning(void)
{
    //Empty function for compatibility, therefore upper layer won't need to know that FLASH in which operation mode and frequency config needs to be tuned
}
#endif  //MSPI_TIMING_FLASH_NEEDS_TUNING


/*------------------------------------------------------------------------------
 * PSRAM Timing Tuning
 *----------------------------------------------------------------------------*/
#if MSPI_TIMING_PSRAM_NEEDS_TUNING
void mspi_timing_psram_tuning(void)
{
    /**
     * set MSPI related regs to 20mhz configuration, to write reference data to PSRAM
     * see detailed comments in this function (`mspi_timing_enter_low_speed_mode`)
     */
    mspi_timing_enter_low_speed_mode(true);

    // write data into psram, used to do timing tuning test.
    uint8_t reference_data[MSPI_TIMING_TEST_DATA_LEN];
    mspi_timing_config_psram_prepare_reference_data(reference_data, MSPI_TIMING_TEST_DATA_LEN);
    mspi_timing_config_psram_write_data(reference_data, MSPI_TIMING_PSRAM_TEST_DATA_ADDR, MSPI_TIMING_TEST_DATA_LEN);

#if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY
    mspi_tuning_cfg_drv_t drv = {
        .psram_tuning_type = MSPI_TIMING_TUNING_MSPI_DIN_DUMMY,
        .sweep_test_nums = 1,
        .psram_init_mspi = mspi_timing_psram_init,
        .psram_tune_mspi = mspi_timing_config_psram_set_tuning_regs,
        .psram_read = mspi_timing_config_psram_read_data,
        .psram_select_best_tuning_config = mspi_timing_psram_select_best_tuning_config,
        .psram_set_best_tuning_config = mspi_timing_psram_set_best_tuning_config,
    };
    bool is_flash = false;
    s_register_config_driver(&drv, is_flash);

    mspi_timing_config_t timing_configs = {};
    mspi_timing_get_psram_tuning_configs(&timing_configs);

    //Disable the variable dummy mode when doing timing tuning
    mspi_timing_ll_enable_flash_variable_dummy(1, false);

    //Get required config, and set them to PSRAM related registers
    s_do_tuning(reference_data, &timing_configs, false);
#endif //#if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY

#if SOC_MEMSPI_TIMING_TUNING_BY_DQS
    bool is_flash = false;
    mspi_timing_config_t timing_configs = {};

    //Phase
    mspi_tuning_cfg_drv_t drv = {
        .psram_tuning_type = MSPI_TIMING_TUNING_MSPI_DQS_PHASE,
        .sweep_test_nums = 1,
        .psram_init_mspi = mspi_timing_psram_init,
        .psram_tune_mspi = mspi_timing_config_psram_set_tuning_phase,
        .psram_read = mspi_timing_config_psram_read_data,
        .psram_select_best_tuning_config = mspi_timing_psram_select_best_tuning_phase,
        .psram_set_best_tuning_config = mspi_timing_psram_set_best_tuning_phase,
    };
    s_register_config_driver(&drv, is_flash);
    mspi_timing_get_psram_tuning_phases(&timing_configs);
    s_do_tuning(reference_data, &timing_configs, false);

    //Delayline
    drv = (mspi_tuning_cfg_drv_t) {
        .psram_tuning_type = MSPI_TIMING_TUNING_MSPI_DELAYLINE,
        .sweep_test_nums = MSPI_TIMING_DELAYLINE_TEST_NUMS,
        .psram_init_mspi = mspi_timing_psram_init,
        .psram_tune_mspi = mspi_timing_config_psram_set_tuning_delayline,
        .psram_read = mspi_timing_config_psram_read_data,
        .psram_select_best_tuning_config = mspi_timing_psram_select_best_tuning_delayline,
        .psram_set_best_tuning_config = mspi_timing_psram_set_best_tuning_delayline,
    };
    s_register_config_driver(&drv, is_flash);
    mspi_timing_get_psram_tuning_delaylines(&timing_configs);
    s_do_tuning(reference_data, &timing_configs, false);
#endif

    mspi_timing_enter_high_speed_mode(true);
}

#else
void mspi_timing_psram_tuning(void)
{
    //Empty function for compatibility, therefore upper layer won't need to know that FLASH in which operation mode and frequency config needs to be tuned
}
#endif  //MSPI_TIMING_PSRAM_NEEDS_TUNING


/*------------------------------------------------------------------------------
 * APIs to make SPI0 (and SPI1) FLASH work for high/low freq
 *----------------------------------------------------------------------------*/
void mspi_timing_enter_low_speed_mode(bool control_spi1)
{
#if SOC_MEMSPI_FLASH_CLK_SRC_IS_INDEPENDENT
    spimem_flash_ll_set_clock_source(MSPI_CLK_SRC_ROM_DEFAULT);
#endif  //SOC_MEMSPI_FLASH_CLK_SRC_IS_INDEPENDENT

#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
    /**
     * Here we are going to slow the SPI1 frequency to 20Mhz, so we need to set SPI1 din_num and din_mode regs.
     *
     * Because SPI0 and SPI1 share the din_num and din_mode regs, so if we clear SPI1 din_num and din_mode to
     * 0, if the SPI0 flash module clock is still in high freq, it may not work correctly.
     *
     * Therefore, here we need to slow both the SPI0 and SPI1 and related timing tuning regs to 20Mhz configuration.
     *
     * Currently we only need to change these clocks on chips with timing tuning
     * Should be extended to other no-timing-tuning chips if needed. e.g.:
     * we still need to turn down Flash / PSRAM clock speed at a certain period of time
     */
    mspi_timing_config_set_flash_clock(20, MSPI_TIMING_SPEED_MODE_LOW_PERF, control_spi1);
    mspi_timing_config_set_psram_clock(20, MSPI_TIMING_SPEED_MODE_LOW_PERF, control_spi1);
#endif  //#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING

#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
    mspi_timing_flash_config_clear_tuning_regs(control_spi1);
    mspi_timing_psram_config_clear_tuning_regs(control_spi1);
#endif  //#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
}

/**
 * Set FLASH and PSRAM module clock, din_num, din_mode and extra dummy,
 * according to the configuration got from timing tuning function (`calculate_best_flash_tuning_config`).
 * iF control_spi1 == 1, will also update SPI1 timing registers. Should only be set to 1 when do tuning.
 *
 * This function should always be called after `mspi_timing_flash_tuning` or `calculate_best_flash_tuning_config`
 */
void mspi_timing_enter_high_speed_mode(bool control_spi1)
{
#if SOC_MEMSPI_FLASH_CLK_SRC_IS_INDEPENDENT
    spimem_flash_ll_set_clock_source(MSPI_CLK_SRC_DEFAULT);
#endif  //SOC_MEMSPI_FLASH_CLK_SRC_IS_INDEPENDENT

#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
    /**
     * Currently we only need to change these clocks on chips with timing tuning
     * Should be extended to other no-timing-tuning chips if needed. e.g.:
     * we still need to turn down Flash / PSRAM clock speed at a certain period of time
     */
    mspi_timing_config_set_flash_clock(FLASH_FREQUENCY_MHZ, MSPI_TIMING_SPEED_MODE_NORMAL_PERF, control_spi1);
#if CONFIG_SPIRAM
    mspi_timing_config_set_psram_clock(CONFIG_SPIRAM_SPEED, MSPI_TIMING_SPEED_MODE_NORMAL_PERF, control_spi1);
#endif  //#if CONFIG_SPIRAM
#endif  //#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING

#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
    mspi_timing_flash_config_set_tuning_regs(control_spi1);
    mspi_timing_psram_config_set_tuning_regs(control_spi1);
#endif  //#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
}

void mspi_timing_change_speed_mode_cache_safe(bool switch_down)
{
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE && !CONFIG_FREERTOS_UNICORE
    // For esp chips with two levels of Cache, if another core attempts to access SPI Flash or PSRAM after the
    // cache is freeze, the access will fail and will keep retrying. This will completely block the L1 Cache,
    // causing the current core to be unable to access the stack and data in the L2 RAM, which will causes a
    // deadlock, so we need to stall another core at first.
    esp_ipc_isr_stall_other_cpu();
#endif
    /**
     * If a no-cache-freeze-supported chip needs timing tuning, add a protection way:
     * - spinlock
     * - or other way
     *
     * for preventing concurrent from MSPI to external memory
     */
#if SOC_CACHE_FREEZE_SUPPORTED
    cache_hal_freeze(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif  //#if SOC_CACHE_FREEZE_SUPPORTED

    if (switch_down) {
        //enter MSPI low speed mode, extra delays should be removed
        mspi_timing_enter_low_speed_mode(false);
    } else {
        //enter MSPI high speed mode, extra delays should be considered
        mspi_timing_enter_high_speed_mode(false);
    }

#if SOC_CACHE_FREEZE_SUPPORTED
    cache_hal_unfreeze(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif  //#if SOC_CACHE_FREEZE_SUPPORTED

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE && !CONFIG_FREERTOS_UNICORE
    esp_ipc_isr_release_other_cpu();
#endif
}

/*------------------------------------------------------------------------------
 * APIs to inform SPI1 Flash driver of necessary timing configurations
 *----------------------------------------------------------------------------*/
bool spi_flash_timing_is_tuned(void)
{
#if MSPI_TIMING_MSPI1_IS_INVOLVED
    //esp flash driver needs to be notified
#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
    //either flash or psram or both is tuned, needs notify flash driver
    return true;
#else
    //otherwise no need
    return false;
#endif
#else
    //if mspi1 is not involved in timing tuning
    return false;
#endif  //MSPI_TIMING_MSPI1_IS_INVOLVED
}

#if MSPI_TIMING_MSPI1_IS_INVOLVED && (MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING)
void spi_timing_get_flash_timing_param(spi_flash_hal_timing_config_t *out_timing_config)
{
    // Get clock configuration directly from system.
    out_timing_config->clock_config.spimem = mspi_timing_config_get_flash_clock_reg();

    // Get extra dummy length here. Therefore, no matter what freq, or mode.
    // If it needs tuning, it will return correct extra dummy len. If no tuning, it will return 0.

    out_timing_config->extra_dummy = mspi_timing_config_get_flash_extra_dummy();

    // Get CS setup/hold value here.
    mspi_timing_config_get_cs_timing(&out_timing_config->cs_setup, &out_timing_config->cs_hold);
}
#else
void spi_timing_get_flash_timing_param(spi_flash_hal_timing_config_t *out_timing_config)
{
    // This function shouldn't be called if timing tuning is not used.
    abort();
}
#endif // #if MSPI_TIMING_MSPI1_IS_INVOLVED && (MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING)

/*------------------------------------------------------------------------------
 * Common settings
 *----------------------------------------------------------------------------*/
void mspi_timing_set_pin_drive_strength(void)
{
#if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY
    //For now, set them all to 3. Need to check after QVL test results are out. TODO: IDF-3663
    //Set default pin drive
    mspi_timing_ll_set_all_pin_drive(0, 3);
#endif  //  #if SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY
}
