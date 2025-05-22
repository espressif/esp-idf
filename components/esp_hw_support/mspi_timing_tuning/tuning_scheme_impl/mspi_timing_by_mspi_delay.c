/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 *
 * This file contains configuration APIs doing MSPI timing tuning by MSPI delay
 * This file will only be built, when `SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY == 1`
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "string.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_log.h"
#include "rom/spi_flash.h"
#include "hal/mspi_ll.h"
#include "hal/psram_types.h"
#include "hal/psram_ctrlr_ll.h"
#include "esp_private/mspi_timing_config.h"
#include "esp_private/mspi_timing_by_mspi_delay.h"
#include "mspi_timing_tuning_configs.h"

#define QPI_PSRAM_FAST_READ            0XEB
#define QPI_PSRAM_WRITE                0X38
#define QPI_PSRAM_FAST_READ_DUMMY      6

const static char *TAG = "MSPI Timing";

//-------------------------------------FLASH timing tuning register config-------------------------------------//
static void s_set_flash_din_mode_num(uint8_t mspi_id, uint8_t din_mode, uint8_t din_num)
{
    mspi_timing_ll_set_flash_din_mode(mspi_id, din_mode);
    mspi_timing_ll_set_flash_din_num(mspi_id, din_num);
}

static void s_set_flash_extra_dummy(uint8_t mspi_id, uint8_t extra_dummy)
{
    mspi_timing_ll_set_flash_extra_dummy(mspi_id, extra_dummy);
}

//-------------------------------------PSRAM timing tuning register config-------------------------------------//
void mspi_timing_psram_init(uint32_t psram_freq_mhz)
{
    mspi_timing_config_set_flash_clock(psram_freq_mhz, MSPI_TIMING_SPEED_MODE_NORMAL_PERF, true);

    //Power on HCLK
    mspi_timinng_ll_enable_psram_timing_adjust_clk(MSPI_TIMING_LL_MSPI_ID_0);
}

void mspi_timing_get_psram_tuning_configs(mspi_timing_config_t *config)
{
#if MSPI_TIMING_PSRAM_DTR_MODE
#define PSRAM_MODE  DTR_MODE
#else //MSPI_TIMING_PSRAM_STR_MODE
#define PSRAM_MODE  STR_MODE
#endif

#if CONFIG_SPIRAM_SPEED_80M
    *config = MSPI_TIMING_PSRAM_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 80, STR_MODE);
#elif CONFIG_SPIRAM_SPEED_120M
    *config = MSPI_TIMING_PSRAM_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 120, PSRAM_MODE);
#else
    assert(false && "should never reach here");
#endif

#undef PSRAM_MODE
}

static void s_set_psram_din_mode_num(uint8_t mspi_id, uint8_t din_mode, uint8_t din_num)
{
    mspi_timing_ll_set_psram_din_mode(mspi_id, din_mode);
    mspi_timing_ll_set_psram_din_num(mspi_id, din_num);
}

static void s_set_psram_extra_dummy(uint8_t mspi_id, uint8_t extra_dummy)
{
    mspi_timing_ll_set_psram_extra_dummy(mspi_id, extra_dummy);
}

void mspi_timing_config_psram_set_tuning_regs(const void *configs, uint8_t id)
{
    const mspi_timing_tuning_param_t *params = &((mspi_timing_config_t *)configs)->tuning_config_table[id];
    /**
     * 1. SPI_MEM_SPI_SMEM_DINx_MODE(1), SPI_MEM_SPI_SMEM_DINx_NUM(1) are meaningless
     *    SPI0 and SPI1 share the SPI_MEM_SPI_SMEM_DINx_MODE(0), SPI_MEM_SPI_SMEM_DINx_NUM(0) for PSRAM timing tuning
     * 2. We use SPI1 to get the best PSRAM timing tuning (mode and num) config
     */
    s_set_psram_din_mode_num(MSPI_TIMING_LL_MSPI_ID_0, params->spi_din_mode, params->spi_din_num);

    s_set_flash_extra_dummy(MSPI_TIMING_LL_MSPI_ID_1, params->extra_dummy_len);
}

//-------------------------------------------PSRAM Read/Write------------------------------------------//
static void psram_exec_cmd(int mspi_id, psram_cmd_mode_t mode,
                    uint32_t cmd, int cmd_bit_len,
                    uint32_t addr, int addr_bit_len,
                    int dummy_bits,
                    uint8_t* mosi_data, int mosi_bit_len,
                    uint8_t* miso_data, int miso_bit_len,
                    uint32_t cs_mask,
                    bool is_write_erase_operation)
{
    esp_rom_spiflash_read_mode_t rd_mode = (mode == PSRAM_HAL_CMD_QPI) ? ESP_ROM_SPIFLASH_QIO_MODE : ESP_ROM_SPIFLASH_SLOWRD_MODE;

    esp_rom_spi_set_op_mode(mspi_id, rd_mode);
    if (mode == PSRAM_HAL_CMD_QPI) {
        psram_ctrlr_ll_enable_quad_command(PSRAM_CTRLR_LL_MSPI_ID_1, true);
    }
    psram_ctrlr_ll_common_transaction_base(mspi_id, rd_mode,
                                           cmd, cmd_bit_len,
                                           addr, addr_bit_len,
                                           dummy_bits,
                                           mosi_data, mosi_bit_len,
                                           miso_data, miso_bit_len,
                                           cs_mask,
                                           is_write_erase_operation);
}

#define FIFO_SIZE_BYTE 32
static void s_psram_write_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    if (len % FIFO_SIZE_BYTE != 0) {
        ESP_EARLY_LOGE(TAG, "wrong length %d", len);
        assert(false);
    }

    uint8_t mspi_id = MSPI_TIMING_LL_MSPI_ID_1;
    uint8_t cmd = QPI_PSRAM_WRITE;
    uint8_t cmd_len = 8;
    uint8_t dummy_len = 0;
    uint8_t addr_len = 24;
    psram_cmd_mode_t spi_mode = PSRAM_HAL_CMD_QPI;
    for (uint32_t idx= 0; idx < len / FIFO_SIZE_BYTE; idx++) {
        psram_exec_cmd(mspi_id, spi_mode,
                    cmd, cmd_len,
                    addr + idx * FIFO_SIZE_BYTE, addr_len,
                    dummy_len,
                    buf + idx * FIFO_SIZE_BYTE, FIFO_SIZE_BYTE * 8,
                    NULL, 0,
                    PSRAM_LL_CS_SEL,
                    false);
    }
}

static void s_psram_read_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    if (len % FIFO_SIZE_BYTE != 0) {
        ESP_EARLY_LOGE(TAG, "wrong length %d", len);
        assert(false);
    }

    uint8_t mspi_id = MSPI_TIMING_LL_MSPI_ID_1;
    uint8_t cmd = QPI_PSRAM_FAST_READ;
    uint8_t cmd_len = 8;
    uint8_t dummy_len = QPI_PSRAM_FAST_READ_DUMMY;
    uint8_t addr_len = 24;
    psram_cmd_mode_t spi_mode = PSRAM_HAL_CMD_QPI;
    for (uint32_t idx = 0; idx < len/FIFO_SIZE_BYTE; idx++) {
        psram_exec_cmd(mspi_id, spi_mode,
                    cmd, cmd_len,
                    addr + idx*FIFO_SIZE_BYTE, addr_len,
                    dummy_len,
                    NULL, 0,
                    buf + idx*FIFO_SIZE_BYTE, FIFO_SIZE_BYTE * 8,
                    PSRAM_LL_CS_SEL,
                    false);
    }
}

static void s_psram_execution(uint8_t *buf, uint32_t addr, uint32_t len, bool is_read)
{
    while (len) {
        uint32_t length = MIN(len, 32);
        if (is_read) {
            s_psram_read_data(buf, addr, length);
        } else {
            s_psram_write_data(buf, addr, length);
        }
        addr += length;
        buf += length;
        len -= length;
    }
}

void mspi_timing_config_psram_prepare_reference_data(uint8_t *buf, uint32_t len)
{
    assert((len == MSPI_TIMING_TEST_DATA_LEN) && (len % 4 == 0));
    for (int i = 0; i < len / 4; i++) {
        ((uint32_t *)buf)[i] = 0xa5ff005a;
    }
}

void mspi_timing_config_psram_write_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    s_psram_execution(buf, addr, len, false);
}

void mspi_timing_config_psram_read_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    s_psram_execution(buf, addr, len, true);
}

/*-------------------------------------------------------------------------------------------------
 * Best Timing Tuning Params Selection
 *-------------------------------------------------------------------------------------------------*/
static uint32_t s_select_best_tuning_config_str(const mspi_timing_config_t *configs, uint32_t consecutive_length, uint32_t end)
{
    //STR best point scheme
    uint32_t best_point = 0;

    if (consecutive_length < 3) {
        //tuning is FAIL, select default point, and generate a warning
        best_point = configs->default_config_id;
        ESP_EARLY_LOGW(TAG, "tuning fail, best point is fallen back to index %"PRIu32"", best_point);
    } else {
        best_point = end - consecutive_length / 2;
        ESP_EARLY_LOGI(TAG, "tuning success, best point is index %"PRIu32"", best_point);
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
    ESP_EARLY_LOGI(TAG, "Flash timing tuning index: %"PRIu32"", best_point);

    return best_point;
}

uint32_t mspi_timing_psram_select_best_tuning_config(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr)
{
    const mspi_timing_config_t *timing_configs = (const mspi_timing_config_t *)configs;
    uint32_t best_point = s_select_best_tuning_config(timing_configs, consecutive_length, end, reference_data, is_ddr, false);
    ESP_EARLY_LOGI(TAG, "PSRAM timing tuning index: %"PRIu32"", best_point);

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

void mspi_timing_flash_config_set_tuning_regs(bool control_both_mspi)
{
    //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    s_set_flash_din_mode_num(MSPI_TIMING_LL_MSPI_ID_0, s_flash_best_timing_tuning_config.spi_din_mode, s_flash_best_timing_tuning_config.spi_din_num);
    s_set_flash_extra_dummy(MSPI_TIMING_LL_MSPI_ID_0, s_flash_best_timing_tuning_config.extra_dummy_len);
    if (control_both_mspi) {
        s_set_flash_extra_dummy(MSPI_TIMING_LL_MSPI_ID_1, s_flash_best_timing_tuning_config.extra_dummy_len);
    } else {
        //Won't touch SPI1 registers
    }

    int spi1_usr_dummy = 0;
    int spi1_extra_dummy = 0;
    int spi0_usr_dummy = 0;
    int spi0_extra_dummy = 0;
    mspi_timing_ll_get_flash_dummy(MSPI_TIMING_LL_MSPI_ID_0, &spi0_usr_dummy, &spi0_extra_dummy);
    mspi_timing_ll_get_flash_dummy(MSPI_TIMING_LL_MSPI_ID_1, &spi1_usr_dummy, &spi1_extra_dummy);
    ESP_EARLY_LOGV(TAG, "flash, spi0_usr_dummy: %d, spi0_extra_dummy: %d, spi1_usr_dummy: %d, spi1_extra_dummy: %d", spi0_usr_dummy, spi0_extra_dummy, spi1_usr_dummy, spi1_extra_dummy);
}

void mspi_timing_psram_config_clear_tuning_regs(bool control_both_mspi)
{
    (void)control_both_mspi;  //for compatibility
    s_set_psram_din_mode_num(MSPI_TIMING_LL_MSPI_ID_0, 0, 0);
    s_set_psram_extra_dummy(MSPI_TIMING_LL_MSPI_ID_0, 0);
}

void mspi_timing_psram_config_set_tuning_regs(bool control_both_mspi)
{
    (void)control_both_mspi;  //for compatibility
    s_set_psram_din_mode_num(MSPI_TIMING_LL_MSPI_ID_0, s_psram_best_timing_tuning_config.spi_din_mode, s_psram_best_timing_tuning_config.spi_din_num);
    s_set_psram_extra_dummy(MSPI_TIMING_LL_MSPI_ID_0, s_psram_best_timing_tuning_config.extra_dummy_len);

    int spi0_usr_rdummy = 0;
    int spi0_extra_dummy = 0;
    mspi_timing_ll_get_psram_dummy(MSPI_TIMING_LL_MSPI_ID_0, &spi0_usr_rdummy, &spi0_extra_dummy);
    ESP_EARLY_LOGV(TAG, "psram, spi0_usr_rdummy: %d, spi0_extra_dummy: %d", spi0_usr_rdummy, spi0_extra_dummy);
}

/*-------------------------------------------------------------------------------------------------
 * To let upper lay (spi_flash_timing_tuning.c) to know the necessary timing registers
 *-------------------------------------------------------------------------------------------------*/
/**
 * Get the SPI1 Flash CS timing setting. The setup time and hold time are both realistic cycles.
 * @note On ESP32-S3, SPI0/1 share the Flash CS timing registers. Therefore, we should not change these values.
 * @note This function inform `spi_flash_timing_tuning.c` (driver layer) of the cycle,
 * and other component (esp_flash driver) should get these cycle and configure the registers accordingly.
 */
void mspi_timing_config_get_cs_timing(uint8_t *setup_time, uint32_t *hold_time)
{
    *setup_time = mspi_timing_ll_get_cs_setup_val(MSPI_TIMING_LL_MSPI_ID_0);
    *hold_time = mspi_timing_ll_get_cs_hold_val(0);
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
#if MSPI_TIMING_FLASH_NEEDS_TUNING
    return s_flash_best_timing_tuning_config.extra_dummy_len;
#else
    return 0;
#endif
}
