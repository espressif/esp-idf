/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "soc/lldesc.h"
#include "soc/soc_caps.h"
#include "hal/i2s_types.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#if SOC_GDMA_SUPPORTED
#include "esp_private/gdma.h"
#endif
#include "esp_pm.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2S_NULL_POINTER_CHECK(tag, p)          ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, tag, "[%s:%d] input parameter '"#p"' is NULL",__FUNCTION__,__LINE__)

/**
 * @brief i2s channel state for checking if the operation in under right driver state
 */
typedef enum {
    I2S_CHAN_STATE_REGISTER,                /*!< i2s channel is registered (not initialized)  */
    I2S_CHAN_STATE_READY,                   /*!< i2s channel is stopped (initialized) */
    I2S_CHAN_STATE_IDLE,                    /*!< i2s channel is idling (initialized and started) */
    I2S_CHAN_STATE_WRITING,                 /*!< i2s channel is writing (initialized and started) */
    I2S_CHAN_STATE_READING                  /*!< i2s channel is reading (initialized and started) */
} i2s_state_t;

/**
 * @brief i2s channel level configurations
 * @note  It performs as channel handle
 */
typedef struct {
#if SOC_GDMA_SUPPORTED
    gdma_channel_handle_t   dma_chan;       /*!< gdma channel handler */
#endif
    uint32_t                desc_num;       /*!< I2S DMA buffer number, it is also the number of DMA descriptor */
    uint32_t                frame_num;      /*!< I2S frame number in one DMA buffer. One frame means one-time sample data in all slots */
    uint32_t                buf_size;       /*!< dma buffer size */
    bool                    auto_clear;     /*!< Set to auto clear DMA TX descriptor, i2s will always send zero automatically if no data to send */
    uint32_t                rw_pos;         /*!< reading/writing pointer position */
    void                    *curr_ptr;      /*!< Pointer to current dma buffer */
    lldesc_t                **desc;         /*!< dma descriptor array */
    uint8_t                 **bufs;         /*!< dma buffer array */
} i2s_dma_t;

/**
 * @brief i2s controller level configurations
 * @note  Both i2s rx and tx channel are under its control
 */
typedef struct {
    i2s_port_t              id;             /*!< i2s port id */
    i2s_hal_context_t       hal;            /*!< hal context */
    uint32_t                chan_occupancy; /*!< channel occupancy (rx/tx) */
    bool                    full_duplex;    /*!< is full_duplex */
    i2s_chan_handle_t       tx_chan;        /*!< tx channel handler */
    i2s_chan_handle_t       rx_chan;        /*!< rx channel handler */
    int                     mclk;           /*!< MCK out pin, shared by tx/rx*/

#if !SOC_GDMA_SUPPORTED
    intr_handle_t           i2s_isr_handle; /*!< i2s interrupt handler */
#endif
} i2s_controller_t;

struct i2s_channel_t {
    /* Channel basic information */
    i2s_controller_t        *parent;        /*!< Parent pointer to controller object */
    i2s_comm_mode_t         mode;           /*!< i2s channel communication mode */
    i2s_role_t              role;           /*!< i2s role */
    i2s_dir_t               dir;            /*!< i2s channel direction */
    i2s_dma_t               dma;            /*!< i2s dma object */
    i2s_state_t             state;          /*!< i2s driver state. Ensuring the driver working in a correct sequence */
    /* Stored configurations */
    void                    *mode_info;     /*!< Slot, clock and gpio information of each mode */
#if SOC_I2S_SUPPORTS_APLL
    bool                    apll_en;        /*!< Flag of wether APLL enabled */
#endif
    uint32_t                active_slot;    /*!< Active slot number */
    uint32_t                total_slot;     /*!< Total slot number */
    /* Locks and queues */
    SemaphoreHandle_t       mutex;          /*!< Mutex for DMA buffer related operations like reading/writing */
    esp_pm_lock_handle_t    pm_lock;        /*!< Power management lock, to avoid apb clock frequency changes while i2s is working */
    QueueHandle_t           msg_queue;      /*!< Message queue handler, used for transporting data between interrupt and read/write task */
    QueueHandle_t           event_queue;    /*!< Event queue handler， used for transporting interrupt event to user */

    /* Function pointers of the corresponding mode */
    void      (*start)(i2s_chan_handle_t);  /*!< start tx/rx channel */
    void      (*stop)(i2s_chan_handle_t);   /*!< stop tx/rx channel */
};

/**
 * @brief i2s platform level configurations
 * @note  All i2s controllers are under its control
 */
typedef struct {
    portMUX_TYPE            spinlock;                   /*!< Platform level lock */
    i2s_controller_t        *controller[I2S_NUM_MAX];   /*!< Controller object */
    const char              *comp_name[I2S_NUM_MAX];    /*!< The component name that occupied i2s controller */
} i2s_platform_t;

extern i2s_platform_t s_i2s;

esp_err_t i2s_init_dma_intr(i2s_chan_handle_t handle, int intr_flag);

esp_err_t i2s_free_dma_desc(i2s_chan_handle_t handle);

esp_err_t i2s_alloc_dma_desc(i2s_chan_handle_t handle, uint32_t num, uint32_t bufsize);

uint32_t i2s_get_buf_size(i2s_chan_handle_t handle, uint32_t data_bit_width, uint32_t dma_frame_num);

#if SOC_I2S_SUPPORTS_APLL
uint32_t i2s_set_get_apll_freq(uint32_t mclk_freq);
#endif

void i2s_gpio_check_and_set(gpio_num_t gpio, uint32_t signal_idx, bool is_input);

esp_err_t i2s_check_set_mclk(i2s_port_t id, gpio_num_t gpio_num);

#ifdef __cplusplus
}
#endif
