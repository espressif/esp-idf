/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "string.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_bit_defs.h"
#include "esp_log.h"
#include "../esp_psram_impl.h"
#include "esp32s3/rom/ets_sys.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"
#include "esp32s3/rom/cache.h"
#include "soc/gpio_periph.h"
#include "soc/io_mux_reg.h"
#include "soc/syscon_reg.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/mspi_timing_tuning.h"
#include "esp_private/esp_gpio_reserve.h"

#define OPI_PSRAM_SYNC_READ             0x0000
#define OPI_PSRAM_SYNC_WRITE            0x8080
#define OPI_PSRAM_REG_READ              0x4040
#define OPI_PSRAM_REG_WRITE             0xC0C0
#define OCT_PSRAM_RD_CMD_BITLEN         16
#define OCT_PSRAM_WR_CMD_BITLEN         16
#define OCT_PSRAM_ADDR_BITLEN           32
#define OCT_PSRAM_RD_DUMMY_BITLEN       (2*(10-1))
#define OCT_PSRAM_WR_DUMMY_BITLEN       (2*(5-1))
#define OCT_PSRAM_CS1_IO                MSPI_IOMUX_PIN_NUM_CS1
#define OCT_PSRAM_VENDOR_ID             0xD

#define OCT_PSRAM_CS_SETUP_TIME         3
#define OCT_PSRAM_CS_HOLD_TIME          3
#define OCT_PSRAM_CS_ECC_HOLD_TIME      3
#define OCT_PSRAM_CS_HOLD_DELAY         2

#define OCT_PSRAM_PAGE_SIZE             2       //2 for 1024B
#define OCT_PSRAM_ECC_ENABLE_MASK       BIT(8)

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

static const char* TAG = "octal_psram";
static uint32_t s_psram_size;   //this stands for physical psram size in bytes
static void s_config_psram_spi_phases(void);

uint8_t esp_psram_impl_get_cs_io(void)
{
    return OCT_PSRAM_CS1_IO;
}

/**
 * Initialise mode registers of the PSRAM
 */
static void s_init_psram_mode_reg(int spi_num, opi_psram_mode_reg_t *mode_reg_config)
{
    esp_rom_spiflash_read_mode_t mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE;
    int cmd_len = 16;
    uint32_t addr = 0x0;    //0x0 is the MR0 register
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

#if CONFIG_SPIRAM_ECC_ENABLE
    addr = 0x8;     //0x8 is the MR8 register
    data_bit_len = 8;
    //read
    esp_rom_opiflash_exec_cmd(spi_num, mode,
                              OPI_PSRAM_REG_READ, cmd_len,
                              addr, addr_bit_len,
                              dummy,
                              NULL, 0,
                              &mode_reg.mr8.val, data_bit_len,
                              BIT(1),
                              false);

    //modify
    mode_reg.mr8.bt = mode_reg_config->mr8.bt;
    mode_reg.mr8.bl = mode_reg_config->mr8.bl;

    //write
    esp_rom_opiflash_exec_cmd(spi_num, mode,
                              OPI_PSRAM_REG_WRITE, cmd_len,
                              addr, addr_bit_len,
                              0,
                              &mode_reg.mr8.val, 16,
                              NULL, 0,
                              BIT(1),
                              false);
#endif
}

static void s_get_psram_mode_reg(int spi_num, opi_psram_mode_reg_t *out_reg)
{
    esp_rom_spiflash_read_mode_t mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE;
    int cmd_len = 16;
    int addr_bit_len = 32;
    int dummy = OCT_PSRAM_RD_DUMMY_BITLEN;
    int data_bit_len = 16;

    //Read MR0~1 register
    esp_rom_opiflash_exec_cmd(spi_num, mode,
                              OPI_PSRAM_REG_READ, cmd_len,
                              0x0, addr_bit_len,
                              dummy,
                              NULL, 0,
                              &out_reg->mr0.val, data_bit_len,
                              BIT(1),
                              false);
    //Read MR2~3 register
    esp_rom_opiflash_exec_cmd(spi_num, mode,
                              OPI_PSRAM_REG_READ, cmd_len,
                              0x2, addr_bit_len,
                              dummy,
                              NULL, 0,
                              &out_reg->mr2.val, data_bit_len,
                              BIT(1),
                              false);
    data_bit_len = 8;
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

static void s_print_psram_info(opi_psram_mode_reg_t *reg_val)
{
    ESP_EARLY_LOGI(TAG, "vendor id    : 0x%02x (%s)", reg_val->mr1.vendor_id, reg_val->mr1.vendor_id == 0x0d ? "AP" : "UNKNOWN");
    ESP_EARLY_LOGI(TAG, "dev id       : 0x%02x (generation %d)", reg_val->mr2.dev_id, reg_val->mr2.dev_id + 1);
    ESP_EARLY_LOGI(TAG, "density      : 0x%02x (%d Mbit)", reg_val->mr2.density, reg_val->mr2.density == 0x1 ? 32 :
                   reg_val->mr2.density == 0X3 ? 64 :
                   reg_val->mr2.density == 0x5 ? 128 :
                   reg_val->mr2.density == 0x7 ? 256 : 0);
    ESP_EARLY_LOGI(TAG, "good-die     : 0x%02x (%s)", reg_val->mr2.gb, reg_val->mr2.gb == 1 ? "Pass" : "Fail");
    ESP_EARLY_LOGI(TAG, "Latency      : 0x%02x (%s)", reg_val->mr0.lt, reg_val->mr0.lt == 1 ? "Fixed" : "Variable");
    ESP_EARLY_LOGI(TAG, "VCC          : 0x%02x (%s)", reg_val->mr3.vcc, reg_val->mr3.vcc == 1 ? "3V" : "1.8V");
    ESP_EARLY_LOGI(TAG, "SRF          : 0x%02x (%s Refresh)", reg_val->mr3.srf, reg_val->mr3.srf == 0x1 ? "Fast" : "Slow");
    ESP_EARLY_LOGI(TAG, "BurstType    : 0x%02x (%s Wrap)", reg_val->mr8.bt, reg_val->mr8.bt == 1 && reg_val->mr8.bl != 3 ? "Hybrid" : "");
    ESP_EARLY_LOGI(TAG, "BurstLen     : 0x%02x (%d Byte)", reg_val->mr8.bl, reg_val->mr8.bl == 0x00 ? 16 :
                   reg_val->mr8.bl == 0x01 ? 32 :
                   reg_val->mr8.bl == 0x10 ? 64 : 1024);
    ESP_EARLY_LOGI(TAG, "Readlatency  : 0x%02x (%d cycles@%s)", reg_val->mr0.read_latency,  reg_val->mr0.read_latency * 2 + 6,
                   reg_val->mr0.lt == 1 ? "Fixed" : "Variable");
    ESP_EARLY_LOGI(TAG, "DriveStrength: 0x%02x (1/%d)", reg_val->mr0.drive_str, reg_val->mr0.drive_str == 0x00 ? 1 :
                   reg_val->mr0.drive_str == 0x01 ? 2 :
                   reg_val->mr0.drive_str == 0x02 ? 4 : 8);
}

static void s_set_psram_cs_timing(void)
{
    //SPI0/1 share the cs_hold / cs_setup, cd_hold_time / cd_setup_time, cs_hold_delay registers for PSRAM, so we only need to set SPI0 related registers here
    SET_PERI_REG_MASK(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_HOLD_M | SPI_MEM_SPI_SMEM_CS_SETUP_M);
    SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_HOLD_TIME_V, OCT_PSRAM_CS_HOLD_TIME, SPI_MEM_SPI_SMEM_CS_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_SETUP_TIME_V, OCT_PSRAM_CS_SETUP_TIME, SPI_MEM_SPI_SMEM_CS_SETUP_TIME_S);
#if CONFIG_SPIRAM_ECC_ENABLE
    SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_ECC_CS_HOLD_TIME_V, OCT_PSRAM_CS_ECC_HOLD_TIME, SPI_MEM_SPI_SMEM_ECC_CS_HOLD_TIME_S);
#endif
    //CS1 high time
    SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_HOLD_DELAY_V, OCT_PSRAM_CS_HOLD_DELAY, SPI_MEM_SPI_SMEM_CS_HOLD_DELAY_S);
}

static void s_init_psram_pins(void)
{
    //Set cs1 pin function
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[OCT_PSRAM_CS1_IO],  FUNC_SPICS1_SPICS1);
    //Set mspi cs1 drive strength
    PIN_SET_DRV(GPIO_PIN_MUX_REG[OCT_PSRAM_CS1_IO], 3);
    //Set psram clock pin drive strength
    REG_SET_FIELD(SPI_MEM_DATE_REG(0), SPI_MEM_SPI_SMEM_SPICLK_FUN_DRV, 3);

    // Preserve psram pins
    esp_gpio_reserve(BIT64(OCT_PSRAM_CS1_IO));
}

/**
 * Enable error correcting code feature
 *
 * Can add an input parameter for selecting ECC mode if needed
 */
static void s_configure_psram_ecc(void)
{
#if CONFIG_SPIRAM_ECC_ENABLE
    //Clear this bit to use ECC 16to17 mode
    CLEAR_PERI_REG_MASK(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_ECC_16TO18_BYTE_EN_M);
    SET_PERI_REG_BITS(SYSCON_SPI_MEM_ECC_CTRL_REG, SYSCON_SRAM_PAGE_SIZE_V, OCT_PSRAM_PAGE_SIZE, SYSCON_SRAM_PAGE_SIZE_S);
    SET_PERI_REG_MASK(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_ECC_SKIP_PAGE_CORNER_M);
    /**
     * Enable ECC region 0 (ACE0)
     * Default: ACE0 range: 0 ~ 256MB
     * Current Octal PSRAM is 8MB, ACE0 is enough
     */
    SET_PERI_REG_MASK(SYSCON_SRAM_ACE0_ATTR_REG, OCT_PSRAM_ECC_ENABLE_MASK);
    ESP_EARLY_LOGI(TAG, "ECC is enabled");
#else
    CLEAR_PERI_REG_MASK(SYSCON_SRAM_ACE0_ATTR_REG, OCT_PSRAM_ECC_ENABLE_MASK);
#endif
}

esp_err_t esp_psram_impl_enable(void)
{
    s_init_psram_pins();
    s_set_psram_cs_timing();
    s_configure_psram_ecc();

    //enter MSPI slow mode to init PSRAM device registers
    mspi_timing_enter_low_speed_mode(true);

    //set to variable dummy mode
    SET_PERI_REG_MASK(SPI_MEM_DDR_REG(1), SPI_MEM_SPI_FMEM_VAR_DUMMY);
    esp_rom_spi_set_dtr_swap_mode(1, false, false);

    //Set PSRAM read latency and drive strength
    static opi_psram_mode_reg_t mode_reg = {0};
    mode_reg.mr0.lt = 1;
    mode_reg.mr0.read_latency = 2;
    mode_reg.mr0.drive_str = 0;
    mode_reg.mr8.bl = 3;
    mode_reg.mr8.bt = 0;
    s_init_psram_mode_reg(1, &mode_reg);
    //Print PSRAM info
    s_get_psram_mode_reg(1, &mode_reg);
    if (mode_reg.mr1.vendor_id != OCT_PSRAM_VENDOR_ID) {
        ESP_EARLY_LOGE(TAG, "PSRAM ID read error: 0x%08x, PSRAM chip not found or not supported, or wrong PSRAM line mode", mode_reg.mr1.vendor_id);
        return ESP_ERR_NOT_SUPPORTED;
    }
    s_print_psram_info(&mode_reg);
    s_psram_size = mode_reg.mr2.density == 0x1 ? PSRAM_SIZE_4MB  :
                   mode_reg.mr2.density == 0X3 ? PSRAM_SIZE_8MB  :
                   mode_reg.mr2.density == 0x5 ? PSRAM_SIZE_16MB :
                   mode_reg.mr2.density == 0x7 ? PSRAM_SIZE_32MB :
                   mode_reg.mr2.density == 0x6 ? PSRAM_SIZE_64MB : 0;

    //Do PSRAM timing tuning, we use SPI1 to do the tuning, and set the SPI0 PSRAM timing related registers accordingly
    mspi_timing_psram_tuning();
    //Back to the high speed mode. Flash/PSRAM clocks are set to the clock that user selected. SPI0/1 registers are all set correctly
    mspi_timing_enter_high_speed_mode(true);

    /**
     * Tuning may change SPI1 regs, whereas legacy spi_flash APIs rely on these regs.
     * This function is to restore SPI1 init state.
     */
    spi_flash_set_rom_required_regs();
    //Flash chip requires MSPI specifically, call this function to set them
    spi_flash_set_vendor_required_regs();

    s_config_psram_spi_phases();
    return ESP_OK;
}

//Configure PSRAM SPI0 phase related registers here according to the PSRAM chip requirement
static void s_config_psram_spi_phases(void)
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

/*---------------------------------------------------------------------------------
 * Following APIs are not required to be IRAM-Safe
 *
 * Consider moving these to another file if this kind of APIs grows dramatically
 *-------------------------------------------------------------------------------*/
esp_err_t esp_psram_impl_get_physical_size(uint32_t *out_size_bytes)
{
    if (!out_size_bytes) {
        return ESP_ERR_INVALID_ARG;
    }

    *out_size_bytes = s_psram_size;
    return (s_psram_size ? ESP_OK : ESP_ERR_INVALID_STATE);
}

/**
 * This function is to get the available physical psram size in bytes.
 * If ECC is enabled, available PSRAM size will be 15/16 times its physical size.
 */
esp_err_t esp_psram_impl_get_available_size(uint32_t *out_size_bytes)
{
    if (!out_size_bytes) {
        return ESP_ERR_INVALID_ARG;
    }

#if CONFIG_SPIRAM_ECC_ENABLE
    *out_size_bytes = s_psram_size * 15 / 16;
#else
    *out_size_bytes = s_psram_size;
#endif
    return (s_psram_size ? ESP_OK : ESP_ERR_INVALID_STATE);
}
