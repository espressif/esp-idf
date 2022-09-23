/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
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
#include "spi_timing_config.h"
#include "esp_private/spi_flash_os.h"
#include "bootloader_flash.h"

#define OPI_PSRAM_SYNC_READ           0x0000
#define OPI_PSRAM_SYNC_WRITE          0x8080
#define OCT_PSRAM_RD_DUMMY_NUM        (2*(10-1))
#define OCT_PSRAM_WR_DUMMY_NUM        (2*(5-1))

#define QPI_PSRAM_FAST_READ            0XEB
#define QPI_PSRAM_WRITE                0X38
#define QPI_PSRAM_FAST_READ_DUMMY      6

#define MULTI_LINE_MASK_OCT_FLASH     (SPI_MEM_FCMD_OCT | SPI_MEM_FADDR_OCT | SPI_MEM_FDIN_OCT | SPI_MEM_FDOUT_OCT)
#define MULTI_LINE_MASK_QUAD_FLASH    (SPI_MEM_FASTRD_MODE | SPI_MEM_FREAD_DUAL | SPI_MEM_FREAD_DIO | SPI_MEM_FREAD_QUAD | SPI_MEM_FREAD_QIO)
#define SPI_FLASH_QIO_MODE            (SPI_MEM_FREAD_QIO | SPI_MEM_FASTRD_MODE)
#define SPI_FLASH_QUAD_MODE           (SPI_MEM_FREAD_QUAD | SPI_MEM_FASTRD_MODE)
#define SPI_FLASH_DIO_MODE            (SPI_MEM_FREAD_DIO | SPI_MEM_FASTRD_MODE)
#define SPI_FLASH_DUAL_MODE           (SPI_MEM_FREAD_DUAL | SPI_MEM_FASTRD_MODE)
#define SPI_FLASH_FAST_MODE           (SPI_MEM_FASTRD_MODE)
#define SPI_FLASH_SLOW_MODE           0

#define NOT_INIT_INT                  127

//-------------------------------------MSPI Clock Setting-------------------------------------//
spi_timing_config_core_clock_t spi_timing_config_get_core_clock(void)
{
    switch (SPI_TIMING_CORE_CLOCK_MHZ) {
        case 80:
            return SPI_TIMING_CONFIG_CORE_CLOCK_80M;
        case 120:
            return SPI_TIMING_CONFIG_CORE_CLOCK_120M;
        case 160:
            return SPI_TIMING_CONFIG_CORE_CLOCK_160M;
        case 240:
            return SPI_TIMING_CONFIG_CORE_CLOCK_240M;
        default:
            abort();
    }
}

void spi_timing_config_set_core_clock(uint8_t spi_num, spi_timing_config_core_clock_t core_clock)
{
    uint32_t reg_val = 0;

    switch (core_clock) {
        case SPI_TIMING_CONFIG_CORE_CLOCK_80M:
            reg_val = 0;
            break;
        case SPI_TIMING_CONFIG_CORE_CLOCK_120M:
            reg_val = 1;
            break;
        case SPI_TIMING_CONFIG_CORE_CLOCK_160M:
            reg_val = 2;
            break;
        case SPI_TIMING_CONFIG_CORE_CLOCK_240M:
            reg_val = 3;
            break;
        default:
            abort();
    }

    REG_SET_FIELD(SPI_MEM_CORE_CLK_SEL_REG(spi_num), SPI_MEM_CORE_CLK_SEL, reg_val);
}

void spi_timing_config_set_flash_clock(uint8_t spi_num, uint32_t freqdiv)
{
    assert(freqdiv > 0);
    if (freqdiv == 1) {
        WRITE_PERI_REG(SPI_MEM_CLOCK_REG(spi_num), SPI_MEM_CLK_EQU_SYSCLK);
    } else {
        uint32_t freqbits = (((freqdiv - 1) << SPI_MEM_CLKCNT_N_S)) | (((freqdiv / 2 - 1) << SPI_MEM_CLKCNT_H_S)) | ((freqdiv - 1) << SPI_MEM_CLKCNT_L_S);
        WRITE_PERI_REG(SPI_MEM_CLOCK_REG(spi_num), freqbits);
    }
}

void spi_timing_config_set_psram_clock(uint8_t spi_num, uint32_t freqdiv)
{
    if (freqdiv == 1) {
        WRITE_PERI_REG(SPI_MEM_SRAM_CLK_REG(spi_num), SPI_MEM_SCLK_EQU_SYSCLK);
    } else {
        uint32_t freqbits = (((freqdiv-1)<<SPI_MEM_SCLKCNT_N_S)) | (((freqdiv/2-1)<<SPI_MEM_SCLKCNT_H_S)) | ((freqdiv-1)<<SPI_MEM_SCLKCNT_L_S);
        WRITE_PERI_REG(SPI_MEM_SRAM_CLK_REG(spi_num), freqbits);
    }
}

/////////////////////////////////////////TIMING TUNING IS NEEDED//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
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
void spi_timing_config_flash_set_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num)
{
    uint32_t reg_val = 0;
    reg_val = (REG_READ(SPI_MEM_DIN_MODE_REG(spi_num)) & (~(SPI_MEM_DIN0_MODE_M | SPI_MEM_DIN1_MODE_M | SPI_MEM_DIN2_MODE_M | SPI_MEM_DIN3_MODE_M | SPI_MEM_DIN4_MODE_M | SPI_MEM_DIN5_MODE_M | SPI_MEM_DIN6_MODE_M | SPI_MEM_DIN7_MODE_M | SPI_MEM_DINS_MODE_M)))
        | (din_mode << SPI_MEM_DIN0_MODE_S) | (din_mode << SPI_MEM_DIN1_MODE_S) | (din_mode << SPI_MEM_DIN2_MODE_S) | (din_mode << SPI_MEM_DIN3_MODE_S)
        | (din_mode << SPI_MEM_DIN4_MODE_S) | (din_mode << SPI_MEM_DIN5_MODE_S) | (din_mode << SPI_MEM_DIN6_MODE_S) | (din_mode << SPI_MEM_DIN7_MODE_S) | (din_mode << SPI_MEM_DINS_MODE_S);
    REG_WRITE(SPI_MEM_DIN_MODE_REG(spi_num), reg_val);

    reg_val = (REG_READ(SPI_MEM_DIN_NUM_REG(spi_num)) & (~(SPI_MEM_DIN0_NUM_M | SPI_MEM_DIN1_NUM_M | SPI_MEM_DIN2_NUM_M | SPI_MEM_DIN3_NUM_M | SPI_MEM_DIN4_NUM_M | SPI_MEM_DIN5_NUM_M | SPI_MEM_DIN6_NUM_M | SPI_MEM_DIN7_NUM_M | SPI_MEM_DINS_NUM_M)))
        | (din_num << SPI_MEM_DIN0_NUM_S) | (din_num << SPI_MEM_DIN1_NUM_S) | (din_num << SPI_MEM_DIN2_NUM_S) | (din_num << SPI_MEM_DIN3_NUM_S)
        | (din_num << SPI_MEM_DIN4_NUM_S) | (din_num << SPI_MEM_DIN5_NUM_S) | (din_num << SPI_MEM_DIN6_NUM_S) | (din_num << SPI_MEM_DIN7_NUM_S) | (din_num << SPI_MEM_DINS_NUM_S);
    REG_WRITE(SPI_MEM_DIN_NUM_REG(spi_num), reg_val);
}

static uint32_t spi_timing_config_get_dummy(void)
{
    uint32_t ctrl_reg = READ_PERI_REG(SPI_MEM_CTRL_REG(0));
    if (ctrl_reg & MULTI_LINE_MASK_OCT_FLASH) {
        abort();
    }

#if CONFIG_SPI_FLASH_HPM_ENABLE
    if (spi_flash_hpm_dummy_adjust()) { // HPM is enabled
        const spi_flash_hpm_dummy_conf_t *hpm_dummy = spi_flash_hpm_get_dummy();
        switch (ctrl_reg & MULTI_LINE_MASK_QUAD_FLASH) {
            case SPI_FLASH_QIO_MODE:
                return hpm_dummy->qio_dummy - 1;
            case SPI_FLASH_QUAD_MODE:
                return hpm_dummy->qout_dummy - 1;
            case SPI_FLASH_DIO_MODE:
                return hpm_dummy->dio_dummy - 1;
            case SPI_FLASH_DUAL_MODE:
                return hpm_dummy->dout_dummy - 1;
            case SPI_FLASH_FAST_MODE:
                return hpm_dummy->fastrd_dummy - 1;
            case SPI_FLASH_SLOW_MODE:
                return 0;
            default:
                abort();
        }
    } else
#endif
    { // HPM is not enabled
        switch (ctrl_reg & MULTI_LINE_MASK_QUAD_FLASH) {
            case SPI_FLASH_QIO_MODE:
                return SPI1_R_QIO_DUMMY_CYCLELEN;
            case SPI_FLASH_QUAD_MODE:
                return SPI1_R_FAST_DUMMY_CYCLELEN;
            case SPI_FLASH_DIO_MODE:
                return SPI1_R_DIO_DUMMY_CYCLELEN;
            case SPI_FLASH_DUAL_MODE:
                return SPI1_R_FAST_DUMMY_CYCLELEN;
            case SPI_FLASH_FAST_MODE:
                return SPI1_R_FAST_DUMMY_CYCLELEN;
            case SPI_FLASH_SLOW_MODE:
                return 0;
            default:
                abort();
        }
    }
}

void spi_timing_config_flash_set_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
    if (bootloader_flash_is_octal_mode_enabled()) {
        if (extra_dummy > 0) {
            SET_PERI_REG_MASK(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_TIMING_CALI_M);
            SET_PERI_REG_BITS(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_EXTRA_DUMMY_CYCLELEN_V, extra_dummy,
                SPI_MEM_EXTRA_DUMMY_CYCLELEN_S);
        } else {
            CLEAR_PERI_REG_MASK(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_TIMING_CALI_M);
            SET_PERI_REG_BITS(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_EXTRA_DUMMY_CYCLELEN_V, 0,
                SPI_MEM_EXTRA_DUMMY_CYCLELEN_S);
        }
        return;
    }
    /**
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
    SET_PERI_REG_BITS(SPI_MEM_USER1_REG(0), SPI_MEM_USR_DUMMY_CYCLELEN_V, dummy + g_rom_spiflash_dummy_len_plus[spi_num], SPI_MEM_USR_DUMMY_CYCLELEN_S);
}

//-------------------------------------PSRAM timing tuning register config-------------------------------------//
void spi_timing_config_psram_set_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num)
{
    uint32_t reg_val = 0;
    reg_val = (REG_READ(SPI_MEM_SPI_SMEM_DIN_MODE_REG(spi_num)) & (~(SPI_MEM_SPI_SMEM_DIN0_MODE_M | SPI_MEM_SPI_SMEM_DIN1_MODE_M | SPI_MEM_SPI_SMEM_DIN2_MODE_M | SPI_MEM_SPI_SMEM_DIN3_MODE_M | SPI_MEM_SPI_SMEM_DIN4_MODE_M | SPI_MEM_SPI_SMEM_DIN5_MODE_M | SPI_MEM_SPI_SMEM_DIN6_MODE_M | SPI_MEM_SPI_SMEM_DIN7_MODE_M | SPI_MEM_SPI_SMEM_DINS_MODE_M)))
        | (din_mode << SPI_MEM_SPI_SMEM_DIN0_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN1_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN2_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN3_MODE_S)
        | (din_mode << SPI_MEM_SPI_SMEM_DIN4_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN5_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN6_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN7_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DINS_MODE_S);
    REG_WRITE(SPI_MEM_SPI_SMEM_DIN_MODE_REG(spi_num), reg_val);

    reg_val = (REG_READ(SPI_MEM_SPI_SMEM_DIN_NUM_REG(spi_num)) & (~(SPI_MEM_SPI_SMEM_DIN0_NUM_M | SPI_MEM_SPI_SMEM_DIN1_NUM_M | SPI_MEM_SPI_SMEM_DIN2_NUM_M | SPI_MEM_SPI_SMEM_DIN3_NUM_M | SPI_MEM_SPI_SMEM_DIN4_NUM_M | SPI_MEM_SPI_SMEM_DIN5_NUM_M | SPI_MEM_SPI_SMEM_DIN6_NUM_M | SPI_MEM_SPI_SMEM_DIN7_NUM_M | SPI_MEM_SPI_SMEM_DINS_NUM_M)))
        | (din_num << SPI_MEM_SPI_SMEM_DIN0_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN1_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN2_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN3_NUM_S)
        | (din_num << SPI_MEM_SPI_SMEM_DIN4_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN5_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN6_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN7_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DINS_NUM_S);
    REG_WRITE(SPI_MEM_SPI_SMEM_DIN_NUM_REG(spi_num), reg_val);
}

void spi_timing_config_psram_set_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
#if CONFIG_SPIRAM_MODE_OCT
    if (extra_dummy > 0) {
        SET_PERI_REG_MASK(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V, extra_dummy,
            SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V, 0,
            SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S);
    }
#elif CONFIG_SPIRAM_MODE_QUAD
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(spi_num), SPI_MEM_USR_RD_SRAM_DUMMY_M);
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(spi_num), SPI_MEM_SRAM_RDUMMY_CYCLELEN_V, (QPI_PSRAM_FAST_READ_DUMMY + extra_dummy - 1), SPI_MEM_SRAM_RDUMMY_CYCLELEN_S);
#endif
}

//-------------------------------------------FLASH/PSRAM Read/Write------------------------------------------//
void spi_timing_config_flash_read_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len)
{
    if (bootloader_flash_is_octal_mode_enabled()) {
        // note that in spi_flash_read API, there is a wait-idle stage, since flash can only be read in idle state.
        // but after we change the timing settings, we might not read correct idle status via RDSR.
        // so, here we should use a read API that won't check idle status.
        for (int i = 0; i < 16; i++) {
            REG_WRITE(SPI_MEM_W0_REG(1) + i*4, 0);
        }
        esp_rom_opiflash_read_raw(addr, buf, len);
    } else {
        esp_rom_spiflash_read(addr, (uint32_t *)buf, len);
    }
}

static void s_psram_write_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len)
{
#if CONFIG_SPIRAM_MODE_OCT
    esp_rom_opiflash_exec_cmd(spi_num, ESP_ROM_SPIFLASH_OPI_DTR_MODE,
                            OPI_PSRAM_SYNC_WRITE, 16,
                            addr, 32,
                            OCT_PSRAM_WR_DUMMY_NUM,
                            buf, len * 8,
                            NULL, 0,
                            BIT(1),
                            false);
#elif CONFIG_SPIRAM_MODE_QUAD
    psram_exec_cmd(spi_num, 0,
                   QPI_PSRAM_WRITE, 8,
                   addr, 24,
                   0,
                   buf, len * 8,
                   NULL, 0,
                   SPI_MEM_CS1_DIS_M,
                   false);
#endif
}

static void s_psram_read_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len)
{
#if CONFIG_SPIRAM_MODE_OCT
    for (int i = 0; i < 16; i++) {
        REG_WRITE(SPI_MEM_W0_REG(1) + i*4, 0);
    }
    esp_rom_opiflash_exec_cmd(spi_num, ESP_ROM_SPIFLASH_OPI_DTR_MODE,
                            OPI_PSRAM_SYNC_READ, 16,
                            addr, 32,
                            OCT_PSRAM_RD_DUMMY_NUM,
                            NULL, 0,
                            buf, len * 8,
                            BIT(1),
                            false);
#elif CONFIG_SPIRAM_MODE_QUAD
    psram_exec_cmd(spi_num, 0,
                   QPI_PSRAM_FAST_READ, 8,
                   addr, 24,
                   QPI_PSRAM_FAST_READ_DUMMY + s_psram_extra_dummy,
                   NULL, 0,
                   buf, len * 8,
                   SPI_MEM_CS1_DIS_M,
                   false);
#endif
}

static void s_psram_execution(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len, bool is_read)
{
    while (len) {
        uint32_t length = MIN(len, 32);
        if (is_read) {
            s_psram_read_data(1, buf, addr, length);
        } else {
            s_psram_write_data(1, buf, addr, length);
        }
        addr += length;
        buf += length;
        len -= length;
    }
}

void spi_timing_config_psram_write_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len)
{
    s_psram_execution(spi_num, buf, addr, len, false);
}

void spi_timing_config_psram_read_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len)
{
    s_psram_execution(spi_num, buf, addr, len, true);
}


/*-------------------------------------------------------------------------------------------------
 * SPI1 Timing Tuning APIs
 * These APIs are only used in `spi_flash_timing_tuning.c/sweep_for_success_sample_points()` for
 * configuring SPI1 timing tuning related registers to find best tuning parameter
 *-------------------------------------------------------------------------------------------------*/
void spi_timing_config_flash_tune_din_num_mode(uint8_t din_mode, uint8_t din_num)
{
    /**
     * 1. SPI_MEM_DINx_MODE(1), SPI_MEM_DINx_NUM(1) are meaningless
     *    SPI0 and SPI1 share the SPI_MEM_DINx_MODE(0), SPI_MEM_DINx_NUM(0) for FLASH timing tuning
     * 2. We use SPI1 to get the best Flash timing tuning (mode and num) config
     */
    spi_timing_config_flash_set_din_mode_num(0, din_mode, din_num);
}

void spi_timing_config_flash_tune_dummy(uint8_t extra_dummy)
{
    spi_timing_config_flash_set_extra_dummy(1, extra_dummy);
}

void spi_timing_config_psram_tune_din_num_mode(uint8_t din_mode, uint8_t din_num)
{
    /**
     * 1. SPI_MEM_SPI_SMEM_DINx_MODE(1), SPI_MEM_SPI_SMEM_DINx_NUM(1) are meaningless
     *    SPI0 and SPI1 share the SPI_MEM_SPI_SMEM_DINx_MODE(0), SPI_MEM_SPI_SMEM_DINx_NUM(0) for PSRAM timing tuning
     * 2. We use SPI1 to get the best PSRAM timing tuning (mode and num) config
     */
    spi_timing_config_psram_set_din_mode_num(0, din_mode, din_num);
}

void spi_timing_config_psram_tune_dummy(uint8_t extra_dummy)
{
#if CONFIG_SPIRAM_MODE_OCT
    //On 728, for SPI1, flash and psram share the extra dummy register
    spi_timing_config_flash_set_extra_dummy(1, extra_dummy);
#elif CONFIG_SPIRAM_MODE_QUAD
    //Update this `s_psram_extra_dummy`, the `s_psram_read_data` will set dummy according to this `s_psram_extra_dummy`
    s_psram_extra_dummy = extra_dummy;
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(1), SPI_MEM_USR_DUMMY); // dummy en
    SET_PERI_REG_BITS(SPI_MEM_USER1_REG(1), SPI_MEM_USR_DUMMY_CYCLELEN_V, extra_dummy - 1, SPI_MEM_USR_DUMMY_CYCLELEN_S);
#endif
}

#endif //#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING


/*-------------------------------------------------------------------------------------------------
 * To let upper lay (spi_flash_timing_tuning.c) to know the necessary timing registers
 *-------------------------------------------------------------------------------------------------*/
static bool s_get_cs_setup_enable(void)
{
    return REG_GET_BIT(SPI_MEM_USER_REG(0), SPI_MEM_CS_SETUP);
}

static bool s_get_cs_hold_enable(void)
{
    return REG_GET_BIT(SPI_MEM_USER_REG(0), SPI_MEM_CS_HOLD);
}

/**
 * Get the SPI1 Flash CS timing setting. The setup time and hold time are both realistic cycles.
 * @note On ESP32-S3, SPI0/1 share the Flash CS timing registers. Therefore, we should not change these values.
 * @note This function inform `spi_flash_timing_tuning.c` (driver layer) of the cycle,
 * and other component (esp_flash driver) should get these cycle and configure the registers accordingly.
 */
void spi_timing_config_get_cs_timing(uint8_t *setup_time, uint32_t *hold_time)
{
    *setup_time = REG_GET_FIELD(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_SETUP_TIME);
    *hold_time = REG_GET_FIELD(SPI_MEM_CTRL2_REG(0), SPI_MEM_CS_HOLD_TIME);
    /**
     * The logic here is, if setup_en / hold_en is false, then we return the realistic cycle number,
     * which is 0. If true, then the realistic cycle number is (reg_value + 1)
     */
    if (s_get_cs_setup_enable()) {
        *setup_time += 1;
    } else {
        *setup_time = 0;
    }
    if (s_get_cs_hold_enable()) {
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
uint32_t spi_timing_config_get_flash_clock_reg(void)
{
    return READ_PERI_REG(SPI_MEM_CLOCK_REG(1));
}
