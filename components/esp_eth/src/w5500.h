// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define W5500_ADDR_OFFSET (16) // Address length
#define W5500_BSB_OFFSET  (3)  // Block Select Bits offset
#define W5500_RWB_OFFSET  (2)  // Read Write Bits offset

#define W5500_BSB_COM_REG        (0x00)    // Common Register
#define W5500_BSB_SOCK_REG(s)    ((s)*4+1) // Socket Register
#define W5500_BSB_SOCK_TX_BUF(s) ((s)*4+2) // Socket TX Buffer
#define W5500_BSB_SOCK_RX_BUF(s) ((s)*4+3) // Socket RX Buffer

#define W5500_ACCESS_MODE_READ  (0) // Read Mode
#define W5500_ACCESS_MODE_WRITE (1) // Write Mode

#define W5500_SPI_OP_MODE_VDM   (0x00) // Variable Data Length Mode (SPI frame is controlled by CS line)
#define W5500_SPI_OP_MODE_FDM_1 (0x01) // Fixed Data Length Mode, 1 Byte Length
#define W5500_SPI_OP_MODE_FDM_2 (0x02) // Fixed Data Length Mode, 2 Bytes Length
#define W5500_SPI_OP_MODE_FDM_4 (0x03) // Fixed Data Length Mode, 4 Bytes Length

#define W5500_MAKE_MAP(offset, bsb) ((offset) << W5500_ADDR_OFFSET | (bsb) << W5500_BSB_OFFSET)

#define W5500_REG_MR        W5500_MAKE_MAP(0x0000, W5500_BSB_COM_REG) // Mode
#define W5500_REG_MAC       W5500_MAKE_MAP(0x0009, W5500_BSB_COM_REG) // MAC Address
#define W5500_REG_INTLEVEL  W5500_MAKE_MAP(0x0013, W5500_BSB_COM_REG) // Interrupt Level Timeout
#define W5500_REG_IR        W5500_MAKE_MAP(0x0015, W5500_BSB_COM_REG) // Interrupt
#define W5500_REG_IMR       W5500_MAKE_MAP(0x0016, W5500_BSB_COM_REG) // Interrupt Mask
#define W5500_REG_SIR       W5500_MAKE_MAP(0x0017, W5500_BSB_COM_REG) // Socket Interrupt
#define W5500_REG_SIMR      W5500_MAKE_MAP(0x0018, W5500_BSB_COM_REG) // Socket Interrupt Mask
#define W5500_REG_RTR       W5500_MAKE_MAP(0x0019, W5500_BSB_COM_REG) // Retry Time
#define W5500_REG_RCR       W5500_MAKE_MAP(0x001B, W5500_BSB_COM_REG) // Retry Count
#define W5500_REG_PHYCFGR   W5500_MAKE_MAP(0x002E, W5500_BSB_COM_REG) // PHY Configuration
#define W5500_REG_VERSIONR  W5500_MAKE_MAP(0x0039, W5500_BSB_COM_REG) // Chip version

#define W5500_REG_SOCK_MR(s)         W5500_MAKE_MAP(0x0000, W5500_BSB_SOCK_REG(s)) // Socket Mode
#define W5500_REG_SOCK_CR(s)         W5500_MAKE_MAP(0x0001, W5500_BSB_SOCK_REG(s)) // Socket Command
#define W5500_REG_SOCK_IR(s)         W5500_MAKE_MAP(0x0002, W5500_BSB_SOCK_REG(s)) // Socket Interrupt
#define W5500_REG_SOCK_SR(s)         W5500_MAKE_MAP(0x0004, W5500_BSB_SOCK_REG(s)) // Socket Status
#define W5500_REG_SOCK_RXBUF_SIZE(s) W5500_MAKE_MAP(0x001E, W5500_BSB_SOCK_REG(s)) // Socket Receive Buffer Size
#define W5500_REG_SOCK_TXBUF_SIZE(s) W5500_MAKE_MAP(0x001F, W5500_BSB_SOCK_REG(s)) // Socket Transmit Buffer Size
#define W5500_REG_SOCK_TX_FSR(s)     W5500_MAKE_MAP(0x0020, W5500_BSB_SOCK_REG(s)) // Socket TX Free Size
#define W5500_REG_SOCK_TX_RD(s)      W5500_MAKE_MAP(0x0022, W5500_BSB_SOCK_REG(s)) // Socket TX Read Pointer
#define W5500_REG_SOCK_TX_WR(s)      W5500_MAKE_MAP(0x0024, W5500_BSB_SOCK_REG(s)) // Socket TX Write Pointer
#define W5500_REG_SOCK_RX_RSR(s)     W5500_MAKE_MAP(0x0026, W5500_BSB_SOCK_REG(s)) // Socket RX Received Size
#define W5500_REG_SOCK_RX_RD(s)      W5500_MAKE_MAP(0x0028, W5500_BSB_SOCK_REG(s)) // Socket RX Read Pointer
#define W5500_REG_SOCK_RX_WR(s)      W5500_MAKE_MAP(0x002A, W5500_BSB_SOCK_REG(s)) // Socket RX Write Pointer
#define W5500_REG_SOCK_IMR(s)        W5500_MAKE_MAP(0x002C, W5500_BSB_SOCK_REG(s)) // Socket Interrupt Mask

#define W5500_MEM_SOCK_TX(s,addr) W5500_MAKE_MAP(addr, W5500_BSB_SOCK_TX_BUF(s)) // Socket TX buffer address
#define W5500_MEM_SOCK_RX(s,addr) W5500_MAKE_MAP(addr, W5500_BSB_SOCK_RX_BUF(s)) // Socket RX buffer address

#define W5500_MR_RST (1<<7) // Software reset
#define W5500_MR_PB  (1<<4) // Ping block (block the response to a ping request)

#define W5500_SIMR_SOCK0 (1<<0) // Socket 0 interrupt

#define W5500_SMR_MAC_RAW    (1<<2) // MAC RAW mode
#define W5500_SMR_MAC_FILTER (1<<7) // MAC filter

#define W5500_SCR_OPEN  (0x01) // Open command
#define W5500_SCR_CLOSE (0x10) // Close command
#define W5500_SCR_SEND  (0x20) // Send command
#define W5500_SCR_RECV  (0x40) // Recv command

#define W5500_SIR_RECV (1<<2)  // Receive done
#define W5500_SIR_SEND (1<<4)  // Send done
