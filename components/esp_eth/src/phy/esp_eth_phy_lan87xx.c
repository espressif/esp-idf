/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth_phy_802_3.h"

static const char *TAG = "lan87xx";

/* It was observed that assert nRST signal on LAN87xx needs to be a little longer than the minimum specified in datasheet */
#define LAN87XX_PHY_RESET_ASSERTION_TIME_US 150

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
    phy_802_3_t phy_802_3;
} phy_lan87xx_t;

static esp_err_t lan87xx_update_link_duplex_speed(phy_lan87xx_t *lan87xx)
{
    esp_err_t ret = ESP_OK;
    esp_eth_mediator_t *eth = lan87xx->phy_802_3.eth;
    uint32_t addr = lan87xx->phy_802_3.addr;
    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;
    bmsr_reg_t bmsr;
    bmcr_reg_t bmcr;
    pscsr_reg_t pscsr;
    uint32_t peer_pause_ability = false;
    anlpar_reg_t anlpar;
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_ANLPAR_REG_ADDR, &(anlpar.val)), err, TAG, "read ANLPAR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_BMSR_REG_ADDR, &(bmsr.val)), err, TAG, "read BMSR failed");
    ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
    /* link status is forced up because LAN87xx reports link down when loopback is enabled and cable is unplugged */
    eth_link_t link;
    if(bmcr.en_loopback) {
        link = ETH_LINK_UP;
    } else {
        link = bmsr.link_status ? ETH_LINK_UP : ETH_LINK_DOWN;
    }
    /* check if link status changed */
    if (lan87xx->phy_802_3.link_status != link) {
        /* when link up, read negotiation result */
        if (link == ETH_LINK_UP) {
            ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, addr, ETH_PHY_PSCSR_REG_ADDR, &(pscsr.val)), err, TAG, "read PSCSR failed");
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
        lan87xx->phy_802_3.link_status = link;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_get_link(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_lan87xx_t *lan87xx = __containerof(esp_eth_phy_into_phy_802_3(phy), phy_lan87xx_t, phy_802_3);
    /* Updata information about link, speed, duplex */
    ESP_GOTO_ON_ERROR(lan87xx_update_link_duplex_speed(lan87xx), err, TAG, "update link duplex speed failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t lan87xx_autonego_ctrl(esp_eth_phy_t *phy, eth_phy_autoneg_cmd_t cmd, bool *autonego_en_stat)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);
    esp_eth_mediator_t *eth = phy_802_3->eth;
    if (cmd == ESP_ETH_PHY_AUTONEGO_EN) {
        bmcr_reg_t bmcr;
        ESP_GOTO_ON_ERROR(eth->phy_reg_read(eth, phy_802_3->addr, ETH_PHY_BMCR_REG_ADDR, &(bmcr.val)), err, TAG, "read BMCR failed");
        ESP_GOTO_ON_FALSE(bmcr.en_loopback == 0, ESP_ERR_INVALID_STATE, err, TAG, "Autonegotiation can't be enabled while in loopback operation");
    }
    return esp_eth_phy_802_3_autonego_ctrl(phy_802_3, cmd, autonego_en_stat);
err:
    return ret;
}

static esp_err_t lan87xx_loopback(esp_eth_phy_t *phy, bool enable)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);
    bool auto_nego_en;
    ESP_GOTO_ON_ERROR(lan87xx_autonego_ctrl(phy, ESP_ETH_PHY_AUTONEGO_G_STAT, &auto_nego_en), err, TAG, "get status of autonegotiation failed");
    ESP_GOTO_ON_FALSE(!(auto_nego_en && enable), ESP_ERR_INVALID_STATE, err, TAG, "Unable to set loopback while autonegotiation is enabled. Disable it to use loopback");
    return esp_eth_phy_802_3_loopback(phy_802_3, enable);
err:
    return ret;
}

static esp_err_t lan87xx_set_speed(esp_eth_phy_t *phy, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);

    /* It was observed that a delay needs to be introduced after setting speed and prior driver's start.
    Otherwise, the very first read of PHY registers is not valid data (0xFFFF's). */
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_set_speed(phy_802_3, speed), err, TAG, "set speed failed");
    vTaskDelay(pdMS_TO_TICKS(10));
err:
    return ret;
}

static esp_err_t lan87xx_init(esp_eth_phy_t *phy)
{
    esp_err_t ret = ESP_OK;
    phy_802_3_t *phy_802_3 = esp_eth_phy_into_phy_802_3(phy);

    /* Basic PHY init */
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_basic_phy_init(phy_802_3), err, TAG, "failed to init PHY");

    /* Check PHY ID */
    uint32_t oui;
    uint8_t model;
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_read_oui(phy_802_3, &oui), err, TAG, "read OUI failed");
    ESP_GOTO_ON_ERROR(esp_eth_phy_802_3_read_manufac_info(phy_802_3, &model, NULL), err, TAG, "read manufacturer's info failed");
    ESP_GOTO_ON_FALSE(oui == 0x1F0, ESP_FAIL, err, TAG, "wrong chip OUI");

    bool supported_model = false;
    for (unsigned int i = 0; i < sizeof(supported_models); i++) {
        if (model == supported_models[i]) {
            supported_model = true;
            break;
        }
    }
    ESP_GOTO_ON_FALSE(supported_model, ESP_FAIL, err, TAG, "unsupported chip model");
    return ESP_OK;
err:
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_lan87xx(const eth_phy_config_t *config)
{
    esp_eth_phy_t *ret = NULL;
    phy_lan87xx_t *lan87xx = calloc(1, sizeof(phy_lan87xx_t));
    ESP_GOTO_ON_FALSE(lan87xx, NULL, err, TAG, "calloc lan87xx failed");
    eth_phy_config_t lan87xx_config = *config;
    // default chip specific configuration
    if (config->hw_reset_assert_time_us == 0) {
        lan87xx_config.hw_reset_assert_time_us = LAN87XX_PHY_RESET_ASSERTION_TIME_US;
    }
    if (config->post_hw_reset_delay_ms == 0) {
        lan87xx_config.post_hw_reset_delay_ms = ESP_ETH_NO_POST_HW_RESET_DELAY;
    }
    ESP_GOTO_ON_FALSE(esp_eth_phy_802_3_obj_config_init(&lan87xx->phy_802_3, &lan87xx_config) == ESP_OK,
                        NULL, err, TAG, "configuration initialization of PHY 802.3 failed");

    // redefine functions which need to be customized for sake of LAN87xx
    lan87xx->phy_802_3.parent.init = lan87xx_init;
    lan87xx->phy_802_3.parent.get_link = lan87xx_get_link;
    lan87xx->phy_802_3.parent.autonego_ctrl = lan87xx_autonego_ctrl;
    lan87xx->phy_802_3.parent.loopback = lan87xx_loopback;
    lan87xx->phy_802_3.parent.set_speed = lan87xx_set_speed;

    return &lan87xx->phy_802_3.parent;
err:
    if (lan87xx != NULL) {
        free(lan87xx);
    }
    return ret;
}
