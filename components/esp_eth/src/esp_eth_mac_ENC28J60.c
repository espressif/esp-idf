#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "esp_err.h"


// Register Masks
#define ADDR_MASK 0x1F
#define BANK_MASK 0x60
#define SPRD_MASK 0x80

// All Banks Registers
#define EIE      0x1B
#define EIR      0x1C
#define ESTAT    0x1D
#define ECON2    0x1E
#define ECON1    0x1F

// Bank 0 Registers
#define ERDPTL   0x00
#define ERDPTH   0x01
#define EWRPTL   0x02
#define EWRPTH   0x03
#define ETXSTL   0x04
#define ETXSTH   0x05
#define ETXNDL   0x06
#define ETXNDH   0x07
#define ERXSTL   0x08
#define ERXSTH   0x09
#define ERXNDL   0x0A
#define ERXNDH   0x0B
#define ERXRDPTL 0x0C
#define ERXRDPTH 0x0D
#define ERXWRPTL 0x0E
#define ERXWRPTH 0x0F
#define EDMASTL  0x10
#define EDMASTH  0x11
#define EDMANDL  0x12
#define EDMANDH  0x13
#define EDMADSTL 0x14
#define EDMADSTH 0x15
#define EDMACSL  0x16
#define EDMACSH  0x17

// Bank 1 Registers
#define EHT0     0x20
#define EHT1     0x21
#define EHT2     0x22
#define EHT3     0x23
#define EHT4     0x24
#define EHT5     0x25
#define EHT6     0x26
#define EHT7     0x27
#define EPMM0    0x28
#define EPMM1    0x29
#define EPMM2    0x2A
#define EPMM3    0x2B
#define EPMM4    0x2C
#define EPMM5    0x2D
#define EPMM6    0x2E
#define EPMM7    0x2F
#define EPMCSL   0x30
#define EPMCSH   0x31
#define EPMOL    0x34
#define EPMOH    0x35
#define EWOLIE   0x36
#define EWOLIR   0x37
#define ERXFCON  0x38
#define EPKTCNT  0x39

// Bank 2 Register
#define MACON1   0xC0
#define MACON2   0xC1
#define MACON3   0xC2
#define MACON4   0xC3
#define MABBIPG  0xC4
#define MAIPGL   0xC6
#define MAIPGH   0xC7
#define MACLCON1 0xC8
#define MACLCON2 0xC9
#define MAMXFLL  0xCA
#define MAMXFLH  0xCB
#define MAPHSUP  0xCD
#define MICMD    0xD2
#define MIREGADR 0xD4
#define MIWRL    0xD6
#define MIWRH    0xD7
#define MIRDL    0xD8
#define MIRDH    0xD9

// Bank 3 Registers
#define MAADR5   0xE0
#define MAADR6   0xE1
#define MAADR3   0xE2
#define MAADR4   0xE3
#define MAADR1   0xE4
#define MAADR2   0xE5
#define EBSTSD   0x66
#define EBSTCON  0x67
#define EBSTCSL  0x68
#define EBSTCSH  0x69
#define MISTAT   0xEA
#define EREVID   0x72
#define ECOCON   0x75
#define EFLOCON  0x77
#define EPAUSL   0x78
#define EPAUSH   0x79

// PHY Registers
#define PHCON1    0x00
#define PHSTAT1   0x01
#define PHHID1    0x02
#define PHHID2    0x03
#define PHCON2    0x10
#define PHSTAT2   0x11
#define PHIE      0x12
#define PHIR      0x13
#define PHLCON    0x14

// ERXFCON bit definitions
#define UCEN      0x80
#define ANDOR     0x40
#define CRCEN     0x20
#define PMEN      0x10
#define MPEN      0x08
#define HTEN      0x04
#define MCEN      0x02
#define BCEN      0x01

// EIE bit definitions
#define INTIE     0x80
#define PKTIE     0x40
#define DMAIE     0x20
#define LINKIE    0x10
#define TXIE      0x08
#define WOLIE     0x04
#define TXERIE    0x02
#define RXERIE    0x01

// EIR bit definitions
#define PKTIF     0x40
#define DMAIF     0x20
#define LINKIF    0x10
#define TXIF      0x08
#define WOLIF     0x04
#define TXERIF    0x02
#define RXERIF    0x01

// ESTAT bit definitions
#define INT       0x80
#define LATECOL   0x10
#define RXBUSY    0x04
#define TXABRT    0x02
#define CLKRDY    0x01

// ECON2 bit definitions
#define AUTOINC   0x80
#define PKTDEC    0x40
#define PWRSV     0x20
#define VRPS      0x08

// ECON1 bit definitions
#define TXRST     0x80
#define RXRST     0x40
#define DMAST     0x20
#define CSUMEN    0x10
#define TXRTS     0x08
#define RXEN      0x04
#define BSEL1     0x02
#define BSEL0     0x01

// MACON1 bit definitions
#define LOOPBK    0x10
#define TXPAUS    0x08
#define RXPAUS    0x04
#define PASSALL   0x02
#define MARXEN    0x01

// MACON2 bit definitions
#define MARST     0x80
#define RNDRST    0x40
#define MARXRST   0x08
#define RFUNRST   0x04
#define MATXRST   0x02
#define TFUNRST   0x01

// MACON3 bit definitions
#define PADCFG2   0x80
#define PADCFG1   0x40
#define PADCFG0   0x20
#define TXCRCEN   0x10
#define PHDRLEN   0x08
#define HFRMLEN   0x04
#define FRMLNEN   0x02
#define FULDPX    0x01

// MICMD bit definitions
#define MIISCAN   0x02
#define MIIRD     0x01

// MISTAT bit definitions
#define NVALID    0x04
#define SCAN      0x02
#define BUSY      0x01

// PHCON1 bit definitions
#define PRST      0x8000
#define PLOOPBK   0x4000
#define PPWRSV    0x0800
#define PDPXMD    0x0100

// PHSTAT1 bit definitions
#define PFDPX     0x1000
#define PHDPX     0x0800
#define LLSTAT    0x0004
#define JBSTAT    0x0002

// PHCON2 bit definitions
#define FRCLINK   0x4000
#define TXDIS     0x2000
#define JABBER    0x0400
#define HDLDIS    0x0100

// Packet Control bit Definitions
#define PHUGEEN   0x08
#define PPADEN    0x04
#define PCRCEN    0x02
#define POVERRIDE 0x01

// SPI Instruction Set
#define RCR 0x00 // Read Control Register
#define RBM 0x3A // Read Buffer Memory
#define WCR 0x40 // Write Control Register
#define WBM 0x7A // Write Buffer Memory
#define BFS 0x80 // Bit Field Set
#define BFC 0xA0 // Bit Field Clear
#define SC  0xFF // Soft Reset

// Buffer
#define MAX_FRAMELEN 1500
const unsigned RXSTART_INIT = 0x0000;
const unsigned RXSTOP_INIT  = 0x19FE;
const unsigned TXSTART_INIT = 0x19FF;
const unsigned TXSTOP_INIT  = 0x1FFF;



static const char *TAG = "emac_ENC28J60";
#define MAC_CHECK(a, str, goto_tag, ret_value, ...)                               \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

#define RX_QUEUE_WAIT_MS (100)
#define ENC28J60_SPI_LOCK_TIMEOUT_MS (50)
#define ENC28J60_PHY_OPERATION_TIMEOUT_US (1000)
static uint8_t ENC28J60_Bank = 0xFF;

typedef struct {
    uint8_t next_packet_low;
    uint8_t next_packet_high;
    uint8_t length_low;
    uint8_t length_high;
    uint8_t status_low;
    uint8_t status_high;
} ENC28J60_rx_header_t;

typedef struct {
    esp_eth_mac_t parent;
    esp_eth_mediator_t *eth;
    spi_device_handle_t spi_hdl;
    SemaphoreHandle_t spi_lock;
    TaskHandle_t rx_task_hdl;
    uint32_t sw_reset_timeout_ms;
    uint8_t addr[6];
    bool packets_remain;
} emac_ENC28J60_t;


static inline bool ENC28J60_lock(emac_ENC28J60_t *emac)
{
    return xSemaphoreTake(emac->spi_lock, ENC28J60_SPI_LOCK_TIMEOUT_MS) == pdTRUE;
}

static inline bool ENC28J60_unlock(emac_ENC28J60_t *emac)
{
    return xSemaphoreGive(emac->spi_lock) == pdTRUE;
}

/**
 * @brief   Set the register bank to which you are reading/writing
 *          Used by register_read, register_write, BFS, BFC
 * @param   eth: ENC28J60 Ethernet mediator
 *          reg_addr: register address to which you want to access
 */
static esp_err_t ENC28J60_set_bank(emac_ENC28J60_t *emac, uint8_t reg_addr)
{
    esp_err_t ret = ESP_OK;
    //check if we need to set the bank
    if ((reg_addr & BANK_MASK) != ENC28J60_Bank) {
        spi_transaction_t trans = {
            .cmd = 0b101,
            .addr = ECON1,
            .length = 8,
            .flags = SPI_TRANS_USE_TXDATA,
            .tx_data[0] = 0x03
        };

        if (ENC28J60_lock(emac)) {
            //First, clear the bank select bits using a BFC on the last two bits of the ECON1 register
            if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
                ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
                ret = ESP_FAIL;
            }
            //Then, set the bank select bits using a BFS on the last two bits of the ECON1 register
            trans.cmd = 0b100;
            trans.tx_data[0] = ((reg_addr & BANK_MASK) >> 5);
            if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
                ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
                ret = ESP_FAIL;
            }
            ENC28J60_unlock(emac);
        } else {
            ret = ESP_ERR_TIMEOUT;
        }
        ENC28J60_Bank = (reg_addr & BANK_MASK);
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ERROR: COULD NOT SET THE BANK!");
    }
    return ret;
}

/**
 * @brief   Bit Field Set
 * @param   eth: ENC28J60 Ethernet mediator
 *          reg_addr: register address to set
 *          value: Which bits should be set
 * @return  ESP_OK: sucess
 *          ESP_FAIL: SPI command fail
 *          ESP_ERR_TIMEOUT: timeout
 *
 *
 * BFS function performs a bit-wise OR operation to set
 * all bits that are a 1 in the argument to be 1 in the register
 *
 * CAN ONLY BE USED FOR ETH REGISTERS (not MAC, MII, PHY registers or buffer)
 *
 * Example: if the register has value       01110110
 * and the argument has value               00110011
 * the specified register will be set to:   01110111
 */
static esp_err_t ENC28J60_BFS(emac_ENC28J60_t *emac, uint8_t reg_addr, uint8_t value)
{
    esp_err_t ret = ESP_OK;
    ret = ENC28J60_set_bank(emac, reg_addr);

    spi_transaction_t trans = {
        .cmd = 0b100,
        .addr = reg_addr,
        .length = 8,
        .flags = SPI_TRANS_USE_TXDATA
    };
    trans.tx_data[0] = value;
    if (ENC28J60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        ENC28J60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief   Bit Field Clear
 * @param   eth: ENC28J60 Ethernet mediator
 *          reg_addr: register address to clear
 *          value: Which bits should be clear
 * @return  ESP_OK: sucess
 *          ESP_FAIL: SPI command fail
 *          ESP_ERR_TIMEOUT: timeout
 *
 * Sets all bits that are a 1 in the argument to be 0 in the register
 *
 * CAN ONLY BE USED FOR ETH REGISTERS (not MAC, MII, PHY registers or buffer)
 *
 * Example: if the register has value       11110001
 * and the argument has value               00010111
 * the specified register will be set to:   11100000
 *
 */
static esp_err_t ENC28J60_BFC(emac_ENC28J60_t *emac, uint8_t reg_addr, uint8_t value)
{
    esp_err_t ret = ESP_OK;
    ret = ENC28J60_set_bank(emac, reg_addr);

    spi_transaction_t trans = {
        .cmd = 0b101,
        .addr = reg_addr,
        .length = 8,
        .flags = SPI_TRANS_USE_TXDATA
    };
    trans.tx_data[0] = value;
    if (ENC28J60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        ENC28J60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief   write value to ENC28J60 internal register
 * @param   eth: ENC28J60 Ethernet mediator
 *          reg_addr: Address of register being read
 *          value: Value written to register
 * @return  ESP_OK: sucess
 *          ESP_FAIL: SPI command fail
 *          ESP_ERR_TIMEOUT: timeout
 */
static esp_err_t ENC28J60_register_write(emac_ENC28J60_t *emac, uint8_t reg_addr, uint8_t value)
{
    esp_err_t ret = ESP_OK;
    ret = ENC28J60_set_bank(emac, reg_addr);

    spi_transaction_t trans = {
        .cmd = 0b010,
        .addr = reg_addr,
        .length = 8,
        .flags = SPI_TRANS_USE_TXDATA
    };
    trans.tx_data[0] = value;

    if (ENC28J60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        ENC28J60_unlock(emac);
        ets_delay_us(10);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief   read value from ENC28J60 internal register
 *
 * @param   mac: ENC28J60 MAC instance
 *          is_eth_reg: 0 if the register is of type ETH, 1 if register is not of type ETH.
 *          reg_addr: Address of register being read
 *          ESP_FAIL: SPI command fail
 *          ESP_ERR_TIMEOUT: timeout
 */
static esp_err_t ENC28J60_register_read(emac_ENC28J60_t *emac, uint8_t is_eth_reg, uint8_t reg_addr, uint8_t *value)
{
    esp_err_t ret = ESP_OK;

    ret = ENC28J60_set_bank(emac, reg_addr);

    spi_transaction_t trans = {
        .cmd = 0b000,
        .addr = reg_addr,
        .length = 8,
        .rxlength = 8,
        .flags = SPI_TRANS_USE_RXDATA
    };
    if (!is_eth_reg) {
        trans.length = 16;
        trans.rxlength = 16;
    }
    if (ENC28J60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        } else {
            if (is_eth_reg) {
                *value = trans.rx_data[0];
            } else {
                *value = trans.rx_data[1];
            }
        }
        ENC28J60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief   verify ENC28J60 chip ID
 * @param   eth: ENC28J60 Ethernet mediator
 * @return  ESP_OK: Device ID returns non-zero value
 *          ESP_ERR_INVALID_VERSION: Cannot read device ID (communication error)
 */
static esp_err_t ENC28J60_verify_id(emac_ENC28J60_t *emac)
{
    esp_err_t ret = ESP_OK;
    uint8_t id;
    MAC_CHECK(ENC28J60_register_read(emac, 1, EREVID, &id) == ESP_OK, "read register failed", err, ESP_FAIL);
    MAC_CHECK(id, "wrong Vendor ID", err, ESP_ERR_INVALID_VERSION);

    ESP_LOGI(TAG, "ENC28J60 Device revision:  %d", id);

    return ESP_OK;
err:
    return ret;
}

/**
 * @brief Write 16-bit value to  ENC28J60 PHY register
 * @param   mac: ENC28J60 MAC instance
 *          NOT_USED: empty (to conform with number of MAC params)
 *          reg_addr: PHY register being written to
 *          reg_value: Value being written to PHY reg
 * @return  ESP_OK: Write success
 *          ESP_FAIL: Cannot write to PHY reg
 */
static esp_err_t ENC28J60_PHY_write(esp_eth_mac_t *mac, uint32_t NOT_USED, uint32_t reg_addr, uint32_t reg_value)
{
    esp_err_t ret = ESP_OK;
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);

    uint8_t data[2];
    uint8_t busy = 1;
    uint8_t MICMD_holder;

    data[0] = (uint8_t)(reg_value & 0x00FF);
    data[1] = (uint8_t)((reg_value & 0xFF00) >> 8);

    //check if the PHY is being read/written to allready
    while (busy) {
        MAC_CHECK(ENC28J60_register_read(emac, 0, MISTAT, &busy) == ESP_OK, "read register failed", err, ESP_FAIL);
        busy &= BUSY;
    }

    MAC_CHECK(ENC28J60_register_write(emac, MIREGADR, reg_addr) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MIWRL, data[0]) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MIWRH, data[1]) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 0, MICMD, &MICMD_holder) == ESP_OK, "read register failed", err, ESP_FAIL);
    MICMD_holder |= MIIRD;
    MAC_CHECK(ENC28J60_register_write(emac, MICMD, MICMD_holder) == ESP_OK, "write register failed", err, ESP_FAIL);

    //wait for task to complete
    ets_delay_us(11);
    busy = 1;
    while (busy) {
        MAC_CHECK(ENC28J60_register_read(emac, 0, MISTAT, &busy) == ESP_OK, "read register failed", err, ESP_FAIL);
        busy &= BUSY;
    }

    return ESP_OK;
err:
    return ret;
}

/**
 * @brief read value from ENC28J60 PHY register
 * @param   mac: device handle
 *          NOT_USED: empty (to conform with number of MAC params)
 *          reg_addr: PHY register being read
 *          reg_value: Pointer to where read value is stored
 * @return  ESP_OK: read success
 *          ESP_FAIL: Cannot read PHY reg
 */
static esp_err_t ENC28J60_PHY_read(esp_eth_mac_t *mac, uint32_t NOT_USED, uint32_t reg_addr, uint32_t *reg_value)
{
    esp_err_t ret = ESP_OK;
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);

    uint8_t data[2];
    uint8_t busy = 1;
    uint8_t MICMD_holder;

    //check if the PHY is being read/written to allready
    while (busy) {
        MAC_CHECK(ENC28J60_register_read(emac, 0, MISTAT, &busy) == ESP_OK, "read register failed", err, ESP_FAIL);
        busy &= BUSY;
    }
    MAC_CHECK(ENC28J60_register_write(emac, MIREGADR, reg_addr) == ESP_OK, "write register failed", err, ESP_FAIL);
    //set MICMD.MIIRD bit to begin read operation
    MAC_CHECK(ENC28J60_register_read(emac, 0, MICMD, &MICMD_holder) == ESP_OK, "read register failed", err, ESP_FAIL);
    MICMD_holder |= MIIRD;
    MAC_CHECK(ENC28J60_register_write(emac, MICMD, MICMD_holder) == ESP_OK, "write register failed", err, ESP_FAIL);
    //wait for task to complete
    busy = 1;
    while (busy) {
        MAC_CHECK(ENC28J60_register_read(emac, 0, MISTAT, &busy) == ESP_OK, "read register failed", err, ESP_FAIL);
        busy &= BUSY;
    }
    //clear MICMD.MIIRD bit
    MICMD_holder &= (~MIIRD);
    MAC_CHECK(ENC28J60_register_write(emac, MICMD, MICMD_holder) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 0, MIRDH, &data[1]) == ESP_OK, "read register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 0, MIRDL, &data[0]) == ESP_OK, "read register failed", err, ESP_FAIL);
    *reg_value = ((uint16_t)data[1] << 8) | data[0];

    return ESP_OK;
err:
    return ret;
}

/**
 * @brief   write buffer to ENC28J60 internal memory (to transmit ETH packets)
 * @param   eth: ENC28J60 Ethernet mediator
 *          buffer: pointer to where the data to be written is stored
 *          len: length of data to write
 * @return  ESP_OK: Write to buffer success
 *          ESP_FAIL: Cannot write to buffer
 */
static esp_err_t ENC28J60_memory_write(emac_ENC28J60_t *emac, uint8_t *buffer, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = 0b011,
        .addr = 0b11010,
        .length = len * 8,
        .tx_buffer = buffer
    };
    if (ENC28J60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        ENC28J60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief   read buffer from ENC28J60 internal memory (to receive ETH packets)
 * @param   eth: ENC28J60 Ethernet mediator
 *          buffer: pointer to where the read data is stored
 *          len: length of read data
 * @return  ESP_OK: Read buffer success
 *          ESP_FAIL: Cannot read buffer
 *
 */
static esp_err_t ENC28J60_memory_read(emac_ENC28J60_t *emac, uint8_t *buffer, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = 0b001,
        .addr = 0b11010,
        .length = len * 8,
        .rxlength = len * 8,
        .rx_buffer = buffer
    };

    if (ENC28J60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        ENC28J60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief   Store MAC address to the Ethernet mediator struct
 * @param   eth: ENC28J60 Ethernet mediator
 * @return  ESP_OK: Read mac address success
 *          ESP_FAIL: Cannot read mac address
 */
static esp_err_t ENC28J60_get_mac_addr(emac_ENC28J60_t *emac)
{
    esp_err_t ret = ESP_OK;

    MAC_CHECK(ENC28J60_register_read(emac, 0, MAADR6, &emac->addr[5]) == ESP_OK, "read register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 0, MAADR5, &emac->addr[4]) == ESP_OK, "read register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 0, MAADR4, &emac->addr[3]) == ESP_OK, "read register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 0, MAADR3, &emac->addr[2]) == ESP_OK, "read register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 0, MAADR2, &emac->addr[1]) == ESP_OK, "read register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 0, MAADR1, &emac->addr[0]) == ESP_OK, "read register failed", err, ESP_FAIL);

    return ESP_OK;
err:
    return ret;
}

/**
 * @brief set new mac address (from ESP32) to internal ENC28J60 registers
 * @param   eth: ENC28J60 Ethernet mediator
 * @return  ESP_OK: Write mac address success
 *          ESP_FAIL: Cannot write mac address
 */
static esp_err_t ENC28J60_set_mac_addr(emac_ENC28J60_t *emac)
{
    esp_err_t ret = ESP_OK;

    uint8_t mac_addr[6];
    memset(mac_addr, 0, sizeof(mac_addr));
    esp_read_mac(mac_addr, 3);
    assert(mac_addr[0] != 0);

    MAC_CHECK(ENC28J60_register_write(emac, MAADR6, mac_addr[5]) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MAADR5, mac_addr[4]) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MAADR4, mac_addr[3]) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MAADR3, mac_addr[2]) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MAADR2, mac_addr[1]) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MAADR1, mac_addr[0]) == ESP_OK, "write register failed", err, ESP_FAIL);

    return ESP_OK;
err:

    return ret;
}

/**
 * @brief   Clear multicast hash table
 * @param   eth: ENC28J60 Ethernet mediator
 * @return  ESP_OK: Clear multicast hash table success
 *          ESP_FAIL: Clear multicast hash table failure
 */
static esp_err_t ENC28J60_clear_multicast_table(emac_ENC28J60_t *emac)
{
    esp_err_t ret = ESP_OK;

    for (int i = 0; i < 7; i++) {
        MAC_CHECK(ENC28J60_BFC(emac, EHT0 + i, 0xFF) == ESP_OK, "BFC failed", err, ESP_FAIL);
    }
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief   ENC28J60 software reset
 * @param   eth: ENC28J60 Ethernet mediator
 * @return  ESP_OK: Reset success
 *          ESP_FAIL: Reset failure
 *
 */
static esp_err_t ENC28J60_reset(emac_ENC28J60_t *emac)
{
    esp_err_t ret = ESP_OK;

    spi_transaction_t trans = {
        .cmd = 0b111,
        .addr = 0b11111,
        .length = 0,
    };
    if (ENC28J60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        } else {
            ENC28J60_Bank = 0xFF;
            ets_delay_us(100);
        }
        ENC28J60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief default setup for ENC28J60 internal registers
 * @param   eth: ENC28J60 Ethernet mediator
 * @return  ESP_OK: Setup success
 *          ESP_FAIL: Setup failure
 */
static esp_err_t ENC28J60_setup_default(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);

    //set up buffer boundaries
    MAC_CHECK(ENC28J60_register_write(emac, ERXSTL, RXSTART_INIT) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, ERXSTH, RXSTART_INIT >> 8) == ESP_OK, "write register failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_write(emac, ERXRDPTL, RXSTART_INIT) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, ERXRDPTH, RXSTART_INIT >> 8) == ESP_OK, "write register failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_write(emac, ERXNDL, RXSTOP_INIT) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, ERXNDH, RXSTOP_INIT >> 8) == ESP_OK, "write register failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_write(emac, ETXSTL, TXSTART_INIT) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, ETXSTH, TXSTART_INIT >> 8) == ESP_OK, "write register failed", err, ESP_FAIL);

    //poll ESTAT.CLKRDY to make sure that the MAC and MII registers are ready to be programmed.

    uint8_t holder;
    MAC_CHECK(ENC28J60_register_read(emac, 1, ESTAT, &holder) == ESP_OK, "read ESTAT.CLKRDY failed", err, ESP_FAIL);

    while ((holder & CLKRDY) == 0) {
        MAC_CHECK(ENC28J60_register_read(emac, 1, ESTAT, &holder) == ESP_OK, "read ESTAT.CLKRDY failed", err, ESP_FAIL);
    }

    MAC_CHECK(ENC28J60_register_write(emac, ERXFCON, 0xE0) == ESP_OK, "write register failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_write(emac, EPMM0, 0x3f) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, EPMM1, 0x30) == ESP_OK, "write register failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_write(emac, EPMCSL, 0xf9) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, EPMCSH, 0xf7) == ESP_OK, "write register failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_write(emac, MACON1, 0x0D) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MACON3, 0xF2) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MACON4, 0x40) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MABBIPG, 0x12) == ESP_OK, "write register failed", err, ESP_FAIL);

    // Stretch pulses for LED, LED_A=Link, LED_B=activity
    MAC_CHECK(ENC28J60_PHY_write(mac, 1, PHLCON, 0x0476) == ESP_OK, "write register failed", err, ESP_FAIL);
    //set PHCON registers
    MAC_CHECK(ENC28J60_PHY_write(mac, 1, PHCON1, 0x0000) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_PHY_write(mac, 1, PHCON2, HDLDIS) == ESP_OK, "write register failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_write(emac, MAIPGL, 0x12) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, MAIPGH, 0x0C) == ESP_OK, "write register failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_write(emac, MABBIPG, 0x12) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_set_mac_addr(emac) == ESP_OK, "set MAC ADDR failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_BFS(emac, ECON2, AUTOINC) == ESP_OK, "BFS failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_BFS(emac, EIE, INTIE | PKTIE) == ESP_OK, "BFS failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief   Start ENC28J60: enable interrupt pin and start ETH receiving packets
 * @param   eth: ENC28J60 Ethernet mediator
 * @return  ESP_OK: Start success
 *          ESP_FAIL: Start failure
 */
static esp_err_t ENC28J60_start(emac_ENC28J60_t *emac)
{
    esp_err_t ret = ESP_OK;

    MAC_CHECK(ENC28J60_register_write(emac, ERXFCON, (UCEN | BCEN | CRCEN)) == ESP_OK, "write register failed", err, ESP_FAIL);
    /* enable interrupt */
    MAC_CHECK(ENC28J60_BFS(emac, EIE, (PKTIE | INTIE)) == ESP_OK, "BFS  failed", err, ESP_FAIL);
    /* enable rx */
    MAC_CHECK(ENC28J60_BFS(emac, ECON1, RXEN) == ESP_OK, "BFS failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_write(emac, ERDPTL, 0x00) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, ERDPTH, 0x00) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_BFS(emac, ECON2, AUTOINC) == ESP_OK, "BFS failed", err, ESP_FAIL);

    return ESP_OK;
err:
    return ret;
}

/**
 * @brief   Stop ENC28J60: disable interrupt pin and stop receiving packets
 * @param   eth: ENC28J60 Ethernet mediator
 * @return  ESP_OK: Stop success
 *          ESP_FAIL: Stop failure
 */
static esp_err_t ENC28J60_stop(emac_ENC28J60_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* disable interrupt */
    MAC_CHECK(ENC28J60_BFC(emac, EIE, (PKTIE | INTIE)) == ESP_OK, "BFC failed", err, ESP_FAIL);
    /* disable rx */
    MAC_CHECK(ENC28J60_BFC(emac, ECON1, RXEN) == ESP_OK, "BFC failed", err, ESP_FAIL);

    return ESP_OK;
err:
    return ret;
}

/**
 * @brief   Interrupt handler
 * @param   arg: Used to pass ENC28J60 Ethernet mediator
 */
static void ENC28J60_isr_handler(void *arg)
{
    emac_ENC28J60_t *emac = (emac_ENC28J60_t *)arg;
    BaseType_t high_task_wakeup = pdFALSE;
    /* notify ENC28J60 task */
    vTaskNotifyGiveFromISR(emac->rx_task_hdl, &high_task_wakeup);
    if (high_task_wakeup != pdFALSE) {
        portYIELD_FROM_ISR();
    }
}

/**
 * @brief   Function to receive the Ethernet packets (called by ENC28J60_isr_handler)
 * @param   *arg: Used to pass ENC28J60 Ethernet mediator
 */
static void emac_ENC28J60_task(void *arg)
{
    emac_ENC28J60_t *emac = (emac_ENC28J60_t *)arg;
    uint8_t status = 0;
    uint8_t *buffer = NULL;
    uint32_t length = 0;

    while (1) {
        if (ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(RX_QUEUE_WAIT_MS))) {
            /* clear interrupt status */
            ENC28J60_register_read(emac, 1, EIR, &status);
            /* packet received */
            if (status & PKTIF) {
                do {
                    buffer = (uint8_t *)heap_caps_malloc(ETH_MAX_PACKET_SIZE, MALLOC_CAP_DMA);

                    if (emac->parent.receive(&emac->parent, buffer, &length) == ESP_OK) {
                        /* pass the buffer to stack (e.g. TCP/IP layer) */
                        emac->eth->stack_input(emac->eth, buffer, length);
                    } else {
                        printf("\n\n\n***FREE***\n\n\n");
                        free(buffer);
                    }
                } while (emac->packets_remain);
            }
        }
    }
    vTaskDelete(NULL);
}

/**
 * @brief   Set mediator for Ethernet MAC
 * @param   mac: ENC28J60 Ethernet MAC instance
 *          eth: ENC28J60 Ethernet mediator
 * @return  ESP_OK: set mediator for Ethernet MAC successfully
 *          ESP_ERR_INVALID_ARG: set mediator for Ethernet MAC failed because of invalid argument
 */
static esp_err_t emac_ENC28J60_set_mediator(esp_eth_mac_t *mac, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(eth, "can't set mac's mediator to null", err, ESP_ERR_INVALID_ARG);
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    emac->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ENC28J60_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(addr, "can't set mac addr to null", err, ESP_ERR_INVALID_ARG);
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    memcpy(emac->addr, addr, 6);
    MAC_CHECK(ENC28J60_set_mac_addr(emac) == ESP_OK, "set mac address failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ENC28J60_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(addr, "can't set mac addr to null", err, ESP_ERR_INVALID_ARG);
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    memcpy(addr, emac->addr, 6);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ENC28J60_set_link(esp_eth_mac_t *mac, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    uint32_t lstat = 0;
    MAC_CHECK(ENC28J60_PHY_read(mac, 1, PHSTAT2, &lstat) == ESP_OK, "read PHY register failed", err, ESP_FAIL);
    switch (link) {
    case ETH_LINK_UP:
        MAC_CHECK(ENC28J60_start(emac) == ESP_OK, "ENC28J60 start failed", err, ESP_FAIL);
        break;
    case ETH_LINK_DOWN:
        MAC_CHECK(!(lstat & 0x0400), "phy is not link down", err, ESP_ERR_INVALID_STATE);
        MAC_CHECK(ENC28J60_stop(emac) == ESP_OK, "ENC28J60 stop failed", err, ESP_FAIL);
        break;
    default:
        MAC_CHECK(false, "unknown link status", err, ESP_ERR_INVALID_ARG);
        break;
    }
    return ESP_OK;
err:
    return ret;
}


/*
 * The ENC28J60 can only support 10 Base-T.
 *
 * Therefore, this command will return an error
 * if the speed is set to anything OTHER than 10 Base-T.
 */
static esp_err_t emac_ENC28J60_set_speed(esp_eth_mac_t *mac, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    switch (speed) {
    case ETH_SPEED_10M:
        break;
    default:
        ESP_LOGE(TAG, "SPEED MUST BE SET TO 10M\n");
        ret = ESP_ERR_INVALID_STATE;
        break;
    }
    return ret;
}

static esp_err_t emac_ENC28J60_set_duplex(esp_eth_mac_t *mac, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    uint8_t mac3 = 0;
    uint32_t phc1 = 0;

    MAC_CHECK(ENC28J60_register_read(emac, 0, MACON3, &mac3) == ESP_OK, "read register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_PHY_read(mac, 1, PHCON1, &phc1) == ESP_OK, "read PHY register failed", err, ESP_FAIL);

    switch (duplex) {
    case ETH_DUPLEX_HALF:
        mac3 &= (~FULDPX);
        phc1 &= (~PDPXMD);

        MAC_CHECK(ENC28J60_register_write(emac, MACON3, mac3) == ESP_OK, "write register failed", err, ESP_FAIL);
        MAC_CHECK(ENC28J60_PHY_write(mac, 1, PHCON1, phc1) == ESP_OK, "write PHY register failed", err, ESP_FAIL);
        break;
    case ETH_DUPLEX_FULL:
        mac3 |= FULDPX;
        phc1 |= PDPXMD;

        MAC_CHECK(ENC28J60_register_write(emac, MACON3, mac3) == ESP_OK, "write register failed", err, ESP_FAIL);
        MAC_CHECK(ENC28J60_PHY_write(mac, 1, PHCON1, phc1) == ESP_OK, "write PHY register failed", err, ESP_FAIL);
        break;
    default:
        MAC_CHECK(false, "unknown duplex", err, ESP_ERR_INVALID_ARG);
        break;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ENC28J60_set_promiscuous(esp_eth_mac_t *mac, bool enable)
{
    esp_err_t ret = ESP_OK;
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    MAC_CHECK(ENC28J60_BFC(emac, ERXFCON, 0xFF) == ESP_OK, "BFC failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ENC28J60_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    uint8_t busy = 1;
    uint8_t pkt_rcvd;

    MAC_CHECK(buf, "can't set buf to null", err, ESP_ERR_INVALID_ARG);
    MAC_CHECK(length, "buf length can't be zero", err, ESP_ERR_INVALID_ARG);

    // Check if last transmit complete
    MAC_CHECK(ENC28J60_register_read(emac, 1, ECON1, &busy) == ESP_OK, "read register failed", err, ESP_FAIL);
    busy &= TXRTS;
    while (busy) {
        MAC_CHECK(ENC28J60_register_read(emac, 1, ECON1, &busy) == ESP_OK, "read register failed", err, ESP_FAIL);
        busy &= TXRTS;
        ESP_LOGW(TAG, "TRANSMIT DELAY, last transmit still in progress");
        MAC_CHECK(ENC28J60_register_read(emac, 1, EIR, &pkt_rcvd) == ESP_OK, "read register failed", err, ESP_FAIL);
        if (pkt_rcvd & TXERIF) {
            MAC_CHECK(ENC28J60_BFS(emac, ECON1, TXRST) == ESP_OK, "BFS failed", err, ESP_FAIL);
            //MAC_CHECK(ENC28J60_BFC(emac, ECON1, TXRST) == ESP_OK, "BFC ECON1 failed", err, ESP_FAIL);
            ESP_LOGE(TAG, "TRANSMIT ERROR!");
        }
    }
    /* set tx length */
    MAC_CHECK(ENC28J60_register_write(emac, EWRPTL, (TXSTART_INIT + 1)) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, EWRPTH, (TXSTART_INIT + 1) >> 8) == ESP_OK, "write register failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_write(emac, ETXNDL, (TXSTART_INIT + length) & 0xFF) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, ETXNDH, (TXSTART_INIT + length) >> 8) == ESP_OK, "write register failed", err, ESP_FAIL);
    /* copy data to tx memory */
    MAC_CHECK(ENC28J60_memory_write(emac, buf, length) == ESP_OK, "buffer memory write failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_BFS(emac, ECON1, TXRTS) == ESP_OK, "BFS failed", err, ESP_FAIL);

    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ENC28J60_receive(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    MAC_CHECK(buf && length, "can't set buf and length to null", err, ESP_ERR_INVALID_ARG);
    uint8_t rxbyte = 0;
    uint16_t rx_len = 0;
    uint16_t current_packet_ptr = 0;
    __attribute__((aligned(4))) ENC28J60_rx_header_t header; // SPI driver needs the rx buffer 4 byte align
    emac->packets_remain = false;

    MAC_CHECK(ENC28J60_BFS(emac, ECON2, PKTDEC) == ESP_OK, "BFS failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 1, ERXFCON, &rxbyte) == ESP_OK, "read register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 1, EPKTCNT, &rxbyte) == ESP_OK, "read register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_read(emac, 1, ERDPTH, &rxbyte) == ESP_OK, "read register failed", err, ESP_FAIL);
    current_packet_ptr = rxbyte << 8;
    MAC_CHECK(ENC28J60_register_read(emac, 1, ERDPTL, &rxbyte) == ESP_OK, "read failed", err, ESP_FAIL);
    current_packet_ptr |= rxbyte;
    MAC_CHECK(ENC28J60_memory_read(emac, (uint8_t *)&header, sizeof(header)) == ESP_OK, "read rx header failed", err, ESP_FAIL);
    rx_len = header.length_low + (header.length_high << 8);

    MAC_CHECK(ENC28J60_memory_read(emac, buf, rx_len) == ESP_OK, "read rx data failed", err, ESP_FAIL);
    *length = rx_len - 4; // substract the CRC length

    MAC_CHECK(ENC28J60_register_write(emac, ERXRDPTL, (current_packet_ptr && 0x00FF)) == ESP_OK, "write register failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_register_write(emac, ERXRDPTH, (current_packet_ptr >> 8)) == ESP_OK, "write register failed", err, ESP_FAIL);

    MAC_CHECK(ENC28J60_register_read(emac, 1, ERDPTL, &rxbyte) == ESP_OK, "read register failed", err, ESP_FAIL);
    if (rxbyte & 1) {
        MAC_CHECK(ENC28J60_register_read(emac, 1, ERDPTL, &rxbyte) == ESP_OK, "read register failed", err, ESP_FAIL);
        MAC_CHECK(ENC28J60_register_write(emac, ERDPTL, (rxbyte + 1)) == ESP_OK, "write register failed", err, ESP_FAIL);
        if (rxbyte == 0xFF) {
            MAC_CHECK(ENC28J60_register_read(emac, 1, ERDPTH, &rxbyte) == ESP_OK, "read register failed", err, ESP_FAIL);
            MAC_CHECK(ENC28J60_register_write(emac, ERDPTH, (rxbyte + 1)) == ESP_OK, "write register failed", err, ESP_FAIL);
        }
    }

    MAC_CHECK(ENC28J60_register_read(emac, 1, EPKTCNT, &rxbyte) == ESP_OK, "read register failed", err, ESP_FAIL);

    emac->packets_remain = (rxbyte > 0);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ENC28J60_init(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    esp_eth_mediator_t *eth = emac->eth;

    /* init gpio used by spi-ethernet interrupt */
    gpio_pad_select_gpio(CONFIG_ENC28J60_INT_GPIO);
    gpio_set_direction(CONFIG_ENC28J60_INT_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(CONFIG_ENC28J60_INT_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(CONFIG_ENC28J60_INT_GPIO, GPIO_INTR_NEGEDGE);
    gpio_intr_enable(CONFIG_ENC28J60_INT_GPIO);
    gpio_isr_handler_add(CONFIG_ENC28J60_INT_GPIO, ENC28J60_isr_handler, emac);
    MAC_CHECK(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL) == ESP_OK, "lowlevel init failed", err, ESP_FAIL);

    //delay 500 us to wait for the clock to be ready
    ets_delay_us(500);
    /* reset ENC28J60 */
    MAC_CHECK(ENC28J60_reset(emac) == ESP_OK, "reset ENC28J60 failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_reset(emac) == ESP_OK, "reset ENC28J60 failed", err, ESP_FAIL);
    /* verify chip id */
    MAC_CHECK(ENC28J60_verify_id(emac) == ESP_OK, "vefiry chip ID failed", err, ESP_FAIL);
    /* default setup of internal registers */
    MAC_CHECK(ENC28J60_setup_default(mac) == ESP_OK, "ENC28J60 default setup failed", err, ESP_FAIL);
    /* clear multicast hash table */
    MAC_CHECK(ENC28J60_clear_multicast_table(emac) == ESP_OK, "clear multicast table failed", err, ESP_FAIL);
    /* get emac address from eeprom */
    MAC_CHECK(ENC28J60_set_mac_addr(emac) == ESP_OK, "fetch ethernet mac address failed", err, ESP_FAIL);
    MAC_CHECK(ENC28J60_get_mac_addr(emac) == ESP_OK, "fetch ethernet mac address failed", err, ESP_FAIL);


    return ESP_OK;
err:
    gpio_isr_handler_remove(CONFIG_ENC28J60_INT_GPIO);
    gpio_reset_pin(CONFIG_ENC28J60_INT_GPIO);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ret;
}

static esp_err_t emac_ENC28J60_deinit(esp_eth_mac_t *mac)
{
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    ENC28J60_stop(emac);
    gpio_isr_handler_remove(CONFIG_ENC28J60_INT_GPIO);
    gpio_reset_pin(CONFIG_ENC28J60_INT_GPIO);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ESP_OK;
}

static esp_err_t emac_ENC28J60_del(esp_eth_mac_t *mac)
{
    emac_ENC28J60_t *emac = __containerof(mac, emac_ENC28J60_t, parent);
    vTaskDelete(emac->rx_task_hdl);
    vSemaphoreDelete(emac->spi_lock);
    free(emac);
    return ESP_OK;
}

esp_eth_mac_t *esp_eth_mac_new_ENC28J60(const eth_ENC28J60_config_t *ENC28J60_config, const eth_mac_config_t *mac_config)
{
    esp_eth_mac_t *ret = NULL;
    MAC_CHECK(mac_config, "can't set mac config to null", err, NULL);
    MAC_CHECK(ENC28J60_config->spi_hdl, "can't set spi handle to null", err, NULL);
    emac_ENC28J60_t *emac = calloc(1, sizeof(emac_ENC28J60_t));
    MAC_CHECK(emac, "calloc emac failed", err, NULL);
    /* bind methods and attributes */
    emac->sw_reset_timeout_ms = mac_config->sw_reset_timeout_ms;
    emac->spi_hdl = ENC28J60_config->spi_hdl;
    emac->parent.set_mediator = emac_ENC28J60_set_mediator;
    emac->parent.init = emac_ENC28J60_init;
    emac->parent.deinit = emac_ENC28J60_deinit;
    emac->parent.del = emac_ENC28J60_del;
    emac->parent.write_phy_reg = ENC28J60_PHY_write;
    emac->parent.read_phy_reg = ENC28J60_PHY_read;
    emac->parent.set_addr = emac_ENC28J60_set_addr;
    emac->parent.get_addr = emac_ENC28J60_get_addr;
    emac->parent.set_speed = emac_ENC28J60_set_speed;
    emac->parent.set_duplex = emac_ENC28J60_set_duplex;
    emac->parent.set_link = emac_ENC28J60_set_link;
    emac->parent.set_promiscuous = emac_ENC28J60_set_promiscuous;
    emac->parent.transmit = emac_ENC28J60_transmit;
    emac->parent.receive = emac_ENC28J60_receive;
    /* create mutex */
    emac->spi_lock = xSemaphoreCreateMutex();
    MAC_CHECK(emac->spi_lock, "create lock failed", err_lock, NULL);
    /* create ENC28J60 task */
    BaseType_t xReturned = xTaskCreate(emac_ENC28J60_task, "ENC28J60_tsk", mac_config->rx_task_stack_size, emac,
                                       mac_config->rx_task_prio, &emac->rx_task_hdl);
    MAC_CHECK(xReturned == pdPASS, "create ENC28J60 task failed", err_tsk, NULL);
    return &(emac->parent);
err_tsk:
    vSemaphoreDelete(emac->spi_lock);
err_lock:
    free(emac);
err:
    return ret;
}
