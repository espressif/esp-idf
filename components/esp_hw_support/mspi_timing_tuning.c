/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/spi_mem_reg.h"
#include "soc/io_mux_reg.h"
#include "esp_private/mspi_timing_tuning.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "hal/spi_flash_hal.h"
#include "hal/mspi_timing_tuning_ll.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "mspi_timing_config.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/cache.h"
#endif

#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
const static char *TAG = "MSPI Timing";
static mspi_timing_tuning_param_t s_flash_best_timing_tuning_config;
static mspi_timing_tuning_param_t s_psram_best_timing_tuning_config;
#endif

/*------------------------------------------------------------------------------
 * Common settings
 *----------------------------------------------------------------------------*/
void mspi_timing_set_pin_drive_strength(void)
{
    //For now, set them all to 3. Need to check after QVL test results are out. TODO: IDF-3663
    //Set default clk
    mspi_timing_ll_set_all_pin_drive(0, 3);
}

/*------------------------------------------------------------------------------
 * Static functions to get clock configs
 *----------------------------------------------------------------------------*/
static mspi_timing_config_core_clock_t get_mspi_core_clock(void)
{
    return mspi_timing_config_get_core_clock();
}

static uint32_t get_flash_clock_divider(void)
{
#if CONFIG_ESPTOOLPY_FLASHFREQ_20M
    return MSPI_TIMING_CORE_CLOCK_MHZ / 20;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    return MSPI_TIMING_CORE_CLOCK_MHZ / 40;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
    return MSPI_TIMING_CORE_CLOCK_MHZ / 80;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_120M
    return MSPI_TIMING_CORE_CLOCK_MHZ / 120;
#else
    abort();
#endif
}

static uint32_t get_psram_clock_divider(void)
{
#if CONFIG_SPIRAM_SPEED_40M
    return MSPI_TIMING_CORE_CLOCK_MHZ / 40;
#elif CONFIG_SPIRAM_SPEED_80M
    return MSPI_TIMING_CORE_CLOCK_MHZ / 80;
#elif CONFIG_SPIRAM_SPEED_120M
    return MSPI_TIMING_CORE_CLOCK_MHZ / 120;
#else
    //Will enter this branch only if PSRAM is not enable
    return 0;
#endif
}

#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
/*------------------------------------------------------------------------------
 * Static functions to do timing tuning
 *----------------------------------------------------------------------------*/
/**
 * Set timing tuning regs, in order to get successful sample points
 */
static void init_spi1_for_tuning(bool is_flash)
{
    //Get required core clock and module clock settings
    mspi_timing_config_core_clock_t core_clock = get_mspi_core_clock();
    //Set SPI1 core clock. SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    mspi_timing_config_set_core_clock(0, core_clock);
    //Set SPI1 module clock as required
    if (is_flash) {
        uint32_t flash_div = get_flash_clock_divider();
        mspi_timing_config_set_flash_clock(1, flash_div);
        //Power on HCLK
        mspi_timinng_ll_enable_flash_hclk(0);
    } else {
        //We use SPI1 Flash to tune PSRAM, PSRAM timing related regs do nothing on SPI1
        uint32_t psram_div = get_psram_clock_divider();
        mspi_timing_config_set_flash_clock(1, psram_div);
        //Power on HCLK
        mspi_timinng_ll_enable_psram_hclk(0);
    }
}

/**
 * We use different SPI1 timing tuning config to read data to see if current MSPI sampling is successful.
 * The sampling result will be stored in an array. In this array, successful item will be 1, failed item will be 0.
 */
static void sweep_for_success_sample_points(const uint8_t *reference_data, const mspi_timing_config_t *config, bool is_flash, uint8_t *out_array)
{
    uint32_t config_idx = 0;
    uint8_t read_data[MSPI_TIMING_TEST_DATA_LEN] = {0};

    for (config_idx = 0; config_idx < config->available_config_num; config_idx++) {
        memset(read_data, 0, MSPI_TIMING_TEST_DATA_LEN);
#if MSPI_TIMING_FLASH_NEEDS_TUNING
        if (is_flash) {
            mspi_timing_config_flash_set_tuning_regs(&(config->tuning_config_table[config_idx]));
            mspi_timing_config_flash_read_data(read_data, MSPI_TIMING_FLASH_TEST_DATA_ADDR, sizeof(read_data));
        }
#endif
#if MSPI_TIMING_PSRAM_NEEDS_TUNING
        if (!is_flash) {
            mspi_timing_config_psram_set_tuning_regs(&(config->tuning_config_table[config_idx]));
            mspi_timing_config_psram_read_data(read_data, MSPI_TIMING_PSRAM_TEST_DATA_ADDR, MSPI_TIMING_TEST_DATA_LEN);
        }
#endif
        if (memcmp(reference_data, read_data, sizeof(read_data)) == 0) {
            out_array[config_idx] = 1;
            ESP_EARLY_LOGD(TAG, "%d, good", config_idx);
        } else {
            ESP_EARLY_LOGD(TAG, "%d, bad", config_idx);
        }
    }
}

/**
 * Find consecutive successful sampling points.
 * e.g. array: {1, 1, 0, 0, 1, 1, 1, 0}
 * out_length: 3
 * outout_end_index: 6
 */
static void find_max_consecutive_success_points(uint8_t *array, uint32_t size, uint32_t *out_length, uint32_t *out_end_index)
{
    uint32_t max = 0;
    uint32_t match_num = 0;
    uint32_t i = 0;
    uint32_t end = 0;

    while (i < size) {
        if (array[i]) {
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

    /**
     * this is to deal with the case when the last points are consecutive 1, e.g.
     * {1, 0, 0, 1, 1, 1, 1, 1, 1}
     */
    if (match_num > max) {
        max = match_num;
        end = i - 1;
    }

    *out_length = max;
    *out_end_index = end;
}

#if (MSPI_TIMING_FLASH_DTR_MODE || MSPI_TIMING_PSRAM_DTR_MODE) && (MSPI_TIMING_CORE_CLOCK_MHZ == 240)
static bool get_working_pll_freq(const uint8_t *reference_data, bool is_flash, uint32_t *out_max_freq, uint32_t *out_min_freq)
{
    uint8_t read_data[MSPI_TIMING_TEST_DATA_LEN] = {0};
    rtc_cpu_freq_config_t previous_config;
    rtc_clk_cpu_freq_get_config(&previous_config);

    uint32_t big_num = MSPI_TIMING_PLL_FREQ_SCAN_RANGE_MHZ_MAX * 2;  //This number should be larger than MSPI_TIMING_PLL_FREQ_SCAN_RANGE_MHZ_MAX, for error handling
    uint32_t max_freq = 0;
    uint32_t min_freq = big_num;
    rtc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();

    for (int pll_mhz_tuning = MSPI_TIMING_PLL_FREQ_SCAN_RANGE_MHZ_MIN; pll_mhz_tuning <= MSPI_TIMING_PLL_FREQ_SCAN_RANGE_MHZ_MAX; pll_mhz_tuning += 8) {
        //bbpll calibration start
        regi2c_ctrl_ll_bbpll_calibration_start();

        /**
         * pll_mhz = xtal_mhz * (oc_div + 4) / (oc_ref_div + 1)
         */
        clk_ll_bbpll_set_frequency_for_mspi_tuning(xtal_freq, pll_mhz_tuning, ((pll_mhz_tuning / 4) - 4), 9);

        //wait calibration done
        while(!regi2c_ctrl_ll_bbpll_calibration_is_done());

        //bbpll calibration stop
        regi2c_ctrl_ll_bbpll_calibration_stop();

        memset(read_data, 0, MSPI_TIMING_TEST_DATA_LEN);
        if (is_flash) {
            mspi_timing_config_flash_read_data(read_data, MSPI_TIMING_FLASH_TEST_DATA_ADDR, MSPI_TIMING_TEST_DATA_LEN);
        } else {
            mspi_timing_config_psram_read_data(read_data, MSPI_TIMING_PSRAM_TEST_DATA_ADDR, MSPI_TIMING_TEST_DATA_LEN);
        }

        if (memcmp(read_data, reference_data, MSPI_TIMING_TEST_DATA_LEN) == 0) {
            max_freq = MAX(pll_mhz_tuning, max_freq);
            min_freq = MIN(pll_mhz_tuning, min_freq);

            //Continue to find successful cases
            continue;
        }

        if (max_freq != 0) {
            //The first fail case after successful case(s) is the end
            break;
        }

        //If no break, no successful case found, continue to find successful cases
    }

    //restore PLL config
    clk_ll_bbpll_set_freq_mhz(previous_config.source_freq_mhz);
    //bbpll calibration start
    regi2c_ctrl_ll_bbpll_calibration_start();
    //set pll
    clk_ll_bbpll_set_config(previous_config.source_freq_mhz, xtal_freq);
    //wait calibration done
    while(!regi2c_ctrl_ll_bbpll_calibration_is_done());
    //bbpll calibration stop
    regi2c_ctrl_ll_bbpll_calibration_stop();


    *out_max_freq = max_freq;
    *out_min_freq = min_freq;

    return (max_freq != 0);
}
#endif  //Frequency Scanning

#if MSPI_TIMING_FLASH_DTR_MODE || MSPI_TIMING_PSRAM_DTR_MODE
static uint32_t select_best_tuning_config_dtr(mspi_timing_config_t *config, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_flash)
{
#if (MSPI_TIMING_CORE_CLOCK_MHZ == 160)
    //Core clock 160M DTR best point scheme
    (void) reference_data;
    (void) is_flash;
    uint32_t best_point = 0;

    //These numbers will probably be same on other chips, if this version of algorithm is utilised
    if (consecutive_length <= 2 || consecutive_length >= 6) {
        //tuning is FAIL, select default point, and generate a warning
        best_point = config->default_config_id;
        ESP_EARLY_LOGW(TAG, "tuning fail, best point is fallen back to index %d", best_point);
    } else if (consecutive_length <= 4) {
        //consecutive length :  3 or 4
        best_point = end - 1;
        ESP_EARLY_LOGD(TAG,"tuning success, best point is index %d", best_point);
    } else {
        //consecutive point list length equals 5
        best_point = end - 2;
        ESP_EARLY_LOGD(TAG,"tuning success, best point is index %d", best_point);
    }

    return best_point;

#elif (MSPI_TIMING_CORE_CLOCK_MHZ == 240)

    uint32_t best_point = 0;
    uint32_t current_point = end + 1 - consecutive_length;
    bool ret = false;

    //This `max_freq` is the max pll frequency that per MSPI timing tuning config can work
    uint32_t max_freq = 0;
    uint32_t temp_max_freq = 0;
    uint32_t temp_min_freq = 0;

    for (; current_point <= end; current_point++) {
        if (is_flash) {
            mspi_timing_config_flash_set_tuning_regs(&(config->tuning_config_table[current_point]));
        } else {
            mspi_timing_config_psram_set_tuning_regs(&(config->tuning_config_table[current_point]));
        }

        ret = get_working_pll_freq(reference_data, is_flash, &temp_max_freq, &temp_min_freq);
        if (ret && temp_min_freq <= MSPI_TIMING_PLL_FREQ_SCAN_THRESH_MHZ_LOW && temp_max_freq >= MSPI_TIMING_PLL_FREQ_SCAN_THRESH_MHZ_HIGH && temp_max_freq > max_freq) {
            max_freq = temp_max_freq;
            best_point = current_point;
        }
        ESP_EARLY_LOGD(TAG, "sample point %d, max pll is %d mhz, min pll is %d\n", current_point, temp_max_freq, temp_min_freq);
    }
    if (max_freq == 0) {
        ESP_EARLY_LOGW(TAG, "freq scan tuning fail, best point is fallen back to index %d", end + 1 - consecutive_length);
        best_point = end + 1 - consecutive_length;
    } else {
        ESP_EARLY_LOGD(TAG, "freq scan success, max pll is %dmhz, best point is index %d", max_freq, best_point);
    }

    return best_point;

#else
    //won't reach here
    abort();
#endif
}
#endif

#if MSPI_TIMING_FLASH_STR_MODE || MSPI_TIMING_PSRAM_STR_MODE
static uint32_t select_best_tuning_config_str(mspi_timing_config_t *config, uint32_t consecutive_length, uint32_t end)
{
#if (MSPI_TIMING_CORE_CLOCK_MHZ == 120 || MSPI_TIMING_CORE_CLOCK_MHZ == 240)
    //STR best point scheme
    uint32_t best_point;

    if (consecutive_length <= 2|| consecutive_length >= 5) {
        //tuning is FAIL, select default point, and generate a warning
        best_point = config->default_config_id;
        ESP_EARLY_LOGW(TAG, "tuning fail, best point is fallen back to index %d", best_point);
    } else {
        //consecutive length :  3 or 4
        best_point = end - consecutive_length / 2;
        ESP_EARLY_LOGD(TAG,"tuning success, best point is index %d", best_point);
    }

    return best_point;
#else
    //won't reach here
    abort();
#endif
}
#endif

static void select_best_tuning_config(mspi_timing_config_t *config, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_flash)
{
    uint32_t best_point = 0;
    if (is_flash) {
#if MSPI_TIMING_FLASH_DTR_MODE
        best_point = select_best_tuning_config_dtr(config, consecutive_length, end, reference_data, is_flash);
#elif MSPI_TIMING_FLASH_STR_MODE
        best_point = select_best_tuning_config_str(config, consecutive_length, end);
#endif
        s_flash_best_timing_tuning_config = config->tuning_config_table[best_point];
        ESP_EARLY_LOGI(TAG, "Flash timing tuning index: %d", best_point);
    } else {
#if MSPI_TIMING_PSRAM_DTR_MODE
        best_point = select_best_tuning_config_dtr(config, consecutive_length, end, reference_data, is_flash);
#elif MSPI_TIMING_PSRAM_STR_MODE
        best_point = select_best_tuning_config_str(config, consecutive_length, end);
#endif
        s_psram_best_timing_tuning_config = config->tuning_config_table[best_point];
        ESP_EARLY_LOGI(TAG, "PSRAM timing tuning index: %d", best_point);
    }
}

static void do_tuning(const uint8_t *reference_data, mspi_timing_config_t *timing_config, bool is_flash)
{
    /**
     * We use SPI1 to tune the timing:
     * 1. Get all SPI1 sampling results.
     * 2. Find the longest consecutive successful sampling points from the result above.
     * 3. The middle one will be the best sampling point.
     */
    uint32_t consecutive_length = 0;
    uint32_t last_success_point = 0;
    uint8_t sample_result[MSPI_TIMING_CONFIG_NUM_DEFAULT] = {0};

    init_spi1_for_tuning(is_flash);
    sweep_for_success_sample_points(reference_data, timing_config, is_flash, sample_result);
    find_max_consecutive_success_points(sample_result, MSPI_TIMING_CONFIG_NUM_DEFAULT, &consecutive_length, &last_success_point);
    select_best_tuning_config(timing_config, consecutive_length, last_success_point, reference_data, is_flash);
}
#endif  //#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING


/*------------------------------------------------------------------------------
 * FLASH Timing Tuning
 *----------------------------------------------------------------------------*/
#if MSPI_TIMING_FLASH_NEEDS_TUNING
static void get_flash_tuning_configs(mspi_timing_config_t *config)
{
#if MSPI_TIMING_FLASH_DTR_MODE
#define FLASH_MODE  DTR_MODE
#else //MSPI_TIMING_FLASH_STR_MODE
#define FLASH_MODE  STR_MODE
#endif

#if CONFIG_ESPTOOLPY_FLASHFREQ_20M
    *config = MSPI_TIMING_FLASH_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 20, FLASH_MODE);
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    *config = MSPI_TIMING_FLASH_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 40, FLASH_MODE);
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
    *config = MSPI_TIMING_FLASH_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 80, FLASH_MODE);
#elif CONFIG_ESPTOOLPY_FLASHFREQ_120M
    *config = MSPI_TIMING_FLASH_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 120, FLASH_MODE);
#endif

#undef FLASH_MODE
}

void mspi_timing_flash_tuning(void)
{
    /**
     * set SPI01 related regs to 20mhz configuration, to get reference data from FLASH
     * see detailed comments in this function (`mspi_timing_enter_low_speed_mode`)
     */
    mspi_timing_enter_low_speed_mode(true);

    //Disable the variable dummy mode when doing timing tuning
    mspi_timing_ll_enable_flash_variable_dummy(1, false);    //GD flash will read error in variable mode with 20MHz

    uint8_t reference_data[MSPI_TIMING_TEST_DATA_LEN] = {0};
    mspi_timing_config_flash_read_data(reference_data, MSPI_TIMING_FLASH_TEST_DATA_ADDR, sizeof(reference_data));
    mspi_timing_config_t timing_configs = {0};
    get_flash_tuning_configs(&timing_configs);

    do_tuning(reference_data, &timing_configs, true);
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
static void get_psram_tuning_configs(mspi_timing_config_t *config)
{
#if MSPI_TIMING_PSRAM_DTR_MODE
#define PSRAM_MODE  DTR_MODE
#else //MSPI_TIMING_PSRAM_STR_MODE
#define PSRAM_MODE  STR_MODE
#endif

#if CONFIG_SPIRAM_SPEED_40M
    *config = MSPI_TIMING_PSRAM_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 40, PSRAM_MODE);
#elif CONFIG_SPIRAM_SPEED_80M
    *config = MSPI_TIMING_PSRAM_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 80, PSRAM_MODE);
#elif CONFIG_SPIRAM_SPEED_120M
    *config = MSPI_TIMING_PSRAM_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 120, PSRAM_MODE);
#endif

#undef PSRAM_MODE
}

void mspi_timing_psram_tuning(void)
{
    /**
     * set SPI01 related regs to 20mhz configuration, to write reference data to PSRAM
     * see detailed comments in this function (`mspi_timing_enter_low_speed_mode`)
     */
    mspi_timing_enter_low_speed_mode(true);

    // write data into psram, used to do timing tuning test.
    uint8_t reference_data[MSPI_TIMING_TEST_DATA_LEN];
    for (int i=0; i < MSPI_TIMING_TEST_DATA_LEN/4; i++) {
        ((uint32_t *)reference_data)[i] = 0xa5ff005a;
    }
    mspi_timing_config_psram_write_data(reference_data, MSPI_TIMING_PSRAM_TEST_DATA_ADDR, MSPI_TIMING_TEST_DATA_LEN);
    mspi_timing_config_t timing_configs = {0};
    get_psram_tuning_configs(&timing_configs);

    //Disable the variable dummy mode when doing timing tuning
    mspi_timing_ll_enable_flash_variable_dummy(1, false);
    //Get required config, and set them to PSRAM related registers
    do_tuning(reference_data, &timing_configs, false);
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
#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
static void clear_timing_tuning_regs(bool control_spi1)
{
    mspi_timing_config_flash_set_din_mode_num(0, 0, 0);  //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    mspi_timing_config_flash_set_extra_dummy(0, 0);
    if (control_spi1) {
        mspi_timing_config_flash_set_extra_dummy(1, 0);
    } else {
        //Won't touch SPI1 registers
    }

    mspi_timing_config_psram_set_din_mode_num(0, 0, 0);
    mspi_timing_config_psram_set_extra_dummy(0, 0);
}
#endif  //#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING

void mspi_timing_enter_low_speed_mode(bool control_spi1)
{
    /**
     * Here we are going to slow the SPI1 frequency to 20Mhz, so we need to set SPI1 din_num and din_mode regs.
     *
     * Because SPI0 and SPI1 share the din_num and din_mode regs, so if we clear SPI1 din_num and din_mode to
     * 0, if the SPI0 flash module clock is still in high freq, it may not work correctly.
     *
     * Therefore, here we need to slow both the SPI0 and SPI1 and related timing tuning regs to 20Mhz configuration.
     */

    //Switch SPI1 and SPI0 clock as 20MHz, set its SPIMEM core clock as 80M and set clock division as 4
    mspi_timing_config_set_core_clock(0, MSPI_TIMING_CONFIG_CORE_CLOCK_80M);  //SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    mspi_timing_config_set_flash_clock(0, 4);
    if (control_spi1) {
        //After tuning, won't touch SPI1 again
        mspi_timing_config_set_flash_clock(1, 4);
    }

    //Set PSRAM module clock
    mspi_timing_config_set_psram_clock(0, 4);

#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
    clear_timing_tuning_regs(control_spi1);
#endif
}

#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
static void set_timing_tuning_regs_as_required(bool control_spi1)
{
    //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    mspi_timing_config_flash_set_din_mode_num(0, s_flash_best_timing_tuning_config.spi_din_mode, s_flash_best_timing_tuning_config.spi_din_num);
    mspi_timing_config_flash_set_extra_dummy(0, s_flash_best_timing_tuning_config.extra_dummy_len);
    if (control_spi1) {
        mspi_timing_config_flash_set_extra_dummy(1, s_flash_best_timing_tuning_config.extra_dummy_len);
    }

    mspi_timing_config_psram_set_din_mode_num(0, s_psram_best_timing_tuning_config.spi_din_mode, s_psram_best_timing_tuning_config.spi_din_num);
    mspi_timing_config_psram_set_extra_dummy(0, s_psram_best_timing_tuning_config.extra_dummy_len);
}
#endif  //#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING

/**
 * Set SPI0 FLASH and PSRAM module clock, din_num, din_mode and extra dummy,
 * according to the configuration got from timing tuning function (`calculate_best_flash_tuning_config`).
 * iF control_spi1 == 1, will also update SPI1 timing registers. Should only be set to 1 when do tuning.
 *
 * This function should always be called after `mspi_timing_flash_tuning` or `calculate_best_flash_tuning_config`
 */
void mspi_timing_enter_high_speed_mode(bool control_spi1)
{
    mspi_timing_config_core_clock_t core_clock = get_mspi_core_clock();
    uint32_t flash_div = get_flash_clock_divider();
    uint32_t psram_div = get_psram_clock_divider();

    //Set SPI01 core clock
    mspi_timing_config_set_core_clock(0, core_clock); //SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    //Set FLASH module clock
    mspi_timing_config_set_flash_clock(0, flash_div);
    if (control_spi1) {
        mspi_timing_config_set_flash_clock(1, flash_div);
    }
    //Set PSRAM module clock
    mspi_timing_config_set_psram_clock(0, psram_div);

#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
    set_timing_tuning_regs_as_required(true);
#endif
}

void mspi_timing_change_speed_mode_cache_safe(bool switch_down)
{
    Cache_Freeze_ICache_Enable(CACHE_FREEZE_ACK_BUSY);
    Cache_Freeze_DCache_Enable(CACHE_FREEZE_ACK_BUSY);
    if (switch_down) {
        //enter MSPI low speed mode, extra delays should be removed
        mspi_timing_enter_low_speed_mode(false);
    } else {
        //enter MSPI high speed mode, extra delays should be considered
        mspi_timing_enter_high_speed_mode(false);
    }
    Cache_Freeze_DCache_Disable();
    Cache_Freeze_ICache_Disable();
}

/*------------------------------------------------------------------------------
 * APIs to inform SPI1 Flash driver of necessary timing configurations
 *----------------------------------------------------------------------------*/
bool spi_timing_is_tuned(void)
{
#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
    return true;
#else
    return false;
#endif
}

#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
void spi_timing_get_flash_timing_param(spi_flash_hal_timing_config_t *out_timing_config)
{
    // Get clock configuration directly from system.
    out_timing_config->clock_config.spimem = mspi_timing_config_get_flash_clock_reg();

    // Get extra dummy length here. Therefore, no matter what freq, or mode.
    // If it needs tuning, it will return correct extra dummy len. If no tuning, it will return 0.

    out_timing_config->extra_dummy = s_flash_best_timing_tuning_config.extra_dummy_len;

    // Get CS setup/hold value here.
    mspi_timing_config_get_cs_timing(&out_timing_config->cs_setup, &out_timing_config->cs_hold);
}
#else
void spi_timing_get_flash_timing_param(spi_flash_hal_timing_config_t *out_timing_config)
{
    // This function shouldn't be called if timing tuning is not used.
    abort();
}
#endif // MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
