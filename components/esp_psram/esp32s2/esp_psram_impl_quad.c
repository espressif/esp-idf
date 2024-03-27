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
#include "esp32s2/rom/spi_flash.h"
#include "esp32s2/rom/opi_flash.h"
#include "rom/efuse.h"
#include "esp_rom_efuse.h"
#include "soc/spi_reg.h"
#include "soc/io_mux_reg.h"
#include "esp_private/esp_gpio_reserve.h"

static const char* TAG = "quad_psram";

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

// For the old version 32Mbit psram, using the spicial driver */
#define PSRAM_IS_32MBIT_VER0(id)  (PSRAM_EID(id) == 0x20)
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

#define PSRAM_IO_MATRIX_DUMMY_20M   0
#define PSRAM_IO_MATRIX_DUMMY_40M   0
#define PSRAM_IO_MATRIX_DUMMY_80M   0
#define _SPI_CACHE_PORT             0
#define _SPI_FLASH_PORT             1
#define _SPI_80M_CLK_DIV            1
#define _SPI_40M_CLK_DIV            2
#define _SPI_20M_CLK_DIV            4

typedef enum {
    PSRAM_VADDR_MODE_NORMAL = 0,
} psram_vaddr_mode_t;

typedef enum {
    PSRAM_CLK_MODE_NORM = 0,  /*!< Normal SPI mode */
    PSRAM_CLK_MODE_A1C,       /*!< ONE extra clock cycles after CS is set high level */
    PSRAM_CLK_MODE_A2C,       /*!< Two extra clock cycles after CS is set high level */
    PSRAM_CLK_MODE_ALON,      /*!< clock always on */
    PSRAM_CLK_MODE_MAX,
} psram_clk_mode_t;

typedef enum {
    PSRAM_EID_SIZE_16MBITS = 0,
    PSRAM_EID_SIZE_32MBITS = 1,
    PSRAM_EID_SIZE_64MBITS = 2,
} psram_eid_size_t;

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

#define PSRAM_IO_CONF_DEFAULT() {             \
    .flash_clk_io       = FLASH_CLK_IO,       \
    .flash_cs_io        = FLASH_CS_IO,        \
    .psram_clk_io       = PSRAM_CLK_IO,       \
    .psram_cs_io        = PSRAM_CS_IO,        \
    .psram_spiq_sd0_io  = PSRAM_SPIQ_SD0_IO,  \
    .psram_spid_sd1_io  = PSRAM_SPID_SD1_IO,  \
    .psram_spiwp_sd3_io = PSRAM_SPIWP_SD3_IO, \
    .psram_spihd_sd2_io = PSRAM_SPIHD_SD2_IO,  \
}

typedef enum {
    PSRAM_SPI_1  = 0x1,
    /* PSRAM_SPI_2, */
    /* PSRAM_SPI_3, */
    PSRAM_SPI_MAX,
} psram_spi_num_t;

typedef enum {
    PSRAM_CMD_QPI,
    PSRAM_CMD_SPI,
} psram_cmd_mode_t;

typedef enum {
    PSRAM_CACHE_S80M = 1,
    PSRAM_CACHE_S40M,
    PSRAM_CACHE_S26M,
    PSRAM_CACHE_S20M,
    PSRAM_CACHE_MAX,
} psram_cache_speed_t;

#if CONFIG_SPIRAM_SPEED_40M
#define PSRAM_SPEED PSRAM_CACHE_S40M
#elif CONFIG_SPIRAM_SPEED_80M
#define PSRAM_SPEED PSRAM_CACHE_S80M
#else
#define PSRAM_SPEED PSRAM_CACHE_S20M
#endif

typedef esp_rom_spi_cmd_t psram_cmd_t;

static uint32_t s_psram_id = 0;
static void psram_cache_init(psram_cache_speed_t psram_cache_mode, psram_vaddr_mode_t vaddrmode);
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
    static uint32_t current_wrap_size = 0;
    if (current_wrap_size == wrap_size) {
        return ESP_OK;
    }
    switch (wrap_size) {
    case 32:
    case 0:
        psram_set_wrap_burst_length(PSRAM_SPI_1, PSRAM_CMD_QPI);
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

//read psram id, should issue `psram_disable_qio_mode` before calling this
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
static void IRAM_ATTR psram_enable_qio_mode(int spi_num)
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

static void psram_set_spi1_cmd_cs_timing(psram_clk_mode_t clk_mode)
{
    if (clk_mode == PSRAM_CLK_MODE_NORM) {
        // SPI1 Flash Operation port
        SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(_SPI_FLASH_PORT), SPI_MEM_CS_HOLD_TIME_V, 1, SPI_MEM_CS_HOLD_TIME_S);
        SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(_SPI_FLASH_PORT), SPI_MEM_CS_SETUP_TIME_V, 0, SPI_MEM_CS_SETUP_TIME_S);
        SET_PERI_REG_MASK(SPI_MEM_USER_REG(_SPI_FLASH_PORT), SPI_MEM_CS_HOLD_M | SPI_MEM_CS_SETUP_M);
    } else {
        SET_PERI_REG_MASK(SPI_MEM_USER_REG(_SPI_FLASH_PORT), SPI_MEM_CS_HOLD_M | SPI_MEM_CS_SETUP_M);
    }
}

static void psram_set_spi0_cache_cs_timing(psram_clk_mode_t clk_mode)
{
    if (clk_mode == PSRAM_CLK_MODE_NORM) {
        // SPI0 SRAM Cache port
        SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(_SPI_CACHE_PORT), SPI_MEM_SPI_SMEM_CS_HOLD_TIME_V, 1, SPI_MEM_SPI_SMEM_CS_HOLD_TIME_S);
        SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_AC_REG(_SPI_CACHE_PORT), SPI_MEM_SPI_SMEM_CS_SETUP_TIME_V, 0, SPI_MEM_SPI_SMEM_CS_SETUP_TIME_S);
        SET_PERI_REG_MASK(SPI_MEM_SPI_SMEM_AC_REG(_SPI_CACHE_PORT), SPI_MEM_SPI_SMEM_CS_HOLD_M | SPI_MEM_SPI_SMEM_CS_SETUP_M);
        // SPI0 Flash Cache port
        SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(_SPI_CACHE_PORT), SPI_MEM_CS_HOLD_TIME_V, 0, SPI_MEM_CS_HOLD_TIME_S);
        SET_PERI_REG_BITS(SPI_MEM_CTRL2_REG(_SPI_CACHE_PORT), SPI_MEM_CS_SETUP_TIME_V, 0, SPI_MEM_CS_SETUP_TIME_S);
        SET_PERI_REG_MASK(SPI_MEM_USER_REG(_SPI_CACHE_PORT), SPI_MEM_CS_HOLD_M | SPI_MEM_CS_SETUP_M);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_USER_REG(_SPI_CACHE_PORT), SPI_CS_HOLD_M | SPI_CS_SETUP_M);
    }
}

//psram gpio init , different working frequency we have different solutions
static void IRAM_ATTR psram_gpio_config(psram_cache_speed_t mode)
{
    psram_io_t psram_io = PSRAM_IO_CONF_DEFAULT();
    const uint32_t spiconfig = esp_rom_efuse_get_flash_gpio_info();
    if (spiconfig == ESP_ROM_EFUSE_FLASH_DEFAULT_SPI) {
        /* FLASH pins(except wp / hd) are all configured via IO_MUX in rom. */
    } else {
        // FLASH pins are all configured via GPIO matrix in ROM.
        psram_io.flash_clk_io       = EFUSE_SPICONFIG_RET_SPICLK(spiconfig);
        psram_io.flash_cs_io        = EFUSE_SPICONFIG_RET_SPICS0(spiconfig);
        psram_io.psram_spiq_sd0_io  = EFUSE_SPICONFIG_RET_SPIQ(spiconfig);
        psram_io.psram_spid_sd1_io  = EFUSE_SPICONFIG_RET_SPID(spiconfig);
        psram_io.psram_spihd_sd2_io = EFUSE_SPICONFIG_RET_SPIHD(spiconfig);
        psram_io.psram_spiwp_sd3_io = esp_rom_efuse_get_flash_wp_gpio();
    }
    esp_rom_spiflash_select_qio_pins(psram_io.psram_spiwp_sd3_io, spiconfig);
    s_psram_cs_io = psram_io.psram_cs_io;

    // Preserve psram pins
    esp_gpio_reserve(BIT64(psram_io.flash_clk_io)        |
                     BIT64(psram_io.flash_cs_io)         |
                     BIT64(psram_io.psram_clk_io)        |
                     BIT64(psram_io.psram_cs_io)         |
                     BIT64(psram_io.psram_spiq_sd0_io)   |
                     BIT64(psram_io.psram_spid_sd1_io)   |
                     BIT64(psram_io.psram_spihd_sd2_io)  |
                     BIT64(psram_io.psram_spiwp_sd3_io));
}

//used in UT only
bool psram_is_32mbit_ver0(void)
{
    return PSRAM_IS_32MBIT_VER0(s_psram_id);
}

static void psram_set_clk_mode(int spi_num, psram_clk_mode_t clk_mode)
{
    if (spi_num == _SPI_CACHE_PORT) {
        REG_SET_FIELD(SPI_MEM_SRAM_CMD_REG(0), SPI_MEM_SCLK_MODE, clk_mode);
    } else if (spi_num == _SPI_FLASH_PORT) {
        REG_SET_FIELD(SPI_MEM_CTRL1_REG(1), SPI_MEM_CLK_MODE, clk_mode);
    }
}

/*
 * Psram mode init will overwrite original flash speed mode, so that it is possible to change psram and flash speed after OTA.
 * Flash read mode(QIO/QOUT/DIO/DOUT) will not be changed in app bin. It is decided by bootloader, OTA can not change this mode.
 */
esp_err_t IRAM_ATTR esp_psram_impl_enable(void)   //psram init
{
    psram_vaddr_mode_t vaddrmode = PSRAM_VADDR_MODE_NORMAL;
    psram_cache_speed_t mode = PSRAM_SPEED;
    assert(mode < PSRAM_CACHE_MAX && "we don't support any other mode for now.");
    // GPIO related settings
    psram_gpio_config(mode);

    /* SPI1: set spi1 clk mode, in order to send commands on SPI1 */
    /* SPI1: set cs timing(hold time) in order to send commands on SPI1 */
    psram_set_clk_mode(_SPI_FLASH_PORT, PSRAM_CLK_MODE_A1C);
    psram_set_spi1_cmd_cs_timing(PSRAM_CLK_MODE_A1C);

    int spi_num = PSRAM_SPI_1;
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

    psram_clk_mode_t clk_mode = PSRAM_CLK_MODE_MAX;
    if (psram_is_32mbit_ver0()) {
        clk_mode = PSRAM_CLK_MODE_A1C;
        // SPI1: keep clock mode and cs timing for spi1
    } else {
        // For other psram, we don't need any extra clock cycles after cs get back to high level
        clk_mode = PSRAM_CLK_MODE_NORM;
        // SPI1: set clock mode and cs timing to normal mode
        psram_set_clk_mode(_SPI_FLASH_PORT, PSRAM_CLK_MODE_NORM);
        psram_set_spi1_cmd_cs_timing(PSRAM_CLK_MODE_NORM);
    }

    /* SPI1: send psram reset command */
    /* SPI1: send QPI enable command  */
    psram_reset_mode(PSRAM_SPI_1);
    psram_enable_qio_mode(PSRAM_SPI_1);

    // after sending commands, set spi1 clock mode and cs timing to normal mode.
    // since all the operations are sent via SPI0 Cache
    /* SPI1: set clock mode to normal mode. */
    /* SPI1: set cs timing to normal        */
    psram_set_clk_mode(_SPI_FLASH_PORT, PSRAM_CLK_MODE_NORM);
    psram_set_spi1_cmd_cs_timing(PSRAM_CLK_MODE_NORM);

    /* SPI0: set spi0 clock mode             */
    /* SPI0: set spi0 flash/cache cs timing  */
    psram_set_clk_mode(_SPI_CACHE_PORT, clk_mode);
    psram_set_spi0_cache_cs_timing(clk_mode);

    // SPI0: init SPI commands for Cache
    psram_cache_init(mode, vaddrmode);

    return ESP_OK;
}

static void IRAM_ATTR psram_clock_set(int spi_num, int8_t freqdiv)
{
    uint32_t  freqbits;
    if (1 >= freqdiv) {
        WRITE_PERI_REG(SPI_MEM_SRAM_CLK_REG(spi_num), SPI_MEM_SCLK_EQU_SYSCLK);
    } else {
        freqbits = (((freqdiv - 1) << SPI_MEM_SCLKCNT_N_S)) | (((freqdiv / 2 - 1) << SPI_MEM_SCLKCNT_H_S)) | ((freqdiv - 1) << SPI_MEM_SCLKCNT_L_S);
        WRITE_PERI_REG(SPI_MEM_SRAM_CLK_REG(spi_num), freqbits);
    }
}

//register initialization for sram cache params and r/w commands
static void IRAM_ATTR psram_cache_init(psram_cache_speed_t psram_cache_mode, psram_vaddr_mode_t vaddrmode)
{
    int extra_dummy = 0;
    switch (psram_cache_mode) {
    case PSRAM_CACHE_S80M:
        psram_clock_set(0, 1);
        extra_dummy = PSRAM_IO_MATRIX_DUMMY_80M;
        break;
    case PSRAM_CACHE_S40M:
        psram_clock_set(0, 2);
        extra_dummy = PSRAM_IO_MATRIX_DUMMY_40M;
        break;
    case PSRAM_CACHE_S26M:
        psram_clock_set(0, 3);
        extra_dummy = PSRAM_IO_MATRIX_DUMMY_20M;
        break;
    case PSRAM_CACHE_S20M:
        psram_clock_set(0, 4);
        extra_dummy = PSRAM_IO_MATRIX_DUMMY_20M;
        break;
    default:
        psram_clock_set(0, 2);
        break;
    }

    CLEAR_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_USR_SRAM_DIO_M);       //disable dio mode for cache command
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_USR_SRAM_QIO_M);         //enable qio mode for cache command
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_CACHE_SRAM_USR_RCMD_M);  //enable cache read command
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_CACHE_SRAM_USR_WCMD_M);  //enable cache write command
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_SRAM_ADDR_BITLEN_V, 23, SPI_MEM_SRAM_ADDR_BITLEN_S); //write address for cache command.
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_USR_RD_SRAM_DUMMY_M);    //enable cache read dummy

    //config sram cache r/w command
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN, 7,
                      SPI_MEM_CACHE_SRAM_USR_WR_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DWR_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE, PSRAM_QUAD_WRITE,
                      SPI_MEM_CACHE_SRAM_USR_WR_CMD_VALUE_S); //0x38
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_V, 7,
                      SPI_MEM_CACHE_SRAM_USR_RD_CMD_BITLEN_S);
    SET_PERI_REG_BITS(SPI_MEM_SRAM_DRD_CMD_REG(0), SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_V, PSRAM_FAST_READ_QUAD,
                      SPI_MEM_CACHE_SRAM_USR_RD_CMD_VALUE_S); //0x0b
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(0), SPI_MEM_SRAM_RDUMMY_CYCLELEN_V, PSRAM_FAST_READ_QUAD_DUMMY + extra_dummy,
                      SPI_MEM_SRAM_RDUMMY_CYCLELEN_S); //dummy, psram cache :  40m--+1dummy,80m--+2dummy

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_HL | DPORT_PRO_DRAM_SPLIT);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_HL | DPORT_APP_DRAM_SPLIT);
    if (vaddrmode == PSRAM_VADDR_MODE_LOWHIGH) {
        DPORT_SET_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_HL);
        DPORT_SET_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_HL);
    } else if (vaddrmode == PSRAM_VADDR_MODE_EVENODD) {
        DPORT_SET_PERI_REG_MASK(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_DRAM_SPLIT);
        DPORT_SET_PERI_REG_MASK(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_DRAM_SPLIT);
    }
#endif

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

    if ((PSRAM_SIZE_ID(s_psram_id) == PSRAM_EID_SIZE_64MBITS) || PSRAM_IS_64MBIT_TRIAL(s_psram_id)) {
        *out_size_bytes = PSRAM_SIZE_8MB;
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
 * On ESP32S2, all of the PSRAM physical region are available
 */
esp_err_t esp_psram_impl_get_available_size(uint32_t *out_size_bytes)
{
    return esp_psram_impl_get_physical_size(out_size_bytes);
}
