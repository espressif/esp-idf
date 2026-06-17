/*
 * Copyright (c) 2006 Uwe Stuehler <uwe@openbsd.org>
 * Adaptations to ESP-IDF Copyright (c) 2016-2024 Espressif Systems (Shanghai) PTE LTD
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "esp_private/sdmmc_common.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#include "sd_pwr_ctrl.h"

static const char* TAG = "sdmmc_init";

#define SDMMC_INIT_STEP(condition, function) \
    do { \
        if ((condition)) { \
            esp_err_t err = (function)(card); \
            if (err != ESP_OK) { \
                ESP_LOGD(TAG, "%s: %s returned 0x%x", __func__, #function, err); \
                return err; \
            } \
        } \
    } while(0);

#define SDMMC_INIT_STEP_PARAM(condition, function, param) \
    do { \
        if ((condition)) { \
            esp_err_t err = (function)(card, param); \
            if (err != ESP_OK) { \
                ESP_LOGD(TAG, "%s: %s returned 0x%x", __func__, #function, err); \
                return err; \
            } \
        } \
    } while(0);

esp_err_t sdmmc_card_init(const sdmmc_host_t* config, sdmmc_card_t* card)
{
    esp_err_t ret = ESP_FAIL;
    memset(card, 0, sizeof(*card));
    memcpy(&card->host, config, sizeof(*config));

    const bool is_spi = host_is_spi(card);
    const bool always = true;
#if !CONFIG_SD_ENABLE_SDIO_SUPPORT
    card->is_mem = 1;
#else
    const bool io_supported = true;
#endif

    if (config->pwr_ctrl_handle) {
        int voltage_mv = config->io_voltage * 1000;
        ret = sd_pwr_ctrl_set_io_voltage(config->pwr_ctrl_handle, voltage_mv);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "failed to set voltage (0x%x)", ret);
            return ret;
        }
    }

    /* Allocate cache-aligned buffer for SDIO over SDMMC.*/
    SDMMC_INIT_STEP(!is_spi, sdmmc_allocate_aligned_buf);

    /* Check if host flags are compatible with slot configuration. */
    SDMMC_INIT_STEP(!is_spi, sdmmc_fix_host_flags);

    /* Check if host function pointers are correctly initialised */
    SDMMC_INIT_STEP(always, sdmmc_check_host_function_ptr_integrity);

#if CONFIG_SD_ENABLE_SDIO_SUPPORT
    /* Reset SDIO (CMD52, RES) before re-initializing IO (CMD5). */
    SDMMC_INIT_STEP(io_supported, sdmmc_io_reset);
#endif

    /* GO_IDLE_STATE (CMD0) command resets the card */
    SDMMC_INIT_STEP(always, sdmmc_send_cmd_go_idle_state);

    /* SEND_IF_COND (CMD8) command is used to identify SDHC/SDXC cards. */
    SDMMC_INIT_STEP(always, sdmmc_init_sd_if_cond);

#if CONFIG_SD_ENABLE_SDIO_SUPPORT
    /* IO_SEND_OP_COND(CMD5), Determine if the card is an IO card. */
    SDMMC_INIT_STEP(io_supported, sdmmc_init_io);
#endif

    const bool is_mem = card->is_mem;
    const bool is_sdio = !is_mem;
    const bool ignore_data_crc = (config->flags & SDMMC_HOST_FLAG_SPI_IGNORE_DATA_CRC);

    if (!ignore_data_crc) {
        /* Enable CRC16 checks for data transfers in SPI mode */
        SDMMC_INIT_STEP(is_spi, sdmmc_init_spi_crc);
    }

    /* Use SEND_OP_COND to set up card OCR */
    SDMMC_INIT_STEP(is_mem, sdmmc_init_ocr);

    const bool is_mmc = is_mem && card->is_mmc;
    const bool is_sdmem = is_mem && !is_mmc;

    ESP_LOGD(TAG, "%s: card type is %s", __func__,
            is_sdio ? "SDIO" : is_mmc ? "MMC" : "SD");

    /* switch to 1.8V if supported (UHS-I) */
    bool is_uhs1 = is_sdmem && (card->ocr & SD_OCR_S18_RA) && (card->ocr & SD_OCR_SDHC_CAP);
    ESP_LOGV(TAG, "is_uhs1: %d", is_uhs1);
    SDMMC_INIT_STEP(is_uhs1, sdmmc_init_sd_uhs1);

    /* Read the contents of CID register*/
    SDMMC_INIT_STEP(is_mem, sdmmc_init_cid);

    /* Assign RCA */
    SDMMC_INIT_STEP(!is_spi, sdmmc_init_rca);

    /* Read and decode the contents of CSD register */
    SDMMC_INIT_STEP(is_mem, sdmmc_init_csd);

    /* Decode the contents of mmc CID register */
    SDMMC_INIT_STEP(is_mmc && !is_spi, sdmmc_init_mmc_decode_cid);

    /* Switch the card from stand-by mode to data transfer mode (not needed if
     * SPI interface is used). This is needed to issue SET_BLOCKLEN and
     * SEND_SCR commands.
     */
    SDMMC_INIT_STEP(!is_spi, sdmmc_init_select_card);

    /* SD memory cards:
     * Set block len for SDSC cards to 512 bytes (same as SDHC)
     * Read SCR
     * Wait to enter data transfer state
     */
    SDMMC_INIT_STEP(is_sdmem, sdmmc_init_sd_blocklen);
    SDMMC_INIT_STEP(is_sdmem, sdmmc_init_sd_scr);
    SDMMC_INIT_STEP(is_sdmem, sdmmc_init_sd_wait_data_ready);

    /* MMC cards: read CXD */
    SDMMC_INIT_STEP(is_mmc, sdmmc_init_mmc_read_ext_csd);

#if CONFIG_SD_ENABLE_SDIO_SUPPORT
    /* SDIO cards: read CCCR card capabilities */
    uint8_t card_cap = 0;
    SDMMC_INIT_STEP_PARAM(is_sdio, sdmmc_io_init_read_card_cap, &card_cap);
#endif

    /* Try to switch card to HS mode if the card supports it.
     * Set card->max_freq_khz value accordingly.
     */
    SDMMC_INIT_STEP(always, sdmmc_init_card_hs_mode);

    /* Set bus width. One call for every kind of card, then one for the host */
    if (!is_spi) {
        SDMMC_INIT_STEP(is_sdmem, sdmmc_init_sd_bus_width);
#if CONFIG_SD_ENABLE_SDIO_SUPPORT
        SDMMC_INIT_STEP(is_sdio, sdmmc_init_io_bus_width);
#endif
        SDMMC_INIT_STEP(is_mmc, sdmmc_init_mmc_bus_width);
        SDMMC_INIT_STEP(always, sdmmc_init_host_bus_width);
    }

    /* Driver Strength */
    SDMMC_INIT_STEP(is_uhs1, sdmmc_init_sd_driver_strength);

    /* Current Limit */
    SDMMC_INIT_STEP(is_uhs1, sdmmc_init_sd_current_limit);

    /* SD card: read SD Status register */
    SDMMC_INIT_STEP(is_sdmem, sdmmc_init_sd_ssr);

    /* Switch to the host to use card->max_freq_khz frequency. */
    SDMMC_INIT_STEP(always, sdmmc_init_host_frequency);

    /* Timing tuning */
    SDMMC_INIT_STEP(is_uhs1, sdmmc_init_sd_timing_tuning);

    /* Sanity check after switching the bus mode and frequency */
    SDMMC_INIT_STEP(is_sdmem, sdmmc_check_scr);
    /* Sanity check after eMMC switch to HS mode */
    SDMMC_INIT_STEP(is_mmc, sdmmc_init_mmc_check_ext_csd);
#if CONFIG_SD_ENABLE_SDIO_SUPPORT
    /* Sanity check for SDIO after switching the frequency */
    SDMMC_INIT_STEP_PARAM(is_sdio, sdmmc_io_init_check_card_cap, &card_cap);
#endif

    return ESP_OK;
}
