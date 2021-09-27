/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "string.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_log.h"
#include "spiram_psram.h"
#include "esp32s3/rom/ets_sys.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"
#include "esp32s3/rom/gpio.h"
#include "esp32s3/rom/cache.h"
#include "soc/io_mux_reg.h"
#include "soc/dport_reg.h"
#include "soc/syscon_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/efuse_reg.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/periph_ctrl.h"

#if CONFIG_SPIRAM_MODE_OCT
#include "soc/rtc.h"
#include "spi_flash_private.h"

#define OPI_PSRAM_SYNC_READ             0x0000
#define OPI_PSRAM_SYNC_WRITE            0x8080
#define OPI_PSRAM_REG_READ              0x4040
#define OPI_PSRAM_REG_WRITE             0xC0C0
#define OCT_PSRAM_RD_CMD_BITLEN         16
#define OCT_PSRAM_WR_CMD_BITLEN         16
#define OCT_PSRAM_ADDR_BITLEN           32
#define OCT_PSRAM_RD_DUMMY_BITLEN       (2*(10-1))
#define OCT_PSRAM_WR_DUMMY_BITLEN       (2*(5-1))
#define OCT_PSRAM_CS1_IO                CONFIG_DEFAULT_PSRAM_CS_IO

#define OCT_PSRAM_CS_SETUP_TIME         3
#define OCT_PSRAM_CS_HOLD_TIME          3
#define OCT_PSRAM_CS_HOLD_DELAY         2

typedef struct {
    union {
        struct {
            uint8_t drive_str: 2;
            uint8_t read_latency: 3;
            uint8_t lt: 1;
            uint8_t rsvd0_1: 2;
        };
        uint8_t val;
    } mr0;
    union {
        struct {
            uint8_t vendor_id: 5;
            uint8_t rsvd0_2: 3;
        };
        uint8_t val;
    } mr1;
    union {
        struct {
            uint8_t density: 3;
            uint8_t dev_id: 2;
            uint8_t rsvd1_2: 2;
            uint8_t gb: 1;
        };
        uint8_t val;
    } mr2;
    union {
        struct {
            uint8_t rsvd3_7: 5;
            uint8_t srf: 1;
            uint8_t vcc: 1;
            uint8_t rsvd0: 1;
        };
        uint8_t val;
    } mr3;
    union {
        struct {
            uint8_t pasr: 3;
            uint8_t rf: 1;
            uint8_t rsvd3: 1;
            uint8_t wr_latency: 3;
        };
        uint8_t val;
    } mr4;
    union {
        struct {
            uint8_t bl: 2;
            uint8_t bt: 1;
            uint8_t rsvd0_4: 5;
        };
        uint8_t val;
    } mr8;
} opi_psram_mode_reg_t;

static const char* TAG = "opi psram";
static DRAM_ATTR psram_size_t s_psram_size;
static void IRAM_ATTR s_config_psram_spi_phases(void);

uint8_t IRAM_ATTR psram_get_cs_io(void)
{
    return OCT_PSRAM_CS1_IO;
}

/**
 * Initialise mode registers of the PSRAM
 */
static void IRAM_ATTR s_init_psram_mode_reg(int spi_num, opi_psram_mode_reg_t *mode_reg_config)
{
    esp_rom_spiflash_read_mode_t mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE;
    int cmd_len = 16;
    uint32_t addr = 0x0;
    int addr_bit_len = 32;
    int dummy = OCT_PSRAM_RD_DUMMY_BITLEN;
    opi_psram_mode_reg_t mode_reg = {0};
    int data_bit_len = 16;

    //read
    esp_rom_opiflash_exec_cmd(spi_num, mode,
                             OPI_PSRAM_REG_READ, cmd_len,
                             addr, addr_bit_len,
                             dummy,
                             NULL, 0,
                             &mode_reg.mr0.val, data_bit_len,
                             BIT(1),
                             false);
    //modify
    mode_reg.mr0.lt = mode_reg_config->mr0.lt;
    mode_reg.mr0.read_latency = mode_reg_config->mr0.read_latency;
    mode_reg.mr0.drive_str = mode_reg_config->mr0.drive_str;

    //write
    esp_rom_opiflash_exec_cmd(spi_num, mode,
                             OPI_PSRAM_REG_WRITE, cmd_len,
                             addr, addr_bit_len,
                             0,
                             &mode_reg.mr0.val, 16,
                             NULL, 0,
                             BIT(1),
                             false);
}

static void IRAM_ATTR s_get_psram_mode_reg(int spi_num, opi_psram_mode_reg_t *out_reg)
{
    esp_rom_spiflash_read_mode_t mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE;
    int cmd_len = 16;
    int addr_bit_len = 32;
    int dummy = OCT_PSRAM_RD_DUMMY_BITLEN;
    int data_bit_len = 16;

    //Read MR0 register
    esp_rom_opiflash_exec_cmd(spi_num, mode,
                             OPI_PSRAM_REG_READ, cmd_len,
                             0x0, addr_bit_len,
                             dummy,
                             NULL, 0,
                             &out_reg->mr0.val, data_bit_len,
                             BIT(1),
                             false);
    //Read MR2 register
    esp_rom_opiflash_exec_cmd(spi_num, mode,
                            OPI_PSRAM_REG_READ, cmd_len,
                            0x2, addr_bit_len,
                            dummy,
                            NULL, 0,
                            &out_reg->mr2.val, data_bit_len,
                            BIT(1),
                            false);
    //Read MR4 register
    esp_rom_opiflash_exec_cmd(spi_num, mode,
                            OPI_PSRAM_REG_READ, cmd_len,
                            0x4, addr_bit_len,
                            dummy,
                            NULL, 0,
                            &out_reg->mr4.val, data_bit_len,
                            BIT(1),
                            false);
    //Read MR8 register
    esp_rom_opiflash_exec_cmd(spi_num, mode,
                            OPI_PSRAM_REG_READ, cmd_len,
                            0x8, addr_bit_len,
                            dummy,
                            NULL, 0,
                            &out_reg->mr8.val, data_bit_len,
                            BIT(1),
                            false);
}

static void IRAM_ATTR s_print_psram_info(opi_psram_mode_reg_t *reg_val)
{
    ESP_EARLY_LOGI(TAG, "vendor id : 0x%02x (%s)", reg_val->mr1.vendor_id, reg_val->mr1.vendor_id == 0x0d ? "AP" : "UNKNOWN");
    ESP_EARLY_LOGI(TAG, "dev id    : 0x%02x (generation %d)", reg_val->mr2.dev_id, reg_val->mr2.dev_id + 1);
    ESP_EARLY_LOGI(TAG, "density   : 0x%02x (%d Mbit)", reg_val->mr2.density, reg_val->mr2.density == 0x1 ? 32 :
                                                                              reg_val->mr2.density == 0X3 ? 64 :
                                                                              reg_val->mr2.density == 0x5 ? 128 :
                                                                              reg_val->mr2.density == 0x7 ? 256 : 0);
    ESP_EARLY_LOGI(TAG, "good-die  : 0x%02x (%s)", reg_val->mr2.gb, reg_val->mr2.gb == 1 ? "Pass" : "Fail");
    ESP_EARLY_LOGI(TAG, "Latency   : 0x%02x (%s)", reg_val->mr0.lt, reg_val->mr0.lt == 1 ? "Fixed" : "Variable");
    ESP_EARLY_LOGI(TAG, "VCC       : 0x%02x (%s)", reg_val->mr3.vcc, reg_val->mr3.vcc == 1 ? "3V" : "1.8V");
    ESP_EARLY_LOGI(TAG, "SRF       : 0x%02x (%s Refresh)", reg_val->mr3.srf, reg_val->mr3.srf == 0x1 ? "Fast" : "Slow");
    ESP_EARLY_LOGI(TAG, "BurstType : 0x%02x (%s Wrap)", reg_val->mr8.bt, reg_val->mr8.bt == 1 && reg_val->mr8.bl != 3 ? "Hybrid" : "");
    ESP_EARLY_LOGI(TAG, "BurstLen  : 0x%02x (%d Byte)", reg_val->mr8.bl, reg_val->mr8.bl == 0x00 ? 16 :
                                                                         reg_val->mr8.bl == 0x01 ? 32 :
                                                                         reg_val->mr8.bl == 0x10 ? 64 : 1024);
    ESP_EARLY_LOGI(TAG, "Readlatency  : 0x%02x (%d cycles@%s)", reg_val->mr0.read_latency,  reg_val->mr0.read_latency * 2 + 6,
                                                                reg_val->mr0.lt == 1 ? "Fixed" : "Variable");
    ESP_EARLY_LOGI(TAG, "DriveStrength: 0x%02x (1/%d)", reg_val->mr0.drive_str, reg_val->mr0.drive_str == 0x00 ? 1 :
                                                                                reg_val->mr0.drive_str == 0x01 ? 2 :
                                                                                reg_val->mr0.drive_str == 0x02 ? 4 : 8);
}

static void psram_set_cs_timing(void)
{
    //SPI0/1 share the cs_hold / cs_setup, cd_hold_time / cd_setup_time, cs_hold_delay registers for PSRAM, so we only need to set SPI0 related registers here
    SET_PERI_REG_MASK(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_HOLD_M | SPI_MEM_SPI_SMEM_CS_SETUP_M);
    SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_HOLD_TIME_V, OCT_PSRAM_CS_HOLD_TIME, SPI_MEM_SPI_SMEM_CS_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_SETUP_TIME_V, OCT_PSRAM_CS_SETUP_TIME, SPI_MEM_SPI_SMEM_CS_SETUP_TIME_S);
    //CS1 high time
    SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_HOLD_DELAY_V, OCT_PSRAM_CS_HOLD_DELAY, SPI_MEM_SPI_SMEM_CS_HOLD_DELAY_S);
}

static void IRAM_ATTR s_init_psram_pins(void)
{
    //Set cs1 pin function
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[OCT_PSRAM_CS1_IO],  FUNC_SPICS1_SPICS1);
    //Set mspi cs1 drive strength
    PIN_SET_DRV(GPIO_PIN_MUX_REG[OCT_PSRAM_CS1_IO], 3);
    //Set psram clock pin drive strength
    REG_SET_FIELD(SPI_MEM_DATE_REG(0), SPI_MEM_SPI_SMEM_SPICLK_FUN_DRV, 3);
}

esp_err_t IRAM_ATTR psram_enable(psram_cache_mode_t mode, psram_vaddr_mode_t vaddrmode)
{
    s_init_psram_pins();
    psram_set_cs_timing();

    //enter MSPI slow mode to init PSRAM device registers
    spi_timing_enter_mspi_low_speed_mode(true);

    //set to variable dummy mode
    SET_PERI_REG_MASK(SPI_MEM_DDR_REG(1), SPI_MEM_SPI_FMEM_VAR_DUMMY);
#if CONFIG_ESPTOOLPY_FLASH_VENDOR_MXIC && CONFIG_ESPTOOLPY_FLASHMODE_OPI_DTR
    esp_rom_spi_set_dtr_swap_mode(1, false, false);
#endif

    //Set PSRAM read latency and drive strength
    static DRAM_ATTR opi_psram_mode_reg_t mode_reg = {0};
    mode_reg.mr0.lt = 1;
    mode_reg.mr0.read_latency = 2;
    mode_reg.mr0.drive_str = 0;
    s_init_psram_mode_reg(1, &mode_reg);
    //Print PSRAM info
    s_get_psram_mode_reg(1, &mode_reg);
    s_print_psram_info(&mode_reg);
    s_psram_size = mode_reg.mr2.density == 0x1 ? PSRAM_SIZE_32MBITS  :
                   mode_reg.mr2.density == 0X3 ? PSRAM_SIZE_64MBITS  :
                   mode_reg.mr2.density == 0x5 ? PSRAM_SIZE_128MBITS :
                   mode_reg.mr2.density == 0x7 ? PSRAM_SIZE_256MBITS : 0;

#if CONFIG_ESPTOOLPY_FLASH_VENDOR_MXIC && CONFIG_ESPTOOLPY_FLASHMODE_OPI_DTR
    esp_rom_spi_set_dtr_swap_mode(1, true, true);
#endif
    //Do PSRAM timing tuning, we use SPI1 to do the tuning, and set the SPI0 PSRAM timing related registers accordingly
    spi_timing_psram_tuning();
    ////Back to the high speed mode. Flash/PSRAM clocks are set to the clock that user selected. SPI0/1 registers are all set correctly
    spi_timing_enter_mspi_high_speed_mode(true);

    /**
     * Tuning may change SPI1 regs, whereas legacy spi_flash APIs rely on these regs.
     * This function is to restore SPI1 init state.
     */
    spi_flash_set_rom_required_regs();

    s_config_psram_spi_phases();
    return ESP_OK;
}

//Configure PSRAM SPI0 phase related registers here according to the PSRAM chip requirement
static void IRAM_ATTR s_config_psram_spi_phases(void)
{
    //Config Write CMD phase for SPI0 to access PSRAM
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_CACHE_SRAM_USR_WCMD_M);
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN, OCT_PSRAM_WR_CMD_BITLEN - 1, SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE, OPI_PSRAM_SYNC_WRITE, SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S);

    //Config Read CMD phase for SPI0 to access PSRAM
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_CACHE_SRAM_USR_RCMD_M);
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V, OCT_PSRAM_RD_CMD_BITLEN - 1, SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V, OPI_PSRAM_SYNC_READ, SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S);

    //Config ADDR phase
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_SRAM_ADDR_BITLEN_V, OCT_PSRAM_ADDR_BITLEN - 1, SPI_MEM_SRAM_ADDR_BITLEN_S);
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_CACHE_USR_SCMD_4BYTE_M);

    //Config RD/WR Dummy phase
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_USR_RD_SRAM_DUMMY_M | SPI_MEM_USR_WR_SRAM_DUMMY_M);
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_SRAM_RDUMMY_CYCLELEN_V, OCT_PSRAM_RD_DUMMY_BITLEN - 1, SPI_MEM_SRAM_RDUMMY_CYCLELEN_S);
    SET_PERI_REG_MASK(SPI_MEM_SPI_SMEM_DDR_REG(0), SPI_MEM_SPI_SMEM_VAR_DUMMY_M);
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_SRAM_WDUMMY_CYCLELEN_V, OCT_PSRAM_WR_DUMMY_BITLEN - 1, SPI_MEM_SRAM_WDUMMY_CYCLELEN_S);

    CLEAR_PERI_REG_MASK(SPI_MEM_SPI_SMEM_DDR_REG(0), SPI_MEM_SPI_SMEM_DDR_WDAT_SWP_M | SPI_MEM_SPI_SMEM_DDR_RDAT_SWP_M);
    SET_PERI_REG_MASK(SPI_MEM_SPI_SMEM_DDR_REG(0), SPI_MEM_SPI_SMEM_DDR_EN_M);

    SET_PERI_REG_MASK(SPI_MEM_SRAM_CMD_REG(0), SPI_MEM_SDUMMY_OUT_M | SPI_MEM_SCMD_OCT_M | SPI_MEM_SADDR_OCT_M | SPI_MEM_SDOUT_OCT_M | SPI_MEM_SDIN_OCT_M);
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_SRAM_OCT_M);

    Cache_Resume_DCache(0);
}

psram_size_t psram_get_size()
{
    return s_psram_size;
}

#endif  //#if CONFIG_SPIRAM_MODE_OCT
