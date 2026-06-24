/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "esp_cache.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/sd_host_sdmmc.h"
#include "esp_private/sd_driver_interface.h"
#include "sd_protocol_defs.h"
#include "soc/soc_caps.h"
#if SOC_SDMMC_HOST_SUPPORTED
#include "hal/sdmmc_hal.h"
#include "hal/sd_types.h"
#include "hal/sdmmc_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SDMMC_HOST_SUPPORTED
#if CONFIG_SD_HOST_SDMMC_ISR_CACHE_SAFE
#define SD_HOST_SDMMC_MEM_ALLOC_CAPS         (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define SD_HOST_SDMMC_MEM_ALLOC_CAPS         MALLOC_CAP_DEFAULT
#endif

#if SOC_SDMMC_PSRAM_DMA_CAPABLE
#define SD_HOST_SDMMC_DMA_ALLOC_CAPS         MALLOC_CAP_DEFAULT | MALLOC_CAP_DMA
#else
#define SD_HOST_SDMMC_DMA_ALLOC_CAPS         (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA)
#endif

#define SD_HOST_SDMMC_CLOCK_UPDATE_CMD_TIMEOUT_US           (1000 * 1000)
#define SD_HOST_SDMMC_START_CMD_TIMEOUT_US                  (1000 * 1000)
#define SD_HOST_SDMMC_RESET_TIMEOUT_US                      (5000 * 1000)

/* Number of DMA descriptors used for transfer.
 * Increasing this value above 4 doesn't improve performance for the usual case
 * of SD memory cards (most data transfers are multiples of 512 bytes).
 */
#define SD_HOST_SDMMC_DMA_DESC_CNT        4

#define GPIO_NUM_CHECK(_gpio_num) \
if (!GPIO_IS_VALID_GPIO(_gpio_num)) { \
    esp_err_t _err = ESP_ERR_INVALID_ARG; \
    ESP_LOGE(TAG, "%s: Invalid GPIO number %d, returned 0x%x", __func__, _gpio_num, _err); \
    return _err; \
}

/**
 * @brief SD Host SDMMC FSM
 */
typedef enum {
    SD_HOST_FSM_INIT = 1,
    SD_HOST_FSM_ENABLED,
    SD_HOST_FSM_STARTED,
} sd_host_sdmmc_fsm_t;

/**
 * @brief SD Host SDMMC event type
 */
typedef struct {
    uint32_t sdmmc_status;      ///< masked SDMMC interrupt status
    uint32_t dma_status;        ///< masked DMA interrupt status
} sd_host_sdmmc_event_t;

/**
 * @brief SD Host SDMMC slot state
 */
typedef enum sd_host_sdmmc_slot_state_t {
    SD_HOST_SLOT_STATE_INIT,     ///< Slot is only initialised
    SD_HOST_SLOT_STATE_READY,    ///< Slot is ready
} sd_host_sdmmc_slot_state_t;

/**
 * @brief SD Host SDMMC transaction state
 */
typedef struct {
    uint8_t* ptr;
    size_t size_remaining;
    size_t next_desc;
    size_t desc_remaining;
} sd_host_sdmmc_trans_state_t;

typedef struct sd_host_sdmmc_slot_t sd_host_sdmmc_slot_t;
typedef struct sd_host_sdmmc_ctlr_t sd_host_sdmmc_ctlr_t;

/**
 * @brief SD Host SDMMC slot context
 */
struct sd_host_sdmmc_slot_t {
    sd_slot_driver_t               drv;
    int                            slot_id;
    sd_mode_t                      sd_mode;
    sd_host_sdmmc_slot_io_cfg_t    io_config;
    bool                           use_gpio_matrix;

    struct {
        int                        width;
        sd_host_sdmmc_slot_state_t width_state;
    } width;
    struct {
        int                        freq_hz;
        sd_host_sdmmc_slot_state_t freq_state;
        int                        real_freq_hz;
    } freq;
    struct {
        sd_sampling_mode_t         mode;
        sd_host_sdmmc_slot_state_t sampling_mode_state;
    } sampling_mode;
    struct {
        sdmmc_delay_phase_t        delayphase;
        sd_host_sdmmc_slot_state_t delay_phase_state;
    } delay_phase;
    struct {
        sdmmc_delay_line_t         delayline;
        sd_host_sdmmc_slot_state_t delay_line_state;
    } delay_line;

    bool                           cclk_always_on;
    soc_periph_sdmmc_clk_src_t     clk_src;

    sd_host_sdmmc_ctlr_t           *ctlr;
    sd_host_evt_cbs_t              cbs;
    void                           *user_data;
};

/**
 * @brief SD Host context
 */
struct sd_host_sdmmc_ctlr_t {
    sd_host_driver_t               drv;                              //host driver
    int                            host_id;                          //host id
    sd_host_sdmmc_slot_t           *slot[SOC_SDMMC_NUM_SLOTS];       //slot
    int                            registered_slot_nums;             //registered slot nums
    sdmmc_hal_context_t            hal;                              //hal context

    sdmmc_desc_t                   *dma_desc;                        //pointer to dma descriptors
    int                            dma_desc_num;                     //number of DMA descriptor
    sd_host_sdmmc_trans_state_t    cur_transfer;                     //current dma trans
    int                            cur_slot_id;                      //current slot id
    bool                           is_app_cmd;                       //this flag is set if the next command is an APP command

    portMUX_TYPE                   spinlock;                         //spinlock
    SemaphoreHandle_t              io_intr_sem;                      //io interrupt semaphore
    SemaphoreHandle_t              mutex;                            //mutex
    QueueHandle_t                  event_queue;                      //event queue

    intr_handle_t                  intr_handle;                      //interrupt handle
    sd_host_evt_cbs_t              cbs;                              //user callbacks
    void                           *cbs_user_data;                   //callback userdata
    esp_pm_lock_handle_t           pm_lock;                          //power management lock
};

/*---------------------------------------------------------------
            Internal APIs (not used by other components)
---------------------------------------------------------------*/
/*---------------------------------------------------------------
                    IO Control
---------------------------------------------------------------*/
/**
 * @brief Do a transaction for the slot
 *
 * @param[in] slot        SD Host slot handle
 * @param[in] cmdinfo     SD command info, see `sdmmc_command_t`
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 */
esp_err_t sd_host_slot_sdmmc_do_transaction(sd_host_slot_handle_t slot, sdmmc_command_t *cmdinfo);

/*---------------------------------------------------------------
                    HW Commands
---------------------------------------------------------------*/
/**
 * @brief HW command for the slot
 *
 * @param[in] slot       SD Host Slot handle
 * @param[in] cmd        SD HW command
 * @param[in] arg        SD command argument
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 *        - ESP_ERR_NOT_FOUND:   Command not found
 */
esp_err_t sd_host_slot_start_command(sd_host_sdmmc_slot_t *slot, sdmmc_hw_cmd_t cmd, uint32_t arg);

/*---------------------------------------------------------------
                Configuration APIs
---------------------------------------------------------------*/
/**
 * @brief Set clock for the slot
 *
 * @param[in] slot       SD Host Slot handle
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 *        - ESP_ERR_NOT_FOUND:   Command not found
 */
esp_err_t sd_host_slot_set_card_clk(sd_host_sdmmc_slot_t *slot);

/**
 * @brief Set bus width for the slot
 *
 * @param[in] slot       SD Host Slot handle
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t sd_host_slot_set_bus_width(sd_host_sdmmc_slot_t *slot);

/**
 * @brief Set sampling mode for the slot
 *
 * @param[in] slot       SD Host Slot handle
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t sd_host_slot_set_bus_sampling_mode(sd_host_sdmmc_slot_t *slot);

/**
 * @brief Set delay phase for the slot
 *
 * @param[in] slot       SD Host Slot handle
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t sd_host_set_delay_phase(sd_host_sdmmc_slot_t *slot);

/**
 * @brief Set delay line for the slot
 *
 * @param[in] slot       SD Host Slot handle
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t sd_host_set_delay_line(sd_host_sdmmc_slot_t *slot);

/**
 * @brief Set clock always on for the slot
 *
 * @param[in] slot       SD Host Slot handle
 * @param[in] always_on  always on or not
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 *        - ESP_ERR_NOT_FOUND:   Command not found
 */
esp_err_t sd_host_slot_set_cclk_always_on_internal(sd_host_sdmmc_slot_t *slot, bool always_on);

/**
 * @brief Enable clock and cmd11 mode
 *
 * @param[in] slot       SD Host Slot handle
 * @param[in] enable     enable or not
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 *        - ESP_ERR_NOT_FOUND:   Command not found
 */
void sd_host_slot_enable_clk_cmd11(sd_host_sdmmc_slot_t *slot, bool enable);

/*---------------------------------------------------------------
                        DMA APIs
---------------------------------------------------------------*/
/**
 * @brief Get free descriptor numbers
 *
 * @param[in] slot       SD Host Slot handle
 *
 * @return
 *        Number of free descriptors
 */
size_t sd_host_get_free_descriptors_count(sd_host_sdmmc_slot_t *slot);

/**
 * @brief Fill descriptor
 *
 * @param[in] slot       SD Host Slot handle
 * @param[in] num_desc   Number of descriptors to fill
 */
void sd_host_fill_dma_descriptors(sd_host_sdmmc_slot_t *slot, size_t num_desc);

/**
 * @brief Stop DMA
 *
 * @param[in] slot       SD Host Slot handle
 */
void sd_host_dma_stop(sd_host_sdmmc_slot_t *slot);

/**
 * @brief Resume DMA
 *
 * @param[in] slot       SD Host Slot handle
 */
void sd_host_dma_resume(sd_host_sdmmc_slot_t *slot);

/**
 * @brief Stop DMA
 *
 * @param[in] slot        SD Host Slot handle
 * @param[in] data_ptr    Pointer to data
 * @param[in] data_size   Size of data
 * @param[in] block_size  Block size
 */
void sd_host_dma_prepare(sd_host_sdmmc_slot_t *slot, void* data_ptr, size_t data_size, size_t block_size);

/*---------------------------------------------------------------
                        Info APIs
---------------------------------------------------------------*/
/**
 * @brief Check SD buffer alignment
 *
 * @param[in] slot        SD Host slot handle
 * @param[in] buf         Buffer pointer
 * @param[in] size        Buffer size
 *
 * @return
 *        - True: alignment requirement is satisfied
 *        - False: alignment requirement is not satisfied
 */
bool sd_host_check_buffer_alignment(sd_host_sdmmc_slot_t *slot, const void *buf, size_t size);

/**
 * @brief Get SD Host slot real frequency
 *
 * @param[in]  slot           SD Host Slot handle
 * @param[out] real_freq_khz  Real frequency in Hz
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t sd_host_slot_get_real_freq(sd_host_sdmmc_slot_t *slot, int *real_freq_khz);

/**
 * @brief Get SD Host slot real calculated frequency
 *
 * @note This is used to for protocol layer to get frequency
 *       SD Host driver will only make all registers valid when a real
 *       transaction is done (in `sd_host_slot_do_transaction`). Whereas protocol
 *       layer is querying real frequency in quite early stage
 *
 * @param[in]  slot           SD Host Slot handle
 * @param[out] real_freq_khz  Real frequency in Hz
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t sd_host_slot_get_calc_real_freq(sd_host_sdmmc_slot_t *slot, int *real_freq_khz);
#endif  //#if SOC_SDMMC_HOST_SUPPORTED

#ifdef __cplusplus
}
#endif
