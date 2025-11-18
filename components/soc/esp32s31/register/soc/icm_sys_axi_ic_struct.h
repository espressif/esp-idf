/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: date */
/** Type of date register
 *  Version Control Register
 */
typedef union {
    struct {
        /** reg_date : R/W; bitpos: [31:0]; default: 23050900;
         *  Version Control Register
         */
        uint32_t reg_date:32;
    };
    uint32_t val;
} axi_ic_date_reg_t;


/** Group: clk_en */
/** Type of clk_en register
 *  AXI IC reg clk enable config register
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 0;
         *  value 1 enable clk, value 0 disable clk, default 1
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} axi_ic_clk_en_reg_t;


/** Group: interrupt */
/** Type of interrupt_raw register
 *  Interrupt raw of AXI IC  Interrupt register.
 */
typedef union {
    struct {
        /** reg_dec_failure_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  Interrupt raw of reg_dec_failure_int_raw
         */
        uint32_t reg_dec_failure_int_raw:1;
        /** reg_timeout_int_raw : R/SS/WTC; bitpos: [1]; default: 0;
         *  Interrupt raw of reg_timeout_int_raw
         */
        uint32_t reg_timeout_int_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_interrupt_raw_reg_t;

/** Type of interrupt_clr register
 *  Interrupt clear of AXI IC  Interrupt register.
 */
typedef union {
    struct {
        /** reg_dec_failure_int_clr : WT; bitpos: [0]; default: 0;
         *  Interrupt clr of reg_dec_failure_int_clr
         */
        uint32_t reg_dec_failure_int_clr:1;
        /** reg_timeout_int_clr : WT; bitpos: [1]; default: 0;
         *  Interrupt clr of reg_timeout_int_clr
         */
        uint32_t reg_timeout_int_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_interrupt_clr_reg_t;

/** Type of interrupt_ena register
 *  Interrupt enable of AXI IC  Interrupt register.
 */
typedef union {
    struct {
        /** reg_dec_failure_int_ena : R/W; bitpos: [0]; default: 0;
         *  Interrupt ena of reg_dec_failure_int_ena
         */
        uint32_t reg_dec_failure_int_ena:1;
        /** reg_timeout_int_ena : R/W; bitpos: [1]; default: 0;
         *  Interrupt ena of reg_timeout_int_ena
         */
        uint32_t reg_timeout_int_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_interrupt_ena_reg_t;

/** Type of interrupt_st register
 *  AXI IC  Interrupt register.
 */
typedef union {
    struct {
        /** reg_dec_failure_int_st : RO; bitpos: [0]; default: 0;
         *  Interrupt st of reg_dec_failure_int_st
         */
        uint32_t reg_dec_failure_int_st:1;
        /** reg_timeout_int_st : RO; bitpos: [1]; default: 0;
         *  Interrupt st of reg_timeout_int_st
         */
        uint32_t reg_timeout_int_st:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_interrupt_st_reg_t;


/** Group: timeout_max_cnt */
/** Type of timeout_max_cnt register
 *  AXI IC core AW or AR channel timeout counter maximum value.
 */
typedef union {
    struct {
        /** reg_timeout_max_cnt : R/W; bitpos: [31:0]; default: 4294967295;
         *  AXI IC core AW or AR channel timeout counter maximum value.
         */
        uint32_t reg_timeout_max_cnt:32;
    };
    uint32_t val;
} axi_ic_timeout_max_cnt_reg_t;


/** Group: timeout_info */
/** Type of timeout_info register
 *  AXI IC core AW or AR channel timeout information.
 */
typedef union {
    struct {
        /** reg_timeout_slave : RO; bitpos: [2:0]; default: 0;
         *  bit[i] is 1 means slave[i] occurs timeout.
         */
        uint32_t reg_timeout_slave:3;
        /** reg_timeout_id : RO; bitpos: [9:3]; default: 0;
         *  (master id, awid)
         */
        uint32_t reg_timeout_id:7;
        /** reg_timeout_wr : RO; bitpos: [10]; default: 0;
         *  1 indicates write operation, 0 indicates read operation.
         */
        uint32_t reg_timeout_wr:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} axi_ic_timeout_info_reg_t;


/** Group: dec_failure_info */
/** Type of dec_failure_info register
 *  Default slave information get control and status.
 */
typedef union {
    struct {
        /** reg_dec_failure_read_first : R/W; bitpos: [0]; default: 0;
         *  Decoder failure recorded control. 1 means record read failure first, else write
         *  failure first.
         */
        uint32_t reg_dec_failure_read_first:1;
        /** reg_dec_failure_valid : RO; bitpos: [1]; default: 0;
         *  Decoder failure valid control.
         */
        uint32_t reg_dec_failure_valid:1;
        /** reg_dec_failure_type : RO; bitpos: [2]; default: 0;
         *  Decoder failure type control. 0 means ar failure, 1 means aw failure.
         */
        uint32_t reg_dec_failure_type:1;
        /** reg_dec_failure_cause : RO; bitpos: [5:3]; default: 0;
         *  bit[0] is 0 means to access the mapped slave address, and is 1 means to access the
         *  unmapped slave address. bit[1] is 0 means to access the visible slave, and is 1
         *  means to access the invisible slave. bit[2] is 0 means to access the secure slave,
         *  and is 1 means to access the non-secure slave.
         */
        uint32_t reg_dec_failure_cause:3;
        /** reg_dec_failure_mst_id : RO; bitpos: [12:6]; default: 0;
         *  (master id, awid/arid)
         */
        uint32_t reg_dec_failure_mst_id:7;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_dec_failure_info_reg_t;


/** Group: qos_debug_mst_sel */
/** Type of qos_debug_mst_sel register
 *  Config the select for debug master.
 */
typedef union {
    struct {
        /** reg_qos_debug_mst_sel : R/W; bitpos: [3:0]; default: 0;
         *  select which master will be debug.
         */
        uint32_t reg_qos_debug_mst_sel:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_qos_debug_mst_sel_reg_t;


/** Group: dec_failure_addr */
/** Type of dec_failure_addr register
 *  Record the address for AXI Master access Default slave.
 */
typedef union {
    struct {
        /** reg_dec_failure_addr : RO; bitpos: [31:0]; default: 0;
         *  AXI IC core AW or AR channel decoder failure address.
         */
        uint32_t reg_dec_failure_addr:32;
    };
    uint32_t val;
} axi_ic_dec_failure_addr_reg_t;


/** Group: reg_slave_is_secure */
/** Type of reg_slave_is_secure register
 *  AXI IC core slave security control.
 */
typedef union {
    struct {
        /** reg_slave_is_secure_0 : R/W; bitpos: [0]; default: 0;
         *  1 means safe and 0 means not safe.
         */
        uint32_t reg_slave_is_secure_0:1;
        /** reg_slave_is_secure_1 : R/W; bitpos: [1]; default: 0;
         *  1 means safe and 0 means not safe.
         */
        uint32_t reg_slave_is_secure_1:1;
        /** reg_slave_is_secure_2 : R/W; bitpos: [2]; default: 0;
         *  1 means safe and 0 means not safe.
         */
        uint32_t reg_slave_is_secure_2:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} axi_ic_reg_slave_is_secure_reg_t;


/** Group: slave_security_override */
/** Type of slave_security_override register
 *  The control signal whether slave security is overridden by software.
 */
typedef union {
    struct {
        /** reg_slave_security_override_0 : R/W; bitpos: [0]; default: 0;
         *  0 means not overridden, and 1 means overridden.
         */
        uint32_t reg_slave_security_override_0:1;
        /** reg_slave_security_override_1 : R/W; bitpos: [1]; default: 0;
         *  0 means not overridden, and 1 means overridden.
         */
        uint32_t reg_slave_security_override_1:1;
        /** reg_slave_security_override_2 : R/W; bitpos: [2]; default: 0;
         *  0 means not overridden, and 1 means overridden.
         */
        uint32_t reg_slave_security_override_2:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} axi_ic_slave_security_override_reg_t;


/** Group: master_is_secure */
/** Type of master_is_secure register
 *  AXI IC core master security control.
 */
typedef union {
    struct {
        /** reg_master_is_secure_0 : R/W; bitpos: [0]; default: 0;
         *  1 means safe and 0 means not safe.
         */
        uint32_t reg_master_is_secure_0:1;
        /** reg_master_is_secure_1 : R/W; bitpos: [1]; default: 0;
         *  1 means safe and 0 means not safe.
         */
        uint32_t reg_master_is_secure_1:1;
        /** reg_master_is_secure_2 : R/W; bitpos: [2]; default: 0;
         *  1 means safe and 0 means not safe.
         */
        uint32_t reg_master_is_secure_2:1;
        /** reg_master_is_secure_3 : R/W; bitpos: [3]; default: 0;
         *  1 means safe and 0 means not safe.
         */
        uint32_t reg_master_is_secure_3:1;
        /** reg_master_is_secure_4 : R/W; bitpos: [4]; default: 0;
         *  1 means safe and 0 means not safe.
         */
        uint32_t reg_master_is_secure_4:1;
        /** reg_master_is_secure_5 : R/W; bitpos: [5]; default: 0;
         *  1 means safe and 0 means not safe.
         */
        uint32_t reg_master_is_secure_5:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} axi_ic_master_is_secure_reg_t;


/** Group: master_security_override */
/** Type of master_security_override register
 *  The control signal whether master security is overridden by software.
 */
typedef union {
    struct {
        /** reg_master_security_override_0 : R/W; bitpos: [0]; default: 0;
         *  0 means not overridden, and 1 means overridden.
         */
        uint32_t reg_master_security_override_0:1;
        /** reg_master_security_override_1 : R/W; bitpos: [1]; default: 0;
         *  0 means not overridden, and 1 means overridden.
         */
        uint32_t reg_master_security_override_1:1;
        /** reg_master_security_override_2 : R/W; bitpos: [2]; default: 0;
         *  0 means not overridden, and 1 means overridden.
         */
        uint32_t reg_master_security_override_2:1;
        /** reg_master_security_override_3 : R/W; bitpos: [3]; default: 0;
         *  0 means not overridden, and 1 means overridden.
         */
        uint32_t reg_master_security_override_3:1;
        /** reg_master_security_override_4 : R/W; bitpos: [4]; default: 0;
         *  0 means not overridden, and 1 means overridden.
         */
        uint32_t reg_master_security_override_4:1;
        /** reg_master_security_override_5 : R/W; bitpos: [5]; default: 0;
         *  0 means not overridden, and 1 means overridden.
         */
        uint32_t reg_master_security_override_5:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} axi_ic_master_security_override_reg_t;


/** Group: aw_qos_value */
/** Type of aw_qos_value_0 register
 *  AXI IC core AW QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_value_0 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AW QOS value specified by software.
         */
        uint32_t reg_aw_qos_value_0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_value_0_reg_t;

/** Type of aw_qos_value_1 register
 *  AXI IC core AW QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_value_1 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AW QOS value specified by software.
         */
        uint32_t reg_aw_qos_value_1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_value_1_reg_t;

/** Type of aw_qos_value_2 register
 *  AXI IC core AW QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_value_2 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AW QOS value specified by software.
         */
        uint32_t reg_aw_qos_value_2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_value_2_reg_t;

/** Type of aw_qos_value_3 register
 *  AXI IC core AW QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_value_3 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AW QOS value specified by software.
         */
        uint32_t reg_aw_qos_value_3:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_value_3_reg_t;

/** Type of aw_qos_value_4 register
 *  AXI IC core AW QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_value_4 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AW QOS value specified by software.
         */
        uint32_t reg_aw_qos_value_4:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_value_4_reg_t;

/** Type of aw_qos_value_5 register
 *  AXI IC core AW QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_value_5 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AW QOS value specified by software.
         */
        uint32_t reg_aw_qos_value_5:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_value_5_reg_t;


/** Group: ar_qos_value */
/** Type of ar_qos_value_0 register
 *  AXI IC core AR QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_value_0 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AR QOS value specified by software.
         */
        uint32_t reg_ar_qos_value_0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_value_0_reg_t;

/** Type of ar_qos_value_1 register
 *  AXI IC core AR QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_value_1 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AR QOS value specified by software.
         */
        uint32_t reg_ar_qos_value_1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_value_1_reg_t;

/** Type of ar_qos_value_2 register
 *  AXI IC core AR QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_value_2 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AR QOS value specified by software.
         */
        uint32_t reg_ar_qos_value_2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_value_2_reg_t;

/** Type of ar_qos_value_3 register
 *  AXI IC core AR QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_value_3 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AR QOS value specified by software.
         */
        uint32_t reg_ar_qos_value_3:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_value_3_reg_t;

/** Type of ar_qos_value_4 register
 *  AXI IC core AR QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_value_4 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AR QOS value specified by software.
         */
        uint32_t reg_ar_qos_value_4:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_value_4_reg_t;

/** Type of ar_qos_value_5 register
 *  AXI IC core AR QOS value specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_value_5 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core AR QOS value specified by software.
         */
        uint32_t reg_ar_qos_value_5:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_value_5_reg_t;


/** Group: b_prior_value */
/** Type of b_prior_value_0 register
 *  AXI IC core B priority value specified by software.
 */
typedef union {
    struct {
        /** reg_b_prior_value_0 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core B priority value specified by software.
         */
        uint32_t reg_b_prior_value_0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_b_prior_value_0_reg_t;

/** Type of b_prior_value_1 register
 *  AXI IC core B priority value specified by software.
 */
typedef union {
    struct {
        /** reg_b_prior_value_1 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core B priority value specified by software.
         */
        uint32_t reg_b_prior_value_1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_b_prior_value_1_reg_t;

/** Type of b_prior_value_2 register
 *  AXI IC core B priority value specified by software.
 */
typedef union {
    struct {
        /** reg_b_prior_value_2 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core B priority value specified by software.
         */
        uint32_t reg_b_prior_value_2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_b_prior_value_2_reg_t;


/** Group: r_prior_value */
/** Type of r_prior_value_0 register
 *  AXI IC core R priority value specified by software.
 */
typedef union {
    struct {
        /** reg_r_prior_value_0 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core R priority value specified by software.
         */
        uint32_t reg_r_prior_value_0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_r_prior_value_0_reg_t;

/** Type of r_prior_value_1 register
 *  AXI IC core R priority value specified by software.
 */
typedef union {
    struct {
        /** reg_r_prior_value_1 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core R priority value specified by software.
         */
        uint32_t reg_r_prior_value_1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_r_prior_value_1_reg_t;

/** Type of r_prior_value_2 register
 *  AXI IC core R priority value specified by software.
 */
typedef union {
    struct {
        /** reg_r_prior_value_2 : R/W; bitpos: [3:0]; default: 0;
         *  AXI IC core R priority value specified by software.
         */
        uint32_t reg_r_prior_value_2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_r_prior_value_2_reg_t;


/** Group: aw_qos_sel */
/** Type of aw_qos_sel_0 register
 *  AXI IC core AW QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_sel_0 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_aw_qos_sel_0:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_aw_qos_sel_0_reg_t;

/** Type of aw_qos_sel_1 register
 *  AXI IC core AW QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_sel_1 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_aw_qos_sel_1:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_aw_qos_sel_1_reg_t;

/** Type of aw_qos_sel_2 register
 *  AXI IC core AW QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_sel_2 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_aw_qos_sel_2:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_aw_qos_sel_2_reg_t;

/** Type of aw_qos_sel_3 register
 *  AXI IC core AW QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_sel_3 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_aw_qos_sel_3:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_aw_qos_sel_3_reg_t;

/** Type of aw_qos_sel_4 register
 *  AXI IC core AW QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_sel_4 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_aw_qos_sel_4:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_aw_qos_sel_4_reg_t;

/** Type of aw_qos_sel_5 register
 *  AXI IC core AW QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_sel_5 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_aw_qos_sel_5:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_aw_qos_sel_5_reg_t;


/** Group: ar_qos_sel */
/** Type of ar_qos_sel_0 register
 *  AXI IC core AR QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_sel_0 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_ar_qos_sel_0:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_ar_qos_sel_0_reg_t;

/** Type of ar_qos_sel_1 register
 *  AXI IC core AR QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_sel_1 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_ar_qos_sel_1:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_ar_qos_sel_1_reg_t;

/** Type of ar_qos_sel_2 register
 *  AXI IC core AR QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_sel_2 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_ar_qos_sel_2:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_ar_qos_sel_2_reg_t;

/** Type of ar_qos_sel_3 register
 *  AXI IC core AR QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_sel_3 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_ar_qos_sel_3:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_ar_qos_sel_3_reg_t;

/** Type of ar_qos_sel_4 register
 *  AXI IC core AR QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_sel_4 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_ar_qos_sel_4:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_ar_qos_sel_4_reg_t;

/** Type of ar_qos_sel_5 register
 *  AXI IC core AR QOS select signal specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_sel_5 : R/W; bitpos: [1:0]; default: 0;
         *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
         *  software, 2'b10 selects the QOS value set by the QOS regulator.
         */
        uint32_t reg_ar_qos_sel_5:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} axi_ic_ar_qos_sel_5_reg_t;


/** Group: aw_qos_recovery */
/** Type of aw_qos_recovery register
 *  AXI IC core AW QOS recovery signal specified by software.
 */
typedef union {
    struct {
        /** reg_aw_qos_recovery_0 : R/W; bitpos: [0]; default: 0;
         *  Use QOS_COPY in SI_IF AW payload when it is asserted, otherwise use QOS_NEW (from
         *  QOS regulator or software).
         */
        uint32_t reg_aw_qos_recovery_0:1;
        /** reg_aw_qos_recovery_1 : R/W; bitpos: [1]; default: 0;
         *  Use QOS_COPY in SI_IF AW payload when it is asserted, otherwise use QOS_NEW (from
         *  QOS regulator or software).
         */
        uint32_t reg_aw_qos_recovery_1:1;
        /** reg_aw_qos_recovery_2 : R/W; bitpos: [2]; default: 0;
         *  Use QOS_COPY in SI_IF AW payload when it is asserted, otherwise use QOS_NEW (from
         *  QOS regulator or software).
         */
        uint32_t reg_aw_qos_recovery_2:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} axi_ic_aw_qos_recovery_reg_t;


/** Group: ar_qos_recovery */
/** Type of ar_qos_recovery register
 *  AXI IC core AR QOS recovery signal specified by software.
 */
typedef union {
    struct {
        /** reg_ar_qos_recovery_0 : R/W; bitpos: [0]; default: 0;
         *  Use QOS_COPY in SI_IF AR payload when it is asserted, otherwise use QOS_NEW (from
         *  QOS regulator or software).
         */
        uint32_t reg_ar_qos_recovery_0:1;
        /** reg_ar_qos_recovery_1 : R/W; bitpos: [1]; default: 0;
         *  Use QOS_COPY in SI_IF AR payload when it is asserted, otherwise use QOS_NEW (from
         *  QOS regulator or software).
         */
        uint32_t reg_ar_qos_recovery_1:1;
        /** reg_ar_qos_recovery_2 : R/W; bitpos: [2]; default: 0;
         *  Use QOS_COPY in SI_IF AR payload when it is asserted, otherwise use QOS_NEW (from
         *  QOS regulator or software).
         */
        uint32_t reg_ar_qos_recovery_2:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} axi_ic_ar_qos_recovery_reg_t;


/** Group: aw_qos_id_mask */
/** Type of aw_qos_id_mask_0 register
 *  Config the aw channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_mask_0 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_mask_0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_mask_0_reg_t;

/** Type of aw_qos_id_mask_1 register
 *  Config the aw channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_mask_1 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_mask_1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_mask_1_reg_t;

/** Type of aw_qos_id_mask_2 register
 *  Config the aw channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_mask_2 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_mask_2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_mask_2_reg_t;

/** Type of aw_qos_id_mask_3 register
 *  Config the aw channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_mask_3 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_mask_3:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_mask_3_reg_t;

/** Type of aw_qos_id_mask_4 register
 *  Config the aw channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_mask_4 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_mask_4:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_mask_4_reg_t;

/** Type of aw_qos_id_mask_5 register
 *  Config the aw channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_mask_5 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_mask_5:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_mask_5_reg_t;


/** Group: ar_qos_id_mask */
/** Type of ar_qos_id_mask_0 register
 *  Config the ar channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_mask_0 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_mask_0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_mask_0_reg_t;

/** Type of ar_qos_id_mask_1 register
 *  Config the ar channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_mask_1 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_mask_1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_mask_1_reg_t;

/** Type of ar_qos_id_mask_2 register
 *  Config the ar channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_mask_2 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_mask_2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_mask_2_reg_t;

/** Type of ar_qos_id_mask_3 register
 *  Config the ar channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_mask_3 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_mask_3:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_mask_3_reg_t;

/** Type of ar_qos_id_mask_4 register
 *  Config the ar channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_mask_4 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_mask_4:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_mask_4_reg_t;

/** Type of ar_qos_id_mask_5 register
 *  Config the ar channel id mask for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_mask_5 : R/W; bitpos: [3:0]; default: 15;
         *  mask for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_mask_5:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_mask_5_reg_t;


/** Group: aw_qos_id_filter */
/** Type of aw_qos_id_filter_0 register
 *  Config the aw channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_filter_0 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_filter_0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_filter_0_reg_t;

/** Type of aw_qos_id_filter_1 register
 *  Config the aw channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_filter_1 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_filter_1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_filter_1_reg_t;

/** Type of aw_qos_id_filter_2 register
 *  Config the aw channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_filter_2 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_filter_2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_filter_2_reg_t;

/** Type of aw_qos_id_filter_3 register
 *  Config the aw channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_filter_3 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_filter_3:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_filter_3_reg_t;

/** Type of aw_qos_id_filter_4 register
 *  Config the aw channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_filter_4 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_filter_4:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_filter_4_reg_t;

/** Type of aw_qos_id_filter_5 register
 *  Config the aw channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_aw_qos_id_filter_5 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of aw channel id.
         */
        uint32_t reg_aw_qos_id_filter_5:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_aw_qos_id_filter_5_reg_t;


/** Group: ar_qos_id_filter */
/** Type of ar_qos_id_filter_0 register
 *  Config the ar channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_filter_0 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_filter_0:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_filter_0_reg_t;

/** Type of ar_qos_id_filter_1 register
 *  Config the ar channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_filter_1 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_filter_1:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_filter_1_reg_t;

/** Type of ar_qos_id_filter_2 register
 *  Config the ar channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_filter_2 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_filter_2:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_filter_2_reg_t;

/** Type of ar_qos_id_filter_3 register
 *  Config the ar channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_filter_3 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_filter_3:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_filter_3_reg_t;

/** Type of ar_qos_id_filter_4 register
 *  Config the ar channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_filter_4 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_filter_4:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_filter_4_reg_t;

/** Type of ar_qos_id_filter_5 register
 *  Config the ar channel id filter for id filter function.
 */
typedef union {
    struct {
        /** reg_ar_qos_id_filter_5 : R/W; bitpos: [3:0]; default: 15;
         *  filter for enable qos of ar channel id.
         */
        uint32_t reg_ar_qos_id_filter_5:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} axi_ic_ar_qos_id_filter_5_reg_t;


/** Group: aw_qos_ctrl_cfg */
/** Type of aw_qos_ctrl_cfg_0 register
 *  Config the QOS Regulator aw module control.
 */
typedef union {
    struct {
        /** reg_aw_qos_ctrl_cfg_0 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_aw_qos_ctrl_cfg_0:32;
    };
    uint32_t val;
} axi_ic_aw_qos_ctrl_cfg_0_reg_t;

/** Type of aw_qos_ctrl_cfg_1 register
 *  Config the QOS Regulator aw module control.
 */
typedef union {
    struct {
        /** reg_aw_qos_ctrl_cfg_1 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_aw_qos_ctrl_cfg_1:32;
    };
    uint32_t val;
} axi_ic_aw_qos_ctrl_cfg_1_reg_t;

/** Type of aw_qos_ctrl_cfg_2 register
 *  Config the QOS Regulator aw module control.
 */
typedef union {
    struct {
        /** reg_aw_qos_ctrl_cfg_2 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_aw_qos_ctrl_cfg_2:32;
    };
    uint32_t val;
} axi_ic_aw_qos_ctrl_cfg_2_reg_t;

/** Type of aw_qos_ctrl_cfg_3 register
 *  Config the QOS Regulator aw module control.
 */
typedef union {
    struct {
        /** reg_aw_qos_ctrl_cfg_3 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_aw_qos_ctrl_cfg_3:32;
    };
    uint32_t val;
} axi_ic_aw_qos_ctrl_cfg_3_reg_t;

/** Type of aw_qos_ctrl_cfg_4 register
 *  Config the QOS Regulator aw module control.
 */
typedef union {
    struct {
        /** reg_aw_qos_ctrl_cfg_4 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_aw_qos_ctrl_cfg_4:32;
    };
    uint32_t val;
} axi_ic_aw_qos_ctrl_cfg_4_reg_t;

/** Type of aw_qos_ctrl_cfg_5 register
 *  Config the QOS Regulator aw module control.
 */
typedef union {
    struct {
        /** reg_aw_qos_ctrl_cfg_5 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_aw_qos_ctrl_cfg_5:32;
    };
    uint32_t val;
} axi_ic_aw_qos_ctrl_cfg_5_reg_t;


/** Group: ar_qos_ctrl_cfg */
/** Type of ar_qos_ctrl_cfg_0 register
 *  Config the QOS Regulator ar module control.
 */
typedef union {
    struct {
        /** reg_ar_qos_ctrl_cfg_0 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_ar_qos_ctrl_cfg_0:32;
    };
    uint32_t val;
} axi_ic_ar_qos_ctrl_cfg_0_reg_t;

/** Type of ar_qos_ctrl_cfg_1 register
 *  Config the QOS Regulator ar module control.
 */
typedef union {
    struct {
        /** reg_ar_qos_ctrl_cfg_1 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_ar_qos_ctrl_cfg_1:32;
    };
    uint32_t val;
} axi_ic_ar_qos_ctrl_cfg_1_reg_t;

/** Type of ar_qos_ctrl_cfg_2 register
 *  Config the QOS Regulator ar module control.
 */
typedef union {
    struct {
        /** reg_ar_qos_ctrl_cfg_2 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_ar_qos_ctrl_cfg_2:32;
    };
    uint32_t val;
} axi_ic_ar_qos_ctrl_cfg_2_reg_t;

/** Type of ar_qos_ctrl_cfg_3 register
 *  Config the QOS Regulator ar module control.
 */
typedef union {
    struct {
        /** reg_ar_qos_ctrl_cfg_3 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_ar_qos_ctrl_cfg_3:32;
    };
    uint32_t val;
} axi_ic_ar_qos_ctrl_cfg_3_reg_t;

/** Type of ar_qos_ctrl_cfg_4 register
 *  Config the QOS Regulator ar module control.
 */
typedef union {
    struct {
        /** reg_ar_qos_ctrl_cfg_4 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_ar_qos_ctrl_cfg_4:32;
    };
    uint32_t val;
} axi_ic_ar_qos_ctrl_cfg_4_reg_t;

/** Type of ar_qos_ctrl_cfg_5 register
 *  Config the QOS Regulator ar module control.
 */
typedef union {
    struct {
        /** reg_ar_qos_ctrl_cfg_5 : R/W; bitpos: [31:0]; default: 8208;
         *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
         *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
         */
        uint32_t reg_ar_qos_ctrl_cfg_5:32;
    };
    uint32_t val;
} axi_ic_ar_qos_ctrl_cfg_5_reg_t;


/** Group: aw_qos_trans_rate */
/** Type of aw_qos_trans_rate_0 register
 *  Config the Current Master Write bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_trans_rate_0 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_aw_qos_trans_rate_0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_trans_rate_0_reg_t;

/** Type of aw_qos_trans_rate_1 register
 *  Config the Current Master Write bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_trans_rate_1 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_aw_qos_trans_rate_1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_trans_rate_1_reg_t;

/** Type of aw_qos_trans_rate_2 register
 *  Config the Current Master Write bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_trans_rate_2 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_aw_qos_trans_rate_2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_trans_rate_2_reg_t;

/** Type of aw_qos_trans_rate_3 register
 *  Config the Current Master Write bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_trans_rate_3 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_aw_qos_trans_rate_3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_trans_rate_3_reg_t;

/** Type of aw_qos_trans_rate_4 register
 *  Config the Current Master Write bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_trans_rate_4 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_aw_qos_trans_rate_4:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_trans_rate_4_reg_t;

/** Type of aw_qos_trans_rate_5 register
 *  Config the Current Master Write bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_trans_rate_5 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_aw_qos_trans_rate_5:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_trans_rate_5_reg_t;


/** Group: ar_qos_trans_rate */
/** Type of ar_qos_trans_rate_0 register
 *  Config the Current Master Read bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_trans_rate_0 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_ar_qos_trans_rate_0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_trans_rate_0_reg_t;

/** Type of ar_qos_trans_rate_1 register
 *  Config the Current Master Read bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_trans_rate_1 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_ar_qos_trans_rate_1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_trans_rate_1_reg_t;

/** Type of ar_qos_trans_rate_2 register
 *  Config the Current Master Read bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_trans_rate_2 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_ar_qos_trans_rate_2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_trans_rate_2_reg_t;

/** Type of ar_qos_trans_rate_3 register
 *  Config the Current Master Read bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_trans_rate_3 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_ar_qos_trans_rate_3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_trans_rate_3_reg_t;

/** Type of ar_qos_trans_rate_4 register
 *  Config the Current Master Read bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_trans_rate_4 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_ar_qos_trans_rate_4:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_trans_rate_4_reg_t;

/** Type of ar_qos_trans_rate_5 register
 *  Config the Current Master Read bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_trans_rate_5 : R/W; bitpos: [15:0]; default: 12296;
         *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
         */
        uint32_t reg_ar_qos_trans_rate_5:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_trans_rate_5_reg_t;


/** Group: aw_qos_peak_rate */
/** Type of aw_qos_peak_rate_0 register
 *  Config the Current Master Write Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_peak_rate_0 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the aw channel request issue rate, default 0.
         */
        uint32_t reg_aw_qos_peak_rate_0:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_peak_rate_0_reg_t;

/** Type of aw_qos_peak_rate_1 register
 *  Config the Current Master Write Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_peak_rate_1 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the aw channel request issue rate, default 0.
         */
        uint32_t reg_aw_qos_peak_rate_1:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_peak_rate_1_reg_t;

/** Type of aw_qos_peak_rate_2 register
 *  Config the Current Master Write Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_peak_rate_2 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the aw channel request issue rate, default 0.
         */
        uint32_t reg_aw_qos_peak_rate_2:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_peak_rate_2_reg_t;

/** Type of aw_qos_peak_rate_3 register
 *  Config the Current Master Write Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_peak_rate_3 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the aw channel request issue rate, default 0.
         */
        uint32_t reg_aw_qos_peak_rate_3:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_peak_rate_3_reg_t;

/** Type of aw_qos_peak_rate_4 register
 *  Config the Current Master Write Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_peak_rate_4 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the aw channel request issue rate, default 0.
         */
        uint32_t reg_aw_qos_peak_rate_4:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_peak_rate_4_reg_t;

/** Type of aw_qos_peak_rate_5 register
 *  Config the Current Master Write Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_aw_qos_peak_rate_5 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the aw channel request issue rate, default 0.
         */
        uint32_t reg_aw_qos_peak_rate_5:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_peak_rate_5_reg_t;


/** Group: ar_qos_peak_rate */
/** Type of ar_qos_peak_rate_0 register
 *  Config the Current Master Read Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_peak_rate_0 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the ar channel request issue rate, default 0.
         */
        uint32_t reg_ar_qos_peak_rate_0:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_peak_rate_0_reg_t;

/** Type of ar_qos_peak_rate_1 register
 *  Config the Current Master Read Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_peak_rate_1 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the ar channel request issue rate, default 0.
         */
        uint32_t reg_ar_qos_peak_rate_1:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_peak_rate_1_reg_t;

/** Type of ar_qos_peak_rate_2 register
 *  Config the Current Master Read Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_peak_rate_2 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the ar channel request issue rate, default 0.
         */
        uint32_t reg_ar_qos_peak_rate_2:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_peak_rate_2_reg_t;

/** Type of ar_qos_peak_rate_3 register
 *  Config the Current Master Read Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_peak_rate_3 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the ar channel request issue rate, default 0.
         */
        uint32_t reg_ar_qos_peak_rate_3:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_peak_rate_3_reg_t;

/** Type of ar_qos_peak_rate_4 register
 *  Config the Current Master Read Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_peak_rate_4 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the ar channel request issue rate, default 0.
         */
        uint32_t reg_ar_qos_peak_rate_4:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_peak_rate_4_reg_t;

/** Type of ar_qos_peak_rate_5 register
 *  Config the Current Master Read Peak bandwidth.
 */
typedef union {
    struct {
        /** reg_ar_qos_peak_rate_5 : R/W; bitpos: [7:0]; default: 0;
         *  peak rate, to control the ar channel request issue rate, default 0.
         */
        uint32_t reg_ar_qos_peak_rate_5:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_peak_rate_5_reg_t;


/** Group: aw_qos_burstiness */
/** Type of aw_qos_burstiness_0 register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_aw_qos_burstiness_0 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the aw channel token, default 128 tokens.
         */
        uint32_t reg_aw_qos_burstiness_0:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_aw_qos_burstiness_0_reg_t;

/** Type of aw_qos_burstiness_1 register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_aw_qos_burstiness_1 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the aw channel token, default 128 tokens.
         */
        uint32_t reg_aw_qos_burstiness_1:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_aw_qos_burstiness_1_reg_t;

/** Type of aw_qos_burstiness_2 register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_aw_qos_burstiness_2 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the aw channel token, default 128 tokens.
         */
        uint32_t reg_aw_qos_burstiness_2:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_aw_qos_burstiness_2_reg_t;

/** Type of aw_qos_burstiness_3 register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_aw_qos_burstiness_3 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the aw channel token, default 128 tokens.
         */
        uint32_t reg_aw_qos_burstiness_3:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_aw_qos_burstiness_3_reg_t;

/** Type of aw_qos_burstiness_4 register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_aw_qos_burstiness_4 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the aw channel token, default 128 tokens.
         */
        uint32_t reg_aw_qos_burstiness_4:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_aw_qos_burstiness_4_reg_t;

/** Type of aw_qos_burstiness_5 register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_aw_qos_burstiness_5 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the aw channel token, default 128 tokens.
         */
        uint32_t reg_aw_qos_burstiness_5:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_aw_qos_burstiness_5_reg_t;


/** Group: ar_qos_burstiness */
/** Type of ar_qos_burstiness_0 register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_ar_qos_burstiness_0 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the ar channel token, default 128 tokens.
         */
        uint32_t reg_ar_qos_burstiness_0:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_ar_qos_burstiness_0_reg_t;

/** Type of ar_qos_burstiness_1 register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_ar_qos_burstiness_1 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the ar channel token, default 128 tokens.
         */
        uint32_t reg_ar_qos_burstiness_1:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_ar_qos_burstiness_1_reg_t;

/** Type of ar_qos_burstiness_2 register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_ar_qos_burstiness_2 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the ar channel token, default 128 tokens.
         */
        uint32_t reg_ar_qos_burstiness_2:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_ar_qos_burstiness_2_reg_t;

/** Type of ar_qos_burstiness_3 register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_ar_qos_burstiness_3 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the ar channel token, default 128 tokens.
         */
        uint32_t reg_ar_qos_burstiness_3:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_ar_qos_burstiness_3_reg_t;

/** Type of ar_qos_burstiness_4 register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_ar_qos_burstiness_4 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the ar channel token, default 128 tokens.
         */
        uint32_t reg_ar_qos_burstiness_4:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_ar_qos_burstiness_4_reg_t;

/** Type of ar_qos_burstiness_5 register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
typedef union {
    struct {
        /** reg_ar_qos_burstiness_5 : R/W; bitpos: [11:0]; default: 128;
         *  the maximum numbers of the ar channel token, default 128 tokens.
         */
        uint32_t reg_ar_qos_burstiness_5:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} axi_ic_ar_qos_burstiness_5_reg_t;


/** Group: aw_qos_cfg_vlu */
/** Type of aw_qos_cfg_vlu_0 register
 *  Config the QOS Priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_cfg_vlu_0 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the aw channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_aw_qos_cfg_vlu_0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_cfg_vlu_0_reg_t;

/** Type of aw_qos_cfg_vlu_1 register
 *  Config the QOS Priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_cfg_vlu_1 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the aw channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_aw_qos_cfg_vlu_1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_cfg_vlu_1_reg_t;

/** Type of aw_qos_cfg_vlu_2 register
 *  Config the QOS Priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_cfg_vlu_2 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the aw channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_aw_qos_cfg_vlu_2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_cfg_vlu_2_reg_t;

/** Type of aw_qos_cfg_vlu_3 register
 *  Config the QOS Priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_cfg_vlu_3 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the aw channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_aw_qos_cfg_vlu_3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_cfg_vlu_3_reg_t;

/** Type of aw_qos_cfg_vlu_4 register
 *  Config the QOS Priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_cfg_vlu_4 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the aw channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_aw_qos_cfg_vlu_4:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_cfg_vlu_4_reg_t;

/** Type of aw_qos_cfg_vlu_5 register
 *  Config the QOS Priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_cfg_vlu_5 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the aw channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_aw_qos_cfg_vlu_5:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_aw_qos_cfg_vlu_5_reg_t;


/** Group: ar_qos_cfg_vlu */
/** Type of ar_qos_cfg_vlu_0 register
 *  Config the QOS Priority in AR channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_cfg_vlu_0 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the ar channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_ar_qos_cfg_vlu_0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_cfg_vlu_0_reg_t;

/** Type of ar_qos_cfg_vlu_1 register
 *  Config the QOS Priority in AR channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_cfg_vlu_1 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the ar channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_ar_qos_cfg_vlu_1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_cfg_vlu_1_reg_t;

/** Type of ar_qos_cfg_vlu_2 register
 *  Config the QOS Priority in AR channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_cfg_vlu_2 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the ar channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_ar_qos_cfg_vlu_2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_cfg_vlu_2_reg_t;

/** Type of ar_qos_cfg_vlu_3 register
 *  Config the QOS Priority in AR channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_cfg_vlu_3 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the ar channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_ar_qos_cfg_vlu_3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_cfg_vlu_3_reg_t;

/** Type of ar_qos_cfg_vlu_4 register
 *  Config the QOS Priority in AR channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_cfg_vlu_4 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the ar channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_ar_qos_cfg_vlu_4:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_cfg_vlu_4_reg_t;

/** Type of ar_qos_cfg_vlu_5 register
 *  Config the QOS Priority in AR channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_cfg_vlu_5 : R/W; bitpos: [15:0]; default: 55906;
         *  the four level of the ar channel qos priority, default is 2,6,10,13
         */
        uint32_t reg_ar_qos_cfg_vlu_5:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} axi_ic_ar_qos_cfg_vlu_5_reg_t;


/** Group: aw_qos_dly_thr0 */
/** Type of aw_qos_dly_thr0_0 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr0_0 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the aw channel delay threshold, default is 32.
         */
        uint32_t reg_aw_qos_dly_thr0_0:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr0_0_reg_t;

/** Type of aw_qos_dly_thr0_1 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr0_1 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the aw channel delay threshold, default is 32.
         */
        uint32_t reg_aw_qos_dly_thr0_1:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr0_1_reg_t;

/** Type of aw_qos_dly_thr0_2 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr0_2 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the aw channel delay threshold, default is 32.
         */
        uint32_t reg_aw_qos_dly_thr0_2:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr0_2_reg_t;

/** Type of aw_qos_dly_thr0_3 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr0_3 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the aw channel delay threshold, default is 32.
         */
        uint32_t reg_aw_qos_dly_thr0_3:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr0_3_reg_t;

/** Type of aw_qos_dly_thr0_4 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr0_4 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the aw channel delay threshold, default is 32.
         */
        uint32_t reg_aw_qos_dly_thr0_4:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr0_4_reg_t;

/** Type of aw_qos_dly_thr0_5 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr0_5 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the aw channel delay threshold, default is 32.
         */
        uint32_t reg_aw_qos_dly_thr0_5:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr0_5_reg_t;


/** Group: aw_qos_dly_thr1 */
/** Type of aw_qos_dly_thr1_0 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr1_0 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the aw channel delay threshold, default is 160.
         */
        uint32_t reg_aw_qos_dly_thr1_0:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr1_0_reg_t;

/** Type of aw_qos_dly_thr1_1 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr1_1 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the aw channel delay threshold, default is 160.
         */
        uint32_t reg_aw_qos_dly_thr1_1:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr1_1_reg_t;

/** Type of aw_qos_dly_thr1_2 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr1_2 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the aw channel delay threshold, default is 160.
         */
        uint32_t reg_aw_qos_dly_thr1_2:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr1_2_reg_t;

/** Type of aw_qos_dly_thr1_3 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr1_3 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the aw channel delay threshold, default is 160.
         */
        uint32_t reg_aw_qos_dly_thr1_3:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr1_3_reg_t;

/** Type of aw_qos_dly_thr1_4 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr1_4 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the aw channel delay threshold, default is 160.
         */
        uint32_t reg_aw_qos_dly_thr1_4:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr1_4_reg_t;

/** Type of aw_qos_dly_thr1_5 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr1_5 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the aw channel delay threshold, default is 160.
         */
        uint32_t reg_aw_qos_dly_thr1_5:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr1_5_reg_t;


/** Group: aw_qos_dly_thr2 */
/** Type of aw_qos_dly_thr2_0 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr2_0 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the aw channel delay threshold, default is 416.
         */
        uint32_t reg_aw_qos_dly_thr2_0:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr2_0_reg_t;

/** Type of aw_qos_dly_thr2_1 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr2_1 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the aw channel delay threshold, default is 416.
         */
        uint32_t reg_aw_qos_dly_thr2_1:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr2_1_reg_t;

/** Type of aw_qos_dly_thr2_2 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr2_2 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the aw channel delay threshold, default is 416.
         */
        uint32_t reg_aw_qos_dly_thr2_2:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr2_2_reg_t;

/** Type of aw_qos_dly_thr2_3 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr2_3 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the aw channel delay threshold, default is 416.
         */
        uint32_t reg_aw_qos_dly_thr2_3:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr2_3_reg_t;

/** Type of aw_qos_dly_thr2_4 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr2_4 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the aw channel delay threshold, default is 416.
         */
        uint32_t reg_aw_qos_dly_thr2_4:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr2_4_reg_t;

/** Type of aw_qos_dly_thr2_5 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr2_5 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the aw channel delay threshold, default is 416.
         */
        uint32_t reg_aw_qos_dly_thr2_5:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr2_5_reg_t;


/** Group: aw_qos_dly_thr3 */
/** Type of aw_qos_dly_thr3_0 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr3_0 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the aw channel delay threshold, default is 928.
         */
        uint32_t reg_aw_qos_dly_thr3_0:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr3_0_reg_t;

/** Type of aw_qos_dly_thr3_1 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr3_1 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the aw channel delay threshold, default is 928.
         */
        uint32_t reg_aw_qos_dly_thr3_1:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr3_1_reg_t;

/** Type of aw_qos_dly_thr3_2 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr3_2 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the aw channel delay threshold, default is 928.
         */
        uint32_t reg_aw_qos_dly_thr3_2:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr3_2_reg_t;

/** Type of aw_qos_dly_thr3_3 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr3_3 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the aw channel delay threshold, default is 928.
         */
        uint32_t reg_aw_qos_dly_thr3_3:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr3_3_reg_t;

/** Type of aw_qos_dly_thr3_4 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr3_4 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the aw channel delay threshold, default is 928.
         */
        uint32_t reg_aw_qos_dly_thr3_4:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr3_4_reg_t;

/** Type of aw_qos_dly_thr3_5 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dly_thr3_5 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the aw channel delay threshold, default is 928.
         */
        uint32_t reg_aw_qos_dly_thr3_5:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_aw_qos_dly_thr3_5_reg_t;


/** Group: ar_qos_dly_thr0 */
/** Type of ar_qos_dly_thr0_0 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr0_0 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the ar channel delay threshold, default is 32.
         */
        uint32_t reg_ar_qos_dly_thr0_0:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr0_0_reg_t;

/** Type of ar_qos_dly_thr0_1 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr0_1 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the ar channel delay threshold, default is 32.
         */
        uint32_t reg_ar_qos_dly_thr0_1:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr0_1_reg_t;

/** Type of ar_qos_dly_thr0_2 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr0_2 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the ar channel delay threshold, default is 32.
         */
        uint32_t reg_ar_qos_dly_thr0_2:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr0_2_reg_t;

/** Type of ar_qos_dly_thr0_3 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr0_3 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the ar channel delay threshold, default is 32.
         */
        uint32_t reg_ar_qos_dly_thr0_3:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr0_3_reg_t;

/** Type of ar_qos_dly_thr0_4 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr0_4 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the ar channel delay threshold, default is 32.
         */
        uint32_t reg_ar_qos_dly_thr0_4:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr0_4_reg_t;

/** Type of ar_qos_dly_thr0_5 register
 *  Config the first level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr0_5 : R/W; bitpos: [12:0]; default: 32;
         *  the first level of the ar channel delay threshold, default is 32.
         */
        uint32_t reg_ar_qos_dly_thr0_5:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr0_5_reg_t;


/** Group: ar_qos_dly_thr1 */
/** Type of ar_qos_dly_thr1_0 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr1_0 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the ar channel delay threshold, default is 160.
         */
        uint32_t reg_ar_qos_dly_thr1_0:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr1_0_reg_t;

/** Type of ar_qos_dly_thr1_1 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr1_1 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the ar channel delay threshold, default is 160.
         */
        uint32_t reg_ar_qos_dly_thr1_1:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr1_1_reg_t;

/** Type of ar_qos_dly_thr1_2 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr1_2 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the ar channel delay threshold, default is 160.
         */
        uint32_t reg_ar_qos_dly_thr1_2:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr1_2_reg_t;

/** Type of ar_qos_dly_thr1_3 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr1_3 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the ar channel delay threshold, default is 160.
         */
        uint32_t reg_ar_qos_dly_thr1_3:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr1_3_reg_t;

/** Type of ar_qos_dly_thr1_4 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr1_4 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the ar channel delay threshold, default is 160.
         */
        uint32_t reg_ar_qos_dly_thr1_4:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr1_4_reg_t;

/** Type of ar_qos_dly_thr1_5 register
 *  Config the second level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr1_5 : R/W; bitpos: [12:0]; default: 160;
         *  the second level of the ar channel delay threshold, default is 160.
         */
        uint32_t reg_ar_qos_dly_thr1_5:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr1_5_reg_t;


/** Group: ar_qos_dly_thr2 */
/** Type of ar_qos_dly_thr2_0 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr2_0 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the ar channel delay threshold, default is 416.
         */
        uint32_t reg_ar_qos_dly_thr2_0:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr2_0_reg_t;

/** Type of ar_qos_dly_thr2_1 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr2_1 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the ar channel delay threshold, default is 416.
         */
        uint32_t reg_ar_qos_dly_thr2_1:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr2_1_reg_t;

/** Type of ar_qos_dly_thr2_2 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr2_2 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the ar channel delay threshold, default is 416.
         */
        uint32_t reg_ar_qos_dly_thr2_2:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr2_2_reg_t;

/** Type of ar_qos_dly_thr2_3 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr2_3 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the ar channel delay threshold, default is 416.
         */
        uint32_t reg_ar_qos_dly_thr2_3:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr2_3_reg_t;

/** Type of ar_qos_dly_thr2_4 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr2_4 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the ar channel delay threshold, default is 416.
         */
        uint32_t reg_ar_qos_dly_thr2_4:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr2_4_reg_t;

/** Type of ar_qos_dly_thr2_5 register
 *  Config the third level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr2_5 : R/W; bitpos: [12:0]; default: 416;
         *  the third level of the ar channel delay threshold, default is 416.
         */
        uint32_t reg_ar_qos_dly_thr2_5:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr2_5_reg_t;


/** Group: ar_qos_dly_thr3 */
/** Type of ar_qos_dly_thr3_0 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr3_0 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the ar channel delay threshold, default is 928.
         */
        uint32_t reg_ar_qos_dly_thr3_0:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr3_0_reg_t;

/** Type of ar_qos_dly_thr3_1 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr3_1 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the ar channel delay threshold, default is 928.
         */
        uint32_t reg_ar_qos_dly_thr3_1:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr3_1_reg_t;

/** Type of ar_qos_dly_thr3_2 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr3_2 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the ar channel delay threshold, default is 928.
         */
        uint32_t reg_ar_qos_dly_thr3_2:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr3_2_reg_t;

/** Type of ar_qos_dly_thr3_3 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr3_3 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the ar channel delay threshold, default is 928.
         */
        uint32_t reg_ar_qos_dly_thr3_3:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr3_3_reg_t;

/** Type of ar_qos_dly_thr3_4 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr3_4 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the ar channel delay threshold, default is 928.
         */
        uint32_t reg_ar_qos_dly_thr3_4:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr3_4_reg_t;

/** Type of ar_qos_dly_thr3_5 register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dly_thr3_5 : R/W; bitpos: [12:0]; default: 928;
         *  the forth level of the ar channel delay threshold, default is 928.
         */
        uint32_t reg_ar_qos_dly_thr3_5:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} axi_ic_ar_qos_dly_thr3_5_reg_t;


/** Group: aw_qos_bur_len_thr */
/** Type of aw_qos_bur_len_thr_0 register
 *  Config the min value of token which can enable axi write transaction.
 */
typedef union {
    struct {
        /** reg_aw_qos_bur_len_thr_0 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of aw channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_aw_qos_bur_len_thr_0:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_aw_qos_bur_len_thr_0_reg_t;

/** Type of aw_qos_bur_len_thr_1 register
 *  Config the min value of token which can enable axi write transaction.
 */
typedef union {
    struct {
        /** reg_aw_qos_bur_len_thr_1 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of aw channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_aw_qos_bur_len_thr_1:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_aw_qos_bur_len_thr_1_reg_t;

/** Type of aw_qos_bur_len_thr_2 register
 *  Config the min value of token which can enable axi write transaction.
 */
typedef union {
    struct {
        /** reg_aw_qos_bur_len_thr_2 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of aw channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_aw_qos_bur_len_thr_2:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_aw_qos_bur_len_thr_2_reg_t;

/** Type of aw_qos_bur_len_thr_3 register
 *  Config the min value of token which can enable axi write transaction.
 */
typedef union {
    struct {
        /** reg_aw_qos_bur_len_thr_3 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of aw channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_aw_qos_bur_len_thr_3:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_aw_qos_bur_len_thr_3_reg_t;

/** Type of aw_qos_bur_len_thr_4 register
 *  Config the min value of token which can enable axi write transaction.
 */
typedef union {
    struct {
        /** reg_aw_qos_bur_len_thr_4 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of aw channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_aw_qos_bur_len_thr_4:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_aw_qos_bur_len_thr_4_reg_t;

/** Type of aw_qos_bur_len_thr_5 register
 *  Config the min value of token which can enable axi write transaction.
 */
typedef union {
    struct {
        /** reg_aw_qos_bur_len_thr_5 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of aw channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_aw_qos_bur_len_thr_5:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_aw_qos_bur_len_thr_5_reg_t;


/** Group: ar_qos_bur_len_thr */
/** Type of ar_qos_bur_len_thr_0 register
 *  Config the min value of token which can enable axi read transaction.
 */
typedef union {
    struct {
        /** reg_ar_qos_bur_len_thr_0 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of ar channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_ar_qos_bur_len_thr_0:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_ar_qos_bur_len_thr_0_reg_t;

/** Type of ar_qos_bur_len_thr_1 register
 *  Config the min value of token which can enable axi read transaction.
 */
typedef union {
    struct {
        /** reg_ar_qos_bur_len_thr_1 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of ar channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_ar_qos_bur_len_thr_1:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_ar_qos_bur_len_thr_1_reg_t;

/** Type of ar_qos_bur_len_thr_2 register
 *  Config the min value of token which can enable axi read transaction.
 */
typedef union {
    struct {
        /** reg_ar_qos_bur_len_thr_2 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of ar channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_ar_qos_bur_len_thr_2:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_ar_qos_bur_len_thr_2_reg_t;

/** Type of ar_qos_bur_len_thr_3 register
 *  Config the min value of token which can enable axi read transaction.
 */
typedef union {
    struct {
        /** reg_ar_qos_bur_len_thr_3 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of ar channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_ar_qos_bur_len_thr_3:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_ar_qos_bur_len_thr_3_reg_t;

/** Type of ar_qos_bur_len_thr_4 register
 *  Config the min value of token which can enable axi read transaction.
 */
typedef union {
    struct {
        /** reg_ar_qos_bur_len_thr_4 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of ar channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_ar_qos_bur_len_thr_4:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_ar_qos_bur_len_thr_4_reg_t;

/** Type of ar_qos_bur_len_thr_5 register
 *  Config the min value of token which can enable axi read transaction.
 */
typedef union {
    struct {
        /** reg_ar_qos_bur_len_thr_5 : R/W; bitpos: [9:0]; default: 64;
         *  limit the starting token value of ar channel to turn on transmission, default value
         *  is 64, 0 means no limit.
         */
        uint32_t reg_ar_qos_bur_len_thr_5:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} axi_ic_ar_qos_bur_len_thr_5_reg_t;


/** Group: aw_qos_dbg_tk_thr */
/** Type of aw_qos_dbg_tk_thr_0 register
 *  Config the debug smple interval in aw channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dbg_tk_thr_0 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_aw_qos_dbg_tk_thr_0:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_dbg_tk_thr_0_reg_t;

/** Type of aw_qos_dbg_tk_thr_1 register
 *  Config the debug smple interval in aw channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dbg_tk_thr_1 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_aw_qos_dbg_tk_thr_1:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_dbg_tk_thr_1_reg_t;

/** Type of aw_qos_dbg_tk_thr_2 register
 *  Config the debug smple interval in aw channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dbg_tk_thr_2 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_aw_qos_dbg_tk_thr_2:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_dbg_tk_thr_2_reg_t;

/** Type of aw_qos_dbg_tk_thr_3 register
 *  Config the debug smple interval in aw channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dbg_tk_thr_3 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_aw_qos_dbg_tk_thr_3:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_dbg_tk_thr_3_reg_t;

/** Type of aw_qos_dbg_tk_thr_4 register
 *  Config the debug smple interval in aw channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dbg_tk_thr_4 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_aw_qos_dbg_tk_thr_4:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_dbg_tk_thr_4_reg_t;

/** Type of aw_qos_dbg_tk_thr_5 register
 *  Config the debug smple interval in aw channel.
 */
typedef union {
    struct {
        /** reg_aw_qos_dbg_tk_thr_5 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_aw_qos_dbg_tk_thr_5:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_aw_qos_dbg_tk_thr_5_reg_t;


/** Group: ar_qos_dbg_tk_thr */
/** Type of ar_qos_dbg_tk_thr_0 register
 *  Config the debug smple interval in ar channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dbg_tk_thr_0 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_ar_qos_dbg_tk_thr_0:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_dbg_tk_thr_0_reg_t;

/** Type of ar_qos_dbg_tk_thr_1 register
 *  Config the debug smple interval in ar channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dbg_tk_thr_1 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_ar_qos_dbg_tk_thr_1:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_dbg_tk_thr_1_reg_t;

/** Type of ar_qos_dbg_tk_thr_2 register
 *  Config the debug smple interval in ar channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dbg_tk_thr_2 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_ar_qos_dbg_tk_thr_2:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_dbg_tk_thr_2_reg_t;

/** Type of ar_qos_dbg_tk_thr_3 register
 *  Config the debug smple interval in ar channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dbg_tk_thr_3 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_ar_qos_dbg_tk_thr_3:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_dbg_tk_thr_3_reg_t;

/** Type of ar_qos_dbg_tk_thr_4 register
 *  Config the debug smple interval in ar channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dbg_tk_thr_4 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_ar_qos_dbg_tk_thr_4:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_dbg_tk_thr_4_reg_t;

/** Type of ar_qos_dbg_tk_thr_5 register
 *  Config the debug smple interval in ar channel.
 */
typedef union {
    struct {
        /** reg_ar_qos_dbg_tk_thr_5 : R/W; bitpos: [7:0]; default: 16;
         *  limit the debug sample interval, default value is 16
         */
        uint32_t reg_ar_qos_dbg_tk_thr_5:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_ar_qos_dbg_tk_thr_5_reg_t;


/** Group: max_ost_aw_s */
/** Type of max_ost_aw_s_0 register
 *  Config the max outstanding num in current slave write port.
 */
typedef union {
    struct {
        /** reg_max_ost_aw_s_0 : R/W; bitpos: [5:0]; default: 15;
         *  limit the number of outstanding for slave port, default value is 15, max is 32.
         */
        uint32_t reg_max_ost_aw_s_0:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} axi_ic_max_ost_aw_s_0_reg_t;

/** Type of max_ost_aw_s_1 register
 *  Config the max outstanding num in current slave write port.
 */
typedef union {
    struct {
        /** reg_max_ost_aw_s_1 : R/W; bitpos: [5:0]; default: 15;
         *  limit the number of outstanding for slave port, default value is 15, max is 32.
         */
        uint32_t reg_max_ost_aw_s_1:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} axi_ic_max_ost_aw_s_1_reg_t;

/** Type of max_ost_aw_s_2 register
 *  Config the max outstanding num in current slave write port.
 */
typedef union {
    struct {
        /** reg_max_ost_aw_s_2 : R/W; bitpos: [5:0]; default: 15;
         *  limit the number of outstanding for slave port, default value is 15, max is 32.
         */
        uint32_t reg_max_ost_aw_s_2:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} axi_ic_max_ost_aw_s_2_reg_t;


/** Group: max_ost_ar_s */
/** Type of max_ost_ar_s_0 register
 *  Config the max outstanding num in current slave read port.
 */
typedef union {
    struct {
        /** reg_max_ost_ar_s_0 : R/W; bitpos: [7:0]; default: 15;
         *  limit the number of outstanding for slave port, default value is 15
         */
        uint32_t reg_max_ost_ar_s_0:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_max_ost_ar_s_0_reg_t;

/** Type of max_ost_ar_s_1 register
 *  Config the max outstanding num in current slave read port.
 */
typedef union {
    struct {
        /** reg_max_ost_ar_s_1 : R/W; bitpos: [7:0]; default: 15;
         *  limit the number of outstanding for slave port, default value is 15
         */
        uint32_t reg_max_ost_ar_s_1:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_max_ost_ar_s_1_reg_t;

/** Type of max_ost_ar_s_2 register
 *  Config the max outstanding num in current slave read port.
 */
typedef union {
    struct {
        /** reg_max_ost_ar_s_2 : R/W; bitpos: [7:0]; default: 15;
         *  limit the number of outstanding for slave port, default value is 15
         */
        uint32_t reg_max_ost_ar_s_2:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} axi_ic_max_ost_ar_s_2_reg_t;


/** Group: aw_qos_peak_rate_mode */
/** Type of aw_qos_peak_rate_mode register
 *  Config the aw channel token store mode in peak rate enable.
 */
typedef union {
    struct {
        /** reg_aw_qos_peak_rate_mode : R/W; bitpos: [5:0]; default: 0;
         *  peak rate, to control the aw channel request token num more than 1, default 0.
         */
        uint32_t reg_aw_qos_peak_rate_mode:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} axi_ic_aw_qos_peak_rate_mode_reg_t;


/** Group: ar_qos_peak_rate_mode */
/** Type of ar_qos_peak_rate_mode register
 *  Config the ar channel token store mode in peak rate enable.
 */
typedef union {
    struct {
        /** reg_ar_qos_peak_rate_mode : R/W; bitpos: [5:0]; default: 0;
         *  peak rate, to control the ar channel request token num  more than 1, default 0.
         */
        uint32_t reg_ar_qos_peak_rate_mode:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} axi_ic_ar_qos_peak_rate_mode_reg_t;


typedef struct {
    volatile axi_ic_date_reg_t date;
    volatile axi_ic_clk_en_reg_t clk_en;
    volatile axi_ic_interrupt_raw_reg_t interrupt_raw;
    volatile axi_ic_interrupt_clr_reg_t interrupt_clr;
    volatile axi_ic_interrupt_ena_reg_t interrupt_ena;
    volatile axi_ic_interrupt_st_reg_t interrupt_st;
    volatile axi_ic_timeout_max_cnt_reg_t timeout_max_cnt;
    volatile axi_ic_timeout_info_reg_t timeout_info;
    volatile axi_ic_dec_failure_info_reg_t dec_failure_info;
    volatile axi_ic_qos_debug_mst_sel_reg_t qos_debug_mst_sel;
    uint32_t reserved_028[22];
    volatile axi_ic_dec_failure_addr_reg_t dec_failure_addr;
    volatile axi_ic_reg_slave_is_secure_reg_t reg_slave_is_secure;
    volatile axi_ic_slave_security_override_reg_t slave_security_override;
    volatile axi_ic_master_is_secure_reg_t master_is_secure;
    volatile axi_ic_master_security_override_reg_t master_security_override;
    volatile axi_ic_aw_qos_value_0_reg_t aw_qos_value_0;
    volatile axi_ic_aw_qos_value_1_reg_t aw_qos_value_1;
    volatile axi_ic_aw_qos_value_2_reg_t aw_qos_value_2;
    volatile axi_ic_aw_qos_value_3_reg_t aw_qos_value_3;
    volatile axi_ic_aw_qos_value_4_reg_t aw_qos_value_4;
    volatile axi_ic_aw_qos_value_5_reg_t aw_qos_value_5;
    volatile axi_ic_ar_qos_value_0_reg_t ar_qos_value_0;
    volatile axi_ic_ar_qos_value_1_reg_t ar_qos_value_1;
    volatile axi_ic_ar_qos_value_2_reg_t ar_qos_value_2;
    volatile axi_ic_ar_qos_value_3_reg_t ar_qos_value_3;
    volatile axi_ic_ar_qos_value_4_reg_t ar_qos_value_4;
    volatile axi_ic_ar_qos_value_5_reg_t ar_qos_value_5;
    volatile axi_ic_b_prior_value_0_reg_t b_prior_value_0;
    volatile axi_ic_b_prior_value_1_reg_t b_prior_value_1;
    volatile axi_ic_b_prior_value_2_reg_t b_prior_value_2;
    volatile axi_ic_r_prior_value_0_reg_t r_prior_value_0;
    volatile axi_ic_r_prior_value_1_reg_t r_prior_value_1;
    volatile axi_ic_r_prior_value_2_reg_t r_prior_value_2;
    volatile axi_ic_aw_qos_sel_0_reg_t aw_qos_sel_0;
    volatile axi_ic_aw_qos_sel_1_reg_t aw_qos_sel_1;
    volatile axi_ic_aw_qos_sel_2_reg_t aw_qos_sel_2;
    volatile axi_ic_aw_qos_sel_3_reg_t aw_qos_sel_3;
    volatile axi_ic_aw_qos_sel_4_reg_t aw_qos_sel_4;
    volatile axi_ic_aw_qos_sel_5_reg_t aw_qos_sel_5;
    volatile axi_ic_ar_qos_sel_0_reg_t ar_qos_sel_0;
    volatile axi_ic_ar_qos_sel_1_reg_t ar_qos_sel_1;
    volatile axi_ic_ar_qos_sel_2_reg_t ar_qos_sel_2;
    volatile axi_ic_ar_qos_sel_3_reg_t ar_qos_sel_3;
    volatile axi_ic_ar_qos_sel_4_reg_t ar_qos_sel_4;
    volatile axi_ic_ar_qos_sel_5_reg_t ar_qos_sel_5;
    volatile axi_ic_aw_qos_recovery_reg_t aw_qos_recovery;
    volatile axi_ic_ar_qos_recovery_reg_t ar_qos_recovery;
    volatile axi_ic_aw_qos_id_mask_0_reg_t aw_qos_id_mask_0;
    volatile axi_ic_aw_qos_id_mask_1_reg_t aw_qos_id_mask_1;
    volatile axi_ic_aw_qos_id_mask_2_reg_t aw_qos_id_mask_2;
    volatile axi_ic_aw_qos_id_mask_3_reg_t aw_qos_id_mask_3;
    volatile axi_ic_aw_qos_id_mask_4_reg_t aw_qos_id_mask_4;
    volatile axi_ic_aw_qos_id_mask_5_reg_t aw_qos_id_mask_5;
    volatile axi_ic_ar_qos_id_mask_0_reg_t ar_qos_id_mask_0;
    volatile axi_ic_ar_qos_id_mask_1_reg_t ar_qos_id_mask_1;
    volatile axi_ic_ar_qos_id_mask_2_reg_t ar_qos_id_mask_2;
    volatile axi_ic_ar_qos_id_mask_3_reg_t ar_qos_id_mask_3;
    volatile axi_ic_ar_qos_id_mask_4_reg_t ar_qos_id_mask_4;
    volatile axi_ic_ar_qos_id_mask_5_reg_t ar_qos_id_mask_5;
    volatile axi_ic_aw_qos_id_filter_0_reg_t aw_qos_id_filter_0;
    volatile axi_ic_aw_qos_id_filter_1_reg_t aw_qos_id_filter_1;
    volatile axi_ic_aw_qos_id_filter_2_reg_t aw_qos_id_filter_2;
    volatile axi_ic_aw_qos_id_filter_3_reg_t aw_qos_id_filter_3;
    volatile axi_ic_aw_qos_id_filter_4_reg_t aw_qos_id_filter_4;
    volatile axi_ic_aw_qos_id_filter_5_reg_t aw_qos_id_filter_5;
    volatile axi_ic_ar_qos_id_filter_0_reg_t ar_qos_id_filter_0;
    volatile axi_ic_ar_qos_id_filter_1_reg_t ar_qos_id_filter_1;
    volatile axi_ic_ar_qos_id_filter_2_reg_t ar_qos_id_filter_2;
    volatile axi_ic_ar_qos_id_filter_3_reg_t ar_qos_id_filter_3;
    volatile axi_ic_ar_qos_id_filter_4_reg_t ar_qos_id_filter_4;
    volatile axi_ic_ar_qos_id_filter_5_reg_t ar_qos_id_filter_5;
    volatile axi_ic_aw_qos_ctrl_cfg_0_reg_t aw_qos_ctrl_cfg_0;
    volatile axi_ic_aw_qos_ctrl_cfg_1_reg_t aw_qos_ctrl_cfg_1;
    volatile axi_ic_aw_qos_ctrl_cfg_2_reg_t aw_qos_ctrl_cfg_2;
    volatile axi_ic_aw_qos_ctrl_cfg_3_reg_t aw_qos_ctrl_cfg_3;
    volatile axi_ic_aw_qos_ctrl_cfg_4_reg_t aw_qos_ctrl_cfg_4;
    volatile axi_ic_aw_qos_ctrl_cfg_5_reg_t aw_qos_ctrl_cfg_5;
    volatile axi_ic_ar_qos_ctrl_cfg_0_reg_t ar_qos_ctrl_cfg_0;
    volatile axi_ic_ar_qos_ctrl_cfg_1_reg_t ar_qos_ctrl_cfg_1;
    volatile axi_ic_ar_qos_ctrl_cfg_2_reg_t ar_qos_ctrl_cfg_2;
    volatile axi_ic_ar_qos_ctrl_cfg_3_reg_t ar_qos_ctrl_cfg_3;
    volatile axi_ic_ar_qos_ctrl_cfg_4_reg_t ar_qos_ctrl_cfg_4;
    volatile axi_ic_ar_qos_ctrl_cfg_5_reg_t ar_qos_ctrl_cfg_5;
    volatile axi_ic_aw_qos_trans_rate_0_reg_t aw_qos_trans_rate_0;
    volatile axi_ic_aw_qos_trans_rate_1_reg_t aw_qos_trans_rate_1;
    volatile axi_ic_aw_qos_trans_rate_2_reg_t aw_qos_trans_rate_2;
    volatile axi_ic_aw_qos_trans_rate_3_reg_t aw_qos_trans_rate_3;
    volatile axi_ic_aw_qos_trans_rate_4_reg_t aw_qos_trans_rate_4;
    volatile axi_ic_aw_qos_trans_rate_5_reg_t aw_qos_trans_rate_5;
    volatile axi_ic_ar_qos_trans_rate_0_reg_t ar_qos_trans_rate_0;
    volatile axi_ic_ar_qos_trans_rate_1_reg_t ar_qos_trans_rate_1;
    volatile axi_ic_ar_qos_trans_rate_2_reg_t ar_qos_trans_rate_2;
    volatile axi_ic_ar_qos_trans_rate_3_reg_t ar_qos_trans_rate_3;
    volatile axi_ic_ar_qos_trans_rate_4_reg_t ar_qos_trans_rate_4;
    volatile axi_ic_ar_qos_trans_rate_5_reg_t ar_qos_trans_rate_5;
    volatile axi_ic_aw_qos_peak_rate_0_reg_t aw_qos_peak_rate_0;
    volatile axi_ic_aw_qos_peak_rate_1_reg_t aw_qos_peak_rate_1;
    volatile axi_ic_aw_qos_peak_rate_2_reg_t aw_qos_peak_rate_2;
    volatile axi_ic_aw_qos_peak_rate_3_reg_t aw_qos_peak_rate_3;
    volatile axi_ic_aw_qos_peak_rate_4_reg_t aw_qos_peak_rate_4;
    volatile axi_ic_aw_qos_peak_rate_5_reg_t aw_qos_peak_rate_5;
    volatile axi_ic_ar_qos_peak_rate_0_reg_t ar_qos_peak_rate_0;
    volatile axi_ic_ar_qos_peak_rate_1_reg_t ar_qos_peak_rate_1;
    volatile axi_ic_ar_qos_peak_rate_2_reg_t ar_qos_peak_rate_2;
    volatile axi_ic_ar_qos_peak_rate_3_reg_t ar_qos_peak_rate_3;
    volatile axi_ic_ar_qos_peak_rate_4_reg_t ar_qos_peak_rate_4;
    volatile axi_ic_ar_qos_peak_rate_5_reg_t ar_qos_peak_rate_5;
    volatile axi_ic_aw_qos_burstiness_0_reg_t aw_qos_burstiness_0;
    volatile axi_ic_aw_qos_burstiness_1_reg_t aw_qos_burstiness_1;
    volatile axi_ic_aw_qos_burstiness_2_reg_t aw_qos_burstiness_2;
    volatile axi_ic_aw_qos_burstiness_3_reg_t aw_qos_burstiness_3;
    volatile axi_ic_aw_qos_burstiness_4_reg_t aw_qos_burstiness_4;
    volatile axi_ic_aw_qos_burstiness_5_reg_t aw_qos_burstiness_5;
    volatile axi_ic_ar_qos_burstiness_0_reg_t ar_qos_burstiness_0;
    volatile axi_ic_ar_qos_burstiness_1_reg_t ar_qos_burstiness_1;
    volatile axi_ic_ar_qos_burstiness_2_reg_t ar_qos_burstiness_2;
    volatile axi_ic_ar_qos_burstiness_3_reg_t ar_qos_burstiness_3;
    volatile axi_ic_ar_qos_burstiness_4_reg_t ar_qos_burstiness_4;
    volatile axi_ic_ar_qos_burstiness_5_reg_t ar_qos_burstiness_5;
    volatile axi_ic_aw_qos_cfg_vlu_0_reg_t aw_qos_cfg_vlu_0;
    volatile axi_ic_aw_qos_cfg_vlu_1_reg_t aw_qos_cfg_vlu_1;
    volatile axi_ic_aw_qos_cfg_vlu_2_reg_t aw_qos_cfg_vlu_2;
    volatile axi_ic_aw_qos_cfg_vlu_3_reg_t aw_qos_cfg_vlu_3;
    volatile axi_ic_aw_qos_cfg_vlu_4_reg_t aw_qos_cfg_vlu_4;
    volatile axi_ic_aw_qos_cfg_vlu_5_reg_t aw_qos_cfg_vlu_5;
    volatile axi_ic_ar_qos_cfg_vlu_0_reg_t ar_qos_cfg_vlu_0;
    volatile axi_ic_ar_qos_cfg_vlu_1_reg_t ar_qos_cfg_vlu_1;
    volatile axi_ic_ar_qos_cfg_vlu_2_reg_t ar_qos_cfg_vlu_2;
    volatile axi_ic_ar_qos_cfg_vlu_3_reg_t ar_qos_cfg_vlu_3;
    volatile axi_ic_ar_qos_cfg_vlu_4_reg_t ar_qos_cfg_vlu_4;
    volatile axi_ic_ar_qos_cfg_vlu_5_reg_t ar_qos_cfg_vlu_5;
    volatile axi_ic_aw_qos_dly_thr0_0_reg_t aw_qos_dly_thr0_0;
    volatile axi_ic_aw_qos_dly_thr0_1_reg_t aw_qos_dly_thr0_1;
    volatile axi_ic_aw_qos_dly_thr0_2_reg_t aw_qos_dly_thr0_2;
    volatile axi_ic_aw_qos_dly_thr0_3_reg_t aw_qos_dly_thr0_3;
    volatile axi_ic_aw_qos_dly_thr0_4_reg_t aw_qos_dly_thr0_4;
    volatile axi_ic_aw_qos_dly_thr0_5_reg_t aw_qos_dly_thr0_5;
    volatile axi_ic_aw_qos_dly_thr1_0_reg_t aw_qos_dly_thr1_0;
    volatile axi_ic_aw_qos_dly_thr1_1_reg_t aw_qos_dly_thr1_1;
    volatile axi_ic_aw_qos_dly_thr1_2_reg_t aw_qos_dly_thr1_2;
    volatile axi_ic_aw_qos_dly_thr1_3_reg_t aw_qos_dly_thr1_3;
    volatile axi_ic_aw_qos_dly_thr1_4_reg_t aw_qos_dly_thr1_4;
    volatile axi_ic_aw_qos_dly_thr1_5_reg_t aw_qos_dly_thr1_5;
    volatile axi_ic_aw_qos_dly_thr2_0_reg_t aw_qos_dly_thr2_0;
    volatile axi_ic_aw_qos_dly_thr2_1_reg_t aw_qos_dly_thr2_1;
    volatile axi_ic_aw_qos_dly_thr2_2_reg_t aw_qos_dly_thr2_2;
    volatile axi_ic_aw_qos_dly_thr2_3_reg_t aw_qos_dly_thr2_3;
    volatile axi_ic_aw_qos_dly_thr2_4_reg_t aw_qos_dly_thr2_4;
    volatile axi_ic_aw_qos_dly_thr2_5_reg_t aw_qos_dly_thr2_5;
    volatile axi_ic_aw_qos_dly_thr3_0_reg_t aw_qos_dly_thr3_0;
    volatile axi_ic_aw_qos_dly_thr3_1_reg_t aw_qos_dly_thr3_1;
    volatile axi_ic_aw_qos_dly_thr3_2_reg_t aw_qos_dly_thr3_2;
    volatile axi_ic_aw_qos_dly_thr3_3_reg_t aw_qos_dly_thr3_3;
    volatile axi_ic_aw_qos_dly_thr3_4_reg_t aw_qos_dly_thr3_4;
    volatile axi_ic_aw_qos_dly_thr3_5_reg_t aw_qos_dly_thr3_5;
    volatile axi_ic_ar_qos_dly_thr0_0_reg_t ar_qos_dly_thr0_0;
    volatile axi_ic_ar_qos_dly_thr0_1_reg_t ar_qos_dly_thr0_1;
    volatile axi_ic_ar_qos_dly_thr0_2_reg_t ar_qos_dly_thr0_2;
    volatile axi_ic_ar_qos_dly_thr0_3_reg_t ar_qos_dly_thr0_3;
    volatile axi_ic_ar_qos_dly_thr0_4_reg_t ar_qos_dly_thr0_4;
    volatile axi_ic_ar_qos_dly_thr0_5_reg_t ar_qos_dly_thr0_5;
    volatile axi_ic_ar_qos_dly_thr1_0_reg_t ar_qos_dly_thr1_0;
    volatile axi_ic_ar_qos_dly_thr1_1_reg_t ar_qos_dly_thr1_1;
    volatile axi_ic_ar_qos_dly_thr1_2_reg_t ar_qos_dly_thr1_2;
    volatile axi_ic_ar_qos_dly_thr1_3_reg_t ar_qos_dly_thr1_3;
    volatile axi_ic_ar_qos_dly_thr1_4_reg_t ar_qos_dly_thr1_4;
    volatile axi_ic_ar_qos_dly_thr1_5_reg_t ar_qos_dly_thr1_5;
    volatile axi_ic_ar_qos_dly_thr2_0_reg_t ar_qos_dly_thr2_0;
    volatile axi_ic_ar_qos_dly_thr2_1_reg_t ar_qos_dly_thr2_1;
    volatile axi_ic_ar_qos_dly_thr2_2_reg_t ar_qos_dly_thr2_2;
    volatile axi_ic_ar_qos_dly_thr2_3_reg_t ar_qos_dly_thr2_3;
    volatile axi_ic_ar_qos_dly_thr2_4_reg_t ar_qos_dly_thr2_4;
    volatile axi_ic_ar_qos_dly_thr2_5_reg_t ar_qos_dly_thr2_5;
    volatile axi_ic_ar_qos_dly_thr3_0_reg_t ar_qos_dly_thr3_0;
    volatile axi_ic_ar_qos_dly_thr3_1_reg_t ar_qos_dly_thr3_1;
    volatile axi_ic_ar_qos_dly_thr3_2_reg_t ar_qos_dly_thr3_2;
    volatile axi_ic_ar_qos_dly_thr3_3_reg_t ar_qos_dly_thr3_3;
    volatile axi_ic_ar_qos_dly_thr3_4_reg_t ar_qos_dly_thr3_4;
    volatile axi_ic_ar_qos_dly_thr3_5_reg_t ar_qos_dly_thr3_5;
    volatile axi_ic_aw_qos_bur_len_thr_0_reg_t aw_qos_bur_len_thr_0;
    volatile axi_ic_aw_qos_bur_len_thr_1_reg_t aw_qos_bur_len_thr_1;
    volatile axi_ic_aw_qos_bur_len_thr_2_reg_t aw_qos_bur_len_thr_2;
    volatile axi_ic_aw_qos_bur_len_thr_3_reg_t aw_qos_bur_len_thr_3;
    volatile axi_ic_aw_qos_bur_len_thr_4_reg_t aw_qos_bur_len_thr_4;
    volatile axi_ic_aw_qos_bur_len_thr_5_reg_t aw_qos_bur_len_thr_5;
    volatile axi_ic_ar_qos_bur_len_thr_0_reg_t ar_qos_bur_len_thr_0;
    volatile axi_ic_ar_qos_bur_len_thr_1_reg_t ar_qos_bur_len_thr_1;
    volatile axi_ic_ar_qos_bur_len_thr_2_reg_t ar_qos_bur_len_thr_2;
    volatile axi_ic_ar_qos_bur_len_thr_3_reg_t ar_qos_bur_len_thr_3;
    volatile axi_ic_ar_qos_bur_len_thr_4_reg_t ar_qos_bur_len_thr_4;
    volatile axi_ic_ar_qos_bur_len_thr_5_reg_t ar_qos_bur_len_thr_5;
    volatile axi_ic_aw_qos_dbg_tk_thr_0_reg_t aw_qos_dbg_tk_thr_0;
    volatile axi_ic_aw_qos_dbg_tk_thr_1_reg_t aw_qos_dbg_tk_thr_1;
    volatile axi_ic_aw_qos_dbg_tk_thr_2_reg_t aw_qos_dbg_tk_thr_2;
    volatile axi_ic_aw_qos_dbg_tk_thr_3_reg_t aw_qos_dbg_tk_thr_3;
    volatile axi_ic_aw_qos_dbg_tk_thr_4_reg_t aw_qos_dbg_tk_thr_4;
    volatile axi_ic_aw_qos_dbg_tk_thr_5_reg_t aw_qos_dbg_tk_thr_5;
    volatile axi_ic_ar_qos_dbg_tk_thr_0_reg_t ar_qos_dbg_tk_thr_0;
    volatile axi_ic_ar_qos_dbg_tk_thr_1_reg_t ar_qos_dbg_tk_thr_1;
    volatile axi_ic_ar_qos_dbg_tk_thr_2_reg_t ar_qos_dbg_tk_thr_2;
    volatile axi_ic_ar_qos_dbg_tk_thr_3_reg_t ar_qos_dbg_tk_thr_3;
    volatile axi_ic_ar_qos_dbg_tk_thr_4_reg_t ar_qos_dbg_tk_thr_4;
    volatile axi_ic_ar_qos_dbg_tk_thr_5_reg_t ar_qos_dbg_tk_thr_5;
    volatile axi_ic_max_ost_aw_s_0_reg_t max_ost_aw_s_0;
    volatile axi_ic_max_ost_aw_s_1_reg_t max_ost_aw_s_1;
    volatile axi_ic_max_ost_aw_s_2_reg_t max_ost_aw_s_2;
    volatile axi_ic_max_ost_ar_s_0_reg_t max_ost_ar_s_0;
    volatile axi_ic_max_ost_ar_s_1_reg_t max_ost_ar_s_1;
    volatile axi_ic_max_ost_ar_s_2_reg_t max_ost_ar_s_2;
    volatile axi_ic_aw_qos_peak_rate_mode_reg_t aw_qos_peak_rate_mode;
    volatile axi_ic_ar_qos_peak_rate_mode_reg_t ar_qos_peak_rate_mode;
} axi_ic_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(axi_ic_dev_t) == 0x3a4, "Invalid size of axi_ic_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
