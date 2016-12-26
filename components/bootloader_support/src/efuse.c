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
#include "esp_efuse.h"

#define EFUSE_CONF_WRITE 0x5A5A /* efuse_pgm_op_ena, force no rd/wr disable */
#define EFUSE_CONF_READ 0x5AA5 /* efuse_read_op_ena, release force */

#define EFUSE_CMD_PGM 0x02
#define EFUSE_CMD_READ 0x01

void esp_efuse_burn_new_values(void)
{
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_WRITE);
    REG_WRITE(EFUSE_CMD_REG,  EFUSE_CMD_PGM);
    while (REG_READ(EFUSE_CMD_REG) != 0) {
    }
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_READ);
    REG_WRITE(EFUSE_CMD_REG,  EFUSE_CMD_READ);
    while (REG_READ(EFUSE_CMD_REG) != 0) {
    }
    esp_efuse_reset();
}

void esp_efuse_reset(void)
{
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_READ);
    const uint32_t block_start[4] = { EFUSE_BLK0_WDATA0_REG, EFUSE_BLK1_WDATA0_REG,
                                      EFUSE_BLK2_WDATA0_REG, EFUSE_BLK3_WDATA0_REG };
    const uint32_t block_end[4] = { EFUSE_BLK0_WDATA6_REG, EFUSE_BLK1_WDATA7_REG,
                                    EFUSE_BLK2_WDATA7_REG, EFUSE_BLK3_WDATA7_REG };
    for (int i = 0; i < 4; i++) {
      for (uint32_t r = block_start[i]; r <= block_end[i]; r+= 4) {
        REG_WRITE(r, 0);
      }
    }
}
