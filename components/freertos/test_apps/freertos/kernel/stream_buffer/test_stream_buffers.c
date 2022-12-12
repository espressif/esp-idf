/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/stream_buffer.h"
#include "freertos/message_buffer.h"
#include "unity.h"
#include "test_utils.h"

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2, ESP32C3)
typedef struct {
    StreamBufferHandle_t sb;
    SemaphoreHandle_t end_test;
    bool send_fail;
    bool receive_fail;
    bool produce_isr;
}test_context;

static void producer_task(void *arg)
{
    test_context *tc  = arg;
    uint8_t produced = 0;
    printf("Starting sender task... \n");

    while(produced < 100) {

        if(!tc->produce_isr) {
            BaseType_t result = xStreamBufferSend(tc->sb, &produced, 1, 0);
            if(!result) {
                tc->send_fail = true;
                xSemaphoreGive(tc->end_test);
                vTaskDelete(NULL);
            } else {
                produced++;
            }
        }

        vTaskDelay(1);
    }

    tc->send_fail = false;
    vTaskDelete(NULL);
}

static void receiver_task(void *arg)
{
    test_context *tc = arg;
    uint8_t expected_consumed = 0;
    printf("Starting receiver task... \n");

    for(;;){
         uint8_t read_byte = 0xFF;
         uint32_t result = xStreamBufferReceive(tc->sb, &read_byte, 1, 1000);

         if((read_byte != expected_consumed) || !result) {
            tc->receive_fail = true;
            xSemaphoreGive(tc->end_test);
            vTaskDelete(NULL);
         } else {
             expected_consumed++;
             if(expected_consumed == 99) {
                 break;
             }
         }
    }

    tc->receive_fail = false;
    xSemaphoreGive(tc->end_test);
    vTaskDelete(NULL);
}

TEST_CASE("Send-receive stream buffer test", "[freertos]")
{
    BaseType_t result;
    test_context tc;

    tc.sb = xStreamBufferCreate(128, 1);
    tc.end_test = xSemaphoreCreateBinary();

    TEST_ASSERT(tc.sb);
    TEST_ASSERT(tc.end_test);

    tc.send_fail = false;
    tc.receive_fail = false;
    tc.produce_isr = false;

    result = xTaskCreatePinnedToCore(producer_task, "sender", 4096, &tc, UNITY_FREERTOS_PRIORITY + 2, NULL, 0);
    TEST_ASSERT(result == pdTRUE);
    result = xTaskCreatePinnedToCore(receiver_task, "receiver", 4096, &tc, UNITY_FREERTOS_PRIORITY + 1, NULL, 1);
    TEST_ASSERT(result == pdTRUE);

    result = xSemaphoreTake(tc.end_test, 2000);
    TEST_ASSERT(result == pdTRUE);

    vTaskDelay(1);

    TEST_ASSERT(tc.send_fail == false);
    TEST_ASSERT(tc.receive_fail == false);

    vStreamBufferDelete(tc.sb);
    vSemaphoreDelete(tc.end_test);
}
#endif
