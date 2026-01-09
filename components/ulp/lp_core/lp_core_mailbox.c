/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_err.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "hal/misc.h"
#include "lp_core_mailbox.h"
#include "ulp_lp_core_mailbox_impl_shared.h"
#include "riscv/rv_utils.h"
#include "soc/interrupts.h"

#ifndef BIT
#define BIT(i)  (1U << (i))
#endif

/**
 * @brief Starting index of the LP -> HP messages
 */
#define LP_MAILBOX_RX_MSG_IDX    (0U)

/**
 * @brief Number of messages allocated for LP -> HP communication
 */
#define LP_MAILBOX_RX_MSG_COUNT  (LP_MAILBOX_MSG_COUNT / 2)

/**
 * @brief Mask for the LP -> HP messages
 */
#define LP_MAILBOX_RX_MSG_MASK   (((1U << LP_MAILBOX_RX_MSG_COUNT) - 1U) << LP_MAILBOX_RX_MSG_IDX)

/**
 * @brief Number of messages allocated for HP -> LP communication
 */
#define LP_MAILBOX_TX_MSG_COUNT  (LP_MAILBOX_MSG_COUNT / 2)

/**
 * @brief Starting index of the HP -> LP messages
 */
#define LP_MAILBOX_TX_MSG_IDX    (8U)

#define LP_MAILBOX_ACK        ((lp_message_t) 0xe5U)

struct lp_mailbox_t {
    lp_core_mailbox_ctx_t      mb_ctx;
    SemaphoreHandle_t          mtx;
    SemaphoreHandle_t          received_sem;
    intr_handle_t              intr_handle;
    lp_core_mailbox_callback_t rcv_callback;
    uint32_t                   rcv_remaining;
    int                        tx_idx;
};

static struct lp_mailbox_t s_mailbox = { 0 };

/******************************************************************************
 *                              STATIC HELPERS                                *
 ******************************************************************************/

static inline int lp_core_next_msg_idx(int idx)
{
    return (idx + 1) % LP_MAILBOX_TX_MSG_COUNT;
}

static inline bool mb_msg_status(lp_mailbox_t mailbox, int msg_idx)
{
    return lp_core_mailbox_impl_intr_raw(mailbox->mb_ctx) & BIT(msg_idx);
}

static inline bool mb_msg_search_read(lp_mailbox_t mailbox, int* msg_idx, lp_message_t* msg)
{
    for (int i = LP_MAILBOX_RX_MSG_IDX; i < (LP_MAILBOX_RX_MSG_IDX + LP_MAILBOX_RX_MSG_COUNT); i += 2) {
        if (mb_msg_status(mailbox, i)) {
            *msg_idx = i;
            *msg = lp_core_mailbox_impl_get_message(mailbox->mb_ctx, i);
            return true;
        }
    }
    return false;
}

static inline bool mb_async_mode(lp_mailbox_t mailbox)
{
    return mailbox->rcv_callback != NULL;
}

static void lp_core_mailbox_intr_handler(void* arg)
{
    lp_mailbox_t mailbox = (lp_mailbox_t) arg;
    BaseType_t task_awoken = pdFALSE;

    /* If we are not in asynchronous mode, simply notify the task that an interrupt arrived */
    if (!mb_async_mode(mailbox)) {
        /* Disable the interrupts for all the messages (do NOT clear them) */
        lp_core_mailbox_impl_intr_disable(mailbox->mb_ctx, ~0);
        xSemaphoreGiveFromISR(mailbox->received_sem, &task_awoken);
        if (task_awoken != pdFALSE) {
            portYIELD_FROM_ISR();
        }
    } else {
        lp_message_t received[LP_MAILBOX_RX_MSG_COUNT / 2];
        int received_count = 0;

        /* Acknowledge all the received message as fast as possible */
        const uint32_t status = lp_core_mailbox_impl_intr_status(mailbox->mb_ctx);
        /* Mask of all the processed messages */
        uint32_t clr_mask = 0;

        /* Skip one message out of two since one is used for acknowledgement */
        for (int i = 0; i < LP_MAILBOX_RX_MSG_COUNT; i += 2) {
            const uint32_t mask = BIT(i);
            if (status & mask) {
                clr_mask |= mask;
                received[received_count] = lp_core_mailbox_impl_get_message(mailbox->mb_ctx, LP_MAILBOX_RX_MSG_IDX + i);
                /* Acknowledge reception by writing to the next message */
                const int ack_msg_idx = LP_MAILBOX_RX_MSG_IDX + lp_core_next_msg_idx(i);
                lp_core_mailbox_impl_set_message(mailbox->mb_ctx, ack_msg_idx, LP_MAILBOX_ACK);
                /* Clear ACK self-interrupt (writing ACK sets HP's own interrupt bit) */
                lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, BIT(ack_msg_idx));
                received_count++;
                mailbox->rcv_remaining--;
                if (mailbox->rcv_remaining == 0) {
                    break;
                }
            }
        }
        /* Clear the interrupt status */
        lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, clr_mask);

        /* Call the callback for all the message received */
        for (int i = 0; i < received_count; i++) {
            mailbox->rcv_callback(received[i]);
        }

        /* Disable interrupts if no more messages are expected */
        if (mailbox->rcv_remaining == 0) {
            lp_core_mailbox_impl_intr_disable(mailbox->mb_ctx, LP_MAILBOX_RX_MSG_MASK);
            mailbox->rcv_callback = NULL;
        }
    }
}

/******************************************************************************
 *                              PUBLIC API                                    *
 ******************************************************************************/

esp_err_t lp_core_mailbox_init(lp_mailbox_t *mailbox, lp_mailbox_config_t *config)
{
    esp_err_t ret;
    /* Configuration is unused for now */
    (void) config;

    if (mailbox == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    lp_core_mailbox_ctx_t context = lp_core_mailbox_impl_get_context();
    if (context == NULL) {
        /* Invalid state! LP core didn't initialize the spinlock? (SW backend) */
        return ESP_ERR_INVALID_STATE;
    }

    if (!rv_utils_compare_and_set((volatile uint32_t *) &s_mailbox.mb_ctx, 0, (uint32_t) context)) {
        /* Mailbox has already been initialized! */
        return ESP_ERR_INVALID_STATE;
    }
    s_mailbox.received_sem = xSemaphoreCreateBinary();
    if (s_mailbox.received_sem == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err_free_dev;
    }
    s_mailbox.mtx = xSemaphoreCreateMutex();
    if (s_mailbox.mtx == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err_free_sem;
    }
    /* Allocate an interrupt for the mailbox */
#if SOC_LP_MAILBOX_SUPPORTED
    ret = esp_intr_alloc(ETS_MB_HP_INTR_SOURCE, MALLOC_CAP_DEFAULT,
                         lp_core_mailbox_intr_handler, &s_mailbox,
                         &s_mailbox.intr_handle);
#else
    ret = esp_intr_alloc(ETS_PMU_INTR_SOURCE, MALLOC_CAP_DEFAULT,
                         lp_core_mailbox_intr_handler, &s_mailbox,
                         &s_mailbox.intr_handle);
#endif // SOC_LP_MAILBOX_SUPPORTED
    if (ret != ESP_OK) {
        goto err_free_all;
    }
    /* Disable interrupts but do not clear them, if the LP core sent a message before HP core
     * had time to initialize its mailbox, the interrupt signal would be lost. */
    lp_core_mailbox_impl_intr_disable(s_mailbox.mb_ctx, ~0);
    /* Enable the global HP mailbox interrupt (control interrupts via mailbox intr register) */
    ret = esp_intr_enable(s_mailbox.intr_handle);
    if (ret != ESP_OK) {
        goto err_free_all;
    }
    s_mailbox.tx_idx = 0;
    *mailbox = &s_mailbox;
    return ESP_OK;
err_free_all:
    vSemaphoreDelete(s_mailbox.mtx);
err_free_sem:
    vSemaphoreDelete(s_mailbox.received_sem);
err_free_dev:
    s_mailbox.mb_ctx = NULL;
    return ret;
}

void lp_core_mailbox_deinit(lp_mailbox_t mailbox)
{
    if (mailbox != NULL) {
        lp_core_mailbox_impl_intr_disable(mailbox->mb_ctx, ~0);
        lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, ~0);
        vSemaphoreDelete(mailbox->mtx);
        vSemaphoreDelete(mailbox->received_sem);
        esp_intr_free(mailbox->intr_handle);
        /* Mark the structure as deinitialized */
        s_mailbox.mb_ctx = NULL;
    }
}

esp_err_t lp_core_mailbox_send(lp_mailbox_t mailbox, lp_message_t msg, TickType_t ticks_to_wait)
{
    if (mailbox == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    xSemaphoreTake(mailbox->mtx, portMAX_DELAY);
    /* Make sure there isn't already an asynchronous transaction going on */
    if (mb_async_mode(mailbox)) {
        xSemaphoreGive(mailbox->mtx);
        return ESP_ERR_INVALID_STATE;
    }
    /* Get the address of the next free message */
    const int msg_idx = LP_MAILBOX_TX_MSG_IDX + mailbox->tx_idx;
    /* The message right after will be used for acknowledgement since writing to one message
     * sets the "ready" signal for both LP and HP status registers */
    const int next_idx = lp_core_next_msg_idx(mailbox->tx_idx);
    const int msg_ack_idx = LP_MAILBOX_TX_MSG_IDX + next_idx;
    /* The message after will be the next free message */
    mailbox->tx_idx = lp_core_next_msg_idx(next_idx);
    /* Make sure the next message status is cleared, to receive acknowledgement */
    lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, BIT(msg_ack_idx));
    /* Enable interrupt for the next message (ACK) */
    lp_core_mailbox_impl_intr_enable(mailbox->mb_ctx, BIT(msg_ack_idx));
    lp_core_mailbox_impl_set_message(mailbox->mb_ctx, msg_idx, msg);

    BaseType_t res = xSemaphoreTake(mailbox->received_sem, ticks_to_wait);
    if (res == pdFALSE) {
        /* Timeout, disable interrupts */
        lp_core_mailbox_impl_intr_disable(mailbox->mb_ctx, BIT(msg_ack_idx) | BIT(msg_idx));
        xSemaphoreGive(mailbox->mtx);
        return ESP_ERR_TIMEOUT;
    }

    if (mb_msg_status(mailbox, msg_ack_idx)) {
        /* Clear the status */
        lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, BIT(msg_ack_idx) | BIT(msg_idx));
    }

    xSemaphoreGive(mailbox->mtx);
    return ESP_OK;
}

esp_err_t lp_core_mailbox_send_async(lp_mailbox_t mailbox, lp_message_t msg)
{
    if (mailbox == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    xSemaphoreTake(mailbox->mtx, portMAX_DELAY);
    if (mb_async_mode(mailbox)) {
        xSemaphoreGive(mailbox->mtx);
        return ESP_ERR_INVALID_STATE;
    }
    /* Get the address of the next free message */
    const int msg_idx = LP_MAILBOX_TX_MSG_IDX + mailbox->tx_idx;
    mailbox->tx_idx = (mailbox->tx_idx + 1) % LP_MAILBOX_TX_MSG_COUNT;
    lp_core_mailbox_impl_set_message(mailbox->mb_ctx, msg_idx, msg);
    xSemaphoreGive(mailbox->mtx);
    return ESP_OK;
}

esp_err_t lp_core_mailbox_receive(lp_mailbox_t mailbox, lp_message_t* msg, TickType_t ticks_to_wait)
{
    if (mailbox == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    xSemaphoreTake(mailbox->mtx, portMAX_DELAY);
    /* Make sure we are not in asynchronous receive mode */
    if (mb_async_mode(mailbox)) {
        xSemaphoreGive(mailbox->mtx);
        return ESP_ERR_INVALID_STATE;
    }

    int msg_idx = -1;
    while (1) {
        /* Check if we received any message */
        if (mb_msg_search_read(mailbox, &msg_idx, msg)) {
            lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, BIT(msg_idx));
            break;
        }
        /* No received message yet, enable interrupts (again) */
        lp_core_mailbox_impl_intr_enable(mailbox->mb_ctx, LP_MAILBOX_RX_MSG_MASK);
        BaseType_t res = xSemaphoreTake(mailbox->received_sem, ticks_to_wait);
        if (res == pdFALSE) {
            /* Timeout, disable interrupts */
            lp_core_mailbox_impl_intr_disable(mailbox->mb_ctx, LP_MAILBOX_RX_MSG_MASK);
            xSemaphoreGive(mailbox->mtx);
            return ESP_ERR_TIMEOUT;
        }
    }

    /* Acknowledge the message by writing any value to the next one (guaranteed no overflow) */
    const int ack_idx = msg_idx + 1;
    lp_core_mailbox_impl_set_message(mailbox->mb_ctx, ack_idx, LP_MAILBOX_ACK);
    /* Clear ACK messages status */
    lp_core_mailbox_impl_intr_clear(mailbox->mb_ctx, BIT(ack_idx));

    xSemaphoreGive(mailbox->mtx);
    return ESP_OK;
}

esp_err_t lp_core_mailbox_receive_async(lp_mailbox_t mailbox, uint32_t count, lp_core_mailbox_callback_t cb)
{
    /* Make sure another asynchronous transaction is not on-going */
    if (mailbox == NULL || count == 0 || cb == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    xSemaphoreTake(mailbox->mtx, portMAX_DELAY);
    /* Make sure there isn't already an asynchronous transaction going on */
    if (mb_async_mode(mailbox)) {
        xSemaphoreGive(mailbox->mtx);
        return ESP_ERR_INVALID_STATE;
    }
    mailbox->rcv_callback = cb;
    mailbox->rcv_remaining = count;
    /* Enable interrupts for receive */
    lp_core_mailbox_impl_intr_enable(mailbox->mb_ctx, LP_MAILBOX_RX_MSG_MASK);
    xSemaphoreGive(mailbox->mtx);
    return ESP_OK;
}

esp_err_t lp_core_mailbox_receive_async_cancel(lp_mailbox_t mailbox, uint32_t* remaining)
{
    if (mailbox == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    xSemaphoreTake(mailbox->mtx, portMAX_DELAY);
    if (!mb_async_mode(mailbox)) {
        xSemaphoreGive(mailbox->mtx);
        return ESP_ERR_INVALID_STATE;
    }
    lp_core_mailbox_impl_intr_disable(mailbox->mb_ctx, LP_MAILBOX_RX_MSG_MASK);
    mailbox->rcv_callback = NULL;
    if (remaining) {
        *remaining = mailbox->rcv_remaining;
    }
    xSemaphoreGive(mailbox->mtx);
    return ESP_OK;
}
