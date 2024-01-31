/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc_caps.h"

#if SOC_IEEE802154_SUPPORTED
#include "soc/ieee802154_reg.h"
#include "soc/ieee802154_struct.h"
#include "esp_attr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_IEEE802154_SUPPORTED

/**
 * @brief IEEE802154 opcode.
 */
typedef enum {
    IEEE802154_CMD_TX_START      = 0x41,
    IEEE802154_CMD_RX_START      = 0x42,
    IEEE802154_CMD_CCA_TX_START  = 0x43,
    IEEE802154_CMD_ED_START      = 0x44,
    IEEE802154_CMD_STOP          = 0x45,
    IEEE802154_CMD_TEST_TX_START = 0x46,
    IEEE802154_CMD_TEST_RX_START = 0x47,
    IEEE802154_CMD_TEST_STOP     = 0x48,
    IEEE802154_CMD_TIMER0_START  = 0x4C,
    IEEE802154_CMD_TIMER0_STOP   = 0x4D,
    IEEE802154_CMD_TIMER1_START  = 0x4E,
    IEEE802154_CMD_TIMER1_STOP   = 0x4F,
} ieee802154_ll_cmd_t;

/**
 * @brief IEEE802154 interrupt.
 */
typedef enum {
    IEEE802154_EVENT_TX_DONE           = BIT(0),
    IEEE802154_EVENT_RX_DONE           = BIT(1),
    IEEE802154_EVENT_ACK_TX_DONE       = BIT(2),
    IEEE802154_EVENT_ACK_RX_DONE       = BIT(3),
    IEEE802154_EVENT_RX_ABORT          = BIT(4),
    IEEE802154_EVENT_TX_ABORT          = BIT(5),
    IEEE802154_EVENT_ED_DONE           = BIT(6),
    IEEE802154_EVENT_TIMER0_OVERFLOW   = BIT(8),
    IEEE802154_EVENT_TIMER1_OVERFLOW   = BIT(9),
    IEEE802154_EVENT_CLOCK_COUNT_MATCH = BIT(10),
    IEEE802154_EVENT_TX_SFD_DONE       = BIT(11),
    IEEE802154_EVENT_RX_SFD_DONE       = BIT(12),
    IEEE802154_EVENT_MASK = IEEE802154_EVENT_EN,
} ieee802154_ll_event_t;

typedef uint16_t ieee802154_ll_events;

/**
 * @brief IEEE802154 security transmission failed reason.
 */
typedef enum {
    IEEE802154_TX_SEC_FRAME_CTRL_NOT_SET = 0x1,
    IEEE802154_TX_SEC_RESERVED_SEC_LEVEL = 0x2,
    IEEE802154_TX_SEC_HEADER_PARSE       = 0x3,
    IEEE802154_TX_SEC_PAYLOAD_ERROR      = 0x4,
    IEEE802154_TX_SEC_FRAME_COUNTER_SUP  = 0x5,
} ieee802154_ll_tx_security_failed_reason_t;

/**
 * @brief IEEE802154 receiving failed reason.
 */
typedef enum {
    IEEE802154_RX_ABORT_BY_RX_STOP           = 1,
    IEEE802154_RX_ABORT_BY_SFD_TIMEOUT       = 2,
    IEEE802154_RX_ABORT_BY_CRC_ERROR         = 3,
    IEEE802154_RX_ABORT_BY_INVALID_LEN       = 4,
    IEEE802154_RX_ABORT_BY_FILTER_FAIL       = 5,
    IEEE802154_RX_ABORT_BY_NO_RSS            = 6,
    IEEE802154_RX_ABORT_BY_COEX_BREAK        = 7,
    IEEE802154_RX_ABORT_BY_UNEXPECTED_ACK    = 8,
    IEEE802154_RX_ABORT_BY_RX_RESTART        = 9,
    IEEE802154_RX_ABORT_BY_TX_ACK_TIMEOUT    = 16,
    IEEE802154_RX_ABORT_BY_TX_ACK_STOP       = 17,
    IEEE802154_RX_ABORT_BY_TX_ACK_COEX_BREAK = 18,
    IEEE802154_RX_ABORT_BY_ENHACK_SECURITY_ERROR   = 19,
    IEEE802154_RX_ABORT_BY_ED_ABORT          = 24,
    IEEE802154_RX_ABORT_BY_ED_STOP           = 25,
    IEEE802154_RX_ABORT_BY_ED_COEX_REJECT    = 26,
} ieee802154_ll_rx_abort_reason_t;

typedef uint32_t ieee802154_ll_rx_abort_events;
#define IEEE802154_RX_ABORT_ALL 0x7fffffff

/**
 * @brief IEEE802154 transmission failed reason.
 */
typedef enum {
    IEEE802154_TX_ABORT_BY_RX_ACK_STOP         = 1,
    IEEE802154_TX_ABORT_BY_RX_ACK_SFD_TIMEOUT  = 2,
    IEEE802154_TX_ABORT_BY_RX_ACK_CRC_ERROR    = 3,
    IEEE802154_TX_ABORT_BY_RX_ACK_INVALID_LEN  = 4,
    IEEE802154_TX_ABORT_BY_RX_ACK_FILTER_FAIL  = 5,
    IEEE802154_TX_ABORT_BY_RX_ACK_NO_RSS       = 6,
    IEEE802154_TX_ABORT_BY_RX_ACK_COEX_BREAK   = 7,
    IEEE802154_TX_ABORT_BY_RX_ACK_TYPE_NOT_ACK = 8,
    IEEE802154_TX_ABORT_BY_RX_ACK_RESTART      = 9,
    IEEE802154_TX_ABORT_BY_RX_ACK_TIMEOUT      = 16,
    IEEE802154_TX_ABORT_BY_TX_STOP             = 17,
    IEEE802154_TX_ABORT_BY_TX_COEX_BREAK       = 18,
    IEEE802154_TX_ABORT_BY_TX_SECURITY_ERROR   = 19,
    IEEE802154_TX_ABORT_BY_CCA_FAILED          = 24,
    IEEE802154_TX_ABORT_BY_CCA_BUSY            = 25,
} ieee802154_ll_tx_abort_reason_t;

typedef uint32_t ieee802154_ll_tx_abort_events;
#define IEEE802154_TX_ABORT_ALL 0x7fffffff

/**
 * @brief IEEE802154 CCA mode.
 */
typedef enum {
    IEEE802154_CCA_MODE_CARRIER        = 0x00,
    IEEE802154_CCA_MODE_ED             = 0x01,
    IEEE802154_CCA_MODE_CARRIER_OR_ED  = 0x02,
    IEEE802154_CCA_MODE_CARRIER_AND_ED = 0x03,
} ieee802154_ll_cca_mode_t;

/**
 * @brief IEEE802154 pending mode.
 */
typedef enum {
    IEEE802154_AUTO_PENDING_DISABLE  = 0x0,
    IEEE802154_AUTO_PENDING_ENABLE   = 0x1,
    IEEE802154_AUTO_PENDING_ENHANCED = 0x2,
    IEEE802154_AUTO_PENDING_ZIGBEE   = 0x3,
} ieee802154_ll_pending_mode_t;

/**
 * @brief IEEE802154 ED sample duration.
 */
typedef enum {
    IEEE802154_ED_SAMPLE_1_PER_US = 0x00,
    IEEE802154_ED_SAMPLE_2_PER_US = 0x01,
    IEEE802154_ED_SAMPLE_4_PER_US = 0x02,
    IEEE802154_ED_SAMPLE_8_PER_US = 0x03,
} ieee802154_ll_ed_sample_dur_t;

/**
 * @brief IEEE802154 multi-pan index.
 */
typedef enum {
    IEEE802154_MULTIPAN_0     = 0,
    IEEE802154_MULTIPAN_1     = 1,
    IEEE802154_MULTIPAN_2     = 2,
    IEEE802154_MULTIPAN_3     = 3,
    IEEE802154_MULTIPAN_MAX   = 4,
} ieee802154_ll_multipan_index_t;

/**
 * @brief IEEE802154 ED sample mode.
 */
typedef enum {
    IEEE802154_ED_SAMPLE_MAX = 0x00,
    IEEE802154_ED_SAMPLE_AVG = 0x01,
} ieee802154_ll_ed_sample_mode_t;

#define IEEE802154_RX_STATUS_RECEIVE_SFD 0x1

FORCE_INLINE_ATTR void ieee802154_ll_set_cmd(ieee802154_ll_cmd_t cmd)
{
    IEEE802154.cmd.cmd = cmd;
}

static inline void ieee802154_ll_enable_events(ieee802154_ll_events events)
{
    IEEE802154.event_en.events |= events;
}

static inline void ieee802154_ll_disable_events(ieee802154_ll_events events)
{
    IEEE802154.event_en.events &= ~events;
}

FORCE_INLINE_ATTR void ieee802154_ll_clear_events(ieee802154_ll_events events)
{
    IEEE802154.event_status.events &= events;
}

FORCE_INLINE_ATTR ieee802154_ll_events ieee802154_ll_get_events(void)
{
    return (ieee802154_ll_events)(IEEE802154.event_status.events);
}

FORCE_INLINE_ATTR bool ieee802154_ll_is_current_rx_frame(void)
{
    return (IEEE802154.rx_status.rx_state > IEEE802154_RX_STATUS_RECEIVE_SFD);
}

static inline void ieee802154_ll_enable_rx_abort_events(ieee802154_ll_rx_abort_events events)
{
    IEEE802154.rx_abort_event_en.rx_abort_en |= events;
}

static inline void ieee802154_ll_disable_rx_abort_events(ieee802154_ll_rx_abort_events events)
{
    IEEE802154.rx_abort_event_en.rx_abort_en &= ~events;
}

static inline void ieee802154_ll_enable_tx_abort_events(ieee802154_ll_rx_abort_events events)
{
    IEEE802154.tx_abort_event_en.tx_abort_en |= events;
}

static inline void ieee802154_ll_disable_tx_abort_events(ieee802154_ll_rx_abort_events events)
{
    IEEE802154.tx_abort_event_en.tx_abort_en &= ~events;
}

FORCE_INLINE_ATTR void ieee802154_ll_set_rx_addr(uint8_t *addr)
{
    IEEE802154.dma_rx_addr = (uint32_t)addr;
}

FORCE_INLINE_ATTR void ieee802154_ll_enhack_generate_done_notify(void)
{
    IEEE802154.enhack_generate_done_notify = true;
}

FORCE_INLINE_ATTR void ieee802154_ll_set_tx_addr(uint8_t *addr)
{
    IEEE802154.dma_tx_addr = (uint32_t)addr;
}

FORCE_INLINE_ATTR uint32_t ieee802154_ll_get_rx_status(void)
{
    return IEEE802154.rx_status.val;
}

FORCE_INLINE_ATTR ieee802154_ll_rx_abort_reason_t ieee802154_ll_get_rx_abort_reason(void)
{
    return (ieee802154_ll_rx_abort_reason_t)(IEEE802154.rx_status.rx_abort_reason);
}

FORCE_INLINE_ATTR uint32_t ieee802154_ll_get_tx_status(void)
{
    return IEEE802154.tx_status.val;
}

FORCE_INLINE_ATTR ieee802154_ll_tx_abort_reason_t ieee802154_ll_get_tx_abort_reason(void)
{
    return (ieee802154_ll_tx_abort_reason_t)(IEEE802154.tx_status.tx_abort_reason);
}

FORCE_INLINE_ATTR ieee802154_ll_tx_security_failed_reason_t ieee802154_ll_get_tx_security_failed_reason(void)
{
    return (ieee802154_ll_tx_security_failed_reason_t)(IEEE802154.tx_status.tx_security_error);
}

static inline uint8_t ieee802154_ll_get_freq(void)
{
    return IEEE802154.channel.freq;
}

static inline void ieee802154_ll_set_freq(uint8_t freq)
{
    IEEE802154.channel.freq = freq;
}

static inline void ieee802154_ll_set_power(uint8_t power)
{
    IEEE802154.txpower.power = power;
}

static inline void ieee802154_ll_set_multipan_panid(ieee802154_ll_multipan_index_t index, uint16_t panid)
{
    IEEE802154.conf.multipan_mask |= BIT(index);
    IEEE802154.multipan[index].panid.id = panid;
}

static inline uint16_t ieee802154_ll_get_multipan_panid(ieee802154_ll_multipan_index_t index)
{
    return IEEE802154.multipan[index].panid.id;
}

static inline void ieee802154_ll_set_multipan_short_addr(ieee802154_ll_multipan_index_t index, uint16_t short_addr)
{
    IEEE802154.conf.multipan_mask |= BIT(index);
    IEEE802154.multipan[index].short_addr.addr = short_addr;
}

static inline uint16_t ieee802154_ll_get_multipan_short_addr(ieee802154_ll_multipan_index_t index)
{
    return IEEE802154.multipan[index].short_addr.addr;
}

static inline void ieee802154_ll_set_multipan_ext_addr(ieee802154_ll_multipan_index_t index, const uint8_t *ext_addr)
{
    IEEE802154.conf.multipan_mask |= BIT(index);
    IEEE802154.multipan[index].ext_addr0 = (ext_addr[0] << 0) | (ext_addr[1] << 8) | (ext_addr[2] << 16) | (ext_addr[3] << 24);
    IEEE802154.multipan[index].ext_addr1 = (ext_addr[4] << 0) | (ext_addr[5] << 8) | (ext_addr[6] << 16) | (ext_addr[7] << 24);
}

static inline void ieee802154_ll_get_multipan_ext_addr(ieee802154_ll_multipan_index_t index, uint8_t *ext_addr)
{
    for (uint8_t i = 0; i < 4; i ++) {
        ext_addr[i] = (IEEE802154.multipan[index].ext_addr0 >> (8 * i)) & 0xff;
        ext_addr[i + 4] = (IEEE802154.multipan[index].ext_addr1 >> (8 * i)) & 0xff;
    }
}

static inline void ieee802154_ll_set_multipan_enable_mask(uint8_t mask)
{
    IEEE802154.conf.multipan_mask = mask;
}

static inline uint8_t ieee802154_ll_get_multipan_enable_mask(void)
{
    return IEEE802154.conf.multipan_mask;
}

FORCE_INLINE_ATTR bool ieee802154_ll_is_cca_busy(void)
{
    return IEEE802154.ed_cfg.cca_busy;
}

FORCE_INLINE_ATTR int8_t ieee802154_ll_get_ed_rss(void)
{
    return IEEE802154.ed_cfg.ed_rss;
}

static inline void ieee802154_ll_set_cca_mode(ieee802154_ll_cca_mode_t cca_mode)
{
    IEEE802154.ed_cfg.cca_mode = cca_mode;
}

static inline void ieee802154_ll_set_cca_threshold(int8_t cca_threshold)
{
    IEEE802154.ed_cfg.cca_threshold = cca_threshold;
}

static inline void ieee802154_ll_set_ed_sample_rate(ieee802154_ll_ed_sample_dur_t ed_sample_rate)
{
    IEEE802154.ed_cfg.ed_sample_rate = ed_sample_rate;
}

static inline void ieee802154_ll_set_ed_sample_mode(ieee802154_ll_ed_sample_mode_t ed_sample_mode)
{
    IEEE802154.ed_cfg.ed_sample_mode = ed_sample_mode;
}

static inline void ieee802154_ll_set_ed_duration(uint16_t duration)
{
    IEEE802154.ed_duration.duration = duration;
}

FORCE_INLINE_ATTR bool ieee802154_ll_get_tx_auto_ack(void)
{
    return IEEE802154.conf.auto_ack_tx;
}

static inline void ieee802154_ll_set_tx_auto_ack(bool enable)
{
    IEEE802154.conf.auto_ack_tx = enable;
}

FORCE_INLINE_ATTR bool ieee802154_ll_get_rx_auto_ack(void)
{
    return IEEE802154.conf.auto_ack_rx;
}

static inline void ieee802154_ll_set_rx_auto_ack(bool enable)
{
    IEEE802154.conf.auto_ack_rx = enable;
}

FORCE_INLINE_ATTR bool ieee802154_ll_get_tx_enhance_ack(void)
{
    return IEEE802154.conf.auto_enhack;
}

static inline void ieee802154_ll_set_tx_enhance_ack(bool enable)
{
    IEEE802154.conf.auto_enhack = enable;
}

static inline void ieee802154_ll_set_coordinator(bool enable)
{
    IEEE802154.conf.coordinator = enable;
}

static inline void ieee802154_ll_set_promiscuous(bool enable)
{
    IEEE802154.conf.promiscuous = enable;
}

static inline void ieee802154_ll_set_pending_mode(bool enable)
{
    IEEE802154.conf.pending_enhance = enable;
}

FORCE_INLINE_ATTR void ieee802154_ll_set_pending_bit(bool pending)
{
    IEEE802154.pending_cfg.pending = pending;
}

FORCE_INLINE_ATTR void ieee802154_ll_timer0_set_threshold(uint32_t value)
{
    IEEE802154.timer0_threshold = value;
}

static inline uint32_t ieee802154_ll_timer0_get_value(void)
{
    return IEEE802154.timer0_value;
}

static inline void ieee802154_ll_timer1_set_threshold(uint32_t value)
{
    IEEE802154.timer1_threshold = value;
}

static inline uint32_t ieee802154_ll_timer1_get_value(void)
{
    return IEEE802154.timer1_value;
}

FORCE_INLINE_ATTR void ieee802154_ll_set_transmit_security(bool enable)
{
    IEEE802154.security_ctrl.tx_security_en = enable;
}

static inline void ieee802154_ll_set_security_offset(uint8_t offset)
{
    IEEE802154.security_ctrl.security_offset = offset;
}

static inline uint8_t ieee802154_ll_get_security_offset(void)
{
    return IEEE802154.security_ctrl.security_offset;
}

static inline void ieee802154_ll_set_security_addr(uint8_t *security_addr)
{
    IEEE802154.security_addr0 = (security_addr[0] << 0) | (security_addr[1] << 8) | (security_addr[2] << 16) | (security_addr[3] << 24);
    IEEE802154.security_addr1 = (security_addr[4] << 0) | (security_addr[5] << 8) | (security_addr[6] << 16) | (security_addr[7] << 24);
}

static inline void ieee802154_ll_set_security_key(uint8_t *security_key)
{
    IEEE802154.security_key0 = (security_key[0]  << 0) | (security_key[1]  << 8) | (security_key[2]  << 16) | (security_key[3]  << 24);
    IEEE802154.security_key1 = (security_key[4]  << 0) | (security_key[5]  << 8) | (security_key[6]  << 16) | (security_key[7]  << 24);
    IEEE802154.security_key2 = (security_key[8]  << 0) | (security_key[9]  << 8) | (security_key[10] << 16) | (security_key[11] << 24);
    IEEE802154.security_key3 = (security_key[12] << 0) | (security_key[13] << 8) | (security_key[14] << 16) | (security_key[15] << 24);
}

static inline void ieee802154_ll_disable_coex(void)
{
    IEEE802154.pti.pti = 1;
    IEEE802154.pti.hw_ack_pti = 1;
}

static inline void ieee802154_ll_clear_debug_cnt(uint32_t clear_bits)
{
    IEEE802154.debug_cnt_clr.val = clear_bits;
}

static inline uint32_t ieee802154_ll_get_sfd_timeout_cnt(void)
{
    return IEEE802154.debug_sfd_timeout_cnt;
}

static inline uint32_t ieee802154_ll_get_crc_error_cnt(void)
{
    return IEEE802154.debug_crc_error_cnt;
}

static inline uint32_t ieee802154_ll_get_ed_abort_cnt(void)
{
    return IEEE802154.debug_ed_abort_cnt;
}

static inline uint32_t ieee802154_ll_get_cca_fail_cnt(void)
{
    return IEEE802154.debug_cca_fail_cnt;
}

static inline uint32_t ieee802154_ll_get_rx_fliter_fail_cnt(void)
{
    return IEEE802154.debug_rx_filter_fail_cnt;
}

static inline uint32_t ieee802154_ll_get_no_rss_detect_cnt(void)
{
    return IEEE802154.debug_no_rss_detect_cnt;
}

static inline uint32_t ieee802154_ll_get_rx_abort_coex_cnt(void)
{
    return IEEE802154.debug_rx_abort_coex_cnt;
}

static inline uint32_t ieee802154_ll_get_rx_restart_cnt(void)
{
    return IEEE802154.debug_rx_restart_cnt;
}

static inline uint32_t ieee802154_ll_get_tx_ack_abort_coex_cnt(void)
{
    return IEEE802154.debug_tx_ack_abort_coex_cnt;
}

static inline uint32_t ieee802154_ll_get_ed_scan_coex_cnt(void)
{
    return IEEE802154.debug_ed_scan_break_coex_cnt;
}

static inline uint32_t ieee802154_ll_get_rx_ack_abort_coex_cnt(void)
{
    return IEEE802154.debug_rx_ack_abort_coex_cnt;
}

static inline uint32_t ieee802154_ll_get_rx_ack_timeout_cnt(void)
{
    return IEEE802154.debug_rx_ack_timeout_cnt;
}

static inline uint32_t ieee802154_ll_get_tx_break_coex_cnt(void)
{
    return IEEE802154.debug_tx_break_coex_cnt;
}

static inline uint32_t ieee802154_ll_get_tx_security_error_cnt(void)
{
    return IEEE802154.debug_tx_security_error_cnt;
}

static inline uint32_t ieee802154_ll_get_cca_busy_cnt(void)
{
    return IEEE802154.debug_cca_busy_cnt;
}
#endif

#ifdef __cplusplus
}
#endif
