/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
#include "soc/soc_memory_layout.h"

/// Max number of transactions in flight (used in start_command_write_blocks)
#define SDSPI_TRANSACTION_COUNT 4
#define SDSPI_MOSI_IDLE_VAL     0xff    //!< Data value which causes MOSI to stay high
#define GPIO_UNUSED 0xff                //!< Flag indicating that CD/WP is unused
/// Size of the buffer returned by get_block_buf
#define SDSPI_BLOCK_BUF_SIZE    (SDSPI_MAX_DATA_LEN + 4)
/// Maximum number of dummy bytes between the request and response (minimum is 1)
#define SDSPI_RESPONSE_MAX_DELAY  8

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
    uint8_t gpio_cs;            //!< CS GPIO, or GPIO_UNUSED
    uint8_t gpio_cd;            //!< Card detect GPIO, or GPIO_UNUSED
    uint8_t gpio_wp;            //!< Write protect GPIO, or GPIO_UNUSED
    uint8_t gpio_int;           //!< Write protect GPIO, or GPIO_UNUSED
    /// GPIO write protect polarity.
    /// 0 means "active low", i.e. card is protected when the GPIO is low;
    /// 1 means "active high", i.e. card is protected when GPIO is high.
    uint8_t gpio_wp_polarity : 1;
    /// Set to 1 if the higher layer has asked the card to enable CRC checks
    uint8_t data_crc_enabled : 1;
    /// Intermediate buffer used when application buffer is not in DMA memory;
    /// allocated on demand, SDSPI_BLOCK_BUF_SIZE bytes long. May be zero.
    uint8_t* block_buf;
    /// semaphore of gpio interrupt
    SemaphoreHandle_t   semphr_int;
    uint16_t duty_cycle_pos;  ///< Duty cycle of positive clock, in 1/256th increments (128 = 50%/50% duty). Setting this to 0 (=not setting it) is equivalent to setting this to 128.
} slot_info_t;

// Reserved for old API to be back-compatible
static slot_info_t *s_slots[SOC_SPI_PERIPH_NUM] = {};
static const char *TAG = "sdspi_host";

static const bool use_polling = true;
static const bool no_use_polling = true;

/// Functions to send out different kinds of commands
static esp_err_t start_command_read_blocks(slot_info_t *slot, sdspi_hw_cmd_t *cmd,
                                           uint8_t *data, uint32_t rx_length, bool need_stop_command);

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

/// Get pointer to a block of DMA memory, allocate if necessary.
/// This is used if the application provided buffer is not in DMA capable memory.
static esp_err_t get_block_buf(slot_info_t *slot, uint8_t **out_buf)
{
    if (slot->block_buf == NULL) {
        slot->block_buf = heap_caps_malloc(SDSPI_BLOCK_BUF_SIZE, MALLOC_CAP_DMA);
        if (slot->block_buf == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }
    *out_buf = slot->block_buf;
    return ESP_OK;
}

/// Clock out one byte (CS has to be high) to make the card release MISO
/// (clocking one bit would work as well, but that triggers a bug in SPI DMA)
static void release_bus(slot_info_t *slot)
{
    spi_transaction_t t = {
        .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0xff}
    };
    spi_device_polling_transmit(slot->spi_handle, &t);
    // don't care if this failed
}

/// Clock out 80 cycles (10 bytes) before GO_IDLE command
static void go_idle_clockout(slot_info_t *slot)
{
    //actually we need 10, declare 12 to meet requirement of RXDMA
    uint8_t data[12];
    memset(data, 0xff, sizeof(data));
    spi_transaction_t t = {
        .length = 10 * 8,
        .tx_buffer = data,
        .rx_buffer = data,
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
        free(slot->block_buf);
        slot->block_buf = NULL;
    }

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
    };

    // Attach the SD card to the SPI bus
    esp_err_t ret = configure_spi_dev(slot, SDMMC_FREQ_PROBING * 1000);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "spi_bus_add_device failed with rc=0x%x", ret);
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
        .pull_up_en = true
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
            io_conf.pull_down_en = true;
            io_conf.pull_up_en = false;
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
            .pull_up_en = true,
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

    spi_device_acquire_bus(slot->spi_handle, portMAX_DELAY);
    poll_busy(slot, 40, true);

    // For CMD0, clock out 80 cycles to help the card enter idle state,
    // *before* CS is asserted.
    if (cmd_index == MMC_GO_IDLE_STATE) {
        go_idle_clockout(slot);
    }
    // actual transaction
    esp_err_t ret = ESP_OK;

    cs_low(slot);
    if (flags & SDSPI_CMD_FLAG_DATA) {
        const bool multi_block = flags & SDSPI_CMD_FLAG_MULTI_BLK;
        //send stop transmission token only when multi-block write and non-SDIO mode
        const bool stop_transmission = multi_block && !(flags & SDSPI_CMD_FLAG_RSP_R5);
        if (flags & SDSPI_CMD_FLAG_WRITE) {
            ret = start_command_write_blocks(slot, cmd, data, data_size, multi_block, stop_transmission);
        } else {
            ret = start_command_read_blocks(slot, cmd, data, data_size, stop_transmission);
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
    spi_transaction_t t = {
        .flags = 0,
        .length = cmd_size * 8,
        .tx_buffer = cmd,
        .rx_buffer = cmd,
    };
    esp_err_t ret = spi_device_polling_transmit(slot->spi_handle, &t);
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
    uint8_t t_rx;
    spi_transaction_t t = {
        .tx_buffer = &t_rx,
        .flags = SPI_TRANS_USE_RXDATA,  //data stored in rx_data
        .length = 8,
    };
    esp_err_t ret;

    int64_t t_end = esp_timer_get_time() + timeout_ms * 1000;
    int nonzero_count = 0;
    do {
        t_rx = SDSPI_MOSI_IDLE_VAL;
        t.rx_data[0] = 0;
        if (polling) {
            ret = spi_device_polling_transmit(slot->spi_handle, &t);
        } else {
            ret = spi_device_transmit(slot->spi_handle, &t);
        }
        if (ret != ESP_OK) {
            return ret;
        }
        if (t.rx_data[0] != 0) {
            if (++nonzero_count == 2) {
                return ESP_OK;
            }
        }
    } while (esp_timer_get_time() < t_end);
    ESP_LOGD(TAG, "%s: timeout", __func__);
    return ESP_ERR_TIMEOUT;
}

// Wait for data token, reading 8 bytes at a time.
// If the token is found, write all subsequent bytes to extra_ptr,
// and store the number of bytes written to extra_size.
static esp_err_t poll_data_token(slot_info_t *slot, uint8_t *extra_ptr, size_t *extra_size, int timeout_ms)
{
    uint8_t t_rx[8];
    spi_transaction_t t = {
        .tx_buffer = &t_rx,
        .rx_buffer = &t_rx,
        .length = sizeof(t_rx) * 8,
    };
    esp_err_t ret;
    int64_t t_end = esp_timer_get_time() + timeout_ms * 1000;
    do {
        memset(t_rx, SDSPI_MOSI_IDLE_VAL, sizeof(t_rx));
        ret = spi_device_polling_transmit(slot->spi_handle, &t);
        if (ret != ESP_OK) {
            return ret;
        }
        bool found = false;
        for (size_t byte_idx = 0; byte_idx < sizeof(t_rx); byte_idx++) {
            uint8_t rd_data = t_rx[byte_idx];
            if (rd_data == TOKEN_BLOCK_START) {
                found = true;
                memcpy(extra_ptr, t_rx + byte_idx + 1, sizeof(t_rx) - byte_idx - 1);
                *extra_size = sizeof(t_rx) - byte_idx - 1;
                break;
            }
            if (rd_data != 0xff && rd_data != 0) {
                ESP_LOGD(TAG, "%s: received 0x%02x while waiting for data",
                         __func__, rd_data);
                return ESP_ERR_INVALID_RESPONSE;
            }
        }
        if (found) {
            return ESP_OK;
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
 * To improve read performance the following sequence is adopted:
 * 1. Do the first transfer: command + r1 response + 8 extra bytes.
 *    Set pre_scan_data_ptr to point to the 8 extra bytes, and set
 *    pre_scan_data_size to 8.
 * 2. Search pre_scan_data_size bytes for TOKEN_BLOCK_START.
 *    If found, the rest of the bytes contain part of the actual data.
 *    Store pointer to and size of that extra data as extra_data_{ptr,size}.
 *    If not found, fall back to polling for TOKEN_BLOCK_START, 8 bytes at a
 *    time (in poll_data_token function). Deal with extra data in the same way,
 *    by setting extra_data_{ptr,size}.
 * 3. Receive the remaining 512 - extra_data_size bytes, plus 4 extra bytes
 *    (i.e. 516 - extra_data_size). Of the 4 extra bytes, first two will capture
 *    the CRC value, and the other two will capture 0xff 0xfe sequence
 *    indicating the start of the next block. Actual scanning is done by
 *    setting pre_scan_data_ptr to point to these last 2 bytes, and setting
 *    pre_scan_data_size = 2, then going to step 2 to receive the next block.
 *    When the final block is being received, the number of extra bytes is 2
 *    (only for CRC), because we don't need to wait for start token of the
 *    next block, and some cards are getting confused by these two extra bytes.
 *
 * With this approach the delay between blocks of a multi-block transfer is
 * ~95 microseconds, out of which 35 microseconds are spend doing the CRC check.
 * Further speedup is possible by pipelining transfers and CRC checks, at an
 * expense of one extra temporary buffer.
 */
static esp_err_t start_command_read_blocks(slot_info_t *slot, sdspi_hw_cmd_t *cmd,
                                           uint8_t *data, uint32_t rx_length, bool need_stop_command)
{
    spi_transaction_t t_command = {
        .length = (SDSPI_CMD_R1_SIZE + SDSPI_RESPONSE_MAX_DELAY) * 8,
        .tx_buffer = cmd,
        .rx_buffer = cmd,
    };
    esp_err_t ret = spi_device_polling_transmit(slot->spi_handle, &t_command);
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t* cmd_u8 = (uint8_t*) cmd;
    size_t pre_scan_data_size = SDSPI_RESPONSE_MAX_DELAY;
    uint8_t* pre_scan_data_ptr = cmd_u8 + SDSPI_CMD_R1_SIZE;

    /* R1 response is delayed by 1-8 bytes from the request.
     * This loop searches for the response and writes it to cmd->r1.
     */
    while ((cmd->r1 & SD_SPI_R1_NO_RESPONSE) != 0 && pre_scan_data_size > 0) {
        cmd->r1 = *pre_scan_data_ptr;
        ++pre_scan_data_ptr;
        --pre_scan_data_size;
    }
    if (cmd->r1 & SD_SPI_R1_NO_RESPONSE) {
        ESP_LOGD(TAG, "no response token found");
        return ESP_ERR_TIMEOUT;
    }

    while (rx_length > 0) {
        size_t extra_data_size = 0;
        const uint8_t* extra_data_ptr = NULL;
        bool need_poll = true;

        for (size_t i = 0; i < pre_scan_data_size; ++i) {
            if (pre_scan_data_ptr[i] == TOKEN_BLOCK_START) {
                extra_data_size = pre_scan_data_size - i - 1;
                extra_data_ptr = pre_scan_data_ptr + i + 1;
                need_poll = false;
                break;
            }
        }

        if (need_poll) {
            // Wait for data to be ready
            ret = poll_data_token(slot, cmd_u8 + SDSPI_CMD_R1_SIZE, &extra_data_size, cmd->timeout_ms);
            if (ret != ESP_OK) {
                return ret;
            }
            if (extra_data_size) {
                extra_data_ptr = cmd_u8 + SDSPI_CMD_R1_SIZE;
            }
        }

        // Arrange RX buffer
        size_t will_receive = MIN(rx_length, SDSPI_MAX_DATA_LEN) - extra_data_size;
        uint8_t* rx_data;
        ret = get_block_buf(slot, &rx_data);
        if (ret != ESP_OK) {
            return ret;
        }

        // receive actual data
        const size_t receive_extra_bytes = (rx_length > SDSPI_MAX_DATA_LEN) ? 4 : 2;
        memset(rx_data, 0xff, will_receive + receive_extra_bytes);
        spi_transaction_t t_data = {
            .length = (will_receive + receive_extra_bytes) * 8,
            .rx_buffer = rx_data,
            .tx_buffer = rx_data
        };

        ret = spi_device_transmit(slot->spi_handle, &t_data);
        if (ret != ESP_OK) {
            return ret;
        }

        // CRC bytes need to be received even if CRC is not enabled
        uint16_t crc = UINT16_MAX;
        memcpy(&crc, rx_data + will_receive, sizeof(crc));

        // Bytes to scan for the start token
        pre_scan_data_size = receive_extra_bytes - sizeof(crc);
        pre_scan_data_ptr = rx_data + will_receive + sizeof(crc);

        // Copy data to the destination buffer
        memcpy(data + extra_data_size, rx_data, will_receive);
        if (extra_data_size) {
            memcpy(data, extra_data_ptr, extra_data_size);
        }

        // compute CRC of the received data
        uint16_t crc_of_data = 0;
        if (slot->data_crc_enabled) {
            crc_of_data = sdspi_crc16(data, will_receive + extra_data_size);
            if (crc_of_data != crc) {
                ESP_LOGE(TAG, "data CRC failed, got=0x%04x expected=0x%04x", crc_of_data, crc);
                ESP_LOG_BUFFER_HEX(TAG, data, 16);
                return ESP_ERR_INVALID_CRC;
            }
        }

        data += will_receive + extra_data_size;
        rx_length -= will_receive + extra_data_size;
        extra_data_size = 0;
        extra_data_ptr = NULL;
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

    spi_transaction_t t_command = {
        .length = send_bytes * 8,
        .tx_buffer = cmd,
        .rx_buffer = cmd,
    };
    esp_err_t ret = spi_device_polling_transmit(slot->spi_handle, &t_command);
    if (ret != ESP_OK) {
        return ret;
    }

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
        const uint8_t* tx_data = data;
        if (!esp_ptr_in_dram(tx_data)) {
            // If the pointer can't be used with DMA, copy data into a new buffer
            uint8_t* tmp;
            ret = get_block_buf(slot, &tmp);
            if (ret != ESP_OK) {
                return ret;
            }
            memcpy(tmp, tx_data, will_send);
            tx_data = tmp;
        }

        // Write data
        spi_transaction_t t_data = {
            .length = will_send * 8,
            .tx_buffer = tx_data,
        };
        ret = spi_device_transmit(slot->spi_handle, &t_data);
        if (ret != ESP_OK) {
            return ret;
        }

        // Write CRC and get the response in one transaction
        uint16_t crc = sdspi_crc16(data, will_send);
        const int size_crc_response = sizeof(crc) + 1;

        spi_transaction_t t_crc_rsp = {
            .length = size_crc_response * 8,
            .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
        };
        memset(t_crc_rsp.tx_data, 0xff, 4);
        memcpy(t_crc_rsp.tx_data, &crc, sizeof(crc));

        ret = spi_device_polling_transmit(slot->spi_handle, &t_crc_rsp);
        if (ret != ESP_OK) {
            return ret;
        }

        uint8_t data_rsp = t_crc_rsp.rx_data[2];
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
esp_err_t sdspi_host_io_int_wait(sdspi_dev_handle_t handle, TickType_t timeout_ticks)
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

esp_err_t sdspi_host_get_dma_info(int slot, esp_dma_mem_info_t *dma_mem_info)
{
    (void)slot;
    dma_mem_info->extra_heap_caps = MALLOC_CAP_DMA;
    dma_mem_info->dma_alignment_bytes = 4;
    return ESP_OK;
}
