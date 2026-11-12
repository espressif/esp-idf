/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"
#include "sd_protocol_defs.h"
#include "driver/sdspi_host.h"
#include "sdspi_private.h"
#include "sdspi_crc.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/spi_common_internal.h"

/// Max number of transactions in flight (used in start_command_write_blocks)
#define SDSPI_TRANSACTION_COUNT 4
#define SDSPI_MOSI_IDLE_VAL     0xff    //!< Data value which causes MOSI to stay high
#define GPIO_UNUSED 0xff                //!< Flag indicating that CD/WP is unused
/// Minimum size of the slot buffer: one data block, its CRC, and 2 bytes to scan for the next start token
#define SDSPI_BLOCK_BUF_SIZE    (SDSPI_MAX_DATA_LEN + 4)
/// Maximum number of dummy bytes between the request and response (minimum is 1)
#define SDSPI_RESPONSE_MAX_DELAY  8
/// Number of bytes read per poll while waiting for a data start token
#define SDSPI_TOKEN_POLL_SIZE   8
#define SDSPI_ALIGN_UP(n, a)    (((n) + (a) - 1) / (a) * (a))

/**
 * @brief Structure containing run time configuration for a single SD slot
 *
 * The slot info is referenced to by an sdspi_dev_handle_t (alias int). The handle may be the raw
 * pointer to the slot info itself (force converted to, new API in IDFv4.2), or the index of the
 * s_slot array (deprecated API). Returning the raw pointer to the caller instead of storing it
 * locally can save some static memory.
 */
typedef struct {
    spi_host_device_t   host_id; //!< SPI host id.
    spi_device_handle_t spi_handle; //!< SPI device handle, used for transactions
    /// DMA buffer for every receive and for transmitting non-DMA-capable data,
    /// allocated at init, aligned to and a multiple of rx_align, block_buf_size bytes.
    /// Receiving only into this buffer, with lengths padded to rx_len_align, means
    /// spi_master never has to allocate a temporary DMA buffer for a transaction.
    uint8_t* block_buf;
    size_t block_buf_size;
    size_t rx_align;              //!< RX DMA address alignment required by the SPI bus
    size_t rx_len_align;          //!< RX DMA length alignment required by the SPI bus (1 if none)
    /// semaphore of gpio interrupt
    SemaphoreHandle_t   semphr_int;
    uint16_t duty_cycle_pos;  ///< Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
    uint8_t gpio_cs;            //!< CS GPIO, or GPIO_UNUSED
    uint8_t gpio_cd;            //!< Card detect GPIO, or GPIO_UNUSED
    uint8_t gpio_wp;            //!< Write protect GPIO, or GPIO_UNUSED
    uint8_t gpio_int;           //!< Write protect GPIO, or GPIO_UNUSED
    uint8_t poll_busy_start_command_timeout;  ///< Timeout value in milliseconds the driver will be waiting for MISO to be high before sending commands.
    /// GPIO write protect polarity.
    /// 0 means "active low", i.e. card is protected when the GPIO is low;
    /// 1 means "active high", i.e. card is protected when GPIO is high.
    uint8_t gpio_wp_polarity : 1;
    /// Set to 1 if the higher layer has asked the card to enable CRC checks
    uint8_t data_crc_enabled : 1;
} slot_info_t;

// Reserved for old API to be back-compatible
static slot_info_t *s_slots[SOC_SPI_PERIPH_NUM] = {};
static const char *TAG = "sdspi_host";

static const bool use_polling = true;
static const bool no_use_polling = true;

/// Functions to send out different kinds of commands
static esp_err_t start_command_read_blocks(slot_info_t *slot, sdspi_hw_cmd_t *cmd,
                                           uint8_t *data, uint32_t rx_length, bool need_stop_command,
                                           size_t response_extra);

static esp_err_t start_command_write_blocks(slot_info_t *slot, sdspi_hw_cmd_t *cmd,
                                            const uint8_t *data, uint32_t tx_length, bool multi_block, bool stop_trans);

static esp_err_t start_command_default(slot_info_t *slot, int flags, sdspi_hw_cmd_t *cmd);

static esp_err_t shift_cmd_response(sdspi_hw_cmd_t *cmd, int sent_bytes);

static esp_err_t poll_busy(slot_info_t *slot, int timeout_ms, bool polling);

/// A few helper functions

/// Map handle to pointer of slot information
static slot_info_t* get_slot_info(sdspi_dev_handle_t handle)
{
    if ((uint32_t) handle < SOC_SPI_PERIPH_NUM) {
        return s_slots[handle];
    } else {
        return (slot_info_t *) handle;
    }
}

/// Store slot information (if possible) and return corresponding handle
static sdspi_dev_handle_t store_slot_info(slot_info_t *slot)
{
    /*
     * To be back-compatible, the first device of each bus will always be stored locally, and
     * referenced to by the handle `host_id`, otherwise the new API return the raw pointer to the
     * slot info as the handle, to save some static memory.
     */
    if (s_slots[slot->host_id] == NULL) {
        s_slots[slot->host_id] = slot;
        return slot->host_id;
    } else {
        return (sdspi_dev_handle_t)slot;
    }
}

/// Get the slot info for a specific handle, and remove the local reference (if exist).
static slot_info_t* remove_slot_info(sdspi_dev_handle_t handle)
{
    if ((uint32_t) handle < SOC_SPI_PERIPH_NUM) {
        slot_info_t* slot = s_slots[handle];
        s_slots[handle] = NULL;
        return slot;
    } else {
        return (slot_info_t *) handle;
    }
}

/// Set CS high for given slot
static void cs_high(slot_info_t *slot)
{
    if (slot->gpio_cs != GPIO_UNUSED) {
        gpio_set_level(slot->gpio_cs, 1);
    }
}

/// Set CS low for given slot
static void cs_low(slot_info_t *slot)
{
    if (slot->gpio_cs != GPIO_UNUSED) {
        gpio_set_level(slot->gpio_cs, 0);
    }
}

/// Return true if WP pin is configured and is set as per its polarity
static bool card_write_protected(slot_info_t *slot)
{
    if (slot->gpio_wp == GPIO_UNUSED) {
        return false;
    }
    return gpio_get_level(slot->gpio_wp) == (slot->gpio_wp_polarity ? 1 : 0);
}

/// Return true if CD pin is configured and is high
static bool card_missing(slot_info_t *slot)
{
    if (slot->gpio_cd == GPIO_UNUSED) {
        return false;
    }
    return gpio_get_level(slot->gpio_cd) == 1;
}

/// RX DMA address and length alignment that spi_master applies to internal
/// RAM on this bus (see spicommon_dma_setup_priv_buffer() in spi_common.c):
/// the address must be a multiple of the larger of the DMA and cache line
/// alignment, and the length only when the cache line alignment is > 1.
/// Both are 1 when the bus has no DMA.
static void get_rx_dma_alignment(spi_host_device_t host_id, size_t *addr_align, size_t *len_align)
{
    *addr_align = 1;
    *len_align = 1;
    const spi_bus_attr_t *bus_attr = spi_bus_get_attr(host_id);
    if (bus_attr && bus_attr->dma_enabled) {
        const spi_dma_ctx_t *dma_ctx = spi_bus_get_dma_ctx(host_id);
        if (dma_ctx) {
            *addr_align = MAX(*addr_align, dma_ctx->dma_align_rx_int);
        }
        *addr_align = MAX(*addr_align, bus_attr->cache_align_int);
        if (bus_attr->cache_align_int > 1) {
            *len_align = *addr_align;
        }
    }
}

/// Allocate the slot's DMA buffer, sized and aligned for the bus RX DMA alignment
static esp_err_t alloc_block_buf(slot_info_t *slot)
{
    get_rx_dma_alignment(slot->host_id, &slot->rx_align, &slot->rx_len_align);
    slot->block_buf_size = SDSPI_ALIGN_UP(SDSPI_BLOCK_BUF_SIZE, slot->rx_align);
    slot->block_buf = heap_caps_aligned_alloc(MAX(slot->rx_align, 4), slot->block_buf_size,
                                              MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (slot->block_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGD(TAG, "%s: RX DMA alignment %u, length alignment %u, buffer %u bytes", __func__,
             (unsigned)slot->rx_align, (unsigned)slot->rx_len_align, (unsigned)slot->block_buf_size);
    return ESP_OK;
}

/**
 * Full-duplex transfer of padded bytes (>= len) through the slot buffer.  The
 * first len bytes sent are copied from tx, or are 0xff if tx is NULL; the rest
 * are 0xff, so the card sees only extra idle clocks.  On success *rx points to
 * the received bytes in the slot buffer and *rx_len is padded.  The data is
 * valid until the next transfer.
 */
static esp_err_t transfer(slot_info_t *slot, const void *tx, size_t len, size_t padded, bool polling,
                          uint8_t **rx, size_t *rx_len)
{
    assert(len <= padded && padded <= slot->block_buf_size);
    uint8_t *buf = slot->block_buf;
    size_t tx_len = 0;
    if (tx) {
        memcpy(buf, tx, len);
        tx_len = len;
    }
    memset(buf + tx_len, SDSPI_MOSI_IDLE_VAL, padded - tx_len);
    spi_transaction_t t = {
        .length = padded * 8,
        .tx_buffer = buf,
        .rx_buffer = buf,
    };
    esp_err_t ret;
    if (polling) {
        ret = spi_device_polling_transmit(slot->spi_handle, &t);
    } else {
        ret = spi_device_transmit(slot->spi_handle, &t);
    }
    *rx = buf;
    *rx_len = padded;
    return ret;
}

/// transfer(), padded to the RX DMA length alignment so spi_master needs no temporary buffer
static esp_err_t rx_transfer(slot_info_t *slot, const void *tx, size_t len, bool polling,
                             uint8_t **rx, size_t *rx_len)
{
    return transfer(slot, tx, len, SDSPI_ALIGN_UP(len, slot->rx_len_align), polling, rx, rx_len);
}

/// Clock out one byte (CS has to be high) to make the card release MISO
/// (clocking one bit would work as well, but that triggers a bug in SPI DMA)
static void release_bus(slot_info_t *slot)
{
    // TX only: the received byte is not used, and where spi_master checks the
    // RX DMA length a 1-byte RX would need a bounce buffer on every command.
    spi_transaction_t t = {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0xff}
    };
    spi_device_polling_transmit(slot->spi_handle, &t);
    // don't care if this failed
}

/// Clock out 80 cycles (10 bytes) before GO_IDLE command
static void go_idle_clockout(slot_info_t *slot)
{
    // 12 bytes for 10 sent: without DMA, the TX FIFO is filled a 32-bit word at a time
    uint8_t data[12];
    memset(data, 0xff, sizeof(data));
    // TX only: the received bytes are not used, and a 10-byte RX would not
    // meet the RX DMA length alignment.
    spi_transaction_t t = {
        .length = 10 * 8,
        .tx_buffer = data,
    };
    spi_device_polling_transmit(slot->spi_handle, &t);
    // don't care if this failed
}

/**
 * (Re)Configure SPI device. Used to change clock speed.
 * @param slot Pointer to the slot to be configured
 * @param clock_speed_hz  clock speed, Hz
 * @return ESP_OK on success
 */
static esp_err_t configure_spi_dev(slot_info_t *slot, int clock_speed_hz)
{
    if (slot->spi_handle) {
        // Reinitializing
        spi_bus_remove_device(slot->spi_handle);
        slot->spi_handle = NULL;
    }
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = clock_speed_hz,
        .mode = 0,
        // For SD cards, CS must stay low during the whole read/write operation,
        // rather than a single SPI transaction.
        .spics_io_num = GPIO_NUM_NC,
        .queue_size = SDSPI_TRANSACTION_COUNT,
        .duty_cycle_pos = slot->duty_cycle_pos,
    };
    return spi_bus_add_device(slot->host_id, &devcfg, &slot->spi_handle);
}

esp_err_t sdspi_host_init(void)
{
    return ESP_OK;
}

static esp_err_t deinit_slot(slot_info_t *slot)
{
    esp_err_t err = ESP_OK;
    if (slot->spi_handle) {
        spi_bus_remove_device(slot->spi_handle);
        slot->spi_handle = NULL;
    }
    // Allocated at init, so free it even if a reconfiguration left no device
    free(slot->block_buf);
    slot->block_buf = NULL;

    uint64_t pin_bit_mask = 0;
    if (slot->gpio_cs != GPIO_UNUSED) {
        pin_bit_mask |= BIT64(slot->gpio_cs);
    }
    if (slot->gpio_cd != GPIO_UNUSED) {
        pin_bit_mask |= BIT64(slot->gpio_cd);
    }
    if (slot->gpio_wp != GPIO_UNUSED) {
        pin_bit_mask |= BIT64(slot->gpio_wp);
    }
    if (slot->gpio_int != GPIO_UNUSED) {
        pin_bit_mask |= BIT64(slot->gpio_int);
        gpio_intr_disable(slot->gpio_int);
        gpio_isr_handler_remove(slot->gpio_int);
    }
    gpio_config_t config = {
        .pin_bit_mask = pin_bit_mask,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };
    if (pin_bit_mask != 0) {
        gpio_config(&config);
    }

    if (slot->semphr_int) {
        vSemaphoreDelete(slot->semphr_int);
        slot->semphr_int = NULL;
    }
    free(slot);
    return err;
}

esp_err_t sdspi_host_remove_device(sdspi_dev_handle_t handle)
{
    //Get the slot info and remove the reference in the static memory (if used)
    slot_info_t* slot = remove_slot_info(handle);
    if (slot == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    deinit_slot(slot);
    return ESP_OK;
}

//only the slots locally stored can be deinit in this function.
esp_err_t sdspi_host_deinit(void)
{
    for (size_t i = 0; i < sizeof(s_slots) / sizeof(s_slots[0]); ++i) {
        slot_info_t* slot = remove_slot_info(i);
        //slot isn't used, skip
        if (slot == NULL) {
            continue;
        }

        deinit_slot(slot);
    }
    return ESP_OK;
}

esp_err_t sdspi_host_set_card_clk(sdspi_dev_handle_t handle, uint32_t freq_khz)
{
    slot_info_t *slot = get_slot_info(handle);
    if (slot == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_LOGD(TAG, "Setting card clock to %"PRIu32" kHz", freq_khz);
    return configure_spi_dev(slot, freq_khz * 1000);
}

esp_err_t sdspi_host_get_real_freq(sdspi_dev_handle_t handle, int* real_freq_khz)
{
    slot_info_t *slot = get_slot_info(handle);
    if (slot == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return spi_device_get_actual_freq(slot->spi_handle, real_freq_khz);
}

static void gpio_intr(void* arg)
{
    BaseType_t awoken = pdFALSE;
    slot_info_t* slot = (slot_info_t*)arg;
    xSemaphoreGiveFromISR(slot->semphr_int, &awoken);
    gpio_intr_disable(slot->gpio_int);
    if (awoken) {
        portYIELD_FROM_ISR();
    }
}

static inline int wait_for_miso_to_poll_busy_timeout_ms(int8_t wait_for_miso)
{
    static int default_timeout = 40; // default timeout in ms
    int ret;
    switch (wait_for_miso) {
    case -1:
        ret = 0; // no waiting
        break;
    case 0:
        ret = default_timeout;
        break;
    case 1 ... 127:
        ret = (int) wait_for_miso; // timeout in ms
        break;
    default:
        ret = default_timeout; // unsupported values (from -128 to -2), use default
        break;
    }
    assert(ret >= 0);
    return ret;
}

esp_err_t sdspi_host_init_device(const sdspi_device_config_t* slot_config, sdspi_dev_handle_t* out_handle)
{
    ESP_LOGD(TAG, "%s: SPI%d cs=%d cd=%d wp=%d wp_polarity:%d",
             __func__, slot_config->host_id + 1, slot_config->gpio_cs,
             slot_config->gpio_cd, slot_config->gpio_wp, slot_config->gpio_wp_polarity);

    slot_info_t* slot = (slot_info_t*)malloc(sizeof(slot_info_t));
    if (slot == NULL) {
        return ESP_ERR_NO_MEM;
    }
    *slot = (slot_info_t) {
        .host_id = slot_config->host_id,
        .gpio_cs = slot_config->gpio_cs,
        .duty_cycle_pos = slot_config->duty_cycle_pos,
        .poll_busy_start_command_timeout = wait_for_miso_to_poll_busy_timeout_ms(slot_config->wait_for_miso),
    };

    // Attach the SD card to the SPI bus
    esp_err_t ret = configure_spi_dev(slot, SDMMC_FREQ_PROBING * 1000);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "spi_bus_add_device failed with rc=0x%x", ret);
        goto cleanup;
    }

    ret = alloc_block_buf(slot);
    if (ret != ESP_OK) {
        goto cleanup;
    }

    // Configure CS pin
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << slot_config->gpio_cs,
    };
    if (slot_config->gpio_cs != SDSPI_SLOT_NO_CS) {
        slot->gpio_cs = slot_config->gpio_cs;
    } else {
        slot->gpio_cs = GPIO_UNUSED;
    }

    if (slot->gpio_cs != GPIO_UNUSED) {
        ret = gpio_config(&io_conf);
        if (ret != ESP_OK) {
            ESP_LOGD(TAG, "gpio_config (CS) failed with rc=0x%x", ret);
            goto cleanup;
        }
        cs_high(slot);
    }

    // Configure CD and WP pins
    io_conf = (gpio_config_t) {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = 0,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    if (slot_config->gpio_cd != SDSPI_SLOT_NO_CD) {
        io_conf.pin_bit_mask |= (1ULL << slot_config->gpio_cd);
        slot->gpio_cd = slot_config->gpio_cd;
    } else {
        slot->gpio_cd = GPIO_UNUSED;
    }

    if (slot_config->gpio_wp != SDSPI_SLOT_NO_WP) {
        io_conf.pin_bit_mask |= (1ULL << slot_config->gpio_wp);
        slot->gpio_wp = slot_config->gpio_wp;
        slot->gpio_wp_polarity = slot_config->gpio_wp_polarity;
        if (slot->gpio_wp_polarity) {
            io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
            io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        }
    } else {
        slot->gpio_wp = GPIO_UNUSED;
    }

    if (io_conf.pin_bit_mask != 0) {
        ret = gpio_config(&io_conf);
        if (ret != ESP_OK) {
            ESP_LOGD(TAG, "gpio_config (CD/WP) failed with rc=0x%x", ret);
            goto cleanup;
        }
    }

    if (slot_config->gpio_int != SDSPI_SLOT_NO_INT) {
        slot->gpio_int = slot_config->gpio_int;
        io_conf = (gpio_config_t) {
            .intr_type = GPIO_INTR_LOW_LEVEL,
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pin_bit_mask = (1ULL << slot_config->gpio_int),
        };
        ret = gpio_config(&io_conf);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "gpio_config (interrupt) failed with rc=0x%x", ret);
            goto cleanup;
        }

        slot->semphr_int = xSemaphoreCreateBinary();
        if (slot->semphr_int == NULL) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }

        gpio_intr_disable(slot->gpio_int);
        // 1. the interrupt is better to be disabled before the ISR is registered
        // 2. the semaphore MUST be initialized before the ISR is registered
        // 3. the gpio_int member should be filled before the ISR is registered
        ret = gpio_isr_handler_add(slot->gpio_int, &gpio_intr, slot);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "gpio_isr_handle_add failed with rc=0x%x", ret);
            goto cleanup;
        }
    } else {
        slot->gpio_int = GPIO_UNUSED;
    }
    //Initialization finished, store the store information if possible
    //Then return corresponding handle
    *out_handle = store_slot_info(slot);
    return ESP_OK;
cleanup:
    if (slot->semphr_int) {
        vSemaphoreDelete(slot->semphr_int);
        slot->semphr_int = NULL;
    }
    if (slot->spi_handle) {
        spi_bus_remove_device(slot->spi_handle);
        slot->spi_handle = NULL;
    }
    free(slot->block_buf);
    free(slot);
    return ret;
}

esp_err_t sdspi_host_start_command(sdspi_dev_handle_t handle, sdspi_hw_cmd_t *cmd, void *data,
                                   uint32_t data_size, int flags)
{
    slot_info_t *slot = get_slot_info(handle);
    if (slot == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (card_missing(slot)) {
        return ESP_ERR_NOT_FOUND;
    }
    // save some parts of cmd, as its contents will be overwritten
    int cmd_index = cmd->cmd_index;
    uint32_t cmd_arg;
    memcpy(&cmd_arg, cmd->arguments, sizeof(cmd_arg));
    cmd_arg = __builtin_bswap32(cmd_arg);
    ESP_LOGV(TAG, "%s: slot=%i, CMD%d, arg=0x%08"PRIx32" flags=0x%x, data=%p, data_size=%"PRIu32" crc=0x%02x",
             __func__, handle, cmd_index, cmd_arg, flags, data, data_size, cmd->crc7);

    esp_err_t ret;
    ret = spi_device_acquire_bus(slot->spi_handle, portMAX_DELAY);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "%s: spi_device_acquire_bus returned 0x%x", __func__, ret);
        return ret;
    }

    ret = poll_busy(slot, slot->poll_busy_start_command_timeout, true);
    if (ret != ESP_OK && ret != ESP_ERR_TIMEOUT) {
        ESP_LOGD(TAG, "%s: poll_busy error=0x%x", __func__, ret);
        cs_high(slot);
        release_bus(slot);
        spi_device_release_bus(slot->spi_handle);
        return ret;
    }

    // For CMD0, clock out 80 cycles to help the card enter idle state,
    // *before* CS is asserted.
    if (cmd_index == MMC_GO_IDLE_STATE) {
        go_idle_clockout(slot);
    }
    // actual transaction
    ret = ESP_OK;

    cs_low(slot);
    if (flags & SDSPI_CMD_FLAG_DATA) {
        const bool multi_block = flags & SDSPI_CMD_FLAG_MULTI_BLK;
        //send stop transmission token only when multi-block write and non-SDIO mode
        const bool stop_transmission = multi_block && !(flags & SDSPI_CMD_FLAG_RSP_R5);
        if (flags & SDSPI_CMD_FLAG_WRITE) {
            ret = start_command_write_blocks(slot, cmd, data, data_size, multi_block, stop_transmission);
        } else {
            // R5 (SDIO CMD53) has a second response byte after R1, not part of the data stream
            const size_t response_extra = (flags & SDSPI_CMD_FLAG_RSP_R5) ? 1 : 0;
            ret = start_command_read_blocks(slot, cmd, data, data_size, stop_transmission, response_extra);
        }
    } else {
        ret = start_command_default(slot, flags, cmd);
    }
    cs_high(slot);

    release_bus(slot);
    spi_device_release_bus(slot->spi_handle);

    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "%s: cmd=%d error=0x%x", __func__, cmd_index, ret);
    } else {
        // Update internal state when some commands are sent successfully
        if (cmd_index == SD_CRC_ON_OFF) {
            slot->data_crc_enabled = (uint8_t) cmd_arg;
            ESP_LOGD(TAG, "data CRC set=%d", slot->data_crc_enabled);
        }
    }
    return ret;
}

static esp_err_t start_command_default(slot_info_t *slot, int flags, sdspi_hw_cmd_t *cmd)
{
    size_t cmd_size = SDSPI_CMD_R1_SIZE;
    if ((flags & SDSPI_CMD_FLAG_RSP_R1) ||
            (flags & SDSPI_CMD_FLAG_NORSP) ||
            (flags & SDSPI_CMD_FLAG_RSP_R1B)) {
        cmd_size = SDSPI_CMD_R1_SIZE;
    } else if (flags & SDSPI_CMD_FLAG_RSP_R2) {
        cmd_size = SDSPI_CMD_R2_SIZE;
    } else if (flags & SDSPI_CMD_FLAG_RSP_R3) {
        cmd_size = SDSPI_CMD_R3_SIZE;
    } else if (flags & SDSPI_CMD_FLAG_RSP_R4) {
        cmd_size = SDSPI_CMD_R4_SIZE;
    } else if (flags & SDSPI_CMD_FLAG_RSP_R5) {
        cmd_size = SDSPI_CMD_R5_SIZE;
    } else if (flags & SDSPI_CMD_FLAG_RSP_R7) {
        cmd_size = SDSPI_CMD_R7_SIZE;
    }
    //add extra clocks to avoid polling
    cmd_size += (SDSPI_NCR_MAX_SIZE - SDSPI_NCR_MIN_SIZE);
    uint8_t *rx;
    size_t rx_len;
    esp_err_t ret = rx_transfer(slot, cmd, cmd_size, use_polling, &rx, &rx_len);
    if (ret == ESP_OK) {
        memcpy(cmd, rx, cmd_size);
    }
    if (cmd->cmd_index == MMC_STOP_TRANSMISSION) {
        /* response is a stuff byte from previous transfer, ignore it */
        cmd->r1 = 0xff;
    }
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "%s: spi_device_polling_transmit returned 0x%x", __func__, ret);
        return ret;
    }
    if (flags & SDSPI_CMD_FLAG_NORSP) {
        /* no (correct) response expected from the card, so skip polling loop */
        ESP_LOGV(TAG, "%s: ignoring response byte", __func__);
        cmd->r1 = 0x00;
    }
    // we have sent and received bytes with enough length.
    // now shift the response to match the offset of sdspi_hw_cmd_t
    ret = shift_cmd_response(cmd, cmd_size);
    if (ret != ESP_OK) {
        return ESP_ERR_TIMEOUT;
    }

    if (flags & SDSPI_CMD_FLAG_RSP_R1B) {
        ret = poll_busy(slot, cmd->timeout_ms, no_use_polling);
        if (ret != ESP_OK) {
            return ret;
        }
    }

    return ESP_OK;
}

// Wait until MISO goes high
static esp_err_t poll_busy(slot_info_t *slot, int timeout_ms, bool polling)
{
    if (timeout_ms < 0) {
        return ESP_ERR_INVALID_ARG;
    } else if (timeout_ms == 0) {
        return ESP_OK;
    }
    uint8_t *rx;
    size_t rx_len;
    esp_err_t ret;

    int64_t t_end = esp_timer_get_time() + timeout_ms * 1000;
    int nonzero_count = 0;
    do {
        // A 1-byte read, padded to the RX DMA length alignment; every byte is a sample of MISO
        ret = rx_transfer(slot, NULL, 1, polling, &rx, &rx_len);
        if (ret != ESP_OK) {
            return ret;
        }
        for (size_t i = 0; i < rx_len; i++) {
            if (rx[i] != 0) {
                if (++nonzero_count == 2) {
                    return ESP_OK;
                }
            }
        }
    } while (esp_timer_get_time() < t_end);
    ESP_LOGD(TAG, "%s: timeout", __func__);
    return ESP_ERR_TIMEOUT;
}

// the r1 respond could appear 1-8 clocks after the command token is sent
// this function search for r1 in the buffer after 1 clocks to max 8 clocks
// then shift the data after R1, to match the definition of sdspi_hw_cmd_t.
static esp_err_t shift_cmd_response(sdspi_hw_cmd_t* cmd, int sent_bytes)
{
    uint8_t* pr1 = &cmd->r1;
    int ncr_cnt = 1;
    while (true) {
        if ((*pr1 & SD_SPI_R1_NO_RESPONSE) == 0) {
            break;
        }
        pr1++;
        if (++ncr_cnt > 8) {
            return ESP_ERR_NOT_FOUND;
        }
    }

    int copy_bytes = sent_bytes - SDSPI_CMD_SIZE - ncr_cnt;
    if (copy_bytes > 0) {
        memcpy(&cmd->r1, pr1, copy_bytes);
    }

    return ESP_OK;
}

/**
 * Receiving one or more blocks of data happens as follows:
 * 1. send command + receive r1 response (SDSPI_CMD_R1_SIZE bytes total)
 * 2. keep receiving bytes until TOKEN_BLOCK_START is encountered (this may
 *    take a while, depending on card's read speed)
 * 3. receive up to SDSPI_MAX_DATA_LEN = 512 bytes of actual data
 * 4. receive 2 bytes of CRC
 * 5. for multi block transfers, go to step 2
 *
 * These steps can be done separately, but that leads to a less than optimal
 * performance on large transfers because of delays between each step.
 * For example, if steps 3 and 4 are separate SPI transactions queued one after
 * another, there will be ~16 microseconds of dead time between end of step 3
 * and the beginning of step 4. A delay between two blocking SPI transactions
 * in step 2 is even higher (~60 microseconds).
 *
 * To improve read performance, everything clocked in after the command is
 * treated as one byte stream and fed to read_stream_feed(), so bytes that
 * arrive ahead of time in one transfer (start token, data, CRC) are consumed
 * in order and never re-read:
 * 1. Do the first transfer: command + r1 response + 8 extra bytes.  The bytes
 *    after r1 go to the parser, which may find the start token among them.
 * 2. While waiting for TOKEN_BLOCK_START, poll SDSPI_TOKEN_POLL_SIZE bytes at
 *    a time.  Bytes after the token are data.
 * 3. Once in a block, receive the rest of the data and the CRC in one
 *    transfer, plus 2 extra bytes when another block follows, which often
 *    contain the next start token so that step 2 is skipped.  When the final
 *    block is being received, the 2 extra bytes are not requested, because
 *    some cards take extra bytes before STOP_TRANSMISSION as part of the command.
 *
 * Every transfer is padded to the bus RX DMA length alignment (1 on most
 * targets, the cache line on ESP32-P4), so spi_master never allocates a
 * temporary buffer.  The padding is parsed like any other received bytes,
 * except after the final CRC, where it is ignored.  The one exception is the
 * transfer ending with the final CRC of a multi-block read, which is done at
 * its exact length so that no extra clocks precede STOP_TRANSMISSION; where
 * the length must be aligned, spi_master allocates a temporary buffer for it,
 * once per multi-block read.
 */
typedef struct {
    size_t skip;            ///< Response bytes after R1 still to be discarded
    uint8_t *data;          ///< Destination of the next data byte
    uint32_t remaining;     ///< Data bytes not yet completed, including the current block
    uint8_t *block_start;   ///< Destination of the current block
    size_t block_len;       ///< Size of the current block
    size_t block_left;      ///< Data bytes still to come in the current block
    size_t crc_got;         ///< CRC bytes received for the current block
    uint8_t crc[2];         ///< CRC of the current block, as received
    bool in_block;          ///< Start token of the current block has been seen
    bool crc_enabled;       ///< Check the CRC of each block
} read_stream_t;

/// Consume received bytes of a block read.  Bytes after the final CRC are ignored.
static esp_err_t read_stream_feed(read_stream_t *rs, const uint8_t *p, size_t n)
{
    size_t skip = MIN(n, rs->skip);
    rs->skip -= skip;
    p += skip;
    n -= skip;
    while (n > 0 && rs->remaining > 0) {
        if (!rs->in_block) {
            uint8_t b = *p++;
            --n;
            if (b == TOKEN_BLOCK_START) {
                rs->in_block = true;
                rs->block_start = rs->data;
                rs->block_len = MIN(rs->remaining, SDSPI_MAX_DATA_LEN);
                rs->block_left = rs->block_len;
                rs->crc_got = 0;
            } else if (b != 0xff && b != 0) {
                ESP_LOGD(TAG, "%s: received 0x%02x while waiting for data", __func__, b);
                return ESP_ERR_INVALID_RESPONSE;
            }
        } else if (rs->block_left > 0) {
            size_t k = MIN(n, rs->block_left);
            memcpy(rs->data, p, k);
            rs->data += k;
            rs->block_left -= k;
            p += k;
            n -= k;
        } else {
            rs->crc[rs->crc_got++] = *p++;
            --n;
            if (rs->crc_got == sizeof(rs->crc)) {
                if (rs->crc_enabled) {
                    uint16_t crc;
                    memcpy(&crc, rs->crc, sizeof(crc));
                    uint16_t crc_of_data = sdspi_crc16(rs->block_start, rs->block_len);
                    if (crc_of_data != crc) {
                        ESP_LOGE(TAG, "data CRC failed, got=0x%04x expected=0x%04x", crc_of_data, crc);
                        ESP_LOG_BUFFER_HEX(TAG, rs->block_start, MIN(rs->block_len, 16));
                        return ESP_ERR_INVALID_CRC;
                    }
                }
                rs->remaining -= rs->block_len;
                rs->in_block = false;
            }
        }
    }
    return ESP_OK;
}

static esp_err_t start_command_read_blocks(slot_info_t *slot, sdspi_hw_cmd_t *cmd,
                                           uint8_t *data, uint32_t rx_length, bool need_stop_command,
                                           size_t response_extra)
{
    const size_t cmd_bytes = SDSPI_CMD_R1_SIZE + SDSPI_RESPONSE_MAX_DELAY;
    uint8_t *rx;
    size_t rx_len;
    esp_err_t ret = rx_transfer(slot, cmd, cmd_bytes, use_polling, &rx, &rx_len);
    if (ret != ESP_OK) {
        return ret;
    }
    memcpy(cmd, rx, cmd_bytes);

    /* R1 response is delayed by 1-8 bytes from the request.
     * This loop searches for the response and writes it to cmd->r1.
     */
    size_t pos = SDSPI_CMD_R1_SIZE;
    while ((cmd->r1 & SD_SPI_R1_NO_RESPONSE) != 0 && pos < cmd_bytes) {
        cmd->r1 = rx[pos++];
    }
    if (cmd->r1 & SD_SPI_R1_NO_RESPONSE) {
        ESP_LOGD(TAG, "no response token found");
        return ESP_ERR_TIMEOUT;
    }

    read_stream_t rs = {
        .skip = response_extra,
        .data = data,
        .remaining = rx_length,
        .crc_enabled = slot->data_crc_enabled,
    };
    ret = read_stream_feed(&rs, rx + pos, rx_len - pos);
    if (ret != ESP_OK) {
        return ret;
    }

    int64_t t_end = esp_timer_get_time() + cmd->timeout_ms * 1000;
    while (rs.remaining > 0) {
        size_t want;
        bool polling;
        if (!rs.in_block) {
            // Wait for data to be ready
            if (esp_timer_get_time() >= t_end) {
                ESP_LOGD(TAG, "%s: timeout", __func__);
                return ESP_ERR_TIMEOUT;
            }
            want = SDSPI_TOKEN_POLL_SIZE;
            polling = true;
        } else {
            want = rs.block_left + sizeof(rs.crc) - rs.crc_got;
            if (rs.remaining > rs.block_len) {
                // Look for the start token of the next block in the same transfer
                want += 2;
            }
            want = MIN(want, slot->block_buf_size);
            polling = false;
        }
        size_t padded = SDSPI_ALIGN_UP(want, slot->rx_len_align);
        if (need_stop_command && rs.in_block && rs.remaining == rs.block_len) {
            // No extra clocks between the final CRC and STOP_TRANSMISSION
            padded = want;
        }
        ret = transfer(slot, NULL, want, padded, polling, &rx, &rx_len);
        if (ret != ESP_OK) {
            return ret;
        }
        ret = read_stream_feed(&rs, rx, rx_len);
        if (ret != ESP_OK) {
            return ret;
        }
        if (!polling) {
            // The wait for the next start token is timed from the end of this block
            t_end = esp_timer_get_time() + cmd->timeout_ms * 1000;
        }
    }

    if (need_stop_command) {
        // To end multi block transfer, send stop command and wait for the
        // card to process it
        sdspi_hw_cmd_t stop_cmd;
        make_hw_cmd(MMC_STOP_TRANSMISSION, 0, cmd->timeout_ms, &stop_cmd);
        ret = start_command_default(slot, SDSPI_CMD_FLAG_RSP_R1B, &stop_cmd);
        if (ret != ESP_OK) {
            return ret;
        }
        if (stop_cmd.r1 != 0) {
            ESP_LOGD(TAG, "%s: STOP_TRANSMISSION response 0x%02x", __func__, stop_cmd.r1);
        }
        ret = poll_busy(slot, cmd->timeout_ms, use_polling);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    return ESP_OK;
}

/* For CMD53, we can send in byte mode, or block mode
 * The data start token is different, and cannot be determined by the length
 * That's why we need ``multi_block``.
 * It's also different that stop transmission token is not needed in the SDIO mode.
 */
static esp_err_t start_command_write_blocks(slot_info_t *slot, sdspi_hw_cmd_t *cmd,
                                            const uint8_t *data, uint32_t tx_length, bool multi_block, bool stop_trans)
{
    if (card_write_protected(slot)) {
        ESP_LOGW(TAG, "%s: card write protected", __func__);
        return ESP_ERR_INVALID_STATE;
    }
    // Send the minimum length that is sure to get the complete response
    // SD cards always return R1 (1bytes), SDIO returns R5 (2 bytes)
    const int send_bytes = SDSPI_CMD_R5_SIZE + SDSPI_NCR_MAX_SIZE - SDSPI_NCR_MIN_SIZE;

    uint8_t *rx;
    size_t rx_len;
    esp_err_t ret = rx_transfer(slot, cmd, send_bytes, use_polling, &rx, &rx_len);
    if (ret != ESP_OK) {
        return ret;
    }
    memcpy(cmd, rx, send_bytes);

    // check if command response valid
    ret = shift_cmd_response(cmd, send_bytes);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "%s: check_cmd_response returned 0x%x", __func__, ret);
        return ret;
    }

    uint8_t start_token = multi_block ?
                          TOKEN_BLOCK_START_WRITE_MULTI : TOKEN_BLOCK_START;

    while (tx_length > 0) {
        // Write block start token
        spi_transaction_t t_start_token = {
            .length = sizeof(start_token) * 8,
            .tx_buffer = &start_token
        };
        ret = spi_device_polling_transmit(slot->spi_handle, &t_start_token);
        if (ret != ESP_OK) {
            return ret;
        }

        // Prepare data to be sent
        size_t will_send = MIN(tx_length, SDSPI_MAX_DATA_LEN);

        // Write data. The SPI master driver handles buffers that are not
        // directly DMA-usable (e.g. PSRAM, or unaligned): with
        // SPI_TRANS_DMA_USE_PSRAM it transfers a suitable PSRAM buffer directly
        // (no copy), and otherwise falls back to an internal aligned buffer
        // itself. This avoids the redundant copy the SDSPI driver used to do.
        // The flag is ignored for buffers that are not in PSRAM.
        spi_transaction_t t_data = {
            .length = will_send * 8,
            .tx_buffer = data,
            .flags = SPI_TRANS_DMA_USE_PSRAM,
        };
        ret = spi_device_transmit(slot->spi_handle, &t_data);
        if (ret != ESP_OK) {
            return ret;
        }

        // Write CRC and get the response in one transaction
        uint16_t crc = sdspi_crc16(data, will_send);
        // CRC (2 bytes) then the data response (1 byte).  Any padding to the
        // RX DMA alignment is clocked while the card is busy; poll_busy follows.
        uint8_t crc_rsp[sizeof(crc) + 1];
        memcpy(crc_rsp, &crc, sizeof(crc));
        crc_rsp[sizeof(crc)] = SDSPI_MOSI_IDLE_VAL;

        ret = rx_transfer(slot, crc_rsp, sizeof(crc_rsp), use_polling, &rx, &rx_len);
        if (ret != ESP_OK) {
            return ret;
        }

        uint8_t data_rsp = rx[sizeof(crc)];
        if (!SD_SPI_DATA_RSP_VALID(data_rsp)) {
            return ESP_ERR_INVALID_RESPONSE;
        }
        switch (SD_SPI_DATA_RSP(data_rsp)) {
        case SD_SPI_DATA_ACCEPTED:
            break;
        case SD_SPI_DATA_CRC_ERROR:
            return ESP_ERR_INVALID_CRC;
        case SD_SPI_DATA_WR_ERROR:
            return ESP_FAIL;
        default:
            return ESP_ERR_INVALID_RESPONSE;
        }

        // Wait for the card to finish writing data
        ret = poll_busy(slot, cmd->timeout_ms, no_use_polling);
        if (ret != ESP_OK) {
            return ret;
        }

        tx_length -= will_send;
        data += will_send;
    }

    if (stop_trans) {
        uint8_t stop_token[2] = {
            TOKEN_BLOCK_STOP_WRITE_MULTI,
            SDSPI_MOSI_IDLE_VAL
        };
        spi_transaction_t t_stop_token = {
            .length = sizeof(stop_token) * 8,
            .tx_buffer = &stop_token,
        };
        ret = spi_device_polling_transmit(slot->spi_handle, &t_stop_token);
        if (ret != ESP_OK) {
            return ret;
        }

        ret = poll_busy(slot, cmd->timeout_ms, use_polling);
        if (ret != ESP_OK) {
            return ret;
        }
    }

    return ESP_OK;
}

esp_err_t sdspi_host_io_int_enable(sdspi_dev_handle_t handle)
{
    //the pin and its interrupt is already initialized, nothing to do here.
    return ESP_OK;
}

//the interrupt will give the semaphore and then disable itself
esp_err_t sdspi_host_io_int_wait(sdspi_dev_handle_t handle, uint32_t timeout_ticks)
{
    slot_info_t* slot = get_slot_info(handle);
    //skip the interrupt and semaphore if the gpio is already low.
    if (gpio_get_level(slot->gpio_int) == 0) {
        return ESP_OK;
    }

    //clear the semaphore before wait
    xSemaphoreTake(slot->semphr_int, 0);
    //enable the interrupt and wait for the semaphore
    gpio_intr_enable(slot->gpio_int);
    BaseType_t ret = xSemaphoreTake(slot->semphr_int, timeout_ticks);
    if (ret == pdFALSE) {
        gpio_intr_disable(slot->gpio_int);
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

bool sdspi_host_check_buffer_alignment(int slot, const void *buf, size_t size)
{
    //for future-proof
    (void)slot;

    if (!buf || !size) {
        return ESP_FAIL;
    }

    //spi master driver will deal with the buffer alignment
    bool is_aligned = ((intptr_t)buf % 4 == 0) && (size % 4 == 0);

    return is_aligned;
}
