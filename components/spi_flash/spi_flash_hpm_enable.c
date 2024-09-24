/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "spi_flash_defs.h"
#include "esp_rom_sys.h"
#include "esp_rom_spiflash.h"
#include "spi_flash_override.h"

// TODO: These dependencies will be removed after remove bootloader_flash to G0.IDF-4609
#include "bootloader_flash_override.h"
#include "bootloader_flash_priv.h"

/*******************************************************************************
 * Flash high speed performance mode.
 * HPM: High performance mode.
 * HPF: High performance flag.
 *
 * Different flash chips might have different high performance strategy.
 * 1. Some flash chips send A3H to enable the HPM.
 * 2. Some flash chips write HPF bit in status register.
 * 3. Some flash chips adjust dummy cycles.
 * 4. Some flash chips do nothing.
 ******************************************************************************/

/*
 * Note: This file should only be compiled when HPM_ON, which is only available when !CONFIG_ESPTOOLPY_OCT_FLASH.
 * However when HPM_ON, there are still some cases this file is not actually used:
 *
 * - !CONFIG_SPI_FLASH_UNDER_HIGH_FREQ:
 *      It mean that the flash not running under frequency requires HPM. spi_flash_enable_high_performance_mode() still
 *      called because caller shouldn't take care of the frequency.
 *
 * - bootloader_flash_is_octal_mode_enabled() == true:
 *      This is possible when `CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT` selected
 *
 *      Octal Flash for now all support 120M. No need to enable HPM. The file is compiled, but will not actually run
 *      into spi_flash_enable_high_performance_mode().
 */

void spi_flash_hpm_get_dummy_generic(spi_flash_hpm_dummy_conf_t *dummy_conf);

#if CONFIG_SPI_FLASH_UNDER_HIGH_FREQ

#if CONFIG_SPI_FLASH_HPM_AUTO
// This only happens on S3, where HPM_AUTO leads to HPM_ON
#warning High Performance Mode (QSPI Flash > 80MHz) is optional feature that depends on flash model. Read Docs First!
#endif

const static char *HPM_TAG = "flash HPM";

// TODO: This function will be changed after remove bootloader_flash to G0.IDF-4609
extern uint32_t bootloader_flash_execute_command_common(
    uint8_t command,
    uint32_t addr_len, uint32_t address,
    uint8_t dummy_len,
    uint8_t mosi_len, uint32_t mosi_data,
    uint8_t miso_len);

extern uint32_t IRAM_ATTR bootloader_flash_read_sfdp(uint32_t sfdp_addr, unsigned int miso_byte_num);

//-----------------For flash chips which enter HPM via command-----------------------//

/**
 * @brief Probe the chip whether use command to enable HPM mode. Take GD as an example:
 *        Some GD send 0xA3 command to enable HPM mode of the flash.
 */
static esp_err_t spi_flash_hpm_probe_chip_with_cmd(uint32_t flash_id)
{
    esp_err_t ret = ESP_OK;
    uint32_t gd_sfdp;

    switch (flash_id) {
    /* The flash listed here should enter the HPM with command 0xA3 */
    case 0xC84016:
    case 0xC84017:
        // Read BYTE4 in SFDP, 0 means C series, 6 means E series
        gd_sfdp = bootloader_flash_read_sfdp(0x4, 1);
        if (gd_sfdp == 0x0) {
            break;
        } else {
            ret = ESP_ERR_NOT_FOUND;
            break;
        }
    default:
        ret = ESP_ERR_NOT_FOUND;
        break;
    }
    return ret;
}

static spi_flash_requirement_t spi_flash_hpm_chip_hpm_requirement_check_with_cmd(uint32_t flash_id, uint32_t freq_mhz, int voltage_mv, int temperautre)
{
    // voltage and temperature are not been used now, to be completed in the future.
    (void)voltage_mv;
    (void)temperautre;
    spi_flash_requirement_t chip_cap = SPI_FLASH_HPM_UNNEEDED;
    if (freq_mhz > 80) {
        chip_cap = SPI_FLASH_HPM_CMD_NEEDED;
    }
    ESP_EARLY_LOGD(HPM_TAG, "HPM with command, status is %d", chip_cap);
    return chip_cap;
}

/**
 * @brief Send HPMEN command (A3H)
 */
static void spi_flash_enable_high_performance_send_cmd(void)
{
    uint32_t dummy = 24;
    bootloader_flash_execute_command_common(CMD_HPMEN, 0, 0, dummy, 0, 0, 0);
    // Delay for T(HPM) referring to datasheet.
    esp_rom_delay_us(20);
}

/**
 * @brief Check whether flash HPM has been enabled. According to flash datasheets, majorities of
 *        HPF bit are at bit-5, sr-3. But some are not. Therefore, this function is only used for those
 *        HPF bit is at bit-5, sr-3.
 */
static esp_err_t spi_flash_high_performance_check_hpf_bit_5(void)
{
    if((bootloader_read_status_8b_rdsr3() & (1 << 4)) == 0) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

//-----------------For flash chips which enter HPM via adjust dummy-----------------------//

#if CONFIG_SPI_FLASH_HPM_DC_ON
/**
 * @brief Probe the chip whether adjust dummy to enable HPM mode. Take XMC as an example:
 *        Adjust dummy bits to enable HPM mode of the flash. If XMC works under 80MHz, the dummy bits
 *        might be 6, but when works under 120MHz, the dummy bits might be 10.
 */
static esp_err_t spi_flash_hpm_probe_chip_with_dummy(uint32_t flash_id)
{
    esp_err_t ret = ESP_OK;
    uint32_t gd_sfdp;

    switch (flash_id) {
    /* The flash listed here should enter the HPM by adjusting dummy cycles */
    // XMC chips.
    case 0x204017:
    case 0x204018:
    case 0x464017:
    case 0x464018:
        break;
    // GD chips.
    case 0xC84017:
    case 0xC84018:
        // Read BYTE4 in SFDP, 0 means C series, 6 means E series
        gd_sfdp = bootloader_flash_read_sfdp(0x4, 1);
        if (gd_sfdp == 0x6) {
            break;
        } else {
            ret = ESP_ERR_NOT_FOUND;
            break;
        }
    default:
        ret = ESP_ERR_NOT_FOUND;
        break;
    }
    return ret;
}

static spi_flash_requirement_t spi_flash_hpm_chip_hpm_requirement_check_with_dummy(uint32_t flash_id, uint32_t freq_mhz, int voltage_mv, int temperautre)
{
    // voltage and temperature are not been used now, to be completed in the future.
    (void)voltage_mv;
    (void)temperautre;
    spi_flash_requirement_t chip_cap = SPI_FLASH_HPM_UNNEEDED;

    if (freq_mhz >= 104) {
        chip_cap = SPI_FLASH_HPM_DUMMY_NEEDED;
    }
    ESP_EARLY_LOGD(HPM_TAG, "HPM with dummy, status is %d", chip_cap);
    return chip_cap;
}

/**
 * @brief Adjust dummy cycles. This function modifies the Dummy Cycle Bits in SR3.
 *        Usually, the bits are at bit-0, bit-1, sr-3 and set DC[1:0]=[1,1].
 *
 * @note Don't forget to adjust dummy configurations for MSPI, you can get the
 *       correct dummy from interface `spi_flash_hpm_get_dummy`.
 */
static void spi_flash_turn_high_performance_reconfig_dummy(void)
{
    uint8_t old_status_3 = bootloader_read_status_8b_rdsr3();
    uint8_t new_status = (old_status_3 | 0x03);
    bootloader_execute_flash_command(CMD_WRENVSR, 0, 0, 0);
    bootloader_write_status_8b_wrsr3(new_status);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
}

/**
 * @brief Check whether HPM has been enabled. This function checks the DC bits
 */
static esp_err_t spi_flash_high_performance_check_dummy_sr(void)
{
    if((bootloader_read_status_8b_rdsr3() & 0x03) == 0) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void spi_flash_hpm_get_dummy_xmc(spi_flash_hpm_dummy_conf_t *dummy_conf)
{
    dummy_conf->dio_dummy = SPI_FLASH_DIO_HPM_DUMMY_BITLEN;
    dummy_conf->dout_dummy = SPI_FLASH_DOUT_DUMMY_BITLEN;
    dummy_conf->qio_dummy = SPI_FLASH_QIO_HPM_DUMMY_BITLEN;
    dummy_conf->qout_dummy = SPI_FLASH_QOUT_DUMMY_BITLEN;
    dummy_conf->fastrd_dummy = SPI_FLASH_FASTRD_DUMMY_BITLEN;
}
#elif !CONFIG_SPI_FLASH_HPM_DC_DISABLE

//This is because bootloader doesn't support this
#warning HPM-DC, which helps to run some flash > 80MHz by adjusting dummy cycles, is no longer enabled by default.
#warning To enable this feature, your bootloader needs to have the support for it (by explicitly selecting BOOTLOADER_FLASH_DC_AWARE).
#warning If your bootloader does not support it, select SPI_FLASH_HPM_DC_DISABLE to suppress the warning. READ DOCS FIRST!

#endif //CONFIG_SPI_FLASH_HPM_DC_ON

//-----------------For flash chips which enter HPM via write status register-----------------------//

/**
 * @brief Probe the chip whether to write status register to enable HPM mode. Take ZB as an example:
 *        Write status register bits to enable HPM mode of the flash. If ZB works under 80MHz, the register value
 *        would be 0, but when works under 120MHz, the register value would be 1.
 */
static esp_err_t spi_flash_hpm_probe_chip_with_write_hpf_bit_5(uint32_t flash_id)
{
    esp_err_t ret = ESP_OK;
    switch (flash_id) {
    /* The flash listed here should enter the HPM by adjusting dummy cycles */
    // ZB chips.
    case 0x5E4016:
        break;
    default:
        ret = ESP_ERR_NOT_FOUND;
        break;
    }
    return ret;
}

static spi_flash_requirement_t spi_flash_hpm_chip_hpm_requirement_check_with_write_hpf_bit_5(uint32_t flash_id, uint32_t freq_mhz, int voltage_mv, int temperautre)
{
    // voltage and temperature are not been used now, to be completed in the future.
    (void)voltage_mv;
    (void)temperautre;
    spi_flash_requirement_t chip_cap = SPI_FLASH_HPM_UNNEEDED;

    if (freq_mhz >= 104) {
        chip_cap = SPI_FLASH_HPM_WRITE_SR_NEEDED;
    }
    ESP_EARLY_LOGD(HPM_TAG, "HPM with dummy, status is %d", chip_cap);
    return chip_cap;
}

/**
 * @brief Write bit 5 in status 3
 */
static void spi_flash_turn_high_performance_write_hpf_bit_5(void)
{
    uint8_t old_status_3 = bootloader_read_status_8b_rdsr3();
    uint8_t new_status = (old_status_3 | 0x10);
    bootloader_execute_flash_command(CMD_WRENVSR, 0, 0, 0);
    bootloader_write_status_8b_wrsr3(new_status);
    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
}

//-----------------For flash chips which enter HPM with doing nothing-----------------------//

/**
 * @brief Probe the chip whether to write status register to enable HPM mode. Take a GD chip as an example:
 * This chip (GD25LQ255E) supports maximum frequency to 133MHz by default. So, we don't need to do any extra
 * thing.
 */
static esp_err_t spi_flash_hpm_probe_chip_with_doing_nothing(uint32_t flash_id)
{
    esp_err_t ret = ESP_OK;
    switch (flash_id) {
    /* The flash listed here should enter the HPM by doing nothing */
    // GD25LQ255E.
    case 0xC86019:
        break;
    default:
        ret = ESP_ERR_NOT_FOUND;
        break;
    }
    return ret;
}

static spi_flash_requirement_t spi_flash_hpm_chip_hpm_requirement_check_with_doing_nothing(uint32_t flash_id, uint32_t freq_mhz, int voltage_mv, int temperautre)
{
    // voltage and temperature are not been used now, to be completed in the future.
    (void)voltage_mv;
    (void)temperautre;
    spi_flash_requirement_t chip_cap = SPI_FLASH_HPM_UNNEEDED;
    ESP_EARLY_LOGD(HPM_TAG, "HPM by default, chip caps is %d", chip_cap);
    return chip_cap;
}

const spi_flash_hpm_info_t __attribute__((weak)) spi_flash_hpm_enable_list[] = {
    /* vendor, chip_id, freq_threshold,  temperature threshold,   operation for setting high performance,         reading HPF status,                         get dummy        */
    { "command",    spi_flash_hpm_probe_chip_with_cmd,     spi_flash_hpm_chip_hpm_requirement_check_with_cmd,        spi_flash_enable_high_performance_send_cmd,       spi_flash_high_performance_check_hpf_bit_5,  spi_flash_hpm_get_dummy_generic },
#if CONFIG_SPI_FLASH_HPM_DC_ON
    { "dummy",   spi_flash_hpm_probe_chip_with_dummy,     spi_flash_hpm_chip_hpm_requirement_check_with_dummy,    spi_flash_turn_high_performance_reconfig_dummy,   spi_flash_high_performance_check_dummy_sr,  spi_flash_hpm_get_dummy_xmc},
#endif //CONFIG_SPI_FLASH_HPM_DC_ON
    { "write sr3-bit5", spi_flash_hpm_probe_chip_with_write_hpf_bit_5, spi_flash_hpm_chip_hpm_requirement_check_with_write_hpf_bit_5, spi_flash_turn_high_performance_write_hpf_bit_5, spi_flash_high_performance_check_hpf_bit_5, spi_flash_hpm_get_dummy_generic},
    { "noting-to-do", spi_flash_hpm_probe_chip_with_doing_nothing, spi_flash_hpm_chip_hpm_requirement_check_with_doing_nothing, NULL, NULL, spi_flash_hpm_get_dummy_generic},
    // default: do nothing, but keep the dummy get function. The first item with NULL as its probe will be the fallback.
    { "NULL",  NULL,                        NULL,              NULL,                            NULL,                             spi_flash_hpm_get_dummy_generic},
};

static const spi_flash_hpm_info_t *chip_hpm = NULL;

#if CONFIG_SPI_FLASH_HPM_DC_ON
static bool s_hpm_dummy_changed = false;
static spi_flash_hpm_dummy_conf_t s_dummy_conf;

const spi_flash_hpm_dummy_conf_t *spi_flash_hpm_get_dummy(void)
{
    chip_hpm->flash_get_dummy(&s_dummy_conf);
    return &s_dummy_conf;
}

bool spi_flash_hpm_dummy_adjust(void)
{
    return s_hpm_dummy_changed;
}
#endif //CONFIG_SPI_FLASH_HPM_DC_ON

#if CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define FLASH_FREQUENCY 120
#endif

esp_err_t spi_flash_enable_high_performance_mode(void)
{
    uint32_t flash_chip_id = g_rom_flashchip.device_id;
    uint32_t flash_freq = FLASH_FREQUENCY;
    spi_flash_requirement_t hpm_requirement_check;
    // voltage and temperature has not been implemented, just leave an interface here. Complete in the future.
    int voltage = 0;
    int temperature = 0;

#if CONFIG_SPI_FLASH_HPM_AUTO
    ESP_EARLY_LOGW(HPM_TAG, "HPM mode is optional feature that depends on flash model. Read Docs First!");
#endif

#if CONFIG_SPI_FLASH_HPM_DC_DISABLE
    // case 1: force disabled
    ESP_EARLY_LOGI(HPM_TAG, "w/o HPM-DC support");
#elif CONFIG_SPI_FLASH_HPM_DC_ON
    // case 2: auto, and actually enabled
    ESP_EARLY_LOGI(HPM_TAG, "with HPM-DC support");
#else
    // case 3: auto, but disabled (not supported by bootloader)
    ESP_EARLY_LOGW(HPM_TAG, "HPM mode with DC adjustment is disabled. Some flash models may not be supported. Read Docs First!");
#endif

    const spi_flash_hpm_info_t *chip = spi_flash_hpm_enable_list;
    esp_err_t ret = ESP_OK;
    while (chip->probe) {
        ret = chip->probe(flash_chip_id);
        if (ret == ESP_OK) {
            break;
        }
        chip++;
    }
    chip_hpm = chip;

    /* When > 80 MHz, flash chips usually need special HPM support to run normally. The support is chip-specific. When
     * the chip is not in the known flash list, nothing will be done and there will be an warning.
     * When <= 80 MHz, it's assumed that all flash chips can run without chip-specific HPM support. This function will not be called and there will be no warning.
     */
    if (ret != ESP_OK) {
        ESP_EARLY_LOGW(HPM_TAG, "High performance mode of this flash model hasn't been supported.");
        return ret;
    }

    hpm_requirement_check = chip_hpm->chip_hpm_requirement_check(flash_chip_id, flash_freq, voltage, temperature);
    if ((hpm_requirement_check == SPI_FLASH_HPM_CMD_NEEDED) || (hpm_requirement_check == SPI_FLASH_HPM_DUMMY_NEEDED) || (hpm_requirement_check == SPI_FLASH_HPM_WRITE_SR_NEEDED)) {
        ESP_EARLY_LOGI(HPM_TAG, "Enabling flash high speed mode by %s", chip_hpm->method);
        chip_hpm->flash_hpm_enable();
        ESP_EARLY_LOGD(HPM_TAG, "Checking whether HPM has been executed");

        if (chip_hpm->flash_hpf_check() != ESP_OK) {
            ESP_EARLY_LOGE(HPM_TAG, "Flash high performance mode hasn't been executed successfully");
            return ESP_FAIL;
        }
#if CONFIG_SPI_FLASH_HPM_DC_ON
        s_hpm_dummy_changed = (hpm_requirement_check == SPI_FLASH_HPM_DUMMY_NEEDED) ? true : false;
#else
        assert(hpm_requirement_check != SPI_FLASH_HPM_DUMMY_NEEDED);
#endif
    } else if (hpm_requirement_check == SPI_FLASH_HPM_BEYOND_LIMIT) {
        ESP_EARLY_LOGE(HPM_TAG, "Flash does not have the ability to raise to that frequency");
        return ESP_FAIL;
    }
    return ESP_OK;
}
#else
//!CONFIG_SPI_FLASH_UNDER_HIGH_FREQ

static spi_flash_hpm_dummy_conf_t s_dummy_conf;

esp_err_t spi_flash_enable_high_performance_mode(void)
{
    return ESP_OK;
}

const spi_flash_hpm_dummy_conf_t *spi_flash_hpm_get_dummy(void)
{
    spi_flash_hpm_get_dummy_generic(&s_dummy_conf);
    return &s_dummy_conf;
}

bool spi_flash_hpm_dummy_adjust(void)
{
    return false;
}
#endif //CONFIG_SPI_FLASH_UNDER_HIGH_FREQ

//-----------------------generic functions-------------------------------------//

/**
 * @brief Default dummy for almost all flash chips. If your flash doesn't need to reconfigure dummy,
 *        just call this function.
 */
void __attribute__((weak)) spi_flash_hpm_get_dummy_generic(spi_flash_hpm_dummy_conf_t *dummy_conf)
{
    dummy_conf->dio_dummy = SPI_FLASH_DIO_DUMMY_BITLEN;
    dummy_conf->dout_dummy = SPI_FLASH_DOUT_DUMMY_BITLEN;
    dummy_conf->qio_dummy = SPI_FLASH_QIO_DUMMY_BITLEN;
    dummy_conf->qout_dummy = SPI_FLASH_QOUT_DUMMY_BITLEN;
    dummy_conf->fastrd_dummy = SPI_FLASH_FASTRD_DUMMY_BITLEN;
}
