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
#include "driver/i2s_types.h"
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

#define I2S_NULL_POINTER_CHECK(tag, p)          ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, tag, "input parameter '"#p"' is NULL")

/**
 * @brief i2s channel state for checking if the operation in under right driver state
 */
typedef enum {
    I2S_CHAN_STATE_REGISTER,                /*!< i2s channel is registered (not initialized)  */
    I2S_CHAN_STATE_READY,                   /*!< i2s channel is disabled (initialized) */
    I2S_CHAN_STATE_RUNNING,                 /*!< i2s channel is idling (initialized and enabled) */
} i2s_state_t;

/**
 * @brief i2s channel level configurations
 * @note  It performs as channel handle
 */
typedef struct {
#if SOC_GDMA_SUPPORTED
    gdma_channel_handle_t   dma_chan;       /*!< gdma channel handle */
#else
    intr_handle_t           dma_chan;       /*!< interrupt channel handle */
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
} i2s_controller_t;

struct i2s_channel_obj_t {
    /* Channel basic information */
    i2s_controller_t        *controller;    /*!< Parent pointer to controller object */
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
    SemaphoreHandle_t       mutex;          /*!< Mutex semaphore for the channel operations */
    SemaphoreHandle_t       binary;         /*!< Binary semaphore for writing / reading / enabling / disabling */
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t    pm_lock;        /*!< Power management lock, to avoid apb clock frequency changes while i2s is working */
#endif
#if CONFIG_I2S_ISR_IRAM_SAFE
    StaticSemaphore_t       *mutex_struct;      /*!< Static mutex struct */
    StaticSemaphore_t       *binary_struct;     /*!< Static binary struct */
    StaticQueue_t           *msg_que_struct;    /*!< Static message queue struct */
    void                    *msg_que_storage;   /*!< Static message queue storage */
#endif
    QueueHandle_t           msg_queue;      /*!< Message queue handler, used for transporting data between interrupt and read/write task */
    i2s_event_callbacks_t   callbacks;      /*!< Callback functions */
    void                    *user_data;     /*!< User data for callback functions */
    void      (*start)(i2s_chan_handle_t);  /*!< start tx/rx channel */
    void      (*stop)(i2s_chan_handle_t);   /*!< stop tx/rx channel */
};

/**
 * @brief i2s platform level configurations
 * @note  All i2s controllers' resources are involved
 */
typedef struct {
    portMUX_TYPE            spinlock;                   /*!< Platform level lock */
    i2s_controller_t        *controller[SOC_I2S_NUM];   /*!< Controller object */
    const char              *comp_name[SOC_I2S_NUM];    /*!< The component name that occupied i2s controller */
} i2s_platform_t;

extern i2s_platform_t g_i2s;

/**
 * @brief Initialize I2S DMA interrupt
 *
 * @param handle        I2S channel handle
 * @param intr_flag     I2S interrupt flags, `ESP_INTR_FLAG_XXX` defined in `esp_intr_alloc.h`
 * @return
 *      - ESP_OK                Initialize interrupt success
 *      - ESP_ERR_INVALID_ARG   Wrong port id or NULL pointer
 */
esp_err_t i2s_init_dma_intr(i2s_chan_handle_t handle, int intr_flag);

/**
 * @brief Free I2S DMA descriptor and DMA buffer
 *
 * @param handle        I2S channel handle
 * @return
 *      - ESP_OK                Free success
 *      - ESP_ERR_INVALID_ARG   NULL pointer
 */
esp_err_t i2s_free_dma_desc(i2s_chan_handle_t handle);

/**
 * @brief Allocate memory for I2S DMA descriptor and DMA buffer
 *
 * @param handle        I2S channel handle
 * @param num           Number of DMA descriptors
 * @param bufsize       The DMA buffer size
 *
 * @return
 *      - ESP_OK                Allocate memory success
 *      - ESP_ERR_INVALID_ARG   NULL pointer or bufsize is too big
 *      - ESP_ERR_NO_MEM        No memmory for DMA descriptor and DMA buffer
 */
esp_err_t i2s_alloc_dma_desc(i2s_chan_handle_t handle, uint32_t num, uint32_t bufsize);

/**
 * @brief Get DMA buffer size
 *
 * @param handle        I2S channel handle
 * @param data_bit_width Data bit width in one slot
 * @param dma_frame_num  Frame number in one DMA buffer
 *
 * @return
 *      - DMA buffer size
 */
uint32_t i2s_get_buf_size(i2s_chan_handle_t handle, uint32_t data_bit_width, uint32_t dma_frame_num);

#if SOC_I2S_SUPPORTS_APLL
/**
 * @brief Set mclk frequency and get the actuall APLL frequency
 *
 * @param mclk_freq_hz  Expected mclk frequenct in Hz
 * @return
 *  - Actuall APLL frequency
 */
uint32_t i2s_set_get_apll_freq(uint32_t mclk_freq_hz);
#endif

/**
 * @brief Check gpio validity and attach to corresponding signal
 *
 * @param gpio          GPIO number
 * @param signal_idx    Signal index
 * @param is_input      Is input gpio
 * @param is_invert     Is invert gpio
 */
void i2s_gpio_check_and_set(gpio_num_t gpio, uint32_t signal_idx, bool is_input, bool is_invert);

/**
 * @brief Check gpio validity and output mclk signal
 *
 * @param id            I2S port id
 * @param gpio_num      GPIO number
 * @param is_apll       Is using APLL as clock source
 * @param is_invert     Is invert the GPIO
 * @return
 *      - ESP_OK                Set mclk output gpio success
 *      - ESP_ERR_INVALID_ARG   Invalid GPIO number
 */
esp_err_t i2s_check_set_mclk(i2s_port_t id, gpio_num_t gpio_num, bool is_apll, bool is_invert);

/**
 * @brief Attach data out signal and data in signal to a same gpio
 *
 * @param gpio          GPIO number
 * @param out_sig_idx   Data out signal index
 * @param in_sig_idx    Data in signal index
 */
void i2s_gpio_loopback_set(gpio_num_t gpio, uint32_t out_sig_idx, uint32_t in_sig_idx);

#ifdef __cplusplus
}
#endif
