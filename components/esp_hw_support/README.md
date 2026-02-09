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

## MSPI Interrupt Logic - Chip Differences Analysis

### 1. Overview

This document describes the implementation differences of MSPI interrupt handling mechanism across different chips in ESP-IDF.

#### Related Files

| File | Description |
|------|-------------|
| `components/esp_hw_support/mspi/mspi_intr/mspi_intr.c` | Shared MSPI interrupt management |
| `components/esp_psram/system_layer/esp_psram_mspi.c` | PSRAM specific interrupt handling |

---

### 2. Architecture Design

The MSPI interrupt system is divided into two modes based on chip characteristics:

| Mode             | Description                                      |
|------------------|--------------------------------------------------|
| **Shared IRQ**   | Flash and PSRAM share a single MSPI IRQ source   |
| **Separate IRQ** | Flash and PSRAM have independent MSPI IRQs       |

#### Architecture Diagrams

##### 2.1 Shared IRQ Mode

```
                    ┌──────────────────────────────────────┐
                    │           MSPI Hardware IRQ          │
                    └──────────────────┬───────────────────┘
                                       │
                                       ▼
                    ┌──────────────────────────────────────┐
                    │      mspi_isr_handler()              │
                    │      [mspi_intr.c]                   │
                    │                                      │
                    │  1. mspi_ll_get_intr_raw()           │
                    │  2. mspi_ll_clear_intr()             │
                    │  3. Check error events and log       │
                    └──────────────────┬───────────────────┘
                                       │
                    ┌──────────────────┴───────────────────┐
                    │                                      │
                    ▼                                      ▼
            ┌───────────────────┐              ┌───────────────────┐
            │  s_isr.psram_isr  │              │  s_isr.flash_isr  │
            │  (PSRAM handler)  │              │  (Flash handler)  │
            └─────────┬─────────┘              └─────────┬─────────┘
                      │                                  │
                      └────────────┬─────────────────────┘
                                   ▼
                      ┌─────────────────────────┐
                      │ abort() if fatal error  │
                      └─────────────────────────┘
```

##### 2.2 Separate IRQ Mode

```
    ┌─────────────────────┐               ┌─────────────────────┐
    │   Flash MSPI IRQ    │               │   PSRAM MSPI IRQ    │
    └──────────┬──────────┘               └──────────┬──────────┘
               │                                     │
               ▼                                     ▼
    ┌─────────────────────────────┐     ┌─────────────────────────────┐
    │ [Reserved]                  │     │ mspi_psram_isr_handler_     │
    │                             │     │ wrapper()                   │
    │ To be registered by flash   │     │ [esp_psram_mspi.c]          │
    │ driver or system component  │     │                             │
    │                             │     │ 1. psram_ctrlr_ll_get_      │
    │ Events to handle:           │     │    intr_raw()               │
    │ - MSPI0 CPU read events     │     │ 2. psram_ctrlr_ll_clear_    │
    │ - MSPI1 ESP-Flash driver    │     │    intr()                   │
    │   events                    │     │ 3. mspi_psram_isr_handler() │
    │                             │     │ 4. abort() if fatal error   │
    └─────────────────────────────┘     └─────────────────────────────┘
```

---

### 3. API Reference

#### 3.1 mspi_intr.c

This file (and these APIs) is only available on MSPI_LL_INTR_SHARED chips

```c
/**
 * @brief Register MSPI ISR
 * @param isr Pointer to structure containing psram_isr and flash_isr
 * @return ESP_OK on success
 */
esp_err_t esp_mspi_register_isr(mspi_isr_t *isr);

/**
 * @brief Unregister MSPI ISR
 * @return ESP_OK on success
 */
esp_err_t esp_mspi_unregister_isr(void);
```

#### 3.2 esp_psram_mspi.c

- On MSPI_LL_INTR_SHARED chips, this file registers PSRAM specific ISR to the shared MSPI dispatcher (in mspi_intr.c)
- On !MSPI_LL_INTR_SHARED chips, this file registers PSRAM own ISR via interrupt allocator

```c
/**
 * @brief Register PSRAM MSPI ISR
 * @return ESP_OK on success
 */
esp_err_t esp_psram_mspi_register_isr(void);

/**
 * @brief Unregister PSRAM MSPI ISR
 * @return ESP_OK on success
 */
esp_err_t esp_psram_mspi_unregister_isr(void);
```
