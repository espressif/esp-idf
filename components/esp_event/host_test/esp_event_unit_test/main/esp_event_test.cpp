/* ESP Event Host-Based Test

   This code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <deque>
#include <vector>
#include "esp_event.h"

#include <catch2/catch_test_macros.hpp>

#include "fixtures.hpp"

extern "C" {
#include "Mocktask.h"
#include "Mockqueue.h"
#include "Mockportmacro.h"
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

/*
 * Minimal in-memory queue backing the mocked FreeRTOS queue primitives.
 *
 * The event data tests below need a posted event to survive the round trip from
 * esp_event_post_to() to esp_event_loop_run(); the *_Ignore mocks used by the
 * other test cases in this file discard the payload instead. The size of a
 * queue item is learned from the xQueueGenericCreate() call, so this fake needs
 * no knowledge of the (private) esp_event_post_instance_t type.
 */
size_t s_queue_item_size = 0;
std::deque<std::vector<uint8_t> > s_queue_items;

QueueHandle_t fake_queue_create(const UBaseType_t, const UBaseType_t item_size, const uint8_t, int)
{
    s_queue_item_size = item_size;
    s_queue_items.clear();
    return reinterpret_cast<QueueHandle_t>(0xdeadbeef);
}

BaseType_t fake_queue_send(QueueHandle_t, const void* const item, TickType_t, const BaseType_t, int)
{
    const uint8_t* bytes = static_cast<const uint8_t*>(item);
    s_queue_items.push_back(std::vector<uint8_t>(bytes, bytes + s_queue_item_size));
    return pdTRUE;
}

BaseType_t fake_queue_receive(QueueHandle_t, void* const buffer, TickType_t, int)
{
    if (s_queue_items.empty()) {
        return pdFALSE;
    }
    memcpy(buffer, s_queue_items.front().data(), s_queue_item_size);
    s_queue_items.pop_front();
    return pdTRUE;
}

/*
 * Stand-in for an opaque handle type such as esp_netif_t. Only the first member
 * matters: it is what a caller passing the handle by value (instead of by
 * address) ends up delivering to the handler.
 */
struct fake_handle_obj {
    void* first_member;
    uint32_t magic;
};

void* const FIRST_MEMBER_SENTINEL = reinterpret_cast<void*>(0xA5A5A5A5);

esp_event_base_t const TEST_BASE = "TEST_BASE";
const int32_t TEST_EVENT_ID = 1;

void* s_delivered_handle = nullptr;

void capture_handle_handler(void*, esp_event_base_t, int32_t, void* event_data)
{
    // The documented contract for an event whose data is a handle: the handler
    // receives a pointer to the handle, so it dereferences once to get it.
    s_delivered_handle = *static_cast<void**>(event_data);
}

void install_queue_fakes(void)
{
    xQueueGenericCreate_Stub(fake_queue_create);
    xQueueGenericSend_Stub(fake_queue_send);
    xQueueReceive_Stub(fake_queue_receive);
    xQueueCreateMutex_IgnoreAndReturn(reinterpret_cast<QueueHandle_t>(0xfeedface));
    vQueueDelete_Ignore();
    xQueueTakeMutexRecursive_IgnoreAndReturn(pdTRUE);
    xQueueGiveMutexRecursive_IgnoreAndReturn(pdTRUE);
    xTaskGetCurrentTaskHandle_IgnoreAndReturn(reinterpret_cast<TaskHandle_t>(1));
    xTaskGetTickCount_IgnoreAndReturn(0);
    vPortEnterCritical_Ignore();
    vPortExitCritical_Ignore();
}

void remove_queue_fakes(void)
{
    xQueueCreateMutex_StopIgnore();
    vQueueDelete_StopIgnore();
    xQueueTakeMutexRecursive_StopIgnore();
    xQueueGiveMutexRecursive_StopIgnore();
    xTaskGetCurrentTaskHandle_StopIgnore();
    xTaskGetTickCount_StopIgnore();
    vPortEnterCritical_StopIgnore();
    vPortExitCritical_StopIgnore();
    s_queue_items.clear();
}

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
    vPortEnterCritical_Ignore();
    vPortExitCritical_Ignore();
    esp_event_loop_handle_t loop = nullptr;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
    loop_args.task_name = nullptr;

    CHECK(ESP_OK == esp_event_loop_create(&loop_args, &loop));
    CHECK(loop != nullptr);

    CHECK(ESP_OK == esp_event_loop_delete(loop));

    xQueueReceive_StopIgnore();
    xQueueTakeMutexRecursive_StopIgnore();
    xQueueGiveMutexRecursive_StopIgnore();
    vPortEnterCritical_StopIgnore();
    vPortExitCritical_StopIgnore();
}

TEST_CASE("test esp_event_loop_create with_task(void)")
{
    MockQueue queue(CreateAnd::IGNORE);
    MockMutex sem(CreateAnd::IGNORE);
    MockTask task(CreateAnd::SUCCEED);
    xQueueTakeMutexRecursive_IgnoreAndReturn(0);
    xQueueGiveMutexRecursive_IgnoreAndReturn(0);
    xQueueReceive_IgnoreAndReturn(0);
    vPortEnterCritical_Ignore();
    vPortExitCritical_Ignore();
    esp_event_loop_handle_t loop = nullptr;

    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
    loop_args.task_name = "test";

    CHECK(ESP_OK == esp_event_loop_create(&loop_args, &loop));
    CHECK(loop != nullptr);

    CHECK(ESP_OK == esp_event_loop_delete(loop));

    xQueueReceive_StopIgnore();
    xQueueTakeMutexRecursive_StopIgnore();
    xQueueGiveMutexRecursive_StopIgnore();
    vPortEnterCritical_StopIgnore();
    vPortExitCritical_StopIgnore();
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

/*
 * esp_event_post*() treats event_data as the *source address* of the payload and
 * copies event_data_size bytes out of it. Posting a handle therefore requires
 * passing the address of the handle variable, not the handle itself.
 *
 * Getting this wrong silently delivers the first sizeof(handle) bytes of the
 * pointed-to object to the handler, which then interprets them as a pointer.
 * See the NETIF_PPP_CONNECT_FAILED regression fixed alongside these tests.
 */
TEST_CASE("posting a handle by address delivers the handle to the handler")
{
    CMOCK_SETUP();
    install_queue_fakes();

    fake_handle_obj obj = { FIRST_MEMBER_SENTINEL, 0x600DF00D };
    fake_handle_obj* handle = &obj;
    s_delivered_handle = nullptr;

    esp_event_loop_handle_t loop = nullptr;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
    loop_args.task_name = nullptr; // no dedicated task, run the loop inline

    REQUIRE(ESP_OK == esp_event_loop_create(&loop_args, &loop));
    REQUIRE(ESP_OK == esp_event_handler_register_with(loop, TEST_BASE, TEST_EVENT_ID,
                                                      capture_handle_handler, nullptr));

    // Correct usage: pass the address of the handle variable.
    REQUIRE(ESP_OK == esp_event_post_to(loop, TEST_BASE, TEST_EVENT_ID,
                                        &handle, sizeof(handle), 0));
    CHECK(ESP_OK == esp_event_loop_run(loop, 0));

    // The handler must receive the handle itself...
    CHECK(s_delivered_handle == handle);
    // ...and it must be safe to dereference, unlike the byte pattern the
    // incorrect form would have produced.
    CHECK(static_cast<fake_handle_obj*>(s_delivered_handle)->magic == 0x600DF00D);

    CHECK(ESP_OK == esp_event_loop_delete(loop));
    remove_queue_fakes();
}

TEST_CASE("posting a handle by value delivers the object's first member instead")
{
    CMOCK_SETUP();
    install_queue_fakes();

    fake_handle_obj obj = { FIRST_MEMBER_SENTINEL, 0x600DF00D };
    fake_handle_obj* handle = &obj;
    s_delivered_handle = nullptr;

    esp_event_loop_handle_t loop = nullptr;
    esp_event_loop_args_t loop_args = test_event_get_default_loop_args();
    loop_args.task_name = nullptr;

    REQUIRE(ESP_OK == esp_event_loop_create(&loop_args, &loop));
    REQUIRE(ESP_OK == esp_event_handler_register_with(loop, TEST_BASE, TEST_EVENT_ID,
                                                      capture_handle_handler, nullptr));

    // Incorrect usage, kept here to pin down the failure mode: the handle is
    // passed by value, so sizeof(handle) bytes are copied *from the object*.
    REQUIRE(ESP_OK == esp_event_post_to(loop, TEST_BASE, TEST_EVENT_ID,
                                        handle, sizeof(handle), 0));
    CHECK(ESP_OK == esp_event_loop_run(loop, 0));

    // The handler gets obj.first_member reinterpreted as a handle - garbage.
    CHECK(s_delivered_handle == FIRST_MEMBER_SENTINEL);
    CHECK(s_delivered_handle != handle);

    CHECK(ESP_OK == esp_event_loop_delete(loop));
    remove_queue_fakes();
}
