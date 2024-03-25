/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_ieee802154_dev.h"
#include "hal/ieee802154_ll.h"
#include "esp_timer.h"
#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_FREERTOS_USE_TICKLESS_IDLE
#define IEEE802154_RF_ENABLE() ieee802154_rf_enable()
#define IEEE802154_RF_DISABLE() ieee802154_rf_disable()
#else
#define IEEE802154_RF_ENABLE()
#define IEEE802154_RF_DISABLE()
#endif // SOC_PM_MODEM_RETENTION_BY_REGDMA && CONFIG_FREERTOS_USE_TICKLESS_IDLE
#define IEEE802154_PROBE(a) do { \
            IEEE802154_RECORD_EVENT(a); \
            ieee802154_record_abort(a); \
            IEEE802154_TXRX_STATISTIC(a); \
            } while(0)

#if CONFIG_IEEE802154_RECORD_EVENT
#define IEEE802154_ASSERT_RECORD_EVENT_SIZE CONFIG_IEEE802154_RECORD_EVENT_SIZE
#define IEEE802154_RECORD_EVENT(a) do { \
            g_ieee802154_probe.event[g_ieee802154_probe.event_index].event = a; \
            g_ieee802154_probe.event[g_ieee802154_probe.event_index].state = ieee802154_get_state(); \
            if (a == IEEE802154_EVENT_RX_ABORT) { \
                g_ieee802154_probe.event[g_ieee802154_probe.event_index].abort_reason.rx \
                    = ieee802154_ll_get_rx_abort_reason(); \
            } else if (a == IEEE802154_EVENT_TX_ABORT) { \
                g_ieee802154_probe.event[g_ieee802154_probe.event_index].abort_reason.tx \
                    = ieee802154_ll_get_tx_abort_reason(); \
            } \
            g_ieee802154_probe.event[g_ieee802154_probe.event_index++].timestamp = esp_timer_get_time(); \
            g_ieee802154_probe.event_index = (g_ieee802154_probe.event_index == IEEE802154_ASSERT_RECORD_EVENT_SIZE) ? \
                0 : g_ieee802154_probe.event_index; \
            } while(0)

/**
 * @brief The table of recording IEEE802154 event command.
 */
typedef struct {
    ieee802154_ll_events event; /*!< record current radio event */
    ieee802154_state_t state;   /*!< record current radio state */
    union {
        ieee802154_ll_rx_abort_reason_t rx;
        ieee802154_ll_tx_abort_reason_t tx;
    } abort_reason;             /*!< record current radio abort reason */
    uint64_t timestamp;         /*!< record timestamp*/
} ieee802154_event_info_t;
#else
#define IEEE802154_RECORD_EVENT(a)
#endif // CONFIG_IEEE802154_RECORD_EVENT

#if CONFIG_IEEE802154_RECORD_STATE
#define IEEE802154_ASSERT_RECORD_STATE_SIZE CONFIG_IEEE802154_RECORD_STATE_SIZE
#define ieee802154_set_state(a) do { s_ieee802154_state = a; \
            sprintf(g_ieee802154_probe.state[g_ieee802154_probe.state_index].line_str, "%d", __LINE__); \
            g_ieee802154_probe.state[g_ieee802154_probe.state_index].timestamp = esp_timer_get_time(); \
            g_ieee802154_probe.state[g_ieee802154_probe.state_index++].state = a; \
            g_ieee802154_probe.state_index = \
            (g_ieee802154_probe.state_index == IEEE802154_ASSERT_RECORD_STATE_SIZE) ? 0 : g_ieee802154_probe.state_index; \
            } while(0)

/**
 * @brief The table of recording IEEE802154 state command.
 */
typedef struct {
    char line_str[5];           /*!< record which line in esp_ieee802154_dev.c changes the state */
    ieee802154_state_t state;   /*!< record current radio state */
    uint64_t timestamp;         /*!< record timestamp */
} ieee802154_state_info_t;
#else
#define ieee802154_set_state(state) (s_ieee802154_state = state)
#endif // CONFIG_IEEE802154_RECORD_STATE

#if CONFIG_IEEE802154_RECORD_CMD
#define IEEE802154_ASSERT_RECORD_CMD_SIZE CONFIG_IEEE802154_RECORD_CMD_SIZE
#define ieee802154_set_cmd(a) do { ieee802154_ll_set_cmd(a); \
            sprintf(g_ieee802154_probe.cmd[g_ieee802154_probe.cmd_index].line_str, "%d", __LINE__); \
            g_ieee802154_probe.cmd[g_ieee802154_probe.cmd_index].timestamp = esp_timer_get_time(); \
            g_ieee802154_probe.cmd[g_ieee802154_probe.cmd_index++].cmd = a; \
            g_ieee802154_probe.cmd_index = \
            (g_ieee802154_probe.cmd_index == IEEE802154_ASSERT_RECORD_CMD_SIZE) ? 0 : g_ieee802154_probe.cmd_index; \
            } while(0)

/**
 * @brief The table of recording IEEE802154 radio command.
 */
typedef struct {
    char line_str[5];           /*!< record which line in esp_ieee802154_dev.c set the command */
    ieee802154_ll_cmd_t cmd;    /*!< record current command */
    uint64_t timestamp;         /*!< record timestamp */
} ieee802154_cmd_info_t;
#else
#define ieee802154_set_cmd(cmd) ieee802154_ll_set_cmd(cmd)
#endif //CONFIG_IEEE802154_RECORD_CMD

#if CONFIG_IEEE802154_RECORD_ABORT
#define IEEE802154_ASSERT_RECORD_ABORT_SIZE CONFIG_IEEE802154_RECORD_ABORT_SIZE
#define ieee802154_record_abort(a) do { \
            if (a == IEEE802154_EVENT_RX_ABORT) { \
                g_ieee802154_probe.abort[g_ieee802154_probe.abort_index].abort_reason.rx \
                    = ieee802154_ll_get_rx_abort_reason(); \
                g_ieee802154_probe.abort[g_ieee802154_probe.abort_index].is_tx_abort = 0; \
                g_ieee802154_probe.abort[g_ieee802154_probe.abort_index++].timestamp = esp_timer_get_time(); \
                g_ieee802154_probe.abort_index = (g_ieee802154_probe.abort_index == IEEE802154_ASSERT_RECORD_ABORT_SIZE) ? \
                    0 : g_ieee802154_probe.abort_index; \
            } else if (a == IEEE802154_EVENT_TX_ABORT) { \
                g_ieee802154_probe.abort[g_ieee802154_probe.abort_index].abort_reason.tx \
                    = ieee802154_ll_get_tx_abort_reason();\
                g_ieee802154_probe.abort[g_ieee802154_probe.abort_index].is_tx_abort = 1; \
                g_ieee802154_probe.abort[g_ieee802154_probe.abort_index++].timestamp = esp_timer_get_time(); \
                g_ieee802154_probe.abort_index = (g_ieee802154_probe.abort_index == IEEE802154_ASSERT_RECORD_ABORT_SIZE) ? \
                    0 : g_ieee802154_probe.abort_index; \
            } \
            } while(0)

/**
 * @brief The table of recording IEEE802154 radio abort.
 */
typedef struct {
    bool is_tx_abort;           /*!< record current abort type */
    union {
        ieee802154_ll_rx_abort_reason_t rx;
        ieee802154_ll_tx_abort_reason_t tx;
    } abort_reason;             /*!< record current radio abort reason */
    uint64_t timestamp;         /*!< record timestamp*/
} ieee802154_abort_info_t;
#else
#define ieee802154_record_abort(a)
#endif // CONFIG_IEEE802154_RECORD_ABORT

/**
 * @brief The table of recording IEEE802154 information.
 */
typedef struct {
#if CONFIG_IEEE802154_RECORD_EVENT
    ieee802154_event_info_t event[IEEE802154_ASSERT_RECORD_EVENT_SIZE]; /*!< record radio event */
    uint8_t event_index;                                                /*!< the index of event */
#endif // CONFIG_IEEE802154_RECORD_EVENT
#if CONFIG_IEEE802154_RECORD_STATE
    ieee802154_state_info_t state[IEEE802154_ASSERT_RECORD_STATE_SIZE]; /*!< record radio state */
    uint8_t state_index;                                                /*!< the index of state */
#endif // CONFIG_IEEE802154_RECORD_STATE
#if CONFIG_IEEE802154_RECORD_CMD
    ieee802154_cmd_info_t cmd[IEEE802154_ASSERT_RECORD_CMD_SIZE];       /*!< record radio command */
    uint8_t cmd_index;                                                  /*!< the index of command */
#endif // CONFIG_IEEE802154_RECORD_CMD
#if CONFIG_IEEE802154_RECORD_ABORT
    ieee802154_abort_info_t abort[IEEE802154_ASSERT_RECORD_ABORT_SIZE]; /*!< record radio abort */
    uint8_t abort_index;                                                  /*!< the index of abort */
#endif // CONFIG_IEEE802154_RECORD_ABORT
} ieee802154_probe_info_t;

extern ieee802154_probe_info_t g_ieee802154_probe;

#if CONFIG_IEEE802154_ASSERT
/**
 * @brief  This function print rich information, which is useful for debug.
 *         Only can be used when `IEEE802154_ASSERT` is enabled.
 *
 */
void ieee802154_assert_print(void);
#define IEEE802154_ASSERT(a) do { \
                                    if(unlikely(!(a))) { \
                                        ieee802154_assert_print(); \
                                        assert(a); \
                                    } \
                                } while (0)
#else // CONFIG_IEEE802154_ASSERT
#define IEEE802154_ASSERT(a) assert(a)
#endif // CONFIG_IEEE802154_ASSERT

#if CONFIG_IEEE802154_TXRX_STATISTIC
typedef struct ieee802154_txrx_statistic{
    struct {
        uint64_t nums;
        uint64_t deferred_nums;
        uint64_t done_nums;
        struct {
            uint64_t rx_ack_coex_break_nums;        // IEEE802154_RX_ACK_ABORT_COEX_CNT_REG
            uint64_t rx_ack_timeout_nums;           // IEEE802154_RX_ACK_TIMEOUT_CNT_REG
            uint64_t tx_coex_break_nums;            // IEEE802154_TX_BREAK_COEX_CNT_REG
            uint64_t tx_security_error_nums;        // IEEE802154_TX_SECURITY_ERROR_CNT_REG
            uint64_t cca_failed_nums;               // IEEE802154_CCA_FAIL_CNT_REG
            uint64_t cca_busy_nums;                 // IEEE802154_CCA_BUSY_CNT_REG
        } abort;
    } tx;
    struct {
        uint64_t done_nums;
        struct {
            uint64_t sfd_timeout_nums;              // IEEE802154_SFD_TIMEOUT_CNT_REG
            uint64_t crc_error_nums;                // IEEE802154_CRC_ERROR_CNT_REG
            uint64_t filter_fail_nums;              // IEEE802154_RX_FILTER_FAIL_CNT_REG
            uint64_t no_rss_nums;                   // IEEE802154_NO_RSS_DETECT_CNT_REG
            uint64_t rx_coex_break_nums;            // IEEE802154_RX_ABORT_COEX_CNT_REG
            uint64_t rx_restart_nums;               // IEEE802154_RX_RESTART_CNT_REG
            uint64_t tx_ack_coex_break_nums;        // IEEE802154_TX_ACK_ABORT_COEX_CNT_REG
            uint64_t ed_abort_nums;                 // IEEE802154_ED_ABORT_CNT_REG
        } abort;
    } rx;
} ieee802154_txrx_statistic_t;

#define IEEE802154_TXRX_STATISTIC_CLEAR() do { \
            ieee802154_txrx_statistic_clear();\
            } while(0)

#define IEEE802154_TXRX_STATISTIC(a) do { \
            ieee802154_txrx_statistic(a);\
            } while(0)

#define IEEE802154_TX_DEFERRED_NUMS_UPDATE() do { \
            ieee802154_tx_deferred_nums_update();\
            } while(0)

#define IEEE802154_TX_NUMS_UPDATE() do { \
            ieee802154_tx_nums_update();\
            } while(0)

#define IEEE802154_TX_BREAK_COEX_NUMS_UPDATE() do { \
            ieee802154_tx_break_coex_nums_update();\
            } while(0)

void ieee802154_txrx_statistic_clear(void);
void ieee802154_txrx_statistic_print(void);
void ieee802154_txrx_statistic(ieee802154_ll_events events);
void ieee802154_tx_nums_update(void);
void ieee802154_tx_deferred_nums_update(void);
void ieee802154_tx_break_coex_nums_update(void);
#else
#define IEEE802154_TXRX_STATISTIC(a)
#define IEEE802154_TX_NUMS_UPDATE()
#define IEEE802154_TX_DEFERRED_NUMS_UPDATE()
#define IEEE802154_TXRX_STATISTIC_CLEAR()
#define IEEE802154_TX_BREAK_COEX_NUMS_UPDATE()
#endif // CONFIG_IEEE802154_TXRX_STATISTIC

// TODO: replace etm code using common interface

#define IEEE802154_ETM_CHANNEL0           0
#define IEEE802154_ETM_CHANNEL1           1

/**
 * @brief The scene of IEEE802154 radio coexistence.
 */
typedef enum {
    IEEE802154_SCENE_IDLE,      /*!< IEEE802154 radio coexistence scene IDLE */
    IEEE802154_SCENE_TX,        /*!< IEEE802154 radio coexistence scene TX */
    IEEE802154_SCENE_RX,        /*!< IEEE802154 radio coexistence scene RX */
    IEEE802154_SCENE_TX_AT,     /*!< IEEE802154 radio coexistence scene TX AT */
    IEEE802154_SCENE_RX_AT,     /*!< IEEE802154 radio coexistence scene RX AT */
} ieee802154_txrx_scene_t;

#if !CONFIG_IEEE802154_TEST && CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE

/**
 * @brief  Set the IEEE802154 radio coexistence scene during transmitting or receiving.
 *
 * @param[in]  txrx_scene  The scene of IEEE802154 radio coexistence.
 *
 */
void ieee802154_set_txrx_pti(ieee802154_txrx_scene_t txrx_scene);

#define IEEE802154_SET_TXRX_PTI(txrx_scene) ieee802154_set_txrx_pti(txrx_scene)

#else

#define IEEE802154_SET_TXRX_PTI(txrx_scene)

#endif // !CONFIG_IEEE802154_TEST && CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE

/**
 * @brief  Convert the frequency to the index of channel.
 *
 * @param[in]  freq  The frequency where the radio is processing.
 *
 * @return
 *          The channel index.
 *
 */
uint8_t ieee802154_freq_to_channel(uint8_t freq);

/**
 * @brief  Convert the index of channel to the frequency.
 *
 * @param[in]  channel  The index of channel where the radio is processing.
 *
 * @return
 *          The frequency.
 *
 */
uint8_t ieee802154_channel_to_freq(uint8_t channel);

// TZ-97: implement these two functions using ETM common interface
/**
 * @brief  Configure the ETM module, using [channel] for monitoring the event, when event appears
 *         hardware will operate the [task].
 *
 * @param[in]  channel  The ETM channel.
 * @param[in]  event  The ETM event.
 * @param[in]  task  The ETM task.
 *
 */
void ieee802154_etm_set_event_task(uint32_t channel, uint32_t event, uint32_t task);

/**
 * @brief  Clear the ETM module [channel].
 *
 * @param[in]  channel  The ETM channel.
 *
 */
void ieee802154_etm_channel_clear(uint32_t channel);

#ifdef __cplusplus
}
#endif
