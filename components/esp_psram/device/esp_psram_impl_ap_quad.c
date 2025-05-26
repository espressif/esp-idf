/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_private/esp_psram_impl.h"
#include "rom/spi_flash.h"
#include "rom/opi_flash.h"
#include "esp_rom_gpio.h"
#include "esp_rom_efuse.h"
#include "hal/gpio_hal.h"
#include "esp_private/mspi_timing_tuning.h"
#include "esp_private/esp_gpio_reserve.h"
#include "hal/psram_ctrlr_ll.h"
#include "esp_quad_psram_defs_ap.h"
#include "soc/soc_caps.h"

static const char* TAG = "quad_psram";

static uint32_t s_psram_size = 0;   //this stands for physical psram size in bytes
static void config_psram_spi_phases(void);

static uint8_t s_psram_cs_io = (uint8_t) -1;

uint8_t esp_psram_impl_get_cs_io(void)
{
    return s_psram_cs_io;
}

void psram_exec_cmd(int spi_num, psram_cmd_mode_t mode,
                    uint32_t cmd, int cmd_bit_len,
                    uint32_t addr, int addr_bit_len,
                    int dummy_bits,
                    uint8_t* mosi_data, int mosi_bit_len,
                    uint8_t* miso_data, int miso_bit_len,
                    uint32_t cs_mask,
                    bool is_write_erase_operation)
{
    esp_rom_spiflash_read_mode_t rd_mode = (mode == PSRAM_HAL_CMD_QPI) ? ESP_ROM_SPIFLASH_QIO_MODE : ESP_ROM_SPIFLASH_SLOWRD_MODE;

    esp_rom_spi_set_op_mode(spi_num, rd_mode);
    if (mode == PSRAM_HAL_CMD_QPI) {
        psram_ctrlr_ll_enable_quad_command(PSRAM_CTRLR_LL_MSPI_ID_1, true);
    }
    psram_ctrlr_ll_common_transaction_base(spi_num, rd_mode,
                                           cmd, cmd_bit_len,
                                           addr, addr_bit_len,
                                           dummy_bits,
                                           mosi_data, mosi_bit_len,
                                           miso_data, miso_bit_len,
                                           cs_mask,
                                           is_write_erase_operation);
}

//exit QPI mode(set back to SPI mode)
static void psram_disable_qio_mode(int spi_num)
{
    psram_exec_cmd(spi_num, PSRAM_HAL_CMD_QPI,
                   PSRAM_QUAD_EXIT_QMODE, 8,              /* command and command bit len*/
                   0, 0,                             /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   NULL, 0,                          /* rx data and rx bit len*/
                   PSRAM_LL_CS_SEL,                     /* cs bit mask*/
                   false);                           /* whether is program/erase operation */
}

//TODO IDF-4307
//switch psram burst length(32 bytes or 1024 bytes)
//datasheet says it should be 1024 bytes by default
static void psram_set_wrap_burst_length(int spi_num, psram_cmd_mode_t mode)
{
    psram_exec_cmd(spi_num, mode,
                   PSRAM_QUAD_SET_BURST_LEN, 8,           /* command and command bit len*/
                   0, 0,                             /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   NULL, 0,                          /* rx data and rx bit len*/
                   PSRAM_LL_CS_SEL,                     /* cs bit mask*/
                   false);                           /* whether is program/erase operation */
}

//send reset command to psram, in spi mode
static void psram_reset_mode(int spi_num)
{
    psram_exec_cmd(spi_num, PSRAM_HAL_CMD_SPI,
                   PSRAM_QUAD_RESET_EN, 8,                /* command and command bit len*/
                   0, 0,                             /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   NULL, 0,                          /* rx data and rx bit len*/
                   PSRAM_LL_CS_SEL,                     /* cs bit mask*/
                   false);                           /* whether is program/erase operation */

    psram_exec_cmd(spi_num, PSRAM_HAL_CMD_SPI,
                   PSRAM_QUAD_RESET, 8,                   /* command and command bit len*/
                   0, 0,                             /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   NULL, 0,                          /* rx data and rx bit len*/
                   PSRAM_LL_CS_SEL,                     /* cs bit mask*/
                   false);                           /* whether is program/erase operation */
}

esp_err_t psram_enable_wrap(uint32_t wrap_size)
{
    //TODO: IDF-4307
    static uint32_t current_wrap_size = 0;
    if (current_wrap_size == wrap_size) {
        return ESP_OK;
    }
    switch (wrap_size) {
    case 32:
    case 0:
        psram_set_wrap_burst_length(1, PSRAM_HAL_CMD_QPI);
        current_wrap_size = wrap_size;
        return ESP_OK;
    case 16:
    case 64:
    default:
        return ESP_FAIL;
    }
}

bool psram_support_wrap_size(uint32_t wrap_size)
{
    switch (wrap_size) {
    case 0:
    case 32:
        return true;
    case 16:
    case 64:
    default:
        return false;
    }

}

//Read ID operation only supports SPI CMD and mode, should issue `psram_disable_qio_mode` before calling this
static void psram_read_id(int spi_num, uint8_t* dev_id, int id_bits)
{
    psram_exec_cmd(spi_num, PSRAM_HAL_CMD_SPI,
                   PSRAM_QUAD_DEVICE_ID, 8,               /* command and command bit len*/
                   0, 24,                            /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   dev_id, id_bits,                  /* rx data and rx bit len*/
                   PSRAM_LL_CS_SEL,                  /* cs bit mask*/
                   false);                           /* whether is program/erase operation */
}

//enter QPI mode
static void psram_enable_qio_mode(int spi_num)
{
    psram_exec_cmd(spi_num, PSRAM_HAL_CMD_SPI,
                   PSRAM_QUAD_ENTER_QMODE, 8,             /* command and command bit len*/
                   0, 0,                             /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   NULL, 0,                          /* rx data and rx bit len*/
                   PSRAM_LL_CS_SEL,                     /* cs bit mask*/
                   false);                           /* whether is program/erase operation */
}

static void psram_set_cs_timing(void)
{
    psram_ctrlr_ll_set_cs_hold(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_QUAD_CS_HOLD_VAL);
    psram_ctrlr_ll_set_cs_setup(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_QUAD_CS_SETUP_VAL);
#if CONFIG_SPIRAM_ECC_ENABLE
    psram_ctrlr_ll_set_ecc_cs_hold(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_QUAD_CS_ECC_HOLD_TIME_VAL);
#endif
}

#if CONFIG_SPIRAM_ECC_ENABLE
static void s_mspi_ecc_show_info(void)
{
    for (int i = 0; i < PSRAM_CTRLR_LL_PMS_REGION_NUMS; i++) {
        ESP_EARLY_LOGV(TAG, "region[%d] addr: 0x%08x", i, psram_ctrlr_ll_get_pms_region_start_addr(PSRAM_CTRLR_LL_MSPI_ID_0, i));
        ESP_EARLY_LOGV(TAG, "region[%d] size: 0x%08x", i, psram_ctrlr_ll_get_pms_region_size(PSRAM_CTRLR_LL_MSPI_ID_0, i));
    }

    uint32_t page_size = psram_ctrlr_ll_get_page_size(PSRAM_CTRLR_LL_MSPI_ID_0);
    ESP_EARLY_LOGV(TAG, "ECC page size: %d", page_size);
}

/**
 * Enable error correcting code feature
 *
 * Can add an input parameter for selecting ECC mode if needed
 */
static void s_configure_psram_ecc(void)
{
    psram_ctrlr_ll_set_ecc_mode(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_LL_ECC_MODE_16TO18);
    psram_ctrlr_ll_enable_skip_page_corner(PSRAM_CTRLR_LL_MSPI_ID_0, true);
    psram_ctrlr_ll_enable_split_trans(PSRAM_CTRLR_LL_MSPI_ID_0, true);
    psram_ctrlr_ll_set_page_size(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_QUAD_PAGE_SIZE);
    psram_ctrlr_ll_enable_ecc_addr_conversion(PSRAM_CTRLR_LL_MSPI_ID_0, true);

    /**
     * Enable ECC region 0 (ACE0)
     * Default: ACE0 range: 0 ~ 256MB
     * For current Quad PSRAM, ACE0 is enough
     */
    psram_ctrlr_ll_set_pms_region_start_addr(PSRAM_CTRLR_LL_MSPI_ID_0, 0, 0);
    psram_ctrlr_ll_set_pms_region_size(PSRAM_CTRLR_LL_MSPI_ID_0, 0, 4096);
    psram_ctrlr_ll_set_pms_region_attr(PSRAM_CTRLR_LL_MSPI_ID_0, 0, PSRAM_CTRLR_LL_PMS_ATTR_WRITABLE | PSRAM_CTRLR_LL_PMS_ATTR_READABLE);
    psram_ctrlr_ll_enable_pms_region_ecc(PSRAM_CTRLR_LL_MSPI_ID_0, 0, true);
    ESP_EARLY_LOGI(TAG, "ECC is enabled");
    s_mspi_ecc_show_info();
}
#endif

static void psram_gpio_config(void)
{
    //CS1
    uint8_t cs1_io = PSRAM_QUAD_CS_IO;
    if (cs1_io == MSPI_IOMUX_PIN_NUM_CS1) {
        gpio_ll_func_sel(&GPIO, cs1_io, FUNC_SPICS1_SPICS1);
    } else {
        esp_rom_gpio_connect_out_signal(cs1_io, FSPICS1_OUT_IDX, 0, 0);
        gpio_ll_func_sel(&GPIO, cs1_io, PIN_FUNC_GPIO);
    }
    s_psram_cs_io = cs1_io;

    //WP HD
    uint8_t wp_io = PSRAM_QUAD_SPIWP_SD3_IO;
#if SOC_SPI_MEM_SUPPORT_CONFIG_GPIO_BY_EFUSE
    const uint32_t spiconfig = esp_rom_efuse_get_flash_gpio_info();
    if (spiconfig == ESP_ROM_EFUSE_FLASH_DEFAULT_SPI) {
        // MSPI pins (except wp / hd) are all configured via IO_MUX in 1st bootloader.
    } else {
        // MSPI pins (except wp / hd) are all configured via GPIO matrix in 1st bootloader.
        wp_io = esp_rom_efuse_get_flash_wp_gpio();
    }
    esp_rom_spiflash_select_qio_pins(wp_io, spiconfig);
#else
    //This ROM function will init both WP and HD pins.
    esp_rom_spiflash_select_qio_pins(wp_io, 0);
#endif

    // Reserve psram pins
    esp_gpio_reserve(BIT64(cs1_io) | BIT64(wp_io));
}

#if !SOC_SPI_MEM_SUPPORT_TIMING_TUNING
static void s_config_psram_clock(bool init_state)
{
    uint32_t clock_conf = 0;
    if (init_state) {
        clock_conf = psram_ctrlr_ll_calculate_clock_reg(4);
        psram_ctrlr_ll_set_spi1_bus_clock(PSRAM_CTRLR_LL_MSPI_ID_1, clock_conf);
    } else {
        // This function can be extended if we have other psram frequency

#if (CONFIG_SPIRAM_SPEED == 80)
        clock_conf = psram_ctrlr_ll_calculate_clock_reg(1);
#elif (CONFIG_SPIRAM_SPEED == 40)
        clock_conf = psram_ctrlr_ll_calculate_clock_reg(2);
#endif
        psram_ctrlr_ll_set_bus_clock(PSRAM_CTRLR_LL_MSPI_ID_0, clock_conf);
    }
}
#endif  //#if !SOC_SPI_MEM_SUPPORT_TIMING_TUNING

/**
 * For mrj069000aa, this wafer version and 8MB case, we consider it as 4MB mode as it uses 2T mode
 */
static bool s_check_mrj069000aa_2tmode(uint32_t eid_47_16)
{
    bool is_2t = false;
    ESP_EARLY_LOGD(TAG, "(eid_47_16 >> 5) & 0xfffff: 0x%"PRIx32, (eid_47_16 >> 5) & 0xfffff);
    if (((eid_47_16 >> 5) & 0xfffff) == 0x8a445) {
        is_2t = true;
    }

    return is_2t;
}

static bool s_check_2tmode(void)
{
    uint64_t full_eid = 0;
    psram_read_id(PSRAM_CTRLR_LL_MSPI_ID_1, (uint8_t *)&full_eid, PSRAM_QUAD_EID_BITS_NUM);

    bool is_2t = false;
    uint32_t eid_47_16 = __builtin_bswap32((full_eid >> 16) & UINT32_MAX);
    ESP_EARLY_LOGD(TAG, "full_eid: 0x%" PRIx64", eid_47_16: 0x%"PRIx32", (eid_47_16 >> 25) & 0x1: 0x%"PRIx32, full_eid, eid_47_16, (eid_47_16 >> 25) & 0x1);
    //EID[41]: 0 for 2t mode; 1 for non-2t mode
    if (((eid_47_16 >> 25) & 0x1) == 0) {
        is_2t = true;
    }

    if (s_check_mrj069000aa_2tmode(eid_47_16)) {
        is_2t = true;
    }

    return is_2t;
}

esp_err_t esp_psram_impl_enable(void)
{
    psram_gpio_config();
    psram_set_cs_timing();
#if CONFIG_SPIRAM_ECC_ENABLE
    s_configure_psram_ecc();
#endif

#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
    //enter MSPI slow mode to init PSRAM device registers
    mspi_timing_enter_low_speed_mode(true);
#else
    s_config_psram_clock(true);
#endif // SOC_SPI_MEM_SUPPORT_TIMING_TUNING

    uint32_t psram_id = 0;

    //We use SPI1 to init PSRAM
    psram_disable_qio_mode(PSRAM_CTRLR_LL_MSPI_ID_1);
    psram_read_id(PSRAM_CTRLR_LL_MSPI_ID_1, (uint8_t *)&psram_id, PSRAM_QUAD_ID_BITS_NUM);
    if (!PSRAM_QUAD_IS_VALID(psram_id)) {
        /* 16Mbit psram ID read error workaround:
         * treat the first read id as a dummy one as the pre-condition,
         * Send Read ID command again
         */
        psram_read_id(PSRAM_CTRLR_LL_MSPI_ID_1, (uint8_t *)&psram_id, PSRAM_QUAD_ID_BITS_NUM);
        if (!PSRAM_QUAD_IS_VALID(psram_id)) {
            ESP_EARLY_LOGE(TAG, "PSRAM ID read error: 0x%08x, PSRAM chip not found or not supported, or wrong PSRAM line mode", (uint32_t)psram_id);
            return ESP_ERR_NOT_SUPPORTED;
        }
    }
    ESP_EARLY_LOGV(TAG, "MFID: 0x%x", PSRAM_QUAD_MFID(psram_id));

    if (PSRAM_QUAD_IS_64MBIT_TRIAL(psram_id)) {
        s_psram_size = PSRAM_SIZE_8MB;
    } else {
        uint8_t density = PSRAM_QUAD_SIZE_ID(psram_id);
        const int eid = PSRAM_QUAD_EID_BIT_47_40(psram_id);
        s_psram_size = density == 0x0 ? PSRAM_SIZE_2MB :
                       density == 0x1 ? PSRAM_SIZE_4MB :
                       density == 0x2 ? PSRAM_SIZE_8MB :
                       /* Do not use `density` for QEMU PSRAM since we don't want any future QSPI PSRAM
                        * that are 16MB or 32MB to be interpreted as QEMU PSRAM devices */
                       eid == PSRAM_QUAD_QEMU_16MB_ID ? PSRAM_SIZE_16MB :
                       eid == PSRAM_QUAD_QEMU_32MB_ID ? PSRAM_SIZE_32MB : 0;

        if (PSRAM_QUAD_MFID(psram_id) == PSRAM_QUAD_MFID_AP && (s_psram_size == PSRAM_SIZE_8MB) && s_check_2tmode()) {
            //2t mode is only valid for EID[47:45] == 0x10 chips
            s_psram_size = s_psram_size / 2;
        }
    }

    //SPI1: send psram reset command
    psram_reset_mode(PSRAM_CTRLR_LL_MSPI_ID_1);
    //SPI1: send QPI enable command
    psram_enable_qio_mode(PSRAM_CTRLR_LL_MSPI_ID_1);

#if SOC_SPI_MEM_SUPPORT_TIMING_TUNING
    //Do PSRAM timing tuning, we use SPI1 to do the tuning, and set the SPI0 PSRAM timing related registers accordingly
    mspi_timing_psram_tuning();
    //Configure SPI0 PSRAM related SPI Phases
    config_psram_spi_phases();
    //Back to the high speed mode. Flash/PSRAM clocks are set to the clock that user selected. SPI0/1 registers are all set correctly
    mspi_timing_enter_high_speed_mode(true);
#else
    s_config_psram_clock(false);
    //Configure SPI0 PSRAM related SPI Phases
    config_psram_spi_phases();
#endif

    return ESP_OK;
}

//Configure PSRAM SPI0 phase related registers here according to the PSRAM chip requirement
static void config_psram_spi_phases(void)
{
    psram_ctrlr_ll_set_read_mode(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_HAL_CMD_QPI);
    psram_ctrlr_ll_set_wr_cmd(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_QUAD_CMD_LENGTH, PSRAM_QUAD_WRITE_QUAD);
    psram_ctrlr_ll_set_rd_cmd(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_QUAD_CMD_LENGTH, PSRAM_QUAD_FAST_READ_QUAD);
    psram_ctrlr_ll_set_addr_bitlen(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_QUAD_ADDR_LENGTH);
    psram_ctrlr_ll_set_rd_dummy(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_QUAD_FAST_READ_QUAD_DUMMY);
    psram_ctrlr_ll_set_cs_pin(PSRAM_CTRLR_LL_MSPI_ID_0, PSRAM_LL_CS_ID_1);
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
