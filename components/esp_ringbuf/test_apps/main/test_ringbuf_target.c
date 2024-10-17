/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file contains esp_ringbuf unit tests which run only on the chip target or QEMU,
 * but NOT on the Linux target since they are too hardware-specific.
 */

#include "sdkconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "driver/gptimer.h"
#include "esp_private/spi_flash_os.h"
#include "esp_memory_utils.h"
#include "esp_heap_caps.h"
#include "unity.h"
#include "esp_rom_sys.h"

#include "test_functions.h"

/* -------------------------- Test ring buffer ISR -----------------------------
 * The following test case tests ring buffer ISR API. A timer is used to trigger
 * the ISR. The test case will do the following
 * 1) ISR will be triggered periodically by timer
 * 2) The ISR will iterate through all ring buffer types where each iteration
 *    will send then receive an item to a ring buffer.
 */

#define ISR_ITERATIONS  ((BUFFER_SIZE / SMALL_ITEM_SIZE) * 2)

static int buf_type;
static int iterations;

static bool on_timer_alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    bool need_yield = false;

    //Test sending to buffer from ISR from ISR
    if (buf_type < NO_OF_RB_TYPES) {
        send_item_and_check(buffer_handles[buf_type], (void *)small_item, SMALL_ITEM_SIZE, 0, true);
    }

    //Receive item from ISR
    if (buf_type == RINGBUF_TYPE_NOSPLIT) {
        //Test receive from ISR for no-split buffer
        receive_check_and_return_item_no_split(buffer_handles[buf_type], (void *)small_item, SMALL_ITEM_SIZE, 0, true);
        buf_type++;
    } else if (buf_type == RINGBUF_TYPE_ALLOWSPLIT) {
        //Test send from ISR to allow-split buffer
        receive_check_and_return_item_allow_split(buffer_handles[buf_type], (void *)small_item, SMALL_ITEM_SIZE, 0, true);
        buf_type++;
    } else if (buf_type == RINGBUF_TYPE_BYTEBUF) {
        //Test receive from ISR for byte buffer
        receive_check_and_return_item_byte_buffer(buffer_handles[buf_type], (void *)small_item, SMALL_ITEM_SIZE, 0, true);
        buf_type++;
    } else if (buf_type == NO_OF_RB_TYPES) {
        //Check if all iterations complete
        if (iterations < ISR_ITERATIONS) {
            iterations++;
            buf_type = 0;   //Reset and iterate through each buffer type again
            goto out;
        } else {
            //Signal complete
            BaseType_t task_woken = pdFALSE;
            xSemaphoreGiveFromISR(done_sem, &task_woken);
            if (task_woken == pdTRUE) {
                buf_type++;
                need_yield = true;
            }
        }
    }

out:
    return need_yield;
}

// IDF-6471 - test hangs up on QEMU
TEST_CASE("Test ring buffer ISR", "[esp_ringbuf][qemu-ignore]")
{
    gptimer_handle_t gptimer;
    for (int i = 0; i < NO_OF_RB_TYPES; i++) {
        buffer_handles[i] = xRingbufferCreate(BUFFER_SIZE, i);
    }
    done_sem = xSemaphoreCreateBinary();
    buf_type = 0;
    iterations = 0;

    //Setup timer for ISR
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    TEST_ESP_OK(gptimer_new_timer(&config, &gptimer));
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 2000,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    TEST_ESP_OK(gptimer_set_alarm_action(gptimer, &alarm_config));
    TEST_ESP_OK(gptimer_enable(gptimer));
    TEST_ESP_OK(gptimer_start(gptimer));
    //Wait for ISR to complete multiple iterations
    xSemaphoreTake(done_sem, portMAX_DELAY);

    //Cleanup
    TEST_ESP_OK(gptimer_stop(gptimer));
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
    vSemaphoreDelete(done_sem);
    for (int i = 0; i < NO_OF_RB_TYPES; i++) {
        vRingbufferDelete(buffer_handles[i]);
    }
}

/* ---------------------------- Test ring buffer SMP ---------------------------
 * The following test case tests each type of ring buffer in an SMP fashion. A
 * sending task and a receiving task is created. The sending task will split
 * a continuous piece of data into items of random length and send it to a ring
 * buffer. The receiving task will receive and check those items.
 * Every permutation of core pinning of the sending and receiving task will be
 * tested.
 */

#if !CONFIG_RINGBUF_PLACE_FUNCTIONS_INTO_FLASH && !CONFIG_RINGBUF_PLACE_ISR_FUNCTIONS_INTO_FLASH
/* -------------------------- Test ring buffer IRAM ------------------------- */

static IRAM_ATTR __attribute__((noinline)) bool iram_ringbuf_test(void)
{
    bool result = true;
    uint8_t item[4];
    size_t item_size;
    RingbufHandle_t handle = xRingbufferCreate(CONT_DATA_TEST_BUFF_LEN, RINGBUF_TYPE_NOSPLIT);
    result = result && (handle != NULL);
    spi_flash_guard_get()->start(); // Disables flash cache

    xRingbufferGetMaxItemSize(handle);
    xRingbufferSendFromISR(handle, (void *)item, sizeof(item), NULL);
    xRingbufferReceiveFromISR(handle, &item_size);

    spi_flash_guard_get()->end(); // Re-enables flash cache
    vRingbufferDelete(handle);

    return result;
}

TEST_CASE("Test ringbuffer functions work with flash cache disabled", "[esp_ringbuf]")
{
    TEST_ASSERT(iram_ringbuf_test());
}
#endif /* !CONFIG_RINGBUF_PLACE_FUNCTIONS_INTO_FLASH && !CONFIG_RINGBUF_PLACE_ISR_FUNCTIONS_INTO_FLASH */

/* ------------------------ Test ring buffer 0 Item Size -----------------------
 * The following test case tests that sending/acquiring an item/bytes of 0 size
 * is permissible.
 */

/* --------------------- Test ring buffer create with caps ---------------------
 * The following test case tests ring buffer creation with caps. Specifically
 * the following APIs:
 *
 * - xRingbufferCreateWithCaps()
 * - vRingbufferDeleteWithCaps()
 * - xRingbufferGetStaticBuffer()
 */

TEST_CASE("Test ringbuffer with caps", "[esp_ringbuf]")
{
    RingbufHandle_t rb_handle;
    uint8_t *rb_storage;
    StaticRingbuffer_t *rb_obj;

    // Create ring buffer with caps
    rb_handle = xRingbufferCreateWithCaps(BUFFER_SIZE, RINGBUF_TYPE_NOSPLIT, (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    TEST_ASSERT_NOT_EQUAL(NULL, rb_handle);

    // Get the ring buffer's memory
    TEST_ASSERT_EQUAL(pdTRUE, xRingbufferGetStaticBuffer(rb_handle, &rb_storage, &rb_obj));
    TEST_ASSERT(esp_ptr_in_dram(rb_storage));
    TEST_ASSERT(esp_ptr_in_dram(rb_obj));

    // Free the ring buffer
    vRingbufferDeleteWithCaps(rb_handle);
}
