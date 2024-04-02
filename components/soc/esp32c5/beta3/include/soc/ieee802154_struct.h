/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

// TODO: ZB-93, rewrite this file using regdesc tools when IEEE802154.csv is ready.
typedef volatile struct esp_ieee802154_s {
    union {
        struct {
            uint32_t cmd:        8;
            uint32_t reserved8:  24;
        };
        uint32_t val;
    } cmd; // 0x00

    union {
        struct {
            uint32_t auto_ack_tx:        1;
            uint32_t auto_enhack:  1;
            uint32_t reserved2:  1;
            uint32_t auto_ack_rx:      1;
            uint32_t reserved4:  1;
            uint32_t ifs_dis: 1;
            uint32_t coordinator: 1;
            uint32_t promiscuous: 1;
            uint32_t reserved8: 3;
            uint32_t version_filter_dis: 1;
            uint32_t pending_enhance: 1;
            uint32_t reserved13: 1;
            uint32_t filter_enhance_dis: 1;
            uint32_t reserved15: 1;
            uint32_t coex_arb_delay: 8;
            uint32_t bit_order: 1;
            uint32_t no_rssi_trigger_break_en: 1;
            uint32_t coex_force_rx: 1;
            uint32_t rx_done_trig_idle: 1;
            uint32_t multipan_mask: 4;
        };
        uint32_t val;
    } conf; // 0x04

    struct {
        union {
            struct {
                uint32_t addr:        16;
                uint32_t reserved16:  16;
            };
            uint32_t val;
        } short_addr; // 0x08

        union {
            struct {
                uint32_t id:        16;
                uint32_t reserved16:  16;
            };
            uint32_t val;
        } panid; // 0x0c

        uint32_t ext_addr0; // 0x10
        uint32_t ext_addr1; // 0x14
    } multipan[4];

    union {
        struct {
            uint32_t freq:        7;
            uint32_t reserved7:  25;
        };
        uint32_t val;
    } channel; //0x48

    union {
        struct {
            uint32_t power:        5;
            uint32_t reserved5:  27;
        };
        uint32_t val;
    } txpower; //0x4c

    union {
        struct {
            uint32_t duration:        24;
            uint32_t delay:  4;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } ed_duration; //0x50

    union {
        struct {
            uint32_t cca_threshold:        8;
            uint32_t reserved8: 3;
            uint32_t ed_sample_rate: 2;
            uint32_t ed_sample_mode: 1;
            uint32_t cca_mode:  2;
            uint32_t ed_rss: 8;
            uint32_t cca_busy: 1;
            uint32_t reserved25: 7;
        };
        uint32_t val;
    } ed_cfg; //0x54

    union {
        struct {
            uint32_t sifs:        8;
            uint32_t reserved8: 8;
            uint32_t lifs: 10;
            uint32_t reserved26: 6;
        };
        uint32_t val;
    } ifs_cfg; //0x58

    union {
        struct {
            uint32_t timeout:        16;
            uint32_t reserved16: 16;
        };
        uint32_t val;
    } ack_timeout; //0x5c

    union {
        struct {
            uint32_t events: 13;
            uint32_t reserved13: 19;
        };
        uint32_t val;
    } event_en; //0x60

    union {
        struct {
            uint32_t events: 13;
            uint32_t reserved13: 19;
        };
        uint32_t val;
    } event_status; //0x64

    union {
        struct {
            uint32_t rx_abort_en: 31;
            uint32_t reserved31: 1;
        };
        uint32_t val;
    } rx_abort_event_en; //0x68

    union {
        struct {
            uint32_t pending: 1;
            uint32_t reserved1: 15;
            uint32_t pending_timeout: 16;
        };
        uint32_t val;
    } pending_cfg; //0x6c

    union {
        struct {
            uint32_t pti: 4;
            uint32_t hw_ack_pti: 4;
            uint32_t close_rf_sel: 1;
            uint32_t reserved9: 23;
        };
        uint32_t val;
    } pti; //0x70

    uint32_t reserved_74; //0x74

    union {
        struct {
            uint32_t tx_abort_en: 31;
            uint32_t reserved31: 1;
        };
        uint32_t val;
    } tx_abort_event_en; //0x78

    uint32_t enhack_generate_done_notify; //0x7c

    union {
        struct {
            uint32_t filter_fail_reason: 4;
            uint32_t rx_abort_reason: 5;
            uint32_t reserved9: 7;
            uint32_t rx_state: 3;
            uint32_t reserved19: 1;
            uint32_t preamble_match: 1;
            uint32_t sfd_match: 1;
            uint32_t reserved22: 10;
        };
        uint32_t val;
    } rx_status; // 0x80

    union {
        struct {
            uint32_t tx_state: 4;
            uint32_t tx_abort_reason: 5;
            uint32_t reserved9: 7;
            uint32_t tx_security_error: 4;
            uint32_t reserved20: 12;
        };
        uint32_t val;
    } tx_status; //0x84

    union {
        struct {
            uint32_t txrx_status: 4;
            uint32_t reserved4: 4;
            uint32_t tx_proc: 1;
            uint32_t rx_proc: 1;
            uint32_t ed_proc: 1;
            uint32_t ed_trig_tx_proc: 1;
            uint32_t reserved12: 4;
            uint32_t rf_ctrl_state: 4;
            uint32_t reserved20: 12;
        };
        uint32_t val;
    } txrx_status; //0x88

    uint32_t tx_sec_schedule_state; //0x8c

    union {
        struct {
            uint32_t pkt_gck: 1;
            uint32_t ctrl_gck: 1;
            uint32_t reserved2: 30;
        };
        uint32_t val;
    } core_gck_cfg; //0x90

    uint32_t reserved_94; //0x94
    uint32_t reserved_98; //0x98
    uint32_t reserved_9c; //0x9c
    uint32_t reserved_a0; //0xa0

    uint32_t rx_length; //0xa4

    uint32_t timer0_threshold; //0xa8
    uint32_t timer0_value; //0xac
    uint32_t timer1_threshold; //0xb0
    uint32_t timer1_value; //0xb4
    uint32_t clk_counter_threshold; //0xb8
    uint32_t clk_counter_value; //0xbc

    union {
        struct {
            uint32_t ifs_counter: 10;
            uint32_t reserved10: 6;
            uint32_t ifs_counter_en: 1;
            uint32_t reserved17: 15;
        };
        uint32_t val;
    } ifs_counter_cfg; //0xc0

    union {
        struct {
            uint32_t sfd_wait_symbol_num: 4;
            uint32_t reserved4: 28;
        };
        uint32_t val;
    } sfd_wait; //0xc4

    union {
        struct {
            uint32_t tx_path_delay: 6;
            uint32_t reserved6: 10;
            uint32_t rx_path_delay: 6;
            uint32_t reserved624: 10;
        };
        uint32_t val;
    } txrx_path_delay; //0xc8

    uint32_t bb_clk; //0xcc
    uint32_t dma_tx_addr; //0xd0

    union {
        struct {
            uint32_t txdma_water_level: 3;
            uint32_t reserved3: 1;
            uint32_t txdma_fill_entry: 3;
            uint32_t reserved7: 9;
            uint32_t txdma_ctrl_state: 5;
            uint32_t reserved21: 3;
            uint32_t txdma_fetch_byte_cnt: 7;
            uint32_t reserved31: 1;
        };
        uint32_t val;
    } dma_tx_cfg; //0xd4

    uint32_t dma_tx_err; //0xd8

    uint32_t reserved_dc; //0xdc

    uint32_t dma_rx_addr; //0xe0

    union {
        struct {
            uint32_t rxdma_water_level: 3;
            uint32_t reserved3: 13;
            uint32_t rxdma_ctrl_state: 5;
            uint32_t reserved21: 3;
            uint32_t rxdma_append_lqi: 1;
            uint32_t rxdma_append_freq_offset: 1;
            uint32_t reserved26: 6;
        };
        uint32_t val;
    } dma_rx_cfg; //0xe4

    uint32_t dma_rx_err; //0xe8

    uint32_t reserved_ec; //x0ec

    uint32_t dma_gck; //0xf0
    uint32_t dma_dummy_data; //0xf4
    uint32_t reserved_f8; //0xf8
    uint32_t reserved_fc; //0xfc

    union {
        struct {
            uint32_t delay: 10;
            uint32_t reserved10: 22;
        };
        uint32_t val;
    } pa_on_delay; //0x100

    union {
        struct {
            uint32_t delay: 10;
            uint32_t reserved10: 22;
        };
        uint32_t val;
    } tx_on_delay; //0x104

    union {
        struct {
            uint32_t delay: 6;
            uint32_t reserved6: 26;
        };
        uint32_t val;
    } txen_stop_delay; //0x108

    union {
        struct {
            uint32_t delay: 6;
            uint32_t reserved6: 26;
        };
        uint32_t val;
    } tx_off_delay; //0x10c

    union {
        struct {
            uint32_t delay: 11;
            uint32_t reserved11: 21;
        };
        uint32_t val;
    } rx_on_delay; //0x110

    union {
        struct {
            uint32_t delay: 10;
            uint32_t reserved10: 22;
        };
        uint32_t val;
    } txrx_switch_delay; //0x114

    uint32_t cont_rx_delay; //0x118

    union {
        struct {
            uint32_t dcdc_pre_up_delay: 8;
            uint32_t dcdc_down_delay: 8;
            uint32_t dcdc_ctrl_en: 1;
            uint32_t reserved17: 14;
            uint32_t tx_dcdc_up: 1;
        };
        uint32_t val;
    } dcdc_ctrl; //0x11c

    union {
        struct {
            uint32_t debug_sel: 3;
            uint32_t reserved3: 5;
            uint32_t trig_st_sel: 4;
            uint32_t ser_debug_sel: 4;
            uint32_t trig_st_match_val: 5;
            uint32_t reserved21: 3;
            uint32_t trig_pulse_sel: 3;
            uint32_t reserved27: 3;
            uint32_t trig_st_match_dump_en: 1;
            uint32_t trig_pulse_dump_en: 1;
        };
        uint32_t val;
    } debug_ctrl; //0x120

    uint32_t tx_dma_err_sts_reg; //0x124
    union {
        struct {
            uint32_t tx_security_en: 1;
            uint32_t reserved1: 7;
            uint32_t security_offset: 7;
            uint32_t reserved15: 17;
        };
        uint32_t val;
    } security_ctrl; //0x128

    uint32_t security_addr0; //0x12c
    uint32_t security_addr1; //0x130
    uint32_t security_key0; //0x134
    uint32_t security_key1; //0x138
    uint32_t security_key2; //0x13c
    uint32_t security_key3; //0x140

    uint32_t debug_sfd_timeout_cnt; //0x144
    uint32_t debug_crc_error_cnt; //0x148
    uint32_t debug_ed_abort_cnt; //0x14c
    uint32_t debug_cca_fail_cnt; //0x150
    uint32_t debug_rx_filter_fail_cnt;  //0x154
    uint32_t debug_no_rss_detect_cnt;  //0x158
    uint32_t debug_rx_abort_coex_cnt;  //0x15c
    uint32_t debug_rx_restart_cnt;  //0x160
    uint32_t debug_tx_ack_abort_coex_cnt; //0x164
    uint32_t debug_ed_scan_break_coex_cnt; //0x168
    uint32_t debug_rx_ack_abort_coex_cnt; //0x16c
    uint32_t debug_rx_ack_timeout_cnt; //0x170
    uint32_t debug_tx_break_coex_cnt; //0x174
    uint32_t debug_tx_security_error_cnt; //0x178
    uint32_t debug_cca_busy_cnt; //0x17c

    union {
        struct {
            uint32_t debug_ed_scan_break_coex_cnt: 1;
            uint32_t debug_cca_busy_cnt: 1;
            uint32_t debug_cca_fail_cnt: 1;
            uint32_t debug_ed_abort_cnt: 1;
            uint32_t debug_tx_security_error_cnt: 1;
            uint32_t debug_tx_break_coex_cnt: 1;
            uint32_t debug_tx_ack_abort_coex_cnt: 1;
            uint32_t debug_rx_ack_timeout_cnt: 1;
            uint32_t debug_rx_restart_cnt: 1;
            uint32_t debug_rx_ack_abort_coex_cnt: 1;
            uint32_t debug_rx_abort_coex_cnt: 1;
            uint32_t debug_no_rss_detect_cnt: 1;
            uint32_t debug_rx_filter_fail_cnt: 1;
            uint32_t debug_crc_error_cnt: 1;
            uint32_t debug_sfd_timeout_cnt: 1;
            uint32_t reserved15: 17;
        };
        uint32_t val;
    } debug_cnt_clr; //0x180
    union {
        struct {
            uint32_t debug_field0_sel: 8;
            uint32_t debug_field1_sel: 8;
            uint32_t debug_field2_sel: 8;
            uint32_t debug_field3_sel: 8;
        };
        uint32_t val;
    } debug_sel_cfg0; //0x184
    union {
        struct {
            uint32_t debug_field4_sel: 8;
            uint32_t debug_field5_sel: 8;
            uint32_t debug_field6_sel: 8;
            uint32_t debug_field7_sel: 8;
        };
        uint32_t val;
    } debug_sel_cfg1; //0x188

    uint32_t i154_version; //0x18c

} esp_ieee802154_t;

extern esp_ieee802154_t IEEE802154;

#ifdef __cplusplus
}
#endif
