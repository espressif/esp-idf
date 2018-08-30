// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_SDMMC_REG_H_
#define _SOC_SDMMC_REG_H_
#include "soc.h"

#define SDMMC_CTRL_REG          (DR_REG_SDMMC_BASE + 0x00)
#define SDMMC_PWREN_REG         (DR_REG_SDMMC_BASE + 0x04)
#define SDMMC_CLKDIV_REG        (DR_REG_SDMMC_BASE + 0x08)
#define SDMMC_CLKSRC_REG        (DR_REG_SDMMC_BASE + 0x0c)
#define SDMMC_CLKENA_REG        (DR_REG_SDMMC_BASE + 0x10)
#define SDMMC_TMOUT_REG         (DR_REG_SDMMC_BASE + 0x14)
#define SDMMC_CTYPE_REG         (DR_REG_SDMMC_BASE + 0x18)
#define SDMMC_BLKSIZ_REG        (DR_REG_SDMMC_BASE + 0x1c)
#define SDMMC_BYTCNT_REG        (DR_REG_SDMMC_BASE + 0x20)
#define SDMMC_INTMASK_REG       (DR_REG_SDMMC_BASE + 0x24)
#define SDMMC_CMDARG_REG        (DR_REG_SDMMC_BASE + 0x28)
#define SDMMC_CMD_REG           (DR_REG_SDMMC_BASE + 0x2c)
#define SDMMC_RESP0_REG         (DR_REG_SDMMC_BASE + 0x30)
#define SDMMC_RESP1_REG         (DR_REG_SDMMC_BASE + 0x34)
#define SDMMC_RESP2_REG         (DR_REG_SDMMC_BASE + 0x38)
#define SDMMC_RESP3_REG         (DR_REG_SDMMC_BASE + 0x3c)

#define SDMMC_MINTSTS_REG       (DR_REG_SDMMC_BASE + 0x40)
#define SDMMC_RINTSTS_REG       (DR_REG_SDMMC_BASE + 0x44)
#define SDMMC_STATUS_REG        (DR_REG_SDMMC_BASE + 0x48)
#define SDMMC_FIFOTH_REG        (DR_REG_SDMMC_BASE + 0x4c)
#define SDMMC_CDETECT_REG       (DR_REG_SDMMC_BASE + 0x50)
#define SDMMC_WRTPRT_REG        (DR_REG_SDMMC_BASE + 0x54)
#define SDMMC_GPIO_REG          (DR_REG_SDMMC_BASE + 0x58)
#define SDMMC_TCBCNT_REG        (DR_REG_SDMMC_BASE + 0x5c)
#define SDMMC_TBBCNT_REG        (DR_REG_SDMMC_BASE + 0x60)
#define SDMMC_DEBNCE_REG        (DR_REG_SDMMC_BASE + 0x64)
#define SDMMC_USRID_REG         (DR_REG_SDMMC_BASE + 0x68)
#define SDMMC_VERID_REG         (DR_REG_SDMMC_BASE + 0x6c)
#define SDMMC_HCON_REG          (DR_REG_SDMMC_BASE + 0x70)
#define SDMMC_UHS_REG_REG       (DR_REG_SDMMC_BASE + 0x74)
#define SDMMC_RST_N_REG         (DR_REG_SDMMC_BASE + 0x78)
#define SDMMC_BMOD_REG          (DR_REG_SDMMC_BASE + 0x80)
#define SDMMC_PLDMND_REG        (DR_REG_SDMMC_BASE + 0x84)
#define SDMMC_DBADDR_REG        (DR_REG_SDMMC_BASE + 0x88)
#define SDMMC_DBADDRU_REG       (DR_REG_SDMMC_BASE + 0x8c)
#define SDMMC_IDSTS_REG         (DR_REG_SDMMC_BASE + 0x8c)
#define SDMMC_IDINTEN_REG       (DR_REG_SDMMC_BASE + 0x90)
#define SDMMC_DSCADDR_REG       (DR_REG_SDMMC_BASE + 0x94)
#define SDMMC_DSCADDRL_REG      (DR_REG_SDMMC_BASE + 0x98)
#define SDMMC_DSCADDRU_REG      (DR_REG_SDMMC_BASE + 0x9c)
#define SDMMC_BUFADDRL_REG      (DR_REG_SDMMC_BASE + 0xa0)
#define SDMMC_BUFADDRU_REG      (DR_REG_SDMMC_BASE + 0xa4)
#define SDMMC_CARDTHRCTL_REG    (DR_REG_SDMMC_BASE + 0x100)
#define SDMMC_BACK_END_POWER_REG    (DR_REG_SDMMC_BASE + 0x104)
#define SDMMC_UHS_REG_EXT_REG   (DR_REG_SDMMC_BASE + 0x108)
#define SDMMC_EMMC_DDR_REG_REG  (DR_REG_SDMMC_BASE + 0x10c)
#define SDMMC_ENABLE_SHIFT_REG  (DR_REG_SDMMC_BASE + 0x110)

#define SDMMC_CLOCK_REG         (DR_REG_SDMMC_BASE + 0x800)

#define SDMMC_INTMASK_IO_SLOT1  BIT(17)
#define SDMMC_INTMASK_IO_SLOT0  BIT(16)
#define SDMMC_INTMASK_EBE       BIT(15)
#define SDMMC_INTMASK_ACD       BIT(14)
#define SDMMC_INTMASK_SBE       BIT(13)
#define SDMMC_INTMASK_BCI       BIT(13)
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

#endif /* _SOC_SDMMC_REG_H_  */
