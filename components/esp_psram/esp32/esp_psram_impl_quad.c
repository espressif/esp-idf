/*
 * SPDX-FileCopyrightText: 2013-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Driver bits for PSRAM chips (at the moment only the ESP-PSRAM32 chip).
*/

#include "sdkconfig.h"
#include "string.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_bit_defs.h"
#include "esp_log.h"
#include "../esp_psram_impl.h"
#include "esp32/rom/spi_flash.h"
#include "esp32/rom/cache.h"
#include "rom/efuse.h"
#include "esp_rom_efuse.h"
#include "soc/dport_reg.h"
#include "soc/efuse_periph.h"
#include "soc/soc_caps.h"
#include "soc/spi_periph.h"
#include "soc/chip_revision.h"
#include "driver/gpio.h"
#include "hal/efuse_hal.h"
#include "hal/gpio_hal.h"
#include "esp_private/spi_common_internal.h"
#include "esp_private/periph_ctrl.h"
#include "bootloader_common.h"
#include "esp_rom_gpio.h"
#include "bootloader_flash_config.h"
#include "esp_private/esp_gpio_reserve.h"

#if CONFIG_SPIRAM
#include "soc/rtc.h"

//Commands for PSRAM chip
#define PSRAM_READ                 0x03
#define PSRAM_FAST_READ            0x0B
#define PSRAM_FAST_READ_DUMMY      0x3
#define PSRAM_FAST_READ_QUAD       0xEB
#define PSRAM_FAST_READ_QUAD_DUMMY 0x5
#define PSRAM_WRITE                0x02
#define PSRAM_QUAD_WRITE           0x38
#define PSRAM_ENTER_QMODE          0x35
#define PSRAM_EXIT_QMODE           0xF5
#define PSRAM_RESET_EN             0x66
#define PSRAM_RESET                0x99
#define PSRAM_SET_BURST_LEN        0xC0
#define PSRAM_DEVICE_ID            0x9F

typedef enum {
    PSRAM_CLK_MODE_NORM = 0,    /*!< Normal SPI mode */
    PSRAM_CLK_MODE_DCLK = 1,    /*!< Two extra clock cycles after CS is set high level */
} psram_clk_mode_t;

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

typedef enum {
    PSRAM_EID_SIZE_16MBITS = 0,
    PSRAM_EID_SIZE_32MBITS = 1,
    PSRAM_EID_SIZE_64MBITS = 2,
} psram_eid_size_t;

#define PSRAM_KGD(id)         (((id) >> PSRAM_ID_KGD_S) & PSRAM_ID_KGD_M)
#define PSRAM_EID(id)         (((id) >> PSRAM_ID_EID_S) & PSRAM_ID_EID_M)
#define PSRAM_SIZE_ID(id)     ((PSRAM_EID(id) >> PSRAM_EID_SIZE_S) & PSRAM_EID_SIZE_M)
#define PSRAM_IS_VALID(id)    (PSRAM_KGD(id) == PSRAM_ID_KGD)

// For the old version 32Mbit psram, using the spicial driver */
#define PSRAM_IS_32MBIT_VER0(id)  (PSRAM_EID(id) == 0x20)
#define PSRAM_IS_64MBIT_TRIAL(id) (PSRAM_EID(id) == 0x26)

// IO-pins for PSRAM.
// WARNING: PSRAM shares all but the CS and CLK pins with the flash, so these defines
// hardcode the flash pins as well, making this code incompatible with either a setup
// that has the flash on non-standard pins or ESP32s with built-in flash.
#define PSRAM_SPIQ_SD0_IO          MSPI_IOMUX_PIN_NUM_MISO
#define PSRAM_SPID_SD1_IO          MSPI_IOMUX_PIN_NUM_MOSI
#define PSRAM_SPIHD_SD2_IO         MSPI_IOMUX_PIN_NUM_HD
#define PSRAM_SPIWP_SD3_IO         MSPI_IOMUX_PIN_NUM_WP

// HSPI Pins
#define FLASH_HSPI_CLK_IO          14
#define FLASH_HSPI_CS_IO           15
#define PSRAM_HSPI_SPIQ_SD0_IO     12
#define PSRAM_HSPI_SPID_SD1_IO     13
#define PSRAM_HSPI_SPIWP_SD3_IO    2
#define PSRAM_HSPI_SPIHD_SD2_IO    4

// PSRAM clock and cs IO should be configured based on hardware design.
// For ESP32-WROVER or ESP32-WROVER-B module, the clock IO is IO17, the cs IO is IO16,
// they are the default value for these two configs.
#define D0WD_PSRAM_CLK_IO          CONFIG_D0WD_PSRAM_CLK_IO  // Default value is 17
#define D0WD_PSRAM_CS_IO           CONFIG_D0WD_PSRAM_CS_IO   // Default value is 16

#define D2WD_PSRAM_CLK_IO          CONFIG_D2WD_PSRAM_CLK_IO  // Default value is 9
#define D2WD_PSRAM_CS_IO           CONFIG_D2WD_PSRAM_CS_IO   // Default value is 10

// There is no reason to change the pin of an embedded psram.
// So define the number of pin directly, instead of configurable.
#define D0WDR2_V3_PSRAM_CLK_IO    6
#define D0WDR2_V3_PSRAM_CS_IO     16

// For ESP32-PICO chip, the psram share clock with flash. The flash clock pin is fixed, which is IO6.
#define PICO_PSRAM_CLK_IO          6
#define PICO_PSRAM_CS_IO           CONFIG_PICO_PSRAM_CS_IO   // Default value is 10

#define PICO_V3_02_PSRAM_CLK_IO    10
#define PICO_V3_02_PSRAM_CS_IO     9

typedef enum {
    PSRAM_CACHE_F80M_S40M = 0,
    PSRAM_CACHE_F40M_S40M,
    PSRAM_CACHE_F80M_S80M,
    PSRAM_CACHE_MAX,
} psram_cache_speed_t;

#if CONFIG_SPIRAM_SPEED_40M && CONFIG_ESPTOOLPY_FLASHFREQ_40M
#define PSRAM_SPEED           PSRAM_CACHE_F40M_S40M
#define PSRAM_CS_HOLD_TIME    0
#elif CONFIG_SPIRAM_SPEED_40M && CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define PSRAM_SPEED           PSRAM_CACHE_F80M_S40M
#define PSRAM_CS_HOLD_TIME    0
#elif CONFIG_SPIRAM_SPEED_80M && CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define PSRAM_SPEED           PSRAM_CACHE_F80M_S80M
#define PSRAM_CS_HOLD_TIME    1
#else
#error "FLASH speed can only be equal to or higher than SRAM speed while SRAM is enabled!"
#endif

typedef struct {
    uint8_t flash_clk_io;
    uint8_t flash_cs_io;
    uint8_t psram_clk_io;
    uint8_t psram_cs_io;
    uint8_t psram_spiq_sd0_io;
    uint8_t psram_spid_sd1_io;
    uint8_t psram_spiwp_sd3_io;
    uint8_t psram_spihd_sd2_io;
} psram_io_t;

#define PSRAM_INTERNAL_IO_28       28
#define PSRAM_INTERNAL_IO_29       29
#define PSRAM_IO_MATRIX_DUMMY_40M  ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_40M
#define PSRAM_IO_MATRIX_DUMMY_80M  ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_80M

#define _SPI_CACHE_PORT             0
#define _SPI_FLASH_PORT             1
#define _SPI_80M_CLK_DIV            1
#define _SPI_40M_CLK_DIV            2

//For 4MB PSRAM, we need one more SPI host, select which one to use by kconfig
#ifdef CONFIG_SPIRAM_OCCUPY_HSPI_HOST
#define PSRAM_SPI_MODULE    PERIPH_HSPI_MODULE
#define PSRAM_SPI_HOST      HSPI_HOST
#define PSRAM_CLK_SIGNAL    HSPICLK_OUT_IDX
#define PSRAM_SPI_NUM       PSRAM_SPI_2
#define PSRAM_SPICLKEN      DPORT_SPI2_CLK_EN
#elif defined CONFIG_SPIRAM_OCCUPY_VSPI_HOST
#define PSRAM_SPI_MODULE    PERIPH_VSPI_MODULE
#define PSRAM_SPI_HOST      VSPI_HOST
#define PSRAM_CLK_SIGNAL    VSPICLK_OUT_IDX
#define PSRAM_SPI_NUM       PSRAM_SPI_3
#define PSRAM_SPICLKEN      DPORT_SPI3_CLK_EN
#else   //set to SPI avoid HSPI and VSPI being used
#define PSRAM_SPI_MODULE    PERIPH_SPI_MODULE
#define PSRAM_SPI_HOST      SPI_HOST
#define PSRAM_CLK_SIGNAL    SPICLK_OUT_IDX
#define PSRAM_SPI_NUM       PSRAM_SPI_1
#define PSRAM_SPICLKEN      DPORT_SPI01_CLK_EN
#endif

/*
See the TRM, chapter PID/MPU/MMU, header 'External RAM' for the definitions of these modes.

Important is that NORMAL works with the app CPU cache disabled, but gives huge cache coherency
issues when both app and pro CPU are enabled. LOWHIGH and EVENODD do not have these coherency
issues but cannot be used when the app CPU cache is disabled.
*/
typedef enum {
    PSRAM_VADDR_MODE_NORMAL = 0, ///< App and pro CPU use their own flash cache for external RAM access
    PSRAM_VADDR_MODE_LOWHIGH,    ///< App and pro CPU share external RAM caches: pro CPU has low 2M, app CPU has high 2M
    PSRAM_VADDR_MODE_EVENODD,    ///< App and pro CPU share external RAM caches: pro CPU does even 32yte ranges, app does odd ones.
} psram_vaddr_mode_t;

#if CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
#define PSRAM_MODE PSRAM_VADDR_MODE_NORMAL
#else
#define PSRAM_MODE PSRAM_VADDR_MODE_LOWHIGH
#endif

static const char *TAG = "quad_psram";
typedef enum {
    PSRAM_SPI_1  = 0x1,
    PSRAM_SPI_2,
    PSRAM_SPI_3,
    PSRAM_SPI_MAX,
} psram_spi_num_t;

static psram_cache_speed_t s_psram_mode = PSRAM_CACHE_MAX;
static psram_clk_mode_t s_clk_mode = PSRAM_CLK_MODE_DCLK;
static uint64_t s_psram_id = 0;
static bool s_2t_mode_enabled = false;

/* dummy_len_plus values defined in ROM for SPI flash configuration */
extern uint8_t g_rom_spiflash_dummy_len_plus[];
static int extra_dummy = 0;
typedef enum {
    PSRAM_CMD_QPI,
    PSRAM_CMD_SPI,
} psram_cmd_mode_t;

typedef struct {
    uint16_t cmd;                /*!< Command value */
    uint16_t cmdBitLen;          /*!< Command byte length*/
    uint32_t *addr;              /*!< Point to address value*/
    uint16_t addrBitLen;         /*!< Address byte length*/
    uint32_t *txData;            /*!< Point to send data buffer*/
    uint16_t txDataBitLen;       /*!< Send data byte length.*/
    uint32_t *rxData;            /*!< Point to receive data buffer*/
    uint16_t rxDataBitLen;       /*!< Receive Data byte length.*/
    uint32_t dummyBitLen;
} psram_cmd_t;

static void psram_cache_init(psram_cache_speed_t psram_cache_mode, psram_vaddr_mode_t vaddrmode);

static uint8_t s_psram_cs_io = (uint8_t) -1;

uint8_t esp_psram_impl_get_cs_io(void)
{
    return s_psram_cs_io;
}

static void psram_clear_spi_fifo(psram_spi_num_t spi_num)
{
    int i;
    for (i = 0; i < 16; i++) {
        WRITE_PERI_REG(SPI_W0_REG(spi_num) + i * 4, 0);
    }
}

//set basic SPI write mode
static void psram_set_basic_write_mode(psram_spi_num_t spi_num)
{
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_QIO);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_DIO);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_QUAD);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_DUAL);
}
//set QPI write mode
static void psram_set_qio_write_mode(psram_spi_num_t spi_num)
{
    SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_QIO);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_DIO);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_QUAD);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_FWRITE_DUAL);
}
//set QPI read mode
static void psram_set_qio_read_mode(psram_spi_num_t spi_num)
{
    SET_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_QIO);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_QUAD);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_DUAL);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_DIO);
}
//set SPI read mode
static void psram_set_basic_read_mode(psram_spi_num_t spi_num)
{
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_QIO);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_QUAD);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_DUAL);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_FREAD_DIO);
}

//start sending cmd/addr and optionally, receiving data
static void IRAM_ATTR psram_cmd_recv_start(psram_spi_num_t spi_num, uint32_t *pRxData, uint16_t rxByteLen,
                                           psram_cmd_mode_t cmd_mode)
{
    //get cs1
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS1_DIS_M);
    SET_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS0_DIS_M);

    uint32_t mode_backup = (READ_PERI_REG(SPI_USER_REG(spi_num)) >> SPI_FWRITE_DUAL_S) & 0xf;
    uint32_t rd_mode_backup = READ_PERI_REG(SPI_CTRL_REG(spi_num)) & (SPI_FREAD_DIO_M | SPI_FREAD_DUAL_M | SPI_FREAD_QUAD_M | SPI_FREAD_QIO_M);
    if (cmd_mode == PSRAM_CMD_SPI) {
        psram_set_basic_write_mode(spi_num);
        psram_set_basic_read_mode(spi_num);
    } else if (cmd_mode == PSRAM_CMD_QPI) {
        psram_set_qio_write_mode(spi_num);
        psram_set_qio_read_mode(spi_num);
    }

    //Wait for SPI0 to idle
    while (READ_PERI_REG(SPI_EXT2_REG(0)) != 0);
    DPORT_SET_PERI_REG_MASK(DPORT_HOST_INF_SEL_REG, 1 << 14);

    // Start send data
    SET_PERI_REG_MASK(SPI_CMD_REG(spi_num), SPI_USR);
    while ((READ_PERI_REG(SPI_CMD_REG(spi_num)) & SPI_USR));
    DPORT_CLEAR_PERI_REG_MASK(DPORT_HOST_INF_SEL_REG, 1 << 14);

    //recover spi mode
    SET_PERI_REG_BITS(SPI_USER_REG(spi_num), (pRxData ? SPI_FWRITE_DUAL_M : 0xf), mode_backup, SPI_FWRITE_DUAL_S);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), (SPI_FREAD_DIO_M | SPI_FREAD_DUAL_M | SPI_FREAD_QUAD_M | SPI_FREAD_QIO_M));
    SET_PERI_REG_MASK(SPI_CTRL_REG(spi_num), rd_mode_backup);

    //return cs to cs0
    SET_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS1_DIS_M);
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(PSRAM_SPI_1), SPI_CS0_DIS_M);

    if (pRxData) {
        int idx = 0;
        // Read data out
        do {
            *pRxData++ = READ_PERI_REG(SPI_W0_REG(spi_num) + (idx << 2));
        } while (++idx < ((rxByteLen / 4) + ((rxByteLen % 4) ? 1 : 0)));
    }
}

static uint32_t backup_usr[3];
static uint32_t backup_usr1[3];
static uint32_t backup_usr2[3];

//setup spi command/addr/data/dummy in user mode
static int psram_cmd_config(psram_spi_num_t spi_num, psram_cmd_t *pInData)
{
    while (READ_PERI_REG(SPI_CMD_REG(spi_num)) & SPI_USR);
    backup_usr[spi_num] = READ_PERI_REG(SPI_USER_REG(spi_num));
    backup_usr1[spi_num] = READ_PERI_REG(SPI_USER1_REG(spi_num));
    backup_usr2[spi_num] = READ_PERI_REG(SPI_USER2_REG(spi_num));
    // Set command by user.
    if (pInData->cmdBitLen != 0) {
        // Max command length 16 bits.
        SET_PERI_REG_BITS(SPI_USER2_REG(spi_num), SPI_USR_COMMAND_BITLEN, pInData->cmdBitLen - 1,
                          SPI_USR_COMMAND_BITLEN_S);
        // Enable command
        SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_COMMAND);
        // Load command,bit15-0 is cmd value.
        SET_PERI_REG_BITS(SPI_USER2_REG(spi_num), SPI_USR_COMMAND_VALUE, pInData->cmd, SPI_USR_COMMAND_VALUE_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_COMMAND);
        SET_PERI_REG_BITS(SPI_USER2_REG(spi_num), SPI_USR_COMMAND_BITLEN, 0, SPI_USR_COMMAND_BITLEN_S);
    }
    // Set Address by user.
    if (pInData->addrBitLen != 0) {
        SET_PERI_REG_BITS(SPI_USER1_REG(spi_num), SPI_USR_ADDR_BITLEN, (pInData->addrBitLen - 1), SPI_USR_ADDR_BITLEN_S);
        // Enable address
        SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_ADDR);
        // Set address
        WRITE_PERI_REG(SPI_ADDR_REG(spi_num), *pInData->addr);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_ADDR);
        SET_PERI_REG_BITS(SPI_USER1_REG(spi_num), SPI_USR_ADDR_BITLEN, 0, SPI_USR_ADDR_BITLEN_S);
    }
    // Set data by user.
    uint32_t *p_tx_val = pInData->txData;
    if (pInData->txDataBitLen != 0) {
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_MOSI);
        // Load send buffer
        int len = (pInData->txDataBitLen + 31) / 32;
        if (p_tx_val != NULL) {
            memcpy((void *)SPI_W0_REG(spi_num), p_tx_val, len * 4);
        }
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(spi_num), SPI_USR_MOSI_DBITLEN, (pInData->txDataBitLen - 1),
                          SPI_USR_MOSI_DBITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_MOSI);
        SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(spi_num), SPI_USR_MOSI_DBITLEN, 0, SPI_USR_MOSI_DBITLEN_S);
    }
    // Set rx data by user.
    if (pInData->rxDataBitLen != 0) {
        // Enable MOSI
        SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_MISO);
        // Set data send buffer length.Max data length 64 bytes.
        SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(spi_num), SPI_USR_MISO_DBITLEN, (pInData->rxDataBitLen - 1),
                          SPI_USR_MISO_DBITLEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_USR_MISO);
        SET_PERI_REG_BITS(SPI_MISO_DLEN_REG(spi_num), SPI_USR_MISO_DBITLEN, 0, SPI_USR_MISO_DBITLEN_S);
    }
    if (pInData->dummyBitLen != 0) {
        SET_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_USR_DUMMY); // dummy en
        SET_PERI_REG_BITS(SPI_USER1_REG(PSRAM_SPI_1), SPI_USR_DUMMY_CYCLELEN_V, pInData->dummyBitLen - 1,
                          SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_USR_DUMMY); // dummy en
        SET_PERI_REG_BITS(SPI_USER1_REG(PSRAM_SPI_1), SPI_USR_DUMMY_CYCLELEN_V, 0, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
    }
    return 0;
}

static void psram_cmd_end(int spi_num)
{
    while (READ_PERI_REG(SPI_CMD_REG(spi_num)) & SPI_USR);
    WRITE_PERI_REG(SPI_USER_REG(spi_num), backup_usr[spi_num]);
    WRITE_PERI_REG(SPI_USER1_REG(spi_num), backup_usr1[spi_num]);
    WRITE_PERI_REG(SPI_USER2_REG(spi_num), backup_usr2[spi_num]);
}

//exit QPI mode(set back to SPI mode)
static void psram_disable_qio_mode(psram_spi_num_t spi_num)
{
    psram_cmd_t ps_cmd;
    uint32_t cmd_exit_qpi;
    cmd_exit_qpi = PSRAM_EXIT_QMODE;
    ps_cmd.txDataBitLen = 8;
    if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
        switch (s_psram_mode) {
        case PSRAM_CACHE_F80M_S80M:
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            cmd_exit_qpi = PSRAM_EXIT_QMODE << 8;
            ps_cmd.txDataBitLen = 16;
            break;
        }
    }
    ps_cmd.txData = &cmd_exit_qpi;
    ps_cmd.cmd = 0;
    ps_cmd.cmdBitLen = 0;
    ps_cmd.addr = 0;
    ps_cmd.addrBitLen = 0;
    ps_cmd.rxData = NULL;
    ps_cmd.rxDataBitLen = 0;
    ps_cmd.dummyBitLen = 0;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_QPI);
    psram_cmd_end(spi_num);
}

//read psram id, should issue `psram_disable_qio_mode` before calling this
static void psram_read_id(psram_spi_num_t spi_num, uint64_t *dev_id)
{
    uint32_t dummy_bits = 0 + extra_dummy;
    uint32_t psram_id[2] = {0};
    psram_cmd_t ps_cmd;

    uint32_t addr = 0;
    ps_cmd.addrBitLen = 3 * 8;
    ps_cmd.cmd = PSRAM_DEVICE_ID;
    ps_cmd.cmdBitLen = 8;
    if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
        switch (s_psram_mode) {
        case PSRAM_CACHE_F80M_S80M:
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            ps_cmd.cmdBitLen = 2;   //this two bits is used to delay 2 clock cycle
            ps_cmd.cmd = 0;
            addr = (PSRAM_DEVICE_ID << 24) | 0;
            ps_cmd.addrBitLen = 4 * 8;
            break;
        }
    }
    ps_cmd.addr = &addr;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.txData = NULL;
    ps_cmd.rxDataBitLen = 8 * 8;
    ps_cmd.rxData = psram_id;
    ps_cmd.dummyBitLen = dummy_bits;

    psram_cmd_config(spi_num, &ps_cmd);
    psram_clear_spi_fifo(spi_num);
    psram_cmd_recv_start(spi_num, ps_cmd.rxData, ps_cmd.rxDataBitLen / 8, PSRAM_CMD_SPI);
    psram_cmd_end(spi_num);
    *dev_id = (uint64_t)(((uint64_t)psram_id[1] << 32) | psram_id[0]);
}

//enter QPI mode
static esp_err_t IRAM_ATTR psram_enable_qio_mode(psram_spi_num_t spi_num)
{
    psram_cmd_t ps_cmd;
    uint32_t addr = (PSRAM_ENTER_QMODE << 24) | 0;

    ps_cmd.cmdBitLen = 0;
    if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
        switch (s_psram_mode) {
        case PSRAM_CACHE_F80M_S80M:
            break;
        case PSRAM_CACHE_F80M_S40M:
        case PSRAM_CACHE_F40M_S40M:
        default:
            ps_cmd.cmdBitLen = 2;
            break;
        }
    }
    ps_cmd.cmd = 0;
    ps_cmd.addr = &addr;
    ps_cmd.addrBitLen = 8;
    ps_cmd.txData = NULL;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.rxData = NULL;
    ps_cmd.rxDataBitLen = 0;
    ps_cmd.dummyBitLen = 0;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_SPI);
    psram_cmd_end(spi_num);
    return ESP_OK;
}

#if CONFIG_SPIRAM_2T_MODE
// use SPI user mode to write psram
static void spi_user_psram_write(psram_spi_num_t spi_num, uint32_t address, uint32_t *data_buffer, uint32_t data_len)
{
    uint32_t addr = (PSRAM_QUAD_WRITE << 24) | (address & 0x7fffff);
    psram_cmd_t ps_cmd;
    ps_cmd.cmdBitLen = 0;
    ps_cmd.cmd = 0;
    ps_cmd.addr = &addr;
    ps_cmd.addrBitLen = 4 * 8;
    ps_cmd.txDataBitLen = 32 * 8;
    ps_cmd.txData = NULL;
    ps_cmd.rxDataBitLen = 0;
    ps_cmd.rxData = NULL;
    ps_cmd.dummyBitLen = 0;

    for (uint32_t i = 0; i < data_len; i += 32) {
        psram_clear_spi_fifo(spi_num);
        addr = (PSRAM_QUAD_WRITE << 24) | ((address & 0x7fffff) + i);
        ps_cmd.txData = data_buffer + (i / 4);
        psram_cmd_config(spi_num, &ps_cmd);
        psram_cmd_recv_start(spi_num, ps_cmd.rxData, ps_cmd.rxDataBitLen / 8, PSRAM_CMD_QPI);
    }
    psram_cmd_end(spi_num);
}

// use SPI user mode to read psram
static void spi_user_psram_read(psram_spi_num_t spi_num, uint32_t address, uint32_t *data_buffer, uint32_t data_len)
{
    uint32_t addr = (PSRAM_FAST_READ_QUAD << 24) | (address & 0x7fffff);
    uint32_t dummy_bits = PSRAM_FAST_READ_QUAD_DUMMY + 1;
    psram_cmd_t ps_cmd;
    ps_cmd.cmdBitLen = 0;
    ps_cmd.cmd = 0;
    ps_cmd.addr = &addr;
    ps_cmd.addrBitLen = 4 * 8;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.txData = NULL;
    ps_cmd.rxDataBitLen = 32 * 8;
    ps_cmd.dummyBitLen = dummy_bits + extra_dummy;

    for (uint32_t i = 0; i < data_len; i += 32) {
        psram_clear_spi_fifo(spi_num);
        addr = (PSRAM_FAST_READ_QUAD << 24) | ((address & 0x7fffff) + i);
        ps_cmd.rxData = data_buffer + (i / 4);
        psram_cmd_config(spi_num, &ps_cmd);
        psram_cmd_recv_start(spi_num, ps_cmd.rxData, ps_cmd.rxDataBitLen / 8, PSRAM_CMD_QPI);
    }
    psram_cmd_end(spi_num);
}

//enable psram 2T mode
static esp_err_t IRAM_ATTR psram_2t_mode_enable(psram_spi_num_t spi_num)
{
    psram_disable_qio_mode(spi_num);
    // configure psram clock as 5 MHz
    uint32_t div = rtc_clk_apb_freq_get() / 5000000;
    esp_rom_spiflash_config_clk(div, spi_num);

    psram_cmd_t ps_cmd;

    // setp1: send cmd 0x5e
    //        send one more bit clock after send cmd
    ps_cmd.cmd = 0x5e;
    ps_cmd.cmdBitLen = 8;
    ps_cmd.addrBitLen = 0;
    ps_cmd.addr = 0;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.txData = NULL;
    ps_cmd.rxDataBitLen = 0;
    ps_cmd.rxData = NULL;
    ps_cmd.dummyBitLen = 1;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_clear_spi_fifo(spi_num);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_SPI);
    psram_cmd_end(spi_num);

    // setp2: send cmd 0x5f
    //        send one more bit clock after send cmd
    ps_cmd.cmd = 0x5f;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_clear_spi_fifo(spi_num);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_SPI);
    psram_cmd_end(spi_num);

    // setp3: keep cs as high level
    //        send 128 cycles clock
    //        send 1 bit high level in ninth clock from the back to PSRAM SIO1
    static gpio_hal_context_t _gpio_hal = {
        .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
    };
    gpio_hal_set_level(&_gpio_hal, D0WD_PSRAM_CS_IO, 1);
    esp_rom_gpio_connect_out_signal(D0WD_PSRAM_CS_IO, SIG_GPIO_OUT_IDX, 0, 0);

    esp_rom_gpio_connect_out_signal(PSRAM_SPID_SD1_IO, SPIQ_OUT_IDX, 0, 0);
    esp_rom_gpio_connect_in_signal(PSRAM_SPID_SD1_IO, SPIQ_IN_IDX, 0);
    esp_rom_gpio_connect_out_signal(PSRAM_SPIQ_SD0_IO, SPID_OUT_IDX, 0, 0);
    esp_rom_gpio_connect_in_signal(PSRAM_SPIQ_SD0_IO, SPID_IN_IDX, 0);

    uint32_t w_data_2t[4] = {0x0, 0x0, 0x0, 0x00010000};

    ps_cmd.cmd = 0;
    ps_cmd.cmdBitLen = 0;
    ps_cmd.txDataBitLen = 128;
    ps_cmd.txData = w_data_2t;
    ps_cmd.dummyBitLen = 0;
    psram_clear_spi_fifo(spi_num);
    psram_cmd_config(spi_num, &ps_cmd);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_SPI);
    psram_cmd_end(spi_num);

    esp_rom_gpio_connect_out_signal(PSRAM_SPIQ_SD0_IO, SPIQ_OUT_IDX, 0, 0);
    esp_rom_gpio_connect_in_signal(PSRAM_SPIQ_SD0_IO, SPIQ_IN_IDX, 0);
    esp_rom_gpio_connect_out_signal(PSRAM_SPID_SD1_IO, SPID_OUT_IDX, 0, 0);
    esp_rom_gpio_connect_in_signal(PSRAM_SPID_SD1_IO, SPID_IN_IDX, 0);

    esp_rom_gpio_connect_out_signal(D0WD_PSRAM_CS_IO, SPICS1_OUT_IDX, 0, 0);

    // setp4: send cmd 0x5f
    //        send one more bit clock after send cmd
    ps_cmd.cmd = 0x5f;
    ps_cmd.cmdBitLen = 8;
    ps_cmd.txDataBitLen = 0;
    ps_cmd.txData = NULL;
    ps_cmd.dummyBitLen = 1;
    psram_cmd_config(spi_num, &ps_cmd);
    psram_clear_spi_fifo(spi_num);
    psram_cmd_recv_start(spi_num, NULL, 0, PSRAM_CMD_SPI);
    psram_cmd_end(spi_num);

    // configure psram clock back to the default value
    switch (s_psram_mode) {
    case PSRAM_CACHE_F80M_S40M:
    case PSRAM_CACHE_F40M_S40M:
        esp_rom_spiflash_config_clk(_SPI_40M_CLK_DIV, spi_num);
        break;
    case PSRAM_CACHE_F80M_S80M:
        esp_rom_spiflash_config_clk(_SPI_80M_CLK_DIV, spi_num);
        break;
    default:
        break;
    }
    psram_enable_qio_mode(spi_num);
    return ESP_OK;
}

#define CHECK_DATA_LEN   (1024)
#define CHECK_ADDR_STEP  (0x100000)
#define SIZE_32MBIT      (0x400000)
#define SIZE_64MBIT      (0x800000)

static esp_err_t psram_2t_mode_check(psram_spi_num_t spi_num)
{
    uint8_t w_check_data[CHECK_DATA_LEN] = {0};
    uint8_t r_check_data[CHECK_DATA_LEN] = {0};

    for (uint32_t addr = 0; addr < SIZE_32MBIT; addr += CHECK_ADDR_STEP) {
        spi_user_psram_write(spi_num, addr, (uint32_t *)w_check_data, CHECK_DATA_LEN);
    }

    memset(w_check_data, 0xff, sizeof(w_check_data));

    for (uint32_t addr = SIZE_32MBIT; addr < SIZE_64MBIT; addr += CHECK_ADDR_STEP) {
        spi_user_psram_write(spi_num, addr, (uint32_t *)w_check_data, CHECK_DATA_LEN);
    }

    for (uint32_t addr = 0; addr < SIZE_32MBIT; addr += CHECK_ADDR_STEP) {
        spi_user_psram_read(spi_num, addr, (uint32_t *)r_check_data, CHECK_DATA_LEN);
        for (uint32_t j = 0; j < CHECK_DATA_LEN; j++) {
            if (r_check_data[j] != 0xff) {
                return ESP_FAIL;
            }
        }
    }

    return ESP_OK;
}
#endif

void psram_set_cs_timing(psram_spi_num_t spi_num, psram_clk_mode_t clk_mode)
{
    if (clk_mode == PSRAM_CLK_MODE_NORM) {
        SET_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_CS_HOLD_M | SPI_CS_SETUP_M);
        // Set cs time.
        SET_PERI_REG_BITS(SPI_CTRL2_REG(spi_num), SPI_HOLD_TIME_V, PSRAM_CS_HOLD_TIME, SPI_HOLD_TIME_S);
        SET_PERI_REG_BITS(SPI_CTRL2_REG(spi_num), SPI_SETUP_TIME_V, 0, SPI_SETUP_TIME_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_CS_HOLD_M | SPI_CS_SETUP_M);
    }
}

//spi param init for psram
void IRAM_ATTR psram_spi_init(psram_spi_num_t spi_num, psram_cache_speed_t mode)
{
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spi_num), SPI_TRANS_DONE << 5);
    // SPI_CPOL & SPI_CPHA
    CLEAR_PERI_REG_MASK(SPI_PIN_REG(spi_num), SPI_CK_IDLE_EDGE);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_CK_OUT_EDGE);
    // SPI bit order
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_WR_BIT_ORDER);
    CLEAR_PERI_REG_MASK(SPI_CTRL_REG(spi_num), SPI_RD_BIT_ORDER);
    // SPI bit order
    CLEAR_PERI_REG_MASK(SPI_USER_REG(spi_num), SPI_DOUTDIN);
    // May be not must to do.
    WRITE_PERI_REG(SPI_USER1_REG(spi_num), 0);
    // SPI mode type
    CLEAR_PERI_REG_MASK(SPI_SLAVE_REG(spi_num), SPI_SLAVE_MODE);
    memset((void *)SPI_W0_REG(spi_num), 0, 16 * 4);
    psram_set_cs_timing(spi_num, s_clk_mode);
}

//psram gpio init , different working frequency we have different solutions
static void IRAM_ATTR psram_gpio_config(psram_io_t *psram_io, psram_cache_speed_t mode)
{
    int spi_cache_dummy = 0;
    uint32_t rd_mode_reg = READ_PERI_REG(SPI_CTRL_REG(0));
    if (rd_mode_reg & SPI_FREAD_QIO_M) {
        spi_cache_dummy = SPI0_R_QIO_DUMMY_CYCLELEN;
    } else if (rd_mode_reg & SPI_FREAD_DIO_M) {
        spi_cache_dummy = SPI0_R_DIO_DUMMY_CYCLELEN;
        SET_PERI_REG_BITS(SPI_USER1_REG(0), SPI_USR_ADDR_BITLEN_V, SPI0_R_DIO_ADDR_BITSLEN, SPI_USR_ADDR_BITLEN_S);
    }  else if (rd_mode_reg & (SPI_FREAD_QUAD_M | SPI_FREAD_DUAL_M)) {
        spi_cache_dummy = SPI0_R_FAST_DUMMY_CYCLELEN;
    } else {
        spi_cache_dummy = SPI0_R_FAST_DUMMY_CYCLELEN;
    }

    switch (mode) {
    case PSRAM_CACHE_F80M_S40M:
        extra_dummy = PSRAM_IO_MATRIX_DUMMY_40M;
        g_rom_spiflash_dummy_len_plus[_SPI_CACHE_PORT] = PSRAM_IO_MATRIX_DUMMY_80M;
        g_rom_spiflash_dummy_len_plus[_SPI_FLASH_PORT] = PSRAM_IO_MATRIX_DUMMY_40M;
        SET_PERI_REG_BITS(SPI_USER1_REG(_SPI_CACHE_PORT), SPI_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + PSRAM_IO_MATRIX_DUMMY_80M, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
        esp_rom_spiflash_config_clk(_SPI_80M_CLK_DIV, _SPI_CACHE_PORT);
        esp_rom_spiflash_config_clk(_SPI_40M_CLK_DIV, _SPI_FLASH_PORT);
        //set drive ability for clock
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->flash_clk_io], FUN_DRV, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->psram_clk_io], FUN_DRV, 2, FUN_DRV_S);
        break;
    case PSRAM_CACHE_F80M_S80M:
        extra_dummy = PSRAM_IO_MATRIX_DUMMY_80M;
        g_rom_spiflash_dummy_len_plus[_SPI_CACHE_PORT] = PSRAM_IO_MATRIX_DUMMY_80M;
        g_rom_spiflash_dummy_len_plus[_SPI_FLASH_PORT] = PSRAM_IO_MATRIX_DUMMY_80M;
        SET_PERI_REG_BITS(SPI_USER1_REG(_SPI_CACHE_PORT), SPI_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + PSRAM_IO_MATRIX_DUMMY_80M, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
        esp_rom_spiflash_config_clk(_SPI_80M_CLK_DIV, _SPI_CACHE_PORT);
        esp_rom_spiflash_config_clk(_SPI_80M_CLK_DIV, _SPI_FLASH_PORT);
        //set drive ability for clock
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->flash_clk_io], FUN_DRV, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->psram_clk_io], FUN_DRV, 3, FUN_DRV_S);
        break;
    case PSRAM_CACHE_F40M_S40M:
        extra_dummy = PSRAM_IO_MATRIX_DUMMY_40M;
        g_rom_spiflash_dummy_len_plus[_SPI_CACHE_PORT] = PSRAM_IO_MATRIX_DUMMY_40M;
        g_rom_spiflash_dummy_len_plus[_SPI_FLASH_PORT] = PSRAM_IO_MATRIX_DUMMY_40M;
        SET_PERI_REG_BITS(SPI_USER1_REG(_SPI_CACHE_PORT), SPI_USR_DUMMY_CYCLELEN_V, spi_cache_dummy + PSRAM_IO_MATRIX_DUMMY_40M, SPI_USR_DUMMY_CYCLELEN_S);  //DUMMY
        esp_rom_spiflash_config_clk(_SPI_40M_CLK_DIV, _SPI_CACHE_PORT);
        esp_rom_spiflash_config_clk(_SPI_40M_CLK_DIV, _SPI_FLASH_PORT);
        //set drive ability for clock
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->flash_clk_io], FUN_DRV, 2, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->psram_clk_io], FUN_DRV, 2, FUN_DRV_S);
        break;
    default:
        break;
    }
    SET_PERI_REG_MASK(SPI_USER_REG(0), SPI_USR_DUMMY); // dummy enable

    // In bootloader, all the signals are already configured,
    // We keep the following code in case the bootloader is some older version.
    esp_rom_gpio_connect_out_signal(psram_io->flash_cs_io, SPICS0_OUT_IDX, 0, 0);
    esp_rom_gpio_connect_out_signal(psram_io->psram_cs_io, SPICS1_OUT_IDX, 0, 0);
    esp_rom_gpio_connect_out_signal(psram_io->psram_spiq_sd0_io, SPIQ_OUT_IDX, 0, 0);
    esp_rom_gpio_connect_in_signal(psram_io->psram_spiq_sd0_io, SPIQ_IN_IDX, 0);
    esp_rom_gpio_connect_out_signal(psram_io->psram_spid_sd1_io, SPID_OUT_IDX, 0, 0);
    esp_rom_gpio_connect_in_signal(psram_io->psram_spid_sd1_io, SPID_IN_IDX, 0);
    esp_rom_gpio_connect_out_signal(psram_io->psram_spiwp_sd3_io, SPIWP_OUT_IDX, 0, 0);
    esp_rom_gpio_connect_in_signal(psram_io->psram_spiwp_sd3_io, SPIWP_IN_IDX, 0);
    esp_rom_gpio_connect_out_signal(psram_io->psram_spihd_sd2_io, SPIHD_OUT_IDX, 0, 0);
    esp_rom_gpio_connect_in_signal(psram_io->psram_spihd_sd2_io, SPIHD_IN_IDX, 0);

    //select pin function gpio
    if ((psram_io->flash_clk_io == MSPI_IOMUX_PIN_NUM_CLK) && (psram_io->flash_clk_io != psram_io->psram_clk_io)) {
        //flash clock signal should come from IO MUX.
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[psram_io->flash_clk_io], FUNC_SD_CLK_SPICLK);
    } else {
        //flash clock signal should come from GPIO matrix.
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[psram_io->flash_clk_io], PIN_FUNC_GPIO);
    }
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[psram_io->flash_cs_io],  PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[psram_io->psram_cs_io],  PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[psram_io->psram_clk_io], PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[psram_io->psram_spiq_sd0_io],  PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[psram_io->psram_spid_sd1_io],  PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[psram_io->psram_spihd_sd2_io], PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[psram_io->psram_spiwp_sd3_io], PIN_FUNC_GPIO);

    uint32_t flash_id = g_rom_flashchip.device_id;
    if (flash_id == FLASH_ID_GD25LQ32C) {
        // Set drive ability for 1.8v flash in 80Mhz.
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->flash_cs_io],  FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->flash_clk_io], FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->psram_cs_io],  FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->psram_clk_io], FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->psram_spiq_sd0_io],  FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->psram_spid_sd1_io],  FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->psram_spihd_sd2_io], FUN_DRV_V, 3, FUN_DRV_S);
        SET_PERI_REG_BITS(GPIO_PIN_MUX_REG[psram_io->psram_spiwp_sd3_io], FUN_DRV_V, 3, FUN_DRV_S);
    }

    // Reserve psram pins
    esp_gpio_reserve(BIT64(psram_io->flash_clk_io)        |
                     BIT64(psram_io->flash_cs_io)         |
                     BIT64(psram_io->psram_clk_io)        |
                     BIT64(psram_io->psram_cs_io)         |
                     BIT64(psram_io->psram_spiq_sd0_io)   |
                     BIT64(psram_io->psram_spid_sd1_io)   |
                     BIT64(psram_io->psram_spihd_sd2_io)  |
                     BIT64(psram_io->psram_spiwp_sd3_io));
}

//used in UT only
bool psram_is_32mbit_ver0(void)
{
    return PSRAM_IS_32MBIT_VER0(s_psram_id);
}

/*
 * Psram mode init will overwrite original flash speed mode, so that it is possible to change psram and flash speed after OTA.
 * Flash read mode(QIO/QOUT/DIO/DOUT) will not be changed in app bin. It is decided by bootloader, OTA can not change this mode.
 */
esp_err_t IRAM_ATTR esp_psram_impl_enable(void)   //psram init
{
    psram_vaddr_mode_t vaddrmode = PSRAM_MODE;
    psram_cache_speed_t mode = PSRAM_SPEED;
    psram_io_t psram_io = {0};
    uint32_t pkg_ver = efuse_ll_get_chip_ver_pkg();
    if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D2WDQ5) {
        ESP_EARLY_LOGI(TAG, "This chip is ESP32-D2WD");
        rtc_vddsdio_config_t cfg = rtc_vddsdio_get_config();
        if (cfg.tieh != RTC_VDDSDIO_TIEH_1_8V) {
            ESP_EARLY_LOGE(TAG, "VDDSDIO is not 1.8V");
            return ESP_FAIL;
        }
        psram_io.psram_clk_io = D2WD_PSRAM_CLK_IO;
        psram_io.psram_cs_io  = D2WD_PSRAM_CS_IO;
    } else if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4 && ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 300)) {
        ESP_EARLY_LOGE(TAG, "This chip is ESP32-PICO-V3. It does not support PSRAM (disable it in Kconfig)");
        return ESP_FAIL;
    } else if ((pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4) || (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32U4WDH)) {
        ESP_EARLY_LOGI(TAG, "This chip is %s",
                       (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOD4) ? "ESP32-PICO" : "ESP32-U4WDH");
        // We have better alternatives, though it's possible to use U4WDH together with PSRAM.
        // U4WDH shares the same pin config with PICO for historical reasons.
        rtc_vddsdio_config_t cfg = rtc_vddsdio_get_config();
        if (cfg.tieh != RTC_VDDSDIO_TIEH_3_3V) {
            ESP_EARLY_LOGE(TAG, "VDDSDIO is not 3.3V");
            return ESP_FAIL;
        }
        psram_io.psram_clk_io = PICO_PSRAM_CLK_IO;
        psram_io.psram_cs_io  = PICO_PSRAM_CS_IO;
    } else if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32PICOV302) {
        ESP_EARLY_LOGI(TAG, "This chip is ESP32-PICO-V3-02");
        rtc_vddsdio_config_t cfg = rtc_vddsdio_get_config();
        if (cfg.tieh != RTC_VDDSDIO_TIEH_3_3V) {
            ESP_EARLY_LOGE(TAG, "VDDSDIO is not 3.3V");
            return ESP_FAIL;
        }
        psram_io.psram_clk_io = PICO_V3_02_PSRAM_CLK_IO;
        psram_io.psram_cs_io  = PICO_V3_02_PSRAM_CS_IO;
    } else if ((pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ6) || (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D0WDQ5)) {
        ESP_EARLY_LOGI(TAG, "This chip is ESP32-D0WD");
        psram_io.psram_clk_io = D0WD_PSRAM_CLK_IO;
        psram_io.psram_cs_io  = D0WD_PSRAM_CS_IO;
    } else if (pkg_ver == EFUSE_RD_CHIP_VER_PKG_ESP32D0WDR2V3) {
        ESP_EARLY_LOGI(TAG, "This chip is ESP32-D0WDR2-V3");
        rtc_vddsdio_config_t cfg = rtc_vddsdio_get_config();
        if (cfg.tieh != RTC_VDDSDIO_TIEH_3_3V) {
            ESP_EARLY_LOGE(TAG, "VDDSDIO is not 3.3V");
            return ESP_FAIL;
        }
        psram_io.psram_clk_io = D0WDR2_V3_PSRAM_CLK_IO;
        psram_io.psram_cs_io  = D0WDR2_V3_PSRAM_CS_IO;
    } else {
        ESP_EARLY_LOGE(TAG, "Not a valid or known package id: %" PRIu32, pkg_ver);
        abort();
    }
    s_psram_cs_io = psram_io.psram_cs_io;

    const uint32_t spiconfig = esp_rom_efuse_get_flash_gpio_info();
    if (spiconfig == ESP_ROM_EFUSE_FLASH_DEFAULT_SPI) {
        psram_io.flash_clk_io       = MSPI_IOMUX_PIN_NUM_CLK;
        psram_io.flash_cs_io        = MSPI_IOMUX_PIN_NUM_CS0;
        psram_io.psram_spiq_sd0_io  = PSRAM_SPIQ_SD0_IO;
        psram_io.psram_spid_sd1_io  = PSRAM_SPID_SD1_IO;
        psram_io.psram_spiwp_sd3_io = PSRAM_SPIWP_SD3_IO;
        psram_io.psram_spihd_sd2_io = PSRAM_SPIHD_SD2_IO;
    } else if (spiconfig == ESP_ROM_EFUSE_FLASH_DEFAULT_HSPI) {
        psram_io.flash_clk_io       = FLASH_HSPI_CLK_IO;
        psram_io.flash_cs_io        = FLASH_HSPI_CS_IO;
        psram_io.psram_spiq_sd0_io  = PSRAM_HSPI_SPIQ_SD0_IO;
        psram_io.psram_spid_sd1_io  = PSRAM_HSPI_SPID_SD1_IO;
        psram_io.psram_spiwp_sd3_io = PSRAM_HSPI_SPIWP_SD3_IO;
        psram_io.psram_spihd_sd2_io = PSRAM_HSPI_SPIHD_SD2_IO;
    } else {
        psram_io.flash_clk_io       = EFUSE_SPICONFIG_RET_SPICLK(spiconfig);
        psram_io.flash_cs_io        = EFUSE_SPICONFIG_RET_SPICS0(spiconfig);
        psram_io.psram_spiq_sd0_io  = EFUSE_SPICONFIG_RET_SPIQ(spiconfig);
        psram_io.psram_spid_sd1_io  = EFUSE_SPICONFIG_RET_SPID(spiconfig);
        psram_io.psram_spihd_sd2_io = EFUSE_SPICONFIG_RET_SPIHD(spiconfig);
        psram_io.psram_spiwp_sd3_io = bootloader_flash_get_wp_pin();
    }

    if (psram_io.flash_clk_io == psram_io.psram_clk_io) {
        s_clk_mode = PSRAM_CLK_MODE_NORM;
    } else {
        s_clk_mode = PSRAM_CLK_MODE_DCLK;
    }

    assert(mode < PSRAM_CACHE_MAX && "we don't support any other mode for now.");
    s_psram_mode = mode;

    WRITE_PERI_REG(SPI_EXT3_REG(0), 0x1);
    CLEAR_PERI_REG_MASK(SPI_USER_REG(PSRAM_SPI_1), SPI_USR_PREP_HOLD_M);

    psram_spi_init(PSRAM_SPI_1, mode);

    switch (mode) {
    case PSRAM_CACHE_F80M_S80M:
        esp_rom_gpio_connect_out_signal(psram_io.psram_clk_io, SPICLK_OUT_IDX, 0, 0);
        break;
    case PSRAM_CACHE_F80M_S40M:
    case PSRAM_CACHE_F40M_S40M:
    default:
        if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
            /* We need to delay CLK to the PSRAM with respect to the clock signal as output by the SPI peripheral.
            We do this by routing it signal to signal 224/225, which are used as a loopback; the extra run through
            the GPIO matrix causes the delay. We use GPIO28/29 (which is not in any package but has pad logic in
            silicon) as a temporary pad for this. So the signal path is:
            SPI CLK --> GPIO28 --> signal224(in then out) --> internal GPIO29 --> signal225(in then out) --> GPIO17(PSRAM CLK)
            */
            esp_rom_gpio_connect_out_signal(PSRAM_INTERNAL_IO_28,  SPICLK_OUT_IDX, 0, 0);
            esp_rom_gpio_connect_in_signal(PSRAM_INTERNAL_IO_28,   SIG_IN_FUNC224_IDX, 0);
            esp_rom_gpio_connect_out_signal(PSRAM_INTERNAL_IO_29,  SIG_IN_FUNC224_IDX, 0, 0);
            esp_rom_gpio_connect_in_signal(PSRAM_INTERNAL_IO_29,   SIG_IN_FUNC225_IDX, 0);
            esp_rom_gpio_connect_out_signal(psram_io.psram_clk_io, SIG_IN_FUNC225_IDX, 0, 0);
        } else {
            esp_rom_gpio_connect_out_signal(psram_io.psram_clk_io, SPICLK_OUT_IDX, 0, 0);
        }
        break;
    }

    // Rise VDDSIO for 1.8V psram.
    bootloader_common_vddsdio_configure();
    // GPIO related settings
    psram_gpio_config(&psram_io, mode);

    psram_spi_num_t spi_num = PSRAM_SPI_1;
    psram_disable_qio_mode(spi_num);
    psram_read_id(spi_num, &s_psram_id);
    if (!PSRAM_IS_VALID(s_psram_id)) {
        /* 16Mbit psram ID read error workaround:
         * treat the first read id as a dummy one as the pre-condition,
         * Send Read ID command again
         */
        psram_read_id(spi_num, &s_psram_id);
        if (!PSRAM_IS_VALID(s_psram_id)) {
            ESP_EARLY_LOGE(TAG, "PSRAM ID read error: 0x%08" PRIx32 ", PSRAM chip not found or not supported", (uint32_t)s_psram_id);
            return ESP_ERR_NOT_SUPPORTED;
        }
    }

    if (psram_is_32mbit_ver0()) {
        if (s_clk_mode != PSRAM_CLK_MODE_DCLK) {
            ESP_EARLY_LOGE(TAG, "PSRAM rev0 can't share CLK with Flash");
            abort();
        }
        if (mode == PSRAM_CACHE_F80M_S80M) {
#ifdef CONFIG_SPIRAM_OCCUPY_NO_HOST
            ESP_EARLY_LOGE(TAG, "This version of PSRAM needs to claim an extra SPI peripheral at 80MHz. Please either: choose lower frequency by SPIRAM_SPEED_, or select one SPI peripheral it by SPIRAM_OCCUPY_*SPI_HOST in the menuconfig.");
            abort();
#else
            /*   note: If the third mode(80Mhz+80Mhz) is enabled for 32MBit 1V8 psram, one of HSPI/VSPI port will be
                 occupied by the system (according to kconfig).
                 Application code should never touch HSPI/VSPI hardware in this case.  We try to stop applications
                 from doing this using the drivers by claiming the port for ourselves */
            periph_module_enable(PSRAM_SPI_MODULE);
            bool r = spicommon_periph_claim(PSRAM_SPI_HOST, "psram");
            if (!r) {
                return ESP_ERR_INVALID_STATE;
            }
            esp_rom_gpio_connect_out_signal(psram_io.psram_clk_io, PSRAM_CLK_SIGNAL, 0, 0);
            //use spi3 clock,but use spi1 data/cs wires
            //We get a solid 80MHz clock from SPI3 by setting it up, starting a transaction, waiting until it
            //is in progress, then cutting the clock (but not the reset!) to that peripheral.
            WRITE_PERI_REG(SPI_ADDR_REG(PSRAM_SPI_NUM), 32 << 24);
            SET_PERI_REG_MASK(SPI_CMD_REG(PSRAM_SPI_NUM), SPI_FLASH_READ_M);
            uint32_t spi_status;
            while (1) {
                spi_status = READ_PERI_REG(SPI_EXT2_REG(PSRAM_SPI_NUM));
                if (spi_status != 0 && spi_status != 1) {
                    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, PSRAM_SPICLKEN);
                    break;
                }
            }
#endif
        }
    } else {
        // For other psram, we don't need any extra clock cycles after cs get back to high level
        s_clk_mode = PSRAM_CLK_MODE_NORM;
        esp_rom_gpio_connect_out_signal(PSRAM_INTERNAL_IO_28, SIG_GPIO_OUT_IDX, 0, 0);
        esp_rom_gpio_connect_out_signal(PSRAM_INTERNAL_IO_29, SIG_GPIO_OUT_IDX, 0, 0);
        esp_rom_gpio_connect_out_signal(psram_io.psram_clk_io, SPICLK_OUT_IDX, 0, 0);
    }

    // Update cs timing according to psram driving method.
    psram_set_cs_timing(PSRAM_SPI_1, s_clk_mode);
    psram_set_cs_timing(_SPI_CACHE_PORT, s_clk_mode);
    psram_enable_qio_mode(PSRAM_SPI_1);

    if (((PSRAM_SIZE_ID(s_psram_id) == PSRAM_EID_SIZE_64MBITS) || PSRAM_IS_64MBIT_TRIAL(s_psram_id))) {
#if CONFIG_SPIRAM_2T_MODE
#if CONFIG_SPIRAM_BANKSWITCH_ENABLE
        ESP_EARLY_LOGE(TAG, "PSRAM 2T mode and SPIRAM bank switching can not enabled meanwhile. Please read the help text for SPIRAM_2T_MODE in the project configuration menu.");
        abort();
#endif
        /* Note: 2T mode command should not be sent twice,
           otherwise psram would get back to normal mode. */
        if (psram_2t_mode_check(PSRAM_SPI_1) != ESP_OK) {
            psram_2t_mode_enable(PSRAM_SPI_1);
            if (psram_2t_mode_check(PSRAM_SPI_1) != ESP_OK) {
                ESP_EARLY_LOGE(TAG, "PSRAM 2T mode enable fail!");
                return ESP_FAIL;
            }
        }
        s_2t_mode_enabled = true;
        ESP_EARLY_LOGI(TAG, "PSRAM is in 2T mode");
#endif
    }

    psram_cache_init(mode, vaddrmode);
    return ESP_OK;
}

//register initialization for sram cache params and r/w commands
static void IRAM_ATTR psram_cache_init(psram_cache_speed_t psram_cache_mode, psram_vaddr_mode_t vaddrmode)
{
    switch (psram_cache_mode) {
    case PSRAM_CACHE_F80M_S80M:
        CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(31));   //flash 1 div clk,80+40;
        CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(30)); //pre clk div , ONLY IF SPI/SRAM@ DIFFERENT SPEED,JUST FOR SPI0. FLASH DIV 2+SRAM DIV4
        break;
    case PSRAM_CACHE_F80M_S40M:
        CLEAR_PERI_REG_MASK(SPI_CLOCK_REG(0), SPI_CLK_EQU_SYSCLK_M);
        SET_PERI_REG_BITS(SPI_CLOCK_REG(0), SPI_CLKDIV_PRE_V, 0, SPI_CLKDIV_PRE_S);
        SET_PERI_REG_BITS(SPI_CLOCK_REG(0), SPI_CLKCNT_N, 1, SPI_CLKCNT_N_S);
        SET_PERI_REG_BITS(SPI_CLOCK_REG(0), SPI_CLKCNT_H, 0, SPI_CLKCNT_H_S);
        SET_PERI_REG_BITS(SPI_CLOCK_REG(0), SPI_CLKCNT_L, 1, SPI_CLKCNT_L_S);
        SET_PERI_REG_MASK(SPI_DATE_REG(0), BIT(31)); //flash 1 div clk
        CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(30)); //pre clk div , ONLY IF SPI/SRAM@ DIFFERENT SPEED,JUST FOR SPI0.
        break;
    case PSRAM_CACHE_F40M_S40M:
    default:
        CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(31)); //flash 1 div clk
        CLEAR_PERI_REG_MASK(SPI_DATE_REG(0), BIT(30)); //pre clk div
        break;
    }

    CLEAR_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_SRAM_DIO_M);       //disable dio mode for cache command
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_SRAM_QIO_M);         //enable qio mode for cache command
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_CACHE_SRAM_USR_RCMD_M);  //enable cache read command
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_CACHE_SRAM_USR_WCMD_M);  //enable cache write command
    SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0), SPI_SRAM_ADDR_BITLEN_V, 23, SPI_SRAM_ADDR_BITLEN_S); //write address for cache command.
    SET_PERI_REG_MASK(SPI_CACHE_SCTRL_REG(0), SPI_USR_RD_SRAM_DUMMY_M);    //enable cache read dummy

    //config sram cache r/w command
    SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 7,
                      SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_VALUE_V, PSRAM_FAST_READ_QUAD,
                      SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0xEB
    SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_BITLEN, 7,
                      SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_VALUE, PSRAM_QUAD_WRITE,
                      SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38
    SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0), SPI_SRAM_DUMMY_CYCLELEN_V, PSRAM_FAST_READ_QUAD_DUMMY + extra_dummy,
                      SPI_SRAM_DUMMY_CYCLELEN_S); //dummy, psram cache : 40m--+1dummy; 80m--+2dummy

    switch (psram_cache_mode) {
    case PSRAM_CACHE_F80M_S80M: //in this mode , no delay is needed
        break;
    case PSRAM_CACHE_F80M_S40M: //if sram is @40M, need 2 cycles of delay
    case PSRAM_CACHE_F40M_S40M:
    default:
        if (s_clk_mode == PSRAM_CLK_MODE_DCLK) {
            SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 15,
                              SPI_CACHE_SRAM_USR_RD_CMD_BITLEN_S); //read command length, 2 bytes(1byte for delay),sending in qio mode in cache
            SET_PERI_REG_BITS(SPI_SRAM_DRD_CMD_REG(0), SPI_CACHE_SRAM_USR_RD_CMD_VALUE_V, ((PSRAM_FAST_READ_QUAD) << 8),
                              SPI_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0xEB, read command value,(0x00 for delay,0xeb for cmd)
            SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_BITLEN, 15,
                              SPI_CACHE_SRAM_USR_WR_CMD_BITLEN_S); //write command length,2 bytes(1byte for delay,send in qio mode in cache)
            SET_PERI_REG_BITS(SPI_SRAM_DWR_CMD_REG(0), SPI_CACHE_SRAM_USR_WR_CMD_VALUE, ((PSRAM_QUAD_WRITE) << 8),
                              SPI_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38, write command value,(0x00 for delay)
            SET_PERI_REG_BITS(SPI_CACHE_SCTRL_REG(0), SPI_SRAM_DUMMY_CYCLELEN_V, PSRAM_FAST_READ_QUAD_DUMMY + extra_dummy,
                              SPI_SRAM_DUMMY_CYCLELEN_S); //dummy, psram cache : 40m--+1dummy; 80m--+2dummy
        }
        break;
    }

    DPORT_CLEAR_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_HL | DPORT_PRO_DRAM_SPLIT);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_HL | DPORT_APP_DRAM_SPLIT);
    if (vaddrmode == PSRAM_VADDR_MODE_LOWHIGH) {
        DPORT_SET_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_HL);
        DPORT_SET_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_HL);
    } else if (vaddrmode == PSRAM_VADDR_MODE_EVENODD) {
        DPORT_SET_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_SPLIT);
        DPORT_SET_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_SPLIT);
    }

    DPORT_CLEAR_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CACHE_MASK_DRAM1 | DPORT_PRO_CACHE_MASK_OPSDRAM); //use Dram1 to visit ext sram.
    //cache page mode : 1 -->16k  4 -->2k  0-->32k,(accord with the settings in cache_sram_mmu_set)
    DPORT_SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL1_REG, DPORT_PRO_CMMU_SRAM_PAGE_MODE, 0, DPORT_PRO_CMMU_SRAM_PAGE_MODE_S);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MASK_DRAM1 | DPORT_APP_CACHE_MASK_OPSDRAM); //use Dram1 to visit ext sram.
    //cache page mode : 1 -->16k  4 -->2k  0-->32k,(accord with the settings in cache_sram_mmu_set)
    DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CMMU_SRAM_PAGE_MODE, 0, DPORT_APP_CMMU_SRAM_PAGE_MODE_S);

    CLEAR_PERI_REG_MASK(SPI_PIN_REG(0), SPI_CS1_DIS_M); //ENABLE SPI0 CS1 TO PSRAM(CS0--FLASH; CS1--SRAM)
}

esp_err_t esp_psram_impl_get_physical_size(uint32_t *out_size_bytes)
{
    if (!out_size_bytes) {
        return ESP_ERR_INVALID_ARG;
    }

    if ((PSRAM_SIZE_ID(s_psram_id) == PSRAM_EID_SIZE_64MBITS) || PSRAM_IS_64MBIT_TRIAL(s_psram_id)) {
        *out_size_bytes = s_2t_mode_enabled ? PSRAM_SIZE_4MB : PSRAM_SIZE_8MB;
    } else if (PSRAM_SIZE_ID(s_psram_id) == PSRAM_EID_SIZE_32MBITS) {
        *out_size_bytes = PSRAM_SIZE_4MB;
    } else if (PSRAM_SIZE_ID(s_psram_id) == PSRAM_EID_SIZE_16MBITS) {
        *out_size_bytes = PSRAM_SIZE_2MB;
    } else {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_OK;
}

/**
 * This function is to get the available physical psram size in bytes.
 * On ESP32, all of the PSRAM physical region are available
 */
esp_err_t esp_psram_impl_get_available_size(uint32_t *out_size_bytes)
{
    return esp_psram_impl_get_physical_size(out_size_bytes);
}
#endif // CONFIG_SPIRAM
