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

typedef struct emac_ptp_dev_s {
    volatile union {
        struct {
            uint32_t en_timestamp : 1;                /* Timestamp Enable */
            uint32_t ts_fine_coarse_update : 1;       /* Timestamp Fine or Coarse Update */
            uint32_t ts_initialize : 1;               /* Timestamp Initialize */
            uint32_t ts_update : 1;                   /* Timestamp Update */
            uint32_t en_ts_int_trig : 1;              /* Timestamp Interrupt Trigger Enable */
            uint32_t addend_reg_update : 1;           /* Addend Reg Update */
            uint32_t reserved1 : 2;                   /* Reserved */
            uint32_t en_ts4all : 1;                   /* Enable Timestamp for All Frames */
            uint32_t ts_digit_bin_roll_ctrl : 1;      /* Timestamp Digital or Binary Rollover Control */
            uint32_t en_ptp_pkg_proc_ver2_fmt : 1;    /* Enable PTP packet Processing for Version 2 Format */
            uint32_t en_proc_ptp_ether_frm : 1;       /* Enable Processing of PTP over Ethernet Frames */
            uint32_t en_proc_ptp_ipv6_udp : 1;        /* Enable Processing of PTP Frames Sent over IPv6-UDP */
            uint32_t en_proc_ptp_ipv4_udp : 1;        /* Enable Processing of PTP Frames Sent over IPv4-UDP */
            uint32_t en_ts_snap_event_msg : 1;        /* Enable Timestamp Snapshot for Event Messages */
            uint32_t en_snap_msg_relevant_master : 1; /* Enable Snapshot for Messages Relevant to Master */
            uint32_t sel_snap_type : 2;               /* Select PTP packets for Taking Snapshots */
            uint32_t en_mac_addr_filter : 1;          /* Enable MAC address for PTP Frame Filtering */
            uint32_t reserved2 : 5;                   /* Reserved */
            uint32_t aux_snap_fifo_clear : 1;         /* Auxiliary Snapshot FIFO Clear */
            uint32_t en_aux_snap0 : 1;                /* Auxiliary Snapshot 0 Enable */
            uint32_t en_aux_snap1 : 1;                /* Auxiliary Snapshot 1 Enable */
            uint32_t en_aux_snap2 : 1;                /* Auxiliary Snapshot 2 Enable */
            uint32_t en_aux_snap3 : 1;                /* Auxiliary Snapshot 3 Enable */
            uint32_t reserved3 : 3;                   /* Reserved */
        };
        uint32_t val;
    } timestamp_ctrl;
    volatile union {
        struct {
            uint32_t sub_second_incre_value : 8; /* Sub-second Increment Value */
            uint32_t reserved : 24;              /* Reserved */
        };
        uint32_t val;
    } sub_sec_incre;
    volatile union {
        struct {
            uint32_t ts_second : 32; /* Timestamp Second */
        };
        uint32_t val;
    } sys_seconds;
    volatile union {
        struct {
            uint32_t ts_sub_seconds : 31; /* Timestamp Sub Seconds */
            uint32_t reserved: 1;         /* Reserved */
        };
        uint32_t val;
    } sys_nanosec;
    volatile union {
        struct {
            uint32_t ts_second : 32; /* Timestamp Second */
        };
        uint32_t val;
    } sys_seconds_update;
    volatile union {
        struct {
            uint32_t ts_sub_seconds : 31; /* Timestamp Sub Seconds */
            uint32_t add_sub : 1;         /* Add or Subtract Time */
        };
        uint32_t val;
    } sys_nanosec_update;
    volatile union {
        struct {
            uint32_t ts_addend_val: 32; /* Timestamp Addend Register */
        };
        uint32_t val;
    } timestamp_addend;
    volatile union {
        struct {
            uint32_t tgt_time_second_val : 32; /* Target Time Seconds Register */
        };
        uint32_t val;
    } tgt_seconds;
    volatile union {
        struct {
            uint32_t tgt_ts_low_reg : 31;   /* Target Timestamp Low Register */
            uint32_t tgt_time_reg_busy : 1; /* Target Time Register Busy */
        };
        uint32_t val;
    } tgt_nanosec;
    volatile union {
        struct {
            uint32_t ts_higher_word : 16; /* Timestamp Higher Word Register */
            uint32_t reserved : 16;       /* Reserved */
        };
        uint32_t val;
    } sys_seconds_high;
    volatile union {
        struct {
            uint32_t ts_secons_ovf : 1;             /* Timestamp Seconds Overflow */
            uint32_t ts_tgt_time_reach : 1;         /* Timestamp Target Time Reached */
            uint32_t aux_ts_trig_snap : 1;          /* Auxiliary Timestamp Trigger Snapshot */
            uint32_t ts_tgt_time_err : 1;           /* Timestamp Target Time Error */
            uint32_t ts_tgt_time_reach_pps1 : 1;    /* Timestamp Target Time Reached for Target Time PPS1 */
            uint32_t ts_tgt_time_err1 : 1;          /* Timestamp Target Time Error */
            uint32_t ts_tgt_time_reach_pps2 : 1;    /* Timestamp Target Time Reached for Target Time PPS2 */
            uint32_t ts_tgt_time_err2 : 1;          /* Timestamp Target Time Error */
            uint32_t ts_tgt_time_reach_pps3 : 1;    /* Timestamp Target Time Reached for Target Time PPS3 */
            uint32_t ts_tgt_time_err3 : 1;          /* Timestamp Target Time Error */
            uint32_t reserved1 : 6;                 /* Reserved */
            uint32_t aux_ts_snap_trig_identify : 4; /* Auxiliary Timestamp Snapshot Trigger Identifier */
            uint32_t reserved2 : 4;                 /* Reserved */
            uint32_t aux_tx_snap_trig_miss : 1;     /* Auxiliary Timestamp Snapshot Trigger Missed */
            uint32_t aux_ts_snap_num : 5;           /* Number of Auxiliary Timestamp Snapshots */
            uint32_t reserved : 2;                  /* Reserved */
        };
        uint32_t val;
    } status;
    volatile union {
        struct {
            uint32_t pps_cmd0 : 4;      /* Flexible PPS0 Output Control */
            uint32_t en_pps0 : 1;       /* Flexible PPS Output Mode Enable */
            uint32_t tgt_mode_sel0 : 2; /* Target Time Register Mode for PPS0 Output */
            uint32_t reserved1 : 1;     /* Reserved */
            uint32_t pps_cmd1 : 3;      /* Flexible PPS1 Output Control */
            uint32_t reserved2 : 2;     /* Reserved */
            uint32_t tgt_mode_sel1 : 2; /* Target Time Register Mode for PPS1 Output */
            uint32_t reserved3 : 1;     /* Reserved */
            uint32_t pps_cmd2 : 3;      /* Flexible PPS2 Output Control */
            uint32_t reserved4 : 2;     /* Reserved */
            uint32_t tgt_mode_sel2 : 2; /* Target Time Register Mode for PPS2 Output */
            uint32_t reserved5 : 1;     /* Reserved */
            uint32_t pps_cmd3 : 3;      /* Flexible PPS3 Output Control */
            uint32_t reserved6 : 2;     /* Reserved */
            uint32_t tgt_mode_sel3 : 2; /* Target Time Register Mode for PPS3 Output */
            uint32_t reserved7 : 1;     /* Reserved */
        };
        uint32_t val;
    } pps_ctrl;
    volatile union {
        struct {
            uint32_t aux_ts_low : 31; /* Contains the lower 31 bits (nano-seconds field) of the auxiliary timestamp. */
            uint32_t reserved : 1;    /* Reserved */
        };
        uint32_t val;
    } aux_nanosec;
    volatile union {
        struct {
            uint32_t aux_tx_high : 32; /* Contains the lower 32 bits of the Seconds field of the auxiliary timestamp. */
        };
        uint32_t val;
    } aux_seconds;
    volatile union {
        struct {
            uint32_t av_ethertype_val : 16;       /* AV EtherType Value */
            uint32_t ac_queue_pri : 3;            /* AV Priority for Queuing */
            uint32_t en_queue_non_av_pkt : 1;     /* VLAN Tagged Non-AV Packets Queueing Enable */
            uint32_t dis_av_chann : 1;            /* AV Channel Disable */
            uint32_t queue_av_ctrl_pkt_chann : 2; /* Channel for Queuing the AV Control Packets */
            uint32_t reserved1 : 1;               /* Reserved */
            uint32_t queue_ptp_pkt_chann : 2;     /* Channel for Queuing the PTP Packets */
            uint32_t reserved2 : 6;               /* Reserved */
        };
        uint32_t val;
    } av_mac_ctrl;
    uint32_t reserved1[9]; /* Reserved */
    volatile union {
        struct {
            uint32_t pps0_interval : 32; /* PPS0 Output Signal Interval */
        };
        uint32_t val;
    } pps0_interval;
    volatile union {
        struct {
            uint32_t pps0_width : 32; /* PPS0 Output Signal Width */
        };
        uint32_t val;
    } pps0_width;
    uint32_t reserved2[6]; /* Reserved */
    volatile union {
        struct {
            uint32_t pps1_tgt_seconds : 32; /* PPS1 Target Time Seconds Register */
        };
        uint32_t val;
    } pps1_tgt_seconds;
    volatile union {
        struct {
            uint32_t pps1_tgt_nanosec : 31;  /* Target Time Low for PPS1 Register */
            uint32_t pps1_tgt_time_busy : 1; /* PPS1 Target Time Register Busy */
        };
        uint32_t val;
    } pps1_tgt_nanosec;
    volatile union {
        struct {
            uint32_t pps1_interval : 32; /* PPS1 Output Signal Interval */
        };
        uint32_t val;
    } pps1_interval;
    volatile union {
        struct {
            uint32_t pps1_width : 32; /* PPS1 Output Signal Width */
        };
        uint32_t val;
    } pps1_width;
    uint32_t reserved3[4]; /* Reserved */
    volatile union {
        struct {
            uint32_t pps2_tgt_seconds : 32; /* PPS2 Target Time Seconds Register */
        };
        uint32_t val;
    } pps2_tgt_seconds;
    volatile union {
        struct {
            uint32_t pps2_tgt_nanosec : 31;  /* Target Time Low for PPS2 Register */
            uint32_t pps2_tgt_time_busy : 1; /* PPS2 Target Time Register Busy */
        };
        uint32_t val;
    } pps2_tgt_nanosec;
    volatile union {
        struct {
            uint32_t pps2_interval : 32; /* PPS2 Output Signal Interval */
        };
        uint32_t val;
    } pps2_interval;
    volatile union {
        struct {
            uint32_t pps2_width : 32; /* PPS2 Output Signal Width */
        };
        uint32_t val;
    } pps2_width;
    uint32_t reserved4[4]; /* Reserved */
    volatile union {
        struct {
            uint32_t pps3_tgt_seconds : 32; /* PPS3 Target Time Seconds Register */
        };
        uint32_t val;
    } pps3_tgt_seconds;
    volatile union {
        struct {
            uint32_t pps3_tgt_nanosec : 31;  /* Target Time Low for PPS3 Register */
            uint32_t pps3_tgt_time_busy : 1; /* PPS3 Target Time Register Busy */
        };
        uint32_t val;
    } pps3_tgt_nanosec;
    volatile union {
        struct {
            uint32_t pps3_interval : 32; /* PPS3 Output Signal Interval */
        };
        uint32_t val;
    } pps3_interval;
    volatile union {
        struct {
            uint32_t pps3_width : 32; /* PPS3 Output Signal Width */
        };
        uint32_t val;
    } pps3_width;
} emac_ptp_dev_t;

extern emac_ptp_dev_t EMAC_PTP;

#ifdef __cplusplus
}
#endif
