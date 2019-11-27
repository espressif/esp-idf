#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "esp_eth.h"
#include "eth_phy_regs_struct.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"


static const char *TAG = "ENC28J60";
#define PHY_CHECK(a, str, goto_tag, ...)                                          \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

/**
 * @brief PHYIDR1(PHY Identifier Register 1)
 *
 */
typedef union {
    struct {
        uint32_t PHY_ID_MSB : 16; /*!< Organizationally Unique Identifier(OUI) most significant bits */
    };
    uint32_t val;
} phy_id1_reg_t;
#define ETH_PHYID1_ADDR (0x02)

/**
 * @brief PHYIDR2(PHY Identifier Register 2)
 *
 */
typedef union {
    struct {
        uint32_t PHY_ID_LSB : 6;    /*!< Model revision number */
        uint32_t PHY_PN : 6;        /*!< Vendor model number */
        uint32_t PHY_rev : 4;       /*!< Organizationally Unique Identifier(OUI) least significant bits */
    };
    uint32_t val;
} phy_id2_reg_t;
#define ETH_PHYID2_ADDR (0x03)


/**
 * @brief PHSTAT2(PHY Configuration and Status Register)
 *
 */
typedef union {
    struct {
        uint32_t reserved_4_0 : 5;
        uint32_t PLRITY : 1;
        uint32_t reserved_8_6 : 3;
        uint32_t DPXSTAT : 1;
        uint32_t LSTAT : 1;
        uint32_t COLSTAT : 1;
        uint32_t RXSTAT : 1;
        uint32_t TXSTAT : 1;
        uint32_t reserved_15_14 : 2;
    };
    uint32_t val;
} phstat2_reg_t;
#define ETH_PHY_PHSTAT2_REG_ADDR (0x11)

typedef struct {
    esp_eth_phy_t parent;
    esp_eth_mediator_t *eth;
    const char *name;
    uint32_t addr;
    uint32_t reset_timeout_ms;
    uint32_t autonego_timeout_ms;
    eth_link_t link_status;
} phy_ENC28J60_t;

static esp_err_t ENC28J60_set_mediator(esp_eth_phy_t *phy, esp_eth_mediator_t *eth)
{
    PHY_CHECK(eth, "can't set mediator for ENC28J60 to null", err);
    phy_ENC28J60_t *ENC28J60 = __containerof(phy, phy_ENC28J60_t, parent);
    ENC28J60->eth = eth;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}




static esp_err_t ENC28J60_get_link(esp_eth_phy_t *phy)
{
    phy_ENC28J60_t *ENC28J60 = __containerof(phy, phy_ENC28J60_t, parent);
    esp_eth_mediator_t *eth = ENC28J60->eth;
    bmsr_reg_t PHstat1;
    phstat2_reg_t PHstat2;

    PHY_CHECK(eth->phy_reg_read(eth, ENC28J60->addr, ETH_PHY_BMSR_REG_ADDR, &(PHstat1.val)) == ESP_OK, "read PHSTAT1 failed", err);

    PHY_CHECK(eth->phy_reg_read(eth, ENC28J60->addr, ETH_PHY_PHSTAT2_REG_ADDR, &(PHstat2.val)) == ESP_OK, "read PHSTAT1 failed", err);
    eth_link_t link = PHstat2.LSTAT ? ETH_LINK_UP : ETH_LINK_DOWN;

    if (ENC28J60->link_status != link) {
        if (link == ETH_LINK_UP) {
            phy->negotiate(phy);
        } else {
            PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link) == ESP_OK, "send link event failed", err);
            ENC28J60->link_status = link;
        }
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t ENC28J60_reset(esp_eth_phy_t *phy)
{
    /*ENC28J60 should be reset using the MAC reset function*/
    return ESP_OK;

}

static esp_err_t ENC28J60_negotiate(esp_eth_phy_t *phy)
{
    phy_ENC28J60_t *ENC28J60 = __containerof(phy, phy_ENC28J60_t, parent);
    esp_eth_mediator_t *eth = ENC28J60->eth;
    /* Start auto negotiation */
    bmcr_reg_t PHCON1 = {
        .speed_select = 0,     /* 10Mbps */
        .duplex_mode = 0,      /* Half Duplex */
        .en_auto_nego = 0,     /* no Auto Negotiation */
        .restart_auto_nego = 0 /* dont Restart Auto Negotiation */
    };
    PHY_CHECK(eth->phy_reg_write(eth, ENC28J60->addr, ETH_PHY_BMCR_REG_ADDR, PHCON1.val) == ESP_OK, "write PHCON1 failed", err);

    bmsr_reg_t PHSTAT1;
    phstat2_reg_t PHSTAT2;

    /* Update information about link, speed, duplex */
    PHY_CHECK(eth->phy_reg_read(eth, ENC28J60->addr, ETH_PHY_BMSR_REG_ADDR, &(PHSTAT1.val)) == ESP_OK, "read PHSTAT1 failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, ENC28J60->addr, 0x11, &(PHSTAT2.val)) == ESP_OK, "read PHSTAT2 failed", err);
    eth_link_t link = PHSTAT2.LSTAT ? ETH_LINK_UP : ETH_LINK_DOWN;

    eth_speed_t speed = ETH_SPEED_10M;
    eth_duplex_t duplex = ETH_DUPLEX_HALF;

    PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_SPEED, (void *)speed) == ESP_OK, "send speed event failed", err);
    PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_DUPLEX, (void *)duplex) == ESP_OK, "send duplex event failed", err);

    if (ENC28J60->link_status != link) {
        PHY_CHECK(eth->on_state_changed(eth, ETH_STATE_LINK, (void *)link) == ESP_OK, "send link event failed", err);
        ENC28J60->link_status = link;
    }

    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t ENC28J60_pwrctl(esp_eth_phy_t *phy, bool enable)
{
    /*ToDo: Add power control function*/
    return ESP_OK;
}

static esp_err_t ENC28J60_set_addr(esp_eth_phy_t *phy, uint32_t addr)
{
    phy_ENC28J60_t *ENC28J60 = __containerof(phy, phy_ENC28J60_t, parent);
    ENC28J60->addr = addr;
    return ESP_OK;
}

static esp_err_t ENC28J60_get_addr(esp_eth_phy_t *phy, uint32_t *addr)
{
    PHY_CHECK(addr, "get phy address failed", err);
    phy_ENC28J60_t *ENC28J60 = __containerof(phy, phy_ENC28J60_t, parent);
    *addr = ENC28J60->addr;
    return ESP_OK;
err:
    return ESP_ERR_INVALID_ARG;
}

static esp_err_t ENC28J60_del(esp_eth_phy_t *phy)
{
    phy_ENC28J60_t *ENC28J60 = __containerof(phy, phy_ENC28J60_t, parent);
    free(ENC28J60);
    return ESP_OK;
}

static esp_err_t ENC28J60_init(esp_eth_phy_t *phy)
{
    phy_ENC28J60_t *ENC28J60 = __containerof(phy, phy_ENC28J60_t, parent);
    esp_eth_mediator_t *eth = ENC28J60->eth;
    /* Power on Ethernet PHY */
    PHY_CHECK(ENC28J60_pwrctl(phy, true) == ESP_OK, "power on Ethernet PHY failed", err);
    /* Reset Ethernet PHY */
    PHY_CHECK(ENC28J60_reset(phy) == ESP_OK, "reset Ethernet PHY failed", err);
    /* Check PHY ID */
    phy_id1_reg_t id1;
    phy_id2_reg_t id2;
    uint32_t id3;

    PHY_CHECK(eth->phy_reg_read(eth, ENC28J60->addr, ETH_PHYID1_ADDR, &(id1.val)) == ESP_OK, "read ID1 failed", err);
    PHY_CHECK(eth->phy_reg_read(eth, ENC28J60->addr, ETH_PHYID2_ADDR, &(id2.val)) == ESP_OK, "read ID2 failed", err);

    id3 = ((id1.PHY_ID_MSB << 16) | id2.PHY_ID_LSB);
    PHY_CHECK(id3, "wrong PHY chip ID", err);

    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t ENC28J60_deinit(esp_eth_phy_t *phy)
{
    /* Power off Ethernet PHY */
    PHY_CHECK(ENC28J60_pwrctl(phy, false) == ESP_OK, "power off Ethernet PHY failed", err);
    return ESP_OK;
err:
    return ESP_FAIL;
}

esp_eth_phy_t *esp_eth_phy_new_ENC28J60(const eth_phy_config_t *config)
{
    PHY_CHECK(config, "can't set phy config to null", err);
    PHY_CHECK(config->phy_addr == 1, "ENC28J60's phy address can only set to 1", err);
    phy_ENC28J60_t *ENC28J60 = calloc(1, sizeof(phy_ENC28J60_t));
    PHY_CHECK(ENC28J60, "calloc ENC28J60 object failed", err);
    ENC28J60->name = "ENC28J60";
    ENC28J60->addr = config->phy_addr;
    ENC28J60->reset_timeout_ms = config->reset_timeout_ms;
    ENC28J60->link_status = ETH_LINK_DOWN;
    ENC28J60->autonego_timeout_ms = config->autonego_timeout_ms;
    ENC28J60->parent.reset = ENC28J60_reset;
    ENC28J60->parent.init = ENC28J60_init;
    ENC28J60->parent.deinit = ENC28J60_deinit;
    ENC28J60->parent.set_mediator = ENC28J60_set_mediator;
    ENC28J60->parent.negotiate = ENC28J60_negotiate;
    ENC28J60->parent.get_link = ENC28J60_get_link;
    ENC28J60->parent.pwrctl = ENC28J60_pwrctl;
    ENC28J60->parent.get_addr = ENC28J60_get_addr;
    ENC28J60->parent.set_addr = ENC28J60_set_addr;
    ENC28J60->parent.del = ENC28J60_del;

    return &(ENC28J60->parent);
err:
    return NULL;
}
