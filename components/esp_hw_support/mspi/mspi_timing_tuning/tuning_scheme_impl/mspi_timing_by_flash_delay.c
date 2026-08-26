/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 *
 * This file contains configuration APIs doing MSPI timing tuning by Flash delay
 * This file will only be built when `SOC_MEMSPI_TIMING_TUNING_BY_FLASH_DELAY == 1`
 */

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_log.h"
#include "esp_private/mspi_timing_by_flash_delay.h"
#include "mspi_timing_tuning_configs.h"
#include "esp_private/mspi_timing_config.h"
#include "hal/mspi_ll.h"
#include "rom/spi_flash.h"
#include "esp_private/spi_flash_os.h"

ESP_LOG_ATTR_TAG(TAG, "Flash Delay");

void mspi_timing_flash_init(uint32_t flash_freq_mhz)
{
    mspi_timing_config_set_flash_clock(flash_freq_mhz, MSPI_TIMING_SPEED_MODE_NORMAL_PERF, true);

    //Power on HCLK
    mspi_timinng_ll_enable_flash_timing_adjust_clk(MSPI_TIMING_LL_MSPI_ID_0);

    ESP_DRAM_LOGD(TAG, "init rom dummy val: %d", g_rom_spiflash_dummy_len_plus[1]);
}

//-------------------------------------FLASH timing tuning register config-------------------------------------//
void mspi_timing_get_flash_tuning_configs(mspi_timing_config_t *config)
{
#if CONFIG_ESPTOOLPY_FLASHFREQ_120M
    *config = MSPI_TIMING_FLASH_GET_TUNING_CONFIG(MSPI_TIMING_FLASH_CORE_CLOCK_MHZ, 120, STR_MODE);
#else
    assert(false && "should never reach here");
#endif
}

static void s_set_flash_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num)
{
    mspi_timing_ll_set_flash_din_mode(spi_num, din_mode);
    mspi_timing_ll_set_flash_din_num(spi_num, din_num);
}

static void s_set_flash_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
    mspi_timing_ll_set_flash_extra_dummy(spi_num, extra_dummy);
}

void mspi_timing_config_flash_set_tuning_regs(const void *configs, uint8_t id)
{
    const mspi_timing_tuning_param_t *params = &((mspi_timing_config_t *)configs)->tuning_config_table[id];
    /**
     * 1. SPI_MEM_DINx_MODE(1), SPI_MEM_DINx_NUM(1) are meaningless
     *    SPI0 and SPI1 share the SPI_MEM_DINx_MODE(0), SPI_MEM_DINx_NUM(0) for FLASH timing tuning
     * 2. We use SPI1 to get the best Flash timing tuning (mode and num) config
     */
    s_set_flash_din_mode_num(MSPI_TIMING_LL_MSPI_ID_0, params->spi_din_mode, params->spi_din_num);
    s_set_flash_extra_dummy(MSPI_TIMING_LL_MSPI_ID_1, params->extra_dummy_len);
}

//-------------------------------------------FLASH Read/Write------------------------------------------//
void mspi_timing_config_flash_read_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
#if CONFIG_ESPTOOLPY_FLASHMODE_QIO && CONFIG_SPI_FLASH_HPM_ON
    g_rom_spiflash_dummy_len_plus[1] = 4;
#endif
    esp_rom_spiflash_read(addr, (uint32_t *)buf, len);

    int spi1_usr_dummy = 0;
    int spi1_extra_dummy = 0;
    int spi0_usr_dummy = 0;
    int spi0_extra_dummy = 0;
    mspi_timing_ll_get_flash_dummy(MSPI_TIMING_LL_MSPI_ID_0, &spi0_usr_dummy, &spi0_extra_dummy);
    mspi_timing_ll_get_flash_dummy(MSPI_TIMING_LL_MSPI_ID_1, &spi1_usr_dummy, &spi1_extra_dummy);
    ESP_DRAM_LOGD(TAG, "spi0_usr_dummy: %d, spi0_extra_dummy: %d, spi1_usr_dummy: %d, spi1_extra_dummy: %d", spi0_usr_dummy, spi0_extra_dummy, spi1_usr_dummy, spi1_extra_dummy);
}

/*-------------------------------------------------------------------------------------------------
 * Best Timing Tuning Params Selection
 *-------------------------------------------------------------------------------------------------*/
static uint32_t s_select_best_tuning_config_str(const mspi_timing_config_t *configs, uint32_t consecutive_length, uint32_t end)
{
    //STR best point scheme
    uint32_t best_point;

    if (consecutive_length < 3) {
        //tuning fails, select default point, and generate a warning
        best_point = configs->flash_default_config_id;
        ESP_DRAM_LOGW(TAG, "tuning fail, best point is fallen back to index %"PRIu32"", best_point);
    } else {
        best_point = end - consecutive_length / 2;
        ESP_DRAM_LOGD(TAG, "tuning success, best point is index %"PRIu32"", best_point);
    }

    return best_point;
}

static uint32_t s_select_best_tuning_config(const mspi_timing_config_t *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr, bool is_flash)
{
    uint32_t best_point = 0;
    assert(!is_ddr);
    best_point = s_select_best_tuning_config_str(configs, consecutive_length, end);

    return best_point;
}

uint32_t mspi_timing_flash_select_best_tuning_config(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr)
{
    const mspi_timing_config_t *timing_configs = (const mspi_timing_config_t *)configs;
    uint32_t best_point = s_select_best_tuning_config(timing_configs, consecutive_length, end, reference_data, is_ddr, true);
    ESP_DRAM_LOGD(TAG, "Flash timing tuning index: %"PRIu32"", best_point);

    return best_point;
}

static mspi_timing_tuning_param_t s_flash_best_timing_tuning_config;
static mspi_timing_tuning_param_t s_psram_best_timing_tuning_config;

void mspi_timing_flash_set_best_tuning_config(const void *configs, uint8_t best_id)
{
    s_flash_best_timing_tuning_config = ((const mspi_timing_config_t *)configs)->tuning_config_table[best_id];
}

void mspi_timing_psram_set_best_tuning_config(const void *configs, uint8_t best_id)
{
    s_psram_best_timing_tuning_config = ((const mspi_timing_config_t *)configs)->tuning_config_table[best_id];
}

/*-------------------------------------------------------------------------------------------------
 * Best Timing Tuning Params Clear / Set
 *-------------------------------------------------------------------------------------------------*/
void mspi_timing_flash_config_clear_tuning_regs(bool control_both_mspi)
{
    s_set_flash_din_mode_num(MSPI_TIMING_LL_MSPI_ID_0, 0, 0);  //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    s_set_flash_extra_dummy(MSPI_TIMING_LL_MSPI_ID_0, 0);

    //Won't touch SPI1 registers if not control_both_mspi
    if (control_both_mspi) {
        s_set_flash_extra_dummy(MSPI_TIMING_LL_MSPI_ID_1, 0);
    }
}

#if MSPI_TIMING_FLASH_NEEDS_TUNING && CONFIG_SPI_FLASH_HPM_ON
static uint32_t spi_timing_config_get_dummy(void)
{
    mspi_timing_ll_flash_mode_t mode = mspi_timing_ll_get_flash_mode(0);

    if (spi_flash_hpm_dummy_adjust()) { // HPM-DC is enabled
        const spi_flash_hpm_dummy_conf_t *hpm_dummy = spi_flash_hpm_get_dummy();
        switch (mode) {
            case MSPI_TIMING_LL_FLASH_QIO_MODE:
                return hpm_dummy->qio_dummy - 1;
            case MSPI_TIMING_LL_FLASH_QUAD_MODE:
                return hpm_dummy->qout_dummy - 1;
            case MSPI_TIMING_LL_FLASH_DIO_MODE:
                return hpm_dummy->dio_dummy - 1;
            case MSPI_TIMING_LL_FLASH_DUAL_MODE:
                return hpm_dummy->dout_dummy - 1;
            case MSPI_TIMING_LL_FLASH_FAST_MODE:
                return hpm_dummy->fastrd_dummy - 1;
            case MSPI_TIMING_LL_FLASH_SLOW_MODE:
                return 0;
            default:
                abort();
        }
    } else { // HPM-DC is not enabled
        switch (mode) {
            case MSPI_TIMING_LL_FLASH_QIO_MODE:
                return SPI1_R_QIO_DUMMY_CYCLELEN;
            case MSPI_TIMING_LL_FLASH_QUAD_MODE:
                return SPI1_R_FAST_DUMMY_CYCLELEN;
            case MSPI_TIMING_LL_FLASH_DIO_MODE:
                return SPI1_R_DIO_DUMMY_CYCLELEN;
            case MSPI_TIMING_LL_FLASH_DUAL_MODE:
                return SPI1_R_FAST_DUMMY_CYCLELEN;
            case MSPI_TIMING_LL_FLASH_FAST_MODE:
                return SPI1_R_FAST_DUMMY_CYCLELEN;
            case MSPI_TIMING_LL_FLASH_SLOW_MODE:
                return 0;
            default:
                abort();
        }
    }
}
#endif // MSPI_TIMING_FLASH_NEEDS_TUNING && CONFIG_SPI_FLASH_HPM_ON

void mspi_timing_flash_config_set_tuning_regs(bool control_both_mspi)
{
    //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    s_set_flash_din_mode_num(MSPI_TIMING_LL_MSPI_ID_0, s_flash_best_timing_tuning_config.spi_din_mode, s_flash_best_timing_tuning_config.spi_din_num);
    s_set_flash_extra_dummy(0, s_flash_best_timing_tuning_config.extra_dummy_len);

    if (control_both_mspi) {
        s_set_flash_extra_dummy(MSPI_TIMING_LL_MSPI_ID_1, s_flash_best_timing_tuning_config.extra_dummy_len);
    } else {
        //Won't touch SPI1 registers
    }

#if MSPI_TIMING_FLASH_NEEDS_TUNING && CONFIG_SPI_FLASH_HPM_ON
    uint32_t dummy_len = spi_timing_config_get_dummy();
#if CONFIG_ESPTOOLPY_FLASHMODE_QIO && !CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_QUAD_FLASH
    // In the ROM configuration, when esp_rom_spi_set_rd_cmd_bit_len issues the read command in QIO mode, it automatically inserts two dummy cycles.
    // Therefore, when validating the dummy cycles at 120 MHz, the effective value should be 10 − 2, i.e. 8 dummy cycles.
    // However, when the 32 MHz cache access configuration is enabled, the corresponding register will be overwritten,
    // so the extra two dummy cycles no longer exist. In that case, the dummy cycle setting should simply be 10.
    mspi_timing_ll_set_flash_user_dummy(MSPI_TIMING_LL_MSPI_ID_0, dummy_len - 2);
#else
    mspi_timing_ll_set_flash_user_dummy(MSPI_TIMING_LL_MSPI_ID_0, dummy_len);
#endif // CONFIG_ESPTOOLPY_FLASHMODE_QIO && !CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_QUAD_FLASH
#endif

    int spi1_usr_dummy = 0;
    int spi1_extra_dummy = 0;
    int spi0_usr_dummy = 0;
    int spi0_extra_dummy = 0;
    mspi_timing_ll_get_flash_dummy(MSPI_TIMING_LL_MSPI_ID_0, &spi0_usr_dummy, &spi0_extra_dummy);
    mspi_timing_ll_get_flash_dummy(MSPI_TIMING_LL_MSPI_ID_1, &spi1_usr_dummy, &spi1_extra_dummy);
    ESP_DRAM_LOGD(TAG, "spi0_usr_dummy: %d, spi0_extra_dummy: %d, spi1_usr_dummy: %d, spi1_extra_dummy: %d", spi0_usr_dummy, spi0_extra_dummy, spi1_usr_dummy, spi1_extra_dummy);
}

/*-------------------------------------------------------------------------------------------------
 * To let upper lay (spi_flash_timing_tuning.c) to know the necessary timing registers
 *-------------------------------------------------------------------------------------------------*/
/**
 * Get the SPI1 Flash CS timing setting. The setup time and hold time are both realistic cycles.
 * @note On ESP32-P4, SPI0/1 share the Flash CS timing registers. Therefore, we should not change these values.
 * @note This function inform `spi_flash_timing_tuning.c` (driver layer) of the cycle,
 * and other component (esp_flash driver) should get these cycle and configure the registers accordingly.
 */
void mspi_timing_config_get_cs_timing(uint8_t *setup_time, uint32_t *hold_time)
{
    *setup_time = mspi_timing_ll_get_cs_setup_val(MSPI_TIMING_LL_MSPI_ID_0);
    *hold_time = mspi_timing_ll_get_cs_hold_val(MSPI_TIMING_LL_MSPI_ID_0);
    /**
     * The logic here is, if setup_en / hold_en is false, then we return the realistic cycle number,
     * which is 0. If true, then the realistic cycle number is (reg_value + 1)
     */
    if (mspi_timing_ll_is_cs_setup_enabled(MSPI_TIMING_LL_MSPI_ID_0)) {
        *setup_time += 1;
    } else {
        *setup_time = 0;
    }
    if (mspi_timing_ll_is_cs_hold_enabled(MSPI_TIMING_LL_MSPI_ID_0)) {
        *hold_time += 1;
    } else {
        *hold_time = 0;
    }
}

/**
 * Get the SPI1 Flash clock setting.
 * @note Similarly, this function inform `spi_flash_timing_tuning.c` (driver layer) of the clock setting,
 * and other component (esp_flash driver) should get these and configure the registers accordingly.
 */
uint32_t mspi_timing_config_get_flash_clock_reg(void)
{
    return mspi_timing_ll_get_clock_reg(MSPI_TIMING_LL_MSPI_ID_1);
}

uint8_t mspi_timing_config_get_flash_extra_dummy(void)
{
    //use hw extra dummy
    return 0;
}

uint32_t mspi_timing_config_get_flash_fdummy_rin(void)
{
    return mspi_timing_ll_get_invalid_dqs_mask(MSPI_TIMING_LL_MSPI_ID_1);
}
