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
    /**
     * These `reserved[12]` are for cache alignment. On P4, L1 Cache alignment is 64B.
     * For those who want to access the DMA descriptor in a non-cacheable way, you can
     * consider remove these `reserved[12]` bytes.
     */
    uint32_t reserved[12];
} sdmmc_desc_t;

#define SDMMC_DMA_MAX_BUF_LEN 4096

#ifndef __cplusplus
_Static_assert(sizeof(sdmmc_desc_t) == 64, "invalid size of sdmmc_desc_t structure");
#endif

/** Group: Control register */
/** Type of ctrl register
 *  Control register
 */
typedef union {
    struct {
        /** controller_reset : R/W; bitpos: [0]; default: 0;
         *  To reset controller, firmware should set this bit. This bit is auto-cleared after
         *  two AHB and two sdhost_cclk_in clock cycles.
         */
        uint32_t controller_reset:1;
        /** fifo_reset : R/W; bitpos: [1]; default: 0;
         *  To reset FIFO, firmware should set bit to 1. This bit is auto-cleared after
         *  completion of reset operation.
         *  Note: FIFO pointers will be out of reset after 2 cycles of system clocks in
         *  addition to synchronization delay (2 cycles of card clock), after the fifo_reset is
         *  cleared.
         */
        uint32_t fifo_reset:1;
        /** dma_reset : R/W; bitpos: [2]; default: 0;
         *  To reset DMA interface, firmware should set bit to 1. This bit is auto-cleared
         *  after two AHB clocks.
         */
        uint32_t dma_reset:1;
        uint32_t reserved_3:1;
        /** int_enable : R/W; bitpos: [4]; default: 0;
         *  Global interrupt enable/disable bit. 0: Disable; 1: Enable.
         */
        uint32_t int_enable:1;
        uint32_t dma_enable:1;
        /** read_wait : R/W; bitpos: [6]; default: 0;
         *  For sending read-wait to SDIO cards.
         */
        uint32_t read_wait:1;
        /** send_irq_response : R/W; bitpos: [7]; default: 0;
         *  Bit automatically clears once response is sent. To wait for MMC card interrupts,
         *  host issues CMD40 and waits for interrupt response from MMC card(s). In the
         *  meantime, if host wants SD/MMC to exit waiting for interrupt state, it can set this
         *  bit, at which time SD/MMC command state-machine sends CMD40 response on bus and
         *  returns to idle state.
         */
        uint32_t send_irq_response:1;
        /** abort_read_data : R/W; bitpos: [8]; default: 0;
         *  After a suspend-command is issued during a read-operation, software polls the card
         *  to find when the suspend-event occurred. Once the suspend-event has occurred,
         *  software sets the bit which will reset the data state machine that is waiting for
         *  the next block of data. This bit is automatically cleared once the data state
         *  machine is reset to idle.
         */
        uint32_t abort_read_data:1;
        /** send_ccsd : R/W; bitpos: [9]; default: 0;
         *  When set, SD/MMC sends CCSD to the CE-ATA device. Software sets this bit only if
         *  the current command is expecting CCS (that is, RW_BLK), and if interrupts are
         *  enabled for the CE-ATA device. Once the CCSD pattern is sent to the device, SD/MMC
         *  automatically clears the SDHOST_SEND_CCSD bit. It also sets the Command Done (CD)
         *  bit  in the SDHOST_RINTSTS_REG register, and generates an interrupt for the host,
         *  in case the Command Done interrupt is not masked.
         *  NOTE: Once the SDHOST_SEND_CCSD bit is set, it takes two card clock cycles to drive
         *  the CCSD on the CMD line. Due to this, within the boundary conditions the CCSD may
         *  be sent to the CE-ATA device, even if the device has signalled CCS.
         */
        uint32_t send_ccsd:1;
        /** send_auto_stop_ccsd : R/W; bitpos: [10]; default: 0;
         *  Always Set SDHOST_SEND_AUTO_STOP_CCSD and SDHOST_SEND_CCSD bits together;
         *  SDHOST_SEND_AUTO_STOP_CCSD should not be set independently of send_ccsd. When set,
         *  SD/MMC automatically sends an internally-generated STOP command (CMD12) to the
         *  CE-ATA device. After sending this internally-generated STOP command, the Auto
         *  Command Done (ACD) bit in SDHOST_RINTSTS_REG is set and an interrupt is generated
         *  for the host, in case the ACD interrupt is not masked. After sending the Command
         *  Completion Signal Disable (CCSD), SD/MMC automatically clears the
         *  SDHOST_SEND_AUTO_STOP_CCSD bit.
         */
        uint32_t send_auto_stop_ccsd:1;
        /** ceata_device_interrupt_status : R/W; bitpos: [11]; default: 0;
         *  Software should appropriately write to this bit after the power-on reset or any
         *  other reset to the CE-ATA device. After reset, the CE-ATA device's interrupt is
         *  usually disabled (nIEN = 1). If the host enables the CE-ATA device's interrupt,
         *  then software should set this bit.
         */
        uint32_t ceata_device_interrupt_status:1;
        uint32_t reserved2:4;
        uint32_t card_voltage_a:4;
        uint32_t card_voltage_b:4;
        uint32_t enable_od_pullup:1;
        uint32_t use_internal_dma:1;
        uint32_t reserved3:6;
    };
    uint32_t val;
} sdhost_ctrl_reg_t;


/** Group: Clock divider configuration register */
/** Type of clkdiv register
 *  Clock divider configuration register
 */
typedef union {
    struct {
        /** clk_divider0 : R/W; bitpos: [7:0]; default: 0;
         *  Clock divider0 value. Clock divisor is 2*n, where n = 0 bypasses the divider
         *  (divisor of 1). For example, a value of 1 means divided by 2*1 = 2, a value of 0xFF
         *  means divided by 2*255 = 510, and so on.
         */
        uint32_t clk_divider0:8;
        /** clk_divider1 : R/W; bitpos: [15:8]; default: 0;
         *  Clock divider1 value. Clock divisor is 2*n, where n = 0 bypasses the divider
         *  (divisor of 1). For example, a value of 1 means divided by 2*1 = 2, a value of 0xFF
         *  means divided by 2*255 = 510, and so on.
         */
        uint32_t clk_divider1:8;
        /** clk_divider2 : R/W; bitpos: [23:16]; default: 0;
         *  Clock divider2 value. Clock divisor is 2*n, where n = 0 bypasses the divider
         *  (divisor of 1). For example, a value of 1 means divided by 2*1 = 2, a value of 0xFF
         *  means divided by 2*255 = 510, and so on.
         */
        uint32_t clk_divider2:8;
        /** clk_divider3 : R/W; bitpos: [31:24]; default: 0;
         *  Clock divider3 value. Clock divisor is 2*n, where n = 0 bypasses the divider
         *  (divisor of 1). For example, a value of 1 means divided by 2*1 = 2, a value of 0xFF
         *  means divided by 2*255 = 510, and so on.
         */
        uint32_t clk_divider3:8;
    };
    uint32_t val;
} sdhost_clkdiv_reg_t;


/** Group: Clock source selection register */
/** Type of clksrc register
 *  Clock source selection register
 */
typedef union {
    struct {
        /** clksrc_reg : R/W; bitpos: [3:0]; default: 0;
         *  Clock divider source for two SD cards is supported. Each card has two bits assigned
         *  to it. For example, bit[1:0] are assigned for card 0, bit[3:2] are assigned for
         *  card 1. Card 0 maps and internally routes clock divider[0:3] outputs to
         *  cclk_out[1:0] pins, depending on bit value.
         *  00 : Clock divider 0;
         *  01 : Clock divider 1;
         *  10 : Clock divider 2;
         *  11 : Clock divider 3.
         */
        uint32_t card0:2;
        uint32_t card1:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} sdhost_clksrc_reg_t;


/** Group: Clock enable register */
/** Type of clkena register
 *  Clock enable register
 */
typedef union {
    struct {
        /** cclk_enable : R/W; bitpos: [1:0]; default: 0;
         *  Clock-enable control for two SD card clocks and one MMC card clock is supported.
         *  One bit per card.
         *  0: Clock disabled;
         *  1: Clock enabled.
         */
        uint32_t cclk_enable:2;
        uint32_t reserved_2:14;
        /** lp_enable : R/W; bitpos: [17:16]; default: 0;
         *  Disable clock when the card is in IDLE state. One bit per card.
         *  0: clock disabled;
         *  1: clock enabled.
         */
        uint32_t lp_enable:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} sdhost_clkena_reg_t;


/** Group: Data and response timeout configuration register */
/** Type of tmout register
 *  Data and response timeout configuration register
 */
typedef union {
    struct {
        /** response_timeout : R/W; bitpos: [7:0]; default: 64;
         *  Response timeout value. Value is specified in terms of number of card output
         *  clocks, i.e., sdhost_cclk_out.
         */
        uint32_t response_timeout:8;
        /** data_timeout : R/W; bitpos: [31:8]; default: 16777215;
         *  Value for card data read timeout. This value is also used for data starvation by
         *  host timeout. The timeout counter is started only after the card clock is stopped.
         *  This value is specified in number of card output clocks, i.e. sdhost_cclk_out of
         *  the selected card.
         *  NOTE: The software timer should be used if the timeout value is in the order of 100
         *  ms. In this case, read data timeout interrupt needs to be disabled.
         */
        uint32_t data_timeout:24;
    };
    uint32_t val;
} sdhost_tmout_reg_t;


/** Group: Card bus width configuration register */
/** Type of ctype register
 *  Card bus width configuration register
 */
typedef union {
    struct {
        /** card_width : R/W; bitpos: [1:0]; default: 0;
         *  One bit per card indicates if card is 1-bit or 4-bit mode.
         *  0: 1-bit mode;
         *  1: 4-bit mode.
         *  Bit[1:0] correspond to card[1:0] respectively.
         */
        uint32_t card_width:2;
        uint32_t reserved_2:14;
        /** card_width_8 : R/W; bitpos: [17:16]; default: 0;
         *  One bit per card indicates if card is in 8-bit mode.
         *  0: Non 8-bit mode;
         *  1: 8-bit mode.
         *  Bit[17:16] correspond to card[1:0] respectively.
         */
        uint32_t card_width_8:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} sdhost_ctype_reg_t;


/** Group: Card data block size configuration register */
/** Type of blksiz register
 *  Card data block size configuration register
 */
typedef union {
    struct {
        /** block_size : R/W; bitpos: [15:0]; default: 512;
         *  Block size.
         */
        uint32_t block_size:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} sdhost_blksiz_reg_t;


/** Group: Data transfer length configuration register */
/** Type of bytcnt register
 *  Data transfer length configuration register
 */
typedef union {
    struct {
        /** byte_count : R/W; bitpos: [31:0]; default: 512;
         *  Number of bytes to be transferred, should be an integral multiple of Block Size for
         *  block transfers. For data transfers of undefined byte lengths, byte count should be
         *  set to 0. When byte count is set to 0, it is the responsibility of host to
         *  explicitly send stop/abort command to terminate data transfer.
         */
        uint32_t byte_count:32;
    };
    uint32_t val;
} sdhost_bytcnt_reg_t;


/** Group: SDIO interrupt mask register */
/** Type of intmask register
 *  SDIO interrupt mask register
 */
typedef union {
    struct {
        /** int_mask : R/W; bitpos: [15:0]; default: 0;
         *  These bits used to mask unwanted interrupts. A value of 0 masks interrupt, and a
         *  value of 1 enables the interrupt.
         *  Bit 15 (EBE): End-bit error/no CRC error;
         *  Bit 14 (ACD): Auto command done;
         *  Bit 13 (SBE/BCI): Rx Start Bit Error;
         *  Bit 12 (HLE): Hardware locked write error;
         *  Bit 11 (FRUN): FIFO underrun/overrun error;
         *  Bit 10 (HTO): Data starvation-by-host timeout;
         *  Bit 9 (DRTO): Data read timeout;
         *  Bit 8 (RTO): Response timeout;
         *  Bit 7 (DCRC): Data CRC error;
         *  Bit 6 (RCRC): Response CRC error;
         *  Bit 5 (RXDR): Receive FIFO data request;
         *  Bit 4 (TXDR): Transmit FIFO data request;
         *  Bit 3 (DTO): Data transfer over;
         *  Bit 2 (CD): Command done;
         *  Bit 1 (RE): Response error;
         *  Bit 0 (CD): Card detect.
         */
        uint32_t int_mask:16;
        /** sdio_int_mask : R/W; bitpos: [17:16]; default: 0;
         *  SDIO interrupt mask, one bit for each card. Bit[17:16] correspond to card[15:0]
         *  respectively. When masked, SDIO interrupt detection for that card is disabled. 0
         *  masks an interrupt, and 1 enables an interrupt.
         */
        uint32_t sdio_int_mask:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} sdhost_intmask_reg_t;


/** Group: Command and boot configuration register */
/** Type of cmd register
 *  Command and boot configuration register
 */
typedef union {
    struct {
        /** cmd_index : R/W; bitpos: [5:0]; default: 0;
         *  Command index.
         */
        uint32_t cmd_index:6;
        /** response_expect : R/W; bitpos: [6]; default: 0;
         *  0: No response expected from card; 1: Response expected from card.
         */
        uint32_t response_expect:1;
        /** response_long : R/W; bitpos: [7]; default: 0;
         *  0: Short response expected from card; 1: Long response expected from card.
         */
        uint32_t response_long:1;
        /** check_response_crc : R/W; bitpos: [8]; default: 0;
         *  0: Do not check; 1: Check response CRC.
         *  Some of command responses do not return valid CRC bits. Software should disable CRC
         *  checks for those commands in order to disable CRC checking by controller.
         */
        uint32_t check_response_crc:1;
        /** data_expected : R/W; bitpos: [9]; default: 0;
         *  0: No data transfer expected; 1: Data transfer expected.
         */
        uint32_t data_expected:1;
        /** rw : R/W; bitpos: [10]; default: 0;
         *  0: Read from card; 1: Write to card.
         *  Don't care if no data is expected from card.
         */
        uint32_t rw:1;
        /** transfer_mode : R/W; bitpos: [11]; default: 0;
         *  0: Block data transfer command; 1: Stream data transfer command.
         *  Don't care if no data expected.
         */
        uint32_t transfer_mode:1;
        /** send_auto_stop : R/W; bitpos: [12]; default: 0;
         *  0: No stop command is sent at the end of data transfer; 1: Send stop command at the
         *  end of data transfer.
         */
        uint32_t send_auto_stop:1;
        /** wait_complete : R/W; bitpos: [13]; default: 0;
         *  0: Send command at once, even if previous data transfer has not completed; 1: Wait
         *  for previous data transfer to complete before sending Command.
         *  The SDHOST_WAIT_COMPLETE] = 0 option is typically used to query status of
         *  card during data transfer or to stop current data transfer. SDHOST_CARD_NUMBERr
         *  should be same as in previous command.
         */
        uint32_t wait_complete:1;
        /** stop_abort_cmd : R/W; bitpos: [14]; default: 0;
         *  0: Neither stop nor abort command can stop current data transfer. If abort is sent
         *  to function-number currently selected or not in data-transfer mode, then bit should
         *  be set to 0; 1: Stop or abort command intended to stop current data transfer in
         *  progress.
         *  When open-ended or predefined data transfer is in progress, and host issues stop or
         *  abort command to stop data transfer, bit should be set so that command/data
         *  state-machines of CIU can return correctly to idle state.
         */
        uint32_t stop_abort_cmd:1;
        /** send_init : R/W; bitpos: [15]; default: 0;
         *  0: Do not send initialization sequence (80 clocks of 1) before sending this
         *  command; 1: Send initialization sequence before sending this command.
         *  After powered on, 80 clocks must be sent to card for initialization before sending
         *  any commands to card. Bit should be set while sending first command to card so that
         *  controller will initialize clocks before sending command to card.
         */
        uint32_t send_init:1;
        /** card_num : R/W; bitpos: [20:16]; default: 0;
         *  Card number in use. Represents physical slot number of card being accessed. In
         *  SD-only mode, up to two cards are supported.
         */
        uint32_t card_num:5;
        /** update_clk_reg : R/W; bitpos: [21]; default: 0;
         *  0: Normal command sequence; 1: Do not send commands, just update clock register
         *  value into card clock domain.
         *  Following register values are transferred into card clock domain: CLKDIV, CLRSRC,
         *  and CLKENA.
         *  Changes card clocks (change frequency, truncate off or on, and set low-frequency
         *  mode). This is provided in order to change clock frequency or stop clock without
         *  having to send command to cards. During normal command sequence, when
         *  sdhost_update_clock_registers_only = 0, following control registers are transferred
         *  from BIU to CIU: CMD, CMDARG, TMOUT, CTYPE, BLKSIZ, and BYTCNT. CIU uses new
         *  register values for new command sequence to card(s). When bit is set, there are no
         *  Command Done interrupts because no command is sent to SD_MMC_CEATA cards.
         */
        uint32_t update_clk_reg:1;
        /** read_ceata_device : R/W; bitpos: [22]; default: 0;
         *  Read access flag.
         *  0: Host is not performing read access (RW_REG or RW_BLK)towards CE-ATA device;
         *  1: Host is performing read access (RW_REG or RW_BLK) towards CE-ATA device.
         *  Software should set this bit to indicate that CE-ATA device is being accessed for
         *  read transfer. This bit is used to disable read data timeout indication while
         *  performing CE-ATA read transfers. Maximum value of I/O transmission delay can be no
         *  less than 10 seconds. SD/MMC should not indicate read data timeout while waiting
         *  for data from CE-ATA device.
         */
        uint32_t read_ceata_device:1;
        /** ccs_expected : R/W; bitpos: [23]; default: 0;
         *  Expected Command Completion Signal (CCS) configuration.
         *  0: Interrupts are not enabled in CE-ATA device (nIEN = 1 in ATA control register),
         *  or command does not expect CCS from device;
         *  1: Interrupts are enabled in CE-ATA device (nIEN = 0), and RW_BLK command expects
         *  command completion signal from CE-ATA device.
         *  If the command expects Command Completion Signal (CCS) from the CE-ATA device, the
         *  software should set this control bit. SD/MMC sets Data Transfer Over (DTO) bit in
         *  RINTSTS register and generates interrupt to host if Data Transfer Over interrupt is
         *  not masked.
         */
        uint32_t ccs_expected:1;
        uint32_t reserved_24:5;
        /** use_hole_reg : R/W; bitpos: [29]; default: 1;
         *  Use Hold Register.
         *  0: CMD and DATA sent to card bypassing HOLD Register;
         *  1: CMD and DATA sent to card through the HOLD Register.
         */
        uint32_t use_hold_reg:1;
        uint32_t reserved_30:1;
        /** start_command : R/W; bitpos: [31]; default: 0;
         *  Start command. Once command is served by the CIU, this bit is automatically
         *  cleared. When this bit is set, host should not attempt to write to any command
         *  registers. If a write is attempted, hardware lock error is set in raw interrupt
         *  register. Once command is sent and a response is received from SD_MMC_CEATA cards,
         *  Command Done bit is set in the raw interrupt Register.
         */
        uint32_t start_command:1;
    };
    uint32_t val;
} sdhost_cmd_reg_t;


/** Group: Response data register */
/** Type of resp0 register
 *  Response data register
 */
typedef union {
    struct {
        /** response0_reg : RO; bitpos: [31:0]; default: 0;
         *  Bit[31:0] of response.
         */
        uint32_t response0_reg:32;
    };
    uint32_t val;
} sdhost_resp0_reg_t;


/** Group: Long response data register */
/** Type of resp1 register
 *  Long response data register
 */
typedef union {
    struct {
        /** response1_reg : RO; bitpos: [31:0]; default: 0;
         *  Bit[63:32] of long response.
         */
        uint32_t response1_reg:32;
    };
    uint32_t val;
} sdhost_resp1_reg_t;

/** Type of resp2 register
 *  Long response data register
 */
typedef union {
    struct {
        /** response2_reg : RO; bitpos: [31:0]; default: 0;
         *  Bit[95:64] of long response.
         */
        uint32_t response2_reg:32;
    };
    uint32_t val;
} sdhost_resp2_reg_t;

/** Type of resp3 register
 *  Long response data register
 */
typedef union {
    struct {
        /** response3_reg : RO; bitpos: [31:0]; default: 0;
         *  Bit[127:96] of long response.
         */
        uint32_t response3_reg:32;
    };
    uint32_t val;
} sdhost_resp3_reg_t;


/** Group: Masked interrupt status register */
/** Type of mintsts register
 *  Masked interrupt status register
 */
typedef union {
    struct {
        /** int_status_msk : RO; bitpos: [15:0]; default: 0;
         *  Interrupt enabled only if corresponding bit in interrupt mask register is set.
         *  Bit 15 (EBE): End-bit error/no CRC error;
         *  Bit 14 (ACD): Auto command done;
         *  Bit 13 (SBE/BCI): RX Start Bit Error;
         *  Bit 12 (HLE): Hardware locked write error;
         *  Bit 11 (FRUN): FIFO underrun/overrun error;
         *  Bit 10 (HTO): Data starvation by host timeout (HTO);
         *  Bit 9 (DTRO): Data read timeout;
         *  Bit 8 (RTO): Response timeout;
         *  Bit 7 (DCRC): Data CRC error;
         *  Bit 6 (RCRC): Response CRC error;
         *  Bit 5 (RXDR): Receive FIFO data request;
         *  Bit 4 (TXDR): Transmit FIFO data request;
         *  Bit 3 (DTO): Data transfer over;
         *  Bit 2 (CD): Command done;
         *  Bit 1 (RE): Response error;
         *  Bit 0 (CD): Card detect.
         */
        uint32_t int_status_msk:16;
        /** sdio_interrupt_msk : RO; bitpos: [17:16]; default: 0;
         *  Interrupt from SDIO card, one bit for each card. Bit[17:16] correspond to card1 and
         *  card0, respectively. SDIO interrupt for card is enabled only if corresponding
         *  sdhost_sdio_int_mask bit is set in Interrupt mask register (Setting mask bit
         *  enables interrupt).
         */
        uint32_t sdio_interrupt_msk:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} sdhost_mintsts_reg_t;


/** Group: Raw interrupt status register */
/** Type of rintsts register
 *  Raw interrupt status register
 */
typedef union {
    struct {
        /** int_status_raw : R/W; bitpos: [15:0]; default: 0;
         *  Setting a bit clears the corresponding interrupt and writing 0 has no effect. Bits
         *  are logged regardless of interrupt mask status.
         *  Bit 15 (EBE): End-bit error/no CRC error;
         *  Bit 14 (ACD): Auto command done;
         *  Bit 13 (SBE/BCI): RX Start Bit Error;
         *  Bit 12 (HLE): Hardware locked write error;
         *  Bit 11 (FRUN): FIFO underrun/overrun error;
         *  Bit 10 (HTO): Data starvation by host timeout (HTO);
         *  Bit 9 (DTRO): Data read timeout;
         *  Bit 8 (RTO): Response timeout;
         *  Bit 7 (DCRC): Data CRC error;
         *  Bit 6 (RCRC): Response CRC error;
         *  Bit 5 (RXDR): Receive FIFO data request;
         *  Bit 4 (TXDR): Transmit FIFO data request;
         *  Bit 3 (DTO): Data transfer over;
         *  Bit 2 (CD): Command done;
         *  Bit 1 (RE): Response error;
         *  Bit 0 (CD): Card detect.
         */
        uint32_t cd:1;
        uint32_t re:1;
        uint32_t cmd_done:1;
        uint32_t dto:1;
        uint32_t txdr:1;
        uint32_t rxdr:1;
        uint32_t rcrc:1;
        uint32_t dcrc:1;
        uint32_t rto:1;
        uint32_t drto:1;
        uint32_t hto:1;
        uint32_t frun:1;
        uint32_t hle:1;
        uint32_t sbi_bci:1;
        uint32_t acd:1;
        uint32_t ebe:1;
        /** sdio_interrupt_raw : R/W; bitpos: [17:16]; default: 0;
         *  Interrupt from SDIO card, one bit for each card. Bit[17:16] correspond to card1 and
         *  card0, respectively. Setting a bit clears the corresponding interrupt bit and
         *  writing 0 has no effect.
         *  0: No SDIO interrupt from card;
         *  1: SDIO interrupt from card.
         */
        uint32_t sdio_interrupt_raw:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} sdhost_rintsts_reg_t;


/** Group: SD/MMC status register */
/** Type of status register
 *  SD/MMC status register
 */
typedef union {
    struct {
        /** fifo_rx_watermark : RO; bitpos: [0]; default: 0;
         *  FIFO reached Receive watermark level, not qualified with data transfer.
         */
        uint32_t fifo_rx_watermark:1;
        /** fifo_tx_watermark : RO; bitpos: [1]; default: 1;
         *  FIFO reached Transmit watermark level, not qualified with data transfer.
         */
        uint32_t fifo_tx_watermark:1;
        /** fifo_empty : RO; bitpos: [2]; default: 1;
         *  FIFO is empty status.
         */
        uint32_t fifo_empty:1;
        /** fifo_full : RO; bitpos: [3]; default: 0;
         *  FIFO is full status.
         */
        uint32_t fifo_full:1;
        /** command_fsm_states : RO; bitpos: [7:4]; default: 1;
         *  Command FSM states.
         *  0: Idle;
         *  1: Send init sequence;
         *  2: Send cmd start bit;
         *  3: Send cmd tx bit;
         *  4: Send cmd index + arg;
         *  5: Send cmd crc7;
         *  6: Send cmd end bit;
         *  7: Receive resp start bit;
         *  8: Receive resp IRQ response;
         *  9: Receive resp tx bit;
         *  10: Receive resp cmd idx;
         *  11: Receive resp data;
         *  12: Receive resp crc7;
         *  13: Receive resp end bit;
         *  14: Cmd path wait NCC;
         *  15: Wait, cmd-to-response turnaround.
         */
        uint32_t command_fsm_states:4;
        /** data_3_status : RO; bitpos: [8]; default: 1;
         *  Raw selected sdhost_card_data[3], checks whether card is present.
         *  0: card not present;
         *  1: card present.
         */
        uint32_t data_3_status:1;
        /** data_busy : RO; bitpos: [9]; default: 1;
         *  Inverted version of raw selected sdhost_card_data[0].
         *  0: Card data not busy;
         *  1: Card data busy.
         */
        uint32_t data_busy:1;
        /** data_state_mc_busy : RO; bitpos: [10]; default: 1;
         *  Data transmit or receive state-machine is busy.
         */
        uint32_t data_state_mc_busy:1;
        /** response_index : RO; bitpos: [16:11]; default: 0;
         *  Index of previous response, including any auto-stop sent by core.
         */
        uint32_t response_index:6;
        /** fifo_count : RO; bitpos: [29:17]; default: 0;
         *  FIFO count, number of filled locations in FIFO.
         */
        uint32_t fifo_count:13;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} sdhost_status_reg_t;


/** Group: FIFO configuration register */
/** Type of fifoth register
 *  FIFO configuration register
 */
typedef union {
    struct {
        /** tx_wmark : R/W; bitpos: [11:0]; default: 0;
         *  FIFO threshold watermark level when transmitting data to card. When FIFO data count
         *  is less than or equal to this number, DMA/FIFO request is raised. If Interrupt is
         *  enabled, then interrupt  occurs. During end of packet, request or interrupt is
         *  generated, regardless of threshold programming.In non-DMA mode, when transmit FIFO
         *  threshold (TXDR) interrupt is enabled, then interrupt is generated instead of DMA
         *  request. During end of packet, on last interrupt, host is responsible for filling
         *  FIFO with only required remaining bytes (not before FIFO is full or after CIU
         *  completes data transfers, because FIFO may not be empty).  In DMA mode, at end of
         *  packet, if last transfer is less than burst  size, DMA controller does single
         *  cycles until required bytes are  transferred.
         */
        uint32_t tx_wmark:12;
        uint32_t reserved_12:4;
        /** rx_wmark : R/W; bitpos: [26:16]; default: 0;
         *  FIFO threshold watermark level when receiving data to card.When FIFO data count
         *  reaches greater than this number , DMA/FIFO request is raised. During end of
         *  packet, request is generated regardless of threshold programming in order to
         *  complete any remaining data.In non-DMA mode, when receiver FIFO threshold (RXDR)
         *  interrupt is enabled, then interrupt is generated instead of DMA request.During end
         *  of packet, interrupt is not generated if threshold programming is larger than any
         *  remaining data. It is responsibility of host to read remaining bytes on seeing Data
         *  Transfer Done interrupt.In DMA mode, at end of packet, even if remaining bytes are
         *  less than threshold, DMA request does single transfers to flush out any remaining
         *  bytes before Data Transfer Done interrupt is set.
         */
        uint32_t rx_wmark:11;
        uint32_t reserved_27:1;
        /** dma_multiple_transaction_size : R/W; bitpos: [30:28]; default: 0;
         *  Burst size of multiple transaction, should be programmed same as DMA controller
         *  multiple-transaction-size SDHOST_SRC/DEST_MSIZE.
         *  000: 1-byte transfer;
         *  001: 4-byte transfer;
         *  010: 8-byte transfer;
         *  011: 16-byte transfer;
         *  100: 32-byte transfer;
         *  101: 64-byte transfer;
         *  110: 128-byte transfer;
         *  111: 256-byte transfer.
         */
        uint32_t dma_multiple_transaction_size:3;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} sdhost_fifoth_reg_t;


/** Group: Card detect register */
/** Type of cdetect register
 *  Card detect register
 */
typedef union {
    struct {
        /** card_detect_n : RO; bitpos: [1:0]; default: 0;
         *  Value on sdhost_card_detect_n input ports (1 bit per card), read-only bits. 0
         *  represents presence of card. Only NUM_CARDS number of bits are implemented.
         */
        uint32_t card_detect_n:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sdhost_cdetect_reg_t;


/** Group: Card write protection (WP) status register */
/** Type of wrtprt register
 *  Card write protection (WP) status register
 */
typedef union {
    struct {
        /** write_protect : RO; bitpos: [1:0]; default: 0;
         *  Value on sdhost_card_write_prt input ports (1 bit per card). 1 represents write
         *  protection. Only NUM_CARDS number of bits are implemented.
         */
        uint32_t write_protect:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sdhost_wrtprt_reg_t;


/** Group: Transferred byte count register */
/** Type of tcbcnt register
 *  Transferred byte count register
 */
typedef union {
    struct {
        /** tcbcnt_reg : RO; bitpos: [31:0]; default: 0;
         *  Number of bytes transferred by CIU unit to card.
         */
        uint32_t tcbcnt_reg:32;
    };
    uint32_t val;
} sdhost_tcbcnt_reg_t;

/** Type of tbbcnt register
 *  Transferred byte count register
 */
typedef union {
    struct {
        /** tbbcnt_reg : RO; bitpos: [31:0]; default: 0;
         *  Number of bytes transferred between Host/DMA memory and BIU FIFO.
         */
        uint32_t tbbcnt_reg:32;
    };
    uint32_t val;
} sdhost_tbbcnt_reg_t;


/** Group: Debounce filter time configuration register */
/** Type of debnce register
 *  Debounce filter time configuration register
 */
typedef union {
    struct {
        /** debounce_count : R/W; bitpos: [23:0]; default: 0;
         *  Number of host clocks (clk) used by debounce filter logic. The typical debounce
         *  time is 5 \verb+~+ 25 ms to prevent the card instability when the card is inserted
         *  or removed.
         */
        uint32_t debounce_count:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} sdhost_debnce_reg_t;


/** Group: User ID (scratchpad) register */
/** Type of usrid register
 *  User ID (scratchpad) register
 */
typedef union {
    struct {
        /** usrid_reg : R/W; bitpos: [31:0]; default: 0;
         *  User identification register, value set by user. Can also be used as a scratchpad
         *  register by user.
         */
        uint32_t usrid_reg:32;
    };
    uint32_t val;
} sdhost_usrid_reg_t;


/** Group: Hardware feature register */
/** Type of hcon register
 *  Hardware feature register
 */
typedef union {
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
         *  Regisger data widht is 32.
         */
        uint32_t data_width_reg:3;
        /** addr_width_reg : RO; bitpos: [15:10]; default: 19;
         *  Register address width is 32.
         */
        uint32_t addr_width_reg:6;
        uint32_t reserved_16:2;
        /** dma_width_reg : RO; bitpos: [20:18]; default: 1;
         *  DMA data witdth is 32.
         */
        uint32_t dma_width_reg:3;
        /** ram_indise_reg : RO; bitpos: [21]; default: 0;
         *  Inside RAM in SDMMC module.
         */
        uint32_t ram_indise_reg:1;
        /** hold_reg : RO; bitpos: [22]; default: 1;
         *  Have a hold regiser in data path .
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
} sdhost_hcon_reg_t;


/** Group: UHS-1 register */
/** Type of uhs register
 *  UHS-1 register
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** ddr : R/W; bitpos: [17:16]; default: 0;
         *  DDR mode selecton,1 bit for each card.
         *  0-Non-DDR mdoe.
         *  1-DDR mdoe.
         */
        uint32_t ddr:2;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} sdhost_uhs_reg_t;


/** Group: Card reset register */
/** Type of rst_n register
 *  Card reset register
 */
typedef union {
    struct {
        /** card_reset : R/W; bitpos: [1:0]; default: 1;
         *  Hardware reset.
         *  1: Active mode;
         *  0: Reset.
         *  These bits cause the cards to enter pre-idle state, which requires them to be
         *  re-initialized. SDHOST_RST_CARD_RESET[0] should be set to 1'b0 to reset card0,
         *  SDHOST_RST_CARD_RESET[1] should be set to 1'b0 to reset card1.
         */
        uint32_t card_reset:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sdhost_rst_n_reg_t;


/** Group: Burst mode transfer configuration register */
/** Type of bmod register
 *  Burst mode transfer configuration register
 */
typedef union {
    struct {
        /** sw_reset : R/W; bitpos: [0]; default: 0;
         *  Software Reset. When set, the DMA Controller resets all its internal registers. It
         *  is automatically cleared after one clock cycle.
         */
        uint32_t sw_reset:1;
        /** fb : R/W; bitpos: [1]; default: 0;
         *  Fixed Burst. Controls whether the AHB Master interface performs fixed burst
         *  transfers or not. When set, the AHB will use only SINGLE, INCR4, INCR8 or INCR16
         *  during start of normal burst transfers. When reset, the AHB will use SINGLE and
         *  INCR burst transfer operations.
         */
        uint32_t fb:1;
        uint32_t reserved_2:5;
        /** enable : R/W; bitpos: [7]; default: 0;
         *  IDMAC Enable. When set, the IDMAC is enabled.
         */
        uint32_t enable:1;
        /** bmod_pbl : R/W; bitpos: [10:8]; default: 0;
         *  Programmable Burst Length. These bits indicate the maximum number of beats to be
         *  performed in one IDMAC???Internal DMA Control???transaction. The IDMAC will always
         *  attempt to burst as specified in PBL each time it starts a burst transfer on the
         *  host bus. The permissible values are 1, 4, 8, 16, 32, 64, 128 and 256. This value
         *  is the mirror of MSIZE of FIFOTH register. In order to change this value, write the
         *  required value to FIFOTH register. This is an encode value as follows:
         *  000: 1-byte transfer;
         *  001: 4-byte transfer;
         *  010: 8-byte transfer;
         *  011: 16-byte transfer;
         *  100: 32-byte transfer;
         *  101: 64-byte transfer;
         *  110: 128-byte transfer;
         *  111: 256-byte transfer.
         *  PBL is a read-only value and is applicable only for data access, it does not apply
         *  to descriptor access.
         */
        uint32_t bmod_pbl:3;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} sdhost_bmod_reg_t;


/** Group: Poll demand configuration register */
/** Type of pldmnd register
 *  Poll demand configuration register
 */
typedef union {
    struct {
        /** pldmnd_pd : WO; bitpos: [31:0]; default: 0;
         *  Poll Demand. If the OWNER bit of a descriptor is not set, the FSM goes to the
         *  Suspend state. The host needs to write any value into this register for the IDMAC
         *  FSM to resume normal descriptor fetch operation. This is a write only .
         */
        uint32_t pldmnd_pd:32;
    };
    uint32_t val;
} sdhost_pldmnd_reg_t;


/** Group: Descriptor base address register */
/** Type of dbaddr register
 *  Descriptor base address register
 */
typedef union {
    struct {
        /** dbaddr_reg : R/W; bitpos: [31:0]; default: 0;
         *  Start of Descriptor List. Contains the base address of the First Descriptor. The
         *  LSB bits [1:0] are ignored and taken as all-zero by the IDMAC internally. Hence
         *  these LSB bits may be treated as read-only.
         */
        uint32_t dbaddr_reg:32;
    };
    uint32_t val;
} sdhost_dbaddr_reg_t;


/** Group: IDMAC status register */
/** Type of idsts register
 *  IDMAC status register
 */
typedef union {
    struct {
        /** idsts_ti : R/W; bitpos: [0]; default: 0;
         *  Transmit Interrupt. Indicates that data transmission is finished for a descriptor.
         *  Writing 1 clears this bit.
         */
        uint32_t idsts_ti:1;
        /** idsts_ri : R/W; bitpos: [1]; default: 0;
         *  Receive Interrupt. Indicates the completion of data reception for a descriptor.
         *  Writing 1 clears this bit.
         */
        uint32_t idsts_ri:1;
        /** idsts_fbe : R/W; bitpos: [2]; default: 0;
         *  Fatal Bus Error Interrupt. Indicates that a Bus Error occurred (IDSTS[12:10]) .
         *  When this bit is set, the DMA disables all its bus accesses. Writing 1 clears this
         *  bit.
         */
        uint32_t idsts_fbe:1;
        uint32_t reserved_3:1;
        /** idsts_du : R/W; bitpos: [4]; default: 0;
         *  Descriptor Unavailable Interrupt. This bit is set when the descriptor is
         *  unavailable due to OWNER bit = 0 (DES0[31] = 0). Writing 1 clears this bit.
         */
        uint32_t idsts_du:1;
        /** idsts_ces : R/W; bitpos: [5]; default: 0;
         *  Card Error Summary. Indicates the status of the transaction to/from the card, also
         *  present in RINTSTS. Indicates the logical OR of the following bits:
         *  EBE : End Bit Error;
         *  RTO : Response Timeout/Boot Ack Timeout;
         *  RCRC : Response CRC;
         *  SBE : Start Bit Error;
         *  DRTO : Data Read Timeout/BDS timeout;
         *  DCRC : Data CRC for Receive;
         *  RE : Response Error.
         *  Writing 1 clears this bit. The abort condition of the IDMAC depends on the setting
         *  of this CES bit. If the CES bit is enabled, then the IDMAC aborts on a response
         *  error.
         */
        uint32_t idsts_ces:1;
        uint32_t reserved_6:2;
        /** idsts_nis : R/W; bitpos: [8]; default: 0;
         *  Normal Interrupt Summary. Logical OR of the following: IDSTS[0] : Transmit
         *  Interrupt, IDSTS[1] : Receive Interrupt. Only unmasked bits affect this bit. This
         *  is a sticky bit and must be cleared each time a corresponding bit that causes NIS
         *  to be set is cleared. Writing 1 clears this bit.
         */
        uint32_t idsts_nis:1;
        /** idsts_ais : R/W; bitpos: [9]; default: 0;
         *  Abnormal Interrupt Summary. Logical OR of the following: IDSTS[2] : Fatal Bus
         *  Interrupt, IDSTS[4] : DU bit Interrupt. Only unmasked bits affect this bit. This is
         *  a sticky bit and must be cleared each time a corresponding bit that causes AIS to
         *  be set is cleared. Writing 1 clears this bit.
         */
        uint32_t idsts_ais:1;
        /** idsts_fbe_code : R/W; bitpos: [12:10]; default: 0;
         *  Fatal Bus Error Code. Indicates the type of error that caused a Bus Error. Valid
         *  only when the Fatal Bus Error bit IDSTS[2] is set. This field does not generate an
         *  interrupt.
         *  001: Host Abort received during transmission;
         *  010: Host Abort received during reception;
         *  Others: Reserved.
         */
        uint32_t idsts_fbe_code:3;
        /** idsts_fsm : R/W; bitpos: [16:13]; default: 0;
         *  DMAC FSM present state.
         *  0: DMA_IDLE (idle state);
         *  1: DMA_SUSPEND (suspend state);
         *  2: DESC_RD (descriptor reading state);
         *  3: DESC_CHK (descriptor checking state);
         *  4: DMA_RD_REQ_WAIT (read-data request waiting state);
         *  5: DMA_WR_REQ_WAIT (write-data request waiting state);
         *  6: DMA_RD (data-read state);
         *  7: DMA_WR (data-write state);
         *  8: DESC_CLOSE (descriptor close state).
         */
        uint32_t idsts_fsm:4;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} sdhost_idsts_reg_t;


/** Group: IDMAC interrupt enable register */
/** Type of idinten register
 *  IDMAC interrupt enable register
 */
typedef union {
    struct {
        /** ti : R/W; bitpos: [0]; default: 0;
         *  Transmit Interrupt Enable. When set with Normal Interrupt Summary Enable, Transmit
         *  Interrupt is enabled. When reset, Transmit Interrupt is disabled.
         */
        uint32_t ti:1;
        /** ri : R/W; bitpos: [1]; default: 0;
         *  Receive Interrupt Enable. When set with Normal Interrupt Summary Enable, Receive
         *  Interrupt is enabled. When reset, Receive Interrupt is disabled.
         */
        uint32_t ri:1;
        /** idinten_fbe : R/W; bitpos: [2]; default: 0;
         *  Fatal Bus Error Enable. When set with Abnormal Interrupt Summary Enable, the Fatal
         *  Bus Error Interrupt is enabled. When reset, Fatal Bus Error Enable Interrupt is
         *  disabled.
         */
        uint32_t idinten_fbe:1;
        uint32_t reserved_3:1;
        /** idinten_du : R/W; bitpos: [4]; default: 0;
         *  Descriptor Unavailable Interrupt. When set along with Abnormal Interrupt Summary
         *  Enable, the DU interrupt is enabled.
         */
        uint32_t idinten_du:1;
        /** idinten_ces : R/W; bitpos: [5]; default: 0;
         *  Card Error summary Interrupt Enable. When set, it enables the Card Interrupt
         *  summary.
         */
        uint32_t idinten_ces:1;
        uint32_t reserved_6:2;
        /** ni : R/W; bitpos: [8]; default: 0;
         *  Normal Interrupt Summary Enable. When set, a normal interrupt is enabled. When
         *  reset, a normal interrupt is disabled. This bit enables the following bits:
         *  IDINTEN[0]: Transmit Interrupt;
         *  IDINTEN[1]: Receive Interrupt.
         */
        uint32_t ni:1;
        /** idinten_ai : R/W; bitpos: [9]; default: 0;
         *  Abnormal Interrupt Summary Enable. When set, an abnormal interrupt is enabled. This
         *  bit enables the following bits:
         *  IDINTEN[2]: Fatal Bus Error Interrupt;
         *  IDINTEN[4]: DU Interrupt.
         */
        uint32_t idinten_ai:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} sdhost_idinten_reg_t;


/** Group: Host descriptor address pointer */
/** Type of dscaddr register
 *  Host descriptor address pointer
 */
typedef union {
    struct {
        /** dscaddr_reg : RO; bitpos: [31:0]; default: 0;
         *  Host Descriptor Address Pointer, updated by IDMAC during operation and cleared on
         *  reset. This register points to the start address of the current descriptor read by
         *  the IDMAC.
         */
        uint32_t dscaddr_reg:32;
    };
    uint32_t val;
} sdhost_dscaddr_reg_t;


/** Group: Host buffer address pointer register */
/** Type of bufaddr register
 *  Host buffer address pointer register
 */
typedef union {
    struct {
        /** bufaddr_reg : RO; bitpos: [31:0]; default: 0;
         *  Host Buffer Address Pointer, updated by IDMAC during operation and cleared on
         *  reset. This register points to the current Data Buffer Address being accessed by
         *  the IDMAC.
         */
        uint32_t bufaddr_reg:32;
    };
    uint32_t val;
} sdhost_bufaddr_reg_t;


/** Group: Card Threshold Control register */
/** Type of cardthrctl register
 *  Card Threshold Control register
 */
typedef union {
    struct {
        /** cardrdthren_reg : R/W; bitpos: [0]; default: 0;
         *  Card read threshold enable.
         *  1'b0-Card read threshold disabled.
         *  1'b1-Card read threshold enabled.
         */
        uint32_t cardrdthren_reg:1;
        /** busy_clr_int_en : R/W; bitpos: [1]; default: 0;
         *  Busy clear interrupt generation:
         *  1'b0-Busy clear interrypt disabled.
         *  1'b1-Busy clear interrypt enabled.
         */
        uint32_t busy_clr_int_en:1;
        /** cardwrthren_reg : R/W; bitpos: [2]; default: 0;
         *  Applicable when HS400 mode is enabled.
         *  1'b0-Card write Threshold disabled.
         *  1'b1-Card write Threshold enabled.
         */
        uint32_t cardwrthren_reg:1;
        uint32_t reserved_3:13;
        /** cardthreshold_reg : R/W; bitpos: [31:16]; default: 0;
         *  The inside FIFO size is 512,This register is applicable when SDHOST_CARDERTHREN_REG
         *  is set to 1 or SDHOST_CARDRDTHREN_REG set to 1.
         */
        uint32_t cardthreshold_reg:16;
    };
    uint32_t val;
} sdhost_cardthrctl_reg_t;


/** Group: eMMC DDR register */
/** Type of emmcddr register
 *  eMMC DDR register
 */
typedef union {
    struct {
        /** halfstartbit_reg : R/W; bitpos: [1:0]; default: 0;
         *  Control for start bit detection mechanism duration of start bit.Each bit refers to
         *  one slot.Set this bit to 1 for eMMC4.5 and above,set to 0 for SD applications.For
         *  eMMC4.5,start bit can be:
         *  1'b0-Full cycle.
         *  1'b1-less than one full cycle.
         */
        uint32_t halfstartbit_reg:2;
        uint32_t reserved_2:29;
        /** hs400_mode_reg : R/W; bitpos: [31]; default: 0;
         *  Set 1 to enable HS400 mode.
         */
        uint32_t hs400_mode_reg:1;
    };
    uint32_t val;
} sdhost_emmcddr_reg_t;


/** Group: Enable Phase Shift register */
/** Type of enshift register
 *  Enable Phase Shift register
 */
typedef union {
    struct {
        /** enable_shift_reg : R/W; bitpos: [3:0]; default: 0;
         *  Control for the amount of phase shift provided on the default enables in the
         *  design.Two bits assigned for each card.
         *  2'b00-Default phase shift.
         *  2'b01-Enables shifted to next immediate positive edge.
         *  2'b10-Enables shifted to next immediate negative edge.
         *  2'b11-Reserved.
         */
        uint32_t enable_shift_reg:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} sdhost_enshift_reg_t;


/** Group: CPU write and read transmit data by FIFO */
/** Type of buffifo register
 *  CPU write and read transmit data by FIFO
 */
typedef union {
    struct {
        /** buffifo_reg : R/W; bitpos: [31:0]; default: 0;
         *  CPU write and read transmit data by FIFO. This register points to the current Data
         *  FIFO .
         */
        uint32_t buffifo_reg:32;
    };
    uint32_t val;
} sdhost_buffifo_reg_t;


/** Group: SDIO Control and configuration registers */
/** Type of clk_edge_sel register
 *  SDIO control register.
 */
typedef union {
    struct {
        /** cclkin_edge_drv_sel : R/W; bitpos: [2:0]; default: 0;
         *  It's used to select the clock phase of the output signal from phase 0, phase 90,
         *  phase 180, phase 270.
         */
        uint32_t cclkin_edge_drv_sel:3;
        /** cclkin_edge_sam_sel : R/W; bitpos: [5:3]; default: 0;
         *  It's used to select the clock phase of the input signal from phase 0, phase 90,
         *  phase 180, phase 270.
         */
        uint32_t cclkin_edge_sam_sel:3;
        /** cclkin_edge_slf_sel : R/W; bitpos: [8:6]; default: 0;
         *  It's used to select the clock phase of the internal signal from phase 0, phase 90,
         *  phase 180, phase 270.
         */
        uint32_t cclkin_edge_slf_sel:3;
        /** ccllkin_edge_h : R/W; bitpos: [12:9]; default: 1;
         *  The high level of the divider clock. The value should be smaller than CCLKIN_EDGE_L.
         */
        uint32_t ccllkin_edge_h:4;
        /** ccllkin_edge_l : R/W; bitpos: [16:13]; default: 0;
         *  The low level of the divider clock. The value should be larger than CCLKIN_EDGE_H.
         */
        uint32_t ccllkin_edge_l:4;
        /** ccllkin_edge_n : R/W; bitpos: [20:17]; default: 1;
         *  The clock division of cclk_in.
         */
        uint32_t ccllkin_edge_n:4;
        /** esdio_mode : R/W; bitpos: [21]; default: 0;
         *  Enable esdio mode.
         */
        uint32_t esdio_mode:1;
        /** esd_mode : R/W; bitpos: [22]; default: 0;
         *  Enable esd mode.
         */
        uint32_t esd_mode:1;
        /** cclk_en : R/W; bitpos: [23]; default: 1;
         *  Sdio clock enable.
         */
        uint32_t cclk_en:1;
        /** ultra_high_speed_mode : R/W; bitpos: [24]; default: 0;
         *  Enable ultra high speed mode, use dll to generate clk.
         */
        uint32_t ultra_high_speed_mode:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} sdhost_clk_edge_sel_reg_t;


/** Group: SDIO raw ints registers */
/** Type of raw_ints register
 *  SDIO raw ints register.
 */
typedef union {
    struct {
        /** raw_ints : RO; bitpos: [31:0]; default: 0;
         *  It indicates raw ints.
         */
        uint32_t raw_ints:32;
    };
    uint32_t val;
} sdhost_raw_ints_reg_t;


/** Group: SDIO dll clock control registers */
/** Type of dll_clk_conf register
 *  SDIO DLL clock control register.
 */
typedef union {
    struct {
        /** dll_cclk_in_slf_en : R/W; bitpos: [0]; default: 0;
         *  Clock enable of cclk_in_slf when ULTRA_HIGH_SPEED_MODE==1.
         */
        uint32_t dll_cclk_in_slf_en:1;
        /** dll_cclk_in_drv_en : R/W; bitpos: [1]; default: 0;
         *  Clock enable of cclk_in_drv when ULTRA_HIGH_SPEED_MODE==1.
         */
        uint32_t dll_cclk_in_drv_en:1;
        /** dll_cclk_in_sam_en : R/W; bitpos: [2]; default: 0;
         *  Clock enable of cclk_in_sam when ULTRA_HIGH_SPEED_MODE==1.
         */
        uint32_t dll_cclk_in_sam_en:1;
        /** dll_cclk_in_slf_phase : R/W; bitpos: [8:3]; default: 0;
         *  It's used to control the phase of cclk_in_slf when ULTRA_HIGH_SPEED_MODE==1.
         */
        uint32_t dll_cclk_in_slf_phase:6;
        /** dll_cclk_in_drv_phase : R/W; bitpos: [14:9]; default: 0;
         *  It's used to control the phase of cclk_in_drv when ULTRA_HIGH_SPEED_MODE==1.
         */
        uint32_t dll_cclk_in_drv_phase:6;
        /** dll_cclk_in_sam_phase : R/W; bitpos: [20:15]; default: 0;
         *  It's used to control the phase of cclk_in_sam when ULTRA_HIGH_SPEED_MODE==1.
         */
        uint32_t dll_cclk_in_sam_phase:6;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} sdhost_dll_clk_conf_reg_t;


/** Group: SDIO dll configuration registers */
/** Type of dll_conf register
 *  SDIO DLL configuration register.
 */
typedef union {
    struct {
        /** dll_cal_stop : R/W; bitpos: [0]; default: 0;
         *  Set 1 to stop calibration.
         */
        uint32_t dll_cal_stop:1;
        /** dll_cal_end : RO; bitpos: [1]; default: 0;
         *  1 means calibration finished.
         */
        uint32_t dll_cal_end:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} sdhost_dll_conf_reg_t;


typedef struct sdmmc_dev_t {
    volatile sdhost_ctrl_reg_t ctrl;
    uint32_t reserved_004;
    volatile sdhost_clkdiv_reg_t clkdiv;
    volatile sdhost_clksrc_reg_t clksrc;
    volatile sdhost_clkena_reg_t clkena;
    volatile sdhost_tmout_reg_t tmout;
    volatile sdhost_ctype_reg_t ctype;
    volatile sdhost_blksiz_reg_t blksiz;
    volatile sdhost_bytcnt_reg_t bytcnt;
    volatile sdhost_intmask_reg_t intmask;
    volatile uint32_t cmdarg;
    volatile sdhost_cmd_reg_t cmd;
    volatile uint32_t resp[4];           ///< Response from card
    volatile sdhost_mintsts_reg_t mintsts;
    volatile sdhost_rintsts_reg_t rintsts;
    volatile sdhost_status_reg_t status;
    volatile sdhost_fifoth_reg_t fifoth;
    volatile sdhost_cdetect_reg_t cdetect;
    volatile sdhost_wrtprt_reg_t wrtprt;
    uint32_t reserved_058;
    volatile sdhost_tcbcnt_reg_t tcbcnt;
    volatile sdhost_tbbcnt_reg_t tbbcnt;
    volatile sdhost_debnce_reg_t debnce;
    volatile sdhost_usrid_reg_t usrid;
    volatile uint32_t verid;
    volatile sdhost_hcon_reg_t hcon;
    volatile sdhost_uhs_reg_t uhs;
    volatile sdhost_rst_n_reg_t rst_n;
    uint32_t reserved_07c;
    volatile sdhost_bmod_reg_t bmod;
    volatile sdhost_pldmnd_reg_t pldmnd;
    volatile sdhost_dbaddr_reg_t dbaddr;
    volatile sdhost_idsts_reg_t idsts;
    volatile sdhost_idinten_reg_t idinten;
    volatile sdhost_dscaddr_reg_t dscaddr;
    volatile sdhost_bufaddr_reg_t bufaddr;
    uint32_t reserved_09c[25];
    volatile sdhost_cardthrctl_reg_t cardthrctl;
    uint32_t reserved_104[2];
    volatile sdhost_emmcddr_reg_t emmcddr;
    volatile sdhost_enshift_reg_t enshift;
    uint32_t reserved_114[59];
    volatile sdhost_buffifo_reg_t buffifo;
    uint32_t reserved_204[383];
    volatile sdhost_clk_edge_sel_reg_t clk_edge_sel;
    volatile sdhost_raw_ints_reg_t raw_ints;
    volatile sdhost_dll_clk_conf_reg_t dll_clk_conf;
    volatile sdhost_dll_conf_reg_t dll_conf;
} sdmmc_dev_t;

extern sdmmc_dev_t SDMMC;

typedef sdhost_cmd_reg_t sdmmc_hw_cmd_t;

#ifndef __cplusplus
_Static_assert(sizeof(sdmmc_dev_t) == 0x810, "Invalid size of sdmmc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
