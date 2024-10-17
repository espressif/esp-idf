/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/ringbuf.h"

//Definitions used in multiple test cases
#define NO_OF_RB_TYPES              3
#define SMALL_ITEM_SIZE             8
#define ITEM_HDR_SIZE               (sizeof(size_t) + sizeof(UBaseType_t))
#define MEDIUM_ITEM_SIZE            ((3 * SMALL_ITEM_SIZE) >> 1)  //12 bytes
#define BUFFER_SIZE                 4 * (ITEM_HDR_SIZE + MEDIUM_ITEM_SIZE)     //4Byte aligned size

#define CONT_DATA_LEN                   continuous_test_string_len()
//32-bit aligned size that guarantees a wrap around at some point
#define CONT_DATA_TEST_BUFF_LEN         (((CONT_DATA_LEN/2) + 0x03) & ~0x3)

typedef struct {
    RingbufHandle_t buffer;
    RingbufferType_t type;
} task_args_t;

extern const uint8_t small_item[SMALL_ITEM_SIZE];
extern RingbufHandle_t buffer_handles[NO_OF_RB_TYPES];
extern SemaphoreHandle_t done_sem;

extern SemaphoreHandle_t tasks_done;
extern SemaphoreHandle_t tx_done;
extern SemaphoreHandle_t rx_done;

void setup(void);

void cleanup(void);

void read_from_buffer(RingbufHandle_t buffer, RingbufferType_t buf_type, size_t max_rec_size);

void send_to_buffer(RingbufHandle_t buffer, size_t max_item_size);

void send_item_and_check(RingbufHandle_t handle, const uint8_t *item,  size_t item_size, TickType_t ticks_to_wait, bool in_isr);

void receive_check_and_return_item_no_split(RingbufHandle_t handle, const uint8_t *expected_data, size_t expected_size, TickType_t ticks_to_wait, bool in_isr);

void receive_check_and_return_item_allow_split(RingbufHandle_t handle, const uint8_t *expected_data, size_t expected_size, TickType_t ticks_to_wait, bool in_isr);

void receive_check_and_return_item_byte_buffer(RingbufHandle_t handle, const uint8_t *expected_data, size_t expected_size, TickType_t ticks_to_wait, bool in_isr);

void send_task(void *args);

void rec_task(void *args);

size_t continuous_test_string_len(void);
