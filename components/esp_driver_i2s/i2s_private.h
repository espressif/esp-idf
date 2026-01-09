/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <sys/lock.h>
#include <stdatomic.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "soc/lldesc.h"
#include "soc/soc_caps.h"
#include "hal/i2s_periph.h"
#include "hal/i2s_hal.h"
#include "hal/lp_i2s_hal.h"
#if SOC_LP_I2S_SUPPORTED
#include "hal/lp_i2s_ll.h"
#endif
#include "driver/i2s_types.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp_clock_output.h"
#endif
#if SOC_GDMA_SUPPORTED
#include "esp_private/gdma.h"
#endif
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_gpio_reserve.h"
#if SOC_HAS(PAU)
#include "esp_private/sleep_retention.h"
#endif
#include "esp_pm.h"
#include "esp_err.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

// If ISR handler is allowed to run whilst cache is disabled,
// Make sure all the code and related variables used by the handler are in the SRAM
#if CONFIG_I2S_ISR_IRAM_SAFE
#define I2S_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
#define I2S_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define I2S_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
#define I2S_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif //CONFIG_I2S_ISR_IRAM_SAFE
#define I2S_DMA_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT)

#if SOC_PERIPH_CLK_CTRL_SHARED
#define I2S_CLOCK_SRC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define I2S_CLOCK_SRC_ATOMIC()
#endif

#if !SOC_RCC_IS_INDEPENDENT
#define I2S_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define I2S_RCC_ATOMIC()
#endif

#define I2S_USE_RETENTION_LINK  (SOC_HAS(PAU) && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)

#define I2S_NULL_POINTER_CHECK(tag, p)          ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, tag, "input parameter '"#p"' is NULL")
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief i2s channel state for checking if the operation in under right driver state
 */
typedef enum {
    I2S_CHAN_STATE_REGISTER,                /*!< i2s channel is registered (not initialized)  */
    I2S_CHAN_STATE_READY,                   /*!< i2s channel is disabled (initialized) */
    I2S_CHAN_STATE_RUNNING,                 /*!< i2s channel is idling (initialized and enabled) */
} i2s_state_t;

/**
 * @brief Group of I2S callbacks
 * @note The callbacks are all running under ISR environment
 * @note When CONFIG_I2S_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 * @note Declare the internal type to remove the dependency of `i2s_common.h`
 */
typedef struct {
    i2s_isr_callback_t on_recv;             /**< Callback of data received event, only for rx channel
                                             *   The event data includes DMA buffer address and size that just finished receiving data
                                             */
    i2s_isr_callback_t on_recv_q_ovf;       /**< Callback of receiving queue overflowed event, only for rx channel
                                             *   The event data includes buffer size that has been overwritten
                                             */
    i2s_isr_callback_t on_sent;             /**< Callback of data sent event, only for tx channel
                                             *   The event data includes DMA buffer address and size that just finished sending data
                                             */
    i2s_isr_callback_t on_send_q_ovf;       /**< Callback of sending queue overflowed event, only for tx channel
                                             *   The event data includes buffer size that has been overwritten
                                             */
} i2s_event_callbacks_internal_t;

/**
 * @brief LP I2S callbacks
 */
typedef struct {
    lp_i2s_callback_t on_thresh_met;           ///< Triggered when the received bytes are bigger than `lp_i2s_chan_config_t:threshold`
    lp_i2s_callback_t on_request_new_trans;    ///< Triggered when a new transaction buffer is needed, when this callback is registered, you don't need to use `lp_i2s_channel_read` to get data, you can get data via this callback asynchronously
} lp_i2s_evt_cbs_internal_t;

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
    bool                    auto_clear_after_cb;     /*!< Set to auto clear DMA TX descriptor after callback, i2s will always send zero automatically if no data to send */
    bool                    auto_clear_before_cb;    /*!< Set to auto clear DMA TX descriptor before callback, i2s will always send zero automatically if no data to send */
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
    int                     id;             /*!< i2s port id */
    i2s_hal_context_t       hal;            /*!< hal context */
    uint32_t                chan_occupancy; /*!< channel occupancy (rx/tx) */
    bool                    full_duplex;    /*!< is full_duplex */
    i2s_chan_handle_t       tx_chan;        /*!< tx channel handler */
    i2s_chan_handle_t       rx_chan;        /*!< rx channel handler */
    _lock_t                 mutex;          /*!< mutex for controller */
#if SOC_HAS(PAU)
    sleep_retention_module_t slp_retention_mod; /*!< Sleep retention module */
    bool                    retention_link_created;  /*!< Whether the retention link is created */
#endif
    int                     mclk;           /*!< MCK out pin, shared by tx/rx*/
#if CONFIG_IDF_TARGET_ESP32
    esp_clock_output_mapping_handle_t mclk_out_hdl; /*!< The handle of MCLK output signal */
#endif
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
    int                     intr_prio_flags;/*!< i2s interrupt priority flags */
    void                    *mode_info;     /*!< Slot, clock and gpio information of each mode */
    struct {
        bool                is_port_auto: 1;   /*!< Whether the port is auto-assigned */
        bool                is_etm_start: 1;   /*!< Whether start by etm tasks */
        bool                is_etm_stop: 1;    /*!< Whether stop by etm tasks */
        bool                is_raw_pdm: 1;     /*!< Flag of whether send/receive PDM in raw data, i.e., no PCM2PDM/PDM2PCM filter enabled */
        bool                is_external: 1;    /*!< Whether use external clock */
        bool                full_duplex_slave: 1; /*!< whether the channel is forced to switch to slave role for full duplex */
#if SOC_I2S_SUPPORTS_APLL
        bool                apll_en: 1;        /*!< Flag of whether APLL enabled */
#endif
    };
    uint32_t                active_slot;    /*!< Active slot number */
    uint32_t                total_slot;     /*!< Total slot number */
    i2s_clock_src_t         clk_src;        /*!< Clock source */
    uint32_t                sclk_hz;        /*!< Source clock frequency */
    uint32_t                origin_mclk_hz; /*!< Original mclk frequency */
    uint32_t                curr_mclk_hz;   /*!< Current mclk frequency */
    uint32_t                bclk_hz;        /*!< BCLK frequency */
    /* Locks and queues */
    SemaphoreHandle_t       mutex;          /*!< Mutex semaphore for the channel operations */
    SemaphoreHandle_t       binary;         /*!< Binary semaphore for writing / reading / enabling / disabling */
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t    pm_lock;        /*!< Power management lock, to avoid apb clock frequency changes while i2s is working */
#endif
    QueueHandle_t           msg_queue;      /*!< Message queue handler, used for transporting data between interrupt and read/write task */
    uint64_t                reserve_gpio_mask; /*!< The gpio mask that has been reserved by I2S */
    i2s_event_callbacks_internal_t   callbacks;      /*!< Callback functions */
    void                    *user_data;     /*!< User data for callback functions */
    void (*start)(i2s_chan_handle_t);       /*!< start tx/rx channel */
    void (*stop)(i2s_chan_handle_t);        /*!< stop tx/rx channel */
};

/**
 * @brief lp i2s controller type
 */
typedef struct {
    int                     id;             /*!< lp i2s port id */
    lp_i2s_hal_context_t    hal;            /*!< hal context */
    uint32_t                chan_occupancy; /*!< channel occupancy (rx/tx) */
    lp_i2s_chan_handle_t    rx_chan;        /*!< rx channel handle */
    intr_handle_t           intr;           /*!< interrupt handle */
} lp_i2s_controller_t;

/**
 * @brief lp i2s channel object type
 */
struct lp_i2s_channel_obj_t {
    /* Channel basic information */
    lp_i2s_controller_t        *ctlr;          /*!< Parent pointer to controller object */
    i2s_comm_mode_t            mode;           /*!< lp i2s channel communication mode */
    i2s_role_t                 role;           /*!< lp i2s role */
    i2s_dir_t                  dir;            /*!< lp i2s channel direction */
    _Atomic i2s_state_t        state;          /*!< lp i2s driver state. Ensuring the driver working in a correct sequence */
    SemaphoreHandle_t          semphr;         /*!< lp i2s event semphr*/
    lp_i2s_trans_t             trans;          /*!< transaction */
    size_t                     threshold;      /*!< lp i2s threshold*/
    lp_i2s_evt_cbs_internal_t  cbs;            /*!< callbacks */
    void                       *user_data;     /*!< user data */
};

/**
 * @brief i2s platform level configurations
 * @note  All i2s controllers' resources are involved
 */
typedef struct {
    portMUX_TYPE            spinlock;                          /*!< Platform level lock */
    i2s_controller_t        *controller[I2S_LL_GET(INST_NUM)];          /*!< Controller object */
    const char              *comp_name[I2S_LL_GET(INST_NUM)];           /*!< The component name that occupied i2s controller */
#if SOC_LP_I2S_SUPPORTED
    lp_i2s_controller_t     *lp_controller[SOC_LP_I2S_NUM];    /*!< LP controller object*/
    const char              *lp_comp_name[I2S_LL_GET(INST_NUM)];        /*!< The component name that occupied lp i2s controller */
#endif
} i2s_platform_t;

extern i2s_platform_t g_i2s;  /*!< Global i2s instance for driver internal use */

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
 * @param bufsize       The DMA buffer size
 *
 * @return
 *      - ESP_OK                Allocate memory success
 *      - ESP_ERR_INVALID_ARG   NULL pointer or bufsize is too big
 *      - ESP_ERR_NO_MEM        No memory for DMA descriptor and DMA buffer
 */
esp_err_t i2s_alloc_dma_desc(i2s_chan_handle_t handle, uint32_t bufsize);

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

/**
 * @brief Get the frequency of the source clock
 *
 * @param clk_src       clock source
 * @param mclk_freq_hz  Expected mclk frequency in Hz
 * @return
 *      - Actual source clock frequency
 */
uint32_t i2s_get_source_clk_freq(i2s_clock_src_t clk_src, uint32_t mclk_freq_hz);

/**
 * @brief Check gpio validity and attach to corresponding signal
 *
 * @param handle        I2S channel handle
 * @param gpio          GPIO number
 * @param signal_idx    Signal index
 * @param is_input      Is input gpio
 * @param is_invert     Is invert gpio
 */
void i2s_gpio_check_and_set(i2s_chan_handle_t handle, int gpio, uint32_t signal_idx, bool is_input, bool is_invert);

/**
 * @brief Check gpio validity and output mclk signal
 *
 * @param handle        I2S channel handle
 * @param id            I2S port id
 * @param gpio_num      GPIO number
 * @param clk_src       The clock source of this I2S port
 * @param is_invert     Is invert the GPIO
 * @return
 *      - ESP_OK                Set mclk output gpio success
 *      - ESP_ERR_INVALID_ARG   Invalid GPIO number
 */
esp_err_t i2s_check_set_mclk(i2s_chan_handle_t handle, int id, int gpio_num, i2s_clock_src_t clk_src, bool is_invert);

/**
 * @brief Attach data out signal and data in signal to a same gpio
 *
 * @param handle        I2S channel handle
 * @param gpio          GPIO number
 * @param out_sig_idx   Data out signal index
 * @param in_sig_idx    Data in signal index
 */
void i2s_gpio_loopback_set(i2s_chan_handle_t handle, int gpio, uint32_t out_sig_idx, uint32_t in_sig_idx);

/**
 * @brief Reserve the GPIO that configured as I2S output signal
 *
 * @param handle    I2S channel handle
 * @param gpio_num  The output gpio number to be reserved
 */
void i2s_output_gpio_reserve(i2s_chan_handle_t handle, int gpio_num);

/**
 * @brief Revoke the GPIO that configured as I2S output signal
 *
 * @param handle    I2S channel handle
 * @param gpio_mask The output gpio mask to be revoked
 */
void i2s_output_gpio_revoke(i2s_chan_handle_t handle, uint64_t gpio_mask);

#if SOC_I2S_HW_VERSION_1
/**
 * @brief Change the port of the I2S channel
 *
 * @param handle        I2S channel handle
 * @param id            I2S port id
 * @return
 *      - ESP_OK                Change port success
 *      - ESP_ERR_NOT_FOUND     No available I2S port found
 */
esp_err_t i2s_channel_change_port(i2s_chan_handle_t handle, int id);
#endif

#ifdef __cplusplus
}
#endif
