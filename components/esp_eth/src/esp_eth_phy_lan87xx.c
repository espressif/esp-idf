/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth.h"
#include "eth_phy_regs_struct.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"

static const char *TAG = "lan87xx";

/***************List of Supported Models***************/

// See Microchip's Application Note AN25.3 summarizing differences among below models
#define LAN8710A_MODEL_NUM 0x0F
#define LAN8720A_MODEL_NUM 0x0F
#define LAN8740A_MODEL_NUM 0x11
#define LAN8741A_MODEL_NUM 0x12
#define LAN8742A_MODEL_NUM 0x13

static const uint8_t supported_models[] = {
    LAN8710A_MODEL_NUM,
#if (LAN8710A_MODEL_NUM != LAN8720A_MODEL_NUM)
    LAN8720A_MODEL_NUM,
#endif
    LAN8740A_MODEL_NUM,
    LAN8741A_MODEL_NUM,
    LAN8742A_MODEL_NUM
};

/***************Vendor Specific Register***************/

/**
 * @brief MCSR(Mode Control Status Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 1;                  /* Reserved */
        uint32_t energy_is_on : 1;               /* Energy is On */
        uint32_t reserved2 : 4;                  /* Reserved */
        uint32_t en_alternate_interrupt : 1;     /* Enable Alternate Interrupt Mode */
        uint32_t reserved3 : 2;                  /* Reserved */
        uint32_t en_far_loopback : 1;            /* Enable Far Loopback Mode */
        uint32_t reserved4 : 3;                  /* Reserved */
        uint32_t en_energy_detect_powerdown : 1; /* Enable Energy Detect Power Down */
        uint32_t reserved5 : 2;                  /* Reserved */
    };
    uint32_t val;
} mcsr_reg_t;
#define ETH_PHY_MCSR_REG_ADDR (0x11)

/**
 * @brief SMR(Special Modes Register)
 *
 */
typedef union {
    struct {
        uint32_t phy_addr : 5;   /* PHY Address */
        uint32_t mode : 3;       /* Transceiver Mode of Operation */
        uint32_t reserved_1 : 6; /* Reserved */
        uint32_t mii_mode : 1;   /* Mode of the digital interface (only LAN8710A/LAN8740A/LAN8741A) */
        uint32_t reserved_2 : 1; /* Reserved */
    };
    uint32_t val;
} smr_reg_t;
#define ETH_PHY_SMR_REG_ADDR (0x12)

/**
 * @brief Time Domain Reflectometry Patterns/Delay Control Register
 * Only available in LAN8740A/LAN8742A
 */
typedef union {
    struct {
        uint32_t tdr_pattern_low : 6;        /* Data pattern sent in TDR mode for the low cycle */
        uint32_t tdr_pattern_high : 6;       /* Data pattern sent in TDR mode for the high cycle */
        uint32_t tdr_line_break_counter : 3; /* Increments of 256ms of break time */
        uint32_t tdr_delay_in : 1;           /* Line break counter used */
    };
    uint32_t val;
} tdr_pattern_reg_t;
#define EHT_PHY_TDRPD_REG_ADDR (0x18)

/**
 * @brief Time Domain Reflectometry Control/Status Register)
 * Only available in LAN8740A/LAN8742A
 */
typedef union {
    struct {
        uint32_t tdr_channel_length : 8;     /* TDR channel length */
        uint32_t tdr_channel_status : 1;     /* TDR channel status */
        uint32_t tdr_channel_cable_type : 2; /* TDR channel cable type */
        uint32_t reserved : 3;               /* Reserved */
        uint32_t tdr_a2d_filter_enable: 1;   /* Analog to Digital Filter Enabled */
        uint32_t tdr_enable : 1;             /* Enable TDR */
    };
    uint32_t val;
} tdr_control_reg_t;
#define EHT_PHY_TDRC_REG_ADDR (0x19)

/**
 * @brief SECR(Symbol Error Counter Register)
 *
 */
typedef union {
    struct {
        uint32_t symbol_err_count : 16; /* Symbol Error Counter */
    };
    uint32_t val;
} secr_reg_t;
#define EHT_PHY_SECR_REG_ADDR (0x1A)

/**
 * @brief CSIR(Control Status Indications Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 4;         /* Reserved */
        uint32_t base10_t_polarity : 1; /* Polarity State of 10Base-T */
        uint32_t reserved2 : 6;         /* Reserved */
        uint32_t dis_sqe : 1;           /* Disable SQE test(Heartbeat) */
        uint32_t reserved3 : 1;         /* Reserved */
        uint32_t select_channel : 1;    /* Manual channel select:MDI(0) or MDIX(1) */
        uint32_t reserved4 : 1;         /* Reserved */
        uint32_t auto_mdix_ctrl : 1;    /* Auto-MDIX Control: EN(0) or DE(1) */
    };
    uint32_t val;
} scsir_reg_t;
#define ETH_PHY_CSIR_REG_ADDR (0x1B)

/**
 * @brief Cable Length Register
 * Only available in LAN8740A/LAN8742A
 */
typedef union {
    struct {
        uint32_t reserved : 12;              /* Reserved */
        uint32_t cable_length : 4;           /* Cable length */
    };
    uint32_t val;
} cbln_reg_t;
#define EHT_PHY_CBLN_REG_ADDR (0x1C)

/**
 * @brief ISR(Interrupt Source Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 1;                /* Reserved */
        uint32_t auto_nego_page_received : 1;  /* Auto-Negotiation Page Received */
        uint32_t parallel_detect_fault : 1;    /* Parallel Detection Fault */
        uint32_t auto_nego_lp_acknowledge : 1; /* Auto-Negotiation LP Acknowledge */
        uint32_t link_down : 1;                /* Link Down */
        uint32_t remote_fault_detect : 1;      /* Remote Fault Detect */
        uint32_t auto_nego_complete : 1;       /* Auto-Negotiation Complete */
        uint32_t energy_on_generate : 1;       /* ENERGY ON generated */
        uint32_t wake_on_lan : 1;              /* Wake on Lan (WOL) event detected (only LAN8740A/LAN8742A) */
        uint32_t reserved2 : 7;                /* Reserved */
    };
    uint32_t val;
} isfr_reg_t;
#define ETH_PHY_ISR_REG_ADDR (0x1D)

/**
 * @brief IMR(Interrupt Mask Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 1;                /* Reserved */
        uint32_t auto_nego_page_received : 1;  /* Auto-Negotiation Page Received */
        uint32_t parallel_detect_fault : 1;    /* Parallel Detection Fault */
        uint32_t auto_nego_lp_acknowledge : 1; /* Auto-Negotiation LP Acknowledge */
        uint32_t link_down : 1;                /* Link Down */
        uint32_t remote_fault_detect : 1;      /* Remote Fault Detect */
        uint32_t auto_nego_complete : 1;       /* Auto-Negotiation Complete */
        uint32_t energy_on_generate : 1;       /* ENERGY ON generated */
        uint32_t wake_on_lan : 1;              /* Wake on Lan (WOL) event detected (only LAN8740A/LAN8742A) */
        uint32_t reserved2 : 7;                /* Reserved */
    };
    uint32_t val;
} imr_reg_t;
#define ETH_PHY_IMR_REG_ADDR (0x1E)

/**
 * @brief PSCSR(PHY Special Control Status Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved1 : 2;        /* Reserved */
        uint32_t speed_indication : 3; /* Speed Indication */
        uint32_t reserved2 : 1;        /* Reserved */
        uint32_t enable_4b5b : 1;      /* Enable 4B5B encoder (only LAN8740A/LAN8741A) */
        uint32_t reserved3 : 5;        /* Reserved */
        uint32_t auto_nego_done : 1;   /* Auto Negotiation Done */
        uint32_t reserved4 : 3;        /* Reserved */
    };
    uint32_t val;
} pscsr_reg_t;
#define ETH_PHY_PSCSR_REG_ADDR (0x1F)

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    int addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
    int reset_gpio_num;
} phy_lan87xx_t;

static esp_err_t lan87xx_update_link_duplex_speed(phy_lan87xx_t *lan87xx)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = lan87xx->eth;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    bmsr_reg_t bmsr;
    pscsr_reg_t pscsr;
    uint32_t peer_pause_ability = false;
    anlpar_reg_t anlpar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    eth_link_t link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    /* check if link status changed */
    if (lan87xx->link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_PSCSR_REG_ADDR, &(pscsr.val)), err, TAG, "read PSCSR failed");
            switch (pscsr.speed_indication) {
            case 1: //10Base-T half-duplex
                speed = ETH_SPEED_10M;
                duplex = ETH_DUPLEX_HALF;
                break;
            case 2: //100Base-TX half-duplex
                speed = ETH_SPEED_100M;
                duplex = ETH_DUPLEX_HALF;
                break;
            case 5: //10Base-T full-duplex
                speed = ETH_SPEED_10M;
                duplex = ETH_DUPLEX_FULL;
                break;
            case 6: //100Base-TX full-duplex
                speed = ETH_SPEED_100M;
                duplex = ETH_DUPLEX_FULL;
                break;
            default:
                break;
            }
            ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_SPEED, (void *)speed), err, TAG, "change speed failed");
            ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_DUPLEX, (void *)duplex), err, TAG, "change duplex failed");
            /* if we're in duplex mode, and peer has the flow control ability */
            if (duplex == ETH_DUPLEX_FULL && anlpar.symmetric_pause) {
                peer_pause_ability = 1;
            } else {
                peer_pause_ability = 0;
            }
            ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_PAUSE, (void *)peer_pause_ability), err, TAG, "change pause ability failed");
        }
        ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link), err, TAG, "change link failed");
        lan87xx->link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mediator to null");
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    lan87xx->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(lan87xx_update_link_duplex_speed(lan87xx), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_reset(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    lan87xx->link_status = ETH_LINK_DOWN;
    esp_eth_mediator_t *eth = lan87xx->eth;
    bmcr_reg_t bmcr = {.reset = 1};
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, lan87xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* wait for reset complete */
    uint32_t to = 0;
    for (to = 0; to < lan87xx->reset_timeout_ms / 10; to++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        if (!bmcr.reset) {
            break;
        }
    }
    ESP_GOTO_ON_FALSE(to < lan87xx->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "reset timeout");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_reset_hw(esp_eth_phy_t *phy)
{
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    if (lan87xx->reset_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(lan87xx->reset_gpio_num);
        gpio_set_direction(lan87xx->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(lan87xx->reset_gpio_num, 0);
        /* assert nRST signal on LAN87xx a little longer than the minimum specified in datasheet */
        esp_rom_delay_us(150);
        gpio_set_level(lan87xx->reset_gpio_num, 1);
    }
    return ESP_OK;
}

/**
 * @note This function is responsible for restarting a new auto-negotiation,
 *       the result of negotiation won't be relected to uppler layers.
 *       Instead, the negotiation result is fetched by linker timer, see `lan87xx_get_link()`
 */
static esp_err_t lan87xx_negotiate(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    esp_eth_mediator_t *eth = lan87xx->eth;
    /* in case any link status has changed, let's assume we're in link down status */
    lan87xx->link_status = ETH_LINK_DOWN;
    /* Restart auto negotiation */
    bmcr_reg_t bmcr = {
        .speed_select = 1,     /* 100Mbps */
        .duplex_mode = 1,      /* Full Duplex */
        .en_auto_nego = 1,     /* Auto Negotiation */
        .restart_auto_nego = 1 /* Restart Auto Negotiation */
    };
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, lan87xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    /* Wait for auto negotiation complete */
    bmsr_reg_t bmsr;
    pscsr_reg_t pscsr;
    uint32_t to = 0;
    for (to = 0; to < lan87xx->autonego_timeout_ms / 100; to++) {
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_PSCSR_REG_ADDR, &(pscsr.val)), err, TAG, "read PSCSR failed");
        if (bmsr.auto_nego_complete && pscsr.auto_nego_done) {
            break;
        }
    }
    /* Auto negotiation failed, maybe no network cable plugged in, so output a warning */
    if (to >= lan87xx->autonego_timeout_ms / 100 && (lan87xx->link_status == ETH_LINK_UP)) {
        ESP_LOGW(TAG, "auto negotiation timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    esp_eth_mediator_t *eth = lan87xx->eth;
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (!enable) {
        /* General Power Down Mode */
        bmcr.power_down = 1;
    } else {
        /* Normal operation Mode */
        bmcr.power_down = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, lan87xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    if (!enable) {
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.power_down == 1, ESP_FAIL, err, TAG, "power down failed");
    } else {
        /* wait for power up complete */
        uint32_t to = 0;
        for (to = 0; to < lan87xx->reset_timeout_ms / 10; to++) {
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
            if (bmcr.power_down == 0) {
                break;
            }
        }
        ESP_GOTO_ON_FALSE(to < lan87xx->reset_timeout_ms / 10, ESP_FAIL, err, TAG, "power up timeout");
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    lan87xx->addr = addr;
    return ESP_OK;
}

static esp_err_t lan87xx_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "addr can't be null");
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    *addr = lan87xx->addr;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_del(esp_eth_phy_t *phy)
{
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    free(lan87xx);
    return ESP_OK;
}

static esp_err_t lan87xx_advertise_pause_ability(esp_eth_phy_t *phy, uint32_t ability)
{
    esp_err_t ret = ESP_OK;
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    esp_eth_mediator_t *eth = lan87xx->eth;
    /* Set PAUSE function ability */
    anar_reg_t anar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_ANAR_REG_ADDR, &(anar.val)), err, TAG, "read ANAR failed");
    if (ability) {
        anar.asymmetric_pause = 1;
        anar.symmetric_pause = 1;
    } else {
        anar.asymmetric_pause = 0;
        anar.symmetric_pause = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, lan87xx->addr, ETH_PHY_ANAR_REG_ADDR, anar.val), err, TAG, "write ANAR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    esp_eth_mediator_t *eth = lan87xx->eth;
    /* Set Loopback function */
    bmcr_reg_t bmcr;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    if (enable) {
        bmcr.en_loopback = 1;
    } else {
        bmcr.en_loopback = 0;
    }
    ESP_GOTO_ON_ERROR(eth->phy_reg_write(eth, lan87xx->addr, ETH_PHY_BMCR_REG_ADDR, bmcr.val), err, TAG, "write BMCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_lan87xx_t *lan87xx = __containerof(phy, phy_lan87xx_t, parent);
    esp_eth_mediator_t *eth = lan87xx->eth;
    // Detect PHY address
    if (lan87xx->addr == ESP_ETH_PHY_ADDR_AUTO) {
        ESP_GOTO_ON_ERROR(esp_eth_detect_phy_addr(eth, &lan87xx->addr), err, TAG, "Detect PHY address failed");
    }
    /* Power on Ethernet PHY */
    ESP_GOTO_ON_ERROR(lan87xx_pwrctl(phy, true), err, TAG, "power control failed");
    /* Reset Ethernet PHY */
    ESP_GOTO_ON_ERROR(lan87xx_reset(phy), err, TAG, "reset failed");
    /* Check PHY ID */
    phyidr1_reg_t id1;
    phyidr2_reg_t id2;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_IDR1_REG_ADDR, &(id1.val)), err, TAG, "read ID1 failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, lan87xx->addr, ETH_PHY_IDR2_REG_ADDR, &(id2.val)), err, TAG, "read ID2 failed");
    ESP_GOTO_ON_FALSE(id1.oui_msb == 0x7 && id2.oui_lsb == 0x30, ESP_FAIL, err, TAG, "wrong chip ID");
    bool supported_model = false;
    for (unsigned int i = 0; i < sizeof(supported_models); i++) {
        if (id2.vendor_model == supported_models[i]) {
            supported_model = true;
            break;
        }
    }
    ESP_GOTO_ON_FALSE(supported_model, ESP_FAIL, err, TAG, "unsupported chip model");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_deinit(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    /* Power off Ethernet PHY */
    ESP_GOTO_ON_ERROR(lan87xx_pwrctl(phy, false), err, TAG, "power control failed");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_lan87xx(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    ESP_GOTO_ON_FALSE(config, NULL, err, TAG, "can't set phy config to null");
    phy_lan87xx_t *lan87xx = calloc(1, sizeof(phy_lan87xx_t));
    ESP_GOTO_ON_FALSE(lan87xx, NULL, err, TAG, "calloc lan87xx failed");
    lan87xx->addr = config->phy_addr;
    lan87xx->reset_gpio_num = config->reset_gpio_num;
    lan87xx->reset_timeout_ms = config->reset_timeout_ms;
    lan87xx->link_status = ETH_LINK_DOWN;
    lan87xx->autonego_timeout_ms = config->autonego_timeout_ms;
    lan87xx->parent.reset = lan87xx_reset;
    lan87xx->parent.reset_hw = lan87xx_reset_hw;
    lan87xx->parent.init = lan87xx_init;
    lan87xx->parent.deinit = lan87xx_deinit;
    lan87xx->parent.set_mediator = lan87xx_set_mediator;
    lan87xx->parent.negotiate = lan87xx_negotiate;
    lan87xx->parent.get_link = lan87xx_get_link;
    lan87xx->parent.pwrctl = lan87xx_pwrctl;
    lan87xx->parent.get_addr = lan87xx_get_addr;
    lan87xx->parent.set_addr = lan87xx_set_addr;
    lan87xx->parent.loopback = lan87xx_loopback;
    lan87xx->parent.advertise_pause_ability = lan87xx_advertise_pause_ability;
    lan87xx->parent.del = lan87xx_del;

    return &(lan87xx->parent);
err:
    return ret;
}
