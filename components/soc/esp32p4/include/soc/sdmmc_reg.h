/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** SDHOST_CTRL_REG register
 *  Control register
 */
#define SDHOST_CTRL_REG (DR_REG_SDHOST_BASE + 0x0)
/** SDHOST_CONTROLLER_RESET : R/W; bitpos: [0]; default: 0;
 *  To reset controller, firmware should set this bit. This bit is auto-cleared after
 *  two AHB and two sdhost_cclk_in clock cycles.
 */
#define SDHOST_CONTROLLER_RESET    (BIT(0))
#define SDHOST_CONTROLLER_RESET_M  (SDHOST_CONTROLLER_RESET_V << SDHOST_CONTROLLER_RESET_S)
#define SDHOST_CONTROLLER_RESET_V  0x00000001U
#define SDHOST_CONTROLLER_RESET_S  0
/** SDHOST_FIFO_RESET : R/W; bitpos: [1]; default: 0;
 *  To reset FIFO, firmware should set bit to 1. This bit is auto-cleared after
 *  completion of reset operation.
 *  Note: FIFO pointers will be out of reset after 2 cycles of system clocks in
 *  addition to synchronization delay (2 cycles of card clock), after the fifo_reset is
 *  cleared.
 */
#define SDHOST_FIFO_RESET    (BIT(1))
#define SDHOST_FIFO_RESET_M  (SDHOST_FIFO_RESET_V << SDHOST_FIFO_RESET_S)
#define SDHOST_FIFO_RESET_V  0x00000001U
#define SDHOST_FIFO_RESET_S  1
/** SDHOST_DMA_RESET : R/W; bitpos: [2]; default: 0;
 *  To reset DMA interface, firmware should set bit to 1. This bit is auto-cleared
 *  after two AHB clocks.
 */
#define SDHOST_DMA_RESET    (BIT(2))
#define SDHOST_DMA_RESET_M  (SDHOST_DMA_RESET_V << SDHOST_DMA_RESET_S)
#define SDHOST_DMA_RESET_V  0x00000001U
#define SDHOST_DMA_RESET_S  2
/** SDHOST_INT_ENABLE : R/W; bitpos: [4]; default: 0;
 *  Global interrupt enable/disable bit. 0: Disable; 1: Enable.
 */
#define SDHOST_INT_ENABLE    (BIT(4))
#define SDHOST_INT_ENABLE_M  (SDHOST_INT_ENABLE_V << SDHOST_INT_ENABLE_S)
#define SDHOST_INT_ENABLE_V  0x00000001U
#define SDHOST_INT_ENABLE_S  4
/** SDHOST_READ_WAIT : R/W; bitpos: [6]; default: 0;
 *  For sending read-wait to SDIO cards.
 */
#define SDHOST_READ_WAIT    (BIT(6))
#define SDHOST_READ_WAIT_M  (SDHOST_READ_WAIT_V << SDHOST_READ_WAIT_S)
#define SDHOST_READ_WAIT_V  0x00000001U
#define SDHOST_READ_WAIT_S  6
/** SDHOST_SEND_IRQ_RESPONSE : R/W; bitpos: [7]; default: 0;
 *  Bit automatically clears once response is sent. To wait for MMC card interrupts,
 *  host issues CMD40 and waits for interrupt response from MMC card(s). In the
 *  meantime, if host wants SD/MMC to exit waiting for interrupt state, it can set this
 *  bit, at which time SD/MMC command state-machine sends CMD40 response on bus and
 *  returns to idle state.
 */
#define SDHOST_SEND_IRQ_RESPONSE    (BIT(7))
#define SDHOST_SEND_IRQ_RESPONSE_M  (SDHOST_SEND_IRQ_RESPONSE_V << SDHOST_SEND_IRQ_RESPONSE_S)
#define SDHOST_SEND_IRQ_RESPONSE_V  0x00000001U
#define SDHOST_SEND_IRQ_RESPONSE_S  7
/** SDHOST_ABORT_READ_DATA : R/W; bitpos: [8]; default: 0;
 *  After a suspend-command is issued during a read-operation, software polls the card
 *  to find when the suspend-event occurred. Once the suspend-event has occurred,
 *  software sets the bit which will reset the data state machine that is waiting for
 *  the next block of data. This bit is automatically cleared once the data state
 *  machine is reset to idle.
 */
#define SDHOST_ABORT_READ_DATA    (BIT(8))
#define SDHOST_ABORT_READ_DATA_M  (SDHOST_ABORT_READ_DATA_V << SDHOST_ABORT_READ_DATA_S)
#define SDHOST_ABORT_READ_DATA_V  0x00000001U
#define SDHOST_ABORT_READ_DATA_S  8
/** SDHOST_SEND_CCSD : R/W; bitpos: [9]; default: 0;
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
#define SDHOST_SEND_CCSD    (BIT(9))
#define SDHOST_SEND_CCSD_M  (SDHOST_SEND_CCSD_V << SDHOST_SEND_CCSD_S)
#define SDHOST_SEND_CCSD_V  0x00000001U
#define SDHOST_SEND_CCSD_S  9
/** SDHOST_SEND_AUTO_STOP_CCSD : R/W; bitpos: [10]; default: 0;
 *  Always Set SDHOST_SEND_AUTO_STOP_CCSD and SDHOST_SEND_CCSD bits together;
 *  SDHOST_SEND_AUTO_STOP_CCSD should not be set independently of send_ccsd. When set,
 *  SD/MMC automatically sends an internally-generated STOP command (CMD12) to the
 *  CE-ATA device. After sending this internally-generated STOP command, the Auto
 *  Command Done (ACD) bit in SDHOST_RINTSTS_REG is set and an interrupt is generated
 *  for the host, in case the ACD interrupt is not masked. After sending the Command
 *  Completion Signal Disable (CCSD), SD/MMC automatically clears the
 *  SDHOST_SEND_AUTO_STOP_CCSD bit.
 */
#define SDHOST_SEND_AUTO_STOP_CCSD    (BIT(10))
#define SDHOST_SEND_AUTO_STOP_CCSD_M  (SDHOST_SEND_AUTO_STOP_CCSD_V << SDHOST_SEND_AUTO_STOP_CCSD_S)
#define SDHOST_SEND_AUTO_STOP_CCSD_V  0x00000001U
#define SDHOST_SEND_AUTO_STOP_CCSD_S  10
/** SDHOST_CEATA_DEVICE_INTERRUPT_STATUS : R/W; bitpos: [11]; default: 0;
 *  Software should appropriately write to this bit after the power-on reset or any
 *  other reset to the CE-ATA device. After reset, the CE-ATA device's interrupt is
 *  usually disabled (nIEN = 1). If the host enables the CE-ATA device's interrupt,
 *  then software should set this bit.
 */
#define SDHOST_CEATA_DEVICE_INTERRUPT_STATUS    (BIT(11))
#define SDHOST_CEATA_DEVICE_INTERRUPT_STATUS_M  (SDHOST_CEATA_DEVICE_INTERRUPT_STATUS_V << SDHOST_CEATA_DEVICE_INTERRUPT_STATUS_S)
#define SDHOST_CEATA_DEVICE_INTERRUPT_STATUS_V  0x00000001U
#define SDHOST_CEATA_DEVICE_INTERRUPT_STATUS_S  11

/** SDHOST_CLKDIV_REG register
 *  Clock divider configuration register
 */
#define SDHOST_CLKDIV_REG (DR_REG_SDHOST_BASE + 0x8)
/** SDHOST_CLK_DIVIDER0 : R/W; bitpos: [7:0]; default: 0;
 *  Clock divider0 value. Clock divisor is 2*n, where n = 0 bypasses the divider
 *  (divisor of 1). For example, a value of 1 means divided by 2*1 = 2, a value of 0xFF
 *  means divided by 2*255 = 510, and so on.
 */
#define SDHOST_CLK_DIVIDER0    0x000000FFU
#define SDHOST_CLK_DIVIDER0_M  (SDHOST_CLK_DIVIDER0_V << SDHOST_CLK_DIVIDER0_S)
#define SDHOST_CLK_DIVIDER0_V  0x000000FFU
#define SDHOST_CLK_DIVIDER0_S  0
/** SDHOST_CLK_DIVIDER1 : R/W; bitpos: [15:8]; default: 0;
 *  Clock divider1 value. Clock divisor is 2*n, where n = 0 bypasses the divider
 *  (divisor of 1). For example, a value of 1 means divided by 2*1 = 2, a value of 0xFF
 *  means divided by 2*255 = 510, and so on.
 */
#define SDHOST_CLK_DIVIDER1    0x000000FFU
#define SDHOST_CLK_DIVIDER1_M  (SDHOST_CLK_DIVIDER1_V << SDHOST_CLK_DIVIDER1_S)
#define SDHOST_CLK_DIVIDER1_V  0x000000FFU
#define SDHOST_CLK_DIVIDER1_S  8
/** SDHOST_CLK_DIVIDER2 : R/W; bitpos: [23:16]; default: 0;
 *  Clock divider2 value. Clock divisor is 2*n, where n = 0 bypasses the divider
 *  (divisor of 1). For example, a value of 1 means divided by 2*1 = 2, a value of 0xFF
 *  means divided by 2*255 = 510, and so on.
 */
#define SDHOST_CLK_DIVIDER2    0x000000FFU
#define SDHOST_CLK_DIVIDER2_M  (SDHOST_CLK_DIVIDER2_V << SDHOST_CLK_DIVIDER2_S)
#define SDHOST_CLK_DIVIDER2_V  0x000000FFU
#define SDHOST_CLK_DIVIDER2_S  16
/** SDHOST_CLK_DIVIDER3 : R/W; bitpos: [31:24]; default: 0;
 *  Clock divider3 value. Clock divisor is 2*n, where n = 0 bypasses the divider
 *  (divisor of 1). For example, a value of 1 means divided by 2*1 = 2, a value of 0xFF
 *  means divided by 2*255 = 510, and so on.
 */
#define SDHOST_CLK_DIVIDER3    0x000000FFU
#define SDHOST_CLK_DIVIDER3_M  (SDHOST_CLK_DIVIDER3_V << SDHOST_CLK_DIVIDER3_S)
#define SDHOST_CLK_DIVIDER3_V  0x000000FFU
#define SDHOST_CLK_DIVIDER3_S  24

/** SDHOST_CLKSRC_REG register
 *  Clock source selection register
 */
#define SDHOST_CLKSRC_REG (DR_REG_SDHOST_BASE + 0xc)
/** SDHOST_CLKSRC : R/W; bitpos: [3:0]; default: 0;
 *  Clock divider source for two SD cards is supported. Each card has two bits assigned
 *  to it. For example, bit[1:0] are assigned for card 0, bit[3:2] are assigned for
 *  card 1. Card 0 maps and internally routes clock divider[0:3] outputs to
 *  cclk_out[1:0] pins, depending on bit value.
 *  00 : Clock divider 0;
 *  01 : Clock divider 1;
 *  10 : Clock divider 2;
 *  11 : Clock divider 3.
 */
#define SDHOST_CLKSRC    0x0000000FU
#define SDHOST_CLKSRC_M  (SDHOST_CLKSRC_V << SDHOST_CLKSRC_S)
#define SDHOST_CLKSRC_V  0x0000000FU
#define SDHOST_CLKSRC_S  0

/** SDHOST_CLKENA_REG register
 *  Clock enable register
 */
#define SDHOST_CLKENA_REG (DR_REG_SDHOST_BASE + 0x10)
/** SDHOST_CCLK_ENABLE : R/W; bitpos: [1:0]; default: 0;
 *  Clock-enable control for two SD card clocks and one MMC card clock is supported.
 *  One bit per card.
 *  0: Clock disabled;
 *  1: Clock enabled.
 */
#define SDHOST_CCLK_ENABLE    0x00000003U
#define SDHOST_CCLK_ENABLE_M  (SDHOST_CCLK_ENABLE_V << SDHOST_CCLK_ENABLE_S)
#define SDHOST_CCLK_ENABLE_V  0x00000003U
#define SDHOST_CCLK_ENABLE_S  0
/** SDHOST_LP_ENABLE : R/W; bitpos: [17:16]; default: 0;
 *  Disable clock when the card is in IDLE state. One bit per card.
 *  0: clock disabled;
 *  1: clock enabled.
 */
#define SDHOST_LP_ENABLE    0x00000003U
#define SDHOST_LP_ENABLE_M  (SDHOST_LP_ENABLE_V << SDHOST_LP_ENABLE_S)
#define SDHOST_LP_ENABLE_V  0x00000003U
#define SDHOST_LP_ENABLE_S  16

/** SDHOST_TMOUT_REG register
 *  Data and response timeout configuration register
 */
#define SDHOST_TMOUT_REG (DR_REG_SDHOST_BASE + 0x14)
/** SDHOST_RESPONSE_TIMEOUT : R/W; bitpos: [7:0]; default: 64;
 *  Response timeout value. Value is specified in terms of number of card output
 *  clocks, i.e., sdhost_cclk_out.
 */
#define SDHOST_RESPONSE_TIMEOUT    0x000000FFU
#define SDHOST_RESPONSE_TIMEOUT_M  (SDHOST_RESPONSE_TIMEOUT_V << SDHOST_RESPONSE_TIMEOUT_S)
#define SDHOST_RESPONSE_TIMEOUT_V  0x000000FFU
#define SDHOST_RESPONSE_TIMEOUT_S  0
/** SDHOST_DATA_TIMEOUT : R/W; bitpos: [31:8]; default: 16777215;
 *  Value for card data read timeout. This value is also used for data starvation by
 *  host timeout. The timeout counter is started only after the card clock is stopped.
 *  This value is specified in number of card output clocks, i.e. sdhost_cclk_out of
 *  the selected card.
 *  NOTE: The software timer should be used if the timeout value is in the order of 100
 *  ms. In this case, read data timeout interrupt needs to be disabled.
 */
#define SDHOST_DATA_TIMEOUT    0x00FFFFFFU
#define SDHOST_DATA_TIMEOUT_M  (SDHOST_DATA_TIMEOUT_V << SDHOST_DATA_TIMEOUT_S)
#define SDHOST_DATA_TIMEOUT_V  0x00FFFFFFU
#define SDHOST_DATA_TIMEOUT_S  8

/** SDHOST_CTYPE_REG register
 *  Card bus width configuration register
 */
#define SDHOST_CTYPE_REG (DR_REG_SDHOST_BASE + 0x18)
/** SDHOST_CARD_WIDTH4 : R/W; bitpos: [1:0]; default: 0;
 *  One bit per card indicates if card is 1-bit or 4-bit mode.
 *  0: 1-bit mode;
 *  1: 4-bit mode.
 *  Bit[1:0] correspond to card[1:0] respectively.
 */
#define SDHOST_CARD_WIDTH4    0x00000003U
#define SDHOST_CARD_WIDTH4_M  (SDHOST_CARD_WIDTH4_V << SDHOST_CARD_WIDTH4_S)
#define SDHOST_CARD_WIDTH4_V  0x00000003U
#define SDHOST_CARD_WIDTH4_S  0
/** SDHOST_CARD_WIDTH8 : R/W; bitpos: [17:16]; default: 0;
 *  One bit per card indicates if card is in 8-bit mode.
 *  0: Non 8-bit mode;
 *  1: 8-bit mode.
 *  Bit[17:16] correspond to card[1:0] respectively.
 */
#define SDHOST_CARD_WIDTH8    0x00000003U
#define SDHOST_CARD_WIDTH8_M  (SDHOST_CARD_WIDTH8_V << SDHOST_CARD_WIDTH8_S)
#define SDHOST_CARD_WIDTH8_V  0x00000003U
#define SDHOST_CARD_WIDTH8_S  16

/** SDHOST_BLKSIZ_REG register
 *  Card data block size configuration register
 */
#define SDHOST_BLKSIZ_REG (DR_REG_SDHOST_BASE + 0x1c)
/** SDHOST_BLOCK_SIZE : R/W; bitpos: [15:0]; default: 512;
 *  Block size.
 */
#define SDHOST_BLOCK_SIZE    0x0000FFFFU
#define SDHOST_BLOCK_SIZE_M  (SDHOST_BLOCK_SIZE_V << SDHOST_BLOCK_SIZE_S)
#define SDHOST_BLOCK_SIZE_V  0x0000FFFFU
#define SDHOST_BLOCK_SIZE_S  0

/** SDHOST_BYTCNT_REG register
 *  Data transfer length configuration register
 */
#define SDHOST_BYTCNT_REG (DR_REG_SDHOST_BASE + 0x20)
/** SDHOST_BYTE_COUNT : R/W; bitpos: [31:0]; default: 512;
 *  Number of bytes to be transferred, should be an integral multiple of Block Size for
 *  block transfers. For data transfers of undefined byte lengths, byte count should be
 *  set to 0. When byte count is set to 0, it is the responsibility of host to
 *  explicitly send stop/abort command to terminate data transfer.
 */
#define SDHOST_BYTE_COUNT    0xFFFFFFFFU
#define SDHOST_BYTE_COUNT_M  (SDHOST_BYTE_COUNT_V << SDHOST_BYTE_COUNT_S)
#define SDHOST_BYTE_COUNT_V  0xFFFFFFFFU
#define SDHOST_BYTE_COUNT_S  0

/** SDHOST_INTMASK_REG register
 *  SDIO interrupt mask register
 */
#define SDHOST_INTMASK_REG (DR_REG_SDHOST_BASE + 0x24)
/** SDHOST_INT_MASK : R/W; bitpos: [15:0]; default: 0;
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
#define SDHOST_INT_MASK    0x0000FFFFU
#define SDHOST_INT_MASK_M  (SDHOST_INT_MASK_V << SDHOST_INT_MASK_S)
#define SDHOST_INT_MASK_V  0x0000FFFFU
#define SDHOST_INT_MASK_S  0
/** SDHOST_SDIO_INT_MASK : R/W; bitpos: [17:16]; default: 0;
 *  SDIO interrupt mask, one bit for each card. Bit[17:16] correspond to card[15:0]
 *  respectively. When masked, SDIO interrupt detection for that card is disabled. 0
 *  masks an interrupt, and 1 enables an interrupt.
 */
#define SDHOST_SDIO_INT_MASK    0x00000003U
#define SDHOST_SDIO_INT_MASK_M  (SDHOST_SDIO_INT_MASK_V << SDHOST_SDIO_INT_MASK_S)
#define SDHOST_SDIO_INT_MASK_V  0x00000003U
#define SDHOST_SDIO_INT_MASK_S  16

/** SDHOST_CMDARG_REG register
 *  Command argument data register
 */
#define SDHOST_CMDARG_REG (DR_REG_SDHOST_BASE + 0x28)
/** SDHOST_CMDARG : R/W; bitpos: [31:0]; default: 0;
 *  Value indicates command argument to be passed to the card.
 */
#define SDHOST_CMDARG    0xFFFFFFFFU
#define SDHOST_CMDARG_M  (SDHOST_CMDARG_V << SDHOST_CMDARG_S)
#define SDHOST_CMDARG_V  0xFFFFFFFFU
#define SDHOST_CMDARG_S  0

/** SDHOST_CMD_REG register
 *  Command and boot configuration register
 */
#define SDHOST_CMD_REG (DR_REG_SDHOST_BASE + 0x2c)
/** SDHOST_CMD_INDEX : R/W; bitpos: [5:0]; default: 0;
 *  Command index.
 */
#define SDHOST_CMD_INDEX    0x0000003FU
#define SDHOST_CMD_INDEX_M  (SDHOST_CMD_INDEX_V << SDHOST_CMD_INDEX_S)
#define SDHOST_CMD_INDEX_V  0x0000003FU
#define SDHOST_CMD_INDEX_S  0
/** SDHOST_RESPONSE_EXPECT : R/W; bitpos: [6]; default: 0;
 *  0: No response expected from card; 1: Response expected from card.
 */
#define SDHOST_RESPONSE_EXPECT    (BIT(6))
#define SDHOST_RESPONSE_EXPECT_M  (SDHOST_RESPONSE_EXPECT_V << SDHOST_RESPONSE_EXPECT_S)
#define SDHOST_RESPONSE_EXPECT_V  0x00000001U
#define SDHOST_RESPONSE_EXPECT_S  6
/** SDHOST_RESPONSE_LENGTH : R/W; bitpos: [7]; default: 0;
 *  0: Short response expected from card; 1: Long response expected from card.
 */
#define SDHOST_RESPONSE_LENGTH    (BIT(7))
#define SDHOST_RESPONSE_LENGTH_M  (SDHOST_RESPONSE_LENGTH_V << SDHOST_RESPONSE_LENGTH_S)
#define SDHOST_RESPONSE_LENGTH_V  0x00000001U
#define SDHOST_RESPONSE_LENGTH_S  7
/** SDHOST_CHECK_RESPONSE_CRC : R/W; bitpos: [8]; default: 0;
 *  0: Do not check; 1: Check response CRC.
 *  Some of command responses do not return valid CRC bits. Software should disable CRC
 *  checks for those commands in order to disable CRC checking by controller.
 */
#define SDHOST_CHECK_RESPONSE_CRC    (BIT(8))
#define SDHOST_CHECK_RESPONSE_CRC_M  (SDHOST_CHECK_RESPONSE_CRC_V << SDHOST_CHECK_RESPONSE_CRC_S)
#define SDHOST_CHECK_RESPONSE_CRC_V  0x00000001U
#define SDHOST_CHECK_RESPONSE_CRC_S  8
/** SDHOST_DATA_EXPECTED : R/W; bitpos: [9]; default: 0;
 *  0: No data transfer expected; 1: Data transfer expected.
 */
#define SDHOST_DATA_EXPECTED    (BIT(9))
#define SDHOST_DATA_EXPECTED_M  (SDHOST_DATA_EXPECTED_V << SDHOST_DATA_EXPECTED_S)
#define SDHOST_DATA_EXPECTED_V  0x00000001U
#define SDHOST_DATA_EXPECTED_S  9
/** SDHOST_READ_WRITE : R/W; bitpos: [10]; default: 0;
 *  0: Read from card; 1: Write to card.
 *  Don't care if no data is expected from card.
 */
#define SDHOST_READ_WRITE    (BIT(10))
#define SDHOST_READ_WRITE_M  (SDHOST_READ_WRITE_V << SDHOST_READ_WRITE_S)
#define SDHOST_READ_WRITE_V  0x00000001U
#define SDHOST_READ_WRITE_S  10
/** SDHOST_TRANSFER_MODE : R/W; bitpos: [11]; default: 0;
 *  0: Block data transfer command; 1: Stream data transfer command.
 *  Don't care if no data expected.
 */
#define SDHOST_TRANSFER_MODE    (BIT(11))
#define SDHOST_TRANSFER_MODE_M  (SDHOST_TRANSFER_MODE_V << SDHOST_TRANSFER_MODE_S)
#define SDHOST_TRANSFER_MODE_V  0x00000001U
#define SDHOST_TRANSFER_MODE_S  11
/** SDHOST_SEND_AUTO_STOP : R/W; bitpos: [12]; default: 0;
 *  0: No stop command is sent at the end of data transfer; 1: Send stop command at the
 *  end of data transfer.
 */
#define SDHOST_SEND_AUTO_STOP    (BIT(12))
#define SDHOST_SEND_AUTO_STOP_M  (SDHOST_SEND_AUTO_STOP_V << SDHOST_SEND_AUTO_STOP_S)
#define SDHOST_SEND_AUTO_STOP_V  0x00000001U
#define SDHOST_SEND_AUTO_STOP_S  12
/** SDHOST_WAIT_PRVDATA_COMPLETE : R/W; bitpos: [13]; default: 0;
 *  0: Send command at once, even if previous data transfer has not completed; 1: Wait
 *  for previous data transfer to complete before sending Command.
 *  The SDHOST_WAIT_PRVDATA_COMPLETE] = 0 option is typically used to query status of
 *  card during data transfer or to stop current data transfer. SDHOST_CARD_NUMBERr
 *  should be same as in previous command.
 */
#define SDHOST_WAIT_PRVDATA_COMPLETE    (BIT(13))
#define SDHOST_WAIT_PRVDATA_COMPLETE_M  (SDHOST_WAIT_PRVDATA_COMPLETE_V << SDHOST_WAIT_PRVDATA_COMPLETE_S)
#define SDHOST_WAIT_PRVDATA_COMPLETE_V  0x00000001U
#define SDHOST_WAIT_PRVDATA_COMPLETE_S  13
/** SDHOST_STOP_ABORT_CMD : R/W; bitpos: [14]; default: 0;
 *  0: Neither stop nor abort command can stop current data transfer. If abort is sent
 *  to function-number currently selected or not in data-transfer mode, then bit should
 *  be set to 0; 1: Stop or abort command intended to stop current data transfer in
 *  progress.
 *  When open-ended or predefined data transfer is in progress, and host issues stop or
 *  abort command to stop data transfer, bit should be set so that command/data
 *  state-machines of CIU can return correctly to idle state.
 */
#define SDHOST_STOP_ABORT_CMD    (BIT(14))
#define SDHOST_STOP_ABORT_CMD_M  (SDHOST_STOP_ABORT_CMD_V << SDHOST_STOP_ABORT_CMD_S)
#define SDHOST_STOP_ABORT_CMD_V  0x00000001U
#define SDHOST_STOP_ABORT_CMD_S  14
/** SDHOST_SEND_INITIALIZATION : R/W; bitpos: [15]; default: 0;
 *  0: Do not send initialization sequence (80 clocks of 1) before sending this
 *  command; 1: Send initialization sequence before sending this command.
 *  After powered on, 80 clocks must be sent to card for initialization before sending
 *  any commands to card. Bit should be set while sending first command to card so that
 *  controller will initialize clocks before sending command to card.
 */
#define SDHOST_SEND_INITIALIZATION    (BIT(15))
#define SDHOST_SEND_INITIALIZATION_M  (SDHOST_SEND_INITIALIZATION_V << SDHOST_SEND_INITIALIZATION_S)
#define SDHOST_SEND_INITIALIZATION_V  0x00000001U
#define SDHOST_SEND_INITIALIZATION_S  15
/** SDHOST_CARD_NUMBER : R/W; bitpos: [20:16]; default: 0;
 *  Card number in use. Represents physical slot number of card being accessed. In
 *  SD-only mode, up to two cards are supported.
 */
#define SDHOST_CARD_NUMBER    0x0000001FU
#define SDHOST_CARD_NUMBER_M  (SDHOST_CARD_NUMBER_V << SDHOST_CARD_NUMBER_S)
#define SDHOST_CARD_NUMBER_V  0x0000001FU
#define SDHOST_CARD_NUMBER_S  16
/** SDHOST_UPDATE_CLOCK_REGISTERS_ONLY : R/W; bitpos: [21]; default: 0;
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
#define SDHOST_UPDATE_CLOCK_REGISTERS_ONLY    (BIT(21))
#define SDHOST_UPDATE_CLOCK_REGISTERS_ONLY_M  (SDHOST_UPDATE_CLOCK_REGISTERS_ONLY_V << SDHOST_UPDATE_CLOCK_REGISTERS_ONLY_S)
#define SDHOST_UPDATE_CLOCK_REGISTERS_ONLY_V  0x00000001U
#define SDHOST_UPDATE_CLOCK_REGISTERS_ONLY_S  21
/** SDHOST_READ_CEATA_DEVICE : R/W; bitpos: [22]; default: 0;
 *  Read access flag.
 *  0: Host is not performing read access (RW_REG or RW_BLK)towards CE-ATA device;
 *  1: Host is performing read access (RW_REG or RW_BLK) towards CE-ATA device.
 *  Software should set this bit to indicate that CE-ATA device is being accessed for
 *  read transfer. This bit is used to disable read data timeout indication while
 *  performing CE-ATA read transfers. Maximum value of I/O transmission delay can be no
 *  less than 10 seconds. SD/MMC should not indicate read data timeout while waiting
 *  for data from CE-ATA device.
 */
#define SDHOST_READ_CEATA_DEVICE    (BIT(22))
#define SDHOST_READ_CEATA_DEVICE_M  (SDHOST_READ_CEATA_DEVICE_V << SDHOST_READ_CEATA_DEVICE_S)
#define SDHOST_READ_CEATA_DEVICE_V  0x00000001U
#define SDHOST_READ_CEATA_DEVICE_S  22
/** SDHOST_CCS_EXPECTED : R/W; bitpos: [23]; default: 0;
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
#define SDHOST_CCS_EXPECTED    (BIT(23))
#define SDHOST_CCS_EXPECTED_M  (SDHOST_CCS_EXPECTED_V << SDHOST_CCS_EXPECTED_S)
#define SDHOST_CCS_EXPECTED_V  0x00000001U
#define SDHOST_CCS_EXPECTED_S  23
/** SDHOST_USE_HOLE_REG : R/W; bitpos: [29]; default: 1;
 *  Use Hold Register.
 *  0: CMD and DATA sent to card bypassing HOLD Register;
 *  1: CMD and DATA sent to card through the HOLD Register.
 */
#define SDHOST_USE_HOLE_REG    (BIT(29))
#define SDHOST_USE_HOLE_REG_M  (SDHOST_USE_HOLE_REG_V << SDHOST_USE_HOLE_REG_S)
#define SDHOST_USE_HOLE_REG_V  0x00000001U
#define SDHOST_USE_HOLE_REG_S  29
/** SDHOST_START_CMD : R/W; bitpos: [31]; default: 0;
 *  Start command. Once command is served by the CIU, this bit is automatically
 *  cleared. When this bit is set, host should not attempt to write to any command
 *  registers. If a write is attempted, hardware lock error is set in raw interrupt
 *  register. Once command is sent and a response is received from SD_MMC_CEATA cards,
 *  Command Done bit is set in the raw interrupt Register.
 */
#define SDHOST_START_CMD    (BIT(31))
#define SDHOST_START_CMD_M  (SDHOST_START_CMD_V << SDHOST_START_CMD_S)
#define SDHOST_START_CMD_V  0x00000001U
#define SDHOST_START_CMD_S  31

/** SDHOST_RESP0_REG register
 *  Response data register
 */
#define SDHOST_RESP0_REG (DR_REG_SDHOST_BASE + 0x30)
/** SDHOST_RESPONSE0_REG : RO; bitpos: [31:0]; default: 0;
 *  Bit[31:0] of response.
 */
#define SDHOST_RESPONSE0_REG    0xFFFFFFFFU
#define SDHOST_RESPONSE0_REG_M  (SDHOST_RESPONSE0_REG_V << SDHOST_RESPONSE0_REG_S)
#define SDHOST_RESPONSE0_REG_V  0xFFFFFFFFU
#define SDHOST_RESPONSE0_REG_S  0

/** SDHOST_RESP1_REG register
 *  Long response data register
 */
#define SDHOST_RESP1_REG (DR_REG_SDHOST_BASE + 0x34)
/** SDHOST_RESPONSE1_REG : RO; bitpos: [31:0]; default: 0;
 *  Bit[63:32] of long response.
 */
#define SDHOST_RESPONSE1_REG    0xFFFFFFFFU
#define SDHOST_RESPONSE1_REG_M  (SDHOST_RESPONSE1_REG_V << SDHOST_RESPONSE1_REG_S)
#define SDHOST_RESPONSE1_REG_V  0xFFFFFFFFU
#define SDHOST_RESPONSE1_REG_S  0

/** SDHOST_RESP2_REG register
 *  Long response data register
 */
#define SDHOST_RESP2_REG (DR_REG_SDHOST_BASE + 0x38)
/** SDHOST_RESPONSE2_REG : RO; bitpos: [31:0]; default: 0;
 *  Bit[95:64] of long response.
 */
#define SDHOST_RESPONSE2_REG    0xFFFFFFFFU
#define SDHOST_RESPONSE2_REG_M  (SDHOST_RESPONSE2_REG_V << SDHOST_RESPONSE2_REG_S)
#define SDHOST_RESPONSE2_REG_V  0xFFFFFFFFU
#define SDHOST_RESPONSE2_REG_S  0

/** SDHOST_RESP3_REG register
 *  Long response data register
 */
#define SDHOST_RESP3_REG (DR_REG_SDHOST_BASE + 0x3c)
/** SDHOST_RESPONSE3_REG : RO; bitpos: [31:0]; default: 0;
 *  Bit[127:96] of long response.
 */
#define SDHOST_RESPONSE3_REG    0xFFFFFFFFU
#define SDHOST_RESPONSE3_REG_M  (SDHOST_RESPONSE3_REG_V << SDHOST_RESPONSE3_REG_S)
#define SDHOST_RESPONSE3_REG_V  0xFFFFFFFFU
#define SDHOST_RESPONSE3_REG_S  0

/** SDHOST_MINTSTS_REG register
 *  Masked interrupt status register
 */
#define SDHOST_MINTSTS_REG (DR_REG_SDHOST_BASE + 0x40)
/** SDHOST_INT_STATUS_MSK : RO; bitpos: [15:0]; default: 0;
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
#define SDHOST_INT_STATUS_MSK    0x0000FFFFU
#define SDHOST_INT_STATUS_MSK_M  (SDHOST_INT_STATUS_MSK_V << SDHOST_INT_STATUS_MSK_S)
#define SDHOST_INT_STATUS_MSK_V  0x0000FFFFU
#define SDHOST_INT_STATUS_MSK_S  0
/** SDHOST_SDIO_INTERRUPT_MSK : RO; bitpos: [17:16]; default: 0;
 *  Interrupt from SDIO card, one bit for each card. Bit[17:16] correspond to card1 and
 *  card0, respectively. SDIO interrupt for card is enabled only if corresponding
 *  sdhost_sdio_int_mask bit is set in Interrupt mask register (Setting mask bit
 *  enables interrupt).
 */
#define SDHOST_SDIO_INTERRUPT_MSK    0x00000003U
#define SDHOST_SDIO_INTERRUPT_MSK_M  (SDHOST_SDIO_INTERRUPT_MSK_V << SDHOST_SDIO_INTERRUPT_MSK_S)
#define SDHOST_SDIO_INTERRUPT_MSK_V  0x00000003U
#define SDHOST_SDIO_INTERRUPT_MSK_S  16

/** SDHOST_RINTSTS_REG register
 *  Raw interrupt status register
 */
#define SDHOST_RINTSTS_REG (DR_REG_SDHOST_BASE + 0x44)
/** SDHOST_INT_STATUS_RAW : R/W; bitpos: [15:0]; default: 0;
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
#define SDHOST_INT_STATUS_RAW    0x0000FFFFU
#define SDHOST_INT_STATUS_RAW_M  (SDHOST_INT_STATUS_RAW_V << SDHOST_INT_STATUS_RAW_S)
#define SDHOST_INT_STATUS_RAW_V  0x0000FFFFU
#define SDHOST_INT_STATUS_RAW_S  0
/** SDHOST_SDIO_INTERRUPT_RAW : R/W; bitpos: [17:16]; default: 0;
 *  Interrupt from SDIO card, one bit for each card. Bit[17:16] correspond to card1 and
 *  card0, respectively. Setting a bit clears the corresponding interrupt bit and
 *  writing 0 has no effect.
 *  0: No SDIO interrupt from card;
 *  1: SDIO interrupt from card.
 */
#define SDHOST_SDIO_INTERRUPT_RAW    0x00000003U
#define SDHOST_SDIO_INTERRUPT_RAW_M  (SDHOST_SDIO_INTERRUPT_RAW_V << SDHOST_SDIO_INTERRUPT_RAW_S)
#define SDHOST_SDIO_INTERRUPT_RAW_V  0x00000003U
#define SDHOST_SDIO_INTERRUPT_RAW_S  16

/** SDHOST_STATUS_REG register
 *  SD/MMC status register
 */
#define SDHOST_STATUS_REG (DR_REG_SDHOST_BASE + 0x48)
/** SDHOST_FIFO_RX_WATERMARK : RO; bitpos: [0]; default: 0;
 *  FIFO reached Receive watermark level, not qualified with data transfer.
 */
#define SDHOST_FIFO_RX_WATERMARK    (BIT(0))
#define SDHOST_FIFO_RX_WATERMARK_M  (SDHOST_FIFO_RX_WATERMARK_V << SDHOST_FIFO_RX_WATERMARK_S)
#define SDHOST_FIFO_RX_WATERMARK_V  0x00000001U
#define SDHOST_FIFO_RX_WATERMARK_S  0
/** SDHOST_FIFO_TX_WATERMARK : RO; bitpos: [1]; default: 1;
 *  FIFO reached Transmit watermark level, not qualified with data transfer.
 */
#define SDHOST_FIFO_TX_WATERMARK    (BIT(1))
#define SDHOST_FIFO_TX_WATERMARK_M  (SDHOST_FIFO_TX_WATERMARK_V << SDHOST_FIFO_TX_WATERMARK_S)
#define SDHOST_FIFO_TX_WATERMARK_V  0x00000001U
#define SDHOST_FIFO_TX_WATERMARK_S  1
/** SDHOST_FIFO_EMPTY : RO; bitpos: [2]; default: 1;
 *  FIFO is empty status.
 */
#define SDHOST_FIFO_EMPTY    (BIT(2))
#define SDHOST_FIFO_EMPTY_M  (SDHOST_FIFO_EMPTY_V << SDHOST_FIFO_EMPTY_S)
#define SDHOST_FIFO_EMPTY_V  0x00000001U
#define SDHOST_FIFO_EMPTY_S  2
/** SDHOST_FIFO_FULL : RO; bitpos: [3]; default: 0;
 *  FIFO is full status.
 */
#define SDHOST_FIFO_FULL    (BIT(3))
#define SDHOST_FIFO_FULL_M  (SDHOST_FIFO_FULL_V << SDHOST_FIFO_FULL_S)
#define SDHOST_FIFO_FULL_V  0x00000001U
#define SDHOST_FIFO_FULL_S  3
/** SDHOST_COMMAND_FSM_STATES : RO; bitpos: [7:4]; default: 1;
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
#define SDHOST_COMMAND_FSM_STATES    0x0000000FU
#define SDHOST_COMMAND_FSM_STATES_M  (SDHOST_COMMAND_FSM_STATES_V << SDHOST_COMMAND_FSM_STATES_S)
#define SDHOST_COMMAND_FSM_STATES_V  0x0000000FU
#define SDHOST_COMMAND_FSM_STATES_S  4
/** SDHOST_DATA_3_STATUS : RO; bitpos: [8]; default: 1;
 *  Raw selected sdhost_card_data[3], checks whether card is present.
 *  0: card not present;
 *  1: card present.
 */
#define SDHOST_DATA_3_STATUS    (BIT(8))
#define SDHOST_DATA_3_STATUS_M  (SDHOST_DATA_3_STATUS_V << SDHOST_DATA_3_STATUS_S)
#define SDHOST_DATA_3_STATUS_V  0x00000001U
#define SDHOST_DATA_3_STATUS_S  8
/** SDHOST_DATA_BUSY : RO; bitpos: [9]; default: 1;
 *  Inverted version of raw selected sdhost_card_data[0].
 *  0: Card data not busy;
 *  1: Card data busy.
 */
#define SDHOST_DATA_BUSY    (BIT(9))
#define SDHOST_DATA_BUSY_M  (SDHOST_DATA_BUSY_V << SDHOST_DATA_BUSY_S)
#define SDHOST_DATA_BUSY_V  0x00000001U
#define SDHOST_DATA_BUSY_S  9
/** SDHOST_DATA_STATE_MC_BUSY : RO; bitpos: [10]; default: 1;
 *  Data transmit or receive state-machine is busy.
 */
#define SDHOST_DATA_STATE_MC_BUSY    (BIT(10))
#define SDHOST_DATA_STATE_MC_BUSY_M  (SDHOST_DATA_STATE_MC_BUSY_V << SDHOST_DATA_STATE_MC_BUSY_S)
#define SDHOST_DATA_STATE_MC_BUSY_V  0x00000001U
#define SDHOST_DATA_STATE_MC_BUSY_S  10
/** SDHOST_RESPONSE_INDEX : RO; bitpos: [16:11]; default: 0;
 *  Index of previous response, including any auto-stop sent by core.
 */
#define SDHOST_RESPONSE_INDEX    0x0000003FU
#define SDHOST_RESPONSE_INDEX_M  (SDHOST_RESPONSE_INDEX_V << SDHOST_RESPONSE_INDEX_S)
#define SDHOST_RESPONSE_INDEX_V  0x0000003FU
#define SDHOST_RESPONSE_INDEX_S  11
/** SDHOST_FIFO_COUNT : RO; bitpos: [29:17]; default: 0;
 *  FIFO count, number of filled locations in FIFO.
 */
#define SDHOST_FIFO_COUNT    0x00001FFFU
#define SDHOST_FIFO_COUNT_M  (SDHOST_FIFO_COUNT_V << SDHOST_FIFO_COUNT_S)
#define SDHOST_FIFO_COUNT_V  0x00001FFFU
#define SDHOST_FIFO_COUNT_S  17

/** SDHOST_FIFOTH_REG register
 *  FIFO configuration register
 */
#define SDHOST_FIFOTH_REG (DR_REG_SDHOST_BASE + 0x4c)
/** SDHOST_TX_WMARK : R/W; bitpos: [11:0]; default: 0;
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
#define SDHOST_TX_WMARK    0x00000FFFU
#define SDHOST_TX_WMARK_M  (SDHOST_TX_WMARK_V << SDHOST_TX_WMARK_S)
#define SDHOST_TX_WMARK_V  0x00000FFFU
#define SDHOST_TX_WMARK_S  0
/** SDHOST_RX_WMARK : R/W; bitpos: [26:16]; default: 0;
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
#define SDHOST_RX_WMARK    0x000007FFU
#define SDHOST_RX_WMARK_M  (SDHOST_RX_WMARK_V << SDHOST_RX_WMARK_S)
#define SDHOST_RX_WMARK_V  0x000007FFU
#define SDHOST_RX_WMARK_S  16
/** SDHOST_DMA_MULTIPLE_TRANSACTION_SIZE : R/W; bitpos: [30:28]; default: 0;
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
#define SDHOST_DMA_MULTIPLE_TRANSACTION_SIZE    0x00000007U
#define SDHOST_DMA_MULTIPLE_TRANSACTION_SIZE_M  (SDHOST_DMA_MULTIPLE_TRANSACTION_SIZE_V << SDHOST_DMA_MULTIPLE_TRANSACTION_SIZE_S)
#define SDHOST_DMA_MULTIPLE_TRANSACTION_SIZE_V  0x00000007U
#define SDHOST_DMA_MULTIPLE_TRANSACTION_SIZE_S  28

/** SDHOST_CDETECT_REG register
 *  Card detect register
 */
#define SDHOST_CDETECT_REG (DR_REG_SDHOST_BASE + 0x50)
/** SDHOST_CARD_DETECT_N : RO; bitpos: [1:0]; default: 0;
 *  Value on sdhost_card_detect_n input ports (1 bit per card), read-only bits. 0
 *  represents presence of card. Only NUM_CARDS number of bits are implemented.
 */
#define SDHOST_CARD_DETECT_N    0x00000003U
#define SDHOST_CARD_DETECT_N_M  (SDHOST_CARD_DETECT_N_V << SDHOST_CARD_DETECT_N_S)
#define SDHOST_CARD_DETECT_N_V  0x00000003U
#define SDHOST_CARD_DETECT_N_S  0

/** SDHOST_WRTPRT_REG register
 *  Card write protection (WP) status register
 */
#define SDHOST_WRTPRT_REG (DR_REG_SDHOST_BASE + 0x54)
/** SDHOST_WRITE_PROTECT : RO; bitpos: [1:0]; default: 0;
 *  Value on sdhost_card_write_prt input ports (1 bit per card). 1 represents write
 *  protection. Only NUM_CARDS number of bits are implemented.
 */
#define SDHOST_WRITE_PROTECT    0x00000003U
#define SDHOST_WRITE_PROTECT_M  (SDHOST_WRITE_PROTECT_V << SDHOST_WRITE_PROTECT_S)
#define SDHOST_WRITE_PROTECT_V  0x00000003U
#define SDHOST_WRITE_PROTECT_S  0

/** SDHOST_TCBCNT_REG register
 *  Transferred byte count register
 */
#define SDHOST_TCBCNT_REG (DR_REG_SDHOST_BASE + 0x5c)
/** SDHOST_TCBCNT : RO; bitpos: [31:0]; default: 0;
 *  Number of bytes transferred by CIU unit to card.
 */
#define SDHOST_TCBCNT    0xFFFFFFFFU
#define SDHOST_TCBCNT_M  (SDHOST_TCBCNT_V << SDHOST_TCBCNT_S)
#define SDHOST_TCBCNT_V  0xFFFFFFFFU
#define SDHOST_TCBCNT_S  0

/** SDHOST_TBBCNT_REG register
 *  Transferred byte count register
 */
#define SDHOST_TBBCNT_REG (DR_REG_SDHOST_BASE + 0x60)
/** SDHOST_TBBCNT : RO; bitpos: [31:0]; default: 0;
 *  Number of bytes transferred between Host/DMA memory and BIU FIFO.
 */
#define SDHOST_TBBCNT    0xFFFFFFFFU
#define SDHOST_TBBCNT_M  (SDHOST_TBBCNT_V << SDHOST_TBBCNT_S)
#define SDHOST_TBBCNT_V  0xFFFFFFFFU
#define SDHOST_TBBCNT_S  0

/** SDHOST_DEBNCE_REG register
 *  Debounce filter time configuration register
 */
#define SDHOST_DEBNCE_REG (DR_REG_SDHOST_BASE + 0x64)
/** SDHOST_DEBOUNCE_COUNT : R/W; bitpos: [23:0]; default: 0;
 *  Number of host clocks (clk) used by debounce filter logic. The typical debounce
 *  time is 5 \verb+~+ 25 ms to prevent the card instability when the card is inserted
 *  or removed.
 */
#define SDHOST_DEBOUNCE_COUNT    0x00FFFFFFU
#define SDHOST_DEBOUNCE_COUNT_M  (SDHOST_DEBOUNCE_COUNT_V << SDHOST_DEBOUNCE_COUNT_S)
#define SDHOST_DEBOUNCE_COUNT_V  0x00FFFFFFU
#define SDHOST_DEBOUNCE_COUNT_S  0

/** SDHOST_USRID_REG register
 *  User ID (scratchpad) register
 */
#define SDHOST_USRID_REG (DR_REG_SDHOST_BASE + 0x68)
/** SDHOST_USRID : R/W; bitpos: [31:0]; default: 0;
 *  User identification register, value set by user. Can also be used as a scratchpad
 *  register by user.
 */
#define SDHOST_USRID    0xFFFFFFFFU
#define SDHOST_USRID_M  (SDHOST_USRID_V << SDHOST_USRID_S)
#define SDHOST_USRID_V  0xFFFFFFFFU
#define SDHOST_USRID_S  0

/** SDHOST_VERID_REG register
 *  Version ID (scratchpad) register
 */
#define SDHOST_VERID_REG (DR_REG_SDHOST_BASE + 0x6c)
/** SDHOST_VERSIONID : RO; bitpos: [31:0]; default: 1412572938;
 *  Hardware version register. Can also be read by fireware.
 */
#define SDHOST_VERSIONID    0xFFFFFFFFU
#define SDHOST_VERSIONID_M  (SDHOST_VERSIONID_V << SDHOST_VERSIONID_S)
#define SDHOST_VERSIONID_V  0xFFFFFFFFU
#define SDHOST_VERSIONID_S  0

/** SDHOST_HCON_REG register
 *  Hardware feature register
 */
#define SDHOST_HCON_REG (DR_REG_SDHOST_BASE + 0x70)
/** SDHOST_CARD_TYPE_REG : RO; bitpos: [0]; default: 1;
 *  Hardware support SDIO and MMC.
 */
#define SDHOST_CARD_TYPE_REG    (BIT(0))
#define SDHOST_CARD_TYPE_REG_M  (SDHOST_CARD_TYPE_REG_V << SDHOST_CARD_TYPE_REG_S)
#define SDHOST_CARD_TYPE_REG_V  0x00000001U
#define SDHOST_CARD_TYPE_REG_S  0
/** SDHOST_CARD_NUM_REG : RO; bitpos: [5:1]; default: 1;
 *  Support card number is 2.
 */
#define SDHOST_CARD_NUM_REG    0x0000001FU
#define SDHOST_CARD_NUM_REG_M  (SDHOST_CARD_NUM_REG_V << SDHOST_CARD_NUM_REG_S)
#define SDHOST_CARD_NUM_REG_V  0x0000001FU
#define SDHOST_CARD_NUM_REG_S  1
/** SDHOST_BUS_TYPE_REG : RO; bitpos: [6]; default: 1;
 *  Register config is APB bus.
 */
#define SDHOST_BUS_TYPE_REG    (BIT(6))
#define SDHOST_BUS_TYPE_REG_M  (SDHOST_BUS_TYPE_REG_V << SDHOST_BUS_TYPE_REG_S)
#define SDHOST_BUS_TYPE_REG_V  0x00000001U
#define SDHOST_BUS_TYPE_REG_S  6
/** SDHOST_DATA_WIDTH_REG : RO; bitpos: [9:7]; default: 1;
 *  Regisger data widht is 32.
 */
#define SDHOST_DATA_WIDTH_REG    0x00000007U
#define SDHOST_DATA_WIDTH_REG_M  (SDHOST_DATA_WIDTH_REG_V << SDHOST_DATA_WIDTH_REG_S)
#define SDHOST_DATA_WIDTH_REG_V  0x00000007U
#define SDHOST_DATA_WIDTH_REG_S  7
/** SDHOST_ADDR_WIDTH_REG : RO; bitpos: [15:10]; default: 19;
 *  Register address width is 32.
 */
#define SDHOST_ADDR_WIDTH_REG    0x0000003FU
#define SDHOST_ADDR_WIDTH_REG_M  (SDHOST_ADDR_WIDTH_REG_V << SDHOST_ADDR_WIDTH_REG_S)
#define SDHOST_ADDR_WIDTH_REG_V  0x0000003FU
#define SDHOST_ADDR_WIDTH_REG_S  10
/** SDHOST_DMA_WIDTH_REG : RO; bitpos: [20:18]; default: 1;
 *  DMA data witdth is 32.
 */
#define SDHOST_DMA_WIDTH_REG    0x00000007U
#define SDHOST_DMA_WIDTH_REG_M  (SDHOST_DMA_WIDTH_REG_V << SDHOST_DMA_WIDTH_REG_S)
#define SDHOST_DMA_WIDTH_REG_V  0x00000007U
#define SDHOST_DMA_WIDTH_REG_S  18
/** SDHOST_RAM_INDISE_REG : RO; bitpos: [21]; default: 0;
 *  Inside RAM in SDMMC module.
 */
#define SDHOST_RAM_INDISE_REG    (BIT(21))
#define SDHOST_RAM_INDISE_REG_M  (SDHOST_RAM_INDISE_REG_V << SDHOST_RAM_INDISE_REG_S)
#define SDHOST_RAM_INDISE_REG_V  0x00000001U
#define SDHOST_RAM_INDISE_REG_S  21
/** SDHOST_HOLD_REG : RO; bitpos: [22]; default: 1;
 *  Have a hold regiser in data path .
 */
#define SDHOST_HOLD_REG    (BIT(22))
#define SDHOST_HOLD_REG_M  (SDHOST_HOLD_REG_V << SDHOST_HOLD_REG_S)
#define SDHOST_HOLD_REG_V  0x00000001U
#define SDHOST_HOLD_REG_S  22
/** SDHOST_NUM_CLK_DIV_REG : RO; bitpos: [25:24]; default: 3;
 *  Have 4 clk divider in design .
 */
#define SDHOST_NUM_CLK_DIV_REG    0x00000003U
#define SDHOST_NUM_CLK_DIV_REG_M  (SDHOST_NUM_CLK_DIV_REG_V << SDHOST_NUM_CLK_DIV_REG_S)
#define SDHOST_NUM_CLK_DIV_REG_V  0x00000003U
#define SDHOST_NUM_CLK_DIV_REG_S  24

/** SDHOST_UHS_REG register
 *  UHS-1 register
 */
#define SDHOST_UHS_REG (DR_REG_SDHOST_BASE + 0x74)
/** SDHOST_DDR_REG : R/W; bitpos: [17:16]; default: 0;
 *  DDR mode selecton,1 bit for each card.
 *  0-Non-DDR mdoe.
 *  1-DDR mdoe.
 */
#define SDHOST_DDR_REG    0x00000003U
#define SDHOST_DDR_REG_M  (SDHOST_DDR_REG_V << SDHOST_DDR_REG_S)
#define SDHOST_DDR_REG_V  0x00000003U
#define SDHOST_DDR_REG_S  16

/** SDHOST_RST_N_REG register
 *  Card reset register
 */
#define SDHOST_RST_N_REG (DR_REG_SDHOST_BASE + 0x78)
/** SDHOST_CARD_RESET : R/W; bitpos: [1:0]; default: 1;
 *  Hardware reset.
 *  1: Active mode;
 *  0: Reset.
 *  These bits cause the cards to enter pre-idle state, which requires them to be
 *  re-initialized. SDHOST_RST_CARD_RESET[0] should be set to 1'b0 to reset card0,
 *  SDHOST_RST_CARD_RESET[1] should be set to 1'b0 to reset card1.
 */
#define SDHOST_CARD_RESET    0x00000003U
#define SDHOST_CARD_RESET_M  (SDHOST_CARD_RESET_V << SDHOST_CARD_RESET_S)
#define SDHOST_CARD_RESET_V  0x00000003U
#define SDHOST_CARD_RESET_S  0

/** SDHOST_BMOD_REG register
 *  Burst mode transfer configuration register
 */
#define SDHOST_BMOD_REG (DR_REG_SDHOST_BASE + 0x80)
/** SDHOST_BMOD_SWR : R/W; bitpos: [0]; default: 0;
 *  Software Reset. When set, the DMA Controller resets all its internal registers. It
 *  is automatically cleared after one clock cycle.
 */
#define SDHOST_BMOD_SWR    (BIT(0))
#define SDHOST_BMOD_SWR_M  (SDHOST_BMOD_SWR_V << SDHOST_BMOD_SWR_S)
#define SDHOST_BMOD_SWR_V  0x00000001U
#define SDHOST_BMOD_SWR_S  0
/** SDHOST_BMOD_FB : R/W; bitpos: [1]; default: 0;
 *  Fixed Burst. Controls whether the AHB Master interface performs fixed burst
 *  transfers or not. When set, the AHB will use only SINGLE, INCR4, INCR8 or INCR16
 *  during start of normal burst transfers. When reset, the AHB will use SINGLE and
 *  INCR burst transfer operations.
 */
#define SDHOST_BMOD_FB    (BIT(1))
#define SDHOST_BMOD_FB_M  (SDHOST_BMOD_FB_V << SDHOST_BMOD_FB_S)
#define SDHOST_BMOD_FB_V  0x00000001U
#define SDHOST_BMOD_FB_S  1
/** SDHOST_BMOD_DE : R/W; bitpos: [7]; default: 0;
 *  IDMAC Enable. When set, the IDMAC is enabled.
 */
#define SDHOST_BMOD_DE    (BIT(7))
#define SDHOST_BMOD_DE_M  (SDHOST_BMOD_DE_V << SDHOST_BMOD_DE_S)
#define SDHOST_BMOD_DE_V  0x00000001U
#define SDHOST_BMOD_DE_S  7
/** SDHOST_BMOD_PBL : R/W; bitpos: [10:8]; default: 0;
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
#define SDHOST_BMOD_PBL    0x00000007U
#define SDHOST_BMOD_PBL_M  (SDHOST_BMOD_PBL_V << SDHOST_BMOD_PBL_S)
#define SDHOST_BMOD_PBL_V  0x00000007U
#define SDHOST_BMOD_PBL_S  8

/** SDHOST_PLDMND_REG register
 *  Poll demand configuration register
 */
#define SDHOST_PLDMND_REG (DR_REG_SDHOST_BASE + 0x84)
/** SDHOST_PLDMND_PD : WO; bitpos: [31:0]; default: 0;
 *  Poll Demand. If the OWNER bit of a descriptor is not set, the FSM goes to the
 *  Suspend state. The host needs to write any value into this register for the IDMAC
 *  FSM to resume normal descriptor fetch operation. This is a write only .
 */
#define SDHOST_PLDMND_PD    0xFFFFFFFFU
#define SDHOST_PLDMND_PD_M  (SDHOST_PLDMND_PD_V << SDHOST_PLDMND_PD_S)
#define SDHOST_PLDMND_PD_V  0xFFFFFFFFU
#define SDHOST_PLDMND_PD_S  0

/** SDHOST_DBADDR_REG register
 *  Descriptor base address register
 */
#define SDHOST_DBADDR_REG (DR_REG_SDHOST_BASE + 0x88)
/** SDHOST_DBADDR : R/W; bitpos: [31:0]; default: 0;
 *  Start of Descriptor List. Contains the base address of the First Descriptor. The
 *  LSB bits [1:0] are ignored and taken as all-zero by the IDMAC internally. Hence
 *  these LSB bits may be treated as read-only.
 */
#define SDHOST_DBADDR    0xFFFFFFFFU
#define SDHOST_DBADDR_M  (SDHOST_DBADDR_V << SDHOST_DBADDR_S)
#define SDHOST_DBADDR_V  0xFFFFFFFFU
#define SDHOST_DBADDR_S  0

/** SDHOST_IDSTS_REG register
 *  IDMAC status register
 */
#define SDHOST_IDSTS_REG (DR_REG_SDHOST_BASE + 0x8c)
/** SDHOST_IDSTS_TI : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt. Indicates that data transmission is finished for a descriptor.
 *  Writing 1 clears this bit.
 */
#define SDHOST_IDSTS_TI    (BIT(0))
#define SDHOST_IDSTS_TI_M  (SDHOST_IDSTS_TI_V << SDHOST_IDSTS_TI_S)
#define SDHOST_IDSTS_TI_V  0x00000001U
#define SDHOST_IDSTS_TI_S  0
/** SDHOST_IDSTS_RI : R/W; bitpos: [1]; default: 0;
 *  Receive Interrupt. Indicates the completion of data reception for a descriptor.
 *  Writing 1 clears this bit.
 */
#define SDHOST_IDSTS_RI    (BIT(1))
#define SDHOST_IDSTS_RI_M  (SDHOST_IDSTS_RI_V << SDHOST_IDSTS_RI_S)
#define SDHOST_IDSTS_RI_V  0x00000001U
#define SDHOST_IDSTS_RI_S  1
/** SDHOST_IDSTS_FBE : R/W; bitpos: [2]; default: 0;
 *  Fatal Bus Error Interrupt. Indicates that a Bus Error occurred (IDSTS[12:10]) .
 *  When this bit is set, the DMA disables all its bus accesses. Writing 1 clears this
 *  bit.
 */
#define SDHOST_IDSTS_FBE    (BIT(2))
#define SDHOST_IDSTS_FBE_M  (SDHOST_IDSTS_FBE_V << SDHOST_IDSTS_FBE_S)
#define SDHOST_IDSTS_FBE_V  0x00000001U
#define SDHOST_IDSTS_FBE_S  2
/** SDHOST_IDSTS_DU : R/W; bitpos: [4]; default: 0;
 *  Descriptor Unavailable Interrupt. This bit is set when the descriptor is
 *  unavailable due to OWNER bit = 0 (DES0[31] = 0). Writing 1 clears this bit.
 */
#define SDHOST_IDSTS_DU    (BIT(4))
#define SDHOST_IDSTS_DU_M  (SDHOST_IDSTS_DU_V << SDHOST_IDSTS_DU_S)
#define SDHOST_IDSTS_DU_V  0x00000001U
#define SDHOST_IDSTS_DU_S  4
/** SDHOST_IDSTS_CES : R/W; bitpos: [5]; default: 0;
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
#define SDHOST_IDSTS_CES    (BIT(5))
#define SDHOST_IDSTS_CES_M  (SDHOST_IDSTS_CES_V << SDHOST_IDSTS_CES_S)
#define SDHOST_IDSTS_CES_V  0x00000001U
#define SDHOST_IDSTS_CES_S  5
/** SDHOST_IDSTS_NIS : R/W; bitpos: [8]; default: 0;
 *  Normal Interrupt Summary. Logical OR of the following: IDSTS[0] : Transmit
 *  Interrupt, IDSTS[1] : Receive Interrupt. Only unmasked bits affect this bit. This
 *  is a sticky bit and must be cleared each time a corresponding bit that causes NIS
 *  to be set is cleared. Writing 1 clears this bit.
 */
#define SDHOST_IDSTS_NIS    (BIT(8))
#define SDHOST_IDSTS_NIS_M  (SDHOST_IDSTS_NIS_V << SDHOST_IDSTS_NIS_S)
#define SDHOST_IDSTS_NIS_V  0x00000001U
#define SDHOST_IDSTS_NIS_S  8
/** SDHOST_IDSTS_AIS : R/W; bitpos: [9]; default: 0;
 *  Abnormal Interrupt Summary. Logical OR of the following: IDSTS[2] : Fatal Bus
 *  Interrupt, IDSTS[4] : DU bit Interrupt. Only unmasked bits affect this bit. This is
 *  a sticky bit and must be cleared each time a corresponding bit that causes AIS to
 *  be set is cleared. Writing 1 clears this bit.
 */
#define SDHOST_IDSTS_AIS    (BIT(9))
#define SDHOST_IDSTS_AIS_M  (SDHOST_IDSTS_AIS_V << SDHOST_IDSTS_AIS_S)
#define SDHOST_IDSTS_AIS_V  0x00000001U
#define SDHOST_IDSTS_AIS_S  9
/** SDHOST_IDSTS_FBE_CODE : R/W; bitpos: [12:10]; default: 0;
 *  Fatal Bus Error Code. Indicates the type of error that caused a Bus Error. Valid
 *  only when the Fatal Bus Error bit IDSTS[2] is set. This field does not generate an
 *  interrupt.
 *  001: Host Abort received during transmission;
 *  010: Host Abort received during reception;
 *  Others: Reserved.
 */
#define SDHOST_IDSTS_FBE_CODE    0x00000007U
#define SDHOST_IDSTS_FBE_CODE_M  (SDHOST_IDSTS_FBE_CODE_V << SDHOST_IDSTS_FBE_CODE_S)
#define SDHOST_IDSTS_FBE_CODE_V  0x00000007U
#define SDHOST_IDSTS_FBE_CODE_S  10
/** SDHOST_IDSTS_FSM : R/W; bitpos: [16:13]; default: 0;
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
#define SDHOST_IDSTS_FSM    0x0000000FU
#define SDHOST_IDSTS_FSM_M  (SDHOST_IDSTS_FSM_V << SDHOST_IDSTS_FSM_S)
#define SDHOST_IDSTS_FSM_V  0x0000000FU
#define SDHOST_IDSTS_FSM_S  13

/** SDHOST_IDINTEN_REG register
 *  IDMAC interrupt enable register
 */
#define SDHOST_IDINTEN_REG (DR_REG_SDHOST_BASE + 0x90)
/** SDHOST_IDINTEN_TI : R/W; bitpos: [0]; default: 0;
 *  Transmit Interrupt Enable. When set with Normal Interrupt Summary Enable, Transmit
 *  Interrupt is enabled. When reset, Transmit Interrupt is disabled.
 */
#define SDHOST_IDINTEN_TI    (BIT(0))
#define SDHOST_IDINTEN_TI_M  (SDHOST_IDINTEN_TI_V << SDHOST_IDINTEN_TI_S)
#define SDHOST_IDINTEN_TI_V  0x00000001U
#define SDHOST_IDINTEN_TI_S  0
/** SDHOST_IDINTEN_RI : R/W; bitpos: [1]; default: 0;
 *  Receive Interrupt Enable. When set with Normal Interrupt Summary Enable, Receive
 *  Interrupt is enabled. When reset, Receive Interrupt is disabled.
 */
#define SDHOST_IDINTEN_RI    (BIT(1))
#define SDHOST_IDINTEN_RI_M  (SDHOST_IDINTEN_RI_V << SDHOST_IDINTEN_RI_S)
#define SDHOST_IDINTEN_RI_V  0x00000001U
#define SDHOST_IDINTEN_RI_S  1
/** SDHOST_IDINTEN_FBE : R/W; bitpos: [2]; default: 0;
 *  Fatal Bus Error Enable. When set with Abnormal Interrupt Summary Enable, the Fatal
 *  Bus Error Interrupt is enabled. When reset, Fatal Bus Error Enable Interrupt is
 *  disabled.
 */
#define SDHOST_IDINTEN_FBE    (BIT(2))
#define SDHOST_IDINTEN_FBE_M  (SDHOST_IDINTEN_FBE_V << SDHOST_IDINTEN_FBE_S)
#define SDHOST_IDINTEN_FBE_V  0x00000001U
#define SDHOST_IDINTEN_FBE_S  2
/** SDHOST_IDINTEN_DU : R/W; bitpos: [4]; default: 0;
 *  Descriptor Unavailable Interrupt. When set along with Abnormal Interrupt Summary
 *  Enable, the DU interrupt is enabled.
 */
#define SDHOST_IDINTEN_DU    (BIT(4))
#define SDHOST_IDINTEN_DU_M  (SDHOST_IDINTEN_DU_V << SDHOST_IDINTEN_DU_S)
#define SDHOST_IDINTEN_DU_V  0x00000001U
#define SDHOST_IDINTEN_DU_S  4
/** SDHOST_IDINTEN_CES : R/W; bitpos: [5]; default: 0;
 *  Card Error summary Interrupt Enable. When set, it enables the Card Interrupt
 *  summary.
 */
#define SDHOST_IDINTEN_CES    (BIT(5))
#define SDHOST_IDINTEN_CES_M  (SDHOST_IDINTEN_CES_V << SDHOST_IDINTEN_CES_S)
#define SDHOST_IDINTEN_CES_V  0x00000001U
#define SDHOST_IDINTEN_CES_S  5
/** SDHOST_IDINTEN_NI : R/W; bitpos: [8]; default: 0;
 *  Normal Interrupt Summary Enable. When set, a normal interrupt is enabled. When
 *  reset, a normal interrupt is disabled. This bit enables the following bits:
 *  IDINTEN[0]: Transmit Interrupt;
 *  IDINTEN[1]: Receive Interrupt.
 */
#define SDHOST_IDINTEN_NI    (BIT(8))
#define SDHOST_IDINTEN_NI_M  (SDHOST_IDINTEN_NI_V << SDHOST_IDINTEN_NI_S)
#define SDHOST_IDINTEN_NI_V  0x00000001U
#define SDHOST_IDINTEN_NI_S  8
/** SDHOST_IDINTEN_AI : R/W; bitpos: [9]; default: 0;
 *  Abnormal Interrupt Summary Enable. When set, an abnormal interrupt is enabled. This
 *  bit enables the following bits:
 *  IDINTEN[2]: Fatal Bus Error Interrupt;
 *  IDINTEN[4]: DU Interrupt.
 */
#define SDHOST_IDINTEN_AI    (BIT(9))
#define SDHOST_IDINTEN_AI_M  (SDHOST_IDINTEN_AI_V << SDHOST_IDINTEN_AI_S)
#define SDHOST_IDINTEN_AI_V  0x00000001U
#define SDHOST_IDINTEN_AI_S  9

/** SDHOST_DSCADDR_REG register
 *  Host descriptor address pointer
 */
#define SDHOST_DSCADDR_REG (DR_REG_SDHOST_BASE + 0x94)
/** SDHOST_DSCADDR : RO; bitpos: [31:0]; default: 0;
 *  Host Descriptor Address Pointer, updated by IDMAC during operation and cleared on
 *  reset. This register points to the start address of the current descriptor read by
 *  the IDMAC.
 */
#define SDHOST_DSCADDR    0xFFFFFFFFU
#define SDHOST_DSCADDR_M  (SDHOST_DSCADDR_V << SDHOST_DSCADDR_S)
#define SDHOST_DSCADDR_V  0xFFFFFFFFU
#define SDHOST_DSCADDR_S  0

/** SDHOST_BUFADDR_REG register
 *  Host buffer address pointer register
 */
#define SDHOST_BUFADDR_REG (DR_REG_SDHOST_BASE + 0x98)
/** SDHOST_BUFADDR : RO; bitpos: [31:0]; default: 0;
 *  Host Buffer Address Pointer, updated by IDMAC during operation and cleared on
 *  reset. This register points to the current Data Buffer Address being accessed by
 *  the IDMAC.
 */
#define SDHOST_BUFADDR    0xFFFFFFFFU
#define SDHOST_BUFADDR_M  (SDHOST_BUFADDR_V << SDHOST_BUFADDR_S)
#define SDHOST_BUFADDR_V  0xFFFFFFFFU
#define SDHOST_BUFADDR_S  0

/** SDHOST_CARDTHRCTL_REG register
 *  Card Threshold Control register
 */
#define SDHOST_CARDTHRCTL_REG (DR_REG_SDHOST_BASE + 0x100)
/** SDHOST_CARDRDTHREN_REG : R/W; bitpos: [0]; default: 0;
 *  Card read threshold enable.
 *  1'b0-Card read threshold disabled.
 *  1'b1-Card read threshold enabled.
 */
#define SDHOST_CARDRDTHREN_REG    (BIT(0))
#define SDHOST_CARDRDTHREN_REG_M  (SDHOST_CARDRDTHREN_REG_V << SDHOST_CARDRDTHREN_REG_S)
#define SDHOST_CARDRDTHREN_REG_V  0x00000001U
#define SDHOST_CARDRDTHREN_REG_S  0
/** SDHOST_CARDCLRINTEN_REG : R/W; bitpos: [1]; default: 0;
 *  Busy clear interrupt generation:
 *  1'b0-Busy clear interrypt disabled.
 *  1'b1-Busy clear interrypt enabled.
 */
#define SDHOST_CARDCLRINTEN_REG    (BIT(1))
#define SDHOST_CARDCLRINTEN_REG_M  (SDHOST_CARDCLRINTEN_REG_V << SDHOST_CARDCLRINTEN_REG_S)
#define SDHOST_CARDCLRINTEN_REG_V  0x00000001U
#define SDHOST_CARDCLRINTEN_REG_S  1
/** SDHOST_CARDWRTHREN_REG : R/W; bitpos: [2]; default: 0;
 *  Applicable when HS400 mode is enabled.
 *  1'b0-Card write Threshold disabled.
 *  1'b1-Card write Threshold enabled.
 */
#define SDHOST_CARDWRTHREN_REG    (BIT(2))
#define SDHOST_CARDWRTHREN_REG_M  (SDHOST_CARDWRTHREN_REG_V << SDHOST_CARDWRTHREN_REG_S)
#define SDHOST_CARDWRTHREN_REG_V  0x00000001U
#define SDHOST_CARDWRTHREN_REG_S  2
/** SDHOST_CARDTHRESHOLD_REG : R/W; bitpos: [31:16]; default: 0;
 *  The inside FIFO size is 512,This register is applicable when SDHOST_CARDERTHREN_REG
 *  is set to 1 or SDHOST_CARDRDTHREN_REG set to 1.
 */
#define SDHOST_CARDTHRESHOLD_REG    0x0000FFFFU
#define SDHOST_CARDTHRESHOLD_REG_M  (SDHOST_CARDTHRESHOLD_REG_V << SDHOST_CARDTHRESHOLD_REG_S)
#define SDHOST_CARDTHRESHOLD_REG_V  0x0000FFFFU
#define SDHOST_CARDTHRESHOLD_REG_S  16

/** SDHOST_EMMCDDR_REG register
 *  eMMC DDR register
 */
#define SDHOST_EMMCDDR_REG (DR_REG_SDHOST_BASE + 0x10c)
/** SDHOST_HALFSTARTBIT_REG : R/W; bitpos: [1:0]; default: 0;
 *  Control for start bit detection mechanism duration of start bit.Each bit refers to
 *  one slot.Set this bit to 1 for eMMC4.5 and above,set to 0 for SD applications.For
 *  eMMC4.5,start bit can be:
 *  1'b0-Full cycle.
 *  1'b1-less than one full cycle.
 */
#define SDHOST_HALFSTARTBIT_REG    0x00000003U
#define SDHOST_HALFSTARTBIT_REG_M  (SDHOST_HALFSTARTBIT_REG_V << SDHOST_HALFSTARTBIT_REG_S)
#define SDHOST_HALFSTARTBIT_REG_V  0x00000003U
#define SDHOST_HALFSTARTBIT_REG_S  0
/** SDHOST_HS400_MODE_REG : R/W; bitpos: [31]; default: 0;
 *  Set 1 to enable HS400 mode.
 */
#define SDHOST_HS400_MODE_REG    (BIT(31))
#define SDHOST_HS400_MODE_REG_M  (SDHOST_HS400_MODE_REG_V << SDHOST_HS400_MODE_REG_S)
#define SDHOST_HS400_MODE_REG_V  0x00000001U
#define SDHOST_HS400_MODE_REG_S  31

/** SDHOST_ENSHIFT_REG register
 *  Enable Phase Shift register
 */
#define SDHOST_ENSHIFT_REG (DR_REG_SDHOST_BASE + 0x110)
/** SDHOST_ENABLE_SHIFT_REG : R/W; bitpos: [3:0]; default: 0;
 *  Control for the amount of phase shift provided on the default enables in the
 *  design.Two bits assigned for each card.
 *  2'b00-Default phase shift.
 *  2'b01-Enables shifted to next immediate positive edge.
 *  2'b10-Enables shifted to next immediate negative edge.
 *  2'b11-Reserved.
 */
#define SDHOST_ENABLE_SHIFT_REG    0x0000000FU
#define SDHOST_ENABLE_SHIFT_REG_M  (SDHOST_ENABLE_SHIFT_REG_V << SDHOST_ENABLE_SHIFT_REG_S)
#define SDHOST_ENABLE_SHIFT_REG_V  0x0000000FU
#define SDHOST_ENABLE_SHIFT_REG_S  0

/** SDHOST_BUFFIFO_REG register
 *  CPU write and read transmit data by FIFO
 */
#define SDHOST_BUFFIFO_REG (DR_REG_SDHOST_BASE + 0x200)
/** SDHOST_BUFFIFO : R/W; bitpos: [31:0]; default: 0;
 *  CPU write and read transmit data by FIFO. This register points to the current Data
 *  FIFO .
 */
#define SDHOST_BUFFIFO    0xFFFFFFFFU
#define SDHOST_BUFFIFO_M  (SDHOST_BUFFIFO_V << SDHOST_BUFFIFO_S)
#define SDHOST_BUFFIFO_V  0xFFFFFFFFU
#define SDHOST_BUFFIFO_S  0

/** SDHOST_CLK_EDGE_SEL_REG register
 *  SDIO control register.
 */
#define SDHOST_CLK_EDGE_SEL_REG (DR_REG_SDHOST_BASE + 0x800)
/** SDHOST_CCLKIN_EDGE_DRV_SEL : R/W; bitpos: [2:0]; default: 0;
 *  It's used to select the clock phase of the output signal from phase 0, phase 90,
 *  phase 180, phase 270.
 */
#define SDHOST_CCLKIN_EDGE_DRV_SEL    0x00000007U
#define SDHOST_CCLKIN_EDGE_DRV_SEL_M  (SDHOST_CCLKIN_EDGE_DRV_SEL_V << SDHOST_CCLKIN_EDGE_DRV_SEL_S)
#define SDHOST_CCLKIN_EDGE_DRV_SEL_V  0x00000007U
#define SDHOST_CCLKIN_EDGE_DRV_SEL_S  0
/** SDHOST_CCLKIN_EDGE_SAM_SEL : R/W; bitpos: [5:3]; default: 0;
 *  It's used to select the clock phase of the input signal from phase 0, phase 90,
 *  phase 180, phase 270.
 */
#define SDHOST_CCLKIN_EDGE_SAM_SEL    0x00000007U
#define SDHOST_CCLKIN_EDGE_SAM_SEL_M  (SDHOST_CCLKIN_EDGE_SAM_SEL_V << SDHOST_CCLKIN_EDGE_SAM_SEL_S)
#define SDHOST_CCLKIN_EDGE_SAM_SEL_V  0x00000007U
#define SDHOST_CCLKIN_EDGE_SAM_SEL_S  3
/** SDHOST_CCLKIN_EDGE_SLF_SEL : R/W; bitpos: [8:6]; default: 0;
 *  It's used to select the clock phase of the internal signal from phase 0, phase 90,
 *  phase 180, phase 270.
 */
#define SDHOST_CCLKIN_EDGE_SLF_SEL    0x00000007U
#define SDHOST_CCLKIN_EDGE_SLF_SEL_M  (SDHOST_CCLKIN_EDGE_SLF_SEL_V << SDHOST_CCLKIN_EDGE_SLF_SEL_S)
#define SDHOST_CCLKIN_EDGE_SLF_SEL_V  0x00000007U
#define SDHOST_CCLKIN_EDGE_SLF_SEL_S  6
/** SDHOST_CCLLKIN_EDGE_H : R/W; bitpos: [12:9]; default: 1;
 *  The high level of the divider clock. The value should be smaller than CCLKIN_EDGE_L.
 */
#define SDHOST_CCLLKIN_EDGE_H    0x0000000FU
#define SDHOST_CCLLKIN_EDGE_H_M  (SDHOST_CCLLKIN_EDGE_H_V << SDHOST_CCLLKIN_EDGE_H_S)
#define SDHOST_CCLLKIN_EDGE_H_V  0x0000000FU
#define SDHOST_CCLLKIN_EDGE_H_S  9
/** SDHOST_CCLLKIN_EDGE_L : R/W; bitpos: [16:13]; default: 0;
 *  The low level of the divider clock. The value should be larger than CCLKIN_EDGE_H.
 */
#define SDHOST_CCLLKIN_EDGE_L    0x0000000FU
#define SDHOST_CCLLKIN_EDGE_L_M  (SDHOST_CCLLKIN_EDGE_L_V << SDHOST_CCLLKIN_EDGE_L_S)
#define SDHOST_CCLLKIN_EDGE_L_V  0x0000000FU
#define SDHOST_CCLLKIN_EDGE_L_S  13
/** SDHOST_CCLLKIN_EDGE_N : R/W; bitpos: [20:17]; default: 1;
 *  The clock division of cclk_in.
 */
#define SDHOST_CCLLKIN_EDGE_N    0x0000000FU
#define SDHOST_CCLLKIN_EDGE_N_M  (SDHOST_CCLLKIN_EDGE_N_V << SDHOST_CCLLKIN_EDGE_N_S)
#define SDHOST_CCLLKIN_EDGE_N_V  0x0000000FU
#define SDHOST_CCLLKIN_EDGE_N_S  17
/** SDHOST_ESDIO_MODE : R/W; bitpos: [21]; default: 0;
 *  Enable esdio mode.
 */
#define SDHOST_ESDIO_MODE    (BIT(21))
#define SDHOST_ESDIO_MODE_M  (SDHOST_ESDIO_MODE_V << SDHOST_ESDIO_MODE_S)
#define SDHOST_ESDIO_MODE_V  0x00000001U
#define SDHOST_ESDIO_MODE_S  21
/** SDHOST_ESD_MODE : R/W; bitpos: [22]; default: 0;
 *  Enable esd mode.
 */
#define SDHOST_ESD_MODE    (BIT(22))
#define SDHOST_ESD_MODE_M  (SDHOST_ESD_MODE_V << SDHOST_ESD_MODE_S)
#define SDHOST_ESD_MODE_V  0x00000001U
#define SDHOST_ESD_MODE_S  22
/** SDHOST_CCLK_EN : R/W; bitpos: [23]; default: 1;
 *  Sdio clock enable.
 */
#define SDHOST_CCLK_EN    (BIT(23))
#define SDHOST_CCLK_EN_M  (SDHOST_CCLK_EN_V << SDHOST_CCLK_EN_S)
#define SDHOST_CCLK_EN_V  0x00000001U
#define SDHOST_CCLK_EN_S  23
/** SDHOST_ULTRA_HIGH_SPEED_MODE : R/W; bitpos: [24]; default: 0;
 *  Enable ultra high speed mode, use dll to generate clk.
 */
#define SDHOST_ULTRA_HIGH_SPEED_MODE    (BIT(24))
#define SDHOST_ULTRA_HIGH_SPEED_MODE_M  (SDHOST_ULTRA_HIGH_SPEED_MODE_V << SDHOST_ULTRA_HIGH_SPEED_MODE_S)
#define SDHOST_ULTRA_HIGH_SPEED_MODE_V  0x00000001U
#define SDHOST_ULTRA_HIGH_SPEED_MODE_S  24

/** SDHOST_RAW_INTS_REG register
 *  SDIO raw ints register.
 */
#define SDHOST_RAW_INTS_REG (DR_REG_SDHOST_BASE + 0x804)
/** SDHOST_RAW_INTS : RO; bitpos: [31:0]; default: 0;
 *  It indicates raw ints.
 */
#define SDHOST_RAW_INTS    0xFFFFFFFFU
#define SDHOST_RAW_INTS_M  (SDHOST_RAW_INTS_V << SDHOST_RAW_INTS_S)
#define SDHOST_RAW_INTS_V  0xFFFFFFFFU
#define SDHOST_RAW_INTS_S  0

/** SDHOST_DLL_CLK_CONF_REG register
 *  SDIO DLL clock control register.
 */
#define SDHOST_DLL_CLK_CONF_REG (DR_REG_SDHOST_BASE + 0x808)
/** SDHOST_DLL_CCLK_IN_SLF_EN : R/W; bitpos: [0]; default: 0;
 *  Clock enable of cclk_in_slf when ULTRA_HIGH_SPEED_MODE==1.
 */
#define SDHOST_DLL_CCLK_IN_SLF_EN    (BIT(0))
#define SDHOST_DLL_CCLK_IN_SLF_EN_M  (SDHOST_DLL_CCLK_IN_SLF_EN_V << SDHOST_DLL_CCLK_IN_SLF_EN_S)
#define SDHOST_DLL_CCLK_IN_SLF_EN_V  0x00000001U
#define SDHOST_DLL_CCLK_IN_SLF_EN_S  0
/** SDHOST_DLL_CCLK_IN_DRV_EN : R/W; bitpos: [1]; default: 0;
 *  Clock enable of cclk_in_drv when ULTRA_HIGH_SPEED_MODE==1.
 */
#define SDHOST_DLL_CCLK_IN_DRV_EN    (BIT(1))
#define SDHOST_DLL_CCLK_IN_DRV_EN_M  (SDHOST_DLL_CCLK_IN_DRV_EN_V << SDHOST_DLL_CCLK_IN_DRV_EN_S)
#define SDHOST_DLL_CCLK_IN_DRV_EN_V  0x00000001U
#define SDHOST_DLL_CCLK_IN_DRV_EN_S  1
/** SDHOST_DLL_CCLK_IN_SAM_EN : R/W; bitpos: [2]; default: 0;
 *  Clock enable of cclk_in_sam when ULTRA_HIGH_SPEED_MODE==1.
 */
#define SDHOST_DLL_CCLK_IN_SAM_EN    (BIT(2))
#define SDHOST_DLL_CCLK_IN_SAM_EN_M  (SDHOST_DLL_CCLK_IN_SAM_EN_V << SDHOST_DLL_CCLK_IN_SAM_EN_S)
#define SDHOST_DLL_CCLK_IN_SAM_EN_V  0x00000001U
#define SDHOST_DLL_CCLK_IN_SAM_EN_S  2
/** SDHOST_DLL_CCLK_IN_SLF_PHASE : R/W; bitpos: [8:3]; default: 0;
 *  It's used to control the phase of cclk_in_slf when ULTRA_HIGH_SPEED_MODE==1.
 */
#define SDHOST_DLL_CCLK_IN_SLF_PHASE    0x0000003FU
#define SDHOST_DLL_CCLK_IN_SLF_PHASE_M  (SDHOST_DLL_CCLK_IN_SLF_PHASE_V << SDHOST_DLL_CCLK_IN_SLF_PHASE_S)
#define SDHOST_DLL_CCLK_IN_SLF_PHASE_V  0x0000003FU
#define SDHOST_DLL_CCLK_IN_SLF_PHASE_S  3
/** SDHOST_DLL_CCLK_IN_DRV_PHASE : R/W; bitpos: [14:9]; default: 0;
 *  It's used to control the phase of cclk_in_drv when ULTRA_HIGH_SPEED_MODE==1.
 */
#define SDHOST_DLL_CCLK_IN_DRV_PHASE    0x0000003FU
#define SDHOST_DLL_CCLK_IN_DRV_PHASE_M  (SDHOST_DLL_CCLK_IN_DRV_PHASE_V << SDHOST_DLL_CCLK_IN_DRV_PHASE_S)
#define SDHOST_DLL_CCLK_IN_DRV_PHASE_V  0x0000003FU
#define SDHOST_DLL_CCLK_IN_DRV_PHASE_S  9
/** SDHOST_DLL_CCLK_IN_SAM_PHASE : R/W; bitpos: [20:15]; default: 0;
 *  It's used to control the phase of cclk_in_sam when ULTRA_HIGH_SPEED_MODE==1.
 */
#define SDHOST_DLL_CCLK_IN_SAM_PHASE    0x0000003FU
#define SDHOST_DLL_CCLK_IN_SAM_PHASE_M  (SDHOST_DLL_CCLK_IN_SAM_PHASE_V << SDHOST_DLL_CCLK_IN_SAM_PHASE_S)
#define SDHOST_DLL_CCLK_IN_SAM_PHASE_V  0x0000003FU
#define SDHOST_DLL_CCLK_IN_SAM_PHASE_S  15

/** SDHOST_DLL_CONF_REG register
 *  SDIO DLL configuration register.
 */
#define SDHOST_DLL_CONF_REG (DR_REG_SDHOST_BASE + 0x80c)
/** SDHOST_DLL_CAL_STOP : R/W; bitpos: [0]; default: 0;
 *  Set 1 to stop calibration.
 */
#define SDHOST_DLL_CAL_STOP    (BIT(0))
#define SDHOST_DLL_CAL_STOP_M  (SDHOST_DLL_CAL_STOP_V << SDHOST_DLL_CAL_STOP_S)
#define SDHOST_DLL_CAL_STOP_V  0x00000001U
#define SDHOST_DLL_CAL_STOP_S  0
/** SDHOST_DLL_CAL_END : RO; bitpos: [1]; default: 0;
 *  1 means calibration finished.
 */
#define SDHOST_DLL_CAL_END    (BIT(1))
#define SDHOST_DLL_CAL_END_M  (SDHOST_DLL_CAL_END_V << SDHOST_DLL_CAL_END_S)
#define SDHOST_DLL_CAL_END_V  0x00000001U
#define SDHOST_DLL_CAL_END_S  1


#define SDMMC_INTMASK_IO_SLOT1  BIT(17)
#define SDMMC_INTMASK_IO_SLOT0  BIT(16)
#define SDMMC_INTMASK_EBE       BIT(15)
#define SDMMC_INTMASK_ACD       BIT(14)
#define SDMMC_INTMASK_SBE       BIT(13)
#define SDMMC_INTMASK_HLE       BIT(12)
#define SDMMC_INTMASK_FRUN      BIT(11)
#define SDMMC_INTMASK_HTO       BIT(10)
#define SDMMC_INTMASK_DTO       BIT(9)
#define SDMMC_INTMASK_RTO       BIT(8)
#define SDMMC_INTMASK_DCRC      BIT(7)
#define SDMMC_INTMASK_RCRC      BIT(6)
#define SDMMC_INTMASK_RXDR      BIT(5)
#define SDMMC_INTMASK_TXDR      BIT(4)
#define SDMMC_INTMASK_DATA_OVER BIT(3)
#define SDMMC_INTMASK_CMD_DONE  BIT(2)
#define SDMMC_INTMASK_RESP_ERR  BIT(1)
#define SDMMC_INTMASK_CD        BIT(0)

#define SDMMC_IDMAC_INTMASK_AI  BIT(9)
#define SDMMC_IDMAC_INTMASK_NI  BIT(8)
#define SDMMC_IDMAC_INTMASK_CES BIT(5)
#define SDMMC_IDMAC_INTMASK_DU  BIT(4)
#define SDMMC_IDMAC_INTMASK_FBE BIT(2)
#define SDMMC_IDMAC_INTMASK_RI  BIT(1)
#define SDMMC_IDMAC_INTMASK_TI  BIT(0)

#ifdef __cplusplus
}
#endif
