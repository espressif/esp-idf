/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct spi_dev_s {
    union {
        struct {
            uint32_t conf_bitlen                   :    18;  /*Define the APB cycles of  SPI_CONF state. Can be configured in CONF state.*/
            uint32_t reserved18                    :    5;  /*reserved*/
            uint32_t update                        :    1;  /*Set this bit to synchronize SPI registers from APB clock domain into SPI module clock domain, which is only used in SPI master mode.*/
            uint32_t usr                           :    1;  /*User define command enable.  An operation will be triggered when the bit is set. The bit will be cleared once the operation done.1: enable 0: disable. Can not be changed by CONF_buf.*/
            uint32_t reserved25                    :    7;  /*reserved*/
        };
        uint32_t val;
    } cmd;
    uint32_t addr;
    union {
        struct {
            uint32_t reserved0                     :    3;  /*reserved*/
            uint32_t dummy_out                     :    1;  /*In the dummy phase the signal level of spi is output by the spi controller. Can be configured in CONF state.*/
            uint32_t reserved4                     :    1;  /*reserved*/
            uint32_t faddr_dual                    :    1;  /*Apply 2 signals during addr phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t faddr_quad                    :    1;  /*Apply 4 signals during addr phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t reserved7                     :    1;  /*reserved*/
            uint32_t fcmd_dual                     :    1;  /*Apply 2 signals during command phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t fcmd_quad                     :    1;  /*Apply 4 signals during command phase 1:enable 0: disable. Can be configured in CONF state.*/
            uint32_t reserved10                    :    4;  /*reserved*/
            uint32_t fread_dual                    :    1;  /*In the read operations, read-data phase apply 2 signals. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t fread_quad                    :    1;  /*In the read operations read-data phase apply 4 signals. 1: enable 0: disable.  Can be configured in CONF state.*/
            uint32_t reserved16                    :    1;  /*reserved*/
            uint32_t reserved17                    :    1;  /*reserved*/
            uint32_t q_pol                         :    1;  /*The bit is used to set MISO line polarity, 1: high 0, low. Can be configured in CONF state.*/
            uint32_t d_pol                         :    1;  /*The bit is used to set MOSI line polarity, 1: high 0, low. Can be configured in CONF state.*/
            uint32_t hold_pol                      :    1;  /*SPI_HOLD output value when SPI is idle. 1: output high, 0: output low. Can be configured in CONF state.*/
            uint32_t wp_pol                        :    1;  /*Write protect signal output when SPI is idle.  1: output high, 0: output low.  Can be configured in CONF state.*/
            uint32_t reserved22                    :    3;  /*reserved*/
            uint32_t rd_bit_order                  :    1;  /*In read-data (MISO) phase 1: LSB first 0: MSB first. Can be configured in CONF state.*/
            uint32_t wr_bit_order                  :    1;  /*In command address write-data (MOSI) phases 1: LSB firs 0: MSB first. Can be configured in CONF state.*/
            uint32_t reserved27                    :    5;  /*reserved*/
        };
        uint32_t val;
    } ctrl;
    union {
        struct {
            uint32_t clkcnt_l                      :    6;  /*In the master mode it must be equal to spi_clkcnt_N. In the slave mode it must be 0. Can be configured in CONF state.*/
            uint32_t clkcnt_h                      :    6;  /*In the master mode it must be floor((spi_clkcnt_N+1)/2-1). In the slave mode it must be 0. Can be configured in CONF state.*/
            uint32_t clkcnt_n                      :    6;  /*In the master mode it is the divider of spi_clk. So spi_clk frequency is system/(spi_clkdiv_pre+1)/(spi_clkcnt_N+1). Can be configured in CONF state.*/
            uint32_t clkdiv_pre                    :    4;  /*In the master mode it is pre-divider of spi_clk.  Can be configured in CONF state.*/
            uint32_t reserved22                    :    9;  /*reserved*/
            uint32_t clk_equ_sysclk                :    1;  /*In the master mode 1: spi_clk is eqaul to system 0: spi_clk is divided from system clock. Can be configured in CONF state.*/
        };
        uint32_t val;
    } clock;
    union {
        struct {
            uint32_t doutdin                       :    1;  /*Set the bit to enable full duplex communication. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t reserved1                     :    2;  /*reserved*/
            uint32_t qpi_mode                      :    1;  /*Both for master mode and slave mode. 1: spi controller is in QPI mode. 0: others. Can be configured in CONF state.*/
            uint32_t reserved4                     :    1;  /*reserved*/
            uint32_t tsck_i_edge                   :    1;  /*In the slave mode, this bit can be used to change the polarity of tsck. 0: tsck = spi_ck_i. 1:tsck = !spi_ck_i.*/
            uint32_t cs_hold                       :    1;  /*spi cs keep low when spi is in  done  phase. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t cs_setup                      :    1;  /*spi cs is enable when spi is in  prepare  phase. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t rsck_i_edge                   :    1;  /*In the slave mode, this bit can be used to change the polarity of rsck. 0: rsck = !spi_ck_i. 1:rsck = spi_ck_i.*/
            uint32_t ck_out_edge                   :    1;  /*the bit combined with spi_mosi_delay_mode bits to set mosi signal delay mode. Can be configured in CONF state.*/
            uint32_t reserved10                    :    2;  /*reserved*/
            uint32_t fwrite_dual                   :    1;  /*In the write operations read-data phase apply 2 signals. Can be configured in CONF state.*/
            uint32_t fwrite_quad                   :    1;  /*In the write operations read-data phase apply 4 signals. Can be configured in CONF state.*/
            uint32_t reserved14                    :    1;  /*reserved*/
            uint32_t usr_conf_nxt                  :    1;  /*1: Enable the DMA CONF phase of next seg-trans operation, which means seg-trans will continue. 0: The seg-trans will end after the current SPI seg-trans or this is not seg-trans mode. Can be configured in CONF state.*/
            uint32_t reserved16                    :    1;  /*reserved*/
            uint32_t sio                           :    1;  /*Set the bit to enable 3-line half duplex communication mosi and miso signals share the same pin. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t reserved18                    :    6;  /*reserved*/
            uint32_t usr_miso_highpart             :    1;  /*read-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable. Can be configured in CONF state.*/
            uint32_t usr_mosi_highpart             :    1;  /*write-data phase only access to high-part of the buffer spi_w8~spi_w15. 1: enable 0: disable.  Can be configured in CONF state.*/
            uint32_t usr_dummy_idle                :    1;  /*spi clock is disable in dummy phase when the bit is enable. Can be configured in CONF state.*/
            uint32_t usr_mosi                      :    1;  /*This bit enable the write-data phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_miso                      :    1;  /*This bit enable the read-data phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_dummy                     :    1;  /*This bit enable the dummy phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_addr                      :    1;  /*This bit enable the address phase of an operation. Can be configured in CONF state.*/
            uint32_t usr_command                   :    1;  /*This bit enable the command phase of an operation. Can be configured in CONF state.*/
        };
        uint32_t val;
    } user;
    union {
        struct {
            uint32_t usr_dummy_cyclelen            :    8;  /*The length in spi_clk cycles of dummy phase. The register value shall be (cycle_num-1). Can be configured in CONF state.*/
            uint32_t reserved8                     :    8;  /*reserved*/
            uint32_t mst_wfull_err_end_en          :    1;  /*1: SPI transfer is ended when SPI RX AFIFO wfull error is valid in GP-SPI master FD/HD-mode. 0: SPI transfer is not ended when SPI RX AFIFO wfull error is valid in GP-SPI master FD/HD-mode.*/
            uint32_t cs_setup_time                 :    5;  /*(cycles+1) of prepare phase by spi clock this bits are combined with spi_cs_setup bit. Can be configured in CONF state.*/
            uint32_t cs_hold_time                  :    5;  /*delay cycles of cs pin by spi clock this bits are combined with spi_cs_hold bit. Can be configured in CONF state.*/
            uint32_t usr_addr_bitlen               :    5;  /*The length in bits of address phase. The register value shall be (bit_num-1). Can be configured in CONF state.*/
        };
        uint32_t val;
    } user1;
    union {
        struct {
            uint32_t usr_command_value             :    16;  /*The value of  command. Can be configured in CONF state.*/
            uint32_t reserved16                    :    11;  /*reserved*/
            uint32_t mst_rempty_err_end_en         :    1;  /*1: SPI transfer is ended when SPI TX AFIFO read empty error is valid in GP-SPI master FD/HD-mode. 0: SPI transfer is not ended when SPI TX AFIFO read empty error is valid in GP-SPI master FD/HD-mode.*/
            uint32_t usr_command_bitlen            :    4;  /*The length in bits of command phase. The register value shall be (bit_num-1). Can be configured in CONF state.*/
        };
        uint32_t val;
    } user2;
    union {
        struct {
            uint32_t ms_data_bitlen                :    18;  /*The value of these bits is the configured SPI transmission data bit length in master mode DMA controlled transfer or CPU controlled transfer. The value is also the configured bit length in slave mode DMA RX controlled transfer. The register value shall be (bit_num-1). Can be configured in CONF state.*/
            uint32_t reserved18                    :    14;  /*reserved*/
        };
        uint32_t val;
    } ms_dlen;
    union {
        struct {
            uint32_t cs0_dis                       :    1;  /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t cs1_dis                       :    1;  /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t cs2_dis                       :    1;  /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t cs3_dis                       :    1;  /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t cs4_dis                       :    1;  /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t cs5_dis                       :    1;  /*SPI CS$n pin enable, 1: disable CS$n, 0: spi_cs$n signal is from/to CS$n pin. Can be configured in CONF state.*/
            uint32_t ck_dis                        :    1;  /*1: spi clk out disable,  0: spi clk out enable. Can be configured in CONF state.*/
            uint32_t master_cs_pol                 :    6;  /*In the master mode the bits are the polarity of spi cs line, the value is equivalent to spi_cs ^ spi_master_cs_pol. Can be configured in CONF state.*/
            uint32_t reserved13                    :    10;  /*reserved*/
            uint32_t slave_cs_pol                  :    1;  /*spi slave input cs polarity select. 1: inv  0: not change. Can be configured in CONF state.*/
            uint32_t reserved24                    :    5;  /*reserved*/
            uint32_t ck_idle_edge                  :    1;  /*1: spi clk line is high when idle     0: spi clk line is low when idle. Can be configured in CONF state.*/
            uint32_t cs_keep_active                :    1;  /*spi cs line keep low when the bit is set. Can be configured in CONF state.*/
            uint32_t quad_din_pin_swap             :    1;  /*1:  spi quad input swap enable  0:  spi quad input swap disable. Can be configured in CONF state.*/
        };
        uint32_t val;
    } misc;
    union {
        struct {
            uint32_t din0_mode                     :    2;  /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din1_mode                     :    2;  /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din2_mode                     :    2;  /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t din3_mode                     :    2;  /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t reserved8                     :    8;  /*the input signals are delayed by SPI module clock cycles, 0: input without delayed, 1: input with the posedge of clk_apb,2 input with the negedge of clk_apb, 3: input with the spi_clk. Can be configured in CONF state.*/
            uint32_t timing_hclk_active            :    1;  /*1:enable hclk in SPI input timing module.  0: disable it. Can be configured in CONF state.*/
            uint32_t reserved17                    :    15;  /*reserved*/
        };
        uint32_t val;
    } din_mode;
    union {
        struct {
            uint32_t din0_num                      :    2;  /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t din1_num                      :    2;  /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t din2_num                      :    2;  /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t din3_num                      :    2;  /*the input signals are delayed by SPI module clock cycles, 0: delayed by 1 cycle, 1: delayed by 2 cycles,...  Can be configured in CONF state.*/
            uint32_t reserved8                     :    24;  /*reserved*/
        };
        uint32_t val;
    } din_num;
    union {
        struct {
            uint32_t dout0_mode                    :    1;  /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t dout1_mode                    :    1;  /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t dout2_mode                    :    1;  /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t dout3_mode                    :    1;  /*The output signal $n is delayed by the SPI module clock, 0: output without delayed, 1: output delay for a SPI module clock cycle at its negative edge. Can be configured in CONF state.*/
            uint32_t reserved4                     :    28;  /*reserved*/
        };
        uint32_t val;
    } dout_mode;
    union {
        struct {
            uint32_t reserved0                     :    18;  /*reserved*/
            uint32_t dma_seg_trans_en              :    1;  /*Enable dma segment transfer in spi dma half slave mode. 1: enable. 0: disable.*/
            uint32_t rx_seg_trans_clr_en           :    1;  /*1: spi_dma_infifo_full_vld is cleared by spi slave cmd 5. 0: spi_dma_infifo_full_vld is cleared by spi_trans_done.*/
            uint32_t tx_seg_trans_clr_en           :    1;  /*1: spi_dma_outfifo_empty_vld is cleared by spi slave cmd 6. 0: spi_dma_outfifo_empty_vld is cleared by spi_trans_done.*/
            uint32_t rx_eof_en                     :    1;  /*1: spi_dma_inlink_eof is set when the number of dma pushed data bytes is equal to the value of spi_slv/mst_dma_rd_bytelen[19:0] in spi dma transition.  0: spi_dma_inlink_eof is set by spi_trans_done in non-seg-trans or spi_dma_seg_trans_done in seg-trans.*/
            uint32_t reserved22                    :    5;  /*reserved*/
            uint32_t dma_rx_ena                    :    1;  /*Set this bit to enable SPI DMA controlled receive data mode.*/
            uint32_t dma_tx_ena                    :    1;  /*Set this bit to enable SPI DMA controlled send data mode.*/
            uint32_t rx_afifo_rst                  :    1;  /*Set this bit to reset RX AFIFO, which is used to receive data in SPI master and slave mode transfer.*/
            uint32_t buf_afifo_rst                 :    1;  /*Set this bit to reset BUF TX AFIFO, which is used send data out in SPI slave CPU controlled mode transfer and master mode transfer.*/
            uint32_t dma_afifo_rst                 :    1;  /*Set this bit to reset DMA TX AFIFO, which is used to send data out in SPI slave DMA controlled mode transfer.*/
        };
        uint32_t val;
    } dma_conf;
    union {
        struct {
            uint32_t infifo_full_err               :    1;  /*The enable bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt.*/
            uint32_t outfifo_empty_err             :    1;  /*The enable bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.*/
            uint32_t ex_qpi                        :    1;  /*The enable bit for SPI slave Ex_QPI interrupt.*/
            uint32_t en_qpi                        :    1;  /*The enable bit for SPI slave En_QPI interrupt.*/
            uint32_t cmd7                          :    1;  /*The enable bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8                          :    1;  /*The enable bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9                          :    1;  /*The enable bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda                          :    1;  /*The enable bit for SPI slave CMDA interrupt.*/
            uint32_t rd_dma_done                   :    1;  /*The enable bit for SPI_SLV_RD_DMA_DONE_INT interrupt.*/
            uint32_t wr_dma_done                   :    1;  /*The enable bit for SPI_SLV_WR_DMA_DONE_INT interrupt.*/
            uint32_t rd_buf_done                   :    1;  /*The enable bit for SPI_SLV_RD_BUF_DONE_INT interrupt.*/
            uint32_t wr_buf_done                   :    1;  /*The enable bit for SPI_SLV_WR_BUF_DONE_INT interrupt.*/
            uint32_t trans_done                    :    1;  /*The enable bit for SPI_TRANS_DONE_INT interrupt.*/
            uint32_t dma_seg_trans_done            :    1;  /*The enable bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt.*/
            uint32_t seg_magic_err                 :    1;  /*The enable bit for SPI_SEG_MAGIC_ERR_INT interrupt.*/
            uint32_t buf_addr_err                  :    1;  /*The enable bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.*/
            uint32_t cmd_err                       :    1;  /*The enable bit for SPI_SLV_CMD_ERR_INT interrupt.*/
            uint32_t mst_rx_afifo_wfull_err        :    1;  /*The enable bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.*/
            uint32_t mst_tx_afifo_rempty_err       :    1;  /*The enable bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.*/
            uint32_t app2                          :    1;  /*The enable bit for SPI_APP2_INT interrupt.*/
            uint32_t app1                          :    1;  /*The enable bit for SPI_APP1_INT interrupt.*/
            uint32_t reserved21                    :    11;  /*reserved*/
        };
        uint32_t val;
    } dma_int_ena;
    union {
        struct {
            uint32_t infifo_full_err               :    1;  /*The clear bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt.*/
            uint32_t outfifo_empty_err             :    1;  /*The clear bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.*/
            uint32_t ex_qpi                        :    1;  /*The clear bit for SPI slave Ex_QPI interrupt.*/
            uint32_t en_qpi                        :    1;  /*The clear bit for SPI slave En_QPI interrupt.*/
            uint32_t cmd7                          :    1;  /*The clear bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8                          :    1;  /*The clear bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9                          :    1;  /*The clear bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda                          :    1;  /*The clear bit for SPI slave CMDA interrupt.*/
            uint32_t rd_dma_done                   :    1;  /*The clear bit for SPI_SLV_RD_DMA_DONE_INT interrupt.*/
            uint32_t wr_dma_done                   :    1;  /*The clear bit for SPI_SLV_WR_DMA_DONE_INT interrupt.*/
            uint32_t rd_buf_done                   :    1;  /*The clear bit for SPI_SLV_RD_BUF_DONE_INT interrupt.*/
            uint32_t wr_buf_done                   :    1;  /*The clear bit for SPI_SLV_WR_BUF_DONE_INT interrupt.*/
            uint32_t trans_done                    :    1;  /*The clear bit for SPI_TRANS_DONE_INT interrupt.*/
            uint32_t dma_seg_trans_done            :    1;  /*The clear bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt.*/
            uint32_t seg_magic_err                 :    1;  /*The clear bit for SPI_SEG_MAGIC_ERR_INT interrupt.*/
            uint32_t buf_addr_err                  :    1;  /*The clear bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.*/
            uint32_t cmd_err                       :    1;  /*The clear bit for SPI_SLV_CMD_ERR_INT interrupt.*/
            uint32_t mst_rx_afifo_wfull_err        :    1;  /*The clear bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.*/
            uint32_t mst_tx_afifo_rempty_err       :    1;  /*The clear bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.*/
            uint32_t app2                          :    1;  /*The clear bit for SPI_APP2_INT interrupt.*/
            uint32_t app1                          :    1;  /*The clear bit for SPI_APP1_INT interrupt.*/
            uint32_t reserved21                    :    11;  /*reserved*/
        };
        uint32_t val;
    } dma_int_clr;
    union {
        struct {
            uint32_t infifo_full_err               :    1;  /*1: The current data rate of DMA Rx is smaller than that of SPI, which will lose the receive data.  0: Others.  */
            uint32_t outfifo_empty_err             :    1;  /*1: The current data rate of DMA TX is smaller than that of SPI. SPI will stop in master mode and send out all 0 in slave mode.  0: Others.  */
            uint32_t ex_qpi                        :    1;  /*The raw bit for SPI slave Ex_QPI interrupt. 1: SPI slave mode Ex_QPI transmission is ended. 0: Others.*/
            uint32_t en_qpi                        :    1;  /*The raw bit for SPI slave En_QPI interrupt. 1: SPI slave mode En_QPI transmission is ended. 0: Others.*/
            uint32_t cmd7                          :    1;  /*The raw bit for SPI slave CMD7 interrupt. 1: SPI slave mode CMD7 transmission is ended. 0: Others.*/
            uint32_t cmd8                          :    1;  /*The raw bit for SPI slave CMD8 interrupt. 1: SPI slave mode CMD8 transmission is ended. 0: Others.*/
            uint32_t cmd9                          :    1;  /*The raw bit for SPI slave CMD9 interrupt. 1: SPI slave mode CMD9 transmission is ended. 0: Others.*/
            uint32_t cmda                          :    1;  /*The raw bit for SPI slave CMDA interrupt. 1: SPI slave mode CMDA transmission is ended. 0: Others.*/
            uint32_t rd_dma_done                   :    1;  /*The raw bit for SPI_SLV_RD_DMA_DONE_INT interrupt. 1: SPI slave mode Rd_DMA transmission is ended. 0: Others.*/
            uint32_t wr_dma_done                   :    1;  /*The raw bit for SPI_SLV_WR_DMA_DONE_INT interrupt. 1: SPI slave mode Wr_DMA transmission is ended. 0: Others.*/
            uint32_t rd_buf_done                   :    1;  /*The raw bit for SPI_SLV_RD_BUF_DONE_INT interrupt. 1: SPI slave mode Rd_BUF transmission is ended. 0: Others.*/
            uint32_t wr_buf_done                   :    1;  /*The raw bit for SPI_SLV_WR_BUF_DONE_INT interrupt. 1: SPI slave mode Wr_BUF transmission is ended. 0: Others.*/
            uint32_t trans_done                    :    1;  /*The raw bit for SPI_TRANS_DONE_INT interrupt. 1: SPI master mode transmission is ended. 0: others.*/
            uint32_t dma_seg_trans_done            :    1;  /*The raw bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt. 1:  spi master DMA full-duplex/half-duplex seg-conf-trans ends or slave half-duplex seg-trans ends. And data has been pushed to corresponding memory.  0:  seg-conf-trans or seg-trans is not ended or not occurred. */
            uint32_t seg_magic_err                 :    1;  /*The raw bit for SPI_SEG_MAGIC_ERR_INT interrupt. 1: The magic value in CONF buffer is error in the DMA seg-conf-trans. 0: others.*/
            uint32_t buf_addr_err                  :    1;  /*The raw bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt. 1: The accessing data address of the current SPI slave mode CPU controlled FD, Wr_BUF or Rd_BUF transmission is bigger than 63. 0: Others.*/
            uint32_t cmd_err                       :    1;  /*The raw bit for SPI_SLV_CMD_ERR_INT interrupt. 1: The slave command value in the current SPI slave HD mode transmission is not supported. 0: Others.*/
            uint32_t mst_rx_afifo_wfull_err        :    1;  /*The raw bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt. 1: There is a RX AFIFO write-full error when SPI inputs data in master mode. 0: Others.*/
            uint32_t mst_tx_afifo_rempty_err       :    1;  /*The raw bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt. 1: There is a TX BUF AFIFO read-empty error when SPI outputs data in master mode. 0: Others.*/
            uint32_t app2                          :    1;  /*The raw bit for SPI_APP2_INT interrupt. The value is only controlled by application.*/
            uint32_t app1                          :    1;  /*The raw bit for SPI_APP1_INT interrupt. The value is only controlled by application.*/
            uint32_t reserved21                    :    11;  /*reserved*/
        };
        uint32_t val;
    } dma_int_raw;
    union {
        struct {
            uint32_t infifo_full_err               :    1;  /*The status bit for SPI_DMA_INFIFO_FULL_ERR_INT interrupt.*/
            uint32_t outfifo_empty_err             :    1;  /*The status bit for SPI_DMA_OUTFIFO_EMPTY_ERR_INT interrupt.*/
            uint32_t ex_qpi                        :    1;  /*The status bit for SPI slave Ex_QPI interrupt.*/
            uint32_t en_qpi                        :    1;  /*The status bit for SPI slave En_QPI interrupt.*/
            uint32_t cmd7                          :    1;  /*The status bit for SPI slave CMD7 interrupt.*/
            uint32_t cmd8                          :    1;  /*The status bit for SPI slave CMD8 interrupt.*/
            uint32_t cmd9                          :    1;  /*The status bit for SPI slave CMD9 interrupt.*/
            uint32_t cmda                          :    1;  /*The status bit for SPI slave CMDA interrupt.*/
            uint32_t rd_dma_done                   :    1;  /*The status bit for SPI_SLV_RD_DMA_DONE_INT interrupt.*/
            uint32_t wr_dma_done                   :    1;  /*The status bit for SPI_SLV_WR_DMA_DONE_INT interrupt.*/
            uint32_t rd_buf_done                   :    1;  /*The status bit for SPI_SLV_RD_BUF_DONE_INT interrupt.*/
            uint32_t wr_buf_done                   :    1;  /*The status bit for SPI_SLV_WR_BUF_DONE_INT interrupt.*/
            uint32_t trans_done                    :    1;  /*The status bit for SPI_TRANS_DONE_INT interrupt.*/
            uint32_t dma_seg_trans_done            :    1;  /*The status bit for SPI_DMA_SEG_TRANS_DONE_INT interrupt.*/
            uint32_t seg_magic_err                 :    1;  /*The status bit for SPI_SEG_MAGIC_ERR_INT interrupt.*/
            uint32_t buf_addr_err                  :    1;  /*The status bit for SPI_SLV_BUF_ADDR_ERR_INT interrupt.*/
            uint32_t cmd_err                       :    1;  /*The status bit for SPI_SLV_CMD_ERR_INT interrupt.*/
            uint32_t mst_rx_afifo_wfull_err        :    1;  /*The status bit for SPI_MST_RX_AFIFO_WFULL_ERR_INT interrupt.*/
            uint32_t mst_tx_afifo_rempty_err       :    1;  /*The status bit for SPI_MST_TX_AFIFO_REMPTY_ERR_INT interrupt.*/
            uint32_t app2                          :    1;  /*The status bit for SPI_APP2_INT interrupt.*/
            uint32_t app1                          :    1;  /*The status bit for SPI_APP1_INT interrupt.*/
            uint32_t reserved21                    :    11;  /*reserved*/
        };
        uint32_t val;
    } dma_int_st;
    uint32_t reserved_44;
    uint32_t reserved_48;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    uint32_t reserved_5c;
    uint32_t reserved_60;
    uint32_t reserved_64;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
    uint32_t reserved_74;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t data_buf[16];
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    union {
        struct {
            uint32_t clk_mode                      :    2;  /*SPI clock mode bits. 0: SPI clock is off when CS inactive 1: SPI clock is delayed one cycle after CS inactive 2: SPI clock is delayed two cycles after CS inactive 3: SPI clock is alwasy on. Can be configured in CONF state.*/
            uint32_t clk_mode_13                   :    1;  /*{CPOL, CPHA},1: support spi clk mode 1 and 3, first edge output data B[0]/B[7].  0: support spi clk mode 0 and 2, first edge output data B[1]/B[6].*/
            uint32_t rsck_data_out                 :    1;  /*It saves half a cycle when tsck is the same as rsck. 1: output data at rsck posedge   0: output data at tsck posedge */
            uint32_t reserved4                     :    4;  /*reserved*/
            uint32_t rddma_bitlen_en               :    1;  /*1: SPI_SLV_DATA_BITLEN stores data bit length of master-read-slave data length in DMA controlled mode(Rd_DMA). 0: others*/
            uint32_t wrdma_bitlen_en               :    1;  /*1: SPI_SLV_DATA_BITLEN stores data bit length of master-write-to-slave data length in DMA controlled mode(Wr_DMA). 0: others*/
            uint32_t rdbuf_bitlen_en               :    1;  /*1: SPI_SLV_DATA_BITLEN stores data bit length of master-read-slave data length in CPU controlled mode(Rd_BUF). 0: others*/
            uint32_t wrbuf_bitlen_en               :    1;  /*1: SPI_SLV_DATA_BITLEN stores data bit length of master-write-to-slave data length in CPU controlled mode(Wr_BUF). 0: others*/
            uint32_t reserved12                    :    10;  /*reserved*/
            uint32_t dma_seg_magic_value           :    4;  /*The magic value of BM table in master DMA seg-trans.*/
            uint32_t slave_mode                    :    1;  /*Set SPI work mode. 1: slave mode 0: master mode.*/
            uint32_t soft_reset                    :    1;  /*Software reset enable, reset the spi clock line cs line and data lines. Can be configured in CONF state.*/
            uint32_t usr_conf                      :    1;  /*1: Enable the DMA CONF phase of current seg-trans operation, which means seg-trans will start. 0: This is not seg-trans mode.*/
            uint32_t reserved29                    :    3;  /*reserved*/
        };
        uint32_t val;
    } slave;
    union {
        struct {
            uint32_t data_bitlen                   :    18;  /*The transferred data bit length in SPI slave FD and HD mode. */
            uint32_t last_command                  :    8;  /*In the slave mode it is the value of command.*/
            uint32_t last_addr                     :    6;  /*In the slave mode it is the value of address.*/
        };
        uint32_t val;
    } slave1;
    union {
        struct {
            uint32_t clk_en                        :    1;  /*Set this bit to enable clk gate*/
            uint32_t mst_clk_active                :    1;  /*Set this bit to power on the SPI module clock.*/
            uint32_t mst_clk_sel                   :    1;  /*This bit is used to select SPI module clock source in master mode. 1: PLL_CLK_80M. 0: XTAL CLK.*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } clk_gate;
    uint32_t reserved_ec;
    union {
        struct {
            uint32_t date                          :    28;  /*SPI register version.*/
            uint32_t reserved28                    :    4;  /*reserved*/
        };
        uint32_t val;
    } date;
} spi_dev_t;

extern spi_dev_t GPSPI2;

#ifdef __cplusplus
}
#endif
