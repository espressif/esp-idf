// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_SPI_STRUCT_H_
#define _SOC_SPI_STRUCT_H_
typedef struct {
    union {
        struct {
            volatile uint32_t reserved0: 16;                         /*reserved*/
            volatile uint32_t flash_per:  1;                         /*program erase resume bit  program erase suspend operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t flash_pes:  1;                         /*program erase suspend bit  program erase suspend operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t usr:        1;                         /*User define command enable.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t flash_hpm:  1;                         /*Drive Flash into high performance mode.  The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t flash_res:  1;                         /*This bit combined with reg_resandres bit releases Flash from the power-down state or high performance mode and obtains the devices ID. The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t flash_dp:   1;                         /*Drive Flash into power down.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t flash_ce:   1;                         /*Chip erase enable. Chip erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t flash_be:   1;                         /*Block erase enable(32KB) .  Block erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t flash_se:   1;                         /*Sector erase enable(4KB). Sector erase operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t flash_pp:   1;                         /*Page program enable(1 byte ~256 bytes data to be programmed). Page program operation  will be triggered when the bit is set. The bit will be cleared once the operation done .1: enable 0: disable.*/
            volatile uint32_t flash_wrsr: 1;                         /*Write status register enable.   Write status operation  will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t flash_rdsr: 1;                         /*Read status register-1.  Read status operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable.*/
            volatile uint32_t flash_rdid: 1;                         /*Read JEDEC ID . Read ID command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
            volatile uint32_t flash_wrdi: 1;                         /*Write flash disable. Write disable command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
            volatile uint32_t flash_wren: 1;                         /*Write flash enable.  Write enable command will be sent when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
            volatile uint32_t flash_read: 1;                         /*Read flash enable. Read flash operation will be triggered when the bit is set. The bit will be cleared once the operation done. 1: enable 0: disable.*/
        };
        volatile uint32_t val;
    }cmd;
    union {
        struct {
            volatile uint32_t reserved      : 8;
            volatile uint32_t usr_addr_value:24;                     /*[31:8]:address to slave [7:0]:Reserved.*/
        };
        volatile uint32_t val;
    }addr;
    union {
        struct {
            volatile uint32_t reserved0:         10;                 /*reserved*/
            volatile uint32_t fcs_crc_en:         1;                 /*For SPI1  initialize crc32 module before writing encrypted data to flash. Active low.*/
            volatile uint32_t tx_crc_en:          1;                 /*For SPI1  enable crc32 when writing encrypted data to flash. 1: enable  0:disable*/
            volatile uint32_t wait_flash_idle_en: 1;                 /*wait flash idle when program flash or erase flash. 1: enable 0: disable.*/
            volatile uint32_t fastrd_mode:        1;                 /*This bit enable the bits: spi_fread_qio  spi_fread_dio  spi_fread_qout and spi_fread_dout. 1: enable 0: disable.*/
            volatile uint32_t fread_dual:         1;                 /*In the read operations  read-data phase apply 2 signals. 1: enable 0: disable.*/
            volatile uint32_t resandres:          1;                 /*The Device ID is read out to SPI_RD_STATUS register, this bit combine with spi_flash_res bit. 1: enable 0: disable.*/
            volatile uint32_t reserved16:         4;                 /*reserved*/
            volatile uint32_t fread_quad:         1;                 /*In the read operations read-data phase apply 4 signals. 1: enable 0: disable.*/
            volatile uint32_t wp:                 1;                 /*Write protect signal output when SPI is idle.  1: output high  0: output low.*/
            volatile uint32_t wrsr_2b:            1;                 /*two bytes data will be written to status register when it is set. 1: enable 0: disable.*/
            volatile uint32_t fread_dio:          1;                 /*In the read operations address phase and read-data phase apply 2 signals. 1: enable 0: disable.*/
            volatile uint32_t fread_qio:          1;                 /*In the read operations address phase and read-data phase apply 4 signals. 1: enable 0: disable.*/
            volatile uint32_t rd_bit_order:       1;                 /*In read-data (MISO) phase 1: LSB first 0: MSB first*/
            volatile uint32_t wr_bit_order:       1;                 /*In command address write-data (MOSI) phases 1: LSB firs 0: MSB first*/
            volatile uint32_t reserved27:         5;                 /*reserved*/
        };
        volatile uint32_t val;
    }ctrl;
    union {
        struct {
            volatile uint32_t reserved0:        16;                  /*reserved*/
            volatile uint32_t cs_hold_delay_res:12;                  /*Delay cycles of resume Flash when resume Flash is enable by spi clock.*/
            volatile uint32_t cs_hold_delay:     4;                  /*SPI cs signal is delayed by spi clock cycles*/
        };
        volatile uint32_t val;
    }ctrl1;
    union {
        struct {
            volatile uint32_t status:    16;                         /*In the slave mode, it is the status for master to read out.*/
            volatile uint32_t wb_mode:    8;                         /*Mode bits in the flash fast read mode, it is combined with spi_fastrd_mode bit.*/
            volatile uint32_t status_ext: 8;                         /*In the slave mode,it is the status for master to read out.*/
        };
        volatile uint32_t val;
    }rd_status;
    union {
        struct {
            volatile uint32_t setup_time:       4;                   /*(cycles-1) of ,prepare, phase by spi clock, this bits combined with spi_cs_setup bit.*/
            volatile uint32_t hold_time:        4;                   /*delay cycles of cs pin by spi clock, this bits combined with spi_cs_hold bit.*/
            volatile uint32_t ck_out_low_mode:  4;                   /*modify spi clock duty ratio when the value is lager than 8, the bits are combined with spi_clkcnt_N bits and spi_clkcnt_L bits.*/
            volatile uint32_t ck_out_high_mode: 4;                   /*modify spi clock duty ratio when the value is lager than 8, the bits are combined with spi_clkcnt_N bits and spi_clkcnt_H bits.*/
            volatile uint32_t miso_delay_mode:  2;                   /*MISO signals are delayed by spi_clk. 0: zero  1: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by half cycle    else delayed by one cycle  2: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by one cycle  else delayed by half cycle  3: delayed one cycle*/
            volatile uint32_t miso_delay_num:   3;                   /*MISO signals are delayed by system clock cycles*/
            volatile uint32_t mosi_delay_mode:  2;                   /*MOSI signals are delayed by spi_clk. 0: zero  1: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by half cycle    else delayed by one cycle  2: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by one cycle  else delayed by half cycle  3: delayed one cycle*/
            volatile uint32_t mosi_delay_num:   3;                   /*MOSI signals are delayed by system clock cycles*/
            volatile uint32_t cs_delay_mode:    2;                   /*spi_cs signal is delayed by spi_clk . 0: zero  1: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by half cycle    else delayed by one cycle  2: if spi_ck_out_edge or spi_ck_i_edge is set 1  delayed by one cycle   else delayed by half cycle  3: delayed one cycle*/
            volatile uint32_t cs_delay_num:     4;                   /*spi_cs signal is delayed by system clock cycles*/
        };
        volatile uint32_t val;
    }ctrl2;
    union {
        struct {
            volatile uint32_t clkcnt_l:       6;                     /*In the master mode it must be equal to spi_clkcnt_N. In the slave mode it must be 0.*/
            volatile uint32_t clkcnt_h:       6;                     /*In the master mode it must be floor((spi_clkcnt_N+1)/2-1). In the slave mode it must be 0.*/
            volatile uint32_t clkcnt_n:       6;                     /*In the master mode it is the divider of spi_clk. So spi_clk frequency is system/(spi_clkdiv_pre+1)/(spi_clkcnt_N+1)*/
            volatile uint32_t clkdiv_pre:    13;                     /*In the master mode it is pre-divider of spi_clk.*/
            volatile uint32_t clk_equ_sysclk: 1;                     /*In the master mode 1: spi_clk is eqaul to system 0: spi_clk is divided from system clock.*/
        };
        volatile uint32_t val;
    }clock;
    union {
        struct {
            volatile uint32_t doutdin:           1;                  /*Set the bit to enable full duplex communication. 1: enable 0: disable.*/
            volatile uint32_t reserved1:         3;                  /*reserved*/
            volatile uint32_t cs_hold:           1;                  /*spi cs keep low when spi is in ,done, phase. 1: enable 0: disable.*/
            volatile uint32_t cs_setup:          1;                  /*spi cs is enable when spi is in ,prepare, phase. 1: enable 0: disable.*/
            volatile uint32_t ck_i_edge:         1;                  /*In the slave mode  the bit is same as spi_ck_out_edge in master mode. It is combined with  spi_miso_delay_mode bits.*/
            volatile uint32_t ck_out_edge:       1;                  /*the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode.*/
            volatile uint32_t reserved8:         2;                  /*reserved*/
            volatile uint32_t rd_byte_order:     1;                  /*In read-data (MISO) phase 1: big-endian 0: little_endian*/
            volatile uint32_t wr_byte_order:     1;                  /*In command address write-data (MOSI) phases 1: big-endian 0: litte_endian*/
            volatile uint32_t fwrite_dual:       1;                  /*In the write operations read-data phase apply 2 signals*/
            volatile uint32_t fwrite_quad:       1;                  /*In the write operations read-data phase apply 4 signals*/
            volatile uint32_t fwrite_dio:        1;                  /*In the write operations address phase and read-data phase apply 2 signals.*/
            volatile uint32_t fwrite_qio:        1;                  /*In the write operations address phase and read-data phase apply 4 signals.*/
            volatile uint32_t sio:               1;                  /*Set the bit to enable 3-line half duplex communication  mosi and miso signals share the same pin. 1: enable 0: disable.*/
            volatile uint32_t usr_hold_pol:      1;                  /*It is combined with hold bits to set the polarity of spi hold line  1: spi will be held when spi hold line is high  0: spi will be held when spi hold line is low*/
            volatile uint32_t usr_dout_hold:     1;                  /*spi is hold at data out state  the bit combined with spi_usr_hold_pol bit.*/
            volatile uint32_t usr_din_hold:      1;                  /*spi is hold at data in state  the bit combined with spi_usr_hold_pol bit.*/
            volatile uint32_t usr_dummy_hold:    1;                  /*spi is hold at dummy state  the bit combined with spi_usr_hold_pol bit.*/
            volatile uint32_t usr_addr_hold:     1;                  /*spi is hold at address state  the bit combined with spi_usr_hold_pol bit.*/
            volatile uint32_t usr_cmd_hold:      1;                  /*spi is hold at command state  the bit combined with spi_usr_hold_pol bit.*/
            volatile uint32_t usr_prep_hold:     1;                  /*spi is hold at prepare state  the bit combined with spi_usr_hold_pol bit.*/
            volatile uint32_t usr_miso_highpart: 1;                  /*read-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable.*/
            volatile uint32_t usr_mosi_highpart: 1;                  /*write-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable.*/
            volatile uint32_t usr_dummy_idle:    1;                  /*spi clock is disable in dummy phase when the bit is enable.*/
            volatile uint32_t usr_mosi:          1;                  /*This bit enable the write-data phase of an operation.*/
            volatile uint32_t usr_miso:          1;                  /*This bit enable the read-data phase of an operation.*/
            volatile uint32_t usr_dummy:         1;                  /*This bit enable the dummy phase of an operation.*/
            volatile uint32_t usr_addr:          1;                  /*This bit enable the address phase of an operation.*/
            volatile uint32_t usr_command:       1;                  /*This bit enable the command phase of an operation.*/
        };
        volatile uint32_t val;
    }user;
    union {
        struct {
            volatile uint32_t usr_dummy_cyclelen: 8;                 /*The length in spi_clk cycles of dummy phase. The register value shall be (cycle_num-1).*/
            volatile uint32_t reserved8:         18;                 /*reserved*/
            volatile uint32_t usr_addr_bitlen:    6;                 /*The length in bits of address phase. The register value shall be (bit_num-1).*/
        };
        volatile uint32_t val;
    }user1;
    union {
        struct {
            volatile uint32_t usr_command_value: 16;                 /*The value of  command.*/
            volatile uint32_t reserved16:        12;                 /*reserved*/
            volatile uint32_t usr_command_bitlen: 4;                 /*The length in bits of command phase. The register value shall be (bit_num-1)*/
        };
        volatile uint32_t val;
    }user2;
    union {
        struct {
            volatile uint32_t usr_mosi_dbitlen:24;                   /*The length in bits of write-data. The register value shall be (bit_num-1).*/
            volatile uint32_t reserved24:       8;                   /*reserved*/
        };
        volatile uint32_t val;
    }mosi_dlen;
    union {
        struct {
            volatile uint32_t usr_miso_dbitlen:24;                   /*The length in bits of  read-data. The register value shall be (bit_num-1).*/
            volatile uint32_t reserved24:       8;                   /*reserved*/
        };
        volatile uint32_t val;
    }miso_dlen;
    volatile uint32_t slv_wr_status;                                 /*In the slave mode this register are the status register for the master to write into. In the master mode this register are the higher 32bits in the 64 bits address condition.*/
    union {
        struct {
            volatile uint32_t cs0_dis:        1;                     /*SPI CS0 pin enable, 1: disable CS0, 0: spi_cs0 signal is from/to CS0 pin*/
            volatile uint32_t cs1_dis:        1;                     /*SPI CS1 pin enable, 1: disable CS1, 0: spi_cs1 signal is from/to CS1 pin*/
            volatile uint32_t cs2_dis:        1;                     /*SPI CS2 pin enable, 1: disable CS2, 0: spi_cs2 signal is from/to CS2 pin*/
            volatile uint32_t reserved3:      2;                     /*reserved*/
            volatile uint32_t ck_dis:         1;                     /*1: spi clk out disable  0: spi clk out enable*/
            volatile uint32_t master_cs_pol:  5;                     /*In the master mode  the bits are the polarity of spi cs line  the value is equivalent to spi_cs ^ spi_master_cs_pol.*/
            volatile uint32_t master_ck_sel:  5;                     /*In the master mode  spi cs line is enable as spi clk  it is combined with spi_cs0_dis spi_cs1_dis spi_cs2_dis.*/
            volatile uint32_t reserved16:    13;                     /*reserved*/
            volatile uint32_t ck_idle_edge:   1;                     /*1: spi clk line is high when idle     0: spi clk line is low when idle*/
            volatile uint32_t cs_keep_active: 1;                     /*spi cs line keep low when the bit is set.*/
            volatile uint32_t reserved31:     1;                     /*reserved*/
        };
        volatile uint32_t val;
    }pin;
    union {
        struct {
            volatile uint32_t slv_rd_buf_done:  1;                   /*The interrupt raw bit for the completion of read-buffer operation in the slave mode.*/
            volatile uint32_t slv_wr_buf_done:  1;                   /*The interrupt raw bit for the completion of write-buffer operation in the slave mode.*/
            volatile uint32_t slv_rd_sta_done:  1;                   /*The interrupt raw bit for the completion of read-status operation in the slave mode.*/
            volatile uint32_t slv_wr_sta_done:  1;                   /*The interrupt raw bit for the completion of write-status operation in the slave mode.*/
            volatile uint32_t trans_done:       1;                   /*The interrupt raw bit for the completion of any operation in both the master mode and the slave mode.*/
            volatile uint32_t int_en:           5;                   /*Interrupt enable bits for the below 5 sources*/
            volatile uint32_t cs_i_mode:        2;                   /*In the slave mode  this bits used to synchronize the input spi cs signal and eliminate spi cs  jitter.*/
            volatile uint32_t reserved12:       5;                   /*reserved*/
            volatile uint32_t slv_last_command: 3;                   /*In the slave mode it is the value of command.*/
            volatile uint32_t slv_last_state:   3;                   /*In the slave mode it is the state of spi state machine.*/
            volatile uint32_t trans_cnt:        4;                   /*The operations counter in both the master mode and the slave mode. 4: read-status*/
            volatile uint32_t slv_cmd_define:   1;                   /*1: slave mode commands are defined in SPI_SLAVE3.  0: slave mode commands are fixed as: 1: write-status 2: write-buffer and 3: read-buffer.*/
            volatile uint32_t slv_wr_rd_sta_en: 1;                   /*write and read status enable  in the slave mode*/
            volatile uint32_t slv_wr_rd_buf_en: 1;                   /*write and read buffer enable in the slave mode*/
            volatile uint32_t slave_mode:       1;                   /*1: slave mode 0: master mode.*/
            volatile uint32_t sync_reset:       1;                   /*Software reset enable, reset the spi clock line cs line and data lines.*/
        };
        volatile uint32_t val;
    }slave;
    union {
        struct {
            volatile uint32_t slv_rdbuf_dummy_en:  1;                /*In the slave mode it is the enable bit of dummy phase for read-buffer operations.*/
            volatile uint32_t slv_wrbuf_dummy_en:  1;                /*In the slave mode it is the enable bit of dummy phase for write-buffer operations.*/
            volatile uint32_t slv_rdsta_dummy_en:  1;                /*In the slave mode it is the enable bit of dummy phase for read-status operations.*/
            volatile uint32_t slv_wrsta_dummy_en:  1;                /*In the slave mode it is the enable bit of dummy phase for write-status operations.*/
            volatile uint32_t slv_wr_addr_bitlen:  6;                /*In the slave mode it is the address length in bits for write-buffer operation. The register value shall be (bit_num-1).*/
            volatile uint32_t slv_rd_addr_bitlen:  6;                /*In the slave mode it is the address length in bits for read-buffer operation. The register value shall be (bit_num-1).*/
            volatile uint32_t reserved16:          9;                /*reserved*/
            volatile uint32_t slv_status_readback: 1;                /*In the slave mode  1:read register of SPI_SLV_WR_STATUS  0: read register of SPI_RD_STATUS.*/
            volatile uint32_t slv_status_fast_en:  1;                /*In the slave mode enable fast read status.*/
            volatile uint32_t slv_status_bitlen:   5;                /*In the slave mode it is the length of status bit.*/
        };
        volatile uint32_t val;
    }slave1;
    union {
        struct {
            volatile uint32_t slv_rdsta_dummy_cyclelen: 8;           /*In the slave mode it is the length in spi_clk cycles of dummy phase for read-status operations. The register value shall be (cycle_num-1).*/
            volatile uint32_t slv_wrsta_dummy_cyclelen: 8;           /*In the slave mode it is the length in spi_clk cycles of dummy phase for write-status operations. The register value shall be (cycle_num-1).*/
            volatile uint32_t slv_rdbuf_dummy_cyclelen: 8;           /*In the slave mode it is the length in spi_clk cycles of dummy phase for read-buffer operations. The register value shall be (cycle_num-1).*/
            volatile uint32_t slv_wrbuf_dummy_cyclelen: 8;           /*In the slave mode it is the length in spi_clk cycles of dummy phase for write-buffer operations. The register value shall be (cycle_num-1).*/
        };
        volatile uint32_t val;
    }slave2;
    union {
        struct {
            volatile uint32_t slv_rdbuf_cmd_value: 8;                /*In the slave mode it is the value of read-buffer command.*/
            volatile uint32_t slv_wrbuf_cmd_value: 8;                /*In the slave mode it is the value of write-buffer command.*/
            volatile uint32_t slv_rdsta_cmd_value: 8;                /*In the slave mode it is the value of read-status command.*/
            volatile uint32_t slv_wrsta_cmd_value: 8;                /*In the slave mode it is the value of write-status command.*/
        };
        volatile uint32_t val;
    }slave3;
    union {
        struct {
            volatile uint32_t slv_wrbuf_dbitlen:24;                  /*In the slave mode it is the length in bits for write-buffer operations. The register value shall be (bit_num-1).*/
            volatile uint32_t reserved24:        8;                  /*reserved*/
        };
        volatile uint32_t val;
    }slv_wrbuf_dlen;
    union {
        struct {
            volatile uint32_t slv_rdbuf_dbitlen:24;                  /*In the slave mode it is the length in bits for read-buffer operations. The register value shall be (bit_num-1).*/
            volatile uint32_t reserved24:        8;                  /*reserved*/
        };
        volatile uint32_t val;
    }slv_rdbuf_dlen;
    union {
        struct {
            volatile uint32_t cache_req_en:        1;                /*For SPI0  Cache access enable  1: enable  0:disable.*/
            volatile uint32_t cache_usr_cmd_4byte: 1;                /*For SPI0  cache  read flash with 4 bytes command  1: enable  0:disable.*/
            volatile uint32_t cache_flash_usr_cmd: 1;                /*For SPI0  cache  read flash for user define command  1: enable  0:disable.*/
            volatile uint32_t cache_flash_pes_en:  1;                /*For SPI0  spi1 send suspend command before cache read flash  1: enable  0:disable.*/
            volatile uint32_t reserved4:          28;                /*reserved*/
        };
        volatile uint32_t val;
    }cache_fctrl;
    union {
        struct {
            volatile uint32_t reserved0:           1;                /*reserved*/
            volatile uint32_t usr_sram_dio:        1;                /*For SPI0  In the spi sram mode  spi dual I/O mode enable  1: enable  0:disable*/
            volatile uint32_t usr_sram_qio:        1;                /*For SPI0  In the spi sram mode  spi quad I/O mode enable  1: enable  0:disable*/
            volatile uint32_t usr_wr_sram_dummy:   1;                /*For SPI0  In the spi sram mode  it is the enable bit of dummy phase for write operations.*/
            volatile uint32_t usr_rd_sram_dummy:   1;                /*For SPI0  In the spi sram mode  it is the enable bit of dummy phase for read operations.*/
            volatile uint32_t cache_sram_usr_rcmd: 1;                /*For SPI0  In the spi sram mode cache read sram for user define command.*/
            volatile uint32_t sram_bytes_len:      8;                /*For SPI0  In the sram mode  it is the byte length of spi read sram data.*/
            volatile uint32_t sram_dummy_cyclelen: 8;                /*For SPI0  In the sram mode  it is the length in bits of address phase. The register value shall be (bit_num-1).*/
            volatile uint32_t sram_addr_bitlen:    6;                /*For SPI0  In the sram mode  it is the length in bits of address phase. The register value shall be (bit_num-1).*/
            volatile uint32_t cache_sram_usr_wcmd: 1;                /*For SPI0  In the spi sram mode cache write sram for user define command*/
            volatile uint32_t reserved29:          3;                /*reserved*/
        };
        volatile uint32_t val;
    }cache_sctrl;
    union {
        struct {
            volatile uint32_t sram_dio:   1;                         /*For SPI0 SRAM DIO mode enable .  SRAM DIO enable command will be send when the bit is set. The bit will be cleared once the operation done.*/
            volatile uint32_t sram_qio:   1;                         /*For SPI0 SRAM QIO mode enable .  SRAM QIO enable command will be send when the bit is set. The bit will be cleared once the operation done.*/
            volatile uint32_t reserved2:  2;                         /*For SPI0 SRAM write enable . SRAM write operation will be triggered when the bit is set. The bit will be cleared once the operation done.*/
            volatile uint32_t sram_rstio: 1;                         /*For SPI0 SRAM IO mode reset enable. SRAM IO mode reset operation will be triggered when the bit is set. The bit will be cleared once the operation done*/
            volatile uint32_t reserved5: 27;                         /*reserved*/
        };
        volatile uint32_t val;
    }sram_cmd;
    union {
        struct {
            volatile uint32_t cache_sram_usr_rd_cmd_value: 16;       /*For SPI0 When cache mode is enable it is the read command value of command phase for SRAM.*/
            volatile uint32_t reserved16:                  12;       /*reserved*/
            volatile uint32_t cache_sram_usr_rd_cmd_bitlen: 4;       /*For SPI0 When cache mode is enable it is the length in bits of command phase for SRAM. The register value shall be (bit_num-1).*/
        };
        volatile uint32_t val;
    }sram_drd_cmd;
    union {
        struct {
            volatile uint32_t cache_sram_usr_wr_cmd_value: 16;       /*For SPI0 When cache mode is enable it is the write command value of command phase for SRAM.*/
            volatile uint32_t reserved16:                  12;       /*reserved*/
            volatile uint32_t cache_sram_usr_wr_cmd_bitlen: 4;       /*For SPI0 When cache mode is enable it is the in bits of command phase  for SRAM. The register value shall be (bit_num-1).*/
        };
        volatile uint32_t val;
    }sram_dwr_cmd;
    union {
        struct {
            volatile uint32_t slv_rdata_bit:24;                      /*In the slave mode it is the bit length of read data. The value is the length - 1.*/
            volatile uint32_t reserved24:    8;                      /*reserved*/
        };
        volatile uint32_t val;
    }slv_rd_bit;
    volatile uint32_t reserved_68;
    volatile uint32_t reserved_6c;
    volatile uint32_t reserved_70;
    volatile uint32_t reserved_74;
    volatile uint32_t reserved_78;
    volatile uint32_t reserved_7c;
    volatile uint32_t data_buf[16];                                  /*data buffer*/
    volatile uint32_t tx_crc;                                        /*For SPI1  the value of crc32 for 256 bits data.*/
    volatile uint32_t reserved_c4;
    volatile uint32_t reserved_c8;
    volatile uint32_t reserved_cc;
    volatile uint32_t reserved_d0;
    volatile uint32_t reserved_d4;
    volatile uint32_t reserved_d8;
    volatile uint32_t reserved_dc;
    volatile uint32_t reserved_e0;
    volatile uint32_t reserved_e4;
    volatile uint32_t reserved_e8;
    volatile uint32_t reserved_ec;
    union {
        struct {
            volatile uint32_t t_pp_time: 12;                         /*page program delay time  by system clock.*/
            volatile uint32_t reserved12: 4;                         /*reserved*/
            volatile uint32_t t_pp_shift: 4;                         /*page program delay time shift .*/
            volatile uint32_t reserved20:11;                         /*reserved*/
            volatile uint32_t t_pp_ena:   1;                         /*page program delay enable.*/
        };
        volatile uint32_t val;
    }ext0;
    union {
        struct {
            volatile uint32_t t_erase_time: 12;                      /*erase flash delay time by system clock.*/
            volatile uint32_t reserved12:    4;                      /*reserved*/
            volatile uint32_t t_erase_shift: 4;                      /*erase flash delay time shift.*/
            volatile uint32_t reserved20:   11;                      /*reserved*/
            volatile uint32_t t_erase_ena:   1;                      /*erase flash delay enable.*/
        };
        volatile uint32_t val;
    }ext1;
    union {
        struct {
            volatile uint32_t st:         3;                         /*The status of spi state machine .*/
            volatile uint32_t reserved3: 29;                         /*reserved*/
        };
        volatile uint32_t val;
    }ext2;
    union {
        struct {
            volatile uint32_t int_hold_ena: 2;                       /*This register is for two SPI masters to share the same cs clock and data signals. The bits of one SPI are set  if the other SPI is busy  the SPI will be hold. 1(3): hold at ,idle, phase 2: hold at ,prepare, phase.*/
            volatile uint32_t reserved2:   30;                       /*reserved*/
        };
        volatile uint32_t val;
    }ext3;
    union {
        struct {
            volatile uint32_t reserved0:         2;                  /*reserved*/
            volatile uint32_t in_rst:            1;                  /*The bit is used to reset in dma fsm and in data fifo pointer.*/
            volatile uint32_t out_rst:           1;                  /*The bit is used to reset out dma fsm and out data fifo pointer.*/
            volatile uint32_t ahbm_fifo_rst:     1;                  /*reset spi dma ahb master fifo pointer.*/
            volatile uint32_t ahbm_rst:          1;                  /*reset spi dma ahb master.*/
            volatile uint32_t in_loop_test:      1;                  /*Set bit to test in link.*/
            volatile uint32_t out_loop_test:     1;                  /*Set bit to test out link.*/
            volatile uint32_t out_auto_wrback:   1;                  /*when the link is empty   jump to next automatically.*/
            volatile uint32_t out_eof_mode:      1;                  /*out eof flag generation mode . 1: when dma pop all data from fifo  0:when ahb push all data to fifo.*/
            volatile uint32_t outdscr_burst_en:  1;                  /*read descriptor use burst mode when read data for memory.*/
            volatile uint32_t indscr_burst_en:   1;                  /*read descriptor use burst mode when write data to memory.*/
            volatile uint32_t out_data_burst_en: 1;                  /*spi dma read data from memory in burst mode.*/
            volatile uint32_t reserved13:        1;                  /*reserved*/
            volatile uint32_t dma_rx_stop:       1;                  /*spi dma read data stop  when in continue tx/rx mode.*/
            volatile uint32_t dma_tx_stop:       1;                  /*spi dma write data stop when in continue tx/rx mode.*/
            volatile uint32_t dma_continue:      1;                  /*spi dma continue tx/rx data.*/
            volatile uint32_t reserved17:       15;                  /*reserved*/
        };
        volatile uint32_t val;
    }dma_conf;
    union {
        struct {
            volatile uint32_t outlink_addr:   20;                    /*The address of the first outlink descriptor.*/
            volatile uint32_t reserved20:      8;                    /*reserved*/
            volatile uint32_t outlink_stop:    1;                    /*Set the bit to stop to use outlink descriptor.*/
            volatile uint32_t outlink_start:   1;                    /*Set the bit to start to use outlink descriptor.*/
            volatile uint32_t outlink_restart: 1;                    /*Set the bit to mount on new outlink descriptors.*/
            volatile uint32_t reserved31:      1;                    /*reserved*/
        };
        volatile uint32_t val;
    }dma_out_link;
    union {
        struct {
            volatile uint32_t inlink_addr:    20;                    /*The address of the first inlink descriptor.*/
            volatile uint32_t inlink_auto_ret: 1;                    /*when the bit is set  inlink descriptor returns to the next descriptor while a packet is wrong*/
            volatile uint32_t reserved21:      7;                    /*reserved*/
            volatile uint32_t inlink_stop:     1;                    /*Set the bit to stop to use inlink descriptor.*/
            volatile uint32_t inlink_start:    1;                    /*Set the bit to start to use inlink descriptor.*/
            volatile uint32_t inlink_restart:  1;                    /*Set the bit to mount on new inlink descriptors.*/
            volatile uint32_t reserved31:      1;                    /*reserved*/
        };
        volatile uint32_t val;
    }dma_in_link;
    union {
        struct {
            volatile uint32_t dma_rx_en:  1;                         /*spi dma read data status bit.*/
            volatile uint32_t dma_tx_en:  1;                         /*spi dma write data status bit.*/
            volatile uint32_t reserved2: 30;                         /*spi dma read data from memory count.*/
        };
        volatile uint32_t val;
    }dma_status;
    union {
        struct {
            volatile uint32_t inlink_dscr_empty_int_ena:  1;         /*The enable bit for lack of enough inlink descriptors.*/
            volatile uint32_t outlink_dscr_error_int_ena: 1;         /*The enable bit for outlink descriptor error.*/
            volatile uint32_t inlink_dscr_error_int_ena:  1;         /*The enable bit for inlink descriptor error.*/
            volatile uint32_t in_done_int_ena:            1;         /*The enable bit for completing usage of a inlink descriptor.*/
            volatile uint32_t in_err_eof_int_ena:         1;         /*The enable bit for receiving error.*/
            volatile uint32_t in_suc_eof_int_ena:         1;         /*The enable bit for completing receiving all the packets from host.*/
            volatile uint32_t out_done_int_ena:           1;         /*The enable bit for completing usage of a outlink descriptor .*/
            volatile uint32_t out_eof_int_ena:            1;         /*The enable bit for sending a packet to host done.*/
            volatile uint32_t out_total_eof_int_ena:      1;         /*The enable bit for sending all the packets to host done.*/
            volatile uint32_t reserved9:                 23;         /*reserved*/
        };
        volatile uint32_t val;
    }dma_int_ena;
    union {
        struct {
            volatile uint32_t inlink_dscr_empty_int_raw:  1;         /*The raw bit for lack of enough inlink descriptors.*/
            volatile uint32_t outlink_dscr_error_int_raw: 1;         /*The raw bit for outlink descriptor error.*/
            volatile uint32_t inlink_dscr_error_int_raw:  1;         /*The raw bit for inlink descriptor error.*/
            volatile uint32_t in_done_int_raw:            1;         /*The raw bit for completing usage of a inlink descriptor.*/
            volatile uint32_t in_err_eof_int_raw:         1;         /*The raw bit for receiving error.*/
            volatile uint32_t in_suc_eof_int_raw:         1;         /*The raw bit for completing receiving all the packets from host.*/
            volatile uint32_t out_done_int_raw:           1;         /*The raw bit for completing usage of a outlink descriptor.*/
            volatile uint32_t out_eof_int_raw:            1;         /*The raw bit for sending a packet to host done.*/
            volatile uint32_t out_total_eof_int_raw:      1;         /*The raw bit for sending all the packets to host done.*/
            volatile uint32_t reserved9:                 23;         /*reserved*/
        };
        volatile uint32_t val;
    }dma_int_raw;
    union {
        struct {
            volatile uint32_t inlink_dscr_empty_int_st:  1;          /*The status bit for lack of enough inlink descriptors.*/
            volatile uint32_t outlink_dscr_error_int_st: 1;          /*The status bit for outlink descriptor error.*/
            volatile uint32_t inlink_dscr_error_int_st:  1;          /*The status bit for inlink descriptor error.*/
            volatile uint32_t in_done_int_st:            1;          /*The status bit for completing usage of a inlink descriptor.*/
            volatile uint32_t in_err_eof_int_st:         1;          /*The status bit for receiving error.*/
            volatile uint32_t in_suc_eof_int_st:         1;          /*The status bit for completing receiving all the packets from host.*/
            volatile uint32_t out_done_int_st:           1;          /*The status bit for completing usage of a outlink descriptor.*/
            volatile uint32_t out_eof_int_st:            1;          /*The status bit for sending a packet to host done.*/
            volatile uint32_t out_total_eof_int_st:      1;          /*The status bit for sending all the packets to host done.*/
            volatile uint32_t reserved9:                23;          /*reserved*/
        };
        volatile uint32_t val;
    }dma_int_st;
    union {
        struct {
            volatile uint32_t inlink_dscr_empty_int_clr:  1;         /*The clear bit for lack of enough inlink descriptors.*/
            volatile uint32_t outlink_dscr_error_int_clr: 1;         /*The clear bit for outlink descriptor error.*/
            volatile uint32_t inlink_dscr_error_int_clr:  1;         /*The clear bit for inlink descriptor error.*/
            volatile uint32_t in_done_int_clr:            1;         /*The clear bit for completing usage of a inlink descriptor.*/
            volatile uint32_t in_err_eof_int_clr:         1;         /*The clear bit for receiving error.*/
            volatile uint32_t in_suc_eof_int_clr:         1;         /*The clear bit for completing receiving all the packets from host.*/
            volatile uint32_t out_done_int_clr:           1;         /*The clear bit for completing usage of a outlink descriptor.*/
            volatile uint32_t out_eof_int_clr:            1;         /*The clear bit for sending a packet to host done.*/
            volatile uint32_t out_total_eof_int_clr:      1;         /*The clear bit for sending all the packets to host done.*/
            volatile uint32_t reserved9:                 23;         /*reserved*/
        };
        volatile uint32_t val;
    }dma_int_clr;
    volatile uint32_t dma_in_err_eof_des_addr;                       /*The inlink descriptor address when spi dma produce receiving error.*/
    volatile uint32_t dma_in_suc_eof_des_addr;                       /*The last inlink descriptor address when spi dma produce from_suc_eof.*/
    volatile uint32_t dma_inlink_dscr;                               /*The content of current in descriptor pointer.*/
    volatile uint32_t dma_inlink_dscr_bf0;                           /*The content of next in descriptor pointer.*/
    volatile uint32_t dma_inlink_dscr_bf1;                           /*The content of current in descriptor data buffer pointer.*/
    volatile uint32_t dma_out_eof_bfr_des_addr;                      /*The address of buffer relative to the outlink descriptor that produce eof.*/
    volatile uint32_t dma_out_eof_des_addr;                          /*The last outlink descriptor address when spi dma produce to_eof.*/
    volatile uint32_t dma_outlink_dscr;                              /*The content of current out descriptor pointer.*/
    volatile uint32_t dma_outlink_dscr_bf0;                          /*The content of next out descriptor pointer.*/
    volatile uint32_t dma_outlink_dscr_bf1;                          /*The content of current out descriptor data buffer pointer.*/
    volatile uint32_t dma_rx_status;                                 /*spi dma read data from memory status.*/
    volatile uint32_t dma_tx_status;                                 /*spi dma write data to memory status.*/
    volatile uint32_t reserved_150;
    volatile uint32_t reserved_154;
    volatile uint32_t reserved_158;
    volatile uint32_t reserved_15c;
    volatile uint32_t reserved_160;
    volatile uint32_t reserved_164;
    volatile uint32_t reserved_168;
    volatile uint32_t reserved_16c;
    volatile uint32_t reserved_170;
    volatile uint32_t reserved_174;
    volatile uint32_t reserved_178;
    volatile uint32_t reserved_17c;
    volatile uint32_t reserved_180;
    volatile uint32_t reserved_184;
    volatile uint32_t reserved_188;
    volatile uint32_t reserved_18c;
    volatile uint32_t reserved_190;
    volatile uint32_t reserved_194;
    volatile uint32_t reserved_198;
    volatile uint32_t reserved_19c;
    volatile uint32_t reserved_1a0;
    volatile uint32_t reserved_1a4;
    volatile uint32_t reserved_1a8;
    volatile uint32_t reserved_1ac;
    volatile uint32_t reserved_1b0;
    volatile uint32_t reserved_1b4;
    volatile uint32_t reserved_1b8;
    volatile uint32_t reserved_1bc;
    volatile uint32_t reserved_1c0;
    volatile uint32_t reserved_1c4;
    volatile uint32_t reserved_1c8;
    volatile uint32_t reserved_1cc;
    volatile uint32_t reserved_1d0;
    volatile uint32_t reserved_1d4;
    volatile uint32_t reserved_1d8;
    volatile uint32_t reserved_1dc;
    volatile uint32_t reserved_1e0;
    volatile uint32_t reserved_1e4;
    volatile uint32_t reserved_1e8;
    volatile uint32_t reserved_1ec;
    volatile uint32_t reserved_1f0;
    volatile uint32_t reserved_1f4;
    volatile uint32_t reserved_1f8;
    volatile uint32_t reserved_1fc;
    volatile uint32_t reserved_200;
    volatile uint32_t reserved_204;
    volatile uint32_t reserved_208;
    volatile uint32_t reserved_20c;
    volatile uint32_t reserved_210;
    volatile uint32_t reserved_214;
    volatile uint32_t reserved_218;
    volatile uint32_t reserved_21c;
    volatile uint32_t reserved_220;
    volatile uint32_t reserved_224;
    volatile uint32_t reserved_228;
    volatile uint32_t reserved_22c;
    volatile uint32_t reserved_230;
    volatile uint32_t reserved_234;
    volatile uint32_t reserved_238;
    volatile uint32_t reserved_23c;
    volatile uint32_t reserved_240;
    volatile uint32_t reserved_244;
    volatile uint32_t reserved_248;
    volatile uint32_t reserved_24c;
    volatile uint32_t reserved_250;
    volatile uint32_t reserved_254;
    volatile uint32_t reserved_258;
    volatile uint32_t reserved_25c;
    volatile uint32_t reserved_260;
    volatile uint32_t reserved_264;
    volatile uint32_t reserved_268;
    volatile uint32_t reserved_26c;
    volatile uint32_t reserved_270;
    volatile uint32_t reserved_274;
    volatile uint32_t reserved_278;
    volatile uint32_t reserved_27c;
    volatile uint32_t reserved_280;
    volatile uint32_t reserved_284;
    volatile uint32_t reserved_288;
    volatile uint32_t reserved_28c;
    volatile uint32_t reserved_290;
    volatile uint32_t reserved_294;
    volatile uint32_t reserved_298;
    volatile uint32_t reserved_29c;
    volatile uint32_t reserved_2a0;
    volatile uint32_t reserved_2a4;
    volatile uint32_t reserved_2a8;
    volatile uint32_t reserved_2ac;
    volatile uint32_t reserved_2b0;
    volatile uint32_t reserved_2b4;
    volatile uint32_t reserved_2b8;
    volatile uint32_t reserved_2bc;
    volatile uint32_t reserved_2c0;
    volatile uint32_t reserved_2c4;
    volatile uint32_t reserved_2c8;
    volatile uint32_t reserved_2cc;
    volatile uint32_t reserved_2d0;
    volatile uint32_t reserved_2d4;
    volatile uint32_t reserved_2d8;
    volatile uint32_t reserved_2dc;
    volatile uint32_t reserved_2e0;
    volatile uint32_t reserved_2e4;
    volatile uint32_t reserved_2e8;
    volatile uint32_t reserved_2ec;
    volatile uint32_t reserved_2f0;
    volatile uint32_t reserved_2f4;
    volatile uint32_t reserved_2f8;
    volatile uint32_t reserved_2fc;
    volatile uint32_t reserved_300;
    volatile uint32_t reserved_304;
    volatile uint32_t reserved_308;
    volatile uint32_t reserved_30c;
    volatile uint32_t reserved_310;
    volatile uint32_t reserved_314;
    volatile uint32_t reserved_318;
    volatile uint32_t reserved_31c;
    volatile uint32_t reserved_320;
    volatile uint32_t reserved_324;
    volatile uint32_t reserved_328;
    volatile uint32_t reserved_32c;
    volatile uint32_t reserved_330;
    volatile uint32_t reserved_334;
    volatile uint32_t reserved_338;
    volatile uint32_t reserved_33c;
    volatile uint32_t reserved_340;
    volatile uint32_t reserved_344;
    volatile uint32_t reserved_348;
    volatile uint32_t reserved_34c;
    volatile uint32_t reserved_350;
    volatile uint32_t reserved_354;
    volatile uint32_t reserved_358;
    volatile uint32_t reserved_35c;
    volatile uint32_t reserved_360;
    volatile uint32_t reserved_364;
    volatile uint32_t reserved_368;
    volatile uint32_t reserved_36c;
    volatile uint32_t reserved_370;
    volatile uint32_t reserved_374;
    volatile uint32_t reserved_378;
    volatile uint32_t reserved_37c;
    volatile uint32_t reserved_380;
    volatile uint32_t reserved_384;
    volatile uint32_t reserved_388;
    volatile uint32_t reserved_38c;
    volatile uint32_t reserved_390;
    volatile uint32_t reserved_394;
    volatile uint32_t reserved_398;
    volatile uint32_t reserved_39c;
    volatile uint32_t reserved_3a0;
    volatile uint32_t reserved_3a4;
    volatile uint32_t reserved_3a8;
    volatile uint32_t reserved_3ac;
    volatile uint32_t reserved_3b0;
    volatile uint32_t reserved_3b4;
    volatile uint32_t reserved_3b8;
    volatile uint32_t reserved_3bc;
    volatile uint32_t reserved_3c0;
    volatile uint32_t reserved_3c4;
    volatile uint32_t reserved_3c8;
    volatile uint32_t reserved_3cc;
    volatile uint32_t reserved_3d0;
    volatile uint32_t reserved_3d4;
    volatile uint32_t reserved_3d8;
    volatile uint32_t reserved_3dc;
    volatile uint32_t reserved_3e0;
    volatile uint32_t reserved_3e4;
    volatile uint32_t reserved_3e8;
    volatile uint32_t reserved_3ec;
    volatile uint32_t reserved_3f0;
    volatile uint32_t reserved_3f4;
    volatile uint32_t reserved_3f8;
    union {
        struct {
            volatile uint32_t date:      28;                         /*SPI register version.*/
            volatile uint32_t reserved28: 4;                         /*reserved*/
        };
        volatile uint32_t val;
    }date;
} spi_dev_t;
extern volatile spi_dev_t SPI0;                                      /* SPI0 IS FOR INTERNAL USE*/
extern volatile spi_dev_t SPI1;
extern volatile spi_dev_t SPI2;
extern volatile spi_dev_t SPI3;
#endif  /* _SOC_SPI_STRUCT_H_ */
