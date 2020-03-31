// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ESP_EVENT_CXX_H_
#define ESP_EVENT_CXX_H_

#ifdef __cpp_exceptions

#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <exception>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "esp_exception.hpp"
#include "esp_event_api.hpp"

namespace idf {

namespace event {

extern const std::chrono::milliseconds PLATFORM_MAX_DELAY_MS;

const std::chrono::microseconds MIN_TIMEOUT(200);

class EventException : public ESPException {
public:
    EventException(esp_err_t error) : ESPException(error) { }
};

/**
 * @brief
 * Thrown to signal a timeout in EventHandlerSync.
 */
class EventTimeout : public idf::event::EventException {
public:
    EventTimeout(esp_err_t error) : EventException(error) { }
};

/**
 * @brief
 * Event ID wrapper class to make C++ APIs more explicit.
 *
 * This prevents APIs from taking raw ints as event IDs which are not very expressive and may be
 * confused with other parameters of a function.
 */
class ESPEventID {
public:
    ESPEventID() : id(0) { }
    explicit ESPEventID(int32_t event_id) : id(event_id) { }

    inline bool operator==(const ESPEventID &rhs) const {
        return id == rhs.get_id();
    }

    inline ESPEventID &operator=(const ESPEventID& other) {
        id = other.id;
        return *this;
    }

    inline int32_t get_id() const {
        return id;
    }

    friend std::ostream& operator<<(std::ostream& os, const ESPEventID& id);

private:
    int32_t id;
};

inline std::ostream& operator<<(std::ostream &os, const ESPEventID& id) {
    os << id.id;
    return os;
}

/*
 * Helper struct to bundle event base and event ID.
 */
struct ESPEvent {
    ESPEvent()
        : base(nullptr), id() { }
    ESPEvent(esp_event_base_t event_base, const ESPEventID &event_id)
        : base(event_base), id(event_id) { }

    esp_event_base_t base;
    ESPEventID id;
};

/**
 * Thrown if event registration, i.e. \c register_event() or \c register_event_timed(), fails.
 */
struct ESPEventRegisterException : public EventException {
    ESPEventRegisterException(esp_err_t err, const ESPEvent& event)
        : EventException(err), esp_event(event) { }

    const char *what() const noexcept
    {
        std::string ret_message = "Event base: " + std::string(esp_event.base)
                + ", Event ID: " + std::to_string(esp_event.id.get_id());
        return ret_message.c_str();
    }

    const ESPEvent esp_event;
};

inline bool operator==(const ESPEvent &lhs, const ESPEvent &rhs)
{
    return lhs.base == rhs.base && lhs.id == rhs.id;
}

TickType_t convert_ms_to_ticks(const std::chrono::milliseconds &time);

/**
 * Callback-event combination for ESPEventLoop.
 *
 * Used to bind class-based handler instances to event_handler_hook which is registered into the C-based
 * esp event loop.
 * It can be used directly, however, the recommended way is to obtain a unique_ptr via ESPEventLoop::register_event().
 */
class ESPEventReg {
public:
    /**
     * Register the event handler \c cb to handle the events defined by \c ev.
     *
     * @param cb The handler to be called.
     * @param ev The event for which the handler is registered.
     * @param api The esp event api implementation.
     */
    ESPEventReg(std::function<void(const ESPEvent &, void*)> cb,
            const ESPEvent& ev,
            std::shared_ptr<ESPEventAPI> api);

    /**
     * Unregister the event handler.
     */
    virtual ~ESPEventReg();

protected:
    /**
     * This is esp_event's handler, all events registered go through this.
     */
    static void event_handler_hook(void *handler_arg,
                                   esp_event_base_t event_base,
                                   int32_t event_id,
                                   void *event_data);

    /**
     * User event handler.
     */
    std::function<void(const ESPEvent &, void*)> cb;

    /**
     * Helper function to enter the instance's scope from the generic \c event_handler_hook().
     */
    virtual void dispatch_event_handling(ESPEvent event, void *event_data);

    /**
     * Save the event here to be able to un-register from the event loop on destruction.
     */
    ESPEvent event;

    /**
     * This API handle allows different sets of APIs to be applied, e.g. default event loop API and
     * custom event loop API.
     */
    std::shared_ptr<ESPEventAPI> api;

    /**
     * Event handler instance from the esp event C API.
     */
    esp_event_handler_instance_t instance;
};

/**
 * Callback-event combination for ESPEventLoop with builtin timeout.
 *
 * Used to bind class-based handler instances to event_handler_hook which is registered into the C-based
 * esp event loop.
 * It can be used directly, however, the recommended way is to obtain a unique_ptr via ESPEventLoop::register_event().
 */
class ESPEventRegTimed : public ESPEventReg {
public:
    /**
     * Register the event handler \c cb to handle the events as well as a timeout callback in case the event doesn't
     * arrive on time.
     *
     * If the event \c ev is received before \c timeout milliseconds, then the event handler is invoked.
     * If no such event is received before \c timeout milliseconds, then the timeout callback is invoked.
     * After the timeout or the first occurance of the event, the timer will be deactivated.
     * The event handler registration will only be deactivated if the timeout occurs.
     * If event handler and timeout occur at the same time, only either the event handler or the timeout callback
     * will be invoked.
     *
     * @param cb The handler to be called.
     * @param ev The event for which the handler is registered.
     * @param timeout_cb The timeout callback which is called in case there is no event for \c timeout microseconds.
     * @param timeout The timeout in microseconds.
     * @param api The esp event api implementation.
     */
    ESPEventRegTimed(std::function<void(const ESPEvent &, void*)> cb,
            const ESPEvent& ev,
            std::function<void(const ESPEvent &)> timeout_cb,
            const std::chrono::microseconds &timeout,
            std::shared_ptr<ESPEventAPI> api);

    /**
     * Unregister the event handler, stop and delete the timer.
     */
    virtual ~ESPEventRegTimed();

protected:

    /**
     * Helper function to hook directly into esp timer callback.
     */
    static void timer_cb_hook(void *arg);

    /**
     * Helper function to enter the instance's scope from the generic \c event_handler_hook().
     */
    void dispatch_event_handling(ESPEvent event, void *event_data) override;

    /**
     * The timer callback which will be called on timeout.
     */
    std::function<void(const ESPEvent &)> timeout_cb;

    /**
     * Timer used for event timeouts.
     */
    esp_timer_handle_t timer;

    /**
     * This mutex makes sure that a timeout and event callbacks aren't invoked both.
     */
    std::mutex timeout_mutex;
};

class ESPEventLoop {
public:
    /**
     * Creates the ESP default event loop.
     *
     * @param api the interface to the esp_event api; this determines whether the default event loop is used
     *            or a custom loop (or just a mock up for tests). May be nullptr, in which case it will created
     *            here.
     *
     * @note may throw EventException
     */
    ESPEventLoop(std::shared_ptr<ESPEventAPI> api = std::make_shared<ESPEventAPIDefault>());

    /**
     * Deletes the event loop implementation (depends on \c api).
     */
    virtual ~ESPEventLoop();

    /**
     * Registers a specific handler-event combination to the event loop.
     *
     * @return a reference to the combination of handler and event which can be used to unregister
     * this combination again later on.
     *
     * @note registering the same event twice will result in unregistering the earlier registered handler.
     * @note may throw EventException, ESPEventRegisterException
     */
    std::unique_ptr<ESPEventReg> register_event(const ESPEvent &event,
            std::function<void(const ESPEvent &, void*)> cb);

    /**
     * Sets a timeout for event. If the specified event isn't received within timeout,
     * timer_cb is called.
     *
     * @note this is independent from the normal event handling. Hence, registering an event for
     * timeout does not interfere with a different client that has registered normally for the
     * same event.
     */
    std::unique_ptr<ESPEventRegTimed> register_event_timed(const ESPEvent &event,
            std::function<void(const ESPEvent &, void*)> cb,
            const std::chrono::microseconds &timeout,
            std::function<void(const ESPEvent &)> timer_cb);

    /**
     * Posts an event and corresponding data.
     *
     * @param event the event to post
     * @param event_data The event data. A copy will be made internally and a pointer to the copy will be passed to the
     *        event handler.
     * @param wait_time the maximum wait time the function tries to post the event
     */
    template<typename T>
    void post_event_data(const ESPEvent &event,
            T &event_data,
            const std::chrono::milliseconds &wait_time = PLATFORM_MAX_DELAY_MS);

    /**
     * Posts an event.
     *
     * No event data will be send. The event handler will receive a nullptr.
     *
     * @param event the event to post
     * @param wait_time the maximum wait time the function tries to post the event
     */
    void post_event_data(const ESPEvent &event,
            const std::chrono::milliseconds &wait_time = PLATFORM_MAX_DELAY_MS);

private:
    /**
     * This API handle allows different sets of APIs to be applied, e.g. default event loop API and
     * custom event loop API.
     */
    std::shared_ptr<ESPEventAPI> api;
};

/**
 * ESPEventHandlerSync builds upon ESPEventLoop to create a class which allows synchronous event handling.
 *
 * It is built around a queue which buffers received events. This queue is also used to wait synchronously (blocking)
 * for an event. The consequence is that once an event is registered with this class, it is guaranteed to be received
 * as long as the queue can handle all incoming events (see \c get_send_queue_errors()).
 */
class ESPEventHandlerSync {
public:
    /**
     * Result type for synchronous waiting.
     */
    struct EventResult {
        EventResult() : event(), ev_data(nullptr) { }
        EventResult(ESPEvent ev, void *ev_data) : event(ev), ev_data(ev_data) { }
        ESPEvent event;
        void *ev_data;
    };

    /**
     * Result type for synchronous waiting with timeout.
     */
    struct EventResultTimed : public EventResult {
        EventResultTimed(EventResult event_result, bool timeout_arg)
            : EventResult(event_result), timeout(timeout_arg) { }
        bool timeout;
    };

    /**
     * Sets up synchronous event handling and registers event with it.
     *
     * @param event_loop ESPEventLoop implementation to manage esp events.
     * @param queue_max_size The queue size of the underlying FreeRTOS queue.
     *        The memory to store queue_max_size number of events is allocated during construction
     *        and held until destruction!
     * @param queue_send_timeout The timeout for posting events to the internal queue
     */
    ESPEventHandlerSync(std::shared_ptr<ESPEventLoop> event_loop,
                        size_t queue_max_size = 10,
                        TickType_t queue_send_timeout = 0);

    /**
     * Unregister all formerly registered events via automatic destruction in registry.
     */
    virtual ~ESPEventHandlerSync();

    /**
     * Waits for any of the events registered before with listen_to().
     */
    EventResult wait_event();

    /**
     * Waits for an event either PLATFORM_MAX_DELAY_MS ms or timeout ms.
     *
     * @param timeout the maximum waiting time for new events if no event is pending
     *       The timeout is restricted by the TickType_t and configTICK_RATE_HZ.
     *       TickType_t's width determines the maximum wait time. configTICK_RATE_HZ
     *       determines the minimum wait time.
     *
     * Throws EventTimeout in case of a timeout.
     */
    EventResultTimed wait_event_for(const std::chrono::milliseconds &timeout);

    /**
     * Register additional event to listen for.
     *
     * @note this will unregister all earlier registered events of the same event type from the event loop.
     */
    void listen_to(const ESPEvent &event);

    /**
     * Indicates whether there were errors inserting an event into the queue.
     * This is the case e.g. if the queue with waiting events is full already.
     * Use this function to adjust the queue size (\c queue_send_timeout in constructor) in your application.
     */
    size_t get_send_queue_errors() const;

protected:
    /**
     * Posts an event to the internal queue.
     */
    void post_event(const EventResult &result);

private:
    /**
     * Keeps track if there are any errors inserting an event into this class's event queue.
     */
    std::atomic<size_t> send_queue_errors;

    /**
     * The queue which saves events if they were received already or waits if no event was
     * received.
     */
    QueueHandle_t event_queue;

    /**
     * Timeout used to posting to the queue when using \c post_event(). Can be adjusted in constructor.
     */
    TickType_t queue_send_timeout;

    /**
     * The event loop used for this synchronous event handling class.
     */
    std::shared_ptr<ESPEventLoop> event_loop;

    /**
     * Keeps track of all events which are registered already for synchronous handling.
     *
     * This is necessary to keep the registration.
     */
    std::vector<std::shared_ptr<ESPEventReg> > registry;
};

template<typename T>
void ESPEventLoop::post_event_data(const ESPEvent &event,
        T &event_data,
        const std::chrono::milliseconds &wait_time)
{
    esp_err_t result = api->post(event.base,
            event.id.get_id(),
            &event_data,
            sizeof(event_data),
            convert_ms_to_ticks(wait_time));

    if (result != ESP_OK) {
        throw ESPException(result);
    }
}

} // namespace event

} // namespace idf

#endif // __cpp_exceptions

#endif // ESP_EVENT_CXX_H_
