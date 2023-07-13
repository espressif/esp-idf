/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"
#include "esp_private/spi_flash_os.h"
#include "opi_flash_private.h"
#include "soc/spi_mem_reg.h"
#include "soc/io_mux_reg.h"
#include "opi_flash_cmd_format_mxic.h"

#define SPI_FLASH_SPI_CMD_WRCR2     0x72
#define SPI_FLASH_SPI_CMD_RDSR      0x05
#define SPI_FLASH_SPI_CMD_RDCR      0x15
#define SPI_FLASH_SPI_CMD_WRSRCR    0x01
#define SPI_FLASH_SPI_CMD_RDSFDP    0x5A

/**
 * Supported Flash chip vendor id
 */
#define ESP_FLASH_CHIP_MXIC_OCT     0xC2

const static char *TAG = "Octal Flash";
// default value is rom_default_spiflash_legacy_flash_func
extern const spiflash_legacy_funcs_t *rom_spiflash_legacy_funcs;
static uint32_t s_chip_id;


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

#if CONFIG_SPI_FLASH_SUPPORT_MXIC_OPI_CHIP
/*----------------------------------------------------------------------------------------------------
                                MXIC Specific Functions
-----------------------------------------------------------------------------------------------------*/
static esp_err_t s_probe_mxic_chip(uint32_t chip_id, uint8_t *out_vendor_id)
{
    if (chip_id >> 16 != ESP_FLASH_CHIP_MXIC_OCT) {
        return ESP_ERR_NOT_FOUND;
    }
    if (((chip_id >> 8) & 0xf0) != 0x80) {
        // We now suppose that middle id of opi flash is 0x8*.
        ESP_EARLY_LOGE(TAG, "Detected MXIC Flash, but memory type is not Octal");
        return ESP_ERR_NOT_FOUND;
    }
    *out_vendor_id = ESP_FLASH_CHIP_MXIC_OCT;

    return ESP_OK;
}

#if CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR
static bool s_mxic_dtr_need_swap(void)
{
    // This function is used for judging the data bytes whether need swap.
    // For some of opi flash chips, the data bytes are ordered by D1-D0-D3-D2. This kinds of order needs swap.
    // On the contrary, some opi flash chips order the data like D0-D1-D2-D3. This kinds of order doesn't need swap.
    // Note: this function must be called when flash works under single line mode.
    // 1. Send 0x5A to read SFDP regs for getting the first address of JEDEC Flash Parameter table.
    // 2. Add offset with first address to get the order in 8D-8D-8D mode.
    // 3. Judge whether the BIT(7) is 1, 1 stands for need swap, vice versa.
    uint8_t JEDEC_first_address = 0;
    uint8_t byte_order_val = 0;
    uint8_t dummy = 8;
    uint8_t cmd_len = 8;
    uint8_t addr_len = 24;
    uint8_t miso_bit_len = 8;
    esp_rom_opiflash_exec_cmd(1, ESP_ROM_SPIFLASH_FASTRD_MODE,
                              SPI_FLASH_SPI_CMD_RDSFDP, cmd_len,
                              0x0C, addr_len,
                              dummy,
                              NULL, 0,
                              (uint8_t*)&JEDEC_first_address, miso_bit_len,
                              ESP_ROM_OPIFLASH_SEL_CS0,
                              false);

    esp_rom_opiflash_exec_cmd(1, ESP_ROM_SPIFLASH_FASTRD_MODE,
                              SPI_FLASH_SPI_CMD_RDSFDP, cmd_len,
                              (JEDEC_first_address + 0x47), addr_len,
                              dummy,
                              NULL, 0,
                              (uint8_t*)&byte_order_val, miso_bit_len,
                              ESP_ROM_OPIFLASH_SEL_CS0,
                              false);

    return ((byte_order_val & 0x80) == 0x80) ? true : false;
}
#endif // CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR

// 0x00: SPI; 0x01: STR OPI;  0x02: DTR OPI
static void s_set_flash_dtr_str_opi_mode(int spi_num, uint8_t val)
{
    uint8_t cmd_len = 8;
    int addr_bit_len = 32;
    int dummy = 0;
    int data_bit_len = 8;

    esp_rom_spiflash_write_enable(&g_rom_flashchip);
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
    esp_rom_spiflash_write_enable(&g_rom_flashchip);
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
#if CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR
    static const esp_rom_opiflash_def_t opiflash_cmd_def_mxic = OPI_CMD_FORMAT_MXIC_STR();
#elif CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR
    static const esp_rom_opiflash_def_t opiflash_cmd_def_mxic = OPI_CMD_FORMAT_MXIC_DTR();
#endif
    esp_rom_opiflash_legacy_driver_init(&opiflash_cmd_def_mxic);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);

    // increase flash output driver strength
    s_set_flash_ouput_driver_strength(1, 7);

    // STR/DTR specific setting
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
#if CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR
    s_set_pin_drive_capability(3);
    s_set_flash_dtr_str_opi_mode(1, 0x1);
    esp_rom_opiflash_cache_mode_config(mode, &rom_opiflash_cmd_def->cache_rd_cmd);
    esp_rom_spi_set_dtr_swap_mode(0, false, false);
    esp_rom_spi_set_dtr_swap_mode(1, false, false);
#else //CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR
    s_set_pin_drive_capability(3);
    bool need_swap = s_mxic_dtr_need_swap();
    s_set_flash_dtr_str_opi_mode(1, 0x2);
    esp_rom_opiflash_cache_mode_config(mode, &rom_opiflash_cmd_def->cache_rd_cmd);
    esp_rom_spi_set_dtr_swap_mode(0, need_swap, need_swap);
    esp_rom_spi_set_dtr_swap_mode(1, need_swap, need_swap);
#endif

    esp_rom_opiflash_wait_idle();
}
#endif   // #if CONFIG_SPI_FLASH_SUPPORT_MXIC_OPI_CHIP

static void s_mxic_set_required_regs(uint32_t chip_id)
{
    bool is_swap = false;
#if CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR
    is_swap = true;
#else
    //STR mode does not need to enable ddr_swap registers
#endif
    esp_rom_spi_set_dtr_swap_mode(0, is_swap, is_swap);
    esp_rom_spi_set_dtr_swap_mode(1, is_swap, is_swap);
}


/*----------------------------------------------------------------------------------------------------
                                General Functions
-----------------------------------------------------------------------------------------------------*/
typedef struct opi_flash_func_t {
    esp_err_t (*probe)(uint32_t flash_id, uint8_t *out_vendor_id);      //Function pointer for detecting Flash chip vendor
    void (*init)(esp_rom_spiflash_read_mode_t mode);                    //Function pointer for initialising certain Flash chips
    void (*regs_set)(uint32_t flash_id);                                //Function pointer for setting required registers, decided by certain flash chips.
} opi_flash_func_t;

#if CONFIG_SPI_FLASH_SUPPORT_MXIC_OPI_CHIP
static const opi_flash_func_t opi_flash_func_mxic = {
    .probe = &s_probe_mxic_chip,
    .init = &s_flash_init_mxic,
    .regs_set = &s_mxic_set_required_regs,
};
#endif

static const opi_flash_func_t *registered_chip_funcs[] = {
#if CONFIG_SPI_FLASH_SUPPORT_MXIC_OPI_CHIP
    &opi_flash_func_mxic,
#endif
    NULL,
};

//To check which Flash chip is used
static const opi_flash_func_t **s_chip_func = NULL;

esp_err_t esp_opiflash_init(uint32_t chip_id)
{
    esp_err_t ret = ESP_FAIL;
    esp_rom_spiflash_read_mode_t mode;
#if CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR
    mode = ESP_ROM_SPIFLASH_OPI_STR_MODE;
#elif CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR
    mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE;
#else
    mode = ESP_ROM_SPIFLASH_FASTRD_MODE;
#endif

    const opi_flash_func_t **chip_func = &registered_chip_funcs[0];

    uint8_t vendor_id = 0;
    while (*chip_func) {
        ret = (*chip_func)->probe(chip_id, &vendor_id);
        if (ret == ESP_OK) {
            // Detect this is the supported chip type
            s_chip_id = chip_id;
            (*chip_func)->init(mode);
            s_register_rom_function();
            break;
        }
        chip_func++;
    }
    s_chip_func = chip_func;

    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "No detected Flash chip, please check the menuconfig to see if the chip is supported");
        abort();
    }

    return ESP_OK;
}

/**
 * Add Flash chip specifically required MSPI register settings here
 */
void esp_opiflash_set_required_regs(void)
{
    (*s_chip_func)->regs_set(s_chip_id);
}
