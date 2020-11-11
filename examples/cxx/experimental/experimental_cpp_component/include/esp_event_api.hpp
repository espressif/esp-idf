#ifndef ESP_EVENT_API_HPP_
#define ESP_EVENT_API_HPP_

#include "esp_event.h"

namespace idf {

namespace event {

/**
 * Abstract interface for direct calls to esp_event C-API.
 * This is generally not intended to be used directly.
 * It's main purpose is to provide ESPEventLoop a unified API not dependent on whether the default event loop or a
 * custom event loop is used.
 * The interface resembles the C-API, have a look there for further documentation.
 */
class ESPEventAPI {
public:
    virtual ~ESPEventAPI() { }

    virtual esp_err_t handler_register(esp_event_base_t event_base,
            int32_t event_id,
            esp_event_handler_t event_handler,
            void* event_handler_arg,
            esp_event_handler_instance_t *instance) = 0;

    virtual esp_err_t handler_unregister(esp_event_base_t event_base,
            int32_t event_id,
            esp_event_handler_instance_t instance) = 0;

    virtual esp_err_t post(esp_event_base_t event_base,
            int32_t event_id,
            void* event_data,
            size_t event_data_size,
            TickType_t ticks_to_wait) = 0;
};

/**
 * @brief API version with default event loop.
 *
 * It will direct calls to the default event loop API.
 */
class ESPEventAPIDefault : public ESPEventAPI {
public:
    ESPEventAPIDefault();
    virtual ~ESPEventAPIDefault();

    /**
     * Copying would lead to deletion of event loop through destructor.
     */
    ESPEventAPIDefault(const ESPEventAPIDefault &o) = delete;
    ESPEventAPIDefault& operator=(const ESPEventAPIDefault&) = delete;

    esp_err_t handler_register(esp_event_base_t event_base,
            int32_t event_id,
            esp_event_handler_t event_handler,
            void* event_handler_arg,
            esp_event_handler_instance_t *instance) override;

    esp_err_t handler_unregister(esp_event_base_t event_base,
            int32_t event_id,
            esp_event_handler_instance_t instance) override;

    esp_err_t post(esp_event_base_t event_base,
            int32_t event_id,
            void* event_data,
            size_t event_data_size,
            TickType_t ticks_to_wait) override;
};

/**
 * @brief API version with custom event loop.
 *
 * It will direct calls to the custom event loop API.
 * The loop parameters are given in the constructor the same way it's done in esp_event_loop_create() in event.h.
 * This class also provides a run method in case the custom event loop was created without its own task.
 */
class ESPEventAPICustom : public ESPEventAPI {
public:
    /**
     * @param event_loop_args the event loop arguments, refer to esp_event_loop_create() in event.h.
     */
    ESPEventAPICustom(const esp_event_loop_args_t &event_loop_args);

    virtual ~ESPEventAPICustom();

    /**
     * Copying would lead to deletion of event loop through destructor.
     */
    ESPEventAPICustom(const ESPEventAPICustom &o) = delete;
    ESPEventAPICustom& operator=(const ESPEventAPICustom&) = delete;

    esp_err_t handler_register(esp_event_base_t event_base,
            int32_t event_id,
            esp_event_handler_t event_handler,
            void* event_handler_arg,
            esp_event_handler_instance_t *instance) override;

    esp_err_t handler_unregister(esp_event_base_t event_base,
            int32_t event_id,
            esp_event_handler_instance_t instance) override;

    esp_err_t post(esp_event_base_t event_base,
            int32_t event_id,
            void* event_data,
            size_t event_data_size,
            TickType_t ticks_to_wait) override;

    /**
     * Run the event loop. The behavior is the same as esp_event_loop_run in esp_event.h.
     */
    esp_err_t run(TickType_t ticks_to_run);

private:
    esp_event_loop_handle_t event_loop;
};

} // event

} // idf

#endif // ESP_EVENT_API_HPP_
