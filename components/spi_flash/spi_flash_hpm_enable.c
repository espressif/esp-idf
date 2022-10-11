/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
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
 ******************************************************************************/

#if CONFIG_ESPTOOLPY_FLASHFREQ_120M
#define FLASH_FREQUENCY 120
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define FLASH_FREQUENCY 80
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
#define FLASH_FREQUENCY 40
#elif CONFIG_ESPTOOLPY_FLASHFREQ_20M
#define FLASH_FREQUENCY 20
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
    switch (flash_id) {
    /* The flash listed here should enter the HPM with command 0xA3 */
    case 0xC84016:
    case 0xC84017:
        // Read BYTE4 in SFDP, 0 means C series, 6 means E series
        uint32_t gd_sfdp = bootloader_flash_read_sfdp(0x4, 1);
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
    // Delay for T(HPM) refering to datasheet.
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

/**
 * @brief Probe the chip whether adjust dummy to enable HPM mode. Take XMC as an example:
 *        Adjust dummy bits to enable HPM mode of the flash. If XMC works under 80MHz, the dummy bits
 *        might be 6, but when works under 120MHz, the dummy bits might be 10.
 */
static esp_err_t spi_flash_hpm_probe_chip_with_dummy(uint32_t flash_id)
{
    esp_err_t ret = ESP_OK;
    switch (flash_id) {
    /* The flash listed here should enter the HPM by adjusting dummy cycles */
    // XMC chips.
    case 0x204017:
    case 0x204018:
        break;
    // GD chips.
    case 0xC84017:
    case 0xC84018:
        // Read BYTE4 in SFDP, 0 means C series, 6 means E series
        uint32_t gd_sfdp = bootloader_flash_read_sfdp(0x4, 1);
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

//-----------------------generic functions-------------------------------------//

/**
 * @brief Default dummy for almost all flash chips. If your flash does't need to reconfigure dummy,
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

const spi_flash_hpm_info_t __attribute__((weak)) spi_flash_hpm_enable_list[] = {
    /* vendor, chip_id, freq_threshold,  temperature threshold,   operation for setting high performance,         reading HPF status,                         get dummy        */
    { "command",    spi_flash_hpm_probe_chip_with_cmd,     spi_flash_hpm_chip_hpm_requirement_check_with_cmd,        spi_flash_enable_high_performance_send_cmd,       spi_flash_high_performance_check_hpf_bit_5,  spi_flash_hpm_get_dummy_generic },
    { "dummy",   spi_flash_hpm_probe_chip_with_dummy,     spi_flash_hpm_chip_hpm_requirement_check_with_dummy,    spi_flash_turn_high_performance_reconfig_dummy,   spi_flash_high_performance_check_dummy_sr,  spi_flash_hpm_get_dummy_xmc},
    { "write sr3-bit5", spi_flash_hpm_probe_chip_with_write_hpf_bit_5, spi_flash_hpm_chip_hpm_requirement_check_with_write_hpf_bit_5, spi_flash_turn_high_performance_write_hpf_bit_5, spi_flash_high_performance_check_hpf_bit_5, spi_flash_hpm_get_dummy_generic},
    // default: do nothing, but keep the dummy get function. The first item with NULL as its probe will be the fallback.
    { "NULL",  NULL,                        NULL,              NULL,                            NULL,                             spi_flash_hpm_get_dummy_generic},
};

static const spi_flash_hpm_info_t *chip_hpm = NULL;
static spi_flash_hpm_dummy_conf_t dummy_conf;
static bool hpm_dummy_changed = false;

esp_err_t spi_flash_enable_high_performance_mode(void)
{
    uint32_t flash_chip_id = g_rom_flashchip.device_id;
    uint32_t flash_freq = FLASH_FREQUENCY;
    spi_flash_requirement_t hpm_requirement_check;
    // voltage and temperature has not been implemented, just leave an interface here. Complete in the future.
    int voltage = 0;
    int temperature = 0;

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

    if (ret != ESP_OK) {
#if (FLASH_FREQUENCY == 120)
            ESP_EARLY_LOGW(HPM_TAG, "Flash high performance mode hasn't been supported");
#endif
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
        hpm_dummy_changed = (hpm_requirement_check == SPI_FLASH_HPM_DUMMY_NEEDED) ? true : false;
    } else if (hpm_requirement_check == SPI_FLASH_HPM_BEYOND_LIMIT) {
        ESP_EARLY_LOGE(HPM_TAG, "Flash does not have the ability to raise to that frequency");
        return ESP_FAIL;
    }
    return ESP_OK;
}

const spi_flash_hpm_dummy_conf_t *spi_flash_hpm_get_dummy(void)
{
    chip_hpm->flash_get_dummy(&dummy_conf);
    return &dummy_conf;
}

bool spi_flash_hpm_dummy_adjust(void)
{
    return hpm_dummy_changed;
}
