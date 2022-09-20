# `esp_hw_support` (G1 component)

This component contains hardware-related operations for supporting the system. These operations are one level above that of `hal` in that:

1. it uses system services such as memory allocation, logging, scheduling
2. it may be multi-step operations involving/affecting multiple parts of the SoC
3. it offers a service for other components vary from multiple layers (G1, G2 and G3) of ESP-IDF

Implementations that don't fit other components cleanly, but are not worth creating a new component for (yet) may also be placed here as long as they don't pull dependencies other than the core system components.

## Event-Task Service (esp_etm)

### esp_etm driver design

`esp_etm` driver is divided into two parts:

* The **core** driver, which focuses on ETM channel allocation and offers APIs to connect the channel with ETM tasks and ETM events that come from other peripherals.
* **Peripheral** side extensions, e.g. GPTimer support generating different kinds of ETM events, and accept multiple ETM tasks. These extensions are implemented in the peripheral driver, and can be located in different components. Usually, the task and event extensions will simply inherit the interface that defined in the core driver.

See the following class diagram, we take the GPIO and GPTimer as the example to illustrate the architecture of `esp_etm` driver.

```mermaid
classDiagram
    esp_etm_channel_t "1" --> "1" esp_etm_event_t : Has
    esp_etm_channel_t "1" --> "1" esp_etm_task_t : Has
    class esp_etm_channel_t {
        -int chan_id
        -esp_etm_event_t event
        -esp_etm_task_t task
        +enable() esp_err_t
        +disable() esp_err_t
        +connect(event, task) esp_err_t
        +dump() esp_err_t
    }

    class esp_etm_event_t {
        <<interface>>
        #int event_id
        #etm_trigger_peripheral_t trig_periph
        #del() esp_err_t
    }

    class esp_etm_task_t {
        <<interface>>
        #int task_id
        #etm_trigger_peripheral_t trig_periph
        #del() esp_err_t
    }

    gpio_etm_event_t --|> esp_etm_event_t : Inheritance
    class gpio_etm_event_t {
        -int chan_id
        +bind_gpio(gpio_num_t gpio) esp_err_t
    }

    gpio_etm_task_t --|> esp_etm_task_t : Inheritance
    class gpio_etm_task_t {
        -int chan_id
        +add_gpio(gpio_num) esp_err_t
        +rm_gpio(gpio_num) esp_err_t
    }

    gptimer_t "1" --> "1..*" gptimer_etm_event_t : Has
    gptimer_t "1" --> "1..*" gptimer_etm_task_t : Has
    class gptimer_t {
        -gptimer_etm_event_t[] events
        -gptimer_etm_task_t[] tasks
    }

    gptimer_etm_event_t --|> esp_etm_event_t : Inheritance
    class gptimer_etm_event_t {
    }

    gptimer_etm_task_t --|> esp_etm_task_t : Inheritance
    class gptimer_etm_task_t {
    }
```
