/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: WORLD1 to WORLD0 configuration Registers */
/** Type of core_0_entry_1_addr register
 *  Core_0 Entry 1 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 1 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_1_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_1_addr_reg_t;

/** Type of core_0_entry_2_addr register
 *  Core_0 Entry 2 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_2_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 2 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_2_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_2_addr_reg_t;

/** Type of core_0_entry_3_addr register
 *  Core_0 Entry 3 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_3_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 3 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_3_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_3_addr_reg_t;

/** Type of core_0_entry_4_addr register
 *  Core_0 Entry 4 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_4_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 4 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_4_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_4_addr_reg_t;

/** Type of core_0_entry_5_addr register
 *  Core_0 Entry 5 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_5_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 5 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_5_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_5_addr_reg_t;

/** Type of core_0_entry_6_addr register
 *  Core_0 Entry 6 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_6_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 6 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_6_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_6_addr_reg_t;

/** Type of core_0_entry_7_addr register
 *  Core_0 Entry 7 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_7_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 7 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_7_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_7_addr_reg_t;

/** Type of core_0_entry_8_addr register
 *  Core_0 Entry 8 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_8_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 8 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_8_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_8_addr_reg_t;

/** Type of core_0_entry_9_addr register
 *  Core_0 Entry 9 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_9_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 9 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_9_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_9_addr_reg_t;

/** Type of core_0_entry_10_addr register
 *  Core_0 Entry 10 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_10_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 10 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_10_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_10_addr_reg_t;

/** Type of core_0_entry_11_addr register
 *  Core_0 Entry 11 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_11_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 11 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_11_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_11_addr_reg_t;

/** Type of core_0_entry_12_addr register
 *  Core_0 Entry 12 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_12_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 12 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_12_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_12_addr_reg_t;

/** Type of core_0_entry_13_addr register
 *  Core_0 Entry 13 address configuration Register
 */
typedef union {
    struct {
        /** core_0_entry_13_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_0 Entry 13 address from WORLD1 to WORLD0
         */
        uint32_t core_0_entry_13_addr:32;
    };
    uint32_t val;
} wcl_core_0_entry_13_addr_reg_t;

/** Type of core_0_entry_check register
 *  Core_0 Entry check configuration Register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** core_0_entry_check : R/W; bitpos: [13:1]; default: 1;
         *  This filed is used to enable entry address check
         */
        uint32_t core_0_entry_check:13;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} wcl_core_0_entry_check_reg_t;

/** Type of core_0_message_addr register
 *  Clear writer_buffer write address configuration register
 */
typedef union {
    struct {
        /** core_0_message_addr : R/W; bitpos: [31:0]; default: 0;
         *  This field is used to set address that need to write when enter WORLD0
         */
        uint32_t core_0_message_addr:32;
    };
    uint32_t val;
} wcl_core_0_message_addr_reg_t;

/** Type of core_0_message_max register
 *  Clear writer_buffer write number configuration register
 */
typedef union {
    struct {
        /** core_0_message_max : R/W; bitpos: [3:0]; default: 0;
         *  This filed is used to set the max value of clear write_buffer
         */
        uint32_t core_0_message_max:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} wcl_core_0_message_max_reg_t;

/** Type of core_0_message_phase register
 *  Clear writer_buffer status register
 */
typedef union {
    struct {
        /** core_0_message_match : RO; bitpos: [0]; default: 0;
         *  This bit indicates whether the check is successful
         */
        uint32_t core_0_message_match:1;
        /** core_0_message_expect : RO; bitpos: [4:1]; default: 0;
         *  This field indicates the data to be written next time
         */
        uint32_t core_0_message_expect:4;
        /** core_0_message_dataphase : RO; bitpos: [5]; default: 0;
         *  If this bit is 1, it means that is checking clear write_buffer operation,and is
         *  checking data
         */
        uint32_t core_0_message_dataphase:1;
        /** core_0_message_addressphase : RO; bitpos: [6]; default: 0;
         *  If this bit is 1, it means that is checking clear write_buffer operation,and is
         *  checking address.
         */
        uint32_t core_0_message_addressphase:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} wcl_core_0_message_phase_reg_t;

/** Type of core_1_entry_1_addr register
 *  Core_1 Entry 1 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_1_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 1 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_1_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_1_addr_reg_t;

/** Type of core_1_entry_2_addr register
 *  Core_1 Entry 2 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_2_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 2 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_2_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_2_addr_reg_t;

/** Type of core_1_entry_3_addr register
 *  Core_1 Entry 3 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_3_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 3 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_3_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_3_addr_reg_t;

/** Type of core_1_entry_4_addr register
 *  Core_1 Entry 4 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_4_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 4 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_4_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_4_addr_reg_t;

/** Type of core_1_entry_5_addr register
 *  Core_1 Entry 5 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_5_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 5 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_5_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_5_addr_reg_t;

/** Type of core_1_entry_6_addr register
 *  Core_1 Entry 6 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_6_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 6 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_6_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_6_addr_reg_t;

/** Type of core_1_entry_7_addr register
 *  Core_1 Entry 7 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_7_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 7 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_7_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_7_addr_reg_t;

/** Type of core_1_entry_8_addr register
 *  Core_1 Entry 8 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_8_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 8 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_8_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_8_addr_reg_t;

/** Type of core_1_entry_9_addr register
 *  Core_1 Entry 9 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_9_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 9 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_9_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_9_addr_reg_t;

/** Type of core_1_entry_10_addr register
 *  Core_1 Entry 10 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_10_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 10 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_10_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_10_addr_reg_t;

/** Type of core_1_entry_11_addr register
 *  Core_1 Entry 11 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_11_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 11 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_11_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_11_addr_reg_t;

/** Type of core_1_entry_12_addr register
 *  Core_1 Entry 12 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_12_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 12 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_12_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_12_addr_reg_t;

/** Type of core_1_entry_13_addr register
 *  Core_1 Entry 13 address configuration Register
 */
typedef union {
    struct {
        /** core_1_entry_13_addr : R/W; bitpos: [31:0]; default: 0;
         *  Core_1 Entry 13 address from WORLD1 to WORLD0
         */
        uint32_t core_1_entry_13_addr:32;
    };
    uint32_t val;
} wcl_core_1_entry_13_addr_reg_t;

/** Type of core_1_entry_check register
 *  Core_1 Entry check configuration Register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** core_1_entry_check : R/W; bitpos: [13:1]; default: 1;
         *  This filed is used to enable entry address check
         */
        uint32_t core_1_entry_check:13;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} wcl_core_1_entry_check_reg_t;

/** Type of core_1_message_addr register
 *  Clear writer_buffer write address configuration register
 */
typedef union {
    struct {
        /** core_1_message_addr : R/W; bitpos: [31:0]; default: 0;
         *  This field is used to set address that need to write when enter WORLD0
         */
        uint32_t core_1_message_addr:32;
    };
    uint32_t val;
} wcl_core_1_message_addr_reg_t;

/** Type of core_1_message_max register
 *  Clear writer_buffer write number configuration register
 */
typedef union {
    struct {
        /** core_1_message_max : R/W; bitpos: [3:0]; default: 0;
         *  This filed is used to set the max value of clear write_buffer
         */
        uint32_t core_1_message_max:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} wcl_core_1_message_max_reg_t;

/** Type of core_1_message_phase register
 *  Clear writer_buffer status register
 */
typedef union {
    struct {
        /** core_1_message_match : RO; bitpos: [0]; default: 0;
         *  This bit indicates whether the check is successful
         */
        uint32_t core_1_message_match:1;
        /** core_1_message_expect : RO; bitpos: [4:1]; default: 0;
         *  This field indicates the data to be written next time
         */
        uint32_t core_1_message_expect:4;
        /** core_1_message_dataphase : RO; bitpos: [5]; default: 0;
         *  If this bit is 1, it means that is checking clear write_buffer operation, and is
         *  checking data
         */
        uint32_t core_1_message_dataphase:1;
        /** core_1_message_addressphase : RO; bitpos: [6]; default: 0;
         *  If this bit is 1, it means that is checking clear write_buffer operation, and is
         *  checking address.
         */
        uint32_t core_1_message_addressphase:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} wcl_core_1_message_phase_reg_t;


/** Group: StatusTable Registers */
/** Type of core_0_statustable1 register
 *  Status register of world switch of entry 1
 */
typedef union {
    struct {
        /** core_0_from_world_1 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 1
         */
        uint32_t core_0_from_world_1:1;
        /** core_0_from_entry_1 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 1
         */
        uint32_t core_0_from_entry_1:4;
        /** core_0_current_1 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 1
         */
        uint32_t core_0_current_1:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable1_reg_t;

/** Type of core_0_statustable2 register
 *  Status register of world switch of entry 2
 */
typedef union {
    struct {
        /** core_0_from_world_2 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 2
         */
        uint32_t core_0_from_world_2:1;
        /** core_0_from_entry_2 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 2
         */
        uint32_t core_0_from_entry_2:4;
        /** core_0_current_2 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 2
         */
        uint32_t core_0_current_2:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable2_reg_t;

/** Type of core_0_statustable3 register
 *  Status register of world switch of entry 3
 */
typedef union {
    struct {
        /** core_0_from_world_3 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 3
         */
        uint32_t core_0_from_world_3:1;
        /** core_0_from_entry_3 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 3
         */
        uint32_t core_0_from_entry_3:4;
        /** core_0_current_3 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 3
         */
        uint32_t core_0_current_3:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable3_reg_t;

/** Type of core_0_statustable4 register
 *  Status register of world switch of entry 4
 */
typedef union {
    struct {
        /** core_0_from_world_4 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 4
         */
        uint32_t core_0_from_world_4:1;
        /** core_0_from_entry_4 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 4
         */
        uint32_t core_0_from_entry_4:4;
        /** core_0_current_4 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 4
         */
        uint32_t core_0_current_4:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable4_reg_t;

/** Type of core_0_statustable5 register
 *  Status register of world switch of entry 5
 */
typedef union {
    struct {
        /** core_0_from_world_5 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 5
         */
        uint32_t core_0_from_world_5:1;
        /** core_0_from_entry_5 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 5
         */
        uint32_t core_0_from_entry_5:4;
        /** core_0_current_5 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 5
         */
        uint32_t core_0_current_5:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable5_reg_t;

/** Type of core_0_statustable6 register
 *  Status register of world switch of entry 6
 */
typedef union {
    struct {
        /** core_0_from_world_6 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 6
         */
        uint32_t core_0_from_world_6:1;
        /** core_0_from_entry_6 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 6
         */
        uint32_t core_0_from_entry_6:4;
        /** core_0_current_6 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 6
         */
        uint32_t core_0_current_6:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable6_reg_t;

/** Type of core_0_statustable7 register
 *  Status register of world switch of entry 7
 */
typedef union {
    struct {
        /** core_0_from_world_7 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 7
         */
        uint32_t core_0_from_world_7:1;
        /** core_0_from_entry_7 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 7
         */
        uint32_t core_0_from_entry_7:4;
        /** core_0_current_7 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 7
         */
        uint32_t core_0_current_7:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable7_reg_t;

/** Type of core_0_statustable8 register
 *  Status register of world switch of entry 8
 */
typedef union {
    struct {
        /** core_0_from_world_8 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 8
         */
        uint32_t core_0_from_world_8:1;
        /** core_0_from_entry_8 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 8
         */
        uint32_t core_0_from_entry_8:4;
        /** core_0_current_8 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 8
         */
        uint32_t core_0_current_8:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable8_reg_t;

/** Type of core_0_statustable9 register
 *  Status register of world switch of entry 9
 */
typedef union {
    struct {
        /** core_0_from_world_9 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 9
         */
        uint32_t core_0_from_world_9:1;
        /** core_0_from_entry_9 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 9
         */
        uint32_t core_0_from_entry_9:4;
        /** core_0_current_9 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 9
         */
        uint32_t core_0_current_9:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable9_reg_t;

/** Type of core_0_statustable10 register
 *  Status register of world switch of entry 10
 */
typedef union {
    struct {
        /** core_0_from_world_10 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 10
         */
        uint32_t core_0_from_world_10:1;
        /** core_0_from_entry_10 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 10
         */
        uint32_t core_0_from_entry_10:4;
        /** core_0_current_10 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 10
         */
        uint32_t core_0_current_10:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable10_reg_t;

/** Type of core_0_statustable11 register
 *  Status register of world switch of entry 11
 */
typedef union {
    struct {
        /** core_0_from_world_11 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 11
         */
        uint32_t core_0_from_world_11:1;
        /** core_0_from_entry_11 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 11
         */
        uint32_t core_0_from_entry_11:4;
        /** core_0_current_11 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 11
         */
        uint32_t core_0_current_11:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable11_reg_t;

/** Type of core_0_statustable12 register
 *  Status register of world switch of entry 12
 */
typedef union {
    struct {
        /** core_0_from_world_12 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 12
         */
        uint32_t core_0_from_world_12:1;
        /** core_0_from_entry_12 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 12
         */
        uint32_t core_0_from_entry_12:4;
        /** core_0_current_12 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 12
         */
        uint32_t core_0_current_12:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable12_reg_t;

/** Type of core_0_statustable13 register
 *  Status register of world switch of entry 13
 */
typedef union {
    struct {
        /** core_0_from_world_13 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 13
         */
        uint32_t core_0_from_world_13:1;
        /** core_0_from_entry_13 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 13
         */
        uint32_t core_0_from_entry_13:4;
        /** core_0_current_13 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 13
         */
        uint32_t core_0_current_13:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_0_statustable13_reg_t;

/** Type of core_0_statustable_current register
 *  Status register of statustable current
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** core_0_statustable_current : R/W; bitpos: [13:1]; default: 0;
         *  This field is used to quickly read and rewrite the current field of all STATUSTABLE
         *  registers,for example,bit 1 represents the current field of STATUSTABLE1,bit2
         *  represents the current field of STATUSTABLE2
         */
        uint32_t core_0_statustable_current:13;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} wcl_core_0_statustable_current_reg_t;

/** Type of core_1_statustable1 register
 *  Status register of world switch of entry 1
 */
typedef union {
    struct {
        /** core_1_from_world_1 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 1
         */
        uint32_t core_1_from_world_1:1;
        /** core_1_from_entry_1 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 1
         */
        uint32_t core_1_from_entry_1:4;
        /** core_1_current_1 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 1
         */
        uint32_t core_1_current_1:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable1_reg_t;

/** Type of core_1_statustable2 register
 *  Status register of world switch of entry 2
 */
typedef union {
    struct {
        /** core_1_from_world_2 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 2
         */
        uint32_t core_1_from_world_2:1;
        /** core_1_from_entry_2 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 2
         */
        uint32_t core_1_from_entry_2:4;
        /** core_1_current_2 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 2
         */
        uint32_t core_1_current_2:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable2_reg_t;

/** Type of core_1_statustable3 register
 *  Status register of world switch of entry 3
 */
typedef union {
    struct {
        /** core_1_from_world_3 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 3
         */
        uint32_t core_1_from_world_3:1;
        /** core_1_from_entry_3 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 3
         */
        uint32_t core_1_from_entry_3:4;
        /** core_1_current_3 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 3
         */
        uint32_t core_1_current_3:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable3_reg_t;

/** Type of core_1_statustable4 register
 *  Status register of world switch of entry 4
 */
typedef union {
    struct {
        /** core_1_from_world_4 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 4
         */
        uint32_t core_1_from_world_4:1;
        /** core_1_from_entry_4 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 4
         */
        uint32_t core_1_from_entry_4:4;
        /** core_1_current_4 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 4
         */
        uint32_t core_1_current_4:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable4_reg_t;

/** Type of core_1_statustable5 register
 *  Status register of world switch of entry 5
 */
typedef union {
    struct {
        /** core_1_from_world_5 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 5
         */
        uint32_t core_1_from_world_5:1;
        /** core_1_from_entry_5 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 5
         */
        uint32_t core_1_from_entry_5:4;
        /** core_1_current_5 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 5
         */
        uint32_t core_1_current_5:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable5_reg_t;

/** Type of core_1_statustable6 register
 *  Status register of world switch of entry 6
 */
typedef union {
    struct {
        /** core_1_from_world_6 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 6
         */
        uint32_t core_1_from_world_6:1;
        /** core_1_from_entry_6 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 6
         */
        uint32_t core_1_from_entry_6:4;
        /** core_1_current_6 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 6
         */
        uint32_t core_1_current_6:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable6_reg_t;

/** Type of core_1_statustable7 register
 *  Status register of world switch of entry 7
 */
typedef union {
    struct {
        /** core_1_from_world_7 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 7
         */
        uint32_t core_1_from_world_7:1;
        /** core_1_from_entry_7 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 7
         */
        uint32_t core_1_from_entry_7:4;
        /** core_1_current_7 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 7
         */
        uint32_t core_1_current_7:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable7_reg_t;

/** Type of core_1_statustable8 register
 *  Status register of world switch of entry 8
 */
typedef union {
    struct {
        /** core_1_from_world_8 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 8
         */
        uint32_t core_1_from_world_8:1;
        /** core_1_from_entry_8 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 8
         */
        uint32_t core_1_from_entry_8:4;
        /** core_1_current_8 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 8
         */
        uint32_t core_1_current_8:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable8_reg_t;

/** Type of core_1_statustable9 register
 *  Status register of world switch of entry 9
 */
typedef union {
    struct {
        /** core_1_from_world_9 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 9
         */
        uint32_t core_1_from_world_9:1;
        /** core_1_from_entry_9 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 9
         */
        uint32_t core_1_from_entry_9:4;
        /** core_1_current_9 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 9
         */
        uint32_t core_1_current_9:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable9_reg_t;

/** Type of core_1_statustable10 register
 *  Status register of world switch of entry 10
 */
typedef union {
    struct {
        /** core_1_from_world_10 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 10
         */
        uint32_t core_1_from_world_10:1;
        /** core_1_from_entry_10 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 10
         */
        uint32_t core_1_from_entry_10:4;
        /** core_1_current_10 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 10
         */
        uint32_t core_1_current_10:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable10_reg_t;

/** Type of core_1_statustable11 register
 *  Status register of world switch of entry 11
 */
typedef union {
    struct {
        /** core_1_from_world_11 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 11
         */
        uint32_t core_1_from_world_11:1;
        /** core_1_from_entry_11 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 11
         */
        uint32_t core_1_from_entry_11:4;
        /** core_1_current_11 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 11
         */
        uint32_t core_1_current_11:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable11_reg_t;

/** Type of core_1_statustable12 register
 *  Status register of world switch of entry 12
 */
typedef union {
    struct {
        /** core_1_from_world_12 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 12
         */
        uint32_t core_1_from_world_12:1;
        /** core_1_from_entry_12 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 12
         */
        uint32_t core_1_from_entry_12:4;
        /** core_1_current_12 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 12
         */
        uint32_t core_1_current_12:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable12_reg_t;

/** Type of core_1_statustable13 register
 *  Status register of world switch of entry 13
 */
typedef union {
    struct {
        /** core_1_from_world_13 : R/W; bitpos: [0]; default: 0;
         *  This bit is used to confirm world before enter entry 13
         */
        uint32_t core_1_from_world_13:1;
        /** core_1_from_entry_13 : R/W; bitpos: [4:1]; default: 0;
         *  This filed is used to confirm in which entry before enter entry 13
         */
        uint32_t core_1_from_entry_13:4;
        /** core_1_current_13 : R/W; bitpos: [5]; default: 0;
         *  This bit is used to confirm whether the current state is in entry 13
         */
        uint32_t core_1_current_13:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} wcl_core_1_statustable13_reg_t;

/** Type of core_1_statustable_current register
 *  Status register of statustable current
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** core_1_statustable_current : R/W; bitpos: [13:1]; default: 0;
         *  This field is used to quickly read and rewrite the current field of all STATUSTABLE
         *  registers,for example,bit 1 represents the current field of STATUSTABLE1
         */
        uint32_t core_1_statustable_current:13;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} wcl_core_1_statustable_current_reg_t;


/** Group: WORLD0 to WORLD1 configuration Registers */
/** Type of core_0_world_trigger_addr register
 *  Core_0 trigger address configuration Register
 */
typedef union {
    struct {
        /** core_0_world_trigger_addr : RW; bitpos: [31:0]; default: 0;
         *  This field is used to configure the entry address from WORLD0 to WORLD1,when the
         *  CPU executes to this address,switch to WORLD1
         */
        uint32_t core_0_world_trigger_addr:32;
    };
    uint32_t val;
} wcl_core_0_world_trigger_addr_reg_t;

/** Type of core_0_world_prepare register
 *  Core_0 prepare world configuration Register
 */
typedef union {
    struct {
        /** core_0_world_prepare : R/W; bitpos: [1:0]; default: 0;
         *  This field to used to set world to enter,  2'b01 means WORLD0, 2'b10 means WORLD1
         */
        uint32_t core_0_world_prepare:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} wcl_core_0_world_prepare_reg_t;

/** Type of core_0_world_update register
 *  Core_0 configuration update register
 */
typedef union {
    struct {
        /** core_0_update : WO; bitpos: [31:0]; default: 0;
         *  This field is used to update configuration completed, can write any value,the
         *  hardware only checks the write operation of this register and does not case about
         *  its value
         */
        uint32_t core_0_update:32;
    };
    uint32_t val;
} wcl_core_0_world_update_reg_t;

/** Type of core_0_world_cancel register
 *  Core_0 configuration cancel register
 */
typedef union {
    struct {
        /** core_0_world_cancel : WO; bitpos: [31:0]; default: 0;
         *  This field is used to cancel switch world configuration,if the trigger address and
         *  update configuration complete,use this register to cancel world switch, jujst need
         *  write any value,the hardware only checks the write operation of this register and
         *  does not case about its value
         */
        uint32_t core_0_world_cancel:32;
    };
    uint32_t val;
} wcl_core_0_world_cancel_reg_t;

/** Type of core_0_world_iram0 register
 *  Core_0 Iram0 world register
 */
typedef union {
    struct {
        /** core_0_world_iram0 : R/W; bitpos: [1:0]; default: 0;
         *  this field is used to read current world of Iram0 bus
         */
        uint32_t core_0_world_iram0:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} wcl_core_0_world_iram0_reg_t;

/** Type of core_0_world_dram0_pif register
 *  Core_0 dram0 and PIF world register
 */
typedef union {
    struct {
        /** core_0_world_dram0_pif : R/W; bitpos: [1:0]; default: 0;
         *  this field is used to read current world of Dram0 bus and PIF bus
         */
        uint32_t core_0_world_dram0_pif:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} wcl_core_0_world_dram0_pif_reg_t;

/** Type of core_0_world_phase register
 *  Core_0 world status register
 */
typedef union {
    struct {
        /** core_0_world_phase : RO; bitpos: [0]; default: 0;
         *  This bit indicates whether is preparing to switch to WORLD1, 1 means value.
         */
        uint32_t core_0_world_phase:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} wcl_core_0_world_phase_reg_t;

/** Type of core_1_world_trigger_addr register
 *  Core_1 trigger address configuration Register
 */
typedef union {
    struct {
        /** core_1_world_trigger_addr : RW; bitpos: [31:0]; default: 0;
         *  This field is used to configure the entry address from WORLD0 to WORLD1,when the
         *  CPU executes to this address,switch to WORLD1
         */
        uint32_t core_1_world_trigger_addr:32;
    };
    uint32_t val;
} wcl_core_1_world_trigger_addr_reg_t;

/** Type of core_1_world_prepare register
 *  Core_1 prepare world configuration Register
 */
typedef union {
    struct {
        /** core_1_world_prepare : R/W; bitpos: [1:0]; default: 0;
         *  This field to used to set world to enter,2'b01 means WORLD0, 2'b10 means WORLD1
         */
        uint32_t core_1_world_prepare:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} wcl_core_1_world_prepare_reg_t;

/** Type of core_1_world_update register
 *  Core_1 configuration update register
 */
typedef union {
    struct {
        /** core_1_update : WO; bitpos: [31:0]; default: 0;
         *  This field is used to update configuration completed, can write any value,the
         *  hardware only checks the write operation of this register and does not case about
         *  its value
         */
        uint32_t core_1_update:32;
    };
    uint32_t val;
} wcl_core_1_world_update_reg_t;

/** Type of core_1_world_cancel register
 *  Core_1 configuration cancel register
 */
typedef union {
    struct {
        /** core_1_world_cancel : WO; bitpos: [31:0]; default: 0;
         *  This field is used to cancel switch world configuration,if the trigger address and
         *  update configuration complete,can use this register to cancel world switch. can
         *  write any value, the hardware only checks the write operation of this register and
         *  does not case about its value
         */
        uint32_t core_1_world_cancel:32;
    };
    uint32_t val;
} wcl_core_1_world_cancel_reg_t;

/** Type of core_1_world_iram0 register
 *  Core_1 Iram0 world register
 */
typedef union {
    struct {
        /** core_1_world_iram0 : R/W; bitpos: [1:0]; default: 0;
         *  this field is used to read current world of Iram0 bus
         */
        uint32_t core_1_world_iram0:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} wcl_core_1_world_iram0_reg_t;

/** Type of core_1_world_dram0_pif register
 *  Core_1 dram0 and PIF world register
 */
typedef union {
    struct {
        /** core_1_world_dram0_pif : R/W; bitpos: [1:0]; default: 0;
         *  this field is used to read current world of Dram0 bus and PIF bus
         */
        uint32_t core_1_world_dram0_pif:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} wcl_core_1_world_dram0_pif_reg_t;

/** Type of core_1_world_phase register
 *  Core_0 world status register
 */
typedef union {
    struct {
        /** core_1_world_phase : RO; bitpos: [0]; default: 0;
         *  This bit indicates whether is preparing to switch to WORLD1,1 means value.
         */
        uint32_t core_1_world_phase:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} wcl_core_1_world_phase_reg_t;


/** Group: NMI mask configuration Registers */
/** Type of core_0_nmi_mask_enable register
 *  Core_0 NMI mask enable register
 */
typedef union {
    struct {
        /** core_0_nmi_mask_enable : WO; bitpos: [31:0]; default: 0;
         *  this field is used to set NMI mask,it can write any value,when write this
         *  register,the hardware start masking NMI interrupt
         */
        uint32_t core_0_nmi_mask_enable:32;
    };
    uint32_t val;
} wcl_core_0_nmi_mask_enable_reg_t;

/** Type of core_0_nmi_mask_trigger_addr register
 *  Core_0 NMI mask trigger address register
 */
typedef union {
    struct {
        /** core_0_nmi_mask_trigger_addr : R/W; bitpos: [31:0]; default: 0;
         *  this field to used to set trigger address, when CPU executes to this address,NMI
         *  mask automatically fails
         */
        uint32_t core_0_nmi_mask_trigger_addr:32;
    };
    uint32_t val;
} wcl_core_0_nmi_mask_trigger_addr_reg_t;

/** Type of core_0_nmi_mask_disable register
 *  Core_0 NMI mask disable register
 */
typedef union {
    struct {
        /** core_0_nmi_mask_disable : WO; bitpos: [31:0]; default: 0;
         *  this field is used to disable NMI mask,it will not take effect immediately,only
         *  when the CPU executes to the trigger address will it start to cancel NMI mask
         */
        uint32_t core_0_nmi_mask_disable:32;
    };
    uint32_t val;
} wcl_core_0_nmi_mask_disable_reg_t;

/** Type of core_0_nmi_mask_cancle register
 *  Core_0 NMI mask disable register
 */
typedef union {
    struct {
        /** core_0_nmi_mask_cancel : WO; bitpos: [31:0]; default: 0;
         *  this field is used to cancel NMI mask disable function.
         */
        uint32_t core_0_nmi_mask_cancel:32;
    };
    uint32_t val;
} wcl_core_0_nmi_mask_cancle_reg_t;

/** Type of core_0_nmi_mask register
 *  Core_0 NMI mask register
 */
typedef union {
    struct {
        /** core_0_nmi_mask : R/W; bitpos: [0]; default: 0;
         *  this bit is used to mask NMI interrupt,it can directly mask NMI interrupt
         */
        uint32_t core_0_nmi_mask:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} wcl_core_0_nmi_mask_reg_t;

/** Type of core_0_nmi_mask_phase register
 *  Core_0 NMI mask phase register
 */
typedef union {
    struct {
        /** core_0_nmi_mask_phase : RO; bitpos: [0]; default: 0;
         *  this bit is used to indicates whether the NMI interrupt is being masked, 1 means
         *  NMI interrupt is being masked
         */
        uint32_t core_0_nmi_mask_phase:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} wcl_core_0_nmi_mask_phase_reg_t;

/** Type of core_1_nmi_mask_enable register
 *  Core_1 NMI mask enable register
 */
typedef union {
    struct {
        /** core_1_nmi_mask_enable : WO; bitpos: [31:0]; default: 0;
         *  this field is used to set NMI mask, it can write any value, when write this
         *  register,the hardware start masking NMI interrupt
         */
        uint32_t core_1_nmi_mask_enable:32;
    };
    uint32_t val;
} wcl_core_1_nmi_mask_enable_reg_t;

/** Type of core_1_nmi_mask_trigger_addr register
 *  Core_1 NMI mask trigger addr register
 */
typedef union {
    struct {
        /** core_1_nmi_mask_trigger_addr : R/W; bitpos: [31:0]; default: 0;
         *  this field to used to set trigger address
         */
        uint32_t core_1_nmi_mask_trigger_addr:32;
    };
    uint32_t val;
} wcl_core_1_nmi_mask_trigger_addr_reg_t;

/** Type of core_1_nmi_mask_disable register
 *  Core_1 NMI mask disable register
 */
typedef union {
    struct {
        /** core_1_nmi_mask_disable : WO; bitpos: [31:0]; default: 0;
         *  this field is used to disable NMI mask, it will not take effect immediately,only
         *  when the CPU executes to the trigger address will it start to cancel NMI mask
         */
        uint32_t core_1_nmi_mask_disable:32;
    };
    uint32_t val;
} wcl_core_1_nmi_mask_disable_reg_t;

/** Type of core_1_nmi_mask_cancle register
 *  Core_1 NMI mask disable register
 */
typedef union {
    struct {
        /** core_1_nmi_mask_cancel : WO; bitpos: [31:0]; default: 0;
         *  this field is used to cancel NMI mask disable function.
         */
        uint32_t core_1_nmi_mask_cancel:32;
    };
    uint32_t val;
} wcl_core_1_nmi_mask_cancle_reg_t;

/** Type of core_1_nmi_mask register
 *  Core_1 NMI mask register
 */
typedef union {
    struct {
        /** core_1_nmi_mask : R/W; bitpos: [0]; default: 0;
         *  this bit is used to mask NMI interrupt,it can directly mask NMI interrupt
         */
        uint32_t core_1_nmi_mask:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} wcl_core_1_nmi_mask_reg_t;

/** Type of core_1_nmi_mask_phase register
 *  Core_1 NMI mask phase register
 */
typedef union {
    struct {
        /** core_1_nmi_mask_phase : RO; bitpos: [0]; default: 0;
         *  this bit is used to indicates whether the NMI interrupt is being masked, 1 means
         *  NMI interrupt is being masked
         */
        uint32_t core_1_nmi_mask_phase:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} wcl_core_1_nmi_mask_phase_reg_t;


typedef struct {
    volatile wcl_core_0_entry_1_addr_reg_t core_0_entry_1_addr;
    volatile wcl_core_0_entry_2_addr_reg_t core_0_entry_2_addr;
    volatile wcl_core_0_entry_3_addr_reg_t core_0_entry_3_addr;
    volatile wcl_core_0_entry_4_addr_reg_t core_0_entry_4_addr;
    volatile wcl_core_0_entry_5_addr_reg_t core_0_entry_5_addr;
    volatile wcl_core_0_entry_6_addr_reg_t core_0_entry_6_addr;
    volatile wcl_core_0_entry_7_addr_reg_t core_0_entry_7_addr;
    volatile wcl_core_0_entry_8_addr_reg_t core_0_entry_8_addr;
    volatile wcl_core_0_entry_9_addr_reg_t core_0_entry_9_addr;
    volatile wcl_core_0_entry_10_addr_reg_t core_0_entry_10_addr;
    volatile wcl_core_0_entry_11_addr_reg_t core_0_entry_11_addr;
    volatile wcl_core_0_entry_12_addr_reg_t core_0_entry_12_addr;
    volatile wcl_core_0_entry_13_addr_reg_t core_0_entry_13_addr;
    uint32_t reserved_034[18];
    volatile wcl_core_0_entry_check_reg_t core_0_entry_check;
    volatile wcl_core_0_statustable1_reg_t core_0_statustable1;
    volatile wcl_core_0_statustable2_reg_t core_0_statustable2;
    volatile wcl_core_0_statustable3_reg_t core_0_statustable3;
    volatile wcl_core_0_statustable4_reg_t core_0_statustable4;
    volatile wcl_core_0_statustable5_reg_t core_0_statustable5;
    volatile wcl_core_0_statustable6_reg_t core_0_statustable6;
    volatile wcl_core_0_statustable7_reg_t core_0_statustable7;
    volatile wcl_core_0_statustable8_reg_t core_0_statustable8;
    volatile wcl_core_0_statustable9_reg_t core_0_statustable9;
    volatile wcl_core_0_statustable10_reg_t core_0_statustable10;
    volatile wcl_core_0_statustable11_reg_t core_0_statustable11;
    volatile wcl_core_0_statustable12_reg_t core_0_statustable12;
    volatile wcl_core_0_statustable13_reg_t core_0_statustable13;
    uint32_t reserved_0b4[18];
    volatile wcl_core_0_statustable_current_reg_t core_0_statustable_current;
    volatile wcl_core_0_message_addr_reg_t core_0_message_addr;
    volatile wcl_core_0_message_max_reg_t core_0_message_max;
    volatile wcl_core_0_message_phase_reg_t core_0_message_phase;
    uint32_t reserved_10c[13];
    volatile wcl_core_0_world_trigger_addr_reg_t core_0_world_trigger_addr;
    volatile wcl_core_0_world_prepare_reg_t core_0_world_prepare;
    volatile wcl_core_0_world_update_reg_t core_0_world_update;
    volatile wcl_core_0_world_cancel_reg_t core_0_world_cancel;
    volatile wcl_core_0_world_iram0_reg_t core_0_world_iram0;
    volatile wcl_core_0_world_dram0_pif_reg_t core_0_world_dram0_pif;
    volatile wcl_core_0_world_phase_reg_t core_0_world_phase;
    uint32_t reserved_15c[9];
    volatile wcl_core_0_nmi_mask_enable_reg_t core_0_nmi_mask_enable;
    volatile wcl_core_0_nmi_mask_trigger_addr_reg_t core_0_nmi_mask_trigger_addr;
    volatile wcl_core_0_nmi_mask_disable_reg_t core_0_nmi_mask_disable;
    volatile wcl_core_0_nmi_mask_cancle_reg_t core_0_nmi_mask_cancle;
    volatile wcl_core_0_nmi_mask_reg_t core_0_nmi_mask;
    volatile wcl_core_0_nmi_mask_phase_reg_t core_0_nmi_mask_phase;
    uint32_t reserved_198[154];
    volatile wcl_core_1_entry_1_addr_reg_t core_1_entry_1_addr;
    volatile wcl_core_1_entry_2_addr_reg_t core_1_entry_2_addr;
    volatile wcl_core_1_entry_3_addr_reg_t core_1_entry_3_addr;
    volatile wcl_core_1_entry_4_addr_reg_t core_1_entry_4_addr;
    volatile wcl_core_1_entry_5_addr_reg_t core_1_entry_5_addr;
    volatile wcl_core_1_entry_6_addr_reg_t core_1_entry_6_addr;
    volatile wcl_core_1_entry_7_addr_reg_t core_1_entry_7_addr;
    volatile wcl_core_1_entry_8_addr_reg_t core_1_entry_8_addr;
    volatile wcl_core_1_entry_9_addr_reg_t core_1_entry_9_addr;
    volatile wcl_core_1_entry_10_addr_reg_t core_1_entry_10_addr;
    volatile wcl_core_1_entry_11_addr_reg_t core_1_entry_11_addr;
    volatile wcl_core_1_entry_12_addr_reg_t core_1_entry_12_addr;
    volatile wcl_core_1_entry_13_addr_reg_t core_1_entry_13_addr;
    uint32_t reserved_434[18];
    volatile wcl_core_1_entry_check_reg_t core_1_entry_check;
    volatile wcl_core_1_statustable1_reg_t core_1_statustable1;
    volatile wcl_core_1_statustable2_reg_t core_1_statustable2;
    volatile wcl_core_1_statustable3_reg_t core_1_statustable3;
    volatile wcl_core_1_statustable4_reg_t core_1_statustable4;
    volatile wcl_core_1_statustable5_reg_t core_1_statustable5;
    volatile wcl_core_1_statustable6_reg_t core_1_statustable6;
    volatile wcl_core_1_statustable7_reg_t core_1_statustable7;
    volatile wcl_core_1_statustable8_reg_t core_1_statustable8;
    volatile wcl_core_1_statustable9_reg_t core_1_statustable9;
    volatile wcl_core_1_statustable10_reg_t core_1_statustable10;
    volatile wcl_core_1_statustable11_reg_t core_1_statustable11;
    volatile wcl_core_1_statustable12_reg_t core_1_statustable12;
    volatile wcl_core_1_statustable13_reg_t core_1_statustable13;
    uint32_t reserved_4b4[18];
    volatile wcl_core_1_statustable_current_reg_t core_1_statustable_current;
    volatile wcl_core_1_message_addr_reg_t core_1_message_addr;
    volatile wcl_core_1_message_max_reg_t core_1_message_max;
    volatile wcl_core_1_message_phase_reg_t core_1_message_phase;
    uint32_t reserved_50c[13];
    volatile wcl_core_1_world_trigger_addr_reg_t core_1_world_trigger_addr;
    volatile wcl_core_1_world_prepare_reg_t core_1_world_prepare;
    volatile wcl_core_1_world_update_reg_t core_1_world_update;
    volatile wcl_core_1_world_cancel_reg_t core_1_world_cancel;
    volatile wcl_core_1_world_iram0_reg_t core_1_world_iram0;
    volatile wcl_core_1_world_dram0_pif_reg_t core_1_world_dram0_pif;
    volatile wcl_core_1_world_phase_reg_t core_1_world_phase;
    uint32_t reserved_55c[9];
    volatile wcl_core_1_nmi_mask_enable_reg_t core_1_nmi_mask_enable;
    volatile wcl_core_1_nmi_mask_trigger_addr_reg_t core_1_nmi_mask_trigger_addr;
    volatile wcl_core_1_nmi_mask_disable_reg_t core_1_nmi_mask_disable;
    volatile wcl_core_1_nmi_mask_cancle_reg_t core_1_nmi_mask_cancle;
    volatile wcl_core_1_nmi_mask_reg_t core_1_nmi_mask;
    volatile wcl_core_1_nmi_mask_phase_reg_t core_1_nmi_mask_phase;
} wcl_dev_t;

extern wcl_dev_t WORLD_CONTROLLER;

#ifndef __cplusplus
_Static_assert(sizeof(wcl_dev_t) == 0x598, "Invalid size of wcl_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
