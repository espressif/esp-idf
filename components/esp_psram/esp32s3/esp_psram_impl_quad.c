/*
 * SPDX-FileCopyrightText: 2013-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"
#include "esp_rom_gpio.h"
#include "esp_rom_efuse.h"
#include "hal/gpio_hal.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/mspi_timing_tuning.h"
#include "esp_private/esp_gpio_reserve.h"

static const char* TAG = "quad_psram";

//Commands for PSRAM chip
#define PSRAM_READ                 0x03
#define PSRAM_FAST_READ            0x0B
#define PSRAM_FAST_READ_QUAD       0xEB
#define PSRAM_WRITE                0x02
#define PSRAM_QUAD_WRITE           0x38
#define PSRAM_ENTER_QMODE          0x35
#define PSRAM_EXIT_QMODE           0xF5
#define PSRAM_RESET_EN             0x66
#define PSRAM_RESET                0x99
#define PSRAM_SET_BURST_LEN        0xC0
#define PSRAM_DEVICE_ID            0x9F

#define PSRAM_FAST_READ_DUMMY      4
#define PSRAM_FAST_READ_QUAD_DUMMY 6

// ID
#define PSRAM_ID_KGD_M          0xff
#define PSRAM_ID_KGD_S             8
#define PSRAM_ID_KGD            0x5d
#define PSRAM_ID_EID_M          0xff
#define PSRAM_ID_EID_S            16

// Use the [7:5](bit7~bit5) of EID to distinguish the psram size:
//
//   BIT7  |  BIT6  |  BIT5  |  SIZE(MBIT)
//   -------------------------------------
//    0    |   0    |   0    |     16
//    0    |   0    |   1    |     32
//    0    |   1    |   0    |     64
#define PSRAM_EID_SIZE_M         0x07
#define PSRAM_EID_SIZE_S            5

#define PSRAM_KGD(id)         (((id) >> PSRAM_ID_KGD_S) & PSRAM_ID_KGD_M)
#define PSRAM_EID(id)         (((id) >> PSRAM_ID_EID_S) & PSRAM_ID_EID_M)
#define PSRAM_SIZE_ID(id)     ((PSRAM_EID(id) >> PSRAM_EID_SIZE_S) & PSRAM_EID_SIZE_M)
#define PSRAM_IS_VALID(id)    (PSRAM_KGD(id) == PSRAM_ID_KGD)

#define PSRAM_IS_64MBIT_TRIAL(id) (PSRAM_EID(id) == 0x26)

// IO-pins for PSRAM.
// WARNING: PSRAM shares all but the CS and CLK pins with the flash, so these defines
// hardcode the flash pins as well, making this code incompatible with either a setup
// that has the flash on non-standard pins or ESP32s with built-in flash.
#define FLASH_CLK_IO          SPI_CLK_GPIO_NUM
#define FLASH_CS_IO           SPI_CS0_GPIO_NUM
// PSRAM clock and cs IO should be configured based on hardware design.
#define PSRAM_CLK_IO          SPI_CLK_GPIO_NUM
#define PSRAM_CS_IO           SPI_CS1_GPIO_NUM
#define PSRAM_SPIQ_SD0_IO     SPI_Q_GPIO_NUM
#define PSRAM_SPID_SD1_IO     SPI_D_GPIO_NUM
#define PSRAM_SPIWP_SD3_IO    SPI_WP_GPIO_NUM
#define PSRAM_SPIHD_SD2_IO    SPI_HD_GPIO_NUM

#define CS_PSRAM_SEL   SPI_MEM_CS1_DIS_M
#define CS_FLASH_SEL   SPI_MEM_CS0_DIS_M

#define SPI1_NUM    1
#define SPI0_NUM    0

typedef enum {
    PSRAM_CMD_QPI,
    PSRAM_CMD_SPI,
} psram_cmd_mode_t;

typedef esp_rom_spi_cmd_t psram_cmd_t;

static uint32_t s_psram_id = 0;
static uint32_t s_psram_size = 0;   //this stands for physical psram size in bytes
static void config_psram_spi_phases(void);
extern void esp_rom_spi_set_op_mode(int spi_num, esp_rom_spiflash_read_mode_t mode);

static uint8_t s_psram_cs_io = (uint8_t) -1;

uint8_t esp_psram_impl_get_cs_io(void)
{
    return s_psram_cs_io;
}

static void psram_set_op_mode(int spi_num, psram_cmd_mode_t mode)
{
    if (mode == PSRAM_CMD_QPI) {
        esp_rom_spi_set_op_mode(spi_num, ESP_ROM_SPIFLASH_QIO_MODE);
        SET_PERI_REG_MASK(SPI_MEM_CTRL_REG(spi_num), SPI_MEM_FCMD_QUAD_M);
    } else if (mode == PSRAM_CMD_SPI) {
        esp_rom_spi_set_op_mode(spi_num, ESP_ROM_SPIFLASH_SLOWRD_MODE);
    }
}
static void _psram_exec_cmd(int spi_num,
                            uint32_t cmd, int cmd_bit_len,
                            uint32_t addr, int addr_bit_len,
                            int dummy_bits,
                            uint8_t* mosi_data, int mosi_bit_len,
                            uint8_t* miso_data, int miso_bit_len)
{
    esp_rom_spi_cmd_t conf;
    uint32_t _addr = addr;
    conf.addr = &_addr;
    conf.addrBitLen = addr_bit_len;
    conf.cmd = cmd;
    conf.cmdBitLen = cmd_bit_len;
    conf.dummyBitLen = dummy_bits; // There is a hardware approach on chip723
    conf.txData = (uint32_t*) mosi_data;
    conf.txDataBitLen = mosi_bit_len;
    conf.rxData = (uint32_t*) miso_data;
    conf.rxDataBitLen = miso_bit_len;
    esp_rom_spi_cmd_config(spi_num, &conf);
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
    uint32_t backup_usr = READ_PERI_REG(SPI_MEM_USER_REG(spi_num));
    uint32_t backup_usr1 = READ_PERI_REG(SPI_MEM_USER1_REG(spi_num));
    uint32_t backup_usr2 = READ_PERI_REG(SPI_MEM_USER2_REG(spi_num));
    uint32_t backup_ctrl = READ_PERI_REG(SPI_MEM_CTRL_REG(spi_num));
    psram_set_op_mode(spi_num, mode);
    _psram_exec_cmd(spi_num, cmd, cmd_bit_len, addr, addr_bit_len,
                    dummy_bits, mosi_data, mosi_bit_len, miso_data, miso_bit_len);
    esp_rom_spi_cmd_start(spi_num, miso_data, miso_bit_len / 8, cs_mask, is_write_erase_operation);

    WRITE_PERI_REG(SPI_MEM_USER_REG(spi_num), backup_usr);
    WRITE_PERI_REG(SPI_MEM_USER1_REG(spi_num), backup_usr1);
    WRITE_PERI_REG(SPI_MEM_USER2_REG(spi_num), backup_usr2);
    WRITE_PERI_REG(SPI_MEM_CTRL_REG(spi_num), backup_ctrl);
}

//exit QPI mode(set back to SPI mode)
static void psram_disable_qio_mode(int spi_num)
{
    psram_exec_cmd(spi_num, PSRAM_CMD_QPI,
                   PSRAM_EXIT_QMODE, 8,              /* command and command bit len*/
                   0, 0,  /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   NULL, 0,                          /* rx data and rx bit len*/
                   CS_PSRAM_SEL,                     /* cs bit mask*/
                   false);                           /* whether is program/erase operation */
}

//TODO IDF-4307
//switch psram burst length(32 bytes or 1024 bytes)
//datasheet says it should be 1024 bytes by default
static void psram_set_wrap_burst_length(int spi_num, psram_cmd_mode_t mode)
{
    psram_exec_cmd(spi_num, mode,
                   PSRAM_SET_BURST_LEN, 8,           /* command and command bit len*/
                   0, 0,  /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   NULL, 0,                          /* rx data and rx bit len*/
                   CS_PSRAM_SEL,                     /* cs bit mask*/
                   false);                           /* whether is program/erase operation */
}

//send reset command to psram, in spi mode
static void psram_reset_mode(int spi_num)
{
    psram_exec_cmd(spi_num, PSRAM_CMD_SPI,
                   PSRAM_RESET_EN, 8,                /* command and command bit len*/
                   0, 0,  /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   NULL, 0,                          /* rx data and rx bit len*/
                   CS_PSRAM_SEL,                     /* cs bit mask*/
                   false);                           /* whether is program/erase operation */

    psram_exec_cmd(spi_num, PSRAM_CMD_SPI,
                   PSRAM_RESET, 8,                   /* command and command bit len*/
                   0, 0,  /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   NULL, 0,                          /* rx data and rx bit len*/
                   CS_PSRAM_SEL,                     /* cs bit mask*/
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
        psram_set_wrap_burst_length(1, PSRAM_CMD_QPI);
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
static void psram_read_id(int spi_num, uint32_t* dev_id)
{
    psram_exec_cmd(spi_num, PSRAM_CMD_SPI,
                   PSRAM_DEVICE_ID, 8,               /* command and command bit len*/
                   0, 24,                            /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   (uint8_t*) dev_id, 24,            /* rx data and rx bit len*/
                   CS_PSRAM_SEL,                     /* cs bit mask*/
                   false);                           /* whether is program/erase operation */
}

//enter QPI mode
static void psram_enable_qio_mode(int spi_num)
{
    psram_exec_cmd(spi_num, PSRAM_CMD_SPI,
                   PSRAM_ENTER_QMODE, 8,             /* command and command bit len*/
                   0, 0,  /* address and address bit len*/
                   0,                                /* dummy bit len */
                   NULL, 0,                          /* tx data and tx bit len*/
                   NULL, 0,                          /* rx data and rx bit len*/
                   CS_PSRAM_SEL,                     /* cs bit mask*/
                   false);                           /* whether is program/erase operation */
}

static void psram_set_cs_timing(void)
{
    //SPI0/1 share the cs_hold / cs_setup, cd_hold_time / cd_setup_time registers for PSRAM, so we only need to set SPI0 related registers here
    SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_HOLD_TIME_V, 0, SPI_MEM_SPI_SMEM_CS_HOLD_TIME_S);
    SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_SETUP_TIME_V, 0, SPI_MEM_SPI_SMEM_CS_SETUP_TIME_S);
    SET_PERI_REG_MASK(SPI_MEM_SPI_SMEM_AC_REG(0), SPI_MEM_SPI_SMEM_CS_HOLD_M | SPI_MEM_SPI_SMEM_CS_SETUP_M);
}

static void psram_gpio_config(void)
{
    //CS1
    uint8_t cs1_io = PSRAM_CS_IO;
    if (cs1_io == SPI_CS1_GPIO_NUM) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[cs1_io],  FUNC_SPICS1_SPICS1);
    } else {
        esp_rom_gpio_connect_out_signal(cs1_io, SPICS1_OUT_IDX, 0, 0);
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[cs1_io],  PIN_FUNC_GPIO);
    }
    s_psram_cs_io = cs1_io;

    //WP HD
    uint8_t wp_io = PSRAM_SPIWP_SD3_IO;
    const uint32_t spiconfig = esp_rom_efuse_get_flash_gpio_info();
    if (spiconfig == ESP_ROM_EFUSE_FLASH_DEFAULT_SPI) {
        // MSPI pins (except wp / hd) are all configured via IO_MUX in 1st bootloader.
    } else {
        // MSPI pins (except wp / hd) are all configured via GPIO matrix in 1st bootloader.
        wp_io = esp_rom_efuse_get_flash_wp_gpio();
    }
    //This ROM function will init both WP and HD pins.
    esp_rom_spiflash_select_qio_pins(wp_io, spiconfig);

    // Reserve psram pins
    esp_gpio_reserve_pins(BIT64(cs1_io) | BIT64(wp_io));
}

esp_err_t esp_psram_impl_enable(void)   //psram init
{
    psram_gpio_config();
    psram_set_cs_timing();

    //enter MSPI slow mode to init PSRAM device registers
    mspi_timing_enter_low_speed_mode(true);

    //We use SPI1 to init PSRAM
    psram_disable_qio_mode(SPI1_NUM);
    psram_read_id(SPI1_NUM, &s_psram_id);
    if (!PSRAM_IS_VALID(s_psram_id)) {
        /* 16Mbit psram ID read error workaround:
         * treat the first read id as a dummy one as the pre-condition,
         * Send Read ID command again
         */
        psram_read_id(SPI1_NUM, &s_psram_id);
        if (!PSRAM_IS_VALID(s_psram_id)) {
            ESP_EARLY_LOGE(TAG, "PSRAM ID read error: 0x%08x, PSRAM chip not found or not supported, or wrong PSRAM line mode", (uint32_t)s_psram_id);
            return ESP_ERR_NOT_SUPPORTED;
        }
    }

    if (PSRAM_IS_64MBIT_TRIAL(s_psram_id)) {
        s_psram_size = PSRAM_SIZE_8MB;
    } else {
        uint8_t density = PSRAM_SIZE_ID(s_psram_id);
        s_psram_size = density == 0x0 ? PSRAM_SIZE_2MB :
                       density == 0x1 ? PSRAM_SIZE_4MB :
                       density == 0x2 ? PSRAM_SIZE_8MB : 0;
    }

    //SPI1: send psram reset command
    psram_reset_mode(SPI1_NUM);
    //SPI1: send QPI enable command
    psram_enable_qio_mode(SPI1_NUM);

    //Do PSRAM timing tuning, we use SPI1 to do the tuning, and set the SPI0 PSRAM timing related registers accordingly
    mspi_timing_psram_tuning();

    //Configure SPI0 PSRAM related SPI Phases
    config_psram_spi_phases();
    //Back to the high speed mode. Flash/PSRAM clocks are set to the clock that user selected. SPI0/1 registers are all set correctly
    mspi_timing_enter_high_speed_mode(true);

    return ESP_OK;
}

//Configure PSRAM SPI0 phase related registers here according to the PSRAM chip requirement
static void config_psram_spi_phases(void)
{
    //Config CMD phase
    CLEAR_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_USR_SRAM_DIO_M);       //disable dio mode for cache command
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_USR_SRAM_QIO_M);         //enable qio mode for cache command
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_CACHE_SRAM_USR_RCMD_M);  //enable cache read command
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_CACHE_SRAM_USR_WCMD_M);  //enable cache write command
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN, 7, SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE, PSRAM_QUAD_WRITE, SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 7, SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V, PSRAM_FAST_READ_QUAD, SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0xEB

    //Config ADDR phase
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_SRAM_ADDR_BITLEN_V, 23, SPI_MEM_SRAM_ADDR_BITLEN_S);

    //Dummy
    /**
     * We set the PSRAM chip required dummy here. If timing tuning is needed,
     * the dummy length will be updated in `mspi_timing_enter_high_speed_mode()`
     */
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_USR_RD_SRAM_DUMMY_M);    //enable cache read dummy
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_SRAM_RDUMMY_CYCLELEN_V, (PSRAM_FAST_READ_QUAD_DUMMY - 1), SPI_MEM_SRAM_RDUMMY_CYCLELEN_S); //dummy

    CLEAR_PERI_REG_MASK(SPI_MEM_MISC_REG(0), SPI_MEM_CS1_DIS_M); //ENABLE SPI0 CS1 TO PSRAM(CS0--FLASH; CS1--SRAM)
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
 *
 * When ECC is enabled, the available size will be reduced.
 * On S3 Quad PSRAM, ECC is not enabled for now.
 */
esp_err_t esp_psram_impl_get_available_size(uint32_t *out_size_bytes)
{
    if (!out_size_bytes) {
        return ESP_ERR_INVALID_ARG;
    }

    *out_size_bytes = s_psram_size;
    return (s_psram_size ? ESP_OK : ESP_ERR_INVALID_STATE);
}
