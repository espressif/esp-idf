/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file contains all esp_ringbuf unit tests which run on both
 * the chip target as well as on the Linux target.
 */

#include "sdkconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "unity.h"
#include "esp_rom_sys.h"

#include "test_functions.h"

//Definitions used in multiple test cases
#define TIMEOUT_TICKS               10
#define LARGE_ITEM_SIZE             (2 * SMALL_ITEM_SIZE)  //16 bytes

const uint8_t small_item[SMALL_ITEM_SIZE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
static const uint8_t large_item[LARGE_ITEM_SIZE] = { 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                                     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
                                                   };
RingbufHandle_t buffer_handles[NO_OF_RB_TYPES];
SemaphoreHandle_t done_sem;

void send_item_and_check(RingbufHandle_t handle, const uint8_t *item,  size_t item_size, TickType_t ticks_to_wait, bool in_isr)
{
    BaseType_t ret;
    if (in_isr) {
        ret = xRingbufferSendFromISR(handle, (void *)item, item_size, NULL);
    } else {
        ret = xRingbufferSend(handle, (void *)item, item_size, ticks_to_wait);
    }
    TEST_ASSERT_MESSAGE(ret == pdTRUE, "Failed to send item");
}

static void send_item_and_check_failure(RingbufHandle_t handle, const uint8_t *item,  size_t item_size, TickType_t ticks_to_wait, bool in_isr)
{
    BaseType_t ret;
    if (in_isr) {
        ret = xRingbufferSendFromISR(handle, (void *)item, item_size, NULL);
    } else {
        ret = xRingbufferSend(handle, (void *)item, item_size, ticks_to_wait);
    }
    TEST_ASSERT_MESSAGE(ret == pdFALSE, "Sent an item to a full buffer");
}

void receive_check_and_return_item_no_split(RingbufHandle_t handle, const uint8_t *expected_data, size_t expected_size, TickType_t ticks_to_wait, bool in_isr)
{
    //Receive item from no-split buffer
    size_t item_size;
    uint8_t *item;
    if (in_isr) {
        item = (uint8_t *)xRingbufferReceiveFromISR(handle, &item_size);
    } else {
        item = (uint8_t *)xRingbufferReceive(handle, &item_size, ticks_to_wait);
    }
    TEST_ASSERT_MESSAGE(item != NULL, "Failed to receive item");
    TEST_ASSERT_MESSAGE(item_size == expected_size, "Item size is incorrect");
    //Check data of received item
    for (int i = 0; i < item_size; i++) {
        TEST_ASSERT_MESSAGE(item[i] == expected_data[i], "Item data is invalid");
    }
    //Return item
    if (in_isr) {
        vRingbufferReturnItemFromISR(handle, (void *)item, NULL);
    } else {
        vRingbufferReturnItem(handle, (void *)item);
    }

}

void receive_check_and_return_item_allow_split(RingbufHandle_t handle, const uint8_t *expected_data, size_t expected_size, TickType_t ticks_to_wait, bool in_isr)
{
    //Receive item
    size_t item_size1, item_size2;
    uint8_t *item1, *item2;
    BaseType_t ret;
    if (in_isr) {
        ret = xRingbufferReceiveSplitFromISR(handle, (void **)&item1, (void **)&item2, &item_size1, &item_size2);
    } else {
        ret = xRingbufferReceiveSplit(handle, (void **)&item1, (void **)&item2, &item_size1, &item_size2, ticks_to_wait);
    }
    TEST_ASSERT_MESSAGE(ret == pdTRUE, "Failed to receive item");
    TEST_ASSERT_MESSAGE(item1 != NULL, "Failed to receive item");

    //Check data of received item(s) and return them
    if (item2 == NULL) {
        TEST_ASSERT_MESSAGE(item_size1 == expected_size, "Item size is incorrect");
        for (int i = 0; i < item_size1; i++) {
            TEST_ASSERT_MESSAGE(item1[i] == expected_data[i], "Item data is invalid");
        }
        //Return item
        if (in_isr) {
            vRingbufferReturnItemFromISR(handle, (void *)item1, NULL);
        } else {
            vRingbufferReturnItem(handle, (void *)item1);
        }
    } else {
        //Item was split
        TEST_ASSERT_MESSAGE(item_size1 + item_size2 == expected_size, "Total item size is incorrect");
        for (int i = 0; i < item_size1; i++) {
            TEST_ASSERT_MESSAGE(item1[i] == expected_data[i], "Head item data is invalid");
        }
        for (int i = 0; i < item_size2; i++) {
            TEST_ASSERT_MESSAGE(item2[i] == expected_data[item_size1 + i], "Head item data is invalid");
        }
        //Return Items
        if (in_isr) {
            vRingbufferReturnItemFromISR(handle, (void *)item1, NULL);
            vRingbufferReturnItemFromISR(handle, (void *)item2, NULL);
        } else {
            vRingbufferReturnItem(handle, (void *)item1);
            vRingbufferReturnItem(handle, (void *)item2);
        }
    }
}

void receive_check_and_return_item_byte_buffer(RingbufHandle_t handle, const uint8_t *expected_data, size_t expected_size, TickType_t ticks_to_wait, bool in_isr)
{
    //Receive item
    size_t item_size;
    uint8_t *item;
    if (in_isr) {
        item = (uint8_t *)xRingbufferReceiveUpToFromISR(handle, &item_size, expected_size);
    } else {
        item = (uint8_t *)xRingbufferReceiveUpTo(handle, &item_size, ticks_to_wait, expected_size);   //Limit amount of bytes returned to the size of one item
    }
    TEST_ASSERT_MESSAGE(item != NULL, "Failed to receive item");

    //Check data of received item
    for (int i = 0; i < item_size; i++) {
        TEST_ASSERT_MESSAGE(item[i] == expected_data[i], "Item data is invalid");
    }
    //Return item
    if (in_isr) {
        vRingbufferReturnItemFromISR(handle, (void *)item, NULL);
    } else {
        vRingbufferReturnItem(handle, (void *)item);
    }

    //Check if item wrapped around
    if (item_size < expected_size) {
        //Item is wrapped, receive second portion
        size_t item_size2;
        uint8_t *item2;
        if (in_isr) {
            item2 = (uint8_t *)xRingbufferReceiveUpToFromISR(handle, &item_size2, expected_size - item_size);
        } else {
            item2 = (uint8_t *)xRingbufferReceiveUpTo(handle, &item_size2, ticks_to_wait, expected_size - item_size);
        }
        //= (uint8_t *)xRingbufferReceiveUpTo(handle, &item_size2, ticks_to_wait, expected_size - item_size);
        TEST_ASSERT_MESSAGE(item2 != NULL, "Failed to receive item");
        TEST_ASSERT_MESSAGE(item_size + item_size2 == expected_size, "Total item size is incorrect");
        for (int i = 0; i < item_size2; i++) {
            TEST_ASSERT_MESSAGE(item2[i] == expected_data[item_size + i], "Item data is invalid");
        }
        if (in_isr) {
            vRingbufferReturnItemFromISR(handle, (void *)item2, NULL);
        } else {
            vRingbufferReturnItem(handle, (void *)item2);
        }
    } else {
        TEST_ASSERT_MESSAGE(item_size == expected_size, "Item size is incorrect");
    }
}

/* ----------------- Basic ring buffer behavior tests cases --------------------
 * The following set of test cases will test basic send, receive, wrap around and buffer full
 * behavior of each type of ring buffer.
 * Test Case 1:
 *     1) Send multiple items (nearly fill the buffer)
 *     2) Receive and check the sent items (also prepares the buffer for a wrap around)
 *     3) Send a final item that causes a wrap around
 *     4) Receive and check the wrapped item
 *
 * Test Case 2:
 *     1) Send multiple items (completely fill the buffer)
 *     2) Send another item and verify that send failure occurs
 *     3) Receive one item and verify that space is freed up in the buffer
 *     4) Receive and check the remaining sent items
 *
 * Test Case 3:
 *     1) Send multiple items (nearly fill the buffer)
 *     2) Send another small sized item to fill the buffer without causing a wrap around (not needed in case of byte buffer)
 *     2) Send another item and verify that send failure occurs
 *     4) Receive and check the sent items
 */

TEST_CASE("TC#1: No-Split", "[esp_ringbuf][linux]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_NOSPLIT);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");

    //Check buffer free size and max item size upon buffer creation. Should be BUFFER_SIZE/2 - ITEM_HDR_SIZE.
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == ((BUFFER_SIZE >> 1) - ITEM_HDR_SIZE), "Incorrect buffer free size received");
    TEST_ASSERT_MESSAGE(xRingbufferGetMaxItemSize(buffer_handle) == ((BUFFER_SIZE >> 1) - ITEM_HDR_SIZE), "Incorrect max item size received");

    //Calculate number of items to send. Aim to almost fill buffer to setup for wrap around
    int no_of_items = (BUFFER_SIZE - (ITEM_HDR_SIZE + SMALL_ITEM_SIZE)) / (ITEM_HDR_SIZE + SMALL_ITEM_SIZE);

    //Test sending items
    for (int i = 0; i < no_of_items; i++) {
        send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify items waiting matches with the number of items sent
    UBaseType_t items_waiting;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == no_of_items, "Incorrect items waiting");

    //Test receiving items
    for (int i = 0; i < no_of_items; i++) {
        receive_check_and_return_item_no_split(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify that no items are waiting
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == 0, "Incorrect items waiting");

    //Write pointer should be near the end, test wrap around
    UBaseType_t write_pos_before, write_pos_after;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_before, NULL, NULL);
    //Send large item that causes wrap around
    send_item_and_check(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    //Receive wrapped item
    receive_check_and_return_item_no_split(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_after, NULL, NULL);
    TEST_ASSERT_MESSAGE(write_pos_after < write_pos_before, "Failed to wrap around");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

TEST_CASE("TC#2: No-Split", "[esp_ringbuf][linux]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_NOSPLIT);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");

    //Check buffer free size and max item size upon buffer creation. Should be BUFFER_SIZE/2 - ITEM_HDR_SIZE.
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == ((BUFFER_SIZE >> 1) - ITEM_HDR_SIZE), "Incorrect buffer free size received");
    TEST_ASSERT_MESSAGE(xRingbufferGetMaxItemSize(buffer_handle) == ((BUFFER_SIZE >> 1) - ITEM_HDR_SIZE), "Incorrect max item size received");

    //Calculate number of items to send. Aim to fill the buffer
    int no_of_items = (BUFFER_SIZE) / (ITEM_HDR_SIZE + SMALL_ITEM_SIZE);

    //Test sending items
    for (int i = 0; i < no_of_items; i++) {
        send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify items waiting matches with the number of items sent
    UBaseType_t items_waiting;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == no_of_items, "Incorrect items waiting");

    //At this point, the buffer should be full.
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == 0, "Buffer full not achieved");

    //Send an item. The item should not be sent to a full buffer.
    send_item_and_check_failure(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //Receive one item
    receive_check_and_return_item_no_split(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //At this point, the buffer should not be full any more
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) > 0, "Buffer should have free space");

    //Test receiving remaining items
    for (int i = 0; i < no_of_items - 1; i++) {
        receive_check_and_return_item_no_split(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify that no items are waiting
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == 0, "Incorrect items waiting");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

TEST_CASE("TC#3: No-Split", "[esp_ringbuf][linux]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_NOSPLIT);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");

    //Check buffer free size and max item size upon buffer creation. Should be BUFFER_SIZE/2 - ITEM_HDR_SIZE.
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == ((BUFFER_SIZE >> 1) - ITEM_HDR_SIZE), "Incorrect buffer free size received");
    TEST_ASSERT_MESSAGE(xRingbufferGetMaxItemSize(buffer_handle) == ((BUFFER_SIZE >> 1) - ITEM_HDR_SIZE), "Incorrect max item size received");

    //Calculate number of medium items to send. Aim to almost fill the buffer
    int no_of_medium_items = (BUFFER_SIZE - (ITEM_HDR_SIZE + MEDIUM_ITEM_SIZE)) / (ITEM_HDR_SIZE + MEDIUM_ITEM_SIZE);

    //Test sending items
    for (int i = 0; i < no_of_medium_items; i++) {
        send_item_and_check(buffer_handle, large_item, MEDIUM_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify items waiting matches with the number of medium items sent
    UBaseType_t items_waiting;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == no_of_medium_items, "Incorrect items waiting");

    //Send one small sized item. This will ensure that the item fits at the end of the buffer without causing the write pointer to wrap around.
    send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //The buffer should not have any free space as the number of bytes remaining should be < ITEM_HDR_SIZE.
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == 0, "Buffer full not achieved");

    //Send an item. The item should not be sent to a full buffer.
    send_item_and_check_failure(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //Test receiving medium items
    for (int i = 0; i < no_of_medium_items; i++) {
        receive_check_and_return_item_no_split(buffer_handle, large_item, MEDIUM_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Test receiving small item
    receive_check_and_return_item_no_split(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //Verify that no items are waiting
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == 0, "Incorrect items waiting");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

TEST_CASE("TC#1: Allow-Split", "[esp_ringbuf][linux]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_ALLOWSPLIT);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");

    //Check buffer free size and max item size upon buffer creation. Should be BUFFER_SIZE - (ITEM_HDR_SIZE * 2).
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == (BUFFER_SIZE - (ITEM_HDR_SIZE * 2)), "Incorrect buffer free size received");
    TEST_ASSERT_MESSAGE(xRingbufferGetMaxItemSize(buffer_handle) == (BUFFER_SIZE - (ITEM_HDR_SIZE * 2)), "Incorrect max item size received");

    //Calculate number of items to send. Aim to almost fill buffer to setup for wrap around
    int no_of_items = (BUFFER_SIZE - (ITEM_HDR_SIZE + SMALL_ITEM_SIZE)) / (ITEM_HDR_SIZE + SMALL_ITEM_SIZE);

    //Test sending items
    for (int i = 0; i < no_of_items; i++) {
        send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify items waiting matches with the number of items sent
    UBaseType_t items_waiting;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == no_of_items, "Incorrect items waiting");

    //Test receiving items
    for (int i = 0; i < no_of_items; i++) {
        receive_check_and_return_item_allow_split(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify that no items are waiting
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == 0, "Incorrect items waiting");

    //Write pointer should be near the end, test wrap around
    UBaseType_t write_pos_before, write_pos_after;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_before, NULL, NULL);
    //Send large item that causes wrap around
    send_item_and_check(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    //Receive wrapped item
    receive_check_and_return_item_allow_split(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_after, NULL, NULL);
    TEST_ASSERT_MESSAGE(write_pos_after < write_pos_before, "Failed to wrap around");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

TEST_CASE("TC#2: Allow-Split", "[esp_ringbuf][linux]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_ALLOWSPLIT);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");

    //Check buffer free size and max item size upon buffer creation. Should be BUFFER_SIZE - (ITEM_HDR_SIZE * 2).
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == (BUFFER_SIZE - (ITEM_HDR_SIZE * 2)), "Incorrect buffer free size received");
    TEST_ASSERT_MESSAGE(xRingbufferGetMaxItemSize(buffer_handle) == (BUFFER_SIZE - (ITEM_HDR_SIZE * 2)), "Incorrect max item size received");

    //Calculate number of items to send. Aim to fill the buffer
    int no_of_items = (BUFFER_SIZE) / (ITEM_HDR_SIZE + SMALL_ITEM_SIZE);

    //Test sending items
    for (int i = 0; i < no_of_items; i++) {
        send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify items waiting matches with the number of items sent
    UBaseType_t items_waiting;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == no_of_items, "Incorrect items waiting");

    //At this point, the buffer should be full.
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == 0, "Buffer full not achieved");

    //Send an item. The item should not be sent to a full buffer.
    send_item_and_check_failure(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //Receive one item
    receive_check_and_return_item_allow_split(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //At this point, the buffer should not be full any more
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) > 0, "Buffer should have free space");

    //Test receiving remaining items
    for (int i = 0; i < no_of_items - 1; i++) {
        receive_check_and_return_item_allow_split(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify that no items are waiting
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == 0, "Incorrect items waiting");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

TEST_CASE("TC#3: Allow-Split", "[esp_ringbuf][linux]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_ALLOWSPLIT);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");

    //Check buffer free size and max item size upon buffer creation. Should be BUFFER_SIZE - (ITEM_HDR_SIZE * 2).
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == (BUFFER_SIZE - (ITEM_HDR_SIZE * 2)), "Incorrect buffer free size received");
    TEST_ASSERT_MESSAGE(xRingbufferGetMaxItemSize(buffer_handle) == (BUFFER_SIZE - (ITEM_HDR_SIZE * 2)), "Incorrect max item size received");

    //Calculate number of medium items to send. Aim to almost fill the buffer
    int no_of_medium_items = (BUFFER_SIZE - (ITEM_HDR_SIZE + MEDIUM_ITEM_SIZE)) / (ITEM_HDR_SIZE + MEDIUM_ITEM_SIZE);

    //Test sending items
    for (int i = 0; i < no_of_medium_items; i++) {
        send_item_and_check(buffer_handle, large_item, MEDIUM_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify items waiting matches with the number of medium items sent
    UBaseType_t items_waiting;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == no_of_medium_items, "Incorrect items waiting");

    //Send one small sized item. This will ensure that the item fits at the end of the buffer without causing the write pointer to wrap around.
    send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //The buffer should not have any free space as the number of bytes remaining should be < ITEM_HDR_SIZE.
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == 0, "Buffer full not achieved");

    //Send an item. The item should not be sent to a full buffer.
    send_item_and_check_failure(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //Test receiving medium items
    for (int i = 0; i < no_of_medium_items; i++) {
        receive_check_and_return_item_allow_split(buffer_handle, large_item, MEDIUM_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Test receiving small item
    receive_check_and_return_item_allow_split(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //Verify that no items are waiting
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == 0, "Incorrect items waiting");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

TEST_CASE("TC#1: Byte buffer", "[esp_ringbuf][linux]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");

    //Check buffer free size and max item size upon buffer creation. Should be BUFFER_SIZE
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == BUFFER_SIZE, "Incorrect buffer free size received");
    TEST_ASSERT_MESSAGE(xRingbufferGetMaxItemSize(buffer_handle) == BUFFER_SIZE, "Incorrect max item size received");

    //Calculate number of items to send. Aim to almost fill buffer to setup for wrap around
    int no_of_items = (BUFFER_SIZE - SMALL_ITEM_SIZE) / SMALL_ITEM_SIZE;

    //Test sending items
    for (int i = 0; i < no_of_items; i++) {
        send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify items waiting matches with the number of items sent
    UBaseType_t items_waiting;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == no_of_items * SMALL_ITEM_SIZE, "Incorrect number of bytes waiting");

    //Test receiving items
    for (int i = 0; i < no_of_items; i++) {
        receive_check_and_return_item_byte_buffer(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify that no items are waiting
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == 0, "Incorrect number of bytes waiting");

    //Write pointer should be near the end, test wrap around
    UBaseType_t write_pos_before, write_pos_after;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_before, NULL, NULL);
    //Send large item that causes wrap around
    send_item_and_check(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    //Receive wrapped item
    receive_check_and_return_item_byte_buffer(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_after, NULL, NULL);
    TEST_ASSERT_MESSAGE(write_pos_after < write_pos_before, "Failed to wrap around");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

TEST_CASE("TC#2: Byte buffer", "[esp_ringbuf][linux]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");

    //Check buffer free size and max item size upon buffer creation. Should be BUFFER_SIZE.
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == BUFFER_SIZE, "Incorrect buffer free size received");
    TEST_ASSERT_MESSAGE(xRingbufferGetMaxItemSize(buffer_handle) == BUFFER_SIZE, "Incorrect max item size received");

    //Calculate number of items to send. Aim to fill the buffer
    int no_of_items = BUFFER_SIZE / SMALL_ITEM_SIZE;

    //Test sending items
    for (int i = 0; i < no_of_items; i++) {
        send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify items waiting matches with the number of items sent
    UBaseType_t items_waiting;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == no_of_items * SMALL_ITEM_SIZE, "Incorrect number of bytes waiting");

    //At this point, the buffer should be full.
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == 0, "Buffer full not achieved");

    //Send an item. The item should not be sent to a full buffer.
    send_item_and_check_failure(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //Receive one item
    receive_check_and_return_item_byte_buffer(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);

    //At this point, the buffer should not be full any more
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) > 0, "Buffer should have free space");

    //Test receiving remaining items
    for (int i = 0; i < no_of_items - 1; i++) {
        receive_check_and_return_item_byte_buffer(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify that no items are waiting
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == 0, "Incorrect items waiting");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

TEST_CASE("TC#3: Byte buffer", "[esp_ringbuf][linux]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");

    //Check buffer free size and max item size upon buffer creation. Should be BUFFER_SIZE.
    TEST_ASSERT_MESSAGE(xRingbufferGetCurFreeSize(buffer_handle) == BUFFER_SIZE, "Incorrect buffer free size received");
    TEST_ASSERT_MESSAGE(xRingbufferGetMaxItemSize(buffer_handle) == BUFFER_SIZE, "Incorrect max item size received");

    //Calculate number of medium items to send. Aim to almost fill the buffer
    int no_of_medium_items = BUFFER_SIZE / MEDIUM_ITEM_SIZE;

    //Test sending items
    for (int i = 0; i < no_of_medium_items; i++) {
        send_item_and_check(buffer_handle, large_item, MEDIUM_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify items waiting matches with the number of medium items sent
    UBaseType_t items_waiting;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == no_of_medium_items * MEDIUM_ITEM_SIZE, "Incorrect number of bytes waiting");

    //The buffer should not have any free space for another item.
    TEST_ASSERT_LESS_THAN_MESSAGE(MEDIUM_ITEM_SIZE, xRingbufferGetCurFreeSize(buffer_handle), "Buffer full not achieved");

    //Send an item. The item should not be sent to a full buffer.
    send_item_and_check_failure(buffer_handle, small_item, MEDIUM_ITEM_SIZE, TIMEOUT_TICKS, false);

    //Test receiving medium items
    for (int i = 0; i < no_of_medium_items; i++) {
        receive_check_and_return_item_byte_buffer(buffer_handle, large_item, MEDIUM_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Verify that no items are waiting
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == 0, "Incorrect items waiting");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

/* ----------------------- Ring buffer queue sets test ------------------------
 * The following test case will test receiving from ring buffers that have been
 * added to a queue set. The test case will do the following...
 * 1) Ring buffer of each type is created and added to the queue set
 * 2) A receiving task is created to select from the queue set and read from the appropriate ring buffer
 */

static void queue_set_receiving_task(void *queue_set_handle)
{
    QueueSetHandle_t queue_set = (QueueSetHandle_t)queue_set_handle;

    //Receive multiple items via queue set
    BaseType_t done = pdFALSE;
    int no_of_items = BUFFER_SIZE / SMALL_ITEM_SIZE;
    int items_rec_count[NO_OF_RB_TYPES] = {0};
    while (done != pdTRUE) {
        QueueSetMemberHandle_t member = xQueueSelectFromSet(queue_set, TIMEOUT_TICKS);
        //Read from selected ring buffer
        if (xRingbufferCanRead(buffer_handles[0], member) == pdTRUE) {
            //No-split buffer
            receive_check_and_return_item_no_split(buffer_handles[0], small_item, SMALL_ITEM_SIZE, 0, false);
            items_rec_count[0] ++;
        } else if (xRingbufferCanRead(buffer_handles[1], member) == pdTRUE) {
            //Allow-split buffer
            receive_check_and_return_item_allow_split(buffer_handles[1], small_item, SMALL_ITEM_SIZE, 0, false);
            items_rec_count[1] ++;
        } else if (xRingbufferCanRead(buffer_handles[2], member) == pdTRUE) {
            //Byte buffer
            receive_check_and_return_item_byte_buffer(buffer_handles[2], small_item, SMALL_ITEM_SIZE, 0, false);
            items_rec_count[2] ++;
        } else {
            TEST_ASSERT_MESSAGE(false, "Error with queue set member");
        }

        //Check for completion
        if (items_rec_count[0] == no_of_items &&
                items_rec_count[1] == no_of_items &&
                items_rec_count[2] == no_of_items) {
            done = pdTRUE;
        }
    }

    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

TEST_CASE("Test ring buffer with queue sets", "[esp_ringbuf][linux]")
{
    QueueSetHandle_t queue_set = xQueueCreateSet(NO_OF_RB_TYPES);
    done_sem = xSemaphoreCreateBinary();

    //Create ring buffer of each type, then add them to a queue set
    for (int i = 0; i < NO_OF_RB_TYPES; i++) {
        buffer_handles[i] = xRingbufferCreate(BUFFER_SIZE, i);
        TEST_ASSERT_MESSAGE(buffer_handles[i] != NULL, "Failed to create ring buffer");
        TEST_ASSERT_MESSAGE(xRingbufferAddToQueueSetRead(buffer_handles[i], queue_set) == pdPASS, "Failed to add to read queue set");
    }
    //Create a task to send items to each ring buffer
    int no_of_items = BUFFER_SIZE / SMALL_ITEM_SIZE;
    xTaskCreatePinnedToCore(queue_set_receiving_task, "rec tsk", 2048, (void *)queue_set, 10, NULL, 0);

    //Send multiple items to each type of ring buffer
    for (int i = 0; i < no_of_items; i++) {
        for (int j = 0; j < NO_OF_RB_TYPES; j++) {
            send_item_and_check(buffer_handles[j], small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
        }
    }

    xSemaphoreTake(done_sem, portMAX_DELAY);
    vSemaphoreDelete(done_sem);
    //Remove and delete ring buffers from queue sets
    for (int i = 0; i < NO_OF_RB_TYPES; i++) {
        TEST_ASSERT_MESSAGE(xRingbufferRemoveFromQueueSetRead(buffer_handles[i], queue_set) == pdTRUE, "Failed to remove from read queue set");
        vRingbufferDelete(buffer_handles[i]);
    }
    vQueueDelete(queue_set);
    vTaskDelay(1);
}

/* ---------------------------- Test ring buffer SMP ---------------------------
 * The following test case tests each type of ring buffer in an SMP fashion. A
 * sending task and a receiving task is created. The sending task will split
 * a continuous piece of data into items of random length and send it to a ring
 * buffer. The receiving task will receive and check those items.
 * Every permutation of core pinning of the sending and receiving task will be
 * tested.
 */

#define SRAND_SEED                      3   //Arbitrarily chosen srand() seed
#define SMP_TEST_ITERATIONS             4

static const char continuous_data[] = {"A_very_long_string_that_will_be_split_into_"
                                       "items_of_random_lengths_and_sent_to_the_ring_"
                                       "buffer._The_maximum_random_length_will_also_"
                                       "be_increased_over_multiple_iterations_in_this"
                                       "_test"
                                      };
SemaphoreHandle_t tasks_done;
SemaphoreHandle_t tx_done;
SemaphoreHandle_t rx_done;

size_t continuous_test_string_len(void)
{
    return sizeof(continuous_data);
}

#if !CONFIG_FREERTOS_UNICORE

void send_to_buffer(RingbufHandle_t buffer, size_t max_item_size)
{
    for (int iter = 0; iter < SMP_TEST_ITERATIONS; iter++) {
        size_t bytes_sent = 0;      //Number of data bytes sent in this iteration
        size_t next_item_size;      //Size of next item to send

        while (bytes_sent < CONT_DATA_LEN) {
            //Get size of next item
            next_item_size = rand() % (max_item_size + 1);
            if (next_item_size + bytes_sent > CONT_DATA_LEN) {
                next_item_size = CONT_DATA_LEN - bytes_sent;
            }

            //Send item
            TEST_ASSERT_MESSAGE(xRingbufferSend(buffer, (void *) & (continuous_data[bytes_sent]), next_item_size, TIMEOUT_TICKS) == pdTRUE, "Failed to send an item");
            bytes_sent += next_item_size;
        }
        xSemaphoreGive(tx_done);
        xSemaphoreTake(rx_done, portMAX_DELAY);
    }
}

void read_from_buffer(RingbufHandle_t buffer, RingbufferType_t buf_type, size_t max_rec_size)
{
    for (int iter = 0; iter < SMP_TEST_ITERATIONS; iter++) {
        size_t bytes_rec = 0;      //Number of data bytes received in this iteration
        while (bytes_rec < CONT_DATA_LEN) {
            size_t item_size, item_size2;    //Possible for allow split buffers to receive two items
            char *item_data, *item_data2;

            //Select appropriate receive function for type of ring buffer
            if (buf_type ==  RINGBUF_TYPE_NOSPLIT) {
                item_data = (char *)xRingbufferReceive(buffer, &item_size, TIMEOUT_TICKS);
            } else if (buf_type == RINGBUF_TYPE_ALLOWSPLIT) {
                BaseType_t ret = xRingbufferReceiveSplit(buffer, (void **)&item_data, (void **)&item_data2, &item_size, &item_size2, TIMEOUT_TICKS);
                TEST_ASSERT_MESSAGE(ret == pdTRUE, "Failed to receive any item");
            } else {
                item_data = (char *)xRingbufferReceiveUpTo(buffer, &item_size, TIMEOUT_TICKS, max_rec_size);
            }

            //Check received item and return it
            TEST_ASSERT_MESSAGE(item_data != NULL, "Failed to receive an item");
            if (buf_type == RINGBUF_TYPE_BYTEBUF) {
                TEST_ASSERT_MESSAGE(item_size <= max_rec_size, "Received data exceeds max size");
            }
            for (int i = 0; i < item_size; i++) {
                //Check item_data is valid
                TEST_ASSERT_MESSAGE(item_data[i] == continuous_data[bytes_rec + i], "Received data is corrupted");
            }
            bytes_rec += item_size;
            vRingbufferReturnItem(buffer, item_data);
            if (buf_type == RINGBUF_TYPE_ALLOWSPLIT && item_data2 != NULL) {
                //Check item_data2 is valid
                for (int i = 0; i < item_size2; i++) {
                    TEST_ASSERT_MESSAGE(item_data2[i] == continuous_data[bytes_rec + i], "Received split data is corrupted");
                }
                bytes_rec += item_size2;
                vRingbufferReturnItem(buffer, item_data2);
            }
        }
        TEST_ASSERT_MESSAGE(bytes_rec == CONT_DATA_LEN, "Total length of received data is incorrect");
        xSemaphoreGive(rx_done);
        xSemaphoreTake(tx_done, portMAX_DELAY);
    }
}

void send_task(void *args)
{
    RingbufHandle_t buffer = ((task_args_t *)args)->buffer;
    size_t max_item_len = xRingbufferGetMaxItemSize(buffer);

    //Test sending short length items
    send_to_buffer(buffer, 1);
    //Test sending mid length items
    send_to_buffer(buffer, max_item_len / 2);
    //Test sending long length items
    send_to_buffer(buffer, max_item_len);
    vTaskDelete(NULL);
}

void rec_task(void *args)
{
    RingbufHandle_t buffer = ((task_args_t *)args)->buffer;
    size_t max_rec_len = xRingbufferGetMaxItemSize(buffer);

    //Test receiving short length items
    read_from_buffer(buffer, ((task_args_t *)args)->type, 1);
    //Test receiving mid length items
    read_from_buffer(buffer, ((task_args_t *)args)->type, max_rec_len / 2);
    //Test receiving long length items
    read_from_buffer(buffer, ((task_args_t *)args)->type, max_rec_len);

    xSemaphoreGive(tasks_done);
    vTaskDelete(NULL);
}

void setup(void)
{
    esp_rom_printf("Size of test data: %d\n", CONT_DATA_LEN);
    tx_done = xSemaphoreCreateBinary();                 //Semaphore to indicate send is done for a particular iteration
    rx_done = xSemaphoreCreateBinary();                 //Semaphore to indicate receive is done for a particular iteration
    tasks_done = xSemaphoreCreateBinary();              //Semaphore used to to indicate send and receive tasks completed running
    srand(SRAND_SEED);                                  //Seed RNG
}

void cleanup(void)
{
    //Cleanup
    vSemaphoreDelete(tx_done);
    vSemaphoreDelete(rx_done);
    vSemaphoreDelete(tasks_done);
}

TEST_CASE("Test ring buffer SMP", "[esp_ringbuf][linux]")
{
    setup();
    //Iterate through buffer types (No split, split, then byte buff)
    for (RingbufferType_t buf_type = 0; buf_type < RINGBUF_TYPE_MAX; buf_type++) {
        //Create buffer
        task_args_t task_args;
        task_args.buffer = xRingbufferCreate(CONT_DATA_TEST_BUFF_LEN, buf_type); //Create buffer of selected type
        task_args.type = buf_type;
        TEST_ASSERT_MESSAGE(task_args.buffer != NULL, "Failed to create ring buffer");

        for (int prior_mod = -1; prior_mod < 2; prior_mod++) {  //Test different relative priorities
            //Test every permutation of core affinity
            for (int send_core = 0; send_core < CONFIG_FREERTOS_NUMBER_OF_CORES; send_core++) {
                for (int rec_core = 0; rec_core < CONFIG_FREERTOS_NUMBER_OF_CORES; rec_core ++) {
                    esp_rom_printf("Type: %d, PM: %d, SC: %d, RC: %d\n", buf_type, prior_mod, send_core, rec_core);
                    xTaskCreatePinnedToCore(send_task, "send tsk", 2048, (void *)&task_args, 10 + prior_mod, NULL, send_core);
                    xTaskCreatePinnedToCore(rec_task, "rec tsk", 2048, (void *)&task_args, 10, NULL, rec_core);
                    xSemaphoreTake(tasks_done, portMAX_DELAY);
                    vTaskDelay(5);  //Allow idle to clean up
                }
            }
        }

        //Delete ring buffer
        vRingbufferDelete(task_args.buffer);
        vTaskDelay(10);
    }
    cleanup();
}

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
TEST_CASE("Test static ring buffer SMP", "[esp_ringbuf][linux]")
{
    setup();
    //Iterate through buffer types (No split, split, then byte buff)
    for (RingbufferType_t buf_type = 0; buf_type < RINGBUF_TYPE_MAX; buf_type++) {
        StaticRingbuffer_t *buffer_struct;
        uint8_t *buffer_storage;
        //Allocate memory and create semaphores
#if CONFIG_SPIRAM_USE_CAPS_ALLOC   //When SPIRAM can only be allocated using heap_caps_malloc()
        buffer_struct = (StaticRingbuffer_t *)heap_caps_malloc(sizeof(StaticRingbuffer_t), MALLOC_CAP_SPIRAM);
        buffer_storage = (uint8_t *)heap_caps_malloc(sizeof(uint8_t) * CONT_DATA_TEST_BUFF_LEN, MALLOC_CAP_SPIRAM);
#else   //Case where SPIRAM is disabled or when SPIRAM is allocatable through malloc()
        buffer_struct = (StaticRingbuffer_t *)malloc(sizeof(StaticRingbuffer_t));
        buffer_storage = (uint8_t *)malloc(sizeof(uint8_t) * CONT_DATA_TEST_BUFF_LEN);
#endif
        TEST_ASSERT(buffer_struct != NULL && buffer_storage != NULL);

        //Create buffer
        task_args_t task_args;
        task_args.buffer = xRingbufferCreateStatic(CONT_DATA_TEST_BUFF_LEN, buf_type, buffer_storage, buffer_struct); //Create buffer of selected type
        task_args.type = buf_type;
        TEST_ASSERT_MESSAGE(task_args.buffer != NULL, "Failed to create ring buffer");

        for (int prior_mod = -1; prior_mod < 2; prior_mod++) {  //Test different relative priorities
            //Test every permutation of core affinity
            for (int send_core = 0; send_core < CONFIG_FREERTOS_NUMBER_OF_CORES; send_core++) {
                for (int rec_core = 0; rec_core < CONFIG_FREERTOS_NUMBER_OF_CORES; rec_core ++) {
                    esp_rom_printf("Type: %d, PM: %d, SC: %d, RC: %d\n", buf_type, prior_mod, send_core, rec_core);
                    xTaskCreatePinnedToCore(send_task, "send tsk", 2048, (void *)&task_args, 10 + prior_mod, NULL, send_core);
                    xTaskCreatePinnedToCore(rec_task, "rec tsk", 2048, (void *)&task_args, 10, NULL, rec_core);
                    xSemaphoreTake(tasks_done, portMAX_DELAY);
                    vTaskDelay(5);  //Allow idle to clean up
                }
            }
        }

        //Delete ring buffer
        vRingbufferDelete(task_args.buffer);

        //Deallocate memory
        free(buffer_storage);
        free(buffer_struct);
        vTaskDelay(10);
    }
    cleanup();
}
#endif

#endif //!CONFIG_FREERTOS_UNICORE

/* ------------------------ Test ring buffer 0 Item Size -----------------------
 * The following test case tests that sending/acquiring an item/bytes of 0 size
 * is permissible.
 */

TEST_CASE("Test ringbuffer 0 item size", "[esp_ringbuf][linux]")
{
    RingbufHandle_t no_split_rb = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_NOSPLIT);
    RingbufHandle_t allow_split_rb = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_ALLOWSPLIT);
    RingbufHandle_t byte_rb = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF);
    TEST_ASSERT_MESSAGE(no_split_rb && allow_split_rb && byte_rb, "Failed to create ring buffers");

    void *pvItem1;
    void *pvItem2;
    size_t xItemSize1;
    size_t xItemSize2;

    //Test that 0 item size on no split buffers should only send a header with no data
    TEST_ASSERT_EQUAL(pdTRUE, xRingbufferSend(no_split_rb, NULL, 0, 0));
    TEST_ASSERT_NOT_EQUAL(NULL, xRingbufferReceive(no_split_rb, &xItemSize1, 0));
    TEST_ASSERT_EQUAL(0, xItemSize1);

    //Test that acquiring 0 item size on no split buffers should only send a header without reserving a data buffer
    TEST_ASSERT_EQUAL(pdTRUE, xRingbufferSendAcquire(no_split_rb, &pvItem1, 0, 0));
    TEST_ASSERT_NOT_EQUAL(NULL, pvItem1);
    TEST_ASSERT_EQUAL(pdTRUE, xRingbufferSendComplete(no_split_rb, pvItem1));
    TEST_ASSERT_NOT_EQUAL(NULL, xRingbufferReceive(no_split_rb, &xItemSize1, 0));
    TEST_ASSERT_EQUAL(0, xItemSize1);

    //Test that 0 item size on allow split buffers should only send a header with no data
    TEST_ASSERT_EQUAL(pdTRUE, xRingbufferSend(allow_split_rb, NULL, 0, 0));
    TEST_ASSERT_EQUAL(pdTRUE, xRingbufferReceiveSplit(allow_split_rb, &pvItem1, &pvItem2, &xItemSize1, &xItemSize2, 0));
    TEST_ASSERT_NOT_EQUAL(NULL, pvItem1);
    TEST_ASSERT_EQUAL(NULL, pvItem2);
    TEST_ASSERT_EQUAL(0, xItemSize1);

    //Test that 0 item size on byte buffers should send nothing
    TEST_ASSERT_EQUAL(pdTRUE, xRingbufferSend(byte_rb, NULL, 0, 0));
    TEST_ASSERT_EQUAL(pdFALSE, xRingbufferReceiveUpTo(byte_rb, &xItemSize1, 0, BUFFER_SIZE));

    //Cleanup
    vRingbufferDelete(no_split_rb);
    vRingbufferDelete(allow_split_rb);
    vRingbufferDelete(byte_rb);
}

/* ---------------------------- Test no-split ring buffer SendAquire and SendComplete ---------------------------
 * The following test case tests the SendAquire and SendComplete functions of the no-split ring buffer.
 *
 * The test case will do the following...
 * 1) Create a no-split ring buffer.
 * 2) Acquire space on the buffer to send an item.
 * 3) Send the item to the buffer.
 * 4) Verify that the item is received correctly.
 * 5) Acquire space on the buffer until the buffer is full.
 * 6) Send the items out-of-order to the buffer.
 * 7) Verify that the items are not received until the first item is sent.
 * 8) Send the first item.
 * 9) Verify that the items are received in the correct order.
 */
TEST_CASE("Test no-split buffers always receive items in order", "[esp_ringbuf][linux]")
{
    // Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_NOSPLIT);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");

    // Acquire space on the buffer to send an item and write to the item
    void *item1;
    TEST_ASSERT_EQUAL(pdTRUE, xRingbufferSendAcquire(buffer_handle, &item1, MEDIUM_ITEM_SIZE, TIMEOUT_TICKS));
    *(uint32_t *)item1 = 0x123;

    // Send the item to the buffer
    TEST_ASSERT_EQUAL(pdTRUE, xRingbufferSendComplete(buffer_handle, item1));

    // Verify that the item is received correctly
    size_t item_size;
    uint32_t *received_item = xRingbufferReceive(buffer_handle, &item_size, TIMEOUT_TICKS);
    TEST_ASSERT_NOT_NULL(received_item);
    TEST_ASSERT_EQUAL(item_size, MEDIUM_ITEM_SIZE);
    TEST_ASSERT_EQUAL(*(uint32_t *)received_item, 0x123);

    // Return the space to the buffer after receiving the item
    vRingbufferReturnItem(buffer_handle, received_item);

    // At this point, the buffer should be empty
    UBaseType_t items_waiting;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, NULL, NULL, &items_waiting);
    TEST_ASSERT_MESSAGE(items_waiting == 0, "Incorrect items waiting");

    // Acquire space on the buffer until the buffer is full
#define MAX_NUM_ITEMS ( BUFFER_SIZE / ( MEDIUM_ITEM_SIZE + ITEM_HDR_SIZE ) )
    void *items[MAX_NUM_ITEMS];
    for (int i = 0; i < MAX_NUM_ITEMS; i++) {
        TEST_ASSERT_EQUAL(pdTRUE, xRingbufferSendAcquire(buffer_handle, &items[i], MEDIUM_ITEM_SIZE, TIMEOUT_TICKS));
        TEST_ASSERT_NOT_NULL(items[i]);
        *(uint32_t *)items[i] = (0x100 + i);
    }

    // Verify that the buffer is full by attempting to acquire space for another item
    void *another_item;
    TEST_ASSERT_EQUAL(pdFALSE, xRingbufferSendAcquire(buffer_handle, &another_item, MEDIUM_ITEM_SIZE, TIMEOUT_TICKS));

    // Send the items out-of-order to the buffer. Verify that the items are not received until the first item is sent.
    // In this case, we send the items in the reverse order until the first item is sent.
    for (int i = MAX_NUM_ITEMS - 1; i > 0; i--) {
        TEST_ASSERT_EQUAL(pdTRUE, xRingbufferSendComplete(buffer_handle, items[i]));
        TEST_ASSERT_NULL(xRingbufferReceive(buffer_handle, &item_size, 0));
    }

    // Send the first item
    TEST_ASSERT_EQUAL(pdTRUE, xRingbufferSendComplete(buffer_handle, items[0]));

    // Verify that the items are received in the correct order
    for (int i = 0; i < MAX_NUM_ITEMS; i++) {
        received_item = xRingbufferReceive(buffer_handle, &item_size, TIMEOUT_TICKS);
        TEST_ASSERT_EQUAL(*(uint32_t *)received_item, (0x100 + i));
        vRingbufferReturnItem(buffer_handle, received_item);
    }

    // Cleanup
    vRingbufferDelete(buffer_handle);
}
