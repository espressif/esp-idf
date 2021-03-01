// Copyright (c) 2021 Vladimir Chistyakov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include "ksz8851.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"

#include <string.h>

static const char *TAG = "ksz8851snl-mac";

#define MAC_CHECK(a, str, goto_tag, ret_value, ...)                               \
    do {                                                                          \
        if (!(a)) {                                                               \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

typedef enum {
    KSZ8851_SPI_COMMAND_READ_REG   = 0x0U,
    KSZ8851_SPI_COMMAND_WRITE_REG  = 0x1U,
    KSZ8851_SPI_COMMAND_READ_FIFO  = 0x2U,
    KSZ8851_SPI_COMMAND_WRITE_FIFO = 0x3U,
} ksz8851_spi_commands_t;

static const unsigned KSZ8851_SPI_COMMAND_BITS    = 2U;
static const unsigned KSZ8851_SPI_ADDR_SHIFT      = 2U;
static const unsigned KSZ8851_SPI_BYTE_MASK_SHIFT = 8U + KSZ8851_SPI_ADDR_SHIFT;
static const unsigned KSZ8851_SPI_LOCK_TIMEOUT_MS = 500U;

typedef enum {
    KSZ8851_QMU_PACKET_LENGTH  = 2000U,
    KSZ8851_QMU_PACKET_PADDING = 16U,
} ksz8851_qmu_packet_size_t;

static const uint16_t RXDTTR_VALUE  = 0x03E8U;
static const uint16_t RXDBCTR_VALUE = 0x1000U;
static const uint16_t RXFCT_VALUE   = 0X0001U;

IRAM_ATTR static void ksz8851_isr_handler(void *arg)
{
    emac_ksz8851snl_t *emac     = (emac_ksz8851snl_t *)arg;
    BaseType_t high_task_wakeup = pdFALSE;
    vTaskNotifyGiveFromISR(emac->rx_task_hdl, &high_task_wakeup);
    if (high_task_wakeup != pdFALSE) {
        portYIELD_FROM_ISR();
    }
}

static inline bool ksz8851_mutex_lock(emac_ksz8851snl_t *emac)
{
    return xSemaphoreTakeRecursive(emac->spi_lock, pdMS_TO_TICKS(KSZ8851_SPI_LOCK_TIMEOUT_MS)) == pdTRUE;
}

static inline bool ksz8851_mutex_unlock(emac_ksz8851snl_t *emac)
{
    return xSemaphoreGiveRecursive(emac->spi_lock) == pdTRUE;
}

static esp_err_t ksz8851_read_reg(emac_ksz8851snl_t *emac, uint32_t address, uint16_t *value)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(value != NULL, "out pointer must not be null", err, ESP_ERR_INVALID_ARG);
    MAC_CHECK((address & ~KSZ8851_VALID_ADDRESS_MASK) == 0U, "address is out of bounds", err, ESP_ERR_INVALID_ARG);

    const unsigned data_size = 16U; // NOTE(v.chistyakov): bits
    // NOTE(v.chistyakov): select upper or lower word inside a dword
    const unsigned byte_mask = 0x3U << (KSZ8851_SPI_BYTE_MASK_SHIFT + (address & 0x2U));
    address <<= KSZ8851_SPI_ADDR_SHIFT;

    spi_transaction_ext_t trans = {
        .base.flags   = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY | SPI_TRANS_USE_RXDATA,
        .base.cmd     = KSZ8851_SPI_COMMAND_READ_REG,
        .base.addr    = address | byte_mask,
        .base.length  = data_size,
        .command_bits = KSZ8851_SPI_COMMAND_BITS,
        .address_bits = 16 - KSZ8851_SPI_COMMAND_BITS,
    };
    if (ksz8851_mutex_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans.base) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        ksz8851_mutex_unlock(emac);
        memcpy(value, trans.base.rx_data, data_size >> 3U);
        ESP_LOGV(TAG, "reading reg 0x%02x == 0x%04x", address, *value);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
err:
    return ret;
}

static esp_err_t ksz8851_write_reg(emac_ksz8851snl_t *emac, uint32_t address, uint16_t value)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK((address & ~KSZ8851_VALID_ADDRESS_MASK) == 0U, "address is out of bounds", err, ESP_ERR_INVALID_ARG);
    ESP_LOGV(TAG, "writing reg 0x%02x = 0x%04x", address, value);

    const unsigned data_size = 16U; // NOTE(v.chistyakov): bits
    // NOTE(v.chistyakov): select upper or lower word inside a dword
    const unsigned byte_mask = 0x3U << (KSZ8851_SPI_BYTE_MASK_SHIFT + (address & 0x2U));
    address <<= KSZ8851_SPI_ADDR_SHIFT;

    spi_transaction_ext_t trans = {
        .base.flags   = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY | SPI_TRANS_USE_TXDATA,
        .base.cmd     = KSZ8851_SPI_COMMAND_WRITE_REG,
        .base.addr    = address | byte_mask,
        .base.length  = data_size,
        .command_bits = KSZ8851_SPI_COMMAND_BITS,
        .address_bits = 16 - KSZ8851_SPI_COMMAND_BITS,
    };

    memcpy(trans.base.tx_data, &value, data_size >> 3U);
    if (ksz8851_mutex_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans.base) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        ksz8851_mutex_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
err:
    return ret;
}

static esp_err_t ksz8851_set_bits(emac_ksz8851snl_t *emac, uint32_t address, uint16_t value)
{
    esp_err_t ret = ESP_OK;
    uint16_t old;
    MAC_CHECK(ksz8851_read_reg(emac, address, &old) == ESP_OK, "failed to read reg 0x%x", err, ESP_FAIL, address);
    old |= value;
    MAC_CHECK(ksz8851_write_reg(emac, address, old) == ESP_OK, "failed to write reg 0x%x", err, ESP_FAIL, address);
err:
    return ret;
}

static esp_err_t ksz8851_clear_bits(emac_ksz8851snl_t *emac, uint32_t address, uint16_t value)
{
    esp_err_t ret = ESP_OK;
    uint16_t old;
    MAC_CHECK(ksz8851_read_reg(emac, address, &old) == ESP_OK, "failed to read reg 0x%x", err, ESP_FAIL, address);
    old &= ~value;
    MAC_CHECK(ksz8851_write_reg(emac, address, old) == ESP_OK, "failed to write reg 0x%x", err, ESP_FAIL, address);
err:
    return ret;
}

static esp_err_t emac_ksz8851_set_mediator(esp_eth_mac_t *mac, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(eth, "mediator can not be null", err, ESP_ERR_INVALID_ARG);
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    emac->eth               = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ksz8851_init(esp_eth_mac_t *mac)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    esp_rom_gpio_pad_select_gpio(emac->int_gpio_num);
    gpio_set_direction(emac->int_gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(emac->int_gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(emac->int_gpio_num, GPIO_INTR_NEGEDGE); // NOTE(v.chistyakov): active low
    gpio_intr_enable(emac->int_gpio_num);
    gpio_isr_handler_add(emac->int_gpio_num, ksz8851_isr_handler, emac);
    MAC_CHECK(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL) == ESP_OK, "lowlevel init failed", err, ESP_FAIL);
    // NOTE(v.chistyakov): soft reset
    {
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_PMECR, PMECR_WAKEUP_TO_NORMAL) == ESP_OK, "PMECR write failed", err,
                  ESP_FAIL);

        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_GRR, GRR_GLOBAL_SOFT_RESET) == ESP_OK, "GRR write failed", err, ESP_FAIL);
        vTaskDelay(pdMS_TO_TICKS(emac->sw_reset_timeout_ms));

        MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_GRR, GRR_GLOBAL_SOFT_RESET) == ESP_OK, "GRR write failed", err,
                  ESP_FAIL);
        vTaskDelay(pdMS_TO_TICKS(emac->sw_reset_timeout_ms));
    }
    // NOTE(v.chistyakov): verify chip id
    {
        uint16_t id;
        MAC_CHECK(ksz8851_read_reg(emac, KSZ8851_CIDER, &id) == ESP_OK, "CIDER read failed", err, ESP_FAIL);
        uint8_t family_id = (id & CIDER_FAMILY_ID_MASK) >> CIDER_FAMILY_ID_SHIFT;
        uint8_t chip_id   = (id & CIDER_CHIP_ID_MASK) >> CIDER_CHIP_ID_SHIFT;
        uint8_t revision  = (id & CIDER_REVISION_ID_MASK) >> CIDER_REVISION_ID_SHIFT;
        ESP_LOGI(TAG, "Family ID = 0x%x\t Chip ID = 0x%x\t Revision ID = 0x%x", family_id, chip_id, revision);
        MAC_CHECK(family_id == CIDER_KSZ8851SNL_FAMILY_ID, "wrong family id", err, ESP_FAIL);
        MAC_CHECK(chip_id == CIDER_KSZ8851SNL_CHIP_ID, "wrong chip id", err, ESP_FAIL);
    }
    // NOTE(v.chistyakov): set default values
    {
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_TXFDPR, TXFDPR_TXFPAI) == ESP_OK, "TXFDPR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_TXCR,
                                   TXCR_TXFCE | TXCR_TXPE | TXCR_TXCE | TXCR_TCGICMP | TXCR_TCGIP | TXCR_TCGTCP) == ESP_OK,
                  "TXCR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXFDPR, RXFDPR_RXFPAI) == ESP_OK, "RXFDPR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXFCTR, RXFCT_VALUE) == ESP_OK, "RXFCTR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXDTTR, RXDTTR_VALUE) == ESP_OK, "RXDTTR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXDBCTR, RXDBCTR_VALUE) == ESP_OK, "RXDBCTR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXCR1,
                                   RXCR1_RXUDPFCC | RXCR1_RXTCPFCC | RXCR1_RXIPFCC | RXCR1_RXPAFMA | RXCR1_RXFCE |
                                   RXCR1_RXBE | RXCR1_RXUE) == ESP_OK,
                  "RXCR1 write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXCR2,
                                   (4 << RXCR2_SRDBL_SHIFT) | RXCR2_IUFFP | RXCR2_RXIUFCEZ | RXCR2_UDPLFE |
                                   RXCR2_RXICMPFCC) == ESP_OK,
                  "RXCR2 write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXQCR, RXQCR_RXIPHTOE | RXQCR_RXFCTE | RXQCR_ADRFE) == ESP_OK,
                  "RXQCR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_P1CR, P1CR_FORCE_DUPLEX) == ESP_OK, "P1CR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_P1CR, P1CR_RESTART_AN) == ESP_OK, "P1CR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_ISR, ISR_ALL) == ESP_OK, "ISR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_IER,
                                   IER_LCIE | IER_TXIE | IER_RXIE | IER_LDIE | IER_SPIBEIE | IER_RXOIE) == ESP_OK,
                  "IER write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_TXQCR, TXQCR_AETFE) == ESP_OK, "TXQCR write failed", err, ESP_FAIL);
    }
    ESP_LOGD(TAG, "MAC initialized");
    return ESP_OK;
err:
    ESP_LOGD(TAG, "MAC initialization failed");
    gpio_isr_handler_remove(emac->int_gpio_num);
    gpio_reset_pin(emac->int_gpio_num);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ret;
}

static esp_err_t emac_ksz8851_deinit(esp_eth_mac_t *mac)
{
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    mac->stop(mac);
    gpio_isr_handler_remove(emac->int_gpio_num);
    gpio_reset_pin(emac->int_gpio_num);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    ESP_LOGD(TAG, "MAC deinitialized");
    return ESP_OK;
}

static esp_err_t emac_ksz8851_start(esp_eth_mac_t *mac)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_TXCR, TXCR_TXE) == ESP_OK, "TXCR write failed", err, ESP_FAIL);
    MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXCR1, RXCR1_RXE) == ESP_OK, "RXCR1 write failed", err, ESP_FAIL);
    ESP_LOGD(TAG, "MAC started");
err:
    return ret;
}

static esp_err_t emac_ksz8851_stop(esp_eth_mac_t *mac)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_TXCR, TXCR_TXE) == ESP_OK, "TXCR write failed", err, ESP_FAIL);
    MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_RXCR1, RXCR1_RXE) == ESP_OK, "RXCR1 write failed", err, ESP_FAIL);
    ESP_LOGD(TAG, "MAC stopped");
err:
    return ret;
}

static esp_err_t emac_ksz8851snl_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    static unsigned s_frame_id = 0U;

    ESP_LOGV(TAG, "transmitting frame of size %u", length);
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    if (!ksz8851_mutex_lock(emac)) {
        return ESP_ERR_TIMEOUT;
    }

    MAC_CHECK(length <= KSZ8851_QMU_PACKET_LENGTH, "packet is too big", err, ESP_ERR_INVALID_ARG);
    // NOTE(v.chistyakov): 4 bytes header + length aligned to 4 bytes
    unsigned transmit_length = 4U + ((length + 3U) & ~0x3U);

    uint16_t free_space;
    MAC_CHECK(ksz8851_read_reg(emac, KSZ8851_TXMIR, &free_space) == ESP_OK, "TXMIR read failed", err, ESP_FAIL);
    MAC_CHECK(transmit_length <= free_space, "TXQ free space (%d) < send length (%d)", err, ESP_FAIL, free_space,
              transmit_length);

    emac->tx_buffer[0] = ++s_frame_id & TXSR_TXFID_MASK;
    emac->tx_buffer[1] = 0x80U;
    emac->tx_buffer[2] = length & 0xFFU;
    emac->tx_buffer[3] = (length >> 8U) & 0xFFU;
    memcpy(emac->tx_buffer + 4U, buf, length);

    spi_transaction_ext_t trans = {
        .base.flags     = SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_CMD,
        .base.cmd       = KSZ8851_SPI_COMMAND_WRITE_FIFO,
        .base.length    = transmit_length * 8U, // NOTE(v.chistyakov): bits
        .base.tx_buffer = emac->tx_buffer,
        .command_bits   = 2U,
        .address_bits   = 6U,
    };

    uint16_t ier;
    MAC_CHECK(ksz8851_read_reg(emac, KSZ8851_IER, &ier) == ESP_OK, "IER read failed", err, ESP_FAIL);
    MAC_CHECK(ksz8851_write_reg(emac, KSZ8851_IER, 0) == ESP_OK, "IER write failed", err, ESP_FAIL);

    MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXQCR, RXQCR_SDA) == ESP_OK, "RXQCR write failed", err, ESP_FAIL);
    if (spi_device_polling_transmit(emac->spi_hdl, &trans.base) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ret = ESP_FAIL;
    }
    MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_RXQCR, RXQCR_SDA) == ESP_OK, "RXQCR write failed", err, ESP_FAIL);

    MAC_CHECK(ksz8851_write_reg(emac, KSZ8851_IER, ier) == ESP_OK, "IER write failed", err, ESP_FAIL);
err:
    ksz8851_mutex_unlock(emac);
    return ret;
}

static esp_err_t emac_ksz8851_receive(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    if (!ksz8851_mutex_lock(emac)) {
        return ESP_ERR_TIMEOUT;
    }

    MAC_CHECK(buf, "receive buffer can not be null", err, ESP_ERR_INVALID_ARG);
    MAC_CHECK(length, "receive buffer length can not be null", err, ESP_ERR_INVALID_ARG);
    MAC_CHECK(*length > 0U, "receive buffer length must be greater than zero", err, ESP_ERR_INVALID_ARG);

    uint16_t header_status;
    MAC_CHECK(ksz8851_read_reg(emac, KSZ8851_RXFHSR, &header_status) == ESP_OK, "RXFHSR read failed", err, ESP_FAIL);

    uint16_t byte_count;
    MAC_CHECK(ksz8851_read_reg(emac, KSZ8851_RXFHBCR, &byte_count) == ESP_OK, "RXFHBCR read failed", err, ESP_FAIL);
    byte_count &= RXFHBCR_RXBC_MASK;

    // NOTE(v.chistyakov): do not include 2 bytes padding at the beginning and 4 bytes CRC at the end
    const unsigned frame_size = byte_count - 6U;
    MAC_CHECK(frame_size <= *length, "frame size is greater than length", err, ESP_FAIL);

    if (header_status & RXFHSR_RXFV) {
        // NOTE(v.chistyakov): 4 dummy + 4 header + alignment
        const unsigned receive_size = 8U + ((byte_count + 3U) & ~0x3U);
        spi_transaction_ext_t trans = {
            .base.flags     = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_DUMMY,
            .base.cmd       = KSZ8851_SPI_COMMAND_READ_FIFO,
            .base.length    = receive_size * 8U, // NOTE(v.chistyakov): bits
            .base.rx_buffer = emac->rx_buffer,
            .command_bits   = 2U,
            .address_bits   = 6U,
        };

        MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_RXFDPR, RXFDPR_RXFP_MASK) == ESP_OK, "RXFDPR write failed", err,
                  ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXQCR, RXQCR_SDA) == ESP_OK, "RXQCR write failed", err, ESP_FAIL);
        if (spi_device_polling_transmit(emac->spi_hdl, &trans.base) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_RXQCR, RXQCR_SDA) == ESP_OK, "RXQCR write failed", err, ESP_FAIL);

        // NOTE(v.chistyakov): skip 4 dummy, 4 header, 2 padding
        memcpy(buf, emac->rx_buffer + 10U, frame_size);
        *length = frame_size;
        ESP_LOGV(TAG, "received frame of size %u", frame_size);
    } else if (header_status & (RXFHSR_RXCE | RXFHSR_RXRF | RXFHSR_RXFTL | RXFHSR_RXMR | RXFHSR_RXUDPFCS | RXFHSR_RXTCPFCS |
                                RXFHSR_RXIPFCS | RXFHSR_RXICMPFCS)) {
        // NOTE(v.chistyakov): RRXEF is a self-clearing bit
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXQCR, RXQCR_RRXEF) == ESP_OK, "RXQCR write failed", err, ESP_FAIL);
        *length = 0U;
    }
err:
    ksz8851_mutex_unlock(emac);
    return ret;
}

static esp_err_t emac_ksz8851_read_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    MAC_CHECK(reg_value, "reg_value can not be null", err, ESP_ERR_INVALID_ARG);
    uint16_t tmp_val;
    MAC_CHECK(ksz8851_read_reg(emac, phy_reg, &tmp_val) == ESP_OK, "read PHY register failed", err, ESP_FAIL);
    *reg_value = tmp_val;
err:
    return ret;
}

static esp_err_t emac_ksz8851_write_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    MAC_CHECK(ksz8851_write_reg(emac, phy_reg, (uint16_t)reg_value) == ESP_OK, "write PHY register failed", err,
              ESP_FAIL);
err:
    return ret;
}

static esp_err_t emac_ksz8851_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    MAC_CHECK(addr, "addr can not be null", err, ESP_ERR_INVALID_ARG);
    uint16_t MARL = addr[5] | ((uint16_t)(addr[4]) << 8);
    uint16_t MARM = addr[3] | ((uint16_t)(addr[2]) << 8);
    uint16_t MARH = addr[1] | ((uint16_t)(addr[0]) << 8);
    MAC_CHECK(ksz8851_write_reg(emac, KSZ8851_MARL, MARL) == ESP_OK, "MARL write failed", err, ESP_FAIL);
    MAC_CHECK(ksz8851_write_reg(emac, KSZ8851_MARM, MARM) == ESP_OK, "MARM write failed", err, ESP_FAIL);
    MAC_CHECK(ksz8851_write_reg(emac, KSZ8851_MARH, MARH) == ESP_OK, "MARH write failed", err, ESP_FAIL);
    ESP_LOGD(TAG, "set MAC address to %02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2], addr[3], addr[4],
             addr[5]);
err:
    return ret;
}

static esp_err_t emac_ksz8851_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    MAC_CHECK(addr, "addr can not be null", err, ESP_ERR_INVALID_ARG);
    uint16_t MARL, MARM, MARH;
    MAC_CHECK(ksz8851_read_reg(emac, KSZ8851_MARL, &MARL) == ESP_OK, "MARL read failed", err, ESP_FAIL);
    MAC_CHECK(ksz8851_read_reg(emac, KSZ8851_MARM, &MARM) == ESP_OK, "MARM read failed", err, ESP_FAIL);
    MAC_CHECK(ksz8851_read_reg(emac, KSZ8851_MARH, &MARH) == ESP_OK, "MARH read failed", err, ESP_FAIL);
    addr[0] = (MARH >> 8) & 0xFF;
    addr[1] = MARH & 0xFF;
    addr[2] = (MARM >> 8) & 0xFF;
    addr[3] = MARM & 0xFF;
    addr[4] = (MARL >> 8) & 0xFF;
    addr[5] = MARL & 0xFF;
err:
    return ret;
}

static esp_err_t emac_ksz8851_set_speed(esp_eth_mac_t *mac, eth_speed_t speed)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    switch (speed) {
    case ETH_SPEED_100M:
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_P1CR, P1CR_FORCE_SPEED) == ESP_OK, "P1CR write failed", err, ESP_FAIL);
        ESP_LOGD(TAG, "set speed to 100M");
        break;
    case ETH_SPEED_10M:
        MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_P1CR, P1CR_FORCE_SPEED) == ESP_OK, "P1CR write failed", err, ESP_FAIL);
        ESP_LOGD(TAG, "set speed to 10M");
        break;
    default: MAC_CHECK(false, "unknown speed", err, ESP_ERR_INVALID_ARG); break;
    }
err:
    return ret;
}

static esp_err_t emac_ksz8851_set_duplex(esp_eth_mac_t *mac, eth_duplex_t duplex)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    switch (duplex) {
    case ETH_DUPLEX_FULL:
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_P1CR, P1CR_FORCE_DUPLEX) == ESP_OK, "P1CR write failed", err, ESP_FAIL);
        ESP_LOGD(TAG, "set duplex to full");
        break;
    case ETH_DUPLEX_HALF:
        MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_P1CR, P1CR_FORCE_DUPLEX) == ESP_OK, "P1CR write failed", err, ESP_FAIL);
        ESP_LOGD(TAG, "set duplex to half");
        break;
    default: MAC_CHECK(false, "unknown duplex", err, ESP_ERR_INVALID_ARG); break;
    }
err:
    return ret;
}

static esp_err_t emac_ksz8851_set_link(esp_eth_mac_t *mac, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    switch (link) {
    case ETH_LINK_UP:
        MAC_CHECK(mac->start(mac) == ESP_OK, "ksz8851 start failed", err, ESP_FAIL);
        ESP_LOGD(TAG, "link is up");
        break;
    case ETH_LINK_DOWN:
        MAC_CHECK(mac->stop(mac) == ESP_OK, "ksz8851 stop failed", err, ESP_FAIL);
        ESP_LOGD(TAG, "link is down");
        break;
    default: MAC_CHECK(false, "unknown link status", err, ESP_ERR_INVALID_ARG); break;
    }
err:
    return ret;
}

static esp_err_t emac_ksz8851_set_promiscuous(esp_eth_mac_t *mac, bool enable)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    uint16_t rxcr1;
    MAC_CHECK(ksz8851_read_reg(emac, KSZ8851_RXCR1, &rxcr1) == ESP_OK, "RXCR1 read failed", err, ESP_FAIL);
    if (enable) {
        // NOTE(v.chistyakov): set promiscuous mode
        ESP_LOGD(TAG, "setting promiscuous mode");
        rxcr1 |= RXCR1_RXINVF | RXCR1_RXAE;
        rxcr1 &= ~(RXCR1_RXPAFMA | RXCR1_RXMAFMA);
    } else {
        // NOTE(v.chistyakov): set hash perfect (default)
        ESP_LOGD(TAG, "setting hash perfect mode");
        rxcr1 |= RXCR1_RXPAFMA;
        rxcr1 &= ~(RXCR1_RXINVF | RXCR1_RXAE | RXCR1_RXMAFMA);
    }
    MAC_CHECK(ksz8851_write_reg(emac, KSZ8851_RXCR1, rxcr1) == ESP_OK, "RXCR1 write failed", err, ESP_FAIL);
err:
    return ret;
}

static esp_err_t emac_ksz8851_enable_flow_ctrl(esp_eth_mac_t *mac, bool enable)
{
    esp_err_t ret           = ESP_OK;
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    if (enable) {
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_TXCR, TXCR_TXFCE) == ESP_OK, "TXCR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_set_bits(emac, KSZ8851_RXCR1, RXCR1_RXFCE) == ESP_OK, "RXCR write failed", err, ESP_FAIL);
        ESP_LOGD(TAG, "flow control enabled");
    } else {
        MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_TXCR, TXCR_TXFCE) == ESP_OK, "TXCR write failed", err, ESP_FAIL);
        MAC_CHECK(ksz8851_clear_bits(emac, KSZ8851_RXCR1, RXCR1_RXFCE) == ESP_OK, "RXCR write failed", err, ESP_FAIL);
        ESP_LOGD(TAG, "flow control disabled");
    }
err:
    return ret;
}

static esp_err_t emac_ksz8851_set_peer_pause_ability(esp_eth_mac_t *mac, uint32_t ability)
{
    // NOTE(v.chistyakov): peer's pause ability is determined with auto-negotiation
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t emac_ksz8851_del(esp_eth_mac_t *mac)
{
    emac_ksz8851snl_t *emac = __containerof(mac, emac_ksz8851snl_t, parent);
    vTaskDelete(emac->rx_task_hdl);
    vSemaphoreDelete(emac->spi_lock);
    heap_caps_free(emac->rx_buffer);
    heap_caps_free(emac->tx_buffer);
    free(emac);
    return ESP_OK;
}

static void emac_ksz8851snl_task(void *arg)
{
    emac_ksz8851snl_t *emac = (emac_ksz8851snl_t *)arg;
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        uint16_t interrupt_status;
        ksz8851_read_reg(emac, KSZ8851_ISR, &interrupt_status);
        ksz8851_write_reg(emac, KSZ8851_ISR, ISR_ALL);

        if (interrupt_status & ISR_LCIS) {
            ESP_LOGD(TAG, "Link Change Interrupt");
        }
        if (interrupt_status & ISR_TXIS) {
            ESP_LOGD(TAG, "TX Interrupt");
        }
        if (interrupt_status & ISR_RXOIS) {
            ESP_LOGD(TAG, "RX Overrun Interrupt");
        }
        if (interrupt_status & ISR_TXPSIS) {
            ESP_LOGD(TAG, "TX Process Stopped Interrupt");
        }
        if (interrupt_status & ISR_RXPSIS) {
            ESP_LOGD(TAG, "RX Process Stopped Interrupt");
        }
        if (interrupt_status & ISR_TXSAIS) {
            ESP_LOGD(TAG, "TX Space Available Interrupt");
        }
        if (interrupt_status & ISR_RXWFDIS) {
            ESP_LOGD(TAG, "RX Wakeup Frame Detect Interrupt");
        }
        if (interrupt_status & ISR_RXMPDIS) {
            ESP_LOGD(TAG, "RX Magic Packet Detect Interrupt");
        }
        if (interrupt_status & ISR_LDIS) {
            ESP_LOGD(TAG, "Linkup Detect Interrupt");
            ksz8851_set_bits(emac, KSZ8851_PMECR, PMECR_WAKEUP_LINK);
        }
        if (interrupt_status & ISR_EDIS) {
            ESP_LOGD(TAG, "Energy Detect Interrupt");
        }
        if (interrupt_status & ISR_SPIBEIS) {
            ESP_LOGD(TAG, "SPI Bus Error Interrupt");
        }
        if (interrupt_status & ISR_RXIS) {
            ESP_LOGD(TAG, "RX Interrupt");

            uint16_t ier;
            ksz8851_read_reg(emac, KSZ8851_IER, &ier);
            ksz8851_write_reg(emac, KSZ8851_IER, 0);

            uint16_t frame_count = 0;
            ksz8851_read_reg(emac, KSZ8851_RXFCTR, &frame_count);
            frame_count = (frame_count & RXFCTR_RXFC_MASK) >> RXFCTR_RXFC_SHIFT;

            while (frame_count--) {
                uint32_t length = ETH_MAX_PACKET_SIZE;
                uint8_t *packet = malloc(ETH_MAX_PACKET_SIZE);
                if (!packet) {
                    continue;
                }

                if (emac->parent.receive(&emac->parent, packet, &length) == ESP_OK && length) {
                    emac->eth->stack_input(emac->eth, packet, length);
                    // NOTE(v.chistyakov): the packet is freed in the upper layers
                } else {
                    free(packet);
                    ksz8851_clear_bits(emac, KSZ8851_RXCR1, RXCR1_RXE);
                    ksz8851_set_bits(emac, KSZ8851_RXCR1, RXCR1_FRXQ);
                    ksz8851_clear_bits(emac, KSZ8851_RXCR1, RXCR1_FRXQ);
                    ksz8851_set_bits(emac, KSZ8851_RXCR1, RXCR1_RXE);
                }
            }
            ksz8851_write_reg(emac, KSZ8851_IER, ier);
        }
    }
    vTaskDelete(NULL);
}

esp_eth_mac_t *esp_eth_mac_new_ksz8851snl(const eth_ksz8851snl_config_t *ksz8851snl_config,
        const eth_mac_config_t *mac_config)
{
    esp_eth_mac_t *ret      = NULL;
    emac_ksz8851snl_t *emac = NULL;

    MAC_CHECK(ksz8851snl_config && mac_config, "arguments can not be null", err, NULL);
    MAC_CHECK(ksz8851snl_config->int_gpio_num >= 0, "invalid interrupt gpio number", err, NULL);

    emac = calloc(1, sizeof(emac_ksz8851snl_t));
    MAC_CHECK(emac, "no mem for MAC instance", err, NULL);

    emac->sw_reset_timeout_ms           = mac_config->sw_reset_timeout_ms;
    emac->int_gpio_num                  = ksz8851snl_config->int_gpio_num;
    emac->spi_hdl                       = ksz8851snl_config->spi_hdl;
    emac->parent.set_mediator           = emac_ksz8851_set_mediator;
    emac->parent.init                   = emac_ksz8851_init;
    emac->parent.deinit                 = emac_ksz8851_deinit;
    emac->parent.start                  = emac_ksz8851_start;
    emac->parent.stop                   = emac_ksz8851_stop;
    emac->parent.transmit               = emac_ksz8851snl_transmit;
    emac->parent.receive                = emac_ksz8851_receive;
    emac->parent.read_phy_reg           = emac_ksz8851_read_phy_reg;
    emac->parent.write_phy_reg          = emac_ksz8851_write_phy_reg;
    emac->parent.set_addr               = emac_ksz8851_set_addr;
    emac->parent.get_addr               = emac_ksz8851_get_addr;
    emac->parent.set_speed              = emac_ksz8851_set_speed;
    emac->parent.set_duplex             = emac_ksz8851_set_duplex;
    emac->parent.set_link               = emac_ksz8851_set_link;
    emac->parent.set_promiscuous        = emac_ksz8851_set_promiscuous;
    emac->parent.enable_flow_ctrl       = emac_ksz8851_enable_flow_ctrl;
    emac->parent.set_peer_pause_ability = emac_ksz8851_set_peer_pause_ability;
    emac->parent.del                    = emac_ksz8851_del;
    emac->spi_lock                      = xSemaphoreCreateRecursiveMutex();
    MAC_CHECK(emac->spi_lock, "create lock failed", err, NULL);
    emac->rx_buffer = NULL;
    emac->tx_buffer = NULL;
    emac->rx_buffer = heap_caps_malloc(KSZ8851_QMU_PACKET_LENGTH + KSZ8851_QMU_PACKET_PADDING, MALLOC_CAP_DMA);
    emac->tx_buffer = heap_caps_malloc(KSZ8851_QMU_PACKET_LENGTH + KSZ8851_QMU_PACKET_PADDING, MALLOC_CAP_DMA);
    MAC_CHECK(emac->rx_buffer, "RX buffer allocation failed", err, NULL);
    MAC_CHECK(emac->tx_buffer, "TX buffer allocation failed", err, NULL);

    BaseType_t core_num = tskNO_AFFINITY;
    if (mac_config->flags & ETH_MAC_FLAG_PIN_TO_CORE) {
        core_num = cpu_hal_get_core_id();
    }
    BaseType_t xReturned = xTaskCreatePinnedToCore(emac_ksz8851snl_task, "ksz8851snl_tsk", mac_config->rx_task_stack_size,
                                                   emac, mac_config->rx_task_prio, &emac->rx_task_hdl, core_num);
    MAC_CHECK(xReturned == pdPASS, "create ksz8851 task failed", err, NULL);
    return &(emac->parent);

err:
    if (emac) {
        if (emac->rx_task_hdl) {
            vTaskDelete(emac->rx_task_hdl);
        }
        if (emac->spi_lock) {
            vSemaphoreDelete(emac->spi_lock);
        }
        if (emac->spi_lock) {
            vSemaphoreDelete(emac->spi_lock);
        }
        // NOTE(v.chistyakov): safe to call with NULL
        heap_caps_free(emac->rx_buffer);
        heap_caps_free(emac->tx_buffer);
        free(emac);
    }
    return ret;
}
