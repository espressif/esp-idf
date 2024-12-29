/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <unistd.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "soc/sdmmc_periph.h"
#include "driver/sdmmc_types.h"
#include "driver/sdmmc_defs.h"
#include "driver/sdmmc_host.h"
#include "esp_cache.h"
#include "esp_private/esp_cache_private.h"
#include "sdmmc_internal.h"
#include "soc/soc_caps.h"
#include "hal/sdmmc_ll.h"

/* Number of DMA descriptors used for transfer.
 * Increasing this value above 4 doesn't improve performance for the usual case
 * of SD memory cards (most data transfers are multiples of 512 bytes).
 */
#define SDMMC_DMA_DESC_CNT  4

#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))

static const char* TAG = "sdmmc_req";

typedef enum {
    SDMMC_IDLE,
    SDMMC_SENDING_CMD,
    SDMMC_SENDING_DATA,
    SDMMC_BUSY,
    SDMMC_SENDING_VOLTAGE_SWITCH,
    SDMMC_WAITING_VOLTAGE_SWITCH,
} sdmmc_req_state_t;

typedef struct {
    uint8_t* ptr;
    size_t size_remaining;
    size_t next_desc;
    size_t desc_remaining;
} sdmmc_transfer_state_t;

const uint32_t SDMMC_DATA_ERR_MASK =
    SDMMC_INTMASK_DTO | SDMMC_INTMASK_DCRC |
    SDMMC_INTMASK_HTO | SDMMC_INTMASK_SBE  |
    SDMMC_INTMASK_EBE;

const uint32_t SDMMC_DMA_DONE_MASK =
    SDMMC_IDMAC_INTMASK_RI | SDMMC_IDMAC_INTMASK_TI |
    SDMMC_IDMAC_INTMASK_NI;

const uint32_t SDMMC_CMD_ERR_MASK =
    SDMMC_INTMASK_RTO |
    SDMMC_INTMASK_RCRC |
    SDMMC_INTMASK_RESP_ERR;

DRAM_DMA_ALIGNED_ATTR static sdmmc_desc_t s_dma_desc[SDMMC_DMA_DESC_CNT];
static sdmmc_transfer_state_t s_cur_transfer = { 0 };
static QueueHandle_t s_request_mutex;
static bool s_is_app_cmd;   // This flag is set if the next command is an APP command
#ifdef CONFIG_PM_ENABLE
static esp_pm_lock_handle_t s_pm_lock;
#endif

static esp_err_t handle_idle_state_events(void);
static sdmmc_hw_cmd_t make_hw_cmd(sdmmc_command_t* cmd);
static esp_err_t handle_event(int slot, sdmmc_command_t* cmd, sdmmc_req_state_t* state,
                              sdmmc_event_t* unhandled_events);
static esp_err_t process_events(int slot, sdmmc_event_t evt, sdmmc_command_t* cmd,
                                sdmmc_req_state_t* pstate, sdmmc_event_t* unhandled_events);
static void process_command_response(uint32_t status, sdmmc_command_t* cmd);
static void fill_dma_descriptors(size_t num_desc);
static size_t get_free_descriptors_count(void);
static bool wait_for_busy_cleared(uint32_t timeout_ms);
static void handle_voltage_switch_stage1(int slot, sdmmc_command_t* cmd);
static void handle_voltage_switch_stage2(int slot, sdmmc_command_t* cmd);
static void handle_voltage_switch_stage3(int slot, sdmmc_command_t* cmd);

esp_err_t sdmmc_host_transaction_handler_init(void)
{
    assert(s_request_mutex == NULL);
    s_request_mutex = xSemaphoreCreateMutex();
    if (!s_request_mutex) {
        return ESP_ERR_NO_MEM;
    }
    s_is_app_cmd = false;
#ifdef CONFIG_PM_ENABLE
    esp_err_t err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "sdmmc", &s_pm_lock);
    if (err != ESP_OK) {
        vSemaphoreDelete(s_request_mutex);
        s_request_mutex = NULL;
        return err;
    }
#endif
    return ESP_OK;
}

void sdmmc_host_transaction_handler_deinit(void)
{
    assert(s_request_mutex);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_delete(s_pm_lock);
    s_pm_lock = NULL;
#endif
    vSemaphoreDelete(s_request_mutex);
    s_request_mutex = NULL;
}

esp_err_t sdmmc_host_do_transaction(int slot, sdmmc_command_t* cmdinfo)
{
    esp_err_t ret;
    xSemaphoreTake(s_request_mutex, portMAX_DELAY);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(s_pm_lock);
#endif

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    // cache sync related
    size_t cache_sync_len = 0;
#endif

    // dispose of any events which happened asynchronously
    handle_idle_state_events();

    // special handling for voltage switch command
    if (cmdinfo->opcode == SD_SWITCH_VOLTAGE) {
        handle_voltage_switch_stage1(slot, cmdinfo);
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
        esp_dma_mem_info_t dma_mem_info;
        sdmmc_host_get_dma_info(slot, &dma_mem_info);
#ifdef SOC_SDMMC_PSRAM_DMA_CAPABLE
        dma_mem_info.extra_heap_caps |= MALLOC_CAP_SPIRAM;
#endif
        if (!esp_dma_is_buffer_alignment_satisfied(cmdinfo->data, cmdinfo->buflen, dma_mem_info)) {
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
        // this clears "owned by IDMAC" bits
        memset(s_dma_desc, 0, sizeof(s_dma_desc));
        // initialize first descriptor
        s_dma_desc[0].first_descriptor = 1;
        // save transfer info
        s_cur_transfer.ptr = (uint8_t*) cmdinfo->data;
        s_cur_transfer.size_remaining = cmdinfo->datalen;
        s_cur_transfer.next_desc = 0;
        s_cur_transfer.desc_remaining = (cmdinfo->datalen + SDMMC_DMA_MAX_BUF_LEN - 1) / SDMMC_DMA_MAX_BUF_LEN;
        // prepare descriptors
        fill_dma_descriptors(SDMMC_DMA_DESC_CNT);
        // write transfer info into hardware
        sdmmc_host_dma_prepare(&s_dma_desc[0], cmdinfo->blklen, cmdinfo->datalen);
    }
    // write command into hardware, this also sends the command to the card
    ret = sdmmc_host_start_command(slot, hw_cmd, cmdinfo->arg);
    if (ret != ESP_OK) {
        goto out;
    }
    // process events until transfer is complete
    cmdinfo->error = ESP_OK;
    sdmmc_req_state_t state = SDMMC_SENDING_CMD;
    if (cmdinfo->opcode == SD_SWITCH_VOLTAGE) {
        state = SDMMC_SENDING_VOLTAGE_SWITCH;
    }
    sdmmc_event_t unhandled_events = { 0 };
    while (state != SDMMC_IDLE) {
        ret = handle_event(slot, cmdinfo, &state, &unhandled_events);
        if (ret != ESP_OK) {
            break;
        }
    }
    if (ret == ESP_OK && (cmdinfo->flags & SCF_WAIT_BUSY)) {
        if (!wait_for_busy_cleared(cmdinfo->timeout_ms)) {
            ret = ESP_ERR_TIMEOUT;
        }
    }
    s_is_app_cmd = (ret == ESP_OK && cmdinfo->opcode == MMC_APP_CMD);

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    if (cmdinfo->data) {
        ret = esp_cache_msync((void *)cmdinfo->data, cache_sync_len, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        if (ret != ESP_OK) {
            goto out;
        }
    }
#endif

out:
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(s_pm_lock);
#endif
    xSemaphoreGive(s_request_mutex);
    return ret;
}

static size_t get_free_descriptors_count(void)
{
    const size_t next = s_cur_transfer.next_desc;
    size_t count = 0;
    /* Starting with the current DMA descriptor, count the number of
     * descriptors which have 'owned_by_idmac' set to 0. These are the
     * descriptors already processed by the DMA engine.
     */
    for (size_t i = 0; i < SDMMC_DMA_DESC_CNT; ++i) {
        sdmmc_desc_t* desc = &s_dma_desc[(next + i) % SDMMC_DMA_DESC_CNT];
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

static void fill_dma_descriptors(size_t num_desc)
{
    for (size_t i = 0; i < num_desc; ++i) {
        if (s_cur_transfer.size_remaining == 0) {
            return;
        }
        const size_t next = s_cur_transfer.next_desc;
        sdmmc_desc_t* desc = &s_dma_desc[next];
        assert(!desc->owned_by_idmac);
        size_t size_to_fill =
            (s_cur_transfer.size_remaining < SDMMC_DMA_MAX_BUF_LEN) ?
            s_cur_transfer.size_remaining : SDMMC_DMA_MAX_BUF_LEN;
        bool last = size_to_fill == s_cur_transfer.size_remaining;
        desc->last_descriptor = last;
        desc->second_address_chained = 1;
        desc->owned_by_idmac = 1;
        desc->buffer1_ptr = s_cur_transfer.ptr;
        desc->next_desc_ptr = (last) ? NULL : &s_dma_desc[(next + 1) % SDMMC_DMA_DESC_CNT];
        assert(size_to_fill < 4 || size_to_fill % 4 == 0);
        desc->buffer1_size = (size_to_fill + 3) & (~3);

        s_cur_transfer.size_remaining -= size_to_fill;
        s_cur_transfer.ptr += size_to_fill;
        s_cur_transfer.next_desc = (s_cur_transfer.next_desc + 1) % SDMMC_DMA_DESC_CNT;
        ESP_LOGV(TAG, "fill %d desc=%d rem=%d next=%d last=%d sz=%d",
                 num_desc, next, s_cur_transfer.size_remaining,
                 s_cur_transfer.next_desc, desc->last_descriptor, desc->buffer1_size);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
        esp_err_t ret = esp_cache_msync((void *)desc, sizeof(sdmmc_desc_t), ESP_CACHE_MSYNC_FLAG_DIR_C2M);
        assert(ret == ESP_OK);
#endif
    }
}

static esp_err_t handle_idle_state_events(void)
{
    /* Handle any events which have happened in between transfers.
     * Under current assumptions (no SDIO support) only card detect events
     * can happen in the idle state.
     */
    sdmmc_event_t evt;
    while (sdmmc_host_wait_for_event(0, &evt) == ESP_OK) {
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

static esp_err_t handle_event(int slot, sdmmc_command_t* cmd, sdmmc_req_state_t* state,
                              sdmmc_event_t* unhandled_events)
{
    sdmmc_event_t event;
    esp_err_t err = sdmmc_host_wait_for_event(cmd->timeout_ms / portTICK_PERIOD_MS, &event);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "sdmmc_host_wait_for_event returned 0x%x", err);
        if (err == ESP_ERR_TIMEOUT) {
            sdmmc_host_dma_stop();
        }
        return err;
    }
    ESP_LOGV(TAG, "sdmmc_handle_event: slot %d event %08"PRIx32" %08"PRIx32", unhandled %08"PRIx32" %08"PRIx32,
             slot, event.sdmmc_status, event.dma_status,
             unhandled_events->sdmmc_status, unhandled_events->dma_status);
    event.sdmmc_status |= unhandled_events->sdmmc_status;
    event.dma_status |= unhandled_events->dma_status;
    process_events(slot, event, cmd, state, unhandled_events);
    ESP_LOGV(TAG, "sdmmc_handle_event: slot %d events unhandled: %08"PRIx32" %08"PRIx32, slot, unhandled_events->sdmmc_status, unhandled_events->dma_status);
    return ESP_OK;
}

static bool cmd_needs_auto_stop(const sdmmc_command_t* cmd)
{
    /* SDMMC host needs an "auto stop" flag for the following commands: */
    return cmd->datalen > 0 &&
           (cmd->opcode == MMC_WRITE_BLOCK_MULTIPLE ||
            cmd->opcode == MMC_READ_BLOCK_MULTIPLE ||
            cmd->opcode == MMC_WRITE_DAT_UNTIL_STOP ||
            cmd->opcode == MMC_READ_DAT_UNTIL_STOP);
}

static sdmmc_hw_cmd_t make_hw_cmd(sdmmc_command_t* cmd)
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

static void process_command_response(uint32_t status, sdmmc_command_t* cmd)
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
            sdmmc_host_dma_stop();
        }
        ESP_LOGD(TAG, "%s: error 0x%x  (status=%08"PRIx32")", __func__, err, status);
    }
}

static void process_data_status(uint32_t status, sdmmc_command_t* cmd)
{
    if (status & SDMMC_DATA_ERR_MASK) {
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
            sdmmc_host_dma_stop();
        }
        ESP_LOGD(TAG, "%s: error 0x%x (status=%08"PRIx32")", __func__, cmd->error, status);
    }

}

static inline bool mask_check_and_clear(uint32_t* state, uint32_t mask)
{
    bool ret = ((*state) & mask) != 0;
    *state &= ~mask;
    return ret;
}

static esp_err_t process_events(int slot, sdmmc_event_t evt, sdmmc_command_t* cmd,
                                sdmmc_req_state_t* pstate, sdmmc_event_t* unhandled_events)
{
    const char* const s_state_names[] __attribute__((unused)) = {
        "IDLE",
        "SENDING_CMD",
        "SENDIND_DATA",
        "BUSY",
        "SENDING_VOLTAGE_SWITCH",
        "WAITING_VOLTAGE_SWITCH",
    };
    sdmmc_event_t orig_evt = evt;
    ESP_LOGV(TAG, "%s: slot=%d state=%s evt=%"PRIx32" dma=%"PRIx32, __func__, slot,
             s_state_names[*pstate], evt.sdmmc_status, evt.dma_status);
    sdmmc_req_state_t next_state = *pstate;
    sdmmc_req_state_t state = (sdmmc_req_state_t) -1;
    while (next_state != state) {
        state = next_state;
        switch (state) {
        case SDMMC_IDLE:
            break;

        case SDMMC_SENDING_CMD:
            if (mask_check_and_clear(&evt.sdmmc_status, SDMMC_CMD_ERR_MASK)) {
                process_command_response(orig_evt.sdmmc_status, cmd);
                // In addition to the error interrupt, CMD_DONE will also be
                // reported. It may occur immediately (in the same sdmmc_event_t) or
                // be delayed until the next interrupt.
            }
            if (mask_check_and_clear(&evt.sdmmc_status, SDMMC_INTMASK_CMD_DONE)) {
                process_command_response(orig_evt.sdmmc_status, cmd);
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
            if (mask_check_and_clear(&evt.sdmmc_status, SDMMC_CMD_ERR_MASK)) {
                process_command_response(orig_evt.sdmmc_status, cmd);
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
            if (mask_check_and_clear(&evt.sdmmc_status, SDMMC_CMD_ERR_MASK)) {
                process_command_response(orig_evt.sdmmc_status, cmd);
                next_state = SDMMC_IDLE;
            }
            if (mask_check_and_clear(&evt.sdmmc_status, SDMMC_INTMASK_VOLT_SW)) {
                handle_voltage_switch_stage3(slot, cmd);
                next_state = SDMMC_IDLE;
            }
            break;

        case SDMMC_SENDING_DATA:
            if (mask_check_and_clear(&evt.sdmmc_status, SDMMC_DATA_ERR_MASK)) {
                process_data_status(orig_evt.sdmmc_status, cmd);
                sdmmc_host_dma_stop();
            }
            if (mask_check_and_clear(&evt.dma_status, SDMMC_DMA_DONE_MASK)) {
                s_cur_transfer.desc_remaining--;
                if (s_cur_transfer.size_remaining) {
                    int desc_to_fill = get_free_descriptors_count();
                    fill_dma_descriptors(desc_to_fill);
                    sdmmc_host_dma_resume();
                }
                if (s_cur_transfer.desc_remaining == 0) {
                    next_state = SDMMC_BUSY;
                }
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
            process_data_status(orig_evt.sdmmc_status, cmd);
            next_state = SDMMC_IDLE;
            break;
        }
        ESP_LOGV(TAG, "%s state=%s next_state=%s", __func__, s_state_names[state], s_state_names[next_state]);
    }
    *pstate = state;
    *unhandled_events = evt;
    return ESP_OK;
}

static bool wait_for_busy_cleared(uint32_t timeout_ms)
{
    if (timeout_ms == 0) {
        return !sdmmc_host_card_busy();
    }

    /* It would have been nice to do this without polling, however the peripheral
     * can only generate Busy Clear Interrupt for data write commands, and waiting
     * for busy clear is mostly needed for other commands such as MMC_SWITCH.
     */
    uint32_t timeout_ticks = (timeout_ms + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS;
    while (timeout_ticks-- > 0) {
        if (!sdmmc_host_card_busy()) {
            return true;
        }
        vTaskDelay(1);
    }
    return false;
}

static void handle_voltage_switch_stage1(int slot, sdmmc_command_t* cmd)
{
    ESP_LOGV(TAG, "%s: enabling clock", __func__);
    sdmmc_host_set_cclk_always_on(slot, true);
}

static void handle_voltage_switch_stage2(int slot, sdmmc_command_t* cmd)
{
    ESP_LOGV(TAG, "%s: disabling clock", __func__);
    sdmmc_host_enable_clk_cmd11(slot, false);
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
    sdmmc_host_enable_clk_cmd11(slot, true);
}

static void handle_voltage_switch_stage3(int slot, sdmmc_command_t* cmd)
{
    ESP_LOGV(TAG, "%s: voltage switch complete, clock back to low-power mode", __func__);
    sdmmc_host_set_cclk_always_on(slot, false);
}
