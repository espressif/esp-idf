/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_private/periph_ctrl.h"
#include "../esp_psram_impl.h"
#include "rom/opi_flash.h"
#include "hal/psram_ctrlr_ll.h"

// Reset and Clock Control registers are mixing with other peripherals, so we need to use a critical section
#define PSRAM_RCC_ATOMIC() PERIPH_RCC_ATOMIC()

#define AP_HEX_PSRAM_SYNC_READ             0x0000
#define AP_HEX_PSRAM_SYNC_WRITE            0x8080
#define AP_HEX_PSRAM_BURST_READ            0x2020
#define AP_HEX_PSRAM_BURST_WRITE           0xA0A0
#define AP_HEX_PSRAM_REG_READ              0x4040
#define AP_HEX_PSRAM_REG_WRITE             0xC0C0
#define AP_HEX_PSRAM_RD_CMD_BITLEN         16
#define AP_HEX_PSRAM_WR_CMD_BITLEN         16
#define AP_HEX_PSRAM_ADDR_BITLEN           32
#define AP_HEX_PSRAM_RD_DUMMY_BITLEN       (2*(10-1))
#define AP_HEX_PSRAM_WR_DUMMY_BITLEN       (2*(5-1))
#define AP_HEX_PSRAM_VENDOR_ID             0xD

#define AP_HEX_PSRAM_CS_SETUP_TIME         4
#define AP_HEX_PSRAM_CS_HOLD_TIME          4
#define AP_HEX_PSRAM_CS_ECC_HOLD_TIME      4
#define AP_HEX_PSRAM_CS_HOLD_DELAY         3

typedef struct {
    union {
        struct {
            uint8_t drive_str: 2;
            uint8_t read_latency: 3;
            uint8_t lt: 1;
            uint8_t rsvd6: 1;
            uint8_t tso: 1;
        };
        uint8_t val;
    } mr0;
    union {
        struct {
            uint8_t vendor_id: 5;
            uint8_t rsvd0_2: 2;
            uint8_t ulp: 1;
        };
        uint8_t val;
    } mr1;
    union {
        struct {
            uint8_t density: 3;
            uint8_t dev_id: 2;
            uint8_t kgd: 3;
        };
        uint8_t val;
    } mr2;
    union {
        struct {
            uint8_t rsvd3_7: 4;
            uint8_t srf: 2;
            uint8_t rsvd0: 1;
            uint8_t rbx_en: 1;
        };
        uint8_t val;
    } mr3;
    union {
        struct {
            uint8_t pasr: 3;
            uint8_t rf: 2;
            uint8_t wr_latency: 3;
        };
        uint8_t val;
    } mr4;
    union {
        struct {
            uint8_t bl: 2;
            uint8_t bt: 1;
            uint8_t rbx: 1;
            uint8_t rsvd5: 2;
            uint8_t x16: 1;
            uint8_t rsvd7: 1;
        };
        uint8_t val;
    } mr8;
} hex_psram_mode_reg_t;

static const char* TAG = "hex_psram";
static uint32_t s_psram_size;   //this stands for physical psram size in bytes

/**
 * Common psram transaction
 */
static void s_psram_common_transaction(uint32_t mspi_id,
                                       uint32_t cmd, uint32_t cmd_bitlen,
                                       uint32_t addr, uint32_t addr_bitlen,
                                       uint32_t dummy_bits,
                                       uint8_t* mosi_data, uint32_t mosi_bitlen,
                                       uint8_t* miso_data, uint32_t miso_bitlen,
                                       bool is_write_erase_operation)
{
    psram_ctrlr_ll_common_transaction(mspi_id, cmd, cmd_bitlen, addr, addr_bitlen, dummy_bits,
                                      mosi_data, mosi_bitlen, miso_data, miso_bitlen,
                                      is_write_erase_operation);
}

/**
 * Initialise mode registers of the PSRAM
 */
static void s_init_psram_mode_reg(int spi_num, hex_psram_mode_reg_t *mode_reg_config)
{
    int cmd_len = 16;
    uint32_t addr = 0x0;
    int addr_bit_len = 32;
    int dummy = AP_HEX_PSRAM_RD_DUMMY_BITLEN;
    hex_psram_mode_reg_t mode_reg = {0};
    int data_bit_len = 16;

    //read
    s_psram_common_transaction(spi_num,
                               AP_HEX_PSRAM_REG_READ, cmd_len,
                               addr, addr_bit_len,
                               dummy,
                               NULL, 0,
                               &mode_reg.mr0.val, data_bit_len,
                               false);

    //modify
    mode_reg.mr0.lt = mode_reg_config->mr0.lt;
    mode_reg.mr0.read_latency = mode_reg_config->mr0.read_latency;
    mode_reg.mr0.drive_str = mode_reg_config->mr0.drive_str;

    //write
    s_psram_common_transaction(spi_num,
                               AP_HEX_PSRAM_REG_WRITE, cmd_len,
                               addr, addr_bit_len,
                               0,
                               &mode_reg.mr0.val, 16,
                               NULL, 0,
                               false);

    addr = 0x8;
    data_bit_len = 8;
    //read
    s_psram_common_transaction(spi_num,
                               AP_HEX_PSRAM_REG_READ, cmd_len,
                               addr, addr_bit_len,
                               dummy,
                               NULL, 0,
                               &mode_reg.mr8.val, data_bit_len,
                               false);

    //modify
    mode_reg.mr8.bt = mode_reg_config->mr8.bt;
    mode_reg.mr8.bl = mode_reg_config->mr8.bl;
    mode_reg.mr8.rbx = mode_reg_config->mr8.rbx;
    mode_reg.mr8.x16 = mode_reg_config->mr8.x16;

    //write
    s_psram_common_transaction(spi_num,
                               AP_HEX_PSRAM_REG_WRITE, cmd_len,
                               addr, addr_bit_len,
                               0,
                               &mode_reg.mr8.val, 16,
                               NULL, 0,
                               false);
}

static void s_get_psram_mode_reg(int spi_num, hex_psram_mode_reg_t *out_reg)
{
    int cmd_len = 16;
    int addr_bit_len = 32;
    int dummy = AP_HEX_PSRAM_RD_DUMMY_BITLEN;
    int data_bit_len = 16;

    //Read MR0~1 register
    s_psram_common_transaction(spi_num,
                               AP_HEX_PSRAM_REG_READ, cmd_len,
                               0x0, addr_bit_len,
                               dummy,
                               NULL, 0,
                               &out_reg->mr0.val, data_bit_len,
                               false);
    //Read MR2~3 register
    s_psram_common_transaction(spi_num,
                               AP_HEX_PSRAM_REG_READ, cmd_len,
                               0x2, addr_bit_len,
                               dummy,
                               NULL, 0,
                               &out_reg->mr2.val, data_bit_len,
                               false);
    data_bit_len = 8;
    //Read MR4 register
    s_psram_common_transaction(spi_num,
                               AP_HEX_PSRAM_REG_READ, cmd_len,
                               0x4, addr_bit_len,
                               dummy,
                               NULL, 0,
                               &out_reg->mr4.val, data_bit_len,
                               false);
    //Read MR8 register
    s_psram_common_transaction(spi_num,
                               AP_HEX_PSRAM_REG_READ, cmd_len,
                               0x8, addr_bit_len,
                               dummy,
                               NULL, 0,
                               &out_reg->mr8.val, data_bit_len,
                               false);
}

static void s_print_psram_info(hex_psram_mode_reg_t *reg_val)
{
    ESP_EARLY_LOGI(TAG, "vendor id    : 0x%02x (%s)", reg_val->mr1.vendor_id, reg_val->mr1.vendor_id == 0x0d ? "AP" : "UNKNOWN");
    ESP_EARLY_LOGI(TAG, "Latency      : 0x%02x (%s)", reg_val->mr0.lt, reg_val->mr0.lt == 1 ? "Fixed" : "Variable");
    ESP_EARLY_LOGI(TAG, "DriveStr.    : 0x%02x (%d Ohm)", reg_val->mr0.drive_str, reg_val->mr0.drive_str < 2 ? 25 * (reg_val->mr0.drive_str + 1) : 100 * (reg_val->mr0.drive_str - 1));
    ESP_EARLY_LOGI(TAG, "dev id       : 0x%02x (generation %d)", reg_val->mr2.dev_id, reg_val->mr2.dev_id + 1);
    ESP_EARLY_LOGI(TAG, "density      : 0x%02x (%d Mbit)", reg_val->mr2.density, reg_val->mr2.density == 0x1 ? 32 :
                   reg_val->mr2.density == 0X3 ? 64 :
                   reg_val->mr2.density == 0x5 ? 128 :
                   reg_val->mr2.density == 0x7 ? 256 : 0);
    ESP_EARLY_LOGI(TAG, "good-die     : 0x%02x (%s)", reg_val->mr2.kgd, reg_val->mr2.kgd == 6 ? "Pass" : "Fail");
    ESP_EARLY_LOGI(TAG, "SRF          : 0x%02x (%s Refresh)", reg_val->mr3.srf, reg_val->mr3.srf == 0x1 ? "Fast" : "Slow");
    ESP_EARLY_LOGI(TAG, "BurstType    : 0x%02x (%s Wrap)", reg_val->mr8.bt, reg_val->mr8.bt == 1 && reg_val->mr8.bl != 3 ? "Hybrid" : "");
    ESP_EARLY_LOGI(TAG, "BurstLen     : 0x%02x (%d Byte)", reg_val->mr8.bl, reg_val->mr8.bl == 0x00 ? 16 :
                   reg_val->mr8.bl == 0x01 ? 32 :
                   reg_val->mr8.bl == 0x10 ? 64 : 2048);
    ESP_EARLY_LOGI(TAG, "BitMode      : 0x%02x (%s Mode)", reg_val->mr8.x16, reg_val->mr8.x16 == 1 ? "X16" : "X8");

    ESP_EARLY_LOGI(TAG, "Readlatency  : 0x%02x (%d cycles@%s)", reg_val->mr0.read_latency,  reg_val->mr0.read_latency * 2 + 6,
                   reg_val->mr0.lt == 1 ? "Fixed" : "Variable");
    ESP_EARLY_LOGI(TAG, "DriveStrength: 0x%02x (1/%d)", reg_val->mr0.drive_str, reg_val->mr0.drive_str == 0x00 ? 1 :
                   reg_val->mr0.drive_str == 0x01 ? 2 :
                   reg_val->mr0.drive_str == 0x02 ? 4 : 8);
}

static void s_config_mspi_for_psram(void)
{
    //Config Write CMD phase for SPI0 to access PSRAM
    psram_ctrlr_ll_set_wr_cmd(PSRAM_CTRLR_LL_MSPI_ID_2, AP_HEX_PSRAM_WR_CMD_BITLEN, AP_HEX_PSRAM_BURST_WRITE);

    //Config Read CMD phase for SPI0 to access PSRAM
    psram_ctrlr_ll_set_rd_cmd(PSRAM_CTRLR_LL_MSPI_ID_2, AP_HEX_PSRAM_RD_CMD_BITLEN, AP_HEX_PSRAM_BURST_READ);

    //Config ADDR phase
    psram_ctrlr_ll_set_addr_bitlen(PSRAM_CTRLR_LL_MSPI_ID_2, AP_HEX_PSRAM_ADDR_BITLEN);
    psram_ctrlr_ll_enable_4byte_addr(PSRAM_CTRLR_LL_MSPI_ID_2, true);

    //Config RD/WR Dummy phase
    psram_ctrlr_ll_set_wr_dummy(PSRAM_CTRLR_LL_MSPI_ID_2, AP_HEX_PSRAM_WR_DUMMY_BITLEN);
    psram_ctrlr_ll_set_rd_dummy(PSRAM_CTRLR_LL_MSPI_ID_2, AP_HEX_PSRAM_RD_DUMMY_BITLEN);
    psram_ctrlr_ll_enable_variable_dummy(PSRAM_CTRLR_LL_MSPI_ID_2, true);
    psram_ctrlr_ll_enable_wr_dummy_level_control(PSRAM_CTRLR_LL_MSPI_ID_2, true);

    //DDR
    psram_ctrlr_ll_enable_ddr_wr_data_swap(PSRAM_CTRLR_LL_MSPI_ID_2, false);
    psram_ctrlr_ll_enable_ddr_rd_data_swap(PSRAM_CTRLR_LL_MSPI_ID_2, false);
    psram_ctrlr_ll_enable_ddr_mode(PSRAM_CTRLR_LL_MSPI_ID_2, true);

    //Line mode
    psram_ctrlr_ll_enable_oct_line_mode(PSRAM_CTRLR_LL_MSPI_ID_2, true);
    psram_ctrlr_ll_enable_hex_data_line_mode(PSRAM_CTRLR_LL_MSPI_ID_2, true);
#if CONFIG_SPIRAM_USE_8LINE_MODE
    psram_ctrlr_ll_enable_hex_data_line_mode(PSRAM_CTRLR_LL_MSPI_ID_2, false);
#endif

    //AXI
    psram_ctrlr_ll_enable_axi_access(PSRAM_CTRLR_LL_MSPI_ID_2, true);
    psram_ctrlr_ll_enable_wr_splice(PSRAM_CTRLR_LL_MSPI_ID_2, true);
    psram_ctrlr_ll_enable_rd_splice(PSRAM_CTRLR_LL_MSPI_ID_2, true);
}

static void s_set_psram_cs_timing(void)
{
    psram_ctrlr_ll_set_cs_setup(PSRAM_CTRLR_LL_MSPI_ID_2, AP_HEX_PSRAM_CS_SETUP_TIME);
    psram_ctrlr_ll_set_cs_hold(PSRAM_CTRLR_LL_MSPI_ID_2, AP_HEX_PSRAM_CS_HOLD_TIME);
    psram_ctrlr_ll_set_cs_hold_delay(PSRAM_CTRLR_LL_MSPI_ID_2, AP_HEX_PSRAM_CS_HOLD_DELAY);
#if CONFIG_SPIRAM_ECC_ENABLE
    psram_ctrlr_ll_set_cs_hold_ecc(PSRAM_CTRLR_LL_MSPI_ID_2, AP_HEX_PSRAM_CS_ECC_HOLD_TIME);
#endif
}

#if CONFIG_SPIRAM_ECC_ENABLE
static void s_mspi_ecc_show_info(void)
{
    for (int i = 0; i < PSRAM_CTRLR_LL_PMS_REGION_NUMS; i++) {
        ESP_EARLY_LOGV(TAG, "region[%d] addr: 0x%08x", i, psram_ctrlr_ll_get_pms_region_start_addr(PSRAM_CTRLR_LL_MSPI_ID_2, i));
        ESP_EARLY_LOGV(TAG, "region[%d] size: 0x%08x", i, psram_ctrlr_ll_get_pms_region_size(PSRAM_CTRLR_LL_MSPI_ID_2, i));
    }

    uint32_t page_size = psram_ctrlr_ll_get_page_size(PSRAM_CTRLR_LL_MSPI_ID_2);
    ESP_EARLY_LOGV(TAG, "ECC page size: %d", page_size);
}

/**
 * Enable error correcting code feature
 *
 * Can add an input parameter for selecting ECC mode if needed
 */
static void s_configure_psram_ecc(void)
{
    psram_ctrlr_ll_enable_16to18_ecc(PSRAM_CTRLR_LL_MSPI_ID_2, true);
    psram_ctrlr_ll_enable_skip_page_corner(PSRAM_CTRLR_LL_MSPI_ID_2, true);
    psram_ctrlr_ll_enable_split_trans(PSRAM_CTRLR_LL_MSPI_ID_2, true);
    psram_ctrlr_ll_set_page_size(PSRAM_CTRLR_LL_MSPI_ID_2, 2048);
    psram_ctrlr_ll_enable_ecc_addr_conversion(PSRAM_CTRLR_LL_MSPI_ID_2, 2048);

    /**
     * Enable ECC region 0 (ACE0)
     * Default: ACE0 range: 0 ~ 256MB
     * Current Hex PSRAM is 8MB, ACE0 is enough
     */
    psram_ctrlr_ll_enable_pms_region_ecc(PSRAM_CTRLR_LL_MSPI_ID_2, 0, true);

    ESP_EARLY_LOGI(TAG, "ECC is enabled");
    s_mspi_ecc_show_info();
}
#endif  //#if CONFIG_SPIRAM_ECC_ENABLE

esp_err_t esp_psram_impl_enable(void)
{
    PSRAM_RCC_ATOMIC() {
        psram_ctrlr_ll_enable_module_clock(PSRAM_CTRLR_LL_MSPI_ID_2, true);
        psram_ctrlr_ll_reset_module_clock(PSRAM_CTRLR_LL_MSPI_ID_2);
        psram_ctrlr_ll_select_clk_source(PSRAM_CTRLR_LL_MSPI_ID_2, PSRAM_CLK_SRC_XTAL);
    }

    s_set_psram_cs_timing();
#if CONFIG_SPIRAM_ECC_ENABLE
    s_configure_psram_ecc();
#endif
    //enter MSPI slow mode to init PSRAM device registers
    psram_ctrlr_ll_set_bus_clock(PSRAM_CTRLR_LL_MSPI_ID_2, 2);
    psram_ctrlr_ll_enable_dll(PSRAM_CTRLR_LL_MSPI_ID_2, true);
    psram_ctrlr_ll_enable_dll(PSRAM_CTRLR_LL_MSPI_ID_3, true);

    static hex_psram_mode_reg_t mode_reg = {};
    mode_reg.mr0.lt = 1;
    mode_reg.mr0.read_latency = 2;
    mode_reg.mr0.drive_str = 0;
    mode_reg.mr8.bl = 3;
    mode_reg.mr8.bt = 0;
    mode_reg.mr8.rbx = 1;
    mode_reg.mr8.x16 = 1;
#if CONFIG_SPIRAM_USE_8LINE_MODE
    mode_reg.mr8.x16 = 0;
#endif
    s_init_psram_mode_reg(PSRAM_CTRLR_LL_MSPI_ID_3, &mode_reg);
    //Print PSRAM info
    s_get_psram_mode_reg(PSRAM_CTRLR_LL_MSPI_ID_3, &mode_reg);
    if (mode_reg.mr1.vendor_id != AP_HEX_PSRAM_VENDOR_ID) {
        ESP_EARLY_LOGE(TAG, "PSRAM ID read error: 0x%08x, PSRAM chip not found or not supported, or wrong PSRAM line mode", mode_reg.mr1.vendor_id);
        return ESP_ERR_NOT_SUPPORTED;
    }
    s_print_psram_info(&mode_reg);
    s_psram_size = mode_reg.mr2.density == 0x1 ? PSRAM_SIZE_4MB  :
                   mode_reg.mr2.density == 0X3 ? PSRAM_SIZE_8MB  :
                   mode_reg.mr2.density == 0x5 ? PSRAM_SIZE_16MB :
                   mode_reg.mr2.density == 0x7 ? PSRAM_SIZE_32MB :
                   mode_reg.mr2.density == 0x6 ? PSRAM_SIZE_64MB : 0;

    s_config_mspi_for_psram();
    return ESP_OK;
}

uint8_t esp_psram_impl_get_cs_io(void)
{
    ESP_EARLY_LOGI(TAG, "psram CS IO is dedicated");
    return -1;
}

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
 * If ECC is enabled, available PSRAM size will be 7/8 times its physical size.
 */
esp_err_t esp_psram_impl_get_available_size(uint32_t *out_size_bytes)
{
    if (!out_size_bytes) {
        return ESP_ERR_INVALID_ARG;
    }

#if CONFIG_SPIRAM_ECC_ENABLE
    *out_size_bytes = s_psram_size * 7 / 8;
#else
    *out_size_bytes = s_psram_size;
#endif
    return (s_psram_size ? ESP_OK : ESP_ERR_INVALID_STATE);
}
