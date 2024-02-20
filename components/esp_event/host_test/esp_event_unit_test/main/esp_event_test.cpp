/* ESP Event Host-Based Test

   This code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "esp_event.h"

#include <catch2/catch_test_macros.hpp>

#include "fixtures.hpp"

extern "C" {
#include "Mocktask.h"
#include "Mockqueue.h"
}

namespace {

const uint32_t QUEUE_SIZE = 32;

static UBaseType_t s_test_priority;

static esp_event_loop_args_t test_event_get_default_loop_args(void)
{
    esp_event_loop_args_t loop_config = {
        .queue_size = QUEUE_SIZE, // TODO: CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE from esp_system should be used
        .task_name = "loop",
        .task_priority = s_test_priority,
        .task_stack_size = 2048,
        .task_core_id = 0
    };

    return loop_config;
}

void dummy_handler(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data) { }

}

// TODO: IDF-2693, function definition just to satisfy linker, implement esp_common instead
const char *esp_err_to_name(esp_err_t code)
{
    return "test";
}

TEST_CASE("create an event loop with any NULL argument fails")
{
    MockQueue queue(CreateAnd::IGNORE);
    MockMutex sem(CreateAnd::IGNORE);
    MockTask task(CreateAnd::IGNORE);
    esp_event_loop_handle_t loop; // with dedicated task
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
    CHECK(ESP_ERR_INVALID_ARG == esp_event_loop_create(NULL, &loop));
    CHECK(ESP_ERR_INVALID_ARG == esp_event_loop_create(&loop_args, NULL));
}

TEST_CASE("test esp_event_loop_create create_queue_fails(void)")
{
    MockQueue queue(CreateAnd::FAIL);
    MockMutex sem(CreateAnd::IGNORE);
    MockTask task(CreateAnd::IGNORE);
    esp_event_loop_handle_t loop;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    CHECK(ESP_ERR_NO_MEM == esp_event_loop_create(&loop_args, &loop));
}

TEST_CASE("test esp_event_loop_create create_mutex_fails(void)")
{
    MockQueue queue(CreateAnd::IGNORE);
    MockMutex sem(CreateAnd::FAIL);
    MockTask task(CreateAnd::IGNORE);
    esp_event_loop_handle_t loop;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    CHECK(ESP_ERR_NO_MEM == esp_event_loop_create(&loop_args, &loop));
}

TEST_CASE("test esp_event_loop_create create_task_fails(void)")
{
    MockQueue queue(CreateAnd::IGNORE);
    MockMutex sem(CreateAnd::IGNORE);
    MockTask task(CreateAnd::FAIL);
    esp_event_loop_handle_t loop;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();

    CHECK(ESP_FAIL == esp_event_loop_create(&loop_args, &loop));
}

TEST_CASE("test esp_event_loop_create no_task(void)")
{
    MockQueue queue(CreateAnd::IGNORE);
    MockMutex sem(CreateAnd::IGNORE);
    xQueueTakeMutexRecursive_IgnoreAndReturn(0);
    xQueueGiveMutexRecursive_IgnoreAndReturn(0);
    xQueueReceive_IgnoreAndReturn(0);
    esp_event_loop_handle_t loop = nullptr;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
    loop_args.task_name = nullptr;

    CHECK(ESP_OK == esp_event_loop_create(&loop_args, &loop));
    CHECK(loop != nullptr);

    CHECK(ESP_OK == esp_event_loop_delete(loop));

    xQueueReceive_StopIgnore();
    xQueueTakeMutexRecursive_StopIgnore();
    xQueueGiveMutexRecursive_StopIgnore();
}

TEST_CASE("test esp_event_loop_create with_task(void)")
{
    MockQueue queue(CreateAnd::IGNORE);
    MockMutex sem(CreateAnd::IGNORE);
    MockTask task(CreateAnd::SUCCEED);
    xQueueTakeMutexRecursive_IgnoreAndReturn(0);
    xQueueGiveMutexRecursive_IgnoreAndReturn(0);
    xQueueReceive_IgnoreAndReturn(0);
    esp_event_loop_handle_t loop = nullptr;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
    loop_args.task_name = "test";

    CHECK(ESP_OK == esp_event_loop_create(&loop_args, &loop));
    CHECK(loop != nullptr);

    CHECK(ESP_OK == esp_event_loop_delete(loop));

    xQueueReceive_StopIgnore();
    xQueueTakeMutexRecursive_StopIgnore();
    xQueueGiveMutexRecursive_StopIgnore();
}

TEST_CASE("registering with ANY_BASE but specific ID fails")
{
    esp_event_loop_handle_t loop = reinterpret_cast<esp_event_loop_handle_t>(1);
    CHECK(esp_event_handler_register_with(loop,
                                          ESP_EVENT_ANY_BASE,
                                          47,
                                          dummy_handler,
                                          nullptr) == ESP_ERR_INVALID_ARG);
}
