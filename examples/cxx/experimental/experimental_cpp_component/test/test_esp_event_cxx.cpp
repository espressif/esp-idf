#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>
#include <memory>
#include "unity.h"
#include "unity_cxx.hpp"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_event_cxx.hpp"
#include "esp_event_api.hpp"

#include "esp_exception.hpp"

#ifdef __cpp_exceptions

#ifdef CONFIG_ESP_TIMER_PROFILING
#define WITH_PROFILING 1
#endif

using namespace idf::event;
using namespace std;

ESP_EVENT_DEFINE_BASE(TEST_EVENT_BASE_0);
ESP_EVENT_DEFINE_BASE(TEST_EVENT_BASE_1);
static ESPEventID TEST_EVENT_ID_0(0);
static ESPEventID TEST_EVENT_ID_1(1);

#define TAG "Event CXX Test"

ESPEvent TEMPLATE_EVENT_0(TEST_EVENT_BASE_0, TEST_EVENT_ID_0);
ESPEvent TEMPLATE_EVENT_1(TEST_EVENT_BASE_0, TEST_EVENT_ID_1);

/**
 * Mock which only returns a certain error message.
 */
class ESPEventMock : public ESPEventAPIDefault {
public:
    esp_err_t next_error;

    esp_err_t handler_register(esp_event_base_t event_base,
            int32_t event_id,
            esp_event_handler_t event_handler,
            void* event_handler_arg,
            esp_event_handler_instance_t *instance) override {
        return next_error;
    }

    esp_err_t handler_unregister(esp_event_base_t event_base,
            int32_t event_id,
            esp_event_handler_instance_t instance) override {
        return next_error;
    }

    esp_err_t post(esp_event_base_t event_base,
            int32_t event_id,
            void* event_data,
            size_t event_data_size,
            TickType_t ticks_to_wait) override {
        return next_error;
    }
};

/* The initial logging "initializing test" is to ensure mutex allocation is not counted against memory not being freed
 * during teardown.
 * esp_event_loop_delete_default() tries to mitigate side effects of failed tests where objects
 * with automatic storage duration weren't destructed.
 *
 * TODO: The final "testing mem..." is to prevent memory leaks which occur for yet unknown reasons
 */
struct EventFixture {
    EventFixture() : free_mem_before(0) {
        ESP_LOGI(TAG, "initializing test");

        esp_event_loop_delete_default();

        free_mem_before = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    }

    virtual ~EventFixture()
    {
        ESP_LOGI(TAG, "de-initializing test...");
    }

    size_t free_mem_before;
};

struct EventLoopFix : public EventFixture {
    EventLoopFix()
        : EventFixture(),
        api(new ESPEventAPIDefault()),
        event_loop(api),
        ev0_called(false),
        ev1_called(false),
        timeout(false),
        ev0(),
        ev1()
    {
        handler0 = [this](const ESPEvent& ev, const void* data) {
            ev0 = ev;
            ev0_called = true;
        };

        handler1 = [this](const ESPEvent& ev, const void* data) {
            ev1 = ev;
            ev1_called = true;
        };

        timer_cb = [this](const ESPEvent& ev) {
            timeout_event = ev;
            timeout = true;
        };
    }

    std::function<void(const ESPEvent &, const void* data)> handler0;

    std::function<void(const ESPEvent &, const void* data)> handler1;

    std::function<void(const ESPEvent &)> timer_cb;

    std::shared_ptr<ESPEventAPI> api;
    ESPEventLoop event_loop;
    bool ev0_called;
    bool ev1_called;
    bool timeout;
    ESPEvent ev0;
    ESPEvent ev1;
    ESPEvent timeout_event;
};

void send_default_event(ESPEventID event_id = TEST_EVENT_ID_0) {
    TEST_ASSERT_EQUAL(ESP_OK, esp_event_post(TEST_EVENT_BASE_0,
            event_id.get_id(),
            nullptr,
            0,
            portMAX_DELAY));
}

TEST_CASE("ESPEventAPIDefault deinitialization without failure", "[cxx event]")
{
    EventFixture f;
    std::shared_ptr<ESPEventAPI> api(new ESPEventAPIDefault());
    esp_event_loop_delete_default();

    // destructor of ESPEventAPI needs to run without failure
}

TEST_CASE("ESPEventReg cb nullptr", "[cxx event]")
{
    EventFixture f;
    std::shared_ptr<ESPEventAPI> api(new ESPEventAPIDefault());
    ESPEventLoop event_loop(api);
    TEST_THROW(ESPEventReg reg(nullptr, TEMPLATE_EVENT_0, api), EventException);
}

TEST_CASE("ESPEventReg api nullptr", "[cxx event]")
{
    EventFixture f;
    function<void(const ESPEvent &, const void *)> cb = [](const ESPEvent &event, const void *data) {};
    shared_ptr<ESPEventAPI> api(new ESPEventAPIDefault());
    ESPEventLoop event_loop(api);
    TEST_THROW(ESPEventReg reg(cb, TEMPLATE_EVENT_0, nullptr), EventException);
}

TEST_CASE("ESPEventReg event api not initialized", "[cxx event]")
{
    EventFixture f;

    std::shared_ptr<ESPEventMock> api(new ESPEventMock());
    api->next_error = ESP_ERR_INVALID_STATE;
    TEST_THROW(ESPEventReg cb([](const ESPEvent &, const void* data) { }, TEMPLATE_EVENT_0, api),
            ESPEventRegisterException);
}

TEST_CASE("ESPEventReg event register failure no loop initialized", "[cxx event]")
{
    EventFixture f;
    // registering will fail because default event loop isn't initialized
    std::shared_ptr<ESPEventAPI> api(new ESPEventAPIDefault());
    esp_event_loop_delete_default();
    TEST_THROW(ESPEventReg cb([](const ESPEvent &, const void* data) { }, TEMPLATE_EVENT_0, api),
            ESPEventRegisterException);
}

TEST_CASE("ESPEventReg initialization failure", "[cxx event]")
{
    ESPEvent event;
    EventFixture f;
    std::shared_ptr<ESPEventAPI> api = std::make_shared<ESPEventAPIDefault>();

    TEST_THROW(ESPEventReg([&](const ESPEvent &ev, const void*) { event = ev; }, ESPEvent(), api),
            ESPEventRegisterException);
}

TEST_CASE("ESPEventReg registration success", "[cxx event]")
{
    ESPEvent event;
    EventFixture f;
    std::shared_ptr<ESPEventAPI> api = std::make_shared<ESPEventAPIDefault>();
    ESPEventLoop loop(api);

    ESPEventReg registration([&event](const ESPEvent &ev, const void *) { event = ev; }, TEMPLATE_EVENT_0, api);
    send_default_event();

    TEST_ASSERT(event == TEMPLATE_EVENT_0);
}

TEST_CASE("ESPEventLoopCB event passes data", "[cxx event]")
{
    EventLoopFix fix;
    int data_sent = 47;
    int data_received = 0;

    ESPEvent event;

    ESPEventReg cb([&event, &data_received](const ESPEvent & ev, const void* data) {
        event = ev;
        data_received = *((int*) data);
    }, TEMPLATE_EVENT_0, fix.api);

    fix.event_loop.post_event_data(ESPEvent(TEST_EVENT_BASE_0, TEST_EVENT_ID_0), data_sent);

    TEST_ASSERT(TEMPLATE_EVENT_0 == event);
    TEST_ASSERT(data_sent == data_received);
}

TEST_CASE("ESPEventLoop Create event loop failure", "[cxx event]")
{
    EventFixture f;
    esp_event_loop_create_default();

    TEST_THROW(ESPEventLoop event_loop, EventException);

    // just in case
    esp_event_loop_delete_default();
}

TEST_CASE("ESPEventLoop registration invalid event callback", "[cxx event]")
{
    EventFixture f;

    ESPEventLoop event_loop;
    std::function<void(const ESPEvent &, const void *)> event_cb;
    TEST_THROW(event_loop.register_event(TEMPLATE_EVENT_0, event_cb), EventException);
}

TEST_CASE("ESPEventLoop timed registration invalid event callback", "[cxx event]")
{
    EventFixture f;

    ESPEventLoop event_loop;
    std::function<void(const ESPEvent &, const void *)> event_cb;
    std::function<void(const ESPEvent &)> timer_cb = [](const ESPEvent &ev) { };
    TEST_THROW(event_loop.register_event_timed(TEMPLATE_EVENT_0, event_cb, std::chrono::microseconds(10), timer_cb),
            EventException);
}

TEST_CASE("ESPEventLoop timed registration invalid timeout callback", "[cxx event]")
{
    EventFixture f;

    ESPEventLoop event_loop;
    std::function<void(const ESPEvent &, const void *)> event_cb = [](const ESPEvent &ev, const void *data) { };
    std::function<void(const ESPEvent &)> timer_cb;
    TEST_THROW(event_loop.register_event_timed(TEMPLATE_EVENT_0, event_cb, std::chrono::microseconds(10), timer_cb),
            EventException);
}

TEST_CASE("ESPEventLoop make sure timeout is off after register exception", "[cxx event]")
{
    EventFixture f;
    ESPEvent timeout_event;
    bool timeout = false;

    ESPEventLoop event_loop;
    std::function<void(const ESPEvent &, const void *)> event_cb = [&](const ESPEvent &ev, const void *data) {
        timeout_event = ev;
    };
    std::function<void(const ESPEvent &)> timer_cb = [&](const ESPEvent& ev) {
        timeout_event = ev;
        timeout = true;
    };
    esp_event_loop_delete_default();

    // Below ~35 microseconds the timer expires too fast for esp_timer_stop() to prevent it from being called.
    TEST_THROW(event_loop.register_event_timed(TEMPLATE_EVENT_0, event_cb, std::chrono::microseconds(40), timer_cb),
            ESPEventRegisterException);

    TEST_ASSERT_EQUAL(false, timeout);
    TEST_ASSERT(timeout_event == ESPEvent());
}

TEST_CASE("ESPEventLoop Delete event loop failure - no error", "[cxx event]")
{
    EventFixture f;

    ESPEventLoop event_loop;

    esp_event_loop_delete_default();

    // destructor of ESPEventLoop needs to run without failure
}

TEST_CASE("ESPEventLoop post nullptr event without registrations", "[cxx event]")
{
    EventFixture f;

    ESPEventLoop event_loop;
    ESPEvent event(TEST_EVENT_BASE_0, TEST_EVENT_ID_0);
    void *ptr = nullptr;
    event_loop.post_event_data(event, ptr);
}

TEST_CASE("ESPEventLoop post int event without registrations", "[cxx event]")
{
    EventFixture f;

    ESPEventLoop event_loop;
    ESPEvent event(TEST_EVENT_BASE_0, TEST_EVENT_ID_0);
    int fourtyseven = 47;
    event_loop.post_event_data(event, fourtyseven);
}

TEST_CASE("ESPEventLoop can create, use and delete ESPEventLoop", "[cxx event]")
{
    EventLoopFix fix;
    bool tested = false;

    std::function<void(const ESPEvent &, const void* data)> cb = [&tested](const ESPEvent& event, const void* data) {
        tested = true;
    };

    ESPEventReg registration(fix.handler0, TEMPLATE_EVENT_0, fix.api);

    void *ptr = nullptr;
    fix.event_loop.post_event_data(ESPEvent(TEST_EVENT_BASE_0, TEST_EVENT_ID_0), ptr);

    TEST_ASSERT_EQUAL(true, fix.ev0_called);
}

TEST_CASE("ESPEventLoop Register, receive, unregister ESPEvent", "[cxx event]")
{
    EventLoopFix fix;

    std::unique_ptr<ESPEventReg> registration(new ESPEventReg(fix.handler0, TEMPLATE_EVENT_0, fix.api));

    send_default_event();

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_0);

    registration.reset();
    fix.ev0 = ESPEvent();

    send_default_event();

    TEST_ASSERT(fix.ev0 == ESPEvent());
}

TEST_CASE("ESPEventLoop register multiple ESPEvents, same cb", "[cxx event]")
{
    EventLoopFix fix;

    ESPEventReg registration0(fix.handler0, TEMPLATE_EVENT_0, fix.api);
    ESPEventReg registration1(fix.handler1, TEMPLATE_EVENT_1, fix.api);

    send_default_event();

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_0);
    fix.ev0 = ESPEvent();

    send_default_event(TEST_EVENT_ID_1);

    TEST_ASSERT(fix.ev1 == TEMPLATE_EVENT_1);
}

TEST_CASE("ESPEventLoop register multiple ESPEvents, multiple cbs", "[cxx event]")
{
    EventLoopFix fix;

    ESPEventReg registration0(fix.handler0, TEMPLATE_EVENT_0, fix.api);
    ESPEventReg registration1(fix.handler1, TEMPLATE_EVENT_1, fix.api);

    send_default_event();
    send_default_event(TEST_EVENT_ID_1);

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_0);
    TEST_ASSERT(fix.ev1 == TEMPLATE_EVENT_1);
}

TEST_CASE("ESPEventLoop register to all events of one event base", "[cxx event]")
{
    EventLoopFix fix;

    ESPEvent any_id_event(ESP_EVENT_ANY_BASE, ESPEventID(ESP_EVENT_ANY_ID));
    ESPEventReg registration(fix.handler0, any_id_event, fix.api);

    send_default_event();

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_0);

    send_default_event(TEST_EVENT_ID_1);

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_1);
}

TEST_CASE("ESPEventLoop register to all ESP events", "[cxx event]")
{
    EventLoopFix fix;

    ESPEvent any_event(ESP_EVENT_ANY_BASE, ESPEventID(ESP_EVENT_ANY_ID));
    ESPEventReg registration(fix.handler0, any_event, fix.api);

    send_default_event();

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_0);

    send_default_event(TEST_EVENT_ID_1);

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_1);

    void *ptr = nullptr;
    fix.event_loop.post_event_data(ESPEvent(TEST_EVENT_BASE_1, TEST_EVENT_ID_0), ptr);

    // check reception of event with different base
    TEST_ASSERT_EQUAL(TEST_EVENT_BASE_1, fix.ev0.base);
    TEST_ASSERT_EQUAL(TEST_EVENT_ID_0.get_id(), fix.ev0.id.get_id());
}

TEST_CASE("ESPEventLoop direct register, receive, unregister ESPEvent", "[cxx event]")
{
    EventLoopFix fix;

    std::unique_ptr<ESPEventReg> registration = fix.event_loop.register_event(TEMPLATE_EVENT_0, fix.handler0);

    send_default_event();

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_0);

    registration.reset();
    fix.ev0 = ESPEvent();

    send_default_event();

    TEST_ASSERT(fix.ev0 == ESPEvent());
}

TEST_CASE("ESPEventLoop set timeout invalid timeout", "[cxx event]")
{
    EventLoopFix fix;

    const std::chrono::microseconds INVALID_US(MIN_TIMEOUT - chrono::microseconds(1));

    TEST_THROW(ESPEventRegTimed(fix.handler0, TEMPLATE_EVENT_0, fix.timer_cb, INVALID_US, fix.api),
            EventException);
}

TEST_CASE("ESPEventLoop lonely timeout", "[cxx event]")
{
    EventLoopFix fix;

    ESPEventRegTimed timed_reg(fix.handler0, TEMPLATE_EVENT_0, fix.timer_cb, MIN_TIMEOUT, fix.api);

    vTaskDelay(10 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(true, fix.timeout);
    TEST_ASSERT_EQUAL(false, fix.ev0_called);
}

TEST_CASE("ESPEventLoop timeout unregisters from loop", "[cxx event]")
{
    EventLoopFix fix;

    ESPEventRegTimed timed_reg(fix.handler0, TEMPLATE_EVENT_0, fix.timer_cb, MIN_TIMEOUT, fix.api);

    vTaskDelay(10 / portTICK_PERIOD_MS);

    send_default_event(TEST_EVENT_ID_0);

    TEST_ASSERT_EQUAL(true, fix.timeout);
    TEST_ASSERT_EQUAL(false, fix.ev0_called);
}

TEST_CASE("ESPEventLoop no timeout", "[cxx event]")
{
    EventLoopFix fix;

    ESPEventRegTimed timed_reg(fix.handler0, TEMPLATE_EVENT_0, fix.timer_cb, std::chrono::microseconds(500000), fix.api);

    vTaskDelay(10 / portTICK_PERIOD_MS);
    send_default_event();

    TEST_ASSERT_EQUAL(false, fix.timeout);
    TEST_ASSERT_EQUAL(true, fix.ev0_called);
}


/**
 * Registers an event via both set_timeout() and register_event().
 * Result: both handlers will be invoked, the timeout callback won't be called.
 */
TEST_CASE("ESPEventLoop register timeout and event - no timeout", "[cxx event]")
{
    EventLoopFix fix;

    ESPEventReg reg(fix.handler0, TEMPLATE_EVENT_0, fix.api);
    ESPEventRegTimed timed_reg(fix.handler1, TEMPLATE_EVENT_0, fix.timer_cb, std::chrono::microseconds(500000), fix.api);

    send_default_event();

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_0);
    TEST_ASSERT(fix.ev1 == TEMPLATE_EVENT_0);
    TEST_ASSERT_EQUAL(false, fix.timeout);
}

/**
 * Registers an event via both set_timeout() and register_event().
 * Result: both handlers will be invoked, the timeout callback won't be called.
 */
TEST_CASE("ESPEventLoop direct register timeout and event - no timeout", "[cxx event]")
{
    EventLoopFix fix;

    unique_ptr<ESPEventReg> reg = fix.event_loop.register_event(TEMPLATE_EVENT_0, fix.handler0);
    unique_ptr<ESPEventRegTimed> timed_reg = fix.event_loop.register_event_timed(TEMPLATE_EVENT_0,
            fix.handler1,
            std::chrono::microseconds(500000),
            fix.timer_cb);

    send_default_event();

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_0);
    TEST_ASSERT(fix.ev1 == TEMPLATE_EVENT_0);
    TEST_ASSERT_EQUAL(false, fix.timeout);
}

/**
 * Registers an event via both set_timeout() and register_event().
 * Result: both handlers will be invoked, the timeout callback won't be called.
 */
TEST_CASE("ESPEventLoop register timeout and event - timeout", "[cxx event]")
{
    EventLoopFix fix;

    ESPEventReg reg(fix.handler0, TEMPLATE_EVENT_0, fix.api);
    ESPEventRegTimed timed_reg(fix.handler1, TEMPLATE_EVENT_0, fix.timer_cb, MIN_TIMEOUT, fix.api);

    vTaskDelay(10 / portTICK_PERIOD_MS);
    send_default_event();

    TEST_ASSERT(fix.ev0 == TEMPLATE_EVENT_0);
    TEST_ASSERT_EQUAL(false, fix.ev1_called);
    TEST_ASSERT(fix.timeout_event == TEMPLATE_EVENT_0);
    TEST_ASSERT_EQUAL(true, fix.timeout);
}

TEST_CASE("ESPEventLoop custom loop register, receive, unregister ESPEvent", "[cxx event]")
{
    EventFixture f;
    ESPEvent event;
    esp_event_loop_args_t loop_args;
    loop_args.queue_size = 32;
    loop_args.task_name = "sys_evt";
    loop_args.task_stack_size = 2304;
    loop_args.task_priority = 20;
    loop_args.task_core_id = 0;

    std::shared_ptr<ESPEventAPICustom> api(new ESPEventAPICustom(loop_args));

    ESPEventLoop event_loop(api);
    std::function<void(const ESPEvent &, const void* data)> cb = [&event](const ESPEvent& ev, const void* data) {
        event = ev;
    };
    shared_ptr<ESPEventReg> registration = event_loop.register_event(TEMPLATE_EVENT_0, cb);

    void *ptr = nullptr;
    event_loop.post_event_data(ESPEvent(TEST_EVENT_BASE_0, TEST_EVENT_ID_0), ptr);

    ESP_ERROR_CHECK(api->run(1));

    TEST_ASSERT(event == TEMPLATE_EVENT_0);

    registration.reset();
    event = ESPEvent();

    event_loop.post_event_data(ESPEvent(TEST_EVENT_BASE_0, TEST_EVENT_ID_0), ptr);

    ESP_ERROR_CHECK(api->run(1));

    TEST_ASSERT(event == ESPEvent());
}

TEST_CASE("ESPEventHandlerSync simple construction and destruction", "[cxx event]")
{
    EventFixture f;
    ESPEventHandlerSync handler(make_shared<ESPEventLoop>());
    handler.listen_to(TEMPLATE_EVENT_0);
}

TEST_CASE("ESPEventHandlerSync simple event wait", "[cxx event]")
{
    EventFixture f;
    ESPEventHandlerSync handler(make_shared<ESPEventLoop>());
    handler.listen_to(TEMPLATE_EVENT_0);

    send_default_event();

    ESPEventHandlerSync::EventResult result = handler.wait_event();

    TEST_ASSERT_EQUAL(TEMPLATE_EVENT_0.base, result.event.base);
    TEST_ASSERT_EQUAL(TEMPLATE_EVENT_0.id.get_id(), result.event.id.get_id());
}

TEST_CASE("ESPEventHandlerSync wait_for(0) succeed", "[cxx event]")
{
    EventFixture f;
    ESPEventHandlerSync handler(make_shared<ESPEventLoop>());
    handler.listen_to(TEMPLATE_EVENT_0);

    send_default_event();

    ESPEventHandlerSync::EventResult result = handler.wait_event_for(chrono::milliseconds(0));
    TEST_ASSERT(TEMPLATE_EVENT_0 == result.event);
}

TEST_CASE("ESPEventHandlerSync start waiting after events arrived", "[cxx event]")
{
    EventFixture f;
    ESPEventHandlerSync handler(make_shared<ESPEventLoop>());
    handler.listen_to(TEMPLATE_EVENT_0);

    send_default_event();
    send_default_event();

    TEST_ASSERT(handler.wait_event().event == TEMPLATE_EVENT_0);
    TEST_ASSERT(handler.wait_event().event == TEMPLATE_EVENT_0);
}

// helper function to post events, simulating an event source
void post_events(int event_num) {
    for (int i = 0; i < event_num; i++) {
        ESP_ERROR_CHECK(esp_event_post(TEST_EVENT_BASE_0,
                TEST_EVENT_ID_0.get_id(),
                nullptr,
                0,
                portMAX_DELAY));
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

TEST_CASE("ESPEventHandlerSync simultaneous event handling", "[cxx event]")
{
    EventFixture f;
    // Create handler with queue size 1
    ESPEventHandlerSync handler(make_shared<ESPEventLoop>());
    handler.listen_to(TEMPLATE_EVENT_0);
    thread th(post_events, 3);

    // no for-loop for better feedback (line numbers)
    TEST_ASSERT(handler.wait_event().event == TEMPLATE_EVENT_0);
    TEST_ASSERT(handler.wait_event().event == TEMPLATE_EVENT_0);
    TEST_ASSERT(handler.wait_event().event == TEMPLATE_EVENT_0);

    TEST_ASSERT_EQUAL(0, handler.get_send_queue_errors());
    th.join();
}

TEST_CASE("ESPEventHandlerSync wait_for(0) timeout", "[cxx event]")
{
    EventFixture f;
    ESPEventHandlerSync handler(make_shared<ESPEventLoop>());

    ESPEventHandlerSync::EventResultTimed result = handler.wait_event_for(chrono::milliseconds(0));
    TEST_ASSERT_EQUAL(true, result.timeout);
}

TEST_CASE("ESPEventHandlerSync register default event fails", "[cxx event]")
{
    EventFixture f;
    ESPEventHandlerSync handler(make_shared<ESPEventLoop>());
    TEST_THROW(handler.listen_to(ESPEvent()), EventException);
}

TEST_CASE("ESPEventHandlerSync null pointer", "[cxx event]")
{
    EventFixture f;
    TEST_THROW(ESPEventHandlerSync handler(nullptr), EventException);
}

TEST_CASE("ESPEventHandlerSync empty shared_ptr", "[cxx event]")
{
    EventFixture f;
    shared_ptr<ESPEventLoop> event_loop;
    TEST_THROW(ESPEventHandlerSync handler(event_loop), EventException);
}

TEST_CASE("ESPEventHandlerSync queue size 0", "[cxx event]")
{
    EventFixture f;

    TEST_THROW(ESPEventHandlerSync handler(make_shared<ESPEventLoop>(), 0), EventException);
}

TEST_CASE("ESPEventHandlerSync receive after timeout", "[cxx event]")
{
    EventFixture f;
    ESPEventHandlerSync handler(make_shared<ESPEventLoop>());
    handler.listen_to(TEMPLATE_EVENT_0);

    TEST_ASSERT_EQUAL(true, handler.wait_event_for(chrono::milliseconds(0)).timeout);

    send_default_event();

    ESPEvent event = handler.wait_event().event;
    TEST_ASSERT(TEMPLATE_EVENT_0 == event);
}

TEST_CASE("ESPEventHandlerSync send too many events", "[cxx event]")
{
    EventFixture f;
    // Create handler with queue size 1
    ESPEventHandlerSync handler(make_shared<ESPEventLoop>(), 1);
    handler.listen_to(TEMPLATE_EVENT_0);
    TEST_ASSERT_EQUAL(0, handler.get_send_queue_errors());

    send_default_event();
    send_default_event();
    TEST_ASSERT(handler.wait_event().event == TEMPLATE_EVENT_0);
    TEST_ASSERT_EQUAL(true, handler.wait_event_for(chrono::milliseconds(10)).timeout);
    TEST_ASSERT_EQUAL(1, handler.get_send_queue_errors());
}

TEST_CASE("ESPEventAPIDefault initialization failure", "[cxx event]")
{
    EventFixture f;
    esp_event_loop_create_default();
    TEST_THROW(std::shared_ptr<ESPEventAPI> api(new ESPEventAPIDefault()), EventException);
    esp_event_loop_delete_default();
}

TEST_CASE("ESPEventAPICustom no mem", "[cxx event]")
{
    EventFixture f;
    esp_event_loop_args_t loop_args;
    loop_args.queue_size = 1000000;
    loop_args.task_name = "custom_evt";
    loop_args.task_stack_size = 2304;
    loop_args.task_priority = 20;
    loop_args.task_core_id = 0;

    esp_event_loop_create_default();

    TEST_THROW(std::shared_ptr<ESPEventAPI> api(new ESPEventAPICustom(loop_args)), EventException);

    esp_event_loop_delete_default();
}

#endif // __cpp_exceptions
