// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/gpio.h"
#include "driver/sdmmc_defs.h"
#include "driver/sdspi_host.h"
#include "sdspi_private.h"
#include "sdspi_crc.h"
#include "esp_timer.h"


/// Max number of transactions in flight (used in start_command_write_blocks)
#define SDSPI_TRANSACTION_COUNT 4
#define SDSPI_MOSI_IDLE_VAL     0xff    //!< Data value which causes MOSI to stay high
#define GPIO_UNUSED 0xff                //!< Flag indicating that CD/WP is unused
/// Size of the buffer returned by get_block_buf
#define SDSPI_BLOCK_BUF_SIZE    (SDSPI_MAX_DATA_LEN + 4)


/// Structure containing run time configuration for a single SD slot
typedef struct {
    spi_device_handle_t handle; //!< SPI device handle, used for transactions
    uint8_t gpio_cs;            //!< CS GPIO
    uint8_t gpio_cd;            //!< Card detect GPIO, or GPIO_UNUSED
    uint8_t gpio_wp;            //!< Write protect GPIO, or GPIO_UNUSED
    /// Set to 1 if the higher layer has asked the card to enable CRC checks
    uint8_t data_crc_enabled : 1;
    /// Number of transactions in 'transactions' array which are in use
    uint8_t used_transaction_count: 3;
    /// Intermediate buffer used when application buffer is not in DMA memory;
    /// allocated on demand, SDSPI_BLOCK_BUF_SIZE bytes long. May be zero.
    uint8_t* block_buf;
    /// array with SDSPI_TRANSACTION_COUNT transaction structures
    spi_transaction_t* transactions;
} slot_info_t;

static slot_info_t s_slots[3];
static const char *TAG = "sdspi_host";

/// Functions to send out different kinds of commands
static esp_err_t start_command_read_blocks(int slot, sdspi_hw_cmd_t *cmd,
        uint8_t *data, uint32_t rx_length);

static esp_err_t start_command_write_blocks(int slot, sdspi_hw_cmd_t *cmd,
        const uint8_t *data, uint32_t tx_length);

static esp_err_t start_command_default(int slot, int flags, sdspi_hw_cmd_t *cmd);

/// A few helper functions

/// Set CS high for given slot
static void cs_high(int slot)
{
    gpio_set_level(s_slots[slot].gpio_cs, 1);
}

/// Set CS low for given slot
static void cs_low(int slot)
{
    gpio_set_level(s_slots[slot].gpio_cs, 0);
}

/// Return true if WP pin is configured and is low
static bool card_write_protected(int slot)
{
    if (s_slots[slot].gpio_wp == GPIO_UNUSED) {
        return false;
    }
    return gpio_get_level(s_slots[slot].gpio_wp) == 0;
}

/// Return true if CD pin is configured and is high
static bool card_missing(int slot)
{
    if (s_slots[slot].gpio_cd == GPIO_UNUSED) {
        return false;
    }
    return gpio_get_level(s_slots[slot].gpio_cd) == 1;
}

/// Check if slot number is within bounds
static bool is_valid_slot(int slot)
{
    return slot == VSPI_HOST || slot == HSPI_HOST;
}

static spi_device_handle_t spi_handle(int slot)
{
    return s_slots[slot].handle;
}

static bool is_slot_initialized(int slot)
{
    return spi_handle(slot) != NULL;
}

static bool data_crc_enabled(int slot)
{
    return s_slots[slot].data_crc_enabled;
}

/// Get pointer to a block of DMA memory, allocate if necessary.
/// This is used if the application provided buffer is not in DMA capable memory.
static esp_err_t get_block_buf(int slot, uint8_t** out_buf)
{
    if (s_slots[slot].block_buf == NULL) {
        s_slots[slot].block_buf = heap_caps_malloc(SDSPI_BLOCK_BUF_SIZE, MALLOC_CAP_DMA);
        if (s_slots[slot].block_buf == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }
    *out_buf = s_slots[slot].block_buf;
    return ESP_OK;
}

static spi_transaction_t* get_transaction(int slot)
{
    size_t used_transaction_count = s_slots[slot].used_transaction_count;
    assert(used_transaction_count < SDSPI_TRANSACTION_COUNT);
    spi_transaction_t* ret = &s_slots[slot].transactions[used_transaction_count];
    ++s_slots[slot].used_transaction_count;
    return ret;
}

static void release_transaction(int slot)
{
    --s_slots[slot].used_transaction_count;
}

static void wait_for_transactions(int slot)
{
    size_t used_transaction_count = s_slots[slot].used_transaction_count;
    for (size_t i = 0; i < used_transaction_count; ++i) {
        spi_transaction_t* t_out;
        spi_device_get_trans_result(spi_handle(slot), &t_out, portMAX_DELAY);
        release_transaction(slot);
    }
}

/// Clock out one byte (CS has to be high) to make the card release MISO
/// (clocking one bit would work as well, but that triggers a bug in SPI DMA)
static void release_bus(int slot)
{
    spi_transaction_t t = {
        .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0xff}
    };
    spi_device_transmit(spi_handle(slot), &t);
    // don't care if this failed
}

/// Clock out 80 cycles (10 bytes) before GO_IDLE command
static void go_idle_clockout(int slot)
{
    //actually we need 10, declare 12 to meet requirement of RXDMA
    uint8_t data[12];
    memset(data, 0xff, sizeof(data));
    spi_transaction_t t = {
        .length = 10*8,
        .tx_buffer = data,
        .rx_buffer = data,
    };
    spi_device_transmit(spi_handle(slot), &t);
    // don't care if this failed
}


/// Return true if the pointer can be used for DMA
static bool ptr_dma_compatible(const void* ptr)
{
    return (uintptr_t) ptr >= 0x3FFAE000 &&
           (uintptr_t) ptr < 0x40000000;
}

/**
 * Initialize SPI device. Used to change clock speed.
 * @param slot  SPI host number
 * @param clock_speed_hz  clock speed, Hz
 * @return ESP_OK on success
 */
static esp_err_t init_spi_dev(int slot, int clock_speed_hz)
{
    if (spi_handle(slot)) {
        // Reinitializing
        spi_bus_remove_device(spi_handle(slot));
        s_slots[slot].handle = NULL;
    }
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = clock_speed_hz,
        .mode = 0,
        // For SD cards, CS must stay low during the whole read/write operation,
        // rather than a single SPI transaction.
        .spics_io_num = -1,
        .queue_size = SDSPI_TRANSACTION_COUNT,
    };
    return spi_bus_add_device((spi_host_device_t) slot, &devcfg, &s_slots[slot].handle);
}

esp_err_t sdspi_host_init()
{
    return ESP_OK;
}

esp_err_t sdspi_host_deinit()
{
    for (size_t i = 0; i < sizeof(s_slots)/sizeof(s_slots[0]); ++i) {
        if (s_slots[i].handle) {
            spi_bus_remove_device(s_slots[i].handle);
            free(s_slots[i].block_buf);
            s_slots[i].block_buf = NULL;
            free(s_slots[i].transactions);
            s_slots[i].transactions = NULL;
            spi_bus_free((spi_host_device_t) i);
            s_slots[i].handle = NULL;
        }
    }
    return ESP_OK;
}

esp_err_t sdspi_host_set_card_clk(int slot, uint32_t freq_khz)
{
    if (!is_valid_slot(slot)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!is_slot_initialized(slot)) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGD(TAG, "Setting card clock to %d kHz", freq_khz);
    return init_spi_dev(slot, freq_khz * 1000);
}

esp_err_t sdspi_host_init_slot(int slot, const sdspi_slot_config_t* slot_config)
{
    ESP_LOGD(TAG, "%s: SPI%d miso=%d mosi=%d sck=%d cs=%d cd=%d wp=%d, dma_ch=%d",
            __func__, slot + 1,
            slot_config->gpio_miso, slot_config->gpio_mosi,
            slot_config->gpio_sck, slot_config->gpio_cs,
            slot_config->gpio_cd, slot_config->gpio_wp,
            slot_config->dma_channel);

    spi_host_device_t host = (spi_host_device_t) slot;
    if (!is_valid_slot(slot)) {
        return ESP_ERR_INVALID_ARG;
    }

    spi_bus_config_t buscfg = {
        .miso_io_num = slot_config->gpio_miso,
        .mosi_io_num = slot_config->gpio_mosi,
        .sclk_io_num = slot_config->gpio_sck,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    // Initialize SPI bus
    esp_err_t ret = spi_bus_initialize((spi_host_device_t)slot, &buscfg,
            slot_config->dma_channel);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "spi_bus_initialize failed with rc=0x%x", ret);
        return ret;
    }

    // Attach the SD card to the SPI bus
    ret = init_spi_dev(slot, SDMMC_FREQ_PROBING * 1000);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "spi_bus_add_device failed with rc=0x%x", ret);
        spi_bus_free(host);
        return ret;
    }

    // Configure CS pin
    s_slots[slot].gpio_cs = (uint8_t) slot_config->gpio_cs;
    gpio_config_t io_conf = {
        .intr_type = GPIO_PIN_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1LL << slot_config->gpio_cs,
    };

    ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "gpio_config (CS) failed with rc=0x%x", ret);
        spi_bus_remove_device(spi_handle(slot));
        s_slots[slot].handle = NULL;
        spi_bus_free(host);
        return ret;
    }
    cs_high(slot);

    // Configure CD and WP pins
    io_conf = (gpio_config_t) {
        .intr_type = GPIO_PIN_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 0,
        .pull_up_en = true
    };
    if (slot_config->gpio_cd != SDSPI_SLOT_NO_CD) {
        io_conf.pin_bit_mask |= (1 << slot_config->gpio_cd);
        s_slots[slot].gpio_wp = slot_config->gpio_wp;
    } else {
        s_slots[slot].gpio_wp = GPIO_UNUSED;
    }

    if (slot_config->gpio_wp != SDSPI_SLOT_NO_WP) {
        io_conf.pin_bit_mask |= (1 << slot_config->gpio_wp);
        s_slots[slot].gpio_cd = slot_config->gpio_cd;
    } else {
        s_slots[slot].gpio_cd = GPIO_UNUSED;
    }

    if (io_conf.pin_bit_mask != 0) {
        ret = gpio_config(&io_conf);
        if (ret != ESP_OK) {
            ESP_LOGD(TAG, "gpio_config (CD/WP) failed with rc=0x%x", ret);
            spi_bus_remove_device(spi_handle(slot));
            s_slots[slot].handle = NULL;
            spi_bus_free(host);
            return ret;
        }
    }

    s_slots[slot].transactions = calloc(SDSPI_TRANSACTION_COUNT, sizeof(spi_transaction_t));
    if (s_slots[slot].transactions == NULL) {
        spi_bus_remove_device(spi_handle(slot));
        s_slots[slot].handle = NULL;
        spi_bus_free(host);
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}


esp_err_t sdspi_host_start_command(int slot, sdspi_hw_cmd_t *cmd, void *data,
                                   uint32_t data_size, int flags)
{
    if (!is_valid_slot(slot)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!is_slot_initialized(slot)) {
        return ESP_ERR_INVALID_STATE;
    }
    if (card_missing(slot)) {
        return ESP_ERR_NOT_FOUND;
    }
    // save some parts of cmd, as its contents will be overwritten
    int cmd_index = cmd->cmd_index;
    uint32_t cmd_arg;
    memcpy(&cmd_arg, cmd->arguments, sizeof(cmd_arg));
    cmd_arg = __builtin_bswap32(cmd_arg);
    ESP_LOGV(TAG, "%s: slot=%i, CMD%d, arg=0x%08x flags=0x%x, data=%p, data_size=%i crc=0x%02x",
             __func__, slot, cmd_index, cmd_arg, flags, data, data_size, cmd->crc7);


    // For CMD0, clock out 80 cycles to help the card enter idle state,
    // *before* CS is asserted.
    if (cmd_index == MMC_GO_IDLE_STATE) {
        go_idle_clockout(slot);
    }
    // actual transaction
    esp_err_t ret = ESP_OK;
    cs_low(slot);
    if (flags & SDSPI_CMD_FLAG_DATA) {
        if (flags & SDSPI_CMD_FLAG_WRITE) {
            ret = start_command_write_blocks(slot, cmd, data, data_size);
        } else {
            ret = start_command_read_blocks(slot, cmd, data, data_size);
        }
    } else {
        ret = start_command_default(slot, flags, cmd);
    }
    cs_high(slot);

    release_bus(slot);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: cmd=%d error=0x%x", __func__, cmd_index, ret);
    } else {
        // Update internal state when some commands are sent successfully
        if (cmd_index == SD_CRC_ON_OFF) {
            s_slots[slot].data_crc_enabled = (uint8_t) cmd_arg;
            ESP_LOGD(TAG, "data CRC set=%d", s_slots[slot].data_crc_enabled);
        }
    }
    return ret;
}

static esp_err_t start_command_default(int slot, int flags, sdspi_hw_cmd_t *cmd)
{
    size_t cmd_size = SDSPI_CMD_R1_SIZE;
    if (flags & SDSPI_CMD_FLAG_RSP_R1) {
        cmd_size = SDSPI_CMD_R1_SIZE;
    } else if (flags & SDSPI_CMD_FLAG_RSP_R2) {
        cmd_size = SDSPI_CMD_R2_SIZE;
    } else if (flags & SDSPI_CMD_FLAG_RSP_R3) {
        cmd_size = SDSPI_CMD_R3_SIZE;
    } else if (flags & SDSPI_CMD_FLAG_RSP_R7) {
        cmd_size = SDSPI_CMD_R7_SIZE;
    }
    spi_transaction_t t = {
        .flags = 0,
        .length = cmd_size * 8,
        .tx_buffer = cmd,
        .rx_buffer = cmd
    };
    esp_err_t ret = spi_device_transmit(spi_handle(slot), &t);
    return ret;
}

// Wait until MISO goes high
static esp_err_t poll_busy(int slot, spi_transaction_t* t, int timeout_ms)
{
    uint8_t t_rx;
    *t = (spi_transaction_t) {
        .tx_buffer = &t_rx,
        .flags = SPI_TRANS_USE_RXDATA,  //data stored in rx_data
        .length = 8,
    };
    esp_err_t ret;

    uint64_t t_end = esp_timer_get_time() + timeout_ms * 1000;
    int nonzero_count = 0;
    do {
        t_rx = SDSPI_MOSI_IDLE_VAL;
        t->rx_data[0] = 0;
        ret = spi_device_transmit(spi_handle(slot), t);
        if (ret != ESP_OK) {
            return ret;
        }
        if (t->rx_data[0] != 0) {
            if (++nonzero_count == 2) {
                return ESP_OK;
            }
        }
    } while(esp_timer_get_time() < t_end);
    ESP_LOGD(TAG, "%s: timeout", __func__);
    return ESP_ERR_TIMEOUT;
}

// Wait for response token
static esp_err_t poll_response_token(int slot, spi_transaction_t* t, int timeout_ms)
{
    uint8_t t_rx;
    *t = (spi_transaction_t) {
        .tx_buffer = &t_rx,
        .flags = SPI_TRANS_USE_RXDATA,
        .length = 8,
    };
    esp_err_t ret;
    uint64_t t_end = esp_timer_get_time() + timeout_ms * 1000;
    do {
        t_rx = SDSPI_MOSI_IDLE_VAL;
        t->rx_data[0] = 0;
        ret = spi_device_transmit(spi_handle(slot), t);
        if (ret != ESP_OK) {
            return ret;
        }
        if ((t->rx_data[0] & TOKEN_RSP_MASK) == TOKEN_RSP_OK) {
            return ESP_OK;
        }
        if ((t->rx_data[0] & TOKEN_RSP_MASK) == TOKEN_RSP_CRC_ERR) {
            return ESP_ERR_INVALID_CRC;
        }
        if ((t->rx_data[0] & TOKEN_RSP_MASK) == TOKEN_RSP_WRITE_ERR) {
            return ESP_ERR_INVALID_RESPONSE;
        }
    } while (esp_timer_get_time() < t_end);

    ESP_LOGD(TAG, "%s: timeout", __func__);
    return ESP_ERR_TIMEOUT;
}

// Wait for data token, reading 8 bytes at a time.
// If the token is found, write all subsequent bytes to extra_ptr,
// and store the number of bytes written to extra_size.
static esp_err_t poll_data_token(int slot, spi_transaction_t* t,
        uint8_t* extra_ptr, size_t* extra_size, int timeout_ms)
{
    uint8_t t_rx[8];
    *t = (spi_transaction_t) {
        .tx_buffer = &t_rx,
        .rx_buffer = &t_rx,
        .length = sizeof(t_rx) * 8,
    };
    esp_err_t ret;
    uint64_t t_end = esp_timer_get_time() + timeout_ms * 1000;
    do {
        memset(t_rx, SDSPI_MOSI_IDLE_VAL, sizeof(t_rx));
        ret = spi_device_transmit(spi_handle(slot), t);
        if (ret != ESP_OK) {
            return ret;
        }
        bool found = false;
        for (int byte_idx = 0; byte_idx < sizeof(t_rx); byte_idx++) {
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
 *
 * With this approach the delay between blocks of a multi-block transfer is
 * ~95 microseconds, out of which 35 microseconds are spend doing the CRC check.
 * Further speedup is possible by pipelining transfers and CRC checks, at an
 * expense of one extra temporary buffer.
 */
static esp_err_t start_command_read_blocks(int slot, sdspi_hw_cmd_t *cmd,
        uint8_t *data, uint32_t rx_length)
{
    bool need_stop_command = rx_length > SDSPI_MAX_DATA_LEN;
    spi_transaction_t* t_command = get_transaction(slot);
    *t_command = (spi_transaction_t) {
        .length = (SDSPI_CMD_R1_SIZE + 8) * 8,
        .tx_buffer = cmd,
        .rx_buffer = cmd,
    };
    esp_err_t ret = spi_device_transmit(spi_handle(slot), t_command);
    if (ret != ESP_OK) {
        return ret;
    }
    release_transaction(slot);

    uint8_t* cmd_u8 = (uint8_t*) cmd;
    size_t pre_scan_data_size = 8;
    uint8_t* pre_scan_data_ptr = cmd_u8 + SDSPI_CMD_R1_SIZE;


    while (rx_length > 0) {
        size_t extra_data_size = 0;
        const uint8_t* extra_data_ptr = NULL;
        bool need_poll = true;

        for (int i = 0; i < pre_scan_data_size; ++i) {
            if (pre_scan_data_ptr[i] == TOKEN_BLOCK_START) {
                extra_data_size = pre_scan_data_size - i - 1;
                extra_data_ptr = pre_scan_data_ptr + i + 1;
                need_poll = false;
                break;
            }
        }

        if (need_poll) {
            // Wait for data to be ready
            spi_transaction_t* t_poll = get_transaction(slot);
            ret = poll_data_token(slot, t_poll, cmd_u8 + SDSPI_CMD_R1_SIZE, &extra_data_size, cmd->timeout_ms);
            release_transaction(slot);
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
        const size_t receive_extra_bytes = 4;
        memset(rx_data, 0xff, will_receive + receive_extra_bytes);
        spi_transaction_t* t_data = get_transaction(slot);
        *t_data = (spi_transaction_t) {
            .length = (will_receive + receive_extra_bytes) * 8,
            .rx_buffer = rx_data,
            .tx_buffer = rx_data
        };

        ret = spi_device_transmit(spi_handle(slot), t_data);
        if (ret != ESP_OK) {
            return ret;
        }
        release_transaction(slot);

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
        if (data_crc_enabled(slot)) {
            crc_of_data = sdspi_crc16(data, will_receive + extra_data_size);
            if (crc_of_data != crc) {
                ESP_LOGE(TAG, "data CRC failed, got=0x%04x expected=0x%04x", crc_of_data, crc);
                esp_log_buffer_hex(TAG, data, 16);
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
        ret = start_command_default(slot, SDSPI_CMD_FLAG_RSP_R1, &stop_cmd);
        if (ret != ESP_OK) {
            return ret;
        }
        spi_transaction_t* t_poll = get_transaction(slot);
        ret = poll_busy(slot, t_poll, cmd->timeout_ms);
        release_transaction(slot);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    return ESP_OK;
}

static esp_err_t start_command_write_blocks(int slot, sdspi_hw_cmd_t *cmd,
        const uint8_t *data, uint32_t tx_length)
{
    if (card_write_protected(slot)) {
        ESP_LOGW(TAG, "%s: card write protected", __func__);
        return ESP_ERR_INVALID_STATE;
    }
    spi_transaction_t* t_command = get_transaction(slot);
    *t_command = (spi_transaction_t) {
        .length = SDSPI_CMD_R1_SIZE * 8,
        .tx_buffer = cmd,
        .rx_buffer = cmd,
    };
    esp_err_t ret = spi_device_queue_trans(spi_handle(slot), t_command, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    uint8_t start_token = tx_length <= SDSPI_MAX_DATA_LEN ?
            TOKEN_BLOCK_START : TOKEN_BLOCK_START_WRITE_MULTI;
    wait_for_transactions(slot);

    while (tx_length > 0) {

        // Write block start token
        spi_transaction_t* t_start_token = get_transaction(slot);
        *t_start_token = (spi_transaction_t) {
            .length = sizeof(start_token) * 8,
            .tx_buffer = &start_token
        };
        esp_err_t ret = spi_device_queue_trans(spi_handle(slot), t_start_token, 0);
        if (ret != ESP_OK) {
            return ret;
        }

        // Prepare data to be sent
        size_t will_send = MIN(tx_length, SDSPI_MAX_DATA_LEN);
        const uint8_t* tx_data = data;
        if (!ptr_dma_compatible(tx_data)) {
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
        spi_transaction_t* t_data = get_transaction(slot);
        *t_data = (spi_transaction_t) {
            .length = will_send * 8,
            .tx_buffer = tx_data,
        };
        ret = spi_device_queue_trans(spi_handle(slot), t_data, 0);
        if (ret != ESP_OK) {
            return ret;
        }

        // Write CRC
        uint16_t crc = sdspi_crc16(data, will_send);
        spi_transaction_t* t_crc = get_transaction(slot);
        *t_crc = (spi_transaction_t) {
            .length = sizeof(crc) * 8,
            .tx_buffer = (uint8_t*) &crc,
        };
        ret = spi_device_queue_trans(spi_handle(slot), t_crc, 0);
        if (ret != ESP_OK) {
            return ret;
        }

        // Wait for data to be sent
        wait_for_transactions(slot);

        // Check if R1 response for the command was correct
        if (cmd->r1 != 0) {
            ESP_LOGD(TAG, "%s: invalid R1 response: 0x%02x", __func__, cmd->r1);
            return ESP_ERR_INVALID_RESPONSE;
        }

        // Poll for response
        spi_transaction_t* t_poll = get_transaction(slot);
        ret = poll_response_token(slot, t_poll, cmd->timeout_ms);
        release_transaction(slot);
        if (ret != ESP_OK) {
            return ret;
        }

        // Wait for the card to finish writing data
        t_poll = get_transaction(slot);
        ret = poll_busy(slot, t_poll, cmd->timeout_ms);
        release_transaction(slot);
        if (ret != ESP_OK) {
            return ret;
        }

        tx_length -= will_send;
        data += will_send;
    }

    if (start_token == TOKEN_BLOCK_START_WRITE_MULTI) {
        uint8_t stop_token[2] = {
                TOKEN_BLOCK_STOP_WRITE_MULTI,
                SDSPI_MOSI_IDLE_VAL
        };
        spi_transaction_t* t_stop_token = get_transaction(slot);
        *t_stop_token = (spi_transaction_t) {
            .length = sizeof(stop_token) * 8,
            .tx_buffer = &stop_token,
        };
        ret = spi_device_queue_trans(spi_handle(slot), t_stop_token, 0);
        if (ret != ESP_OK) {
            return ret;
        }
        wait_for_transactions(slot);

        spi_transaction_t* t_poll = get_transaction(slot);
        ret = poll_busy(slot, t_poll, cmd->timeout_ms);
        release_transaction(slot);
        if (ret != ESP_OK) {
            return ret;
        }
    }

    return ESP_OK;
}
