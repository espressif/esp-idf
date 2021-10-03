/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_rom_gpio.h"
#include "esp32s3/rom/gpio.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"
#include "spi_flash_private.h"
#include "soc/spi_mem_reg.h"
#include "soc/io_mux_reg.h"
#if CONFIG_ESPTOOLPY_FLASH_VENDOR_MXIC
#include "opi_flash_cmd_format_mxic.h"
#endif

#define SPI_FLASH_SPI_CMD_WRCR2     0x72
#define SPI_FLASH_SPI_CMD_RDSR      0x05
#define SPI_FLASH_SPI_CMD_RDCR      0x15
#define SPI_FLASH_SPI_CMD_WRSRCR    0x01

// default value is rom_default_spiflash_legacy_flash_func
extern const spiflash_legacy_funcs_t *rom_spiflash_legacy_funcs;
extern int SPI_write_enable(void *spi);
DRAM_ATTR const esp_rom_opiflash_def_t opiflash_cmd_def = OPI_CMD_FORMAT();


static void s_register_rom_function(void)
{
    static spiflash_legacy_funcs_t rom_func =
    {
        .read_sub_len  = 32,
        .write_sub_len = 32,
        .unlock = esp_rom_opiflash_wait_idle,
        .erase_block = esp_rom_opiflash_erase_block_64k,
        .erase_sector = esp_rom_opiflash_erase_sector,
        .read = esp_rom_opiflash_read,
        .write = esp_rom_opiflash_write,
        .wait_idle = esp_rom_opiflash_wait_idle,
        .wren = esp_rom_opiflash_wren,
        .erase_area = esp_rom_opiflash_erase_area,
    };
    rom_spiflash_legacy_funcs = &rom_func;
}

#if CONFIG_ESPTOOLPY_FLASH_VENDOR_MXIC
// 0x00: SPI; 0x01: STR OPI;  0x02: DTR OPI
static void s_set_flash_dtr_str_opi_mode(int spi_num, uint8_t val)
{
    uint8_t cmd_len = 8;
    int addr_bit_len = 32;
    int dummy = 0;
    int data_bit_len = 8;

    SPI_write_enable(&g_rom_flashchip);
    //SPI command, WRCR2
    esp_rom_opiflash_exec_cmd(spi_num, ESP_ROM_SPIFLASH_FASTRD_MODE,
                              SPI_FLASH_SPI_CMD_WRCR2, cmd_len,
                              0, addr_bit_len,
                              dummy,
                              (uint8_t *)&val, data_bit_len,
                              NULL, 0,
                              ESP_ROM_OPIFLASH_SEL_CS0,
                              false);
}

//To set the output driver strength
static void s_set_flash_ouput_driver_strength(int spi_num, uint8_t strength)
{
    uint16_t reg_val = 0;
    uint8_t sr_reg_val = 0;
    uint8_t cr_reg_val = 0;
    uint8_t cmd_len = 8;
    uint32_t addr = 0;
    int addr_bit_len = 0;
    int dummy = 0;
    int data_bit_len = 8;

    //Read
    //SPI command, RDSR
    esp_rom_opiflash_exec_cmd(spi_num, ESP_ROM_SPIFLASH_FASTRD_MODE,
                              SPI_FLASH_SPI_CMD_RDSR, cmd_len,
                              addr, addr_bit_len,
                              dummy,
                              NULL, 0,
                              (uint8_t*)&sr_reg_val, data_bit_len,
                              ESP_ROM_OPIFLASH_SEL_CS0,
                              false);

    //SPI command, RDCR
    esp_rom_opiflash_exec_cmd(spi_num, ESP_ROM_SPIFLASH_FASTRD_MODE,
                              SPI_FLASH_SPI_CMD_RDCR, cmd_len,
                              addr, addr_bit_len,
                              dummy,
                              NULL, 0,
                              (uint8_t*)&cr_reg_val, data_bit_len,
                              ESP_ROM_OPIFLASH_SEL_CS0,
                              false);

    //Modify
    reg_val = (((cr_reg_val & 0xf8) | strength) << 8) | sr_reg_val;

    //Write
    //SPI command, WRSR/WRCR
    data_bit_len = 16;
    SPI_write_enable(&g_rom_flashchip);
    esp_rom_opiflash_exec_cmd(spi_num, ESP_ROM_SPIFLASH_FASTRD_MODE,
                              SPI_FLASH_SPI_CMD_WRSRCR, cmd_len,
                              addr, addr_bit_len,
                              dummy,
                              (uint8_t*)&reg_val, data_bit_len,
                              NULL, 0,
                              ESP_ROM_OPIFLASH_SEL_CS0,
                              false);
}

static void s_set_pin_drive_capability(uint8_t drv)
{
    //flash clock
    REG_SET_FIELD(SPI_MEM_DATE_REG(0), SPI_MEM_SPI_FMEM_SPICLK_FUN_DRV, 3);
    //cs0
    PIN_SET_DRV(IO_MUX_GPIO29_REG, 3);
}

static void s_flash_init_mxic(esp_rom_spiflash_read_mode_t mode)
{
    esp_rom_opiflash_legacy_driver_init(&opiflash_cmd_def);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);

    // increase flash output driver strength
    s_set_flash_ouput_driver_strength(1, 7);

    // STR/DTR specific setting
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
#if CONFIG_ESPTOOLPY_FLASHMODE_OPI_STR
    s_set_pin_drive_capability(3);
    s_set_flash_dtr_str_opi_mode(1, 0x1);
    esp_rom_opiflash_cache_mode_config(mode, &rom_opiflash_cmd_def->cache_rd_cmd);
    esp_rom_spi_set_dtr_swap_mode(0, false, false);
    esp_rom_spi_set_dtr_swap_mode(1, false, false);
#else //CONFIG_ESPTOOLPY_FLASHMODE_OPI_DTR
    s_set_pin_drive_capability(3);
    s_set_flash_dtr_str_opi_mode(1, 0x2);
    esp_rom_opiflash_cache_mode_config(mode, &rom_opiflash_cmd_def->cache_rd_cmd);
    esp_rom_spi_set_dtr_swap_mode(0, true, true);
    esp_rom_spi_set_dtr_swap_mode(1, true, true);
#endif

    s_register_rom_function();
    esp_rom_opiflash_wait_idle();
}
#endif   // #if CONFIG_FLASH_VENDOR_XXX

esp_err_t esp_opiflash_init(void)
{
    esp_rom_spiflash_read_mode_t mode;
#if CONFIG_ESPTOOLPY_FLASHMODE_OPI_STR
    mode = ESP_ROM_SPIFLASH_OPI_STR_MODE;
#elif CONFIG_ESPTOOLPY_FLASHMODE_OPI_DTR
    mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE;
#else
    mode = ESP_ROM_SPIFLASH_FASTRD_MODE;
#endif

#if CONFIG_ESPTOOLPY_FLASH_VENDOR_MXIC
    s_flash_init_mxic(mode);
#else
    abort();
#endif
    return ESP_OK;
}
