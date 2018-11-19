#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "driver/timer.h"
#include "esp_spi_flash.h"
#include "unity.h"
#include "test_utils.h"

//Definitions used in multiple test cases
#define TIMEOUT_TICKS               10
#define NO_OF_RB_TYPES              3
#define ITEM_HDR_SIZE               8
#define SMALL_ITEM_SIZE             8
#define LARGE_ITEM_SIZE             (2 * SMALL_ITEM_SIZE)
#define BUFFER_SIZE                 160     //4Byte aligned size

static const uint8_t small_item[SMALL_ITEM_SIZE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
static const uint8_t large_item[LARGE_ITEM_SIZE] = { 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                                     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
static RingbufHandle_t buffer_handles[NO_OF_RB_TYPES];
static SemaphoreHandle_t done_sem;

static void send_item_and_check(RingbufHandle_t handle, const uint8_t *item,  size_t item_size, TickType_t ticks_to_wait, bool in_isr)
{
    BaseType_t ret;
    if (in_isr) {
        ret = xRingbufferSendFromISR(handle, (void *)item, item_size, NULL);
    } else {
        ret = xRingbufferSend(handle, (void *)item, item_size, ticks_to_wait);
    }
    TEST_ASSERT_MESSAGE(ret == pdTRUE, "Failed to send item");
}

static void receive_check_and_return_item_no_split(RingbufHandle_t handle, const uint8_t *expected_data, size_t expected_size, TickType_t ticks_to_wait, bool in_isr)
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

static void receive_check_and_return_item_allow_split(RingbufHandle_t handle, const uint8_t *expected_data, size_t expected_size, TickType_t ticks_to_wait, bool in_isr)
{
    //Receive item
    size_t item_size1, item_size2;
    uint8_t *item1, *item2;
    BaseType_t ret;
    if (in_isr) {
        ret = xRingbufferReceiveSplitFromISR(handle, (void**)&item1, (void **)&item2, &item_size1, &item_size2);
    } else {
        ret = xRingbufferReceiveSplit(handle, (void**)&item1, (void **)&item2, &item_size1, &item_size2, ticks_to_wait);
    }
    //= xRingbufferReceiveSplit(handle, (void**)&item1, (void **)&item2, &item_size1, &item_size2, ticks_to_wait);
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

static void receive_check_and_return_item_byte_buffer(RingbufHandle_t handle, const uint8_t *expected_data, size_t expected_size, TickType_t ticks_to_wait, bool in_isr)
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
 * The following test cases will test basic send, receive, and wrap around
 * behavior of each type of ring buffer. Each test case will do the following
 * 1) Send multiple items (nearly fill the buffer)
 * 2) Receive and check the sent items (also prepares the buffer for a wrap around
 * 3) Send a final item that causes a wrap around
 * 4) Receive and check the wrapped item
 */

TEST_CASE("Test ring buffer No-Split", "[freertos]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_NOSPLIT);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");
    //Calculate number of items to send. Aim to almost fill buffer to setup for wrap around
    int no_of_items = (BUFFER_SIZE - (ITEM_HDR_SIZE + SMALL_ITEM_SIZE)) / (ITEM_HDR_SIZE + SMALL_ITEM_SIZE);

    //Test sending items
    for (int i = 0; i < no_of_items; i++) {
        send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }
    //Test receiving items
    for (int i = 0; i < no_of_items; i++) {
        receive_check_and_return_item_no_split(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Write pointer should be near the end, test wrap around
    uint32_t write_pos_before, write_pos_after;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_before, NULL);
    //Send large item that causes wrap around
    send_item_and_check(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    //Receive wrapped item
    receive_check_and_return_item_no_split(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_after, NULL);
    TEST_ASSERT_MESSAGE(write_pos_after < write_pos_before, "Failed to wrap around");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

TEST_CASE("Test ring buffer Allow-Split", "[freertos]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_ALLOWSPLIT);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");
    //Calculate number of items to send. Aim to almost fill buffer to setup for wrap around
    int no_of_items = (BUFFER_SIZE - (ITEM_HDR_SIZE + SMALL_ITEM_SIZE)) / (ITEM_HDR_SIZE + SMALL_ITEM_SIZE);

    //Test sending items
    for (int i = 0; i < no_of_items; i++) {
        send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }
    //Test receiving items
    for (int i = 0; i < no_of_items; i++) {
        receive_check_and_return_item_allow_split(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Write pointer should be near the end, test wrap around
    uint32_t write_pos_before, write_pos_after;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_before, NULL);
    //Send large item that causes wrap around
    send_item_and_check(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    //Receive wrapped item
    receive_check_and_return_item_allow_split(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_after, NULL);
    TEST_ASSERT_MESSAGE(write_pos_after < write_pos_before, "Failed to wrap around");

    //Cleanup
    vRingbufferDelete(buffer_handle);
}

TEST_CASE("Test ring buffer Byte Buffer", "[freertos]")
{
    //Create buffer
    RingbufHandle_t buffer_handle = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF);
    TEST_ASSERT_MESSAGE(buffer_handle != NULL, "Failed to create ring buffer");
    //Calculate number of items to send. Aim to almost fill buffer to setup for wrap around
    int no_of_items = (BUFFER_SIZE - SMALL_ITEM_SIZE) / SMALL_ITEM_SIZE;

    //Test sending items
    for (int i = 0; i < no_of_items; i++) {
        send_item_and_check(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }
    //Test receiving items
    for (int i = 0; i < no_of_items; i++) {
        receive_check_and_return_item_byte_buffer(buffer_handle, small_item, SMALL_ITEM_SIZE, TIMEOUT_TICKS, false);
    }

    //Write pointer should be near the end, test wrap around
    uint32_t write_pos_before, write_pos_after;
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_before, NULL);
    //Send large item that causes wrap around
    send_item_and_check(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    //Receive wrapped item
    receive_check_and_return_item_byte_buffer(buffer_handle, large_item, LARGE_ITEM_SIZE, TIMEOUT_TICKS, false);
    vRingbufferGetInfo(buffer_handle, NULL, NULL, &write_pos_after, NULL);
    TEST_ASSERT_MESSAGE(write_pos_after < write_pos_before, "Failed to wrap around");

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
        xQueueSetMemberHandle member = xQueueSelectFromSet(queue_set, TIMEOUT_TICKS);
        //Read from selected ring buffer
        if (xRingbufferCanRead(buffer_handles[0], member) == pdTRUE) {
            //No-split buffer
            receive_check_and_return_item_no_split(buffer_handles[0], small_item, SMALL_ITEM_SIZE, 0, false);
            items_rec_count[0] ++;
        } else if (xRingbufferCanRead(buffer_handles[1], member) == pdTRUE) {
            //Allow-split buffer
            receive_check_and_return_item_allow_split(buffer_handles[1], small_item, SMALL_ITEM_SIZE, 0, false);
            items_rec_count[1] ++;
        } else if (xRingbufferCanRead(buffer_handles[2], member) == pdTRUE){
            //Byte buffer
            receive_check_and_return_item_byte_buffer(buffer_handles[2], small_item, SMALL_ITEM_SIZE, 0, false);
            items_rec_count[2] ++;
        } else {
            TEST_ASSERT_MESSAGE( false, "Error with queue set member");
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

TEST_CASE("Test ring buffer with queue sets", "[freertos]")
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
    xTaskCreatePinnedToCore(queue_set_receiving_task, "rec tsk", 2048, (void *)queue_set, UNITY_FREERTOS_PRIORITY + 1 , NULL, 0);

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
}

/* -------------------------- Test ring buffer ISR -----------------------------
 * The following test case tests ring buffer ISR API. A timer is used to trigger
 * the ISR. The test case will do the following
 * 1) ISR will be triggered periodically by timer
 * 2) The ISR will iterate through all ring buffer types where each iteration
 *    will send then receive an item to a ring buffer.
 */

#define TIMER_GROUP     0
#define TIMER_NUMBER    0
#define ISR_ITERATIONS  ((BUFFER_SIZE / SMALL_ITEM_SIZE) * 2)

intr_handle_t ringbuffer_isr_handle;
static int buf_type;
static int iterations;

static void ringbuffer_isr(void *arg)
{
    //Clear timer interrupt
    TIMERG0.int_clr_timers.t0 = 1;
    TIMERG0.hw_timer[xPortGetCoreID()].config.alarm_en = 1;

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
            return;
        } else {
            //Signal complete
            BaseType_t task_woken = pdFALSE;
            xSemaphoreGiveFromISR(done_sem, &task_woken);
            if (task_woken == pdTRUE) {
                buf_type++;
                portYIELD_FROM_ISR();
            }
        }
    }
}

static void setup_timer()
{
    //Setup timer for ISR
    int timer_group = TIMER_GROUP;
    int timer_idx = TIMER_NUMBER;
    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = 10000;
    config.intr_type = TIMER_INTR_LEVEL;
    config.counter_en = TIMER_PAUSE;
    timer_init(timer_group, timer_idx, &config);    //Configure timer
    timer_pause(timer_group, timer_idx);    //Stop timer counter
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL); //Load counter value
    timer_set_alarm_value(timer_group, timer_idx, 20); //Set alarm value
    timer_enable_intr(timer_group, timer_idx);  //Enable timer interrupt
    timer_set_auto_reload(timer_group, timer_idx, 1);   //Auto Reload
    timer_isr_register(timer_group, timer_idx, ringbuffer_isr, NULL, 0, &ringbuffer_isr_handle);    //Set ISR handler
}

static void cleanup_timer()
{
    timer_disable_intr(TIMER_GROUP, TIMER_NUMBER);
    esp_intr_free(ringbuffer_isr_handle);
}

TEST_CASE("Test ring buffer ISR", "[freertos]")
{
    for (int i = 0; i < NO_OF_RB_TYPES; i++) {
        buffer_handles[i] = xRingbufferCreate(BUFFER_SIZE, i);
    }
    done_sem = xSemaphoreCreateBinary();
    buf_type = 0;
    iterations = 0;
    setup_timer();
    //Start timer to trigger ISR
    timer_start(TIMER_GROUP, TIMER_NUMBER);
    //Wait for ISR to complete multiple iterations
    xSemaphoreTake(done_sem, portMAX_DELAY);

    //Cleanup
    cleanup_timer();
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

#define SRAND_SEED                      3   //Arbitrarily chosen srand() seed
#define SMP_TEST_ITERATIONS             4

static const char continuous_data[] = {"A_very_long_string_that_will_be_split_into_"
                                       "items_of_random_lengths_and_sent_to_the_ring_"
                                       "buffer._The_maximum_random_length_will_also_"
                                       "be_increased_over_multiple_iterations_in_this"
                                       "_test"};
#define CONT_DATA_LEN                   sizeof(continuous_data)
#define CONT_DATA_TEST_BUFF_LEN         (CONT_DATA_LEN/2)   //This will guarantee that the buffer must do a wrap around at some point

typedef struct {
    RingbufHandle_t buffer;
    ringbuf_type_t type;
} task_args_t;

static SemaphoreHandle_t tasks_done;
static SemaphoreHandle_t tx_done;
static SemaphoreHandle_t rx_done;

static void send_to_buffer(RingbufHandle_t buffer, size_t max_item_size)
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
            TEST_ASSERT_MESSAGE(xRingbufferSend(buffer, (void *)&(continuous_data[bytes_sent]), next_item_size, TIMEOUT_TICKS) == pdTRUE, "Failed to send an item");
            bytes_sent += next_item_size;
        }
        xSemaphoreGive(tx_done);
        xSemaphoreTake(rx_done, portMAX_DELAY);
    }
}

static void read_from_buffer(RingbufHandle_t buffer, ringbuf_type_t buf_type, size_t max_rec_size)
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

static void send_task(void *args)
{
    RingbufHandle_t buffer = ((task_args_t *)args)->buffer;
    size_t max_item_len = xRingbufferGetMaxItemSize(buffer);

    //Test sending short length items
    send_to_buffer(buffer, 1);
    //Test sending mid length items
    send_to_buffer(buffer, max_item_len/2);
    //Test sending long length items
    send_to_buffer(buffer, max_item_len);
    vTaskDelete(NULL);
}

static void rec_task(void *args)
{
    RingbufHandle_t buffer = ((task_args_t *)args)->buffer;
    size_t max_rec_len = xRingbufferGetMaxItemSize(buffer);

    //Test receiving short length items
    read_from_buffer(buffer, ((task_args_t *)args)->type, 1);
    //Test receiving mid length items
    read_from_buffer(buffer, ((task_args_t *)args)->type, max_rec_len/2);
    //Test receiving long length items
    read_from_buffer(buffer, ((task_args_t *)args)->type, max_rec_len);

    xSemaphoreGive(tasks_done);
    vTaskDelete(NULL);
}

TEST_CASE("Test ring buffer SMP", "[freertos]")
{
    ets_printf("size of buf %d\n", CONT_DATA_LEN);
    tx_done = xSemaphoreCreateBinary();                 //Semaphore to indicate send is done for a particular iteration
    rx_done = xSemaphoreCreateBinary();                 //Semaphore to indicate receive is done for a particular iteration
    tasks_done = xSemaphoreCreateBinary();                //Semaphore used to to indicate send and receive tasks completed running
    srand(SRAND_SEED);                                  //Seed RNG

    //Iterate through buffer types (No split, split, then byte buff)
    for (ringbuf_type_t buf_type = 0; buf_type <= RINGBUF_TYPE_BYTEBUF; buf_type++) {
        //Create buffer
        task_args_t task_args;
        task_args.buffer = xRingbufferCreate(CONT_DATA_TEST_BUFF_LEN, buf_type); //Create buffer of selected type
        task_args.type = buf_type;

        for (int prior_mod = -1; prior_mod < 2; prior_mod++) {  //Test different relative priorities
            //Test every permutation of core affinity
            for (int send_core = 0; send_core < portNUM_PROCESSORS; send_core++) {
                for (int rec_core = 0; rec_core < portNUM_PROCESSORS; rec_core ++) {
                    ets_printf("Type: %d, PM: %d, SC: %d, RC: %d\n", buf_type, prior_mod, send_core, rec_core);
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

    //Cleanup
    vSemaphoreDelete(tx_done);
    vSemaphoreDelete(rx_done);
    vSemaphoreDelete(tasks_done);
}

static IRAM_ATTR __attribute__((noinline)) bool iram_ringbuf_test()
{
    bool result = true;

    spi_flash_guard_get()->start(); // Disables flash cache
    RingbufHandle_t handle = xRingbufferCreate(CONT_DATA_TEST_BUFF_LEN, RINGBUF_TYPE_NOSPLIT);
    result = result && (handle != NULL);
    xRingbufferGetMaxItemSize(handle);
    vRingbufferDelete(handle);
    spi_flash_guard_get()->end(); // Re-enables flash cache

    return result;
}

TEST_CASE("Test ringbuffer functions work with flash cache disabled", "[freertos]")
{
    TEST_ASSERT( iram_ringbuf_test() );
}
