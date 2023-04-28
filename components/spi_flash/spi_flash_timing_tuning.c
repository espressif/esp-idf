/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
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
#include "esp_private/spi_flash_os.h"
#include "soc/soc.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/spi_timing_config.h"
#include "esp32s3/rom/cache.h"
#endif

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(*(arr)))

#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
const static char *TAG = "MSPI Timing";
static spi_timing_tuning_param_t s_flash_best_timing_tuning_config;
static spi_timing_tuning_param_t s_psram_best_timing_tuning_config;
#endif

/*------------------------------------------------------------------------------
 * Common settings
 *----------------------------------------------------------------------------*/
void spi_timing_set_pin_drive_strength(void)
{
    //For now, set them all to 3. Need to check after QVL test results are out. TODO: IDF-3663
    //Set default clk
    SET_PERI_REG_MASK(SPI_MEM_DATE_REG(0), SPI_MEM_SPICLK_PAD_DRV_CTL_EN);
    REG_SET_FIELD(SPI_MEM_DATE_REG(0), SPI_MEM_SPI_SMEM_SPICLK_FUN_DRV, 3);
    REG_SET_FIELD(SPI_MEM_DATE_REG(0), SPI_MEM_SPI_FMEM_SPICLK_FUN_DRV, 3);
    //Set default mspi d0 ~ d7, dqs pin drive strength
    uint32_t regs[] = {IO_MUX_GPIO27_REG, IO_MUX_GPIO28_REG,
                       IO_MUX_GPIO31_REG, IO_MUX_GPIO32_REG,
                       IO_MUX_GPIO33_REG, IO_MUX_GPIO34_REG,
                       IO_MUX_GPIO35_REG, IO_MUX_GPIO36_REG,
                       IO_MUX_GPIO37_REG};
    for (int i = 0; i < ARRAY_SIZE(regs); i++) {
        PIN_SET_DRV(regs[i], 3);
    }
}

/*------------------------------------------------------------------------------
 * Static functions to get clock configs
 *----------------------------------------------------------------------------*/
static spi_timing_config_core_clock_t get_mspi_core_clock(void)
{
    return spi_timing_config_get_core_clock();
}

static uint32_t get_flash_clock_divider(void)
{
#if CONFIG_ESPTOOLPY_FLASHFREQ_20M
    return SPI_TIMING_CORE_CLOCK_MHZ / 20;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    return SPI_TIMING_CORE_CLOCK_MHZ / 40;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
    return SPI_TIMING_CORE_CLOCK_MHZ / 80;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_120M
    return SPI_TIMING_CORE_CLOCK_MHZ / 120;
#else
    abort();
#endif
}

static uint32_t get_psram_clock_divider(void)
{
#if CONFIG_SPIRAM_SPEED_40M
    return SPI_TIMING_CORE_CLOCK_MHZ / 40;
#elif CONFIG_SPIRAM_SPEED_80M
    return SPI_TIMING_CORE_CLOCK_MHZ / 80;
#elif CONFIG_SPIRAM_SPEED_120M
    return SPI_TIMING_CORE_CLOCK_MHZ / 120;
#else
    //Will enter this branch only if PSRAM is not enable
    return 0;
#endif
}

#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
/*------------------------------------------------------------------------------
 * Static functions to do timing tuning
 *----------------------------------------------------------------------------*/
/**
 * Set timing tuning regs, in order to get successful sample points
 */
static void init_spi1_for_tuning(bool is_flash)
{
    //Get required core clock and module clock settings
    spi_timing_config_core_clock_t core_clock = get_mspi_core_clock();
    //Set SPI1 core clock. SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    spi_timing_config_set_core_clock(0, core_clock);
    //Set SPI1 module clock as required
    if (is_flash) {
        uint32_t flash_div = get_flash_clock_divider();
        spi_timing_config_set_flash_clock(1, flash_div);
        //Power on HCLK
        REG_SET_BIT(SPI_MEM_TIMING_CALI_REG(0), SPI_MEM_TIMING_CLK_ENA);
    } else {
        //We use SPI1 Flash to tune PSRAM, PSRAM timing related regs do nothing on SPI1
        uint32_t psram_div = get_psram_clock_divider();
        spi_timing_config_set_flash_clock(1, psram_div);
        //Power on HCLK
        REG_SET_BIT(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(0), SPI_MEM_SPI_SMEM_TIMING_CLK_ENA);
    }
}

/**
 * We use different SPI1 timing tuning config to read data to see if current MSPI sampling is successful.
 * The sampling result will be stored in an array. In this array, successful item will be 1, failed item will be 0.
 */
static void sweep_for_success_sample_points(uint8_t *reference_data, const spi_timing_config_t *config, bool is_flash, uint8_t *out_array)
{
    uint32_t config_idx = 0;
    uint8_t read_data[SPI_TIMING_TEST_DATA_LEN] = {0};

    for (config_idx = 0; config_idx < config->available_config_num; config_idx++) {
        memset(read_data, 0, SPI_TIMING_TEST_DATA_LEN);
#if SPI_TIMING_FLASH_NEEDS_TUNING
        if (is_flash) {
            spi_timing_config_flash_tune_din_num_mode(config->tuning_config_table[config_idx].spi_din_mode, config->tuning_config_table[config_idx].spi_din_num);
            spi_timing_config_flash_tune_dummy(config->tuning_config_table[config_idx].extra_dummy_len);
            spi_timing_config_flash_read_data(1, read_data, SPI_TIMING_FLASH_TEST_DATA_ADDR, sizeof(read_data));
        }
#endif
#if SPI_TIMING_PSRAM_NEEDS_TUNING
        if (!is_flash) {
            spi_timing_config_psram_tune_din_num_mode(config->tuning_config_table[config_idx].spi_din_mode, config->tuning_config_table[config_idx].spi_din_num);
            spi_timing_config_psram_tune_dummy(config->tuning_config_table[config_idx].extra_dummy_len);
            spi_timing_config_psram_read_data(1, read_data, SPI_TIMING_PSRAM_TEST_DATA_ADDR, SPI_TIMING_TEST_DATA_LEN);
        }
#endif
        if (memcmp(reference_data, read_data, sizeof(read_data)) == 0) {
            out_array[config_idx] = 1;
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

    *out_length = match_num > max ? match_num : max;
    *out_end_index = match_num == size ? size : end;
}

#if SPI_TIMING_FLASH_DTR_MODE || SPI_TIMING_PSRAM_DTR_MODE
static uint32_t select_best_tuning_config_dtr(spi_timing_config_t *config, uint32_t consecutive_length, uint32_t end)
{
#if (SPI_TIMING_CORE_CLOCK_MHZ == 160)
    //Core clock 160M DTR best point scheme
    uint32_t best_point;

    //Define these magic number in macros in `spi_timing_config.h`. TODO: IDF-3663
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
#else
    //won't reach here
    abort();
#endif
}
#endif

#if SPI_TIMING_FLASH_STR_MODE || SPI_TIMING_PSRAM_STR_MODE
static uint32_t select_best_tuning_config_str(spi_timing_config_t *config, uint32_t consecutive_length, uint32_t end)
{
#if (SPI_TIMING_CORE_CLOCK_MHZ == 120 || SPI_TIMING_CORE_CLOCK_MHZ == 240)
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

static void select_best_tuning_config(spi_timing_config_t *config, uint32_t consecutive_length, uint32_t end, bool is_flash)
{
    uint32_t best_point = 0;
    if (is_flash) {
#if SPI_TIMING_FLASH_DTR_MODE
        best_point = select_best_tuning_config_dtr(config, consecutive_length, end);
#elif SPI_TIMING_FLASH_STR_MODE
        best_point = select_best_tuning_config_str(config, consecutive_length, end);
#endif
        s_flash_best_timing_tuning_config = config->tuning_config_table[best_point];
    } else {
#if SPI_TIMING_PSRAM_DTR_MODE
        best_point = select_best_tuning_config_dtr(config, consecutive_length, end);
#elif SPI_TIMING_PSRAM_STR_MODE
        best_point = select_best_tuning_config_str(config, consecutive_length, end);
#endif
        s_psram_best_timing_tuning_config = config->tuning_config_table[best_point];
    }
}

static void do_tuning(uint8_t *reference_data, spi_timing_config_t *timing_config, bool is_flash)
{
    /**
     * We use SPI1 to tune the timing:
     * 1. Get all SPI1 sampling results.
     * 2. Find the longest consecutive successful sampling points from the result above.
     * 3. The middle one will be the best sampling point.
     */
    uint32_t consecutive_length = 0;
    uint32_t last_success_point = 0;
    uint8_t sample_result[SPI_TIMING_CONFIG_NUM_DEFAULT] = {0};

    init_spi1_for_tuning(is_flash);
    sweep_for_success_sample_points(reference_data, timing_config, is_flash, sample_result);
    find_max_consecutive_success_points(sample_result, SPI_TIMING_CONFIG_NUM_DEFAULT, &consecutive_length, &last_success_point);
    select_best_tuning_config(timing_config, consecutive_length, last_success_point, is_flash);
}
#endif  //#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING


/*------------------------------------------------------------------------------
 * FLASH Timing Tuning
 *----------------------------------------------------------------------------*/
#if SPI_TIMING_FLASH_NEEDS_TUNING
static void get_flash_tuning_configs(spi_timing_config_t *config)
{
#if SPI_TIMING_FLASH_DTR_MODE
#define FLASH_MODE  DTR_MODE
#else //SPI_TIMING_FLASH_STR_MODE
#define FLASH_MODE  STR_MODE
#endif

#if CONFIG_ESPTOOLPY_FLASHFREQ_20M
    *config = SPI_TIMING_FLASH_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 20, FLASH_MODE);
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    *config = SPI_TIMING_FLASH_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 40, FLASH_MODE);
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
    *config = SPI_TIMING_FLASH_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 80, FLASH_MODE);
#elif CONFIG_ESPTOOLPY_FLASHFREQ_120M
    *config = SPI_TIMING_FLASH_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 120, FLASH_MODE);
#endif

#undef FLASH_MODE
}

void spi_timing_flash_tuning(void)
{
    ESP_EARLY_LOGW("FLASH", "DO NOT USE FOR MASS PRODUCTION! Timing parameters will be updated in future IDF version.");
    /**
     * set SPI01 related regs to 20mhz configuration, to get reference data from FLASH
     * see detailed comments in this function (`spi_timing_enter_mspi_low_speed_mode`)
     */
    spi_timing_enter_mspi_low_speed_mode(true);

    //Disable the variable dummy mode when doing timing tuning
    CLEAR_PERI_REG_MASK(SPI_MEM_DDR_REG(1), SPI_MEM_SPI_FMEM_VAR_DUMMY);    //GD flash will read error in variable mode with 20MHz

    uint8_t reference_data[SPI_TIMING_TEST_DATA_LEN] = {0};
    spi_timing_config_flash_read_data(1, reference_data, SPI_TIMING_FLASH_TEST_DATA_ADDR, sizeof(reference_data));
    spi_timing_config_t timing_configs = {0};
    get_flash_tuning_configs(&timing_configs);

    do_tuning(reference_data, &timing_configs, true);
    spi_timing_enter_mspi_high_speed_mode(true);
}
#else
void spi_timing_flash_tuning(void)
{
    //Empty function for compatibility, therefore upper layer won't need to know that FLASH in which operation mode and frequency config needs to be tuned
}
#endif  //SPI_TIMING_FLASH_NEEDS_TUNING


/*------------------------------------------------------------------------------
 * PSRAM Timing Tuning
 *----------------------------------------------------------------------------*/
#if SPI_TIMING_PSRAM_NEEDS_TUNING
static void get_psram_tuning_configs(spi_timing_config_t *config)
{
#if SPI_TIMING_PSRAM_DTR_MODE
#define PSRAM_MODE  DTR_MODE
#else //SPI_TIMING_PSRAM_STR_MODE
#define PSRAM_MODE  STR_MODE
#endif

#if CONFIG_SPIRAM_SPEED_40M
    *config = SPI_TIMING_PSRAM_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 40, PSRAM_MODE);
#elif CONFIG_SPIRAM_SPEED_80M
    *config = SPI_TIMING_PSRAM_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 80, PSRAM_MODE);
#elif CONFIG_SPIRAM_SPEED_120M
    *config = SPI_TIMING_PSRAM_GET_TUNING_CONFIG(SPI_TIMING_CORE_CLOCK_MHZ, 120, PSRAM_MODE);
#endif

#undef PSRAM_MODE
}

void spi_timing_psram_tuning(void)
{
    ESP_EARLY_LOGW("PSRAM", "DO NOT USE FOR MASS PRODUCTION! Timing parameters will be updated in future IDF version.");
    /**
     * set SPI01 related regs to 20mhz configuration, to write reference data to PSRAM
     * see detailed comments in this function (`spi_timing_enter_mspi_low_speed_mode`)
     */
    spi_timing_enter_mspi_low_speed_mode(true);

    // write data into psram, used to do timing tuning test.
    uint8_t reference_data[SPI_TIMING_TEST_DATA_LEN];
    for (int i=0; i < SPI_TIMING_TEST_DATA_LEN/4; i++) {
        ((uint32_t *)reference_data)[i] = 0xa5ff005a;
    }
    spi_timing_config_psram_write_data(1, reference_data, SPI_TIMING_PSRAM_TEST_DATA_ADDR, SPI_TIMING_TEST_DATA_LEN);
    spi_timing_config_t timing_configs = {0};
    get_psram_tuning_configs(&timing_configs);

    //Disable the variable dummy mode when doing timing tuning
    CLEAR_PERI_REG_MASK(SPI_MEM_DDR_REG(1), SPI_MEM_SPI_FMEM_VAR_DUMMY);
    //Get required config, and set them to PSRAM related registers
    do_tuning(reference_data, &timing_configs, false);
    spi_timing_enter_mspi_high_speed_mode(true);
}

#else
void spi_timing_psram_tuning(void)
{
    //Empty function for compatibility, therefore upper layer won't need to know that FLASH in which operation mode and frequency config needs to be tuned
}
#endif  //SPI_TIMING_PSRAM_NEEDS_TUNING


/*------------------------------------------------------------------------------
 * APIs to make SPI0 (and SPI1) FLASH work for high/low freq
 *----------------------------------------------------------------------------*/
#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
static void clear_timing_tuning_regs(bool control_spi1)
{
    spi_timing_config_flash_set_din_mode_num(0, 0, 0);  //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    spi_timing_config_flash_set_extra_dummy(0, 0);
    if (control_spi1) {
        spi_timing_config_flash_set_extra_dummy(1, 0);
    } else {
        //Won't touch SPI1 registers
    }
}
#endif  //#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING

void spi_timing_enter_mspi_low_speed_mode(bool control_spi1)
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
    spi_timing_config_set_core_clock(0, SPI_TIMING_CONFIG_CORE_CLOCK_80M);  //SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    spi_timing_config_set_flash_clock(0, 4);
    if (control_spi1) {
        //After tuning, won't touch SPI1 again
        spi_timing_config_set_flash_clock(1, 4);
    }

#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
    clear_timing_tuning_regs(control_spi1);
#endif
}

#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
static void set_timing_tuning_regs_as_required(bool control_spi1)
{
    //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    spi_timing_config_flash_set_din_mode_num(0, s_flash_best_timing_tuning_config.spi_din_mode, s_flash_best_timing_tuning_config.spi_din_num);
    spi_timing_config_flash_set_extra_dummy(0, s_flash_best_timing_tuning_config.extra_dummy_len);
    if (control_spi1) {
        spi_timing_config_flash_set_extra_dummy(1, s_flash_best_timing_tuning_config.extra_dummy_len);
    }

    spi_timing_config_psram_set_din_mode_num(0, s_psram_best_timing_tuning_config.spi_din_mode, s_psram_best_timing_tuning_config.spi_din_num);
    spi_timing_config_psram_set_extra_dummy(0, s_psram_best_timing_tuning_config.extra_dummy_len);
}
#endif  //#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING

/**
 * Set SPI0 FLASH and PSRAM module clock, din_num, din_mode and extra dummy,
 * according to the configuration got from timing tuning function (`calculate_best_flash_tuning_config`).
 * iF control_spi1 == 1, will also update SPI1 timing registers. Should only be set to 1 when do tuning.
 *
 * This function should always be called after `spi_timing_flash_tuning` or `calculate_best_flash_tuning_config`
 */
void spi_timing_enter_mspi_high_speed_mode(bool control_spi1)
{
    spi_timing_config_core_clock_t core_clock = get_mspi_core_clock();
    uint32_t flash_div = get_flash_clock_divider();
    uint32_t psram_div = get_psram_clock_divider();

    //Set SPI01 core clock
    spi_timing_config_set_core_clock(0, core_clock); //SPI0 and SPI1 share the register for core clock. So we only set SPI0 here.
    //Set FLASH module clock
    spi_timing_config_set_flash_clock(0, flash_div);
    if (control_spi1) {
        spi_timing_config_set_flash_clock(1, flash_div);
    }
    //Set PSRAM module clock
    spi_timing_config_set_psram_clock(0, psram_div);

#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
    set_timing_tuning_regs_as_required(true);
#endif
}

void spi_timing_change_speed_mode_cache_safe(bool switch_down)
{
    Cache_Freeze_ICache_Enable(1);
    Cache_Freeze_DCache_Enable(1);
    if (switch_down) {
        //enter MSPI low speed mode, extra delays should be removed
        spi_timing_enter_mspi_low_speed_mode(false);
    } else {
        //enter MSPI high speed mode, extra delays should be considered
        spi_timing_enter_mspi_high_speed_mode(false);
    }
    Cache_Freeze_DCache_Disable();
    Cache_Freeze_ICache_Disable();
}

/*------------------------------------------------------------------------------
 * APIs to inform SPI1 Flash driver of necessary timing configurations
 *----------------------------------------------------------------------------*/
bool spi_timing_is_tuned(void)
{
#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
    return true;
#else
    return false;
#endif
}

#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
void spi_timing_get_flash_timing_param(spi_flash_hal_timing_config_t *out_timing_config)
{
    // Get clock configuration directly from system.
    out_timing_config->clock_config.spimem = spi_timing_config_get_flash_clock_reg();

    // Get extra dummy length here. Therefore, no matter what freq, or mode.
    // If it needs tuning, it will return correct extra dummy len. If no tuning, it will return 0.

    out_timing_config->extra_dummy = s_flash_best_timing_tuning_config.extra_dummy_len;

    // Get CS setup/hold value here.
    spi_timing_config_get_cs_timing(&out_timing_config->cs_setup, &out_timing_config->cs_hold);
}
#else
void spi_timing_get_flash_timing_param(spi_flash_hal_timing_config_t *out_timing_config)
{
    // This function shouldn't be called if timing tuning is not used.
    abort();
}
#endif // SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
