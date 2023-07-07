/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: User-defined control registers */
/** Type of cmd register
 *  SPI1 memory command register
 */
typedef union {
    struct {
        /** mst_st : RO; bitpos: [3:0]; default: 0;
         *  The current status of SPI1 master FSM.
         */
        uint32_t mst_st:4;
        /** slv_st : RO; bitpos: [7:4]; default: 0;
         *  The current status of SPI1 slave FSM: mspi_st. 0: idle state, 1: preparation state,
         *  2: send command state, 3: send address state, 4: wait state, 5: read data state,
         *  6:write data state, 7: done state, 8: read data end state.
         */
        uint32_t slv_st:4;
        uint32_t reserved_8:9;
        /** flash_pe : R/W/SC; bitpos: [17]; default: 0;
         *  In user mode, it is set to indicate that program/erase operation will be triggered.
         *  The bit is combined with spi1_mem_s_usr bit. The bit will be cleared once the
         *  operation done.1: enable 0: disable.
         */
        uint32_t flash_pe:1;
        /** usr : R/W/SC; bitpos: [18]; default: 0;
         *  User define command enable.  An operation will be triggered when the bit is set.
         *  The bit will be cleared once the operation done.1: enable 0: disable.
         */
        uint32_t usr:1;
        /** flash_hpm : R/W/SC; bitpos: [19]; default: 0;
         *  Drive Flash into high performance mode.  The bit will be cleared once the operation
         *  done.1: enable 0: disable.
         */
        uint32_t flash_hpm:1;
        /** flash_res : R/W/SC; bitpos: [20]; default: 0;
         *  This bit combined with reg_resandres bit releases Flash from the power-down state
         *  or high performance mode and obtains the devices ID. The bit will be cleared once
         *  the operation done.1: enable 0: disable.
         */
        uint32_t flash_res:1;
        /** flash_dp : R/W/SC; bitpos: [21]; default: 0;
         *  Drive Flash into power down.  An operation will be triggered when the bit is set.
         *  The bit will be cleared once the operation done.1: enable 0: disable.
         */
        uint32_t flash_dp:1;
        /** flash_ce : R/W/SC; bitpos: [22]; default: 0;
         *  Chip erase enable. Chip erase operation will be triggered when the bit is set. The
         *  bit will be cleared once the operation done.1: enable 0: disable.
         */
        uint32_t flash_ce:1;
        /** flash_be : R/W/SC; bitpos: [23]; default: 0;
         *  Block erase enable(32KB) .  Block erase operation will be triggered when the bit is
         *  set. The bit will be cleared once the operation done.1: enable 0: disable.
         */
        uint32_t flash_be:1;
        /** flash_se : R/W/SC; bitpos: [24]; default: 0;
         *  Sector erase enable(4KB). Sector erase operation will be triggered when the bit is
         *  set. The bit will be cleared once the operation done.1: enable 0: disable.
         */
        uint32_t flash_se:1;
        /** flash_pp : R/W/SC; bitpos: [25]; default: 0;
         *  Page program enable(1 byte ~256 bytes data to be programmed). Page program
         *  operation  will be triggered when the bit is set. The bit will be cleared once the
         *  operation done .1: enable 0: disable.
         */
        uint32_t flash_pp:1;
        /** flash_wrsr : R/W/SC; bitpos: [26]; default: 0;
         *  Write status register enable.   Write status operation  will be triggered when the
         *  bit is set. The bit will be cleared once the operation done.1: enable 0: disable.
         */
        uint32_t flash_wrsr:1;
        /** flash_rdsr : R/W/SC; bitpos: [27]; default: 0;
         *  Read status register-1.  Read status operation will be triggered when the bit is
         *  set. The bit will be cleared once the operation done.1: enable 0: disable.
         */
        uint32_t flash_rdsr:1;
        /** flash_rdid : R/W/SC; bitpos: [28]; default: 0;
         *  Read JEDEC ID . Read ID command will be sent when the bit is set. The bit will be
         *  cleared once the operation done. 1: enable 0: disable.
         */
        uint32_t flash_rdid:1;
        /** flash_wrdi : R/W/SC; bitpos: [29]; default: 0;
         *  Write flash disable. Write disable command will be sent when the bit is set. The
         *  bit will be cleared once the operation done. 1: enable 0: disable.
         */
        uint32_t flash_wrdi:1;
        /** flash_wren : R/W/SC; bitpos: [30]; default: 0;
         *  Write flash enable.  Write enable command will be sent when the bit is set. The bit
         *  will be cleared once the operation done. 1: enable 0: disable.
         */
        uint32_t flash_wren:1;
        /** flash_read : R/W/SC; bitpos: [31]; default: 0;
         *  Read flash enable. Read flash operation will be triggered when the bit is set. The
         *  bit will be cleared once the operation done. 1: enable 0: disable.
         */
        uint32_t flash_read:1;
    };
    uint32_t val;
} spi1_mem_s_cmd_reg_t;

/** Type of addr register
 *  SPI1 address register
 */
typedef union {
    struct {
        /** usr_addr_value : R/W; bitpos: [31:0]; default: 0;
         *  In user mode, it is the memory address. other then the bit0-bit23 is the memory
         *  address, the bit24-bit31 are the byte length of a transfer.
         */
        uint32_t usr_addr_value:32;
    };
    uint32_t val;
} spi1_mem_s_addr_reg_t;

/** Type of user register
 *  SPI1 user register.
 */
typedef union {
    struct {
        uint32_t reserved_0:9;
        /** ck_out_edge : R/W; bitpos: [9]; default: 0;
         *  the bit combined with spi1_mem_s_mosi_delay_mode bits to set mosi signal delay mode.
         */
        uint32_t ck_out_edge:1;
        uint32_t reserved_10:2;
        /** fwrite_dual : R/W; bitpos: [12]; default: 0;
         *  In the write operations read-data phase apply 2 signals
         */
        uint32_t fwrite_dual:1;
        /** fwrite_quad : R/W; bitpos: [13]; default: 0;
         *  In the write operations read-data phase apply 4 signals
         */
        uint32_t fwrite_quad:1;
        /** fwrite_dio : R/W; bitpos: [14]; default: 0;
         *  In the write operations address phase and read-data phase apply 2 signals.
         */
        uint32_t fwrite_dio:1;
        /** fwrite_qio : R/W; bitpos: [15]; default: 0;
         *  In the write operations address phase and read-data phase apply 4 signals.
         */
        uint32_t fwrite_qio:1;
        uint32_t reserved_16:8;
        /** usr_miso_highpart : R/W; bitpos: [24]; default: 0;
         *  read-data phase only access to high-part of the buffer spi1_mem_s_w8~spi1_mem_s_w15. 1:
         *  enable 0: disable.
         */
        uint32_t usr_miso_highpart:1;
        /** usr_mosi_highpart : R/W; bitpos: [25]; default: 0;
         *  write-data phase only access to high-part of the buffer spi1_mem_s_w8~spi1_mem_s_w15. 1:
         *  enable 0: disable.
         */
        uint32_t usr_mosi_highpart:1;
        /** usr_dummy_idle : R/W; bitpos: [26]; default: 0;
         *  SPI clock is disable in dummy phase when the bit is enable.
         */
        uint32_t usr_dummy_idle:1;
        /** usr_mosi : R/W; bitpos: [27]; default: 0;
         *  This bit enable the write-data phase of an operation.
         */
        uint32_t usr_mosi:1;
        /** usr_miso : R/W; bitpos: [28]; default: 0;
         *  This bit enable the read-data phase of an operation.
         */
        uint32_t usr_miso:1;
        /** usr_dummy : R/W; bitpos: [29]; default: 0;
         *  This bit enable the dummy phase of an operation.
         */
        uint32_t usr_dummy:1;
        /** usr_addr : R/W; bitpos: [30]; default: 0;
         *  This bit enable the address phase of an operation.
         */
        uint32_t usr_addr:1;
        /** usr_command : R/W; bitpos: [31]; default: 1;
         *  This bit enable the command phase of an operation.
         */
        uint32_t usr_command:1;
    };
    uint32_t val;
} spi1_mem_s_user_reg_t;

/** Type of user1 register
 *  SPI1 user1 register.
 */
typedef union {
    struct {
        /** usr_dummy_cyclelen : R/W; bitpos: [5:0]; default: 7;
         *  The length in spi1_mem_s_clk cycles of dummy phase. The register value shall be
         *  (cycle_num-1).
         */
        uint32_t usr_dummy_cyclelen:6;
        uint32_t reserved_6:20;
        /** usr_addr_bitlen : R/W; bitpos: [31:26]; default: 23;
         *  The length in bits of address phase. The register value shall be (bit_num-1).
         */
        uint32_t usr_addr_bitlen:6;
    };
    uint32_t val;
} spi1_mem_s_user1_reg_t;

/** Type of user2 register
 *  SPI1 user2 register.
 */
typedef union {
    struct {
        /** usr_command_value : R/W; bitpos: [15:0]; default: 0;
         *  The value of  command.
         */
        uint32_t usr_command_value:16;
        uint32_t reserved_16:12;
        /** usr_command_bitlen : R/W; bitpos: [31:28]; default: 7;
         *  The length in bits of command phase. The register value shall be (bit_num-1)
         */
        uint32_t usr_command_bitlen:4;
    };
    uint32_t val;
} spi1_mem_s_user2_reg_t;


/** Group: Control and configuration registers */
/** Type of ctrl register
 *  SPI1 control register.
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** fdummy_rin : R/W; bitpos: [2]; default: 1;
         *  In the dummy phase of a MSPI read data transfer when accesses to flash, the signal
         *  level of SPI bus is output by the MSPI controller.
         */
        uint32_t fdummy_rin:1;
        /** fdummy_wout : R/W; bitpos: [3]; default: 1;
         *  In the dummy phase of a MSPI write data transfer when accesses to flash, the signal
         *  level of SPI bus is output by the MSPI controller.
         */
        uint32_t fdummy_wout:1;
        /** fdout_oct : R/W; bitpos: [4]; default: 0;
         *  Apply 8 signals during write-data phase 1:enable 0: disable
         */
        uint32_t fdout_oct:1;
        /** fdin_oct : R/W; bitpos: [5]; default: 0;
         *  Apply 8 signals during read-data phase 1:enable 0: disable
         */
        uint32_t fdin_oct:1;
        /** faddr_oct : R/W; bitpos: [6]; default: 0;
         *  Apply 8 signals during address phase 1:enable 0: disable
         */
        uint32_t faddr_oct:1;
        uint32_t reserved_7:1;
        /** fcmd_quad : R/W; bitpos: [8]; default: 0;
         *  Apply 4 signals during command phase 1:enable 0: disable
         */
        uint32_t fcmd_quad:1;
        /** fcmd_oct : R/W; bitpos: [9]; default: 0;
         *  Apply 8 signals during command phase 1:enable 0: disable
         */
        uint32_t fcmd_oct:1;
        /** fcs_crc_en : R/W; bitpos: [10]; default: 0;
         *  For SPI1,  initialize crc32 module before writing encrypted data to flash. Active
         *  low.
         */
        uint32_t fcs_crc_en:1;
        /** tx_crc_en : R/W; bitpos: [11]; default: 0;
         *  For SPI1,  enable crc32 when writing encrypted data to flash. 1: enable 0:disable
         */
        uint32_t tx_crc_en:1;
        uint32_t reserved_12:1;
        /** fastrd_mode : R/W; bitpos: [13]; default: 1;
         *  This bit enable the bits: spi1_mem_s_fread_qio, spi1_mem_s_fread_dio, spi1_mem_s_fread_qout
         *  and spi1_mem_s_fread_dout. 1: enable 0: disable.
         */
        uint32_t fastrd_mode:1;
        /** fread_dual : R/W; bitpos: [14]; default: 0;
         *  In the read operations, read-data phase apply 2 signals. 1: enable 0: disable.
         */
        uint32_t fread_dual:1;
        /** resandres : R/W; bitpos: [15]; default: 1;
         *  The Device ID is read out to SPI1_MEM_S_RD_STATUS register,  this bit combine with
         *  spi1_mem_s_flash_res bit. 1: enable 0: disable.
         */
        uint32_t resandres:1;
        uint32_t reserved_16:2;
        /** q_pol : R/W; bitpos: [18]; default: 1;
         *  The bit is used to set MISO line polarity, 1: high 0, low
         */
        uint32_t q_pol:1;
        /** d_pol : R/W; bitpos: [19]; default: 1;
         *  The bit is used to set MOSI line polarity, 1: high 0, low
         */
        uint32_t d_pol:1;
        /** fread_quad : R/W; bitpos: [20]; default: 0;
         *  In the read operations read-data phase apply 4 signals. 1: enable 0: disable.
         */
        uint32_t fread_quad:1;
        /** wp_reg : R/W; bitpos: [21]; default: 1;
         *  Write protect signal output when SPI is idle.  1: output high, 0: output low.
         */
        uint32_t wp_reg:1;
        /** wrsr_2b : R/W; bitpos: [22]; default: 0;
         *  two bytes data will be written to status register when it is set. 1: enable 0:
         *  disable.
         */
        uint32_t wrsr_2b:1;
        /** fread_dio : R/W; bitpos: [23]; default: 0;
         *  In the read operations address phase and read-data phase apply 2 signals. 1: enable
         *  0: disable.
         */
        uint32_t fread_dio:1;
        /** fread_qio : R/W; bitpos: [24]; default: 0;
         *  In the read operations address phase and read-data phase apply 4 signals. 1: enable
         *  0: disable.
         */
        uint32_t fread_qio:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} spi1_mem_s_ctrl_reg_t;

/** Type of ctrl1 register
 *  SPI1 control1 register.
 */
typedef union {
    struct {
        /** clk_mode : R/W; bitpos: [1:0]; default: 0;
         *  SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed
         *  one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3:
         *  SPI clock is alwasy on.
         */
        uint32_t clk_mode:2;
        /** cs_hold_dly_res : R/W; bitpos: [11:2]; default: 1023;
         *  After RES/DP/HPM command is sent, SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 512)
         *  SPI_CLK cycles.
         */
        uint32_t cs_hold_dly_res:10;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} spi1_mem_s_ctrl1_reg_t;

/** Type of ctrl2 register
 *  SPI1 control2 register.
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** sync_reset : WT; bitpos: [31]; default: 0;
         *  The FSM will be reset.
         */
        uint32_t sync_reset:1;
    };
    uint32_t val;
} spi1_mem_s_ctrl2_reg_t;

/** Type of clock register
 *  SPI1 clock division control register.
 */
typedef union {
    struct {
        /** clkcnt_l : R/W; bitpos: [7:0]; default: 3;
         *  In the master mode it must be equal to spi1_mem_s_clkcnt_N.
         */
        uint32_t clkcnt_l:8;
        /** clkcnt_h : R/W; bitpos: [15:8]; default: 1;
         *  In the master mode it must be floor((spi1_mem_s_clkcnt_N+1)/2-1).
         */
        uint32_t clkcnt_h:8;
        /** clkcnt_n : R/W; bitpos: [23:16]; default: 3;
         *  In the master mode it is the divider of spi1_mem_s_clk. So spi1_mem_s_clk frequency is
         *  system/(spi1_mem_s_clkcnt_N+1)
         */
        uint32_t clkcnt_n:8;
        uint32_t reserved_24:7;
        /** clk_equ_sysclk : R/W; bitpos: [31]; default: 0;
         *  reserved
         */
        uint32_t clk_equ_sysclk:1;
    };
    uint32_t val;
} spi1_mem_s_clock_reg_t;

/** Type of mosi_dlen register
 *  SPI1 send data bit length control register.
 */
typedef union {
    struct {
        /** usr_mosi_dbitlen : R/W; bitpos: [9:0]; default: 0;
         *  The length in bits of write-data. The register value shall be (bit_num-1).
         */
        uint32_t usr_mosi_dbitlen:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} spi1_mem_s_mosi_dlen_reg_t;

/** Type of miso_dlen register
 *  SPI1 receive data bit length control register.
 */
typedef union {
    struct {
        /** usr_miso_dbitlen : R/W; bitpos: [9:0]; default: 0;
         *  The length in bits of  read-data. The register value shall be (bit_num-1).
         */
        uint32_t usr_miso_dbitlen:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} spi1_mem_s_miso_dlen_reg_t;

/** Type of rd_status register
 *  SPI1 status register.
 */
typedef union {
    struct {
        /** status : R/W/SS; bitpos: [15:0]; default: 0;
         *  The value is stored when set spi1_mem_s_flash_rdsr bit and spi1_mem_s_flash_res bit.
         */
        uint32_t status:16;
        /** wb_mode : R/W; bitpos: [23:16]; default: 0;
         *  Mode bits in the flash fast read mode  it is combined with spi1_mem_s_fastrd_mode bit.
         */
        uint32_t wb_mode:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} spi1_mem_s_rd_status_reg_t;

/** Type of misc register
 *  SPI1 misc register
 */
typedef union {
    struct {
        /** cs0_dis : R/W; bitpos: [0]; default: 0;
         *  SPI_CS0 pin enable, 1: disable SPI_CS0, 0: SPI_CS0 pin is active to select SPI
         *  device, such as flash, external RAM and so on.
         */
        uint32_t cs0_dis:1;
        /** cs1_dis : R/W; bitpos: [1]; default: 1;
         *  SPI_CS1 pin enable, 1: disable SPI_CS1, 0: SPI_CS1 pin is active to select SPI
         *  device, such as flash, external RAM and so on.
         */
        uint32_t cs1_dis:1;
        uint32_t reserved_2:7;
        /** ck_idle_edge : R/W; bitpos: [9]; default: 0;
         *  1: spi clk line is high when idle     0: spi clk line is low when idle
         */
        uint32_t ck_idle_edge:1;
        /** cs_keep_active : R/W; bitpos: [10]; default: 0;
         *  spi cs line keep low when the bit is set.
         */
        uint32_t cs_keep_active:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} spi1_mem_s_misc_reg_t;

/** Type of cache_fctrl register
 *  SPI1 bit mode control register.
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** cache_usr_addr_4byte : R/W; bitpos: [1]; default: 0;
         *  For SPI1,  cache  read flash with 4 bytes address, 1: enable, 0:disable.
         */
        uint32_t cache_usr_addr_4byte:1;
        uint32_t reserved_2:1;
        /** fdin_dual : R/W; bitpos: [3]; default: 0;
         *  For SPI1, din phase apply 2 signals. 1: enable 0: disable. The bit is the same with
         *  spi1_mem_s_fread_dio.
         */
        uint32_t fdin_dual:1;
        /** fdout_dual : R/W; bitpos: [4]; default: 0;
         *  For SPI1, dout phase apply 2 signals. 1: enable 0: disable. The bit is the same
         *  with spi1_mem_s_fread_dio.
         */
        uint32_t fdout_dual:1;
        /** faddr_dual : R/W; bitpos: [5]; default: 0;
         *  For SPI1, address phase apply 2 signals. 1: enable 0: disable.  The bit is the same
         *  with spi1_mem_s_fread_dio.
         */
        uint32_t faddr_dual:1;
        /** fdin_quad : R/W; bitpos: [6]; default: 0;
         *  For SPI1, din phase apply 4 signals. 1: enable 0: disable.  The bit is the same
         *  with spi1_mem_s_fread_qio.
         */
        uint32_t fdin_quad:1;
        /** fdout_quad : R/W; bitpos: [7]; default: 0;
         *  For SPI1, dout phase apply 4 signals. 1: enable 0: disable.  The bit is the same
         *  with spi1_mem_s_fread_qio.
         */
        uint32_t fdout_quad:1;
        /** faddr_quad : R/W; bitpos: [8]; default: 0;
         *  For SPI1, address phase apply 4 signals. 1: enable 0: disable.  The bit is the same
         *  with spi1_mem_s_fread_qio.
         */
        uint32_t faddr_quad:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} spi1_mem_s_cache_fctrl_reg_t;

/** Type of flash_waiti_ctrl register
 *  SPI1 wait idle control register
 */
typedef union {
    struct {
        /** waiti_en : R/W; bitpos: [0]; default: 1;
         *  1: The hardware will wait idle after SE/PP/WRSR automatically, and hardware auto
         *  Suspend/Resume can be enabled. 0: The functions of hardware wait idle and auto
         *  Suspend/Resume are not supported.
         */
        uint32_t waiti_en:1;
        /** waiti_dummy : R/W; bitpos: [1]; default: 0;
         *  The dummy phase enable when wait flash idle (RDSR)
         */
        uint32_t waiti_dummy:1;
        /** waiti_addr_en : R/W; bitpos: [2]; default: 0;
         *  1: Output  address 0 in RDSR or read SUS command transfer. 0: Do not send out
         *  address in RDSR or read SUS command transfer.
         */
        uint32_t waiti_addr_en:1;
        /** waiti_addr_cyclelen : R/W; bitpos: [4:3]; default: 0;
         *  When SPI1_MEM_S_WAITI_ADDR_EN is set, the  cycle length of sent out address is
         *  (SPI1_MEM_S_WAITI_ADDR_CYCLELEN[1:0] + 1) SPI  bus clock cycles. It is not active when
         *  SPI1_MEM_S_WAITI_ADDR_EN is cleared.
         */
        uint32_t waiti_addr_cyclelen:2;
        uint32_t reserved_5:4;
        /** waiti_cmd_2b : R/W; bitpos: [9]; default: 0;
         *  1:The wait idle command bit length is 16. 0: The wait idle command bit length is 8.
         */
        uint32_t waiti_cmd_2b:1;
        /** waiti_dummy_cyclelen : R/W; bitpos: [15:10]; default: 0;
         *  The dummy cycle length when wait flash idle(RDSR).
         */
        uint32_t waiti_dummy_cyclelen:6;
        /** waiti_cmd : R/W; bitpos: [31:16]; default: 5;
         *  The command value to wait flash idle(RDSR).
         */
        uint32_t waiti_cmd:16;
    };
    uint32_t val;
} spi1_mem_s_flash_waiti_ctrl_reg_t;

/** Type of flash_sus_ctrl register
 *  SPI1 flash suspend control register
 */
typedef union {
    struct {
        /** flash_per : R/W/SC; bitpos: [0]; default: 0;
         *  program erase resume bit, program erase suspend operation will be triggered when
         *  the bit is set. The bit will be cleared once the operation done.1: enable 0:
         *  disable.
         */
        uint32_t flash_per:1;
        /** flash_pes : R/W/SC; bitpos: [1]; default: 0;
         *  program erase suspend bit, program erase suspend operation will be triggered when
         *  the bit is set. The bit will be cleared once the operation done.1: enable 0:
         *  disable.
         */
        uint32_t flash_pes:1;
        /** flash_per_wait_en : R/W; bitpos: [2]; default: 0;
         *  1: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4 or *128) SPI_CLK cycles after
         *  program erase resume command is sent. 0: SPI1 does not wait after program erase
         *  resume command is sent.
         */
        uint32_t flash_per_wait_en:1;
        /** flash_pes_wait_en : R/W; bitpos: [3]; default: 0;
         *  1: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4 or *128) SPI_CLK cycles after
         *  program erase suspend command is sent. 0: SPI1 does not wait after program erase
         *  suspend command is sent.
         */
        uint32_t flash_pes_wait_en:1;
        /** pes_per_en : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable PES end triggers PER transfer option. If this bit is 0,
         *  application should send PER after PES is done.
         */
        uint32_t pes_per_en:1;
        /** flash_pes_en : R/W; bitpos: [5]; default: 0;
         *  Set this bit to enable Auto-suspending function.
         */
        uint32_t flash_pes_en:1;
        /** pesr_end_msk : R/W; bitpos: [21:6]; default: 128;
         *  The mask value when check SUS/SUS1/SUS2 status bit. If the read status value is
         *  status_in[15:0](only status_in[7:0] is valid when only one byte of data is read
         *  out, status_in[15:0] is valid when two bytes of data are read out), SUS/SUS1/SUS2 =
         *  status_in[15:0]^ SPI1_MEM_S_PESR_END_MSK[15:0].
         */
        uint32_t pesr_end_msk:16;
        /** fmem_rd_sus_2b : R/W; bitpos: [22]; default: 0;
         *  1: Read two bytes when check flash SUS/SUS1/SUS2 status bit. 0:  Read one byte when
         *  check flash SUS/SUS1/SUS2 status bit
         */
        uint32_t fmem_rd_sus_2b:1;
        /** per_end_en : R/W; bitpos: [23]; default: 0;
         *  1: Both WIP and SUS/SUS1/SUS2 bits should be checked to insure the resume status of
         *  flash. 0: Only need to check WIP is 0.
         */
        uint32_t per_end_en:1;
        /** pes_end_en : R/W; bitpos: [24]; default: 0;
         *  1: Both WIP and SUS/SUS1/SUS2 bits should be checked to insure the suspend status
         *  of flash. 0: Only need to check WIP is 0.
         */
        uint32_t pes_end_en:1;
        /** sus_timeout_cnt : R/W; bitpos: [31:25]; default: 4;
         *  When SPI1 checks SUS/SUS1/SUS2 bits fail for SPI1_MEM_S_SUS_TIMEOUT_CNT[6:0] times, it
         *  will be treated as check pass.
         */
        uint32_t sus_timeout_cnt:7;
    };
    uint32_t val;
} spi1_mem_s_flash_sus_ctrl_reg_t;

/** Type of flash_sus_cmd register
 *  SPI1 flash suspend command register
 */
typedef union {
    struct {
        /** flash_pes_command : R/W; bitpos: [15:0]; default: 30069;
         *  Program/Erase suspend command.
         */
        uint32_t flash_pes_command:16;
        /** wait_pesr_command : R/W; bitpos: [31:16]; default: 5;
         *  Flash SUS/SUS1/SUS2 status bit read command. The command should be sent when
         *  SUS/SUS1/SUS2 bit should be checked to insure the suspend or resume status of flash.
         */
        uint32_t wait_pesr_command:16;
    };
    uint32_t val;
} spi1_mem_s_flash_sus_cmd_reg_t;

/** Type of sus_status register
 *  SPI1 flash suspend status register
 */
typedef union {
    struct {
        /** flash_sus : R/W/SS/SC; bitpos: [0]; default: 0;
         *  The status of flash suspend, only used in SPI1.
         */
        uint32_t flash_sus:1;
        /** wait_pesr_cmd_2b : R/W; bitpos: [1]; default: 0;
         *  1: SPI1 sends out SPI1_MEM_S_WAIT_PESR_COMMAND[15:0] to check SUS/SUS1/SUS2 bit. 0:
         *  SPI1 sends out SPI1_MEM_S_WAIT_PESR_COMMAND[7:0] to check SUS/SUS1/SUS2 bit.
         */
        uint32_t wait_pesr_cmd_2b:1;
        /** flash_hpm_dly_128 : R/W; bitpos: [2]; default: 0;
         *  1: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after HPM
         *  command is sent. 0: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles
         *  after HPM command is sent.
         */
        uint32_t flash_hpm_dly_128:1;
        /** flash_res_dly_128 : R/W; bitpos: [3]; default: 0;
         *  1: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after RES
         *  command is sent. 0: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles
         *  after RES command is sent.
         */
        uint32_t flash_res_dly_128:1;
        /** flash_dp_dly_128 : R/W; bitpos: [4]; default: 0;
         *  1: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after DP
         *  command is sent. 0: SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles
         *  after DP command is sent.
         */
        uint32_t flash_dp_dly_128:1;
        /** flash_per_dly_128 : R/W; bitpos: [5]; default: 0;
         *  Valid when SPI1_MEM_S_FLASH_PER_WAIT_EN is 1. 1: SPI1 waits
         *  (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after PER command is sent. 0:
         *  SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PER command is
         *  sent.
         */
        uint32_t flash_per_dly_128:1;
        /** flash_pes_dly_128 : R/W; bitpos: [6]; default: 0;
         *  Valid when SPI1_MEM_S_FLASH_PES_WAIT_EN is 1. 1: SPI1 waits
         *  (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 128) SPI_CLK cycles after PES command is sent. 0:
         *  SPI1 waits (SPI1_MEM_S_CS_HOLD_DELAY_RES[9:0] * 4) SPI_CLK cycles after PES command is
         *  sent.
         */
        uint32_t flash_pes_dly_128:1;
        /** spi0_lock_en : R/W; bitpos: [7]; default: 0;
         *  1: Enable SPI0 lock SPI0/1 arbiter option. 0: Disable it.
         */
        uint32_t spi0_lock_en:1;
        uint32_t reserved_8:7;
        /** flash_pesr_cmd_2b : R/W; bitpos: [15]; default: 0;
         *  1: The bit length of Program/Erase Suspend/Resume command is 16. 0: The bit length
         *  of Program/Erase Suspend/Resume command is 8.
         */
        uint32_t flash_pesr_cmd_2b:1;
        /** flash_per_command : R/W; bitpos: [31:16]; default: 31354;
         *  Program/Erase resume command.
         */
        uint32_t flash_per_command:16;
    };
    uint32_t val;
} spi1_mem_s_sus_status_reg_t;

/** Type of ddr register
 *  SPI1 DDR control register
 */
typedef union {
    struct {
        /** fmem_ddr_en : R/W; bitpos: [0]; default: 0;
         *  1: in ddr mode,  0 in sdr mode
         */
        uint32_t fmem_ddr_en:1;
        /** fmem_var_dummy : R/W; bitpos: [1]; default: 0;
         *  Set the bit to enable variable dummy cycle in spi ddr mode.
         */
        uint32_t fmem_var_dummy:1;
        /** fmem_ddr_rdat_swp : R/W; bitpos: [2]; default: 0;
         *  Set the bit to reorder rx data of the word in spi ddr mode.
         */
        uint32_t fmem_ddr_rdat_swp:1;
        /** fmem_ddr_wdat_swp : R/W; bitpos: [3]; default: 0;
         *  Set the bit to reorder tx data of the word in spi ddr mode.
         */
        uint32_t fmem_ddr_wdat_swp:1;
        /** fmem_ddr_cmd_dis : R/W; bitpos: [4]; default: 0;
         *  the bit is used to disable dual edge in command phase when ddr mode.
         */
        uint32_t fmem_ddr_cmd_dis:1;
        /** fmem_outminbytelen : R/W; bitpos: [11:5]; default: 1;
         *  It is the minimum output data length in the panda device.
         */
        uint32_t fmem_outminbytelen:7;
        uint32_t reserved_12:2;
        /** fmem_usr_ddr_dqs_thd : R/W; bitpos: [20:14]; default: 0;
         *  The delay number of data strobe which from memory based on SPI clock.
         */
        uint32_t fmem_usr_ddr_dqs_thd:7;
        /** fmem_ddr_dqs_loop : R/W; bitpos: [21]; default: 0;
         *  1: Do not need the input of SPI_DQS signal, SPI0 starts to receive data when
         *  spi0_slv_st is in SPI1_MEM_S_DIN state. It is used when there is no SPI_DQS signal or
         *  SPI_DQS signal is not stable. 0: SPI0 starts to store data at the positive and
         *  negative edge of SPI_DQS.
         */
        uint32_t fmem_ddr_dqs_loop:1;
        uint32_t reserved_22:2;
        /** fmem_clk_diff_en : R/W; bitpos: [24]; default: 0;
         *  Set this bit to enable the differential SPI_CLK#.
         */
        uint32_t fmem_clk_diff_en:1;
        uint32_t reserved_25:1;
        /** fmem_dqs_ca_in : R/W; bitpos: [26]; default: 0;
         *  Set this bit to enable the input of SPI_DQS signal in SPI phases of CMD and ADDR.
         */
        uint32_t fmem_dqs_ca_in:1;
        /** fmem_hyperbus_dummy_2x : R/W; bitpos: [27]; default: 0;
         *  Set this bit to enable the vary dummy function in SPI HyperBus mode, when SPI0
         *  accesses flash or SPI1 accesses flash or sram.
         */
        uint32_t fmem_hyperbus_dummy_2x:1;
        /** fmem_clk_diff_inv : R/W; bitpos: [28]; default: 0;
         *  Set this bit to invert SPI_DIFF when accesses to flash. .
         */
        uint32_t fmem_clk_diff_inv:1;
        /** fmem_octa_ram_addr : R/W; bitpos: [29]; default: 0;
         *  Set this bit to enable octa_ram address out when accesses to flash, which means
         *  ADDR_OUT[31:0] = {spi_usr_addr_value[25:4], 6'd0, spi_usr_addr_value[3:1], 1'b0}.
         */
        uint32_t fmem_octa_ram_addr:1;
        /** fmem_hyperbus_ca : R/W; bitpos: [30]; default: 0;
         *  Set this bit to enable HyperRAM address out when accesses to flash, which means
         *  ADDR_OUT[31:0] = {spi_usr_addr_value[19:4], 13'd0, spi_usr_addr_value[3:1]}.
         */
        uint32_t fmem_hyperbus_ca:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} spi1_mem_s_ddr_reg_t;

/** Type of clock_gate register
 *  SPI1 clk_gate register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Register clock gate enable signal. 1: Enable. 0: Disable.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} spi1_mem_s_clock_gate_reg_t;


/** Group: Status register */
/** Type of tx_crc register
 *  SPI1 TX CRC data register.
 */
typedef union {
    struct {
        /** tx_crc_data : RO; bitpos: [31:0]; default: 4294967295;
         *  For SPI1, the value of crc32.
         */
        uint32_t tx_crc_data:32;
    };
    uint32_t val;
} spi1_mem_s_tx_crc_reg_t;


/** Group: Memory data buffer register */
/** Type of w0 register
 *  SPI1 memory data buffer0
 */
typedef union {
    struct {
        /** buf0 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf0:32;
    };
    uint32_t val;
} spi1_mem_s_w0_reg_t;

/** Type of w1 register
 *  SPI1 memory data buffer1
 */
typedef union {
    struct {
        /** buf1 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf1:32;
    };
    uint32_t val;
} spi1_mem_s_w1_reg_t;

/** Type of w2 register
 *  SPI1 memory data buffer2
 */
typedef union {
    struct {
        /** buf2 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf2:32;
    };
    uint32_t val;
} spi1_mem_s_w2_reg_t;

/** Type of w3 register
 *  SPI1 memory data buffer3
 */
typedef union {
    struct {
        /** buf3 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf3:32;
    };
    uint32_t val;
} spi1_mem_s_w3_reg_t;

/** Type of w4 register
 *  SPI1 memory data buffer4
 */
typedef union {
    struct {
        /** buf4 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf4:32;
    };
    uint32_t val;
} spi1_mem_s_w4_reg_t;

/** Type of w5 register
 *  SPI1 memory data buffer5
 */
typedef union {
    struct {
        /** buf5 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf5:32;
    };
    uint32_t val;
} spi1_mem_s_w5_reg_t;

/** Type of w6 register
 *  SPI1 memory data buffer6
 */
typedef union {
    struct {
        /** buf6 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf6:32;
    };
    uint32_t val;
} spi1_mem_s_w6_reg_t;

/** Type of w7 register
 *  SPI1 memory data buffer7
 */
typedef union {
    struct {
        /** buf7 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf7:32;
    };
    uint32_t val;
} spi1_mem_s_w7_reg_t;

/** Type of w8 register
 *  SPI1 memory data buffer8
 */
typedef union {
    struct {
        /** buf8 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf8:32;
    };
    uint32_t val;
} spi1_mem_s_w8_reg_t;

/** Type of w9 register
 *  SPI1 memory data buffer9
 */
typedef union {
    struct {
        /** buf9 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf9:32;
    };
    uint32_t val;
} spi1_mem_s_w9_reg_t;

/** Type of w10 register
 *  SPI1 memory data buffer10
 */
typedef union {
    struct {
        /** buf10 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf10:32;
    };
    uint32_t val;
} spi1_mem_s_w10_reg_t;

/** Type of w11 register
 *  SPI1 memory data buffer11
 */
typedef union {
    struct {
        /** buf11 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf11:32;
    };
    uint32_t val;
} spi1_mem_s_w11_reg_t;

/** Type of w12 register
 *  SPI1 memory data buffer12
 */
typedef union {
    struct {
        /** buf12 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf12:32;
    };
    uint32_t val;
} spi1_mem_s_w12_reg_t;

/** Type of w13 register
 *  SPI1 memory data buffer13
 */
typedef union {
    struct {
        /** buf13 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf13:32;
    };
    uint32_t val;
} spi1_mem_s_w13_reg_t;

/** Type of w14 register
 *  SPI1 memory data buffer14
 */
typedef union {
    struct {
        /** buf14 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf14:32;
    };
    uint32_t val;
} spi1_mem_s_w14_reg_t;

/** Type of w15 register
 *  SPI1 memory data buffer15
 */
typedef union {
    struct {
        /** buf15 : R/W/SS; bitpos: [31:0]; default: 0;
         *  data buffer
         */
        uint32_t buf15:32;
    };
    uint32_t val;
} spi1_mem_s_w15_reg_t;


/** Group: Interrupt registers */
/** Type of int_ena register
 *  SPI1 interrupt enable register
 */
typedef union {
    struct {
        /** per_end_int_ena : R/W; bitpos: [0]; default: 0;
         *  The enable bit for SPI1_MEM_S_PER_END_INT interrupt.
         */
        uint32_t per_end_int_ena:1;
        /** pes_end_int_ena : R/W; bitpos: [1]; default: 0;
         *  The enable bit for SPI1_MEM_S_PES_END_INT interrupt.
         */
        uint32_t pes_end_int_ena:1;
        /** wpe_end_int_ena : R/W; bitpos: [2]; default: 0;
         *  The enable bit for SPI1_MEM_S_WPE_END_INT interrupt.
         */
        uint32_t wpe_end_int_ena:1;
        /** slv_st_end_int_ena : R/W; bitpos: [3]; default: 0;
         *  The enable bit for SPI1_MEM_S_SLV_ST_END_INT interrupt.
         */
        uint32_t slv_st_end_int_ena:1;
        /** mst_st_end_int_ena : R/W; bitpos: [4]; default: 0;
         *  The enable bit for SPI1_MEM_S_MST_ST_END_INT interrupt.
         */
        uint32_t mst_st_end_int_ena:1;
        uint32_t reserved_5:5;
        /** brown_out_int_ena : R/W; bitpos: [10]; default: 0;
         *  The enable bit for SPI1_MEM_S_BROWN_OUT_INT interrupt.
         */
        uint32_t brown_out_int_ena:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} spi1_mem_s_int_ena_reg_t;

/** Type of int_clr register
 *  SPI1 interrupt clear register
 */
typedef union {
    struct {
        /** per_end_int_clr : WT; bitpos: [0]; default: 0;
         *  The clear bit for SPI1_MEM_S_PER_END_INT interrupt.
         */
        uint32_t per_end_int_clr:1;
        /** pes_end_int_clr : WT; bitpos: [1]; default: 0;
         *  The clear bit for SPI1_MEM_S_PES_END_INT interrupt.
         */
        uint32_t pes_end_int_clr:1;
        /** wpe_end_int_clr : WT; bitpos: [2]; default: 0;
         *  The clear bit for SPI1_MEM_S_WPE_END_INT interrupt.
         */
        uint32_t wpe_end_int_clr:1;
        /** slv_st_end_int_clr : WT; bitpos: [3]; default: 0;
         *  The clear bit for SPI1_MEM_S_SLV_ST_END_INT interrupt.
         */
        uint32_t slv_st_end_int_clr:1;
        /** mst_st_end_int_clr : WT; bitpos: [4]; default: 0;
         *  The clear bit for SPI1_MEM_S_MST_ST_END_INT interrupt.
         */
        uint32_t mst_st_end_int_clr:1;
        uint32_t reserved_5:5;
        /** brown_out_int_clr : WT; bitpos: [10]; default: 0;
         *  The status bit for SPI1_MEM_S_BROWN_OUT_INT interrupt.
         */
        uint32_t brown_out_int_clr:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} spi1_mem_s_int_clr_reg_t;

/** Type of int_raw register
 *  SPI1 interrupt raw register
 */
typedef union {
    struct {
        /** per_end_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw bit for SPI1_MEM_S_PER_END_INT interrupt. 1: Triggered when Auto Resume
         *  command (0x7A) is sent and flash is resumed successfully. 0: Others.
         */
        uint32_t per_end_int_raw:1;
        /** pes_end_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw bit for SPI1_MEM_S_PES_END_INT interrupt.1: Triggered when Auto Suspend
         *  command (0x75) is sent and flash is suspended successfully. 0: Others.
         */
        uint32_t pes_end_int_raw:1;
        /** wpe_end_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw bit for SPI1_MEM_S_WPE_END_INT interrupt. 1: Triggered when WRSR/PP/SE/BE/CE
         *  is sent and flash is already idle. 0: Others.
         */
        uint32_t wpe_end_int_raw:1;
        /** slv_st_end_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw bit for SPI1_MEM_S_SLV_ST_END_INT interrupt. 1: Triggered when spi1_slv_st is
         *  changed from non idle state to idle state. It means that SPI_CS raises high. 0:
         *  Others
         */
        uint32_t slv_st_end_int_raw:1;
        /** mst_st_end_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw bit for SPI1_MEM_S_MST_ST_END_INT interrupt. 1: Triggered when spi1_mst_st is
         *  changed from non idle state to idle state. 0: Others.
         */
        uint32_t mst_st_end_int_raw:1;
        uint32_t reserved_5:5;
        /** brown_out_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw bit for SPI1_MEM_S_BROWN_OUT_INT interrupt. 1: Triggered condition is that
         *  chip is loosing power and RTC module sends out brown out close flash request to
         *  SPI1. After SPI1 sends out suspend command to flash, this interrupt is triggered
         *  and MSPI returns to idle state. 0: Others.
         */
        uint32_t brown_out_int_raw:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} spi1_mem_s_int_raw_reg_t;

/** Type of int_st register
 *  SPI1 interrupt status register
 */
typedef union {
    struct {
        /** per_end_int_st : RO; bitpos: [0]; default: 0;
         *  The status bit for SPI1_MEM_S_PER_END_INT interrupt.
         */
        uint32_t per_end_int_st:1;
        /** pes_end_int_st : RO; bitpos: [1]; default: 0;
         *  The status bit for SPI1_MEM_S_PES_END_INT interrupt.
         */
        uint32_t pes_end_int_st:1;
        /** wpe_end_int_st : RO; bitpos: [2]; default: 0;
         *  The status bit for SPI1_MEM_S_WPE_END_INT interrupt.
         */
        uint32_t wpe_end_int_st:1;
        /** slv_st_end_int_st : RO; bitpos: [3]; default: 0;
         *  The status bit for SPI1_MEM_S_SLV_ST_END_INT interrupt.
         */
        uint32_t slv_st_end_int_st:1;
        /** mst_st_end_int_st : RO; bitpos: [4]; default: 0;
         *  The status bit for SPI1_MEM_S_MST_ST_END_INT interrupt.
         */
        uint32_t mst_st_end_int_st:1;
        uint32_t reserved_5:5;
        /** brown_out_int_st : RO; bitpos: [10]; default: 0;
         *  The status bit for SPI1_MEM_S_BROWN_OUT_INT interrupt.
         */
        uint32_t brown_out_int_st:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} spi1_mem_s_int_st_reg_t;


/** Group: Timing registers */
/** Type of timing_cali register
 *  SPI1 timing control register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** timing_cali : R/W; bitpos: [1]; default: 0;
         *  The bit is used to enable timing auto-calibration for all reading operations.
         */
        uint32_t timing_cali:1;
        /** extra_dummy_cyclelen : R/W; bitpos: [4:2]; default: 0;
         *  add extra dummy spi clock cycle length for spi clock calibration.
         */
        uint32_t extra_dummy_cyclelen:3;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} spi1_mem_s_timing_cali_reg_t;


/** Group: Version register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 34673216;
         *  Version control register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} spi1_mem_s_date_reg_t;


typedef struct spi1_mem_s_dev_s {
    volatile spi1_mem_s_cmd_reg_t cmd;
    volatile spi1_mem_s_addr_reg_t addr;
    volatile spi1_mem_s_ctrl_reg_t ctrl;
    volatile spi1_mem_s_ctrl1_reg_t ctrl1;
    volatile spi1_mem_s_ctrl2_reg_t ctrl2;
    volatile spi1_mem_s_clock_reg_t clock;
    volatile spi1_mem_s_user_reg_t user;
    volatile spi1_mem_s_user1_reg_t user1;
    volatile spi1_mem_s_user2_reg_t user2;
    volatile spi1_mem_s_mosi_dlen_reg_t mosi_dlen;
    volatile spi1_mem_s_miso_dlen_reg_t miso_dlen;
    volatile spi1_mem_s_rd_status_reg_t rd_status;
    uint32_t reserved_030;
    volatile spi1_mem_s_misc_reg_t misc;
    volatile spi1_mem_s_tx_crc_reg_t tx_crc;
    volatile spi1_mem_s_cache_fctrl_reg_t cache_fctrl;
    uint32_t reserved_040[6];
    volatile spi1_mem_s_w0_reg_t w0;
    volatile spi1_mem_s_w1_reg_t w1;
    volatile spi1_mem_s_w2_reg_t w2;
    volatile spi1_mem_s_w3_reg_t w3;
    volatile spi1_mem_s_w4_reg_t w4;
    volatile spi1_mem_s_w5_reg_t w5;
    volatile spi1_mem_s_w6_reg_t w6;
    volatile spi1_mem_s_w7_reg_t w7;
    volatile spi1_mem_s_w8_reg_t w8;
    volatile spi1_mem_s_w9_reg_t w9;
    volatile spi1_mem_s_w10_reg_t w10;
    volatile spi1_mem_s_w11_reg_t w11;
    volatile spi1_mem_s_w12_reg_t w12;
    volatile spi1_mem_s_w13_reg_t w13;
    volatile spi1_mem_s_w14_reg_t w14;
    volatile spi1_mem_s_w15_reg_t w15;
    volatile spi1_mem_s_flash_waiti_ctrl_reg_t flash_waiti_ctrl;
    volatile spi1_mem_s_flash_sus_ctrl_reg_t flash_sus_ctrl;
    volatile spi1_mem_s_flash_sus_cmd_reg_t flash_sus_cmd;
    volatile spi1_mem_s_sus_status_reg_t sus_status;
    uint32_t reserved_0a8[6];
    volatile spi1_mem_s_int_ena_reg_t int_ena;
    volatile spi1_mem_s_int_clr_reg_t int_clr;
    volatile spi1_mem_s_int_raw_reg_t int_raw;
    volatile spi1_mem_s_int_st_reg_t int_st;
    uint32_t reserved_0d0;
    volatile spi1_mem_s_ddr_reg_t ddr;
    uint32_t reserved_0d8[42];
    volatile spi1_mem_s_timing_cali_reg_t timing_cali;
    uint32_t reserved_184[31];
    volatile spi1_mem_s_clock_gate_reg_t clock_gate;
    uint32_t reserved_204[126];
    volatile spi1_mem_s_date_reg_t date;
} spi1_mem_s_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(spi1_mem_s_dev_t) == 0x400, "Invalid size of spi1_mem_s_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
