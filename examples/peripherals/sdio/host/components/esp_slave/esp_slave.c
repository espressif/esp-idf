// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_slave.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "soc/host_reg.h"

static const char TAG[] = "esp_slave";

#define ESP_SLAVE_CMD53_END_ADDR    0x1f800

#define TX_BUFFER_MAX   0x1000
#define TX_BUFFER_MASK  0xFFF
#define RX_BYTE_MAX     0x100000
#define RX_BYTE_MASK    0xFFFFF

#define FUNC1_EN_MASK   (BIT(1))

esp_err_t esp_slave_init_io(esp_slave_context_t *context)
{
    esp_err_t err;
    uint8_t ioe;
    sdmmc_card_t* card = context->card;

    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_ENABLE, &ioe);
    if (err != ESP_OK) return err;
    ESP_LOGD(TAG, "IOE: 0x%02x", ioe);

    uint8_t ior = 0;
    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_READY, &ior);
    if (err != ESP_OK) return err;
    ESP_LOGD(TAG, "IOR: 0x%02x", ior);

    // enable function 1
    ioe |= FUNC1_EN_MASK;
    err = sdmmc_io_write_byte(card, 0, SD_IO_CCCR_FN_ENABLE, ioe, &ioe);
    if (err != ESP_OK) return err;
    ESP_LOGD(TAG, "IOE: 0x%02x", ioe);

    // wait for the card to become ready
    while ((ior & FUNC1_EN_MASK) == 0) {
        err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_READY, &ior);
        if (err != ESP_OK) return err;
        ESP_LOGD(TAG, "IOR: 0x%02x", ior);
    }

    // get interrupt status
    uint8_t ie;
    err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_INT_ENABLE, &ie);
    if (err != ESP_OK) return err;
    ESP_LOGD(TAG,"IE: 0x%02x", ie);

    // enable interrupts for function 1&2 and master enable
    ie |= BIT(0) | FUNC1_EN_MASK;
    err = sdmmc_io_write_byte(card, 0, SD_IO_CCCR_INT_ENABLE, ie, &ie);
    if (err != ESP_OK) return err;
    ESP_LOGD(TAG, "IE: 0x%02x", ie);

    uint16_t bs = 512;
    const uint8_t* bs_u8 = (const uint8_t*) &bs;
    uint16_t bs_read = 0;
    uint8_t* bs_read_u8 = (uint8_t*) &bs_read;
    // Set block sizes for functions 0 to 512 bytes
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, SD_IO_CCCR_BLKSIZEL, &bs_read_u8[0]));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, SD_IO_CCCR_BLKSIZEH, &bs_read_u8[1]));
    ESP_LOGI(TAG, "Function 0 BS: %04x", (int) bs_read);

    ESP_ERROR_CHECK(sdmmc_io_write_byte(card, 0, SD_IO_CCCR_BLKSIZEL, bs_u8[0], NULL));
    ESP_ERROR_CHECK(sdmmc_io_write_byte(card, 0, SD_IO_CCCR_BLKSIZEH, bs_u8[1], NULL));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, SD_IO_CCCR_BLKSIZEL, &bs_read_u8[0]));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, SD_IO_CCCR_BLKSIZEH, &bs_read_u8[1]));
    ESP_LOGI(TAG, "Function 0 BS: %04x", (int) bs_read);

    // Set block sizes for functions 1 to given value (default value = 512).
    if (context->block_size > 0 || context->block_size <= 2048) {
        bs = context->block_size;
    } else {
        bs = 512;
    }
    size_t offset = SD_IO_FBR_START * 1;
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEL, &bs_read_u8[0]));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEH, &bs_read_u8[1]));
    ESP_LOGI(TAG, "Function 1 BS: %04x", (int) bs_read);

    ESP_ERROR_CHECK(sdmmc_io_write_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEL, bs_u8[0], NULL));
    ESP_ERROR_CHECK(sdmmc_io_write_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEH, bs_u8[1], NULL));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEL, &bs_read_u8[0]));
    ESP_ERROR_CHECK(sdmmc_io_read_byte(card, 0, offset + SD_IO_CCCR_BLKSIZEH, &bs_read_u8[1]));
    ESP_LOGI(TAG, "Function 1 BS: %04x", (int) bs_read);

    if (bs_read != context->block_size) {
        ESP_LOGW(TAG, "Function1 block size %d different than set value %d", bs_read, context->block_size);
        context->block_size = bs_read;
    }
    return ESP_OK;
}

esp_err_t esp_slave_wait_for_ioready(esp_slave_context_t *context)
{
    ESP_LOGV(TAG, "wait_for_ioready");
    esp_err_t err;
    sdmmc_card_t *card = context->card;
    // wait for the card to become ready
    uint8_t ior = 0;
    while ((ior & FUNC1_EN_MASK) == 0) {
        err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_READY, &ior);
        if (err != ESP_OK) return err;
        ESP_LOGI(TAG, "IOR: 0x%02x", ior);
    }
    return ESP_OK;
}

static inline esp_err_t esp_slave_write_byte(esp_slave_context_t *context, uint32_t addr, uint8_t val, uint8_t *val_o)
{
   return sdmmc_io_write_byte(context->card, 1, addr&0x3FF, val, val_o);
}

static inline esp_err_t esp_slave_write_bytes(esp_slave_context_t *context, uint32_t addr, uint8_t *val, int len)
{
   return sdmmc_io_write_bytes(context->card, 1, addr&0x3FF, val, len);
}

static inline esp_err_t esp_slave_read_byte(esp_slave_context_t *context, uint32_t addr, uint8_t *val_o)
{
   return sdmmc_io_read_byte(context->card, 1, addr&0x3FF, val_o);
}

static inline esp_err_t esp_slave_read_bytes(esp_slave_context_t *context, uint32_t addr, uint8_t *val_o, int len)
{
   return sdmmc_io_read_bytes(context->card, 1, addr&0x3FF, val_o, len);
}

esp_err_t esp_slave_send_packet(esp_slave_context_t *context, const void* start, size_t length, uint32_t wait_ms)
{
    sdmmc_card_t *card = context->card;
    uint16_t buffer_size = context->buffer_size;
    int buffer_used = (length + buffer_size - 1)/buffer_size;
    esp_err_t err;
    const uint32_t wait_ticks = wait_ms/portTICK_PERIOD_MS;
    uint32_t pre = xTaskGetTickCount();

    assert(length>0);
    for(;;) {
        uint32_t num = 0;
        err = esp_slave_get_tx_buffer_num(context, &num);
        if (err == ESP_OK && num * buffer_size >= length) break;
        if (err != ESP_OK && err != ESP_ERR_TIMEOUT) return err;
        //not error and buffer not enough, retry ``timeout_cnt`` times
        uint32_t now = xTaskGetTickCount();
        if (now-pre >= wait_ticks) {
            ESP_LOGD(TAG, "buffer is not enough: %d, %d required.", num, buffer_used);
            return ESP_ERR_TIMEOUT;
        } else {
            ESP_LOGV(TAG, "buffer is not enough: %d, %d required. Retry...", num, buffer_used);
        }
        vTaskDelay(1);
    }

    ESP_LOGV(TAG, "send_packet: len: %d", length);
    uint8_t *start_ptr = (uint8_t*)start;
    uint32_t len_remain = length;
    do {
        const int block_size = 512;
        /* Though the driver supports to split packet of unaligned size into
         * length of 4x and 1~3, we still send aligned size of data to get
         * higher effeciency. The length is determined by the SDIO address, and
         * the remainning will be discard by the slave hardware.
         */
        int block_n = len_remain/block_size;
        int len_to_send;
        if (block_n) {
            len_to_send = block_n * block_size;
            err = sdmmc_io_write_blocks(card, 1, ESP_SLAVE_CMD53_END_ADDR - len_remain, start_ptr, len_to_send);
        } else {
            len_to_send = len_remain;
            err = sdmmc_io_write_bytes(card, 1, ESP_SLAVE_CMD53_END_ADDR - len_remain, start_ptr, (len_to_send + 3) & (~3));
        }
        if (err != ESP_OK) return err;
        start_ptr += len_to_send;
        len_remain -= len_to_send;
    } while (len_remain);

    context->tx_sent_buffers += buffer_used;
    return ESP_OK;
}

esp_err_t esp_slave_get_packet(esp_slave_context_t *context, void* out_data, size_t size, size_t *out_length, uint32_t wait_ms)
{
    sdmmc_card_t *card = context->card;
    esp_err_t err;
    esp_err_t ret = ESP_OK;
    uint32_t len;
    const uint32_t wait_ticks = wait_ms/portTICK_PERIOD_MS;
    uint32_t pre = xTaskGetTickCount();

    assert (size>0);
    for (;;) {
        err = esp_slave_get_rx_data_size(context, &len);
        if (err == ESP_OK && len > 0) break;
        if (err != ESP_OK && err != ESP_ERR_TIMEOUT) return err;
        //not error and no data, retry ``timeout_cnt`` times.
        uint32_t now = xTaskGetTickCount();
        if (now-pre >= wait_ticks) return ESP_ERR_NOT_FOUND;
        vTaskDelay(1);
    }

    ESP_LOGV(TAG, "get_packet: slave len=%d, max read size=%d", len, size);
    if (len > size) {
       len = size;
       ret = ESP_ERR_NOT_FINISHED;
    }

    uint8_t *start = out_data;
    uint32_t len_remain = len;
    do {
        const int block_size = 512; //currently our driver don't support block size other than 512
        int len_to_send;

        int block_n = len_remain/block_size;
        if (block_n != 0) {
            len_to_send = block_n * block_size;
            err = sdmmc_io_read_blocks(card, 1, ESP_SLAVE_CMD53_END_ADDR - len_remain, start, len_to_send);
        } else {
            len_to_send = len_remain;
            /* though the driver supports to split packet of unaligned size into length
             * of 4x and 1~3, we still get aligned size of data to get higher
             * effeciency. The length is determined by the SDIO address, and the
             * remainning will be ignored by the slave hardware.
             */
            err = sdmmc_io_read_bytes(card, 1, ESP_SLAVE_CMD53_END_ADDR - len_remain, start, (len_to_send + 3) & (~3));
        }
        if (err != ESP_OK) return err;
        start += len_to_send;
        len_remain -= len_to_send;
    } while(len_remain!=0);

    context->rx_got_bytes += len;
    *out_length = len;
    return ret;
}

esp_err_t esp_slave_get_tx_buffer_num(esp_slave_context_t *context, uint32_t* tx_num)
{
    uint32_t len;
    esp_err_t err;

    ESP_LOGV(TAG, "get_tx_buffer_num");
    err = esp_slave_read_bytes(context, HOST_SLC0HOST_TOKEN_RDATA_REG, (uint8_t*)&len, 4);
    if (err != ESP_OK) return err;
    len = (len>>16)&TX_BUFFER_MASK;
    len = (len + TX_BUFFER_MAX - context->tx_sent_buffers)%TX_BUFFER_MAX;
    *tx_num = len;
    return ESP_OK;
}

esp_err_t esp_slave_get_rx_data_size(esp_slave_context_t *context, uint32_t* rx_size)
{
    uint32_t len;
    esp_err_t err;

    ESP_LOGV(TAG, "get_rx_data_size: got_bytes: %d", context->rx_got_bytes);
    err = esp_slave_read_bytes(context, HOST_SLCHOST_PKT_LEN_REG, (uint8_t*)&len, 4);
    if (err != ESP_OK) return err;
    len &= RX_BYTE_MASK;
    len = (len + RX_BYTE_MAX - context->rx_got_bytes)%RX_BYTE_MAX;
    *rx_size = len;
    return ESP_OK;
}

esp_err_t esp_slave_clear_intr(esp_slave_context_t *context, uint32_t intr_mask)
{
    ESP_LOGV(TAG, "clear_intr: %08X", intr_mask);
    return esp_slave_write_bytes(context, HOST_SLC0HOST_INT_CLR_REG, (uint8_t*)&intr_mask, 4);
}

esp_err_t esp_slave_get_intr(esp_slave_context_t *context, uint32_t *intr_raw, uint32_t *intr_st)
{
    esp_err_t r;
    ESP_LOGV(TAG, "get_intr");
    if (intr_raw == NULL && intr_st == NULL) return ESP_ERR_INVALID_ARG;

    if (intr_raw != NULL) {
        r= esp_slave_read_bytes(context, HOST_SLC0HOST_INT_RAW_REG, (uint8_t*)intr_raw, 4);
        if (r != ESP_OK) return r;
    }
    if (intr_st != NULL) {
        r = esp_slave_read_bytes(context, HOST_SLC0HOST_INT_ST_REG, (uint8_t*)intr_st, 4);
        if (r != ESP_OK) return r;
    }
    return ESP_OK;
}

esp_err_t esp_slave_set_intr_ena(esp_slave_context_t *context, uint32_t ena_mask)
{
    ESP_LOGV(TAG, "set_intr_ena: %08X", ena_mask);
    return esp_slave_write_bytes(context, HOST_SLC0HOST_INT_ENA_REG, (uint8_t*)&ena_mask, 4);
}

esp_err_t esp_slave_get_intr_ena(esp_slave_context_t *context, uint32_t *ena_mask_o)
{
    ESP_LOGV(TAG, "get_intr_ena");
    esp_err_t ret = esp_slave_read_bytes(context, HOST_SLC0HOST_INT_ENA_REG, (uint8_t*)ena_mask_o, 4);
    ESP_LOGV(TAG, "ena: %08X", *ena_mask_o);
    return ret;
}

esp_err_t esp_slave_write_reg(esp_slave_context_t *context, uint8_t addr, uint8_t value, uint8_t* value_o)
{
    ESP_LOGV(TAG, "write_reg: %08X", value);
    // addrress over range
    if (addr >= 64) return ESP_ERR_INVALID_ARG;
    // reserved for interrupts
    if (addr >= 28 && addr <= 31) return ESP_ERR_INVALID_ARG;
    return esp_slave_write_byte(context, HOST_SLCHOST_CONF_W_REG(addr), value, value_o);
}

esp_err_t esp_slave_read_reg(esp_slave_context_t *context, uint8_t add, uint8_t *value_o)
{
    ESP_LOGV(TAG, "read_reg");
    // address over range
    if (add >= 64) return ESP_ERR_INVALID_ARG;
    esp_err_t ret = esp_slave_read_byte(context, HOST_SLCHOST_CONF_W_REG(add), value_o);
    ESP_LOGV(TAG, "reg: %08X", *value_o);
    return ret;
}

esp_err_t esp_slave_send_slave_intr(esp_slave_context_t *context, uint8_t intr_mask)
{
    ESP_LOGV(TAG, "send_slave_intr: %02x", intr_mask);
    return esp_slave_write_byte(context, HOST_SLCHOST_CONF_W7_REG+0, intr_mask, NULL);
}


