/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"
#include "soc/soc_caps.h"
#include "driver/sd_host.h"
#include "esp_private/sd_host_private.h"

typedef enum {
    SDMMC_IDLE,
    SDMMC_SENDING_CMD,
    SDMMC_SENDING_DATA,
    SDMMC_BUSY,
    SDMMC_SENDING_VOLTAGE_SWITCH,
    SDMMC_WAITING_VOLTAGE_SWITCH,
} sdmmc_req_state_t;

static const char *TAG = "SD_TRANS";

const uint32_t SD_DATA_ERR_MASK =
    SDMMC_INTMASK_DTO | SDMMC_INTMASK_DCRC |
    SDMMC_INTMASK_HTO | SDMMC_INTMASK_SBE  |
    SDMMC_INTMASK_EBE;

const uint32_t SD_DMA_DONE_MASK =
    SDMMC_IDMAC_INTMASK_RI | SDMMC_IDMAC_INTMASK_TI |
    SDMMC_IDMAC_INTMASK_NI;

const uint32_t SD_CMD_ERR_MASK =
    SDMMC_INTMASK_RTO |
    SDMMC_INTMASK_RCRC |
    SDMMC_INTMASK_RESP_ERR;

/*---------------------------------------------------------------
                    Voltage
---------------------------------------------------------------*/
static void handle_voltage_switch_stage1(sd_host_sdmmc_slot_t *slot, sdmmc_command_t* cmd)
{
    ESP_LOGV(TAG, "%s: enabling clock", __func__);
    sd_host_slot_set_cclk_always_on_internal(slot, true);
}

static void handle_voltage_switch_stage2(sd_host_sdmmc_slot_t *slot, sdmmc_command_t* cmd)
{
    ESP_LOGV(TAG, "%s: disabling clock", __func__);
    sd_host_slot_enable_clk_cmd11(slot, false);
    usleep(100);
    ESP_LOGV(TAG, "%s: switching voltage", __func__);
    esp_err_t err = cmd->volt_switch_cb(cmd->volt_switch_cb_arg, 1800);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to switch voltage (0x%x)", err);
        cmd->error = err;
    }
    ESP_LOGV(TAG, "%s: waiting 10ms", __func__);
    usleep(10000);
    ESP_LOGV(TAG, "%s: enabling clock", __func__);
    sd_host_slot_enable_clk_cmd11(slot, true);
}

static void handle_voltage_switch_stage3(sd_host_sdmmc_slot_t *slot, sdmmc_command_t* cmd)
{
    ESP_LOGV(TAG, "%s: voltage switch complete, clock back to low-power mode", __func__);
    sd_host_slot_set_cclk_always_on_internal(slot, false);
}

/*---------------------------------------------------------------
                    DMA
---------------------------------------------------------------*/
size_t sd_host_get_free_descriptors_count(sd_host_sdmmc_slot_t *slot)
{
    const size_t next = slot->ctlr->cur_transfer.next_desc;
    size_t count = 0;
    /* Starting with the current DMA descriptor, count the number of
     * descriptors which have 'owned_by_idmac' set to 0. These are the
     * descriptors already processed by the DMA engine.
     */
    int dma_desc_num = slot->ctlr->dma_desc_num;
    for (size_t i = 0; i < dma_desc_num; ++i) {
        sdmmc_desc_t *desc = slot->ctlr->dma_desc + ((next + i) % dma_desc_num);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_err_t ret = esp_cache_msync((void *)desc, sizeof(sdmmc_desc_t), ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        assert(ret == ESP_OK);
#endif
        if (desc->owned_by_idmac) {
            break;
        }
        ++count;
        if (desc->next_desc_ptr == NULL) {
            /* final descriptor in the chain */
            break;
        }
    }
    return count;
}

void sd_host_fill_dma_descriptors(sd_host_sdmmc_slot_t *slot, size_t num_desc)
{
    sd_host_sdmmc_trans_state_t *cur_trans = &slot->ctlr->cur_transfer;
    for (size_t i = 0; i < num_desc; ++i) {
        if (cur_trans->size_remaining == 0) {
            return;
        }
        const size_t next = cur_trans->next_desc;
        sdmmc_desc_t *desc = slot->ctlr->dma_desc + next;
        assert(!desc->owned_by_idmac);
        size_t size_to_fill =
            (cur_trans->size_remaining < SDMMC_DMA_MAX_BUF_LEN) ?
            cur_trans->size_remaining : SDMMC_DMA_MAX_BUF_LEN;
        bool last = size_to_fill == cur_trans->size_remaining;
        desc->last_descriptor = last;
        desc->second_address_chained = 1;
        desc->owned_by_idmac = 1;
        desc->buffer1_ptr = cur_trans->ptr;
        desc->next_desc_ptr = (last) ? NULL : slot->ctlr->dma_desc + ((next + 1) % num_desc);
        assert(size_to_fill < 4 || size_to_fill % 4 == 0);
        desc->buffer1_size = (size_to_fill + 3) & (~3);

        cur_trans->size_remaining -= size_to_fill;
        cur_trans->ptr += size_to_fill;
        cur_trans->next_desc = (cur_trans->next_desc + 1) % num_desc;
        ESP_LOGV(TAG, "fill %d desc=%d rem=%d next=%d last=%d sz=%d",
                 num_desc, next, cur_trans->size_remaining,
                 cur_trans->next_desc, desc->last_descriptor, desc->buffer1_size);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_err_t ret = esp_cache_msync((void *)desc, sizeof(sdmmc_desc_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert(ret == ESP_OK);
#endif
    }
}

void sd_host_dma_stop(sd_host_sdmmc_slot_t *slot)
{
    sdmmc_ll_stop_dma(slot->ctlr->hal.dev);
}

void sd_host_dma_resume(sd_host_sdmmc_slot_t *slot)
{
    sdmmc_ll_poll_demand(slot->ctlr->hal.dev);
}

void sd_host_dma_prepare(sd_host_sdmmc_slot_t *slot, void *data_ptr, size_t data_size, size_t block_size)
{
    // this clears "owned by IDMAC" bits
    memset(slot->ctlr->dma_desc, 0, sizeof(sdmmc_desc_t) * slot->ctlr->dma_desc_num);
    // initialize first descriptor
    sdmmc_desc_t *desc = slot->ctlr->dma_desc;
    desc->first_descriptor = 1;
    // save transfer info
    slot->ctlr->cur_transfer.ptr = data_ptr;
    slot->ctlr->cur_transfer.size_remaining = data_size;
    slot->ctlr->cur_transfer.next_desc = 0;
    slot->ctlr->cur_transfer.desc_remaining = (data_size + SDMMC_DMA_MAX_BUF_LEN - 1) / SDMMC_DMA_MAX_BUF_LEN;
    // prepare descriptors
    sd_host_fill_dma_descriptors(slot, slot->ctlr->dma_desc_num);

    // Set size of data and DMA descriptor pointer
    sdmmc_ll_set_data_transfer_len(slot->ctlr->hal.dev, data_size);
    sdmmc_ll_set_block_size(slot->ctlr->hal.dev, block_size);
    sdmmc_ll_set_desc_addr(slot->ctlr->hal.dev, (uint32_t)desc);
    // Enable everything needed to use DMA
    sdmmc_ll_enable_dma(slot->ctlr->hal.dev, true);
    sd_host_dma_resume(slot);
}

/*---------------------------------------------------------------
                    CMD
---------------------------------------------------------------*/
static bool cmd_needs_auto_stop(const sdmmc_command_t *cmd)
{
    /* SDMMC host needs an "auto stop" flag for the following commands: */
    return cmd->datalen > 0 &&
           (cmd->opcode == MMC_WRITE_BLOCK_MULTIPLE ||
            cmd->opcode == MMC_READ_BLOCK_MULTIPLE ||
            cmd->opcode == MMC_WRITE_DAT_UNTIL_STOP ||
            cmd->opcode == MMC_READ_DAT_UNTIL_STOP);
}

static sdmmc_hw_cmd_t make_hw_cmd(sdmmc_command_t *cmd)
{
    sdmmc_hw_cmd_t res = { 0 };

    res.cmd_index = cmd->opcode;
    if (cmd->opcode == MMC_STOP_TRANSMISSION) {
        res.stop_abort_cmd = 1;
    } else if (cmd->opcode == MMC_GO_IDLE_STATE) {
        res.send_init = 1;
    } else if (cmd->opcode == SD_SWITCH_VOLTAGE) {
        res.volt_switch = 1;
    } else {
        res.wait_complete = 1;
    }
    if (cmd->opcode == MMC_GO_IDLE_STATE) {
        res.send_init = 1;
    }

    if (cmd->flags & SCF_RSP_PRESENT) {
        res.response_expect = 1;
        if (cmd->flags & SCF_RSP_136) {
            res.response_long = 1;
        }
    }
    if (cmd->flags & SCF_RSP_CRC) {
        res.check_response_crc = 1;
    }
    if (cmd->data) {
        res.data_expected = 1;
        if ((cmd->flags & SCF_CMD_READ) == 0) {
            res.rw = 1;
        }
        assert(cmd->datalen % cmd->blklen == 0);
        res.send_auto_stop = cmd_needs_auto_stop(cmd) ? 1 : 0;
    }
    ESP_LOGV(TAG, "%s: opcode=%d, rexp=%d, crc=%d, auto_stop=%d", __func__,
             res.cmd_index, res.response_expect, res.check_response_crc,
             res.send_auto_stop);
    return res;
}

static inline bool mask_check_and_clear(uint32_t *state, uint32_t mask)
{
    bool ret = ((*state) & mask) != 0;
    *state &= ~mask;
    return ret;
}

static void process_command_response(sd_host_sdmmc_slot_t *slot, uint32_t status, sdmmc_command_t *cmd)
{
    if (cmd->flags & SCF_RSP_PRESENT) {
        if (cmd->flags & SCF_RSP_136) {
            /* Destination is 4-byte aligned, can memcopy from peripheral registers */
            memcpy(cmd->response, (uint32_t*) SDMMC.resp, 4 * sizeof(uint32_t));
        } else {
            cmd->response[0] = SDMMC.resp[0];
            cmd->response[1] = 0;
            cmd->response[2] = 0;
            cmd->response[3] = 0;
        }
    }
    esp_err_t err = ESP_OK;
    if (status & SDMMC_INTMASK_RTO) {
        // response timeout is only possible when response is expected
        assert(cmd->flags & SCF_RSP_PRESENT);
        err = ESP_ERR_TIMEOUT;
    } else if ((cmd->flags & SCF_RSP_CRC) && (status & SDMMC_INTMASK_RCRC)) {
        err = ESP_ERR_INVALID_CRC;
    } else if (status & SDMMC_INTMASK_RESP_ERR) {
        err = ESP_ERR_INVALID_RESPONSE;
    }
    if (err != ESP_OK) {
        cmd->error = err;
        if (cmd->data) {
            sd_host_dma_stop(slot);
        }
        ESP_LOGD(TAG, "%s: error 0x%x  (status=%08"PRIx32")", __func__, err, status);
    }
}

/*---------------------------------------------------------------
                    Events
---------------------------------------------------------------*/
static esp_err_t sd_host_wait_for_event(sd_host_sdmmc_slot_t *slot, int tick_count, sd_host_sdmmc_event_t *out_event)
{
    assert(out_event);
    assert(slot->ctlr->event_queue);

    int ret = xQueueReceive(slot->ctlr->event_queue, out_event, tick_count);
    if (ret == pdFALSE) {
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

static esp_err_t sd_host_handle_idle_state_events(sd_host_sdmmc_slot_t *slot)
{
    /* Handle any events which have happened in between transfers.
     * Under current assumptions (no SDIO support) only card detect events
     * can happen in the idle state.
     */
    sd_host_sdmmc_event_t evt;
    while (sd_host_wait_for_event(slot, 0, &evt) == ESP_OK) {
        if (evt.sdmmc_status & SDMMC_INTMASK_CD) {
            ESP_LOGV(TAG, "card detect event");
            evt.sdmmc_status &= ~SDMMC_INTMASK_CD;
        }
        if (evt.sdmmc_status != 0 || evt.dma_status != 0) {
            ESP_LOGE(TAG, "handle_idle_state_events unhandled: %08"PRIx32" %08"PRIx32,
                     evt.sdmmc_status, evt.dma_status);
        }

    }
    return ESP_OK;
}

static void process_data_status(sd_host_sdmmc_slot_t *slot, uint32_t status, sdmmc_command_t *cmd)
{
    if (status & SD_DATA_ERR_MASK) {
        if (status & SDMMC_INTMASK_DTO) {
            cmd->error = ESP_ERR_TIMEOUT;
        } else if (status & SDMMC_INTMASK_DCRC) {
            cmd->error = ESP_ERR_INVALID_CRC;
        } else if ((status & SDMMC_INTMASK_EBE) &&
                   (cmd->flags & SCF_CMD_READ) == 0) {
            cmd->error = ESP_ERR_TIMEOUT;
        } else {
            cmd->error = ESP_FAIL;
        }
        SDMMC.ctrl.fifo_reset = 1;
    }
    if (cmd->error != 0) {
        if (cmd->data) {
            sd_host_dma_stop(slot);
        }
        ESP_LOGD(TAG, "%s: error 0x%x (status=%08"PRIx32")", __func__, cmd->error, status);
    }

}

static esp_err_t process_events(sd_host_sdmmc_slot_t *slot, sd_host_sdmmc_event_t evt, sdmmc_command_t *cmd,
                                sdmmc_req_state_t *pstate, sd_host_sdmmc_event_t *unhandled_events)
{
    const char* const s_state_names[] __attribute__((unused)) = {
        "IDLE",
        "SENDING_CMD",
        "SENDIND_DATA",
        "BUSY",
        "SENDING_VOLTAGE_SWITCH",
        "WAITING_VOLTAGE_SWITCH",
    };
    sd_host_sdmmc_event_t orig_evt = evt;
    ESP_LOGV(TAG, "%s: slot_id=%d state=%s evt=%"PRIx32" dma=%"PRIx32, __func__, slot->slot_id,
             s_state_names[*pstate], evt.sdmmc_status, evt.dma_status);
    sdmmc_req_state_t next_state = *pstate;
    sdmmc_req_state_t state = (sdmmc_req_state_t) -1;
    while (next_state != state) {
        state = next_state;
        switch (state) {
        case SDMMC_IDLE:
            break;

        case SDMMC_SENDING_CMD:
            if (mask_check_and_clear(&evt.sdmmc_status, SD_CMD_ERR_MASK)) {
                process_command_response(slot, orig_evt.sdmmc_status, cmd);
                // In addition to the error interrupt, CMD_DONE will also be
                // reported. It may occur immediately (in the same sd_host_sdmmc_event_t) or
                // be delayed until the next interrupt.
            }
            if (mask_check_and_clear(&evt.sdmmc_status, SDMMC_INTMASK_CMD_DONE)) {
                process_command_response(slot, orig_evt.sdmmc_status, cmd);
                if (cmd->error != ESP_OK) {
                    next_state = SDMMC_IDLE;
                    break;
                }

                if (cmd->data == NULL) {
                    next_state = SDMMC_IDLE;
                } else {
                    next_state = SDMMC_SENDING_DATA;
                }
            }
            break;

        case SDMMC_SENDING_VOLTAGE_SWITCH:
            if (mask_check_and_clear(&evt.sdmmc_status, SD_CMD_ERR_MASK)) {
                process_command_response(slot, orig_evt.sdmmc_status, cmd);
                next_state = SDMMC_IDLE;
            }
            if (mask_check_and_clear(&evt.sdmmc_status, SDMMC_INTMASK_VOLT_SW)) {
                handle_voltage_switch_stage2(slot, cmd);
                if (cmd->error != ESP_OK) {
                    next_state = SDMMC_IDLE;
                } else {
                    next_state = SDMMC_WAITING_VOLTAGE_SWITCH;
                }
            }
            break;

        case SDMMC_WAITING_VOLTAGE_SWITCH:
            if (mask_check_and_clear(&evt.sdmmc_status, SD_CMD_ERR_MASK)) {
                process_command_response(slot, orig_evt.sdmmc_status, cmd);
                next_state = SDMMC_IDLE;
            }
            if (mask_check_and_clear(&evt.sdmmc_status, SDMMC_INTMASK_VOLT_SW)) {
                handle_voltage_switch_stage3(slot, cmd);
                next_state = SDMMC_IDLE;
            }
            break;

        case SDMMC_SENDING_DATA:
            if (mask_check_and_clear(&evt.sdmmc_status, SD_DATA_ERR_MASK)) {
                process_data_status(slot, orig_evt.sdmmc_status, cmd);
                sd_host_dma_stop(slot);
            }
            if (mask_check_and_clear(&evt.dma_status, SD_DMA_DONE_MASK)) {
                next_state = SDMMC_BUSY;
            }
            if (orig_evt.sdmmc_status & (SDMMC_INTMASK_SBE | SDMMC_INTMASK_DATA_OVER)) {
                // On start bit error, DATA_DONE interrupt will not be generated
                next_state = SDMMC_IDLE;
                break;
            }
            break;

        case SDMMC_BUSY:
            if (!mask_check_and_clear(&evt.sdmmc_status, SDMMC_INTMASK_DATA_OVER)) {
                break;
            }
            process_data_status(slot, orig_evt.sdmmc_status, cmd);
            next_state = SDMMC_IDLE;
            break;
        }
        ESP_LOGV(TAG, "%s state=%s next_state=%s", __func__, s_state_names[state], s_state_names[next_state]);
    }
    *pstate = state;
    *unhandled_events = evt;
    return ESP_OK;
}

static esp_err_t handle_event(sd_host_sdmmc_slot_t *slot, sdmmc_command_t* cmd, sdmmc_req_state_t *state,
                              sd_host_sdmmc_event_t *unhandled_events)
{
    sd_host_sdmmc_event_t event;
    esp_err_t err = sd_host_wait_for_event(slot, cmd->timeout_ms / portTICK_PERIOD_MS, &event);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "sd_host_wait_for_event returned 0x%x", err);
        if (err == ESP_ERR_TIMEOUT) {
            sd_host_dma_stop(slot);
        }
        return err;
    }
    ESP_LOGV(TAG, "sdmmc_handle_event: slot_id %d event %08"PRIx32" %08"PRIx32", unhandled %08"PRIx32" %08"PRIx32,
             slot->slot_id, event.sdmmc_status, event.dma_status,
             unhandled_events->sdmmc_status, unhandled_events->dma_status);
    event.sdmmc_status |= unhandled_events->sdmmc_status;
    event.dma_status |= unhandled_events->dma_status;
    process_events(slot, event, cmd, state, unhandled_events);
    ESP_LOGV(TAG, "sdmmc_handle_event: slot_id %d events unhandled: %08"PRIx32" %08"PRIx32, slot->slot_id, unhandled_events->sdmmc_status, unhandled_events->dma_status);
    return ESP_OK;
}

static bool sd_host_card_busy(sd_host_sdmmc_slot_t *slot)
{
    return sdmmc_ll_is_card_data_busy(slot->ctlr->hal.dev);
}

static bool wait_for_busy_cleared(sd_host_sdmmc_slot_t *slot, uint32_t timeout_ms)
{
    if (timeout_ms == 0) {
        return !sd_host_card_busy(slot);
    }

    /* It would have been nice to do this without polling, however the peripheral
     * can only generate Busy Clear Interrupt for data write commands, and waiting
     * for busy clear is mostly needed for other commands such as MMC_SWITCH.
     */
    uint32_t timeout_ticks = (timeout_ms + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS;
    while (timeout_ticks-- > 0) {
        if (!sd_host_card_busy(slot)) {
            return true;
        }
        vTaskDelay(1);
    }
    return false;
}

/*---------------------------------------------------------------
                    Public API Impl
---------------------------------------------------------------*/
esp_err_t sd_host_slot_sdmmc_do_transaction(sd_host_slot_handle_t slot, sdmmc_command_t *cmdinfo)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(slot && cmdinfo, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    sd_host_sdmmc_slot_t *slot_ctx = __containerof(slot, sd_host_sdmmc_slot_t, drv);

    xSemaphoreTake(slot_ctx->ctlr->mutex, portMAX_DELAY);
#if CONFIG_PM_ENABLE
    if (slot_ctx->ctlr->pm_lock) {
        ESP_GOTO_ON_ERROR(esp_pm_lock_acquire(slot_ctx->ctlr->pm_lock), out, TAG, "acquire pm_lock failed");
    }
#endif
    slot_ctx->ctlr->cur_slot_id = slot_ctx->slot_id;

    // By default, set probing frequency (400kHz) and 1-bit bus
    if (slot_ctx->freq.freq_state != SD_HOST_SLOT_STATE_READY) {
        ESP_GOTO_ON_ERROR(sd_host_slot_set_card_clk(slot_ctx), out, TAG, "failed to set clk");
    } else {
        int real_freq_khz = 0;
        ret = sd_host_slot_get_real_freq(slot_ctx, &real_freq_khz);
        assert(ret == ESP_OK);
        if (real_freq_khz != (slot_ctx->freq.freq_hz / 1000)) {
            ESP_GOTO_ON_ERROR(sd_host_slot_set_card_clk(slot_ctx), out, TAG, "failed to set clk");
        }
    }
    ESP_GOTO_ON_ERROR(sd_host_slot_set_bus_width(slot_ctx), out, TAG, "failed to set width");
    ESP_GOTO_ON_ERROR(sd_host_slot_set_bus_sampling_mode(slot_ctx), out, TAG, "failed to set sampling mode");
    ESP_GOTO_ON_ERROR(sd_host_set_delay_phase(slot_ctx), out, TAG, "failed to set delay phase");
    ESP_GOTO_ON_ERROR(sd_host_set_delay_line(slot_ctx), out, TAG, "failed to set delay line");

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    // cache sync related
    size_t cache_sync_len = 0;
#endif

    // dispose of any events which happened asynchronously
    sd_host_handle_idle_state_events(slot_ctx);

    // special handling for voltage switch command
    if (cmdinfo->opcode == SD_SWITCH_VOLTAGE) {
        handle_voltage_switch_stage1(slot_ctx, cmdinfo);
    }

    // convert cmdinfo to hardware register value
    sdmmc_hw_cmd_t hw_cmd = make_hw_cmd(cmdinfo);
    if (cmdinfo->data) {
        // Length should be either <4 or >=4 and =0 (mod 4).
        if (cmdinfo->datalen >= 4 && cmdinfo->datalen % 4 != 0) {
            ESP_LOGE(TAG, "%s: invalid size: total=%d",
                     __func__, cmdinfo->datalen);
            ret = ESP_ERR_INVALID_SIZE;
            goto out;
        }

        bool is_aligned = sd_host_check_buffer_alignment(slot_ctx, cmdinfo->data, cmdinfo->buflen);
        if (!is_aligned) {
            ESP_LOGE(TAG, "%s: buffer %p can not be used for DMA", __func__, cmdinfo->data);
            ret = ESP_ERR_INVALID_ARG;
            goto out;
        }

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        cache_sync_len = cmdinfo->buflen;
        ret = esp_cache_msync((void *)cmdinfo->data, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        if (ret != ESP_OK) {
            goto out;
        }
#endif
        // write transfer info into hardware
        sd_host_dma_prepare(slot_ctx, cmdinfo->data, cmdinfo->datalen, cmdinfo->blklen);
    }
    // write command into hardware, this also sends the command to the card
    ret = sd_host_slot_start_command(slot_ctx, hw_cmd, cmdinfo->arg);
    if (ret != ESP_OK) {
        goto out;
    }
    // process events until transfer is complete
    cmdinfo->error = ESP_OK;
    sdmmc_req_state_t state = SDMMC_SENDING_CMD;
    if (cmdinfo->opcode == SD_SWITCH_VOLTAGE) {
        state = SDMMC_SENDING_VOLTAGE_SWITCH;
    }
    sd_host_sdmmc_event_t unhandled_events = {};
    while (state != SDMMC_IDLE) {
        ret = handle_event(slot_ctx, cmdinfo, &state, &unhandled_events);
        if (ret != ESP_OK) {
            break;
        }
    }
    if (ret == ESP_OK && (cmdinfo->flags & SCF_WAIT_BUSY)) {
        if (!wait_for_busy_cleared(slot_ctx, cmdinfo->timeout_ms)) {
            ret = ESP_ERR_TIMEOUT;
        }
    }
    slot_ctx->ctlr->is_app_cmd = (ret == ESP_OK && cmdinfo->opcode == MMC_APP_CMD);

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    if (cmdinfo->data) {
        ret = esp_cache_msync((void *)cmdinfo->data, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        if (ret != ESP_OK) {
            goto out;
        }
    }
#endif

out:
#if CONFIG_PM_ENABLE
    if (slot_ctx->ctlr->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(slot_ctx->ctlr->pm_lock), TAG, "release pm_lock failed");
    }
#endif
    xSemaphoreGive(slot_ctx->ctlr->mutex);

    return ret;
}
