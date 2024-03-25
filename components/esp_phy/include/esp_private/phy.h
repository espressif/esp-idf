/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_phy_init.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_CAL_DATA_CHECK_FAIL 1

typedef enum {
    PHY_I2C_MST_CMD_TYPE_OFF = 0,
    PHY_I2C_MST_CMD_TYPE_ON,
    PHY_I2C_MST_CMD_TYPE_MAX
} phy_i2c_master_command_type_t;

typedef struct {
    struct {
        uint8_t start, end; /* the start and end index of phy i2c master command memory */
        uint8_t host_id;    /* phy i2c master host id */
    } config[PHY_I2C_MST_CMD_TYPE_MAX];
} phy_i2c_master_command_attribute_t;

/**
 * @file phy.h
 * @brief Declarations for functions provided by libphy.a
 */

/**
 * @brief Return ROM function pointer table from PHY library.
 */
void phy_get_romfunc_addr(void);

/**
 * @brief Initialize PHY module and do RF calibration
 * @param[in] init_data Initialization parameters to be used by the PHY
 * @param[inout] cal_data As input, calibration data previously obtained. As output, will contain new calibration data.
 * @param[in] cal_mode  RF calibration mode
 * @return ESP_CAL_DATA_CHECK_FAIL if calibration data checksum fails, other values are reserved for future use
 */
int register_chipv7_phy(const esp_phy_init_data_t* init_data, esp_phy_calibration_data_t *cal_data, esp_phy_calibration_mode_t cal_mode);

/**
 * @brief Get the format version of calibration data used by PHY library.
 * @return Format version number, OR'ed with BIT(16) if PHY is in WIFI only mode.
 */
uint32_t phy_get_rf_cal_version(void);

/**
 * @brief Set RF/BB for only WIFI mode or coexist(WIFI & BT) mode
 * @param[in] true is for only WIFI mode, false is for coexist mode. default is 0.
 * @return NULL
 */
void phy_set_wifi_mode_only(bool wifi_only);

/**
 * @brief Set BT the highest priority in coexist mode.
 * @return NULL
 */
void coex_bt_high_prio(void);

/**
 * @brief Open PHY and RF.
 */
void phy_wakeup_init(void);

/**
 * @brief Shutdown PHY and RF.
 */
void phy_close_rf(void);

#if !CONFIG_IDF_TARGET_ESP32
/**
 * @brief Disable PHY temperature sensor.
 */
void phy_xpd_tsens(void);
#endif

#if CONFIG_IDF_TARGET_ESP32C3
/**
 * @brief Update internal state of PHY when wifi deinit powers off the wifi power domain.
 */
void phy_init_flag(void);
#endif

#if CONFIG_IDF_TARGET_ESP32C6
/**
 * @brief Get the configuration info of PHY i2c master command memory.
 *
 * @param   attr the configuration info of PHY i2c master command memory
 */
void phy_i2c_master_mem_cfg(phy_i2c_master_command_attribute_t *attr);
#endif

/**
 * @brief Store and load PHY digital registers.
 *
 * @param     backup_en  if backup_en is true, store PHY digital registers to memory. Otherwise load PHY digital registers from memory
 * @param     mem_addr   Memory address to store and load PHY digital registers
 *
 * @return    memory size
 */
uint8_t phy_dig_reg_backup(bool backup_en, uint32_t *mem_addr);

#if CONFIG_MAC_BB_PD
/**
 * @brief Store and load baseband registers.
 */
void phy_freq_mem_backup(bool backup_en, uint32_t *mem);
#endif

#if CONFIG_ESP_PHY_ENABLE_USB
/**
 * @brief Enable or disable USB when phy init.
 */
void phy_bbpll_en_usb(bool en);
#endif

#if CONFIG_IDF_TARGET_ESP32S2
/**
 * @brief Phy version select for ESP32S2
 */
void phy_eco_version_sel(uint8_t chip_ver);
#endif

#if CONFIG_ESP_PHY_IMPROVE_RX_11B
/**
 * @brief Improve Wi-Fi receive 11b pkts when modules with high interference.
 *
 * @attention 1.This is a workaround to improve Wi-Fi receive 11b pkts for some modules using AC-DC power supply with high interference.
 * @attention 2.Enable this will sacrifice Wi-Fi OFDM receive performance.But to guarantee 11b receive performance serves as a bottom line in this case.
 *
 * @param     enable  Enable or disable.
 */
void phy_improve_rx_special(bool enable);
#endif

/**
 * @brief Enable phy track pll
 *
 */
void phy_track_pll_init(void);

/**
 * @brief Disable phy track pll
 *
 */
void phy_track_pll_deinit(void);

/**
 * @brief Set the flag recorded which modem has already enabled phy
 *
 */
void phy_set_modem_flag(esp_phy_modem_t modem);

/**
 * @brief Clear the flag to record which modem calls phy disenable
 */
void phy_clr_modem_flag(esp_phy_modem_t modem);

/**
 * @brief Get the flag recorded which modem has already enabled phy
 *
 */
esp_phy_modem_t phy_get_modem_flag(void);

/**
 * @brief Get the PHY lock, only used in esp_phy, the user should not use this function.
 *
 */
_lock_t phy_get_lock(void);

/**
 * @brief Call this funnction to track pll immediately.
 *
 */
void phy_track_pll(void);

/**
 * @brief PHY antenna default configuration
 *
 */
void ant_dft_cfg(bool default_ant);

/**
 * @brief PHY tx antenna config
 *
 */
void ant_tx_cfg(uint8_t ant0);

/**
 * @brief PHY rx antenna config
 *
 */
void ant_rx_cfg(bool auto_en, uint8_t ant0, uint8_t ant1);

/**
 * @brief PHY antenna need update
 *
 */
bool phy_ant_need_update(void);

/**
 * @brief PHY antenna need update
 *
 */
void phy_ant_clr_update_flag(void);

/**
 * @brief PHY antenna configuration update
 *
 */
void phy_ant_update(void);


#ifdef __cplusplus
}
#endif
