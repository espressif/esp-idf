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

#define OPI_PSRAM_SYNC_READ                 0x0000
#define OPI_PSRAM_SYNC_WRITE                0x8080
#define OCT_PSRAM_RD_DUMMY_NUM              (2*(10-1))
#define OCT_PSRAM_WR_DUMMY_NUM              (2*(5-1))


/////////////////////////////////////////TIMING TUNING IS NEEDED//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING  //If one of the FLASH / PSRAM or both of them need timing tuning, we should build following code

spi_timing_config_core_clock_t IRAM_ATTR spi_timing_config_get_core_clock(void)
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

void IRAM_ATTR spi_timing_config_set_core_clock(uint8_t spi_num, spi_timing_config_core_clock_t core_clock)
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

//-------------------------------------FLASH timing tuning-------------------------------------//
void IRAM_ATTR spi_timing_config_set_flash_clock(uint8_t spi_num, uint32_t freqdiv)
{
    assert(freqdiv > 0);
    if (freqdiv == 1) {
        WRITE_PERI_REG(SPI_MEM_CLOCK_REG(spi_num), SPI_MEM_CLK_EQU_SYSCLK);
    } else {
        uint32_t freqbits = (((freqdiv - 1) << SPI_MEM_CLKCNT_N_S)) | (((freqdiv / 2 - 1) << SPI_MEM_CLKCNT_H_S)) | ((freqdiv - 1) << SPI_MEM_CLKCNT_L_S);
        WRITE_PERI_REG(SPI_MEM_CLOCK_REG(spi_num), freqbits);
    }
}

void IRAM_ATTR spi_timing_config_flash_set_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num)
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

void IRAM_ATTR spi_timing_config_flash_set_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
    if (extra_dummy > 0) {
        SET_PERI_REG_MASK(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_EXTRA_DUMMY_CYCLELEN_V, extra_dummy,
            SPI_MEM_EXTRA_DUMMY_CYCLELEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_EXTRA_DUMMY_CYCLELEN_V, 0,
            SPI_MEM_EXTRA_DUMMY_CYCLELEN_S);
    }
}

//-------------------------------------PSRAM timing tuning-------------------------------------//
void IRAM_ATTR spi_timing_config_set_psram_clock(uint8_t spi_num, uint32_t freqdiv)
{
    if (freqdiv == 1) {
        WRITE_PERI_REG(SPI_MEM_SRAM_CLK_REG(spi_num), SPI_MEM_SCLK_EQU_SYSCLK);
    } else {
        uint32_t freqbits = (((freqdiv-1)<<SPI_MEM_SCLKCNT_N_S)) | (((freqdiv/2-1)<<SPI_MEM_SCLKCNT_H_S)) | ((freqdiv-1)<<SPI_MEM_SCLKCNT_L_S);
        WRITE_PERI_REG(SPI_MEM_SRAM_CLK_REG(spi_num), freqbits);
    }
}

void IRAM_ATTR spi_timing_config_psram_set_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num)
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

void IRAM_ATTR spi_timing_config_psram_set_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
    if (extra_dummy > 0) {
        SET_PERI_REG_MASK(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V, extra_dummy,
            SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V, 0,
            SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S);
    }
}

//-------------------------------------------FLASH/PSRAM Read/Write------------------------------------------//
void IRAM_ATTR spi_timing_config_flash_read_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len)
{
#if CONFIG_ESPTOOLPY_OCT_FLASH
    // note that in spi_flash_read API, there is a wait-idle stage, since flash can only be read in idle state.
    // but after we change the timing settings, we might not read correct idle status via RDSR.
    // so, here we should use a read API that won't check idle status.
    for (int i = 0; i < 16; i++) {
        REG_WRITE(SPI_MEM_W0_REG(1) + i*4, 0);
    }
    esp_rom_opiflash_read_raw(addr, buf, len);
#else
    abort();
#endif
}

static void IRAM_ATTR s_psram_write_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len)
{
#if CONFIG_SPIRAM_MODE_OCT
    esp_rom_opiflash_exec_cmd(spi_num, ESP_ROM_SPIFLASH_OPI_DTR_MODE,
                            OPI_PSRAM_SYNC_WRITE, 16,
                            addr, 32,
                            OCT_PSRAM_WR_DUMMY_NUM,
                            buf, 8 * len,
                            NULL, 0,
                            BIT(1),
                            false);
#else
    abort();
#endif
}

static void IRAM_ATTR s_psram_read_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len)
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
                            buf, 8 * len,
                            BIT(1),
                            false);
#else
    abort();
#endif
}

static void IRAM_ATTR s_psram_execution(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len, bool is_read)
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

void IRAM_ATTR spi_timing_config_psram_write_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len)
{
    s_psram_execution(spi_num, buf, addr, len, false);
}

void IRAM_ATTR spi_timing_config_psram_read_data(uint8_t spi_num, uint8_t *buf, uint32_t addr, uint32_t len)
{
    s_psram_execution(spi_num, buf, addr, len, true);
}

#endif //#if SPI_TIMING_FLASH_NEEDS_TUNING || SPI_TIMING_PSRAM_NEEDS_TUNING
