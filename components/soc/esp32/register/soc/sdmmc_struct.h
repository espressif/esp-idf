/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_SDMMC_STRUCT_H_
#define _SOC_SDMMC_STRUCT_H_

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct sdmmc_desc_s {
    struct {
        uint32_t reserved1: 1;
        uint32_t disable_int_on_completion: 1;
        uint32_t last_descriptor: 1;
        uint32_t first_descriptor: 1;
        uint32_t second_address_chained: 1;
        uint32_t end_of_ring: 1;
        uint32_t reserved2: 24;
        uint32_t card_error_summary: 1;
        uint32_t owned_by_idmac: 1;
    };
    struct {
        uint32_t buffer1_size: 13;
        uint32_t buffer2_size: 13;
        uint32_t reserved3: 6;
    };
    void* buffer1_ptr;
    union {
        void* buffer2_ptr;
        void* next_desc_ptr;
    };
} sdmmc_desc_t;

#define SDMMC_DMA_MAX_BUF_LEN 4096

#ifndef __cplusplus
_Static_assert(sizeof(sdmmc_desc_t) == 16, "invalid size of sdmmc_desc_t structure");
#endif


typedef struct sdmmc_hw_cmd_s {
    uint32_t cmd_index: 6;          ///< Command index
    uint32_t response_expect: 1;    ///< set if response is expected
    uint32_t response_long: 1;      ///< 0: short response expected, 1: long response expected
    uint32_t check_response_crc: 1; ///< set if controller should check response CRC
    uint32_t data_expected: 1;      ///< 0: no data expected, 1: data expected
    uint32_t rw: 1;                 ///< 0: read from card, 1: write to card (don't care if no data expected)
    uint32_t stream_mode: 1;        ///< 0: block transfer, 1: stream transfer (don't care if no data expected)
    uint32_t send_auto_stop: 1;     ///< set to send stop at the end of the transfer
    uint32_t wait_complete: 1;      ///< 0: send command at once, 1: wait for previous command to complete
    uint32_t stop_abort_cmd: 1;     ///< set if this is a stop or abort command intended to stop current transfer
    uint32_t send_init: 1;          ///< set to send init sequence (80 clocks of 1)
    uint32_t card_num: 5;           ///< card number
    uint32_t update_clk_reg: 1;     ///< 0: normal command, 1: don't send command, just update clock registers
    uint32_t read_ceata: 1;         ///< set if performing read from CE-ATA device
    uint32_t ccs_expected: 1;       ///< set if CCS is expected from CE-ATA device
    uint32_t enable_boot: 1;        ///< set for mandatory boot mode
    uint32_t expect_boot_ack: 1;    ///< when set along with enable_boot, controller expects boot ack pattern
    uint32_t disable_boot: 1;       ///< set to terminate boot operation (don't set along with enable_boot)
    uint32_t boot_mode: 1;          ///< 0: mandatory boot operation, 1: alternate boot operation
    uint32_t volt_switch: 1;        ///< set to enable voltage switching (for CMD11 only)
    uint32_t use_hold_reg: 1;       ///< clear to bypass HOLD register
    uint32_t reserved: 1;
    uint32_t start_command: 1;      ///< Start command; once command is sent to the card, bit is cleared.
} sdmmc_hw_cmd_t;  ///< command format used in cmd register; this structure is defined to make it easier to build command values

#ifndef __cplusplus
_Static_assert(sizeof(sdmmc_hw_cmd_t) == 4, "invalid size of sdmmc_cmd_t structure");
#endif


typedef struct sdmmc_dev_t {
    volatile union {
        struct {
            uint32_t controller_reset: 1;
            uint32_t fifo_reset: 1;
            uint32_t dma_reset: 1;
            uint32_t reserved1: 1;
            uint32_t int_enable: 1;
            uint32_t dma_enable: 1;
            uint32_t read_wait: 1;
            uint32_t send_irq_response: 1;
            uint32_t abort_read_data: 1;
            uint32_t send_ccsd: 1;
            uint32_t send_auto_stop_ccsd: 1;
            uint32_t ceata_device_interrupt_status: 1;
            uint32_t reserved2: 4;
            uint32_t card_voltage_a: 4;
            uint32_t card_voltage_b: 4;
            uint32_t enable_od_pullup: 1;
            uint32_t use_internal_dma: 1;
            uint32_t reserved3: 6;
        };
        uint32_t val;
    } ctrl;

    volatile uint32_t pwren;             ///< 1: enable power to card, 0: disable power to card

    volatile union {
        struct {
            uint32_t div0: 8;   ///< 0: bypass, 1-255: divide clock by (2*div0).
            uint32_t div1: 8;   ///< 0: bypass, 1-255: divide clock by (2*div0).
            uint32_t div2: 8;   ///< 0: bypass, 1-255: divide clock by (2*div0).
            uint32_t div3: 8;   ///< 0: bypass, 1-255: divide clock by (2*div0).
        };
        uint32_t val;
    } clkdiv;

    volatile union {
        struct {
            uint32_t card0: 2;            ///< 0-3: select clock divider for card 0 among div0-div3
            uint32_t card1: 2;            ///< 0-3: select clock divider for card 1 among div0-div3
            uint32_t reserved: 28;
        };
        uint32_t val;
    } clksrc;

    volatile union {
        struct {
            uint32_t cclk_enable: 16;       ///< 1: enable clock to card, 0: disable clock
            uint32_t cclk_low_power: 16;    ///< 1: enable clock gating when card is idle, 0: disable clock gating
        };
        uint32_t val;
    } clkena;

    volatile union {
        struct {
            uint32_t response: 8;       ///< response timeout, in card output clock cycles
            uint32_t data: 24;          ///< data read timeout, in card output clock cycles
        };
        uint32_t val;
    } tmout;

    volatile union {
        struct {
            uint32_t card_width: 16;    ///< one bit for each card: 0: 1-bit mode, 1: 4-bit mode
            uint32_t card_width_8: 16;  ///< one bit for each card: 0: not 8-bit mode (corresponding card_width bit is used), 1: 8-bit mode (card_width bit is ignored)
        };
        uint32_t val;
    } ctype;

    volatile union {
        struct {
            uint32_t block_size: 16;        ///< block size, default 0x200
            uint32_t reserved: 16;
        };
        uint32_t val;
    } blksiz;

    volatile uint32_t bytcnt;            ///< number of bytes to be transferred

    volatile union {
        struct {
            uint32_t cd: 1;             ///< Card detect interrupt enable
            uint32_t re: 1;             ///< Response error interrupt enable
            uint32_t cmd_done: 1;       ///< Command done interrupt enable
            uint32_t dto: 1;            ///< Data transfer over interrupt enable
            uint32_t txdr: 1;           ///< Transmit FIFO data request interrupt enable
            uint32_t rxdr: 1;           ///< Receive FIFO data request interrupt enable
            uint32_t rcrc: 1;           ///< Response CRC error interrupt enable
            uint32_t dcrc: 1;           ///< Data CRC error interrupt enable
            uint32_t rto: 1;            ///< Response timeout interrupt enable
            uint32_t drto: 1;           ///< Data read timeout interrupt enable
            uint32_t hto: 1;            ///< Data starvation-by-host timeout interrupt enable
            uint32_t frun: 1;           ///< FIFO underrun/overrun error interrupt enable
            uint32_t hle: 1;            ///< Hardware locked write error interrupt enable
            uint32_t sbi_bci: 1;        ///< Start bit error / busy clear interrupt enable
            uint32_t acd: 1;            ///< Auto command done interrupt enable
            uint32_t ebe: 1;            ///< End bit error / write no CRC interrupt enable
            uint32_t sdio: 16;          ///< SDIO interrupt enable
        };
        uint32_t val;
    } intmask;

    volatile uint32_t cmdarg;        ///< Command argument to be passed to card

    volatile sdmmc_hw_cmd_t cmd;

    volatile uint32_t resp[4];           ///< Response from card

    volatile union {
        struct {
            uint32_t cd: 1;             ///< Card detect interrupt masked status
            uint32_t re: 1;             ///< Response error interrupt masked status
            uint32_t cmd_done: 1;       ///< Command done interrupt masked status
            uint32_t dto: 1;            ///< Data transfer over interrupt masked status
            uint32_t txdr: 1;           ///< Transmit FIFO data request interrupt masked status
            uint32_t rxdr: 1;           ///< Receive FIFO data request interrupt masked status
            uint32_t rcrc: 1;           ///< Response CRC error interrupt masked status
            uint32_t dcrc: 1;           ///< Data CRC error interrupt masked status
            uint32_t rto: 1;            ///< Response timeout interrupt masked status
            uint32_t drto: 1;           ///< Data read timeout interrupt masked status
            uint32_t hto: 1;            ///< Data starvation-by-host timeout interrupt masked status
            uint32_t frun: 1;           ///< FIFO underrun/overrun error interrupt masked status
            uint32_t hle: 1;            ///< Hardware locked write error interrupt masked status
            uint32_t sbi_bci: 1;        ///< Start bit error / busy clear interrupt masked status
            uint32_t acd: 1;            ///< Auto command done interrupt masked status
            uint32_t ebe: 1;            ///< End bit error / write no CRC interrupt masked status
            uint32_t sdio: 16;          ///< SDIO interrupt masked status
        };
        uint32_t val;
    } mintsts;

    volatile union {
        struct {
            uint32_t cd: 1;             ///< Card detect raw interrupt status
            uint32_t re: 1;             ///< Response error raw interrupt status
            uint32_t cmd_done: 1;       ///< Command done raw interrupt status
            uint32_t dto: 1;            ///< Data transfer over raw interrupt status
            uint32_t txdr: 1;           ///< Transmit FIFO data request raw interrupt status
            uint32_t rxdr: 1;           ///< Receive FIFO data request raw interrupt status
            uint32_t rcrc: 1;           ///< Response CRC error raw interrupt status
            uint32_t dcrc: 1;           ///< Data CRC error raw interrupt status
            uint32_t rto: 1;            ///< Response timeout raw interrupt status
            uint32_t drto: 1;           ///< Data read timeout raw interrupt status
            uint32_t hto: 1;            ///< Data starvation-by-host timeout raw interrupt status
            uint32_t frun: 1;           ///< FIFO underrun/overrun error raw interrupt status
            uint32_t hle: 1;            ///< Hardware locked write error raw interrupt status
            uint32_t sbi_bci: 1;        ///< Start bit error / busy clear raw interrupt status
            uint32_t acd: 1;            ///< Auto command done raw interrupt status
            uint32_t ebe: 1;            ///< End bit error / write no CRC raw interrupt status
            uint32_t sdio: 16;          ///< SDIO raw interrupt status
        };
        uint32_t val;
    } rintsts;                          ///< interrupts can be cleared by writing this register

    volatile union {
        struct {
            uint32_t fifo_rx_watermark: 1;  ///< FIFO reached receive watermark level
            uint32_t fifo_tx_watermark: 1;  ///< FIFO reached transmit watermark level
            uint32_t fifo_empty: 1;         ///< FIFO is empty
            uint32_t fifo_full: 1;          ///< FIFO is full
            uint32_t cmd_fsm_state: 4;      ///< command FSM state
            uint32_t data3_status: 1;       ///< this bit reads 1 if card is present
            uint32_t data_busy: 1;          ///< this bit reads 1 if card is busy
            uint32_t data_fsm_busy: 1;      ///< this bit reads 1 if transmit/receive FSM is busy
            uint32_t response_index: 6;     ///< index of the previous response
            uint32_t fifo_count: 13;        ///< number of filled locations in the FIFO
            uint32_t dma_ack: 1;            ///< DMA acknowledge signal
            uint32_t dma_req: 1;            ///< DMA request signal
        };
        uint32_t val;
    } status;

    volatile union {
        struct {
            uint32_t tx_watermark: 12;      ///< FIFO TX watermark level
            uint32_t reserved1: 4;
            uint32_t rx_watermark: 12;      ///< FIFO RX watermark level
            uint32_t dw_dma_mts: 3;
            uint32_t reserved2: 1;
        };
        uint32_t val;
    } fifoth;

    volatile union {
        struct {
            uint32_t cards: 2;              ///< bit N reads 0 if card N is present
            uint32_t reserved: 30;
        };
        uint32_t val;
    } cdetect;

    volatile union {
        struct {
            uint32_t cards: 2;              ///< bit N reads 1 if card N is write protected
            uint32_t reserved: 30;
        };
        uint32_t val;
    } wrtprt;

    volatile uint32_t gpio;      ///< unused
    volatile uint32_t tcbcnt;    ///< transferred (to card) byte count
    volatile uint32_t tbbcnt;    ///< transferred from host to FIFO byte count

    volatile union {
        struct {
            uint32_t debounce_count: 24;    ///< number of host cycles used by debounce filter, typical time should be 5-25ms
            uint32_t reserved: 8;
        };
    } debnce;

    volatile uint32_t usrid;     ///< user ID
    volatile uint32_t verid;     ///< IP block version

    volatile union {
        struct {
            /** card_type_reg : RO; bitpos: [0]; default: 1;
             *  Hardware support SDIO and MMC.
             */
            uint32_t card_type_reg:1;
            /** card_num_reg : RO; bitpos: [5:1]; default: 1;
             *  Support card number is 2.
             */
            uint32_t card_num_reg:5;
            /** bus_type_reg : RO; bitpos: [6]; default: 1;
             *  Register config is APB bus.
             */
            uint32_t bus_type_reg:1;
            /** data_width_reg : RO; bitpos: [9:7]; default: 1;
             *  Regisger data width is 32.
             */
            uint32_t data_width_reg:3;
            /** addr_width_reg : RO; bitpos: [15:10]; default: 19;
             *  Register address width is 32.
             */
            uint32_t addr_width_reg:6;
            uint32_t reserved_16:2;
            /** dma_width_reg : RO; bitpos: [20:18]; default: 1;
             *  DMA data width is 32.
             */
            uint32_t dma_width_reg:3;
            /** ram_indise_reg : RO; bitpos: [21]; default: 0;
             *  Inside RAM in SDMMC module.
             */
            uint32_t ram_indise_reg:1;
            /** hold_reg : RO; bitpos: [22]; default: 1;
             *  Have a hold register in data path .
             */
            uint32_t hold_reg:1;
            uint32_t reserved_23:1;
            /** num_clk_div_reg : RO; bitpos: [25:24]; default: 3;
             *  Have 4 clk divider in design .
             */
            uint32_t num_clk_div_reg:2;
            uint32_t reserved_26:6;
        };
        uint32_t val;
    } hcon;

    volatile union {
        struct {
            uint32_t voltage: 16;           ///< voltage control for slots; no-op on ESP32.
            uint32_t ddr: 16;                ///< bit N enables DDR mode for card N
        };
        uint32_t val;
    } uhs;              ///< UHS related settings

    volatile union {
        struct {
            uint32_t cards: 2;          ///< bit N resets card N, active low
            uint32_t reserved: 30;
        };
    } rst_n;

    uint32_t reserved_7c;

    volatile union {
        struct {
            uint32_t sw_reset: 1;       ///< set to reset DMA controller
            uint32_t fb: 1;             ///< set if AHB master performs fixed burst transfers
            uint32_t dsl: 5;            ///< descriptor skip length: number of words to skip between two unchained descriptors
            uint32_t enable: 1;         ///< set to enable IDMAC
            uint32_t pbl: 3;            ///< programmable burst length
            uint32_t reserved: 21;
        };
        uint32_t val;
    } bmod;

    volatile uint32_t pldmnd;                    ///< set any bit to resume IDMAC FSM from suspended state
    volatile sdmmc_desc_t* dbaddr;        ///< descriptor list base

    volatile union {
        struct {
            uint32_t ti: 1;         ///< transmit interrupt status
            uint32_t ri: 1;         ///< receive interrupt status
            uint32_t fbe: 1;        ///< fatal bus error
            uint32_t reserved1: 1;
            uint32_t du: 1;         ///< descriptor unavailable
            uint32_t ces: 1;        ///< card error summary
            uint32_t reserved2: 2;
            uint32_t nis: 1;        ///< normal interrupt summary
            uint32_t ais: 1;        ///< abnormal interrupt summary
            uint32_t fbe_code: 3;   ///< code of fatal bus error
            uint32_t fsm: 4;        ///< DMAC FSM state
            uint32_t reserved3: 15;
        };
        uint32_t val;
    } idsts;

    volatile union {
        struct {
            uint32_t ti: 1;         ///< transmit interrupt enable
            uint32_t ri: 1;         ///< receive interrupt enable
            uint32_t fbe: 1;        ///< fatal bus error interrupt enable
            uint32_t reserved1: 1;
            uint32_t du: 1;         ///< descriptor unavailable interrupt enable
            uint32_t ces: 1;        ///< card error interrupt enable
            uint32_t reserved2: 2;
            uint32_t ni: 1;         ///< normal interrupt interrupt enable
            uint32_t ai: 1;         ///< abnormal interrupt enable
            uint32_t reserved3: 22;
        };
        uint32_t val;
    } idinten;

    volatile uint32_t dscaddr;       ///< current host descriptor address
    volatile uint32_t dscaddrl;      ///< unused
    volatile uint32_t dscaddru;      ///< unused
    volatile uint32_t bufaddrl;      ///< unused
    volatile uint32_t bufaddru;      ///< unused
    volatile uint32_t reserved_a8[22];
    volatile union {
        struct {
            uint32_t read_thr_en : 1;       ///< initiate transfer only if FIFO has more space than the read threshold
            uint32_t busy_clr_int_en : 1;   ///< enable generation of busy clear interrupts
            uint32_t write_thr_en : 1;      ///< equivalent of read_thr_en for writes
            uint32_t reserved1 : 13;
            uint32_t card_threshold : 12;   ///< threshold value for reads/writes, in bytes
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } cardthrctl;
    volatile uint32_t back_end_power;
    volatile uint32_t uhs_reg_ext;
    volatile uint32_t emmc_ddr_reg;
    volatile uint32_t enable_shift;
    volatile uint32_t reserved_114[443];
    volatile union {
        struct {
            uint32_t phase_dout: 3;         ///< phase of data output clock (0x0: 0, 0x1: 90, 0x4: 180, 0x6: 270)
            uint32_t phase_din: 3;          ///< phase of data input clock
            uint32_t phase_core: 3;         ///< phase of the clock to SDMMC peripheral
            uint32_t div_factor_h: 4;       ///< controls length of high pulse; it will be (div_factor_h + 1) / 160MHz
            uint32_t div_factor_l: 4;       ///< controls clock period; it will be (div_factor_l + 1) / 160MHz
            uint32_t div_factor_n: 4;       ///< should be equal to div_factor_l
            uint32_t reserved21: 11;
        };
        uint32_t val;
    } clock;
} sdmmc_dev_t;
extern sdmmc_dev_t SDMMC;

#ifndef __cplusplus
_Static_assert(sizeof(sdmmc_dev_t) == 0x804, "invalid size of sdmmc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif

#endif //_SOC_SDMMC_STRUCT_H_
