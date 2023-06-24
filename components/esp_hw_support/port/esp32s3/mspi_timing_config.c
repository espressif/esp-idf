/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "string.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_log.h"
#include "soc/spi_mem_reg.h"
#include "hal/mspi_timing_tuning_ll.h"
#include "../../mspi_timing_config.h"
#include "bootloader_flash.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"

#define OPI_PSRAM_SYNC_READ           0x0000
#define OPI_PSRAM_SYNC_WRITE          0x8080
#define OCT_PSRAM_RD_DUMMY_NUM        (2*(10-1))
#define OCT_PSRAM_WR_DUMMY_NUM        (2*(5-1))

#define QPI_PSRAM_FAST_READ            0XEB
#define QPI_PSRAM_WRITE                0X38
#define QPI_PSRAM_FAST_READ_DUMMY      6

#define NOT_INIT_INT                  127

//-------------------------------------MSPI Clock Setting-------------------------------------//
mspi_timing_config_core_clock_t mspi_timing_config_get_core_clock(void)
{
    switch (MSPI_TIMING_CORE_CLOCK_MHZ) {
        case 80:
            return MSPI_TIMING_CONFIG_CORE_CLOCK_80M;
        case 120:
            return MSPI_TIMING_CONFIG_CORE_CLOCK_120M;
        case 160:
            return MSPI_TIMING_CONFIG_CORE_CLOCK_160M;
        case 240:
            return MSPI_TIMING_CONFIG_CORE_CLOCK_240M;
        default:
            abort();
    }
}

void mspi_timing_config_set_core_clock(uint8_t spi_num, mspi_timing_config_core_clock_t core_clock)
{
    uint32_t reg_val = 0;

    switch (core_clock) {
        case MSPI_TIMING_CONFIG_CORE_CLOCK_80M:
            reg_val = 0;
            break;
        case MSPI_TIMING_CONFIG_CORE_CLOCK_120M:
            reg_val = 1;
            break;
        case MSPI_TIMING_CONFIG_CORE_CLOCK_160M:
            reg_val = 2;
            break;
        case MSPI_TIMING_CONFIG_CORE_CLOCK_240M:
            reg_val = 3;
            break;
        default:
            abort();
    }

    mspi_timing_ll_set_core_clock_divider(spi_num, reg_val);
}

void mspi_timing_config_set_flash_clock(uint8_t spi_num, uint32_t freqdiv)
{
    assert(freqdiv > 0);
    mspi_timing_ll_set_flash_clock(spi_num, freqdiv);
}

void mspi_timing_config_set_psram_clock(uint8_t spi_num, uint32_t freqdiv)
{
    mspi_timing_ll_set_psram_clock(spi_num, freqdiv);
}

/////////////////////////////////////////TIMING TUNING IS NEEDED//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
//If one of the FLASH / PSRAM or both of them need timing tuning, we should build following code
typedef enum {
    PSRAM_CMD_QPI,
    PSRAM_CMD_SPI,
} psram_cmd_mode_t;

static uint8_t s_rom_flash_extra_dummy[2] = {NOT_INIT_INT, NOT_INIT_INT};

#if CONFIG_SPIRAM_MODE_QUAD
static uint8_t s_psram_extra_dummy;
extern void psram_exec_cmd(int spi_num, psram_cmd_mode_t mode,
                           uint32_t cmd, int cmd_bit_len,
                           uint32_t addr, int addr_bit_len,
                           int dummy_bits,
                           uint8_t* mosi_data, int mosi_bit_len,
                           uint8_t* miso_data, int miso_bit_len,
                           uint32_t cs_mask,
                           bool is_write_erase_operation);
#endif

//-------------------------------------FLASH timing tuning register config-------------------------------------//
void mspi_timing_config_flash_set_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num)
{
    mspi_timing_ll_set_flash_din_mode(spi_num, din_mode);
    mspi_timing_ll_set_flash_din_num(spi_num, din_num);
}

static uint32_t spi_timing_config_get_dummy(void)
{
    mspi_timing_ll_flash_mode_t mode = mspi_timing_ll_get_flash_mode(0);
    if (mode == MSPI_TIMING_LL_FLASH_OPI_MODE) {
        abort();
    }

#if CONFIG_SPI_FLASH_HPM_DC_ON
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
    } else
#endif
    { // HPM-DC is not enabled
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

void mspi_timing_config_flash_set_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
    if (bootloader_flash_is_octal_mode_enabled()) {
        mspi_timing_ll_set_octal_flash_extra_dummy(spi_num, extra_dummy);
        return;
    }
    /**
     * HW workaround:
     * The `SPI_MEM_TIMING_CALI_REG` register is only used for OPI on 728
     * Here we only need to update this global variable for extra dummy. Since we use the ROM Flash API, which will set the dummy based on this.
     * We only initialise the SPI0. And leave the SPI1 for flash driver to configure.
     */
    if (s_rom_flash_extra_dummy[spi_num] == NOT_INIT_INT) {
        s_rom_flash_extra_dummy[spi_num] = g_rom_spiflash_dummy_len_plus[spi_num];
    }
    g_rom_spiflash_dummy_len_plus[spi_num] = s_rom_flash_extra_dummy[spi_num] + extra_dummy;

    // Only Quad Flash will run into this branch.
    uint32_t dummy = spi_timing_config_get_dummy();
    mspi_timing_ll_set_quad_flash_dummy(spi_num, dummy + g_rom_spiflash_dummy_len_plus[spi_num]);
}

//-------------------------------------PSRAM timing tuning register config-------------------------------------//
void mspi_timing_config_psram_set_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num)
{
    mspi_timing_ll_set_psram_din_mode(spi_num, din_mode);
    mspi_timing_ll_set_psram_din_num(spi_num, din_num);
}

void mspi_timing_config_psram_set_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
#if CONFIG_SPIRAM_MODE_OCT
    mspi_timing_ll_set_octal_psram_extra_dummy(spi_num, extra_dummy);
#elif CONFIG_SPIRAM_MODE_QUAD
    //HW workaround: Use normal dummy register to set extra dummy, the calibration dedicated extra dummy register doesn't work for quad mode
    mspi_timing_ll_set_quad_psram_dummy(spi_num, (QPI_PSRAM_FAST_READ_DUMMY + extra_dummy - 1));
#endif
}

//-------------------------------------------FLASH/PSRAM Read/Write------------------------------------------//
void mspi_timing_config_flash_read_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    if (bootloader_flash_is_octal_mode_enabled()) {
        // note that in spi_flash_read API, there is a wait-idle stage, since flash can only be read in idle state.
        // but after we change the timing settings, we might not read correct idle status via RDSR.
        // so, here we should use a read API that won't check idle status.
        mspi_timing_ll_clear_fifo(1);
        esp_rom_opiflash_read_raw(addr, buf, len);
    } else {
        esp_rom_spiflash_read(addr, (uint32_t *)buf, len);
    }
}

static void s_psram_write_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
#if CONFIG_SPIRAM_MODE_OCT
    esp_rom_opiflash_exec_cmd(1, ESP_ROM_SPIFLASH_OPI_DTR_MODE,
                            OPI_PSRAM_SYNC_WRITE, 16,
                            addr, 32,
                            OCT_PSRAM_WR_DUMMY_NUM,
                            buf, len * 8,
                            NULL, 0,
                            BIT(1),
                            false);
#elif CONFIG_SPIRAM_MODE_QUAD
    psram_exec_cmd(1, 0,
                   QPI_PSRAM_WRITE, 8,
                   addr, 24,
                   0,
                   buf, len * 8,
                   NULL, 0,
                   SPI_MEM_CS1_DIS_M,
                   false);
#endif
}

static void s_psram_read_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
#if CONFIG_SPIRAM_MODE_OCT
    mspi_timing_ll_clear_fifo(1);
    esp_rom_opiflash_exec_cmd(1, ESP_ROM_SPIFLASH_OPI_DTR_MODE,
                            OPI_PSRAM_SYNC_READ, 16,
                            addr, 32,
                            OCT_PSRAM_RD_DUMMY_NUM,
                            NULL, 0,
                            buf, len * 8,
                            BIT(1),
                            false);
#elif CONFIG_SPIRAM_MODE_QUAD
    psram_exec_cmd(1, 0,
                   QPI_PSRAM_FAST_READ, 8,
                   addr, 24,
                   QPI_PSRAM_FAST_READ_DUMMY + s_psram_extra_dummy,
                   NULL, 0,
                   buf, len * 8,
                   SPI_MEM_CS1_DIS_M,
                   false);
#endif
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

void mspi_timing_config_psram_write_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    s_psram_execution(buf, addr, len, false);
}

void mspi_timing_config_psram_read_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    s_psram_execution(buf, addr, len, true);
}


/*-------------------------------------------------------------------------------------------------
 * SPI1 Timing Tuning APIs
 * These APIs are only used in `spi_flash_timing_tuning.c/sweep_for_success_sample_points()` for
 * configuring SPI1 timing tuning related registers to find best tuning parameter
 *-------------------------------------------------------------------------------------------------*/
void mspi_timing_config_flash_set_tuning_regs(const mspi_timing_tuning_param_t *params)
{
    /**
     * 1. SPI_MEM_DINx_MODE(1), SPI_MEM_DINx_NUM(1) are meaningless
     *    SPI0 and SPI1 share the SPI_MEM_DINx_MODE(0), SPI_MEM_DINx_NUM(0) for FLASH timing tuning
     * 2. We use SPI1 to get the best Flash timing tuning (mode and num) config
     */
    mspi_timing_config_flash_set_din_mode_num(0, params->spi_din_mode, params->spi_din_num);
    mspi_timing_config_flash_set_extra_dummy(1, params->extra_dummy_len);
}

void mspi_timing_config_psram_set_tuning_regs(const mspi_timing_tuning_param_t *params)
{
    /**
     * 1. SPI_MEM_SPI_SMEM_DINx_MODE(1), SPI_MEM_SPI_SMEM_DINx_NUM(1) are meaningless
     *    SPI0 and SPI1 share the SPI_MEM_SPI_SMEM_DINx_MODE(0), SPI_MEM_SPI_SMEM_DINx_NUM(0) for PSRAM timing tuning
     * 2. We use SPI1 to get the best PSRAM timing tuning (mode and num) config
     */
    mspi_timing_config_psram_set_din_mode_num(0, params->spi_din_mode, params->spi_din_num);

#if CONFIG_SPIRAM_MODE_OCT
    //On 728, for SPI1, flash and psram share the extra dummy register
    mspi_timing_config_flash_set_extra_dummy(1, params->extra_dummy_len);
#elif CONFIG_SPIRAM_MODE_QUAD
    //Update this `s_psram_extra_dummy`, the `s_psram_read_data` will set dummy according to this `s_psram_extra_dummy`
    s_psram_extra_dummy = params->extra_dummy_len;
    mspi_timing_ll_set_quad_flash_dummy(1, s_psram_extra_dummy - 1);
#endif
}

#endif //#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING


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
    *setup_time = mspi_timing_ll_get_cs_setup_val(0);
    *hold_time = mspi_timing_ll_get_cs_hold_val(0);
    /**
     * The logic here is, if setup_en / hold_en is false, then we return the realistic cycle number,
     * which is 0. If true, then the realistic cycle number is (reg_value + 1)
     */
    if (mspi_timing_ll_is_cs_setup_enabled(0)) {
        *setup_time += 1;
    } else {
        *setup_time = 0;
    }
    if (mspi_timing_ll_is_cs_hold_enabled(0)) {
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
    return mspi_timing_ll_get_clock_reg(1);
}
