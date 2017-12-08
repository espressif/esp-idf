// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "sys/lock.h"
#include "soc/sdmmc_reg.h"
#include "soc/sdmmc_struct.h"
#include "driver/sdmmc_types.h"
#include "driver/sdmmc_defs.h"
#include "driver/sdmmc_host.h"
#include "sdspi_private.h"
#include "sdspi_crc.h"

static const char* TAG = "sdspi_transaction";

static _lock_t s_lock;
static bool s_app_cmd;

static uint8_t sdspi_msg_crc7(sdspi_hw_cmd_t* hw_cmd)
{
    const size_t bytes_to_crc = offsetof(sdspi_hw_cmd_t, arguments) +
            sizeof(hw_cmd->arguments); /* can't take address of bit fields */
    return sdspi_crc7((const uint8_t *)hw_cmd, bytes_to_crc);
}

void make_hw_cmd(uint32_t opcode, uint32_t arg, int timeout_ms, sdspi_hw_cmd_t *hw_cmd)
{
    hw_cmd->start_bit = 0;
    hw_cmd->transmission_bit = 1;
    hw_cmd->cmd_index = opcode;
    hw_cmd->stop_bit = 1;
    hw_cmd->r1 = 0xff;
    memset(hw_cmd->response, 0xff, sizeof(hw_cmd->response));
    hw_cmd->ncr = 0xff;
    uint32_t arg_s = __builtin_bswap32(arg);
    memcpy(hw_cmd->arguments, &arg_s, sizeof(arg_s));
    hw_cmd->crc7 = sdspi_msg_crc7(hw_cmd);
    hw_cmd->timeout_ms = timeout_ms;
}

esp_err_t sdspi_host_do_transaction(int slot, sdmmc_command_t *cmdinfo)
{
    _lock_acquire(&s_lock);
    // Convert the command to wire format
    sdspi_hw_cmd_t hw_cmd;
    make_hw_cmd(cmdinfo->opcode, cmdinfo->arg, cmdinfo->timeout_ms, &hw_cmd);

    // Flags indicate which of the transfer types should be used
    int flags = 0;
    if (SCF_CMD(cmdinfo->flags) == SCF_CMD_ADTC) {
        flags = SDSPI_CMD_FLAG_DATA | SDSPI_CMD_FLAG_WRITE;
    } else if (SCF_CMD(cmdinfo->flags) == (SCF_CMD_ADTC | SCF_CMD_READ)) {
        flags = SDSPI_CMD_FLAG_DATA;
    }

    // In SD host, response format is encoded using SCF_RSP_* flags which come
    // as part of sdmmc_command_t from the upper layer (sdmmc_cmd.c).
    // SPI mode uses different command formats. In fact, most of the commands
    // use R1 response. Therefore, instead of adding another parallel set of
    // response flags for the SPI mode, response format is determined here:
    if (!s_app_cmd && cmdinfo->opcode == SD_SEND_IF_COND) {
        flags |= SDSPI_CMD_FLAG_RSP_R7;
    } else if (!s_app_cmd && cmdinfo->opcode == MMC_SEND_STATUS) {
        flags |= SDSPI_CMD_FLAG_RSP_R2;
    } else if (!s_app_cmd && cmdinfo->opcode == SD_READ_OCR) {
        flags |= SDSPI_CMD_FLAG_RSP_R3;
    } else if (s_app_cmd && cmdinfo->opcode == SD_APP_SD_STATUS) {
        flags |= SDSPI_CMD_FLAG_RSP_R2;
    } else {
        flags |= SDSPI_CMD_FLAG_RSP_R1;
    }

    // Send the command and get the response.
    esp_err_t ret = sdspi_host_start_command(slot, &hw_cmd,
            cmdinfo->data, cmdinfo->datalen, flags);

    // Extract response bytes and store them into cmdinfo structure
    if (ret == ESP_OK) {
        ESP_LOGV(TAG, "r1 = 0x%02x hw_cmd.r[0]=0x%08x", hw_cmd.r1, hw_cmd.response[0]);
        // Some errors should be reported using return code
        if (flags & SDSPI_CMD_FLAG_RSP_R1) {
            cmdinfo->response[0] = hw_cmd.r1;
            if (hw_cmd.r1 == 0xff) {
                // No response received at all
            } else if (hw_cmd.r1 & SD_SPI_R1_CMD_CRC_ERR) {
                ret = ESP_ERR_INVALID_CRC;
            } else if (hw_cmd.r1 & SD_SPI_R1_IDLE_STATE) {
                // Idle state is handled at command layer
            } else if (hw_cmd.r1 != 0) {
                ESP_LOGD(TAG, "Unexpected R1 response: 0x%02x", hw_cmd.r1);
            }
        } else if (flags & SDSPI_CMD_FLAG_RSP_R2) {
            cmdinfo->response[0] = (((uint32_t)hw_cmd.r1) << 8) | (hw_cmd.response[0] >> 24);
        } else if (flags & (SDSPI_CMD_FLAG_RSP_R3 | SDSPI_CMD_FLAG_RSP_R7)) {
            // Drop r1 response, only copy the other 4 bytes of data
            // TODO: can we somehow preserve r1 response and keep upper layer
            // same as in SD mode?
            cmdinfo->response[0] = __builtin_bswap32(hw_cmd.response[0]);
        }
    }

    // Save a flag whether the next command is expected to be an app command
    if (ret == ESP_OK) {
        s_app_cmd = (cmdinfo->opcode == MMC_APP_CMD);
    } else {
        s_app_cmd = false;
    }
    _lock_release(&s_lock);
    return ret;
}
