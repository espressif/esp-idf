// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "soc/soc_caps.h"
#ifdef SOC_CAN_SUPPORTED

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_types.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_pm.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "driver/can.h"
#include "soc/can_periph.h"
#include "hal/can_hal.h"

/* ---------------------------- Definitions --------------------------------- */
//Internal Macros
#define CAN_CHECK(cond, ret_val) ({                                         \
            if (!(cond)) {                                                  \
                return (ret_val);                                           \
            }                                                               \
})
#define CAN_CHECK_FROM_CRIT(cond, ret_val) ({                               \
            if (!(cond)) {                                                  \
                CAN_EXIT_CRITICAL();                                        \
                return ret_val;                                             \
            }                                                               \
})
#define CAN_SET_FLAG(var, mask)     ((var) |= (mask))
#define CAN_RESET_FLAG(var, mask)   ((var) &= ~(mask))
#define CAN_TAG "CAN"

#define DRIVER_DEFAULT_INTERRUPTS   0xE7        //Exclude data overrun (bit[3]) and brp_div (bit[4])

//Control flags
#define CTRL_FLAG_STOPPED           0x001       //CAN peripheral in stopped state
#define CTRL_FLAG_RECOVERING        0x002       //Bus is undergoing bus recovery
#define CTRL_FLAG_ERR_WARN          0x004       //TEC or REC is >= error warning limit
#define CTRL_FLAG_ERR_PASSIVE       0x008       //TEC or REC is >= 128
#define CTRL_FLAG_BUS_OFF           0x010       //Bus-off due to TEC >= 256
#define CTRL_FLAG_TX_BUFF_OCCUPIED  0x020       //Transmit buffer is occupied

#define ALERT_LOG_LEVEL_WARNING     CAN_ALERT_ARB_LOST  //Alerts above and including this level use ESP_LOGW
#define ALERT_LOG_LEVEL_ERROR       CAN_ALERT_TX_FAILED //Alerts above and including this level use ESP_LOGE

/* ------------------ Typedefs, structures, and variables ------------------- */

//Control structure for CAN driver
typedef struct {
    //Control and status members
    uint32_t control_flags;
    can_mode_t mode;
    uint32_t rx_missed_count;
    uint32_t tx_failed_count;
    uint32_t arb_lost_count;
    uint32_t bus_error_count;
    intr_handle_t isr_handle;
    //TX and RX
    QueueHandle_t tx_queue;
    QueueHandle_t rx_queue;
    int tx_msg_count;
    int rx_msg_count;
    //Alerts
    SemaphoreHandle_t alert_semphr;
    uint32_t alerts_enabled;
    uint32_t alerts_triggered;
#ifdef CONFIG_PM_ENABLE
    //Power Management
    esp_pm_lock_handle_t pm_lock;
#endif
} can_obj_t;

static can_obj_t *p_can_obj = NULL;
static portMUX_TYPE can_spinlock = portMUX_INITIALIZER_UNLOCKED;
#define CAN_ENTER_CRITICAL_ISR()    portENTER_CRITICAL_ISR(&can_spinlock)
#define CAN_EXIT_CRITICAL_ISR()     portEXIT_CRITICAL_ISR(&can_spinlock)
#define CAN_ENTER_CRITICAL()        portENTER_CRITICAL(&can_spinlock)
#define CAN_EXIT_CRITICAL()         portEXIT_CRITICAL(&can_spinlock)

static can_hal_context_t can_context;

/* -------------------- Interrupt and Alert Handlers ------------------------ */

static void can_alert_handler(uint32_t alert_code, int *alert_req)
{
    if (p_can_obj->alerts_enabled & alert_code) {
        //Signify alert has occurred
        CAN_SET_FLAG(p_can_obj->alerts_triggered, alert_code);
        *alert_req = 1;
        if (p_can_obj->alerts_enabled & CAN_ALERT_AND_LOG) {
            if (alert_code >= ALERT_LOG_LEVEL_ERROR) {
                ESP_EARLY_LOGE(CAN_TAG, "Alert %d", alert_code);
            } else if (alert_code >= ALERT_LOG_LEVEL_WARNING) {
                ESP_EARLY_LOGW(CAN_TAG, "Alert %d", alert_code);
            } else {
                ESP_EARLY_LOGI(CAN_TAG, "Alert %d", alert_code);
            }
        }
    }
}

static inline void can_handle_bus_off(int *alert_req)
{
    //Bus-Off condition. TEC should set and held at 127, REC should be 0, reset mode entered
    CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_BUS_OFF);
    /* Note: REC is still allowed to increase during bus-off. REC > err_warn
       can prevent "bus recovery complete" interrupt from occurring. Set to
       listen only mode to freeze REC. */
    can_hal_handle_bus_off(&can_context);
    can_alert_handler(CAN_ALERT_BUS_OFF, alert_req);
}

static inline void can_handle_recovery_complete(int *alert_req)
{
    //Bus recovery complete.
    assert(can_hal_handle_bus_recov_cplt(&can_context));

    //Reset and set flags to the equivalent of the stopped state
    CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_RECOVERING | CTRL_FLAG_ERR_WARN |
                                             CTRL_FLAG_ERR_PASSIVE | CTRL_FLAG_BUS_OFF |
                                             CTRL_FLAG_TX_BUFF_OCCUPIED);
    CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_STOPPED);
    can_alert_handler(CAN_ALERT_BUS_RECOVERED, alert_req);
}

static inline void can_handle_recovery_in_progress(int * alert_req)
{
    //Bus-recovery in progress. TEC has dropped below error warning limit
    can_alert_handler(CAN_ALERT_RECOVERY_IN_PROGRESS, alert_req);
}

static inline void can_handle_above_ewl(int *alert_req)
{
    //TEC or REC surpassed error warning limit
    CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_ERR_WARN);
    can_alert_handler(CAN_ALERT_ABOVE_ERR_WARN, alert_req);
}

static inline void can_handle_below_ewl(int *alert_req)
{
    //TEC and REC are both below error warning
    CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_ERR_WARN);
    can_alert_handler(CAN_ALERT_BELOW_ERR_WARN, alert_req);
}

static inline void can_handle_error_passive(int *alert_req)
{
    //Entered error passive
    CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_ERR_PASSIVE);
    can_alert_handler(CAN_ALERT_ERR_PASS, alert_req);
}

static inline void can_handle_error_active(int *alert_req)
{
    //Returned to error active
    CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_ERR_PASSIVE);
    can_alert_handler(CAN_ALERT_ERR_ACTIVE, alert_req);
}

static inline void can_handle_bus_error(int *alert_req)
{
    // ECC register is read to re-arm bus error interrupt. ECC is not used
    can_hal_handle_bus_error(&can_context);
    p_can_obj->bus_error_count++;
    can_alert_handler(CAN_ALERT_BUS_ERROR, alert_req);
}

static inline void can_handle_arb_lost(int *alert_req)
{
    //ALC register is read to re-arm arb lost interrupt. ALC is not used
    can_hal_handle_arb_lost(&can_context);
    p_can_obj->arb_lost_count++;
    can_alert_handler(CAN_ALERT_ARB_LOST, alert_req);
}

static inline void can_handle_rx_buffer_frames(BaseType_t *task_woken, int *alert_req)
{
    uint32_t msg_count = can_hal_get_rx_msg_count(&can_context);

    for (int i = 0; i < msg_count; i++) {
        can_hal_frame_t frame;
        can_hal_read_rx_buffer_and_clear(&can_context, &frame);
        //Copy frame into RX Queue
        if (xQueueSendFromISR(p_can_obj->rx_queue, &frame, task_woken) == pdTRUE) {
            p_can_obj->rx_msg_count++;
        } else {
            p_can_obj->rx_missed_count++;
            can_alert_handler(CAN_ALERT_RX_QUEUE_FULL, alert_req);
        }
    }
    //Todo: Add Software Filters
    //Todo: Check for data overrun of RX FIFO, then trigger alert
}

static inline void can_handle_tx_buffer_frame(BaseType_t *task_woken, int *alert_req)
{
    //Handle previously transmitted frame
    if (can_hal_check_last_tx_successful(&can_context)) {
        can_alert_handler(CAN_ALERT_TX_SUCCESS, alert_req);
    } else {
        p_can_obj->tx_failed_count++;
        can_alert_handler(CAN_ALERT_TX_FAILED, alert_req);
    }

    //Update TX message count
    p_can_obj->tx_msg_count--;
    assert(p_can_obj->tx_msg_count >= 0);     //Sanity check

    //Check if there are more frames to transmit
    if (p_can_obj->tx_msg_count > 0 && p_can_obj->tx_queue != NULL) {
        can_hal_frame_t frame;
        int res = xQueueReceiveFromISR(p_can_obj->tx_queue, &frame, task_woken);
        if (res == pdTRUE) {
            can_hal_set_tx_buffer_and_transmit(&can_context, &frame);
        } else {
            assert(false && "failed to get a frame from TX queue");
        }
    } else {
        //No more frames to transmit
        CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_TX_BUFF_OCCUPIED);
        can_alert_handler(CAN_ALERT_TX_IDLE, alert_req);
    }
}

static void can_intr_handler_main(void *arg)
{
    BaseType_t task_woken = pdFALSE;
    int alert_req = 0;
    uint32_t event;
    CAN_ENTER_CRITICAL_ISR();
    if (p_can_obj == NULL) {    //Incase intr occurs whilst driver is being uninstalled
        CAN_EXIT_CRITICAL_ISR();
        return;
    }
    event = can_hal_decode_interrupt_events(&can_context, p_can_obj->control_flags & CTRL_FLAG_RECOVERING);

    if (event & CAN_HAL_EVENT_BUS_OFF) {
        can_handle_bus_off(&alert_req);
    }
    if (event & CAN_HAL_EVENT_BUS_RECOV_CPLT) {
        can_handle_recovery_complete(&alert_req);
    }
    if (event & CAN_HAL_EVENT_BUS_RECOV_PROGRESS) {
        can_handle_recovery_in_progress(&alert_req);
    }
    if (event & CAN_HAL_EVENT_ABOVE_EWL) {
        can_handle_above_ewl(&alert_req);
    }
    if (event & CAN_HAL_EVENT_BELOW_EWL) {
        can_handle_below_ewl(&alert_req);
    }
    if (event & CAN_HAL_EVENT_ERROR_PASSIVE) {
        can_handle_error_passive(&alert_req);
    }
    if (event & CAN_HAL_EVENT_ERROR_ACTIVE) {
        can_handle_error_active(&alert_req);
    }
    if (event & CAN_HAL_EVENT_BUS_ERR) {
        can_handle_bus_error(&alert_req);
    }
    if (event & CAN_HAL_EVENT_ARB_LOST) {
        can_handle_arb_lost(&alert_req);
    }
    if (event & CAN_HAL_EVENT_RX_BUFF_FRAME) {
        can_handle_rx_buffer_frames(&task_woken, &alert_req);
    }
    //TX command related handlers should be called last, so that other commands
    //do not overwrite the TX command related bits in the command register.
    if (event & CAN_HAL_EVENT_TX_BUFF_FREE) {
        can_handle_tx_buffer_frame(&task_woken, &alert_req);
    }
    CAN_EXIT_CRITICAL_ISR();

    if (p_can_obj->alert_semphr != NULL && alert_req) {
        //Give semaphore if alerts were triggered
        xSemaphoreGiveFromISR(p_can_obj->alert_semphr, &task_woken);
    }
    if (task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/* --------------------------- GPIO functions  ------------------------------ */

static void can_configure_gpio(gpio_num_t tx, gpio_num_t rx, gpio_num_t clkout, gpio_num_t bus_status)
{
    //Set TX pin
    gpio_set_pull_mode(tx, GPIO_FLOATING);
    gpio_matrix_out(tx, CAN_TX_IDX, false, false);
    gpio_pad_select_gpio(tx);

    //Set RX pin
    gpio_set_pull_mode(rx, GPIO_FLOATING);
    gpio_matrix_in(rx, CAN_RX_IDX, false);
    gpio_pad_select_gpio(rx);
    gpio_set_direction(rx, GPIO_MODE_INPUT);

    //Configure output clock pin (Optional)
    if (clkout >= 0 && clkout < GPIO_NUM_MAX) {
        gpio_set_pull_mode(clkout, GPIO_FLOATING);
        gpio_matrix_out(clkout, CAN_CLKOUT_IDX, false, false);
        gpio_pad_select_gpio(clkout);
    }

    //Configure bus status pin (Optional)
    if (bus_status >= 0 && bus_status < GPIO_NUM_MAX) {
        gpio_set_pull_mode(bus_status, GPIO_FLOATING);
        gpio_matrix_out(bus_status, CAN_BUS_OFF_ON_IDX, false, false);
        gpio_pad_select_gpio(bus_status);
    }
}

/* ---------------------------- Public Functions ---------------------------- */

esp_err_t can_driver_install(const can_general_config_t *g_config, const can_timing_config_t *t_config, const can_filter_config_t *f_config)
{
    //Check arguments
    CAN_CHECK(g_config != NULL, ESP_ERR_INVALID_ARG);
    CAN_CHECK(t_config != NULL, ESP_ERR_INVALID_ARG);
    CAN_CHECK(f_config != NULL, ESP_ERR_INVALID_ARG);
    CAN_CHECK(g_config->rx_queue_len > 0, ESP_ERR_INVALID_ARG);
    CAN_CHECK(g_config->tx_io >= 0 && g_config->tx_io < GPIO_NUM_MAX, ESP_ERR_INVALID_ARG);
    CAN_CHECK(g_config->rx_io >= 0 && g_config->rx_io < GPIO_NUM_MAX, ESP_ERR_INVALID_ARG);
    CAN_CHECK(CAN_BRP_IS_VALID(t_config->brp), ESP_ERR_INVALID_ARG);

    esp_err_t ret;
    can_obj_t *p_can_obj_dummy;

    //Create a CAN object
    p_can_obj_dummy = calloc(1, sizeof(can_obj_t));
    CAN_CHECK(p_can_obj_dummy != NULL, ESP_ERR_NO_MEM);

    //Initialize queues, semaphores, and power management locks
    p_can_obj_dummy->tx_queue = (g_config->tx_queue_len > 0) ? xQueueCreate(g_config->tx_queue_len, sizeof(can_hal_frame_t)) : NULL;
    p_can_obj_dummy->rx_queue = xQueueCreate(g_config->rx_queue_len, sizeof(can_hal_frame_t));
    p_can_obj_dummy->alert_semphr = xSemaphoreCreateBinary();
    if ((g_config->tx_queue_len > 0 && p_can_obj_dummy->tx_queue == NULL) ||
        p_can_obj_dummy->rx_queue == NULL || p_can_obj_dummy->alert_semphr == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
#ifdef CONFIG_PM_ENABLE
    esp_err_t pm_err = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "can", &(p_can_obj_dummy->pm_lock));
    if (pm_err != ESP_OK ) {
        ret = pm_err;
        goto err;
    }
#endif

    //Initialize flags and variables. All other members are 0 initialized by calloc()
    p_can_obj_dummy->control_flags = CTRL_FLAG_STOPPED;
    p_can_obj_dummy->mode = g_config->mode;
    p_can_obj_dummy->alerts_enabled = g_config->alerts_enabled;

    //Initialize CAN peripheral registers, and allocate interrupt
    CAN_ENTER_CRITICAL();
    if (p_can_obj == NULL) {
        p_can_obj = p_can_obj_dummy;
    } else {
        //Check if driver is already installed
        CAN_EXIT_CRITICAL();
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    periph_module_reset(PERIPH_CAN_MODULE);
    periph_module_enable(PERIPH_CAN_MODULE);            //Enable APB CLK to CAN peripheral
    assert(can_hal_init(&can_context));
    can_hal_configure(&can_context, t_config, f_config, DRIVER_DEFAULT_INTERRUPTS, g_config->clkout_divider);
    //Todo: Allow interrupt to be registered to specified CPU
    CAN_EXIT_CRITICAL();

    //Allocate GPIO and Interrupts
    can_configure_gpio(g_config->tx_io, g_config->rx_io, g_config->clkout_io, g_config->bus_off_io);
    ESP_ERROR_CHECK(esp_intr_alloc(ETS_CAN_INTR_SOURCE, 0, can_intr_handler_main, NULL, &p_can_obj->isr_handle));

#ifdef CONFIG_PM_ENABLE
    ESP_ERROR_CHECK(esp_pm_lock_acquire(p_can_obj->pm_lock));     //Acquire pm_lock to keep APB clock at 80MHz
#endif
    return ESP_OK;      //CAN module is still in reset mode, users need to call can_start() afterwards

    err:
    //Cleanup CAN object and return error
    if (p_can_obj_dummy != NULL) {
        if (p_can_obj_dummy->tx_queue != NULL) {
            vQueueDelete(p_can_obj_dummy->tx_queue);
            p_can_obj_dummy->tx_queue = NULL;
        }
        if (p_can_obj_dummy->rx_queue != NULL) {
            vQueueDelete(p_can_obj_dummy->rx_queue);
            p_can_obj_dummy->rx_queue = NULL;
        }
        if (p_can_obj_dummy->alert_semphr != NULL) {
            vSemaphoreDelete(p_can_obj_dummy->alert_semphr);
            p_can_obj_dummy->alert_semphr = NULL;
        }
#ifdef CONFIG_PM_ENABLE
        if (p_can_obj_dummy->pm_lock != NULL) {
            ESP_ERROR_CHECK(esp_pm_lock_delete(p_can_obj_dummy->pm_lock));
        }
#endif
        free(p_can_obj_dummy);
    }
    return ret;
}

esp_err_t can_driver_uninstall(void)
{
    can_obj_t *p_can_obj_dummy;

    CAN_ENTER_CRITICAL();
    //Check state
    CAN_CHECK_FROM_CRIT(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK_FROM_CRIT(p_can_obj->control_flags & (CTRL_FLAG_STOPPED | CTRL_FLAG_BUS_OFF), ESP_ERR_INVALID_STATE);
    //Todo: Add check to see if in reset mode.  //Enter reset mode to stop any CAN bus activity
    //Clear registers by reading
    can_hal_deinit(&can_context);
    periph_module_disable(PERIPH_CAN_MODULE);               //Disable CAN peripheral
    p_can_obj_dummy = p_can_obj;        //Use dummy to shorten critical section
    p_can_obj = NULL;
    CAN_EXIT_CRITICAL();

    ESP_ERROR_CHECK(esp_intr_free(p_can_obj_dummy->isr_handle));  //Free interrupt

    //Delete queues, semaphores, and power management locks
    if (p_can_obj_dummy->tx_queue != NULL) {
        vQueueDelete(p_can_obj_dummy->tx_queue);
    }
    vQueueDelete(p_can_obj_dummy->rx_queue);
    vSemaphoreDelete(p_can_obj_dummy->alert_semphr);
#ifdef CONFIG_PM_ENABLE
    //Release and delete power management lock
    ESP_ERROR_CHECK(esp_pm_lock_release(p_can_obj_dummy->pm_lock));
    ESP_ERROR_CHECK(esp_pm_lock_delete(p_can_obj_dummy->pm_lock));
#endif
    free(p_can_obj_dummy);        //Free can driver object

    return ESP_OK;
}

esp_err_t can_start(void)
{
    //Check state
    CAN_ENTER_CRITICAL();
    CAN_CHECK_FROM_CRIT(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK_FROM_CRIT(p_can_obj->control_flags & CTRL_FLAG_STOPPED, ESP_ERR_INVALID_STATE);

    //Reset RX queue, and RX message count
    xQueueReset(p_can_obj->rx_queue);
    p_can_obj->rx_msg_count = 0;
    //Todo: Add assert to see if in reset mode. //Should already be in bus-off mode, set again to make sure

    //Currently in listen only mode, need to set to mode specified by configuration
    assert(can_hal_start(&can_context, p_can_obj->mode));

    CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_STOPPED);
    CAN_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t can_stop(void)
{
    //Check state
    CAN_ENTER_CRITICAL();
    CAN_CHECK_FROM_CRIT(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK_FROM_CRIT(!(p_can_obj->control_flags & (CTRL_FLAG_STOPPED | CTRL_FLAG_BUS_OFF)), ESP_ERR_INVALID_STATE);

    assert(can_hal_stop(&can_context));

    CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_TX_BUFF_OCCUPIED);
    CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_STOPPED);

    //Reset TX Queue and message count
    if (p_can_obj->tx_queue != NULL) {
        xQueueReset(p_can_obj->tx_queue);
    }
    p_can_obj->tx_msg_count = 0;

    CAN_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t can_transmit(const can_message_t *message, TickType_t ticks_to_wait)
{
    //Check arguments
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK(message != NULL, ESP_ERR_INVALID_ARG);
    CAN_CHECK((message->data_length_code <= CAN_FRAME_MAX_DLC) || message->dlc_non_comp, ESP_ERR_INVALID_ARG);

    CAN_ENTER_CRITICAL();
    //Check State
    CAN_CHECK_FROM_CRIT(!(p_can_obj->mode == CAN_MODE_LISTEN_ONLY), ESP_ERR_NOT_SUPPORTED);
    CAN_CHECK_FROM_CRIT(!(p_can_obj->control_flags & (CTRL_FLAG_STOPPED | CTRL_FLAG_BUS_OFF)), ESP_ERR_INVALID_STATE);
    //Format frame
    esp_err_t ret = ESP_FAIL;
    can_hal_frame_t tx_frame;
    can_hal_format_frame(message, &tx_frame);

    //Check if frame can be sent immediately
    if ((p_can_obj->tx_msg_count == 0) && !(p_can_obj->control_flags & CTRL_FLAG_TX_BUFF_OCCUPIED)) {
        //No other frames waiting to transmit. Bypass queue and transmit immediately
        can_hal_set_tx_buffer_and_transmit(&can_context, &tx_frame);
        p_can_obj->tx_msg_count++;
        CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_TX_BUFF_OCCUPIED);
        ret = ESP_OK;
    }
    CAN_EXIT_CRITICAL();

    if (ret != ESP_OK) {
        if (p_can_obj->tx_queue == NULL) {
            //TX Queue is disabled and TX buffer is occupied, message was not sent
            ret = ESP_FAIL;
        } else if (xQueueSend(p_can_obj->tx_queue, &tx_frame, ticks_to_wait) == pdTRUE) {
            //Copied to TX Queue
            CAN_ENTER_CRITICAL();
            if (p_can_obj->control_flags & (CTRL_FLAG_STOPPED | CTRL_FLAG_BUS_OFF)) {
                //TX queue was reset (due to stop/bus_off), remove copied frame from queue to prevent transmission
                int res = xQueueReceive(p_can_obj->tx_queue, &tx_frame, 0);
                assert(res == pdTRUE);
                ret = ESP_ERR_INVALID_STATE;
            } else if ((p_can_obj->tx_msg_count == 0) && !(p_can_obj->control_flags & CTRL_FLAG_TX_BUFF_OCCUPIED)) {
                //TX buffer was freed during copy, manually trigger transmission
                int res = xQueueReceive(p_can_obj->tx_queue, &tx_frame, 0);
                assert(res == pdTRUE);
                can_hal_set_tx_buffer_and_transmit(&can_context, &tx_frame);
                p_can_obj->tx_msg_count++;
                CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_TX_BUFF_OCCUPIED);
                ret = ESP_OK;
            } else {
                //Frame was copied to queue, waiting to be transmitted
                p_can_obj->tx_msg_count++;
                ret = ESP_OK;
            }
            CAN_EXIT_CRITICAL();
        } else {
            //Timed out waiting for free space on TX queue
            ret = ESP_ERR_TIMEOUT;
        }
    }
    return ret;
}

esp_err_t can_receive(can_message_t *message, TickType_t ticks_to_wait)
{
    //Check arguments and state
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK(message != NULL, ESP_ERR_INVALID_ARG);

    //Get frame from RX Queue or RX Buffer
    can_hal_frame_t rx_frame;
    if (xQueueReceive(p_can_obj->rx_queue, &rx_frame, ticks_to_wait) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    CAN_ENTER_CRITICAL();
    p_can_obj->rx_msg_count--;
    CAN_EXIT_CRITICAL();

    //Decode frame
    can_hal_parse_frame(&rx_frame, message);
    return ESP_OK;
}

esp_err_t can_read_alerts(uint32_t *alerts, TickType_t ticks_to_wait)
{
    //Check arguments and state
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK(alerts != NULL, ESP_ERR_INVALID_ARG);

    //Wait for an alert to occur
    if (xSemaphoreTake(p_can_obj->alert_semphr, ticks_to_wait) == pdTRUE) {
        CAN_ENTER_CRITICAL();
        *alerts = p_can_obj->alerts_triggered;
        p_can_obj->alerts_triggered = 0;    //Clear triggered alerts
        CAN_EXIT_CRITICAL();
        return ESP_OK;
    } else {
        *alerts = 0;
        return ESP_ERR_TIMEOUT;
    }
}

esp_err_t can_reconfigure_alerts(uint32_t alerts_enabled, uint32_t *current_alerts)
{
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);

    CAN_ENTER_CRITICAL();
    //Clear any unhandled alerts
    if (current_alerts != NULL) {
        *current_alerts = p_can_obj->alerts_triggered;;
    }
    p_can_obj->alerts_triggered = 0;
    p_can_obj->alerts_enabled = alerts_enabled;         //Update enabled alerts
    CAN_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t can_initiate_recovery(void)
{
    CAN_ENTER_CRITICAL();
    //Check state
    CAN_CHECK_FROM_CRIT(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK_FROM_CRIT(p_can_obj->control_flags & CTRL_FLAG_BUS_OFF, ESP_ERR_INVALID_STATE);
    CAN_CHECK_FROM_CRIT(!(p_can_obj->control_flags & CTRL_FLAG_RECOVERING), ESP_ERR_INVALID_STATE);

    //Reset TX Queue/Counters
    if (p_can_obj->tx_queue != NULL) {
        xQueueReset(p_can_obj->tx_queue);
    }
    p_can_obj->tx_msg_count = 0;
    CAN_RESET_FLAG(p_can_obj->control_flags, CTRL_FLAG_TX_BUFF_OCCUPIED);
    CAN_SET_FLAG(p_can_obj->control_flags, CTRL_FLAG_RECOVERING);

    //Trigger start of recovery process
    assert(can_hal_start_bus_recovery(&can_context));
    CAN_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t can_get_status_info(can_status_info_t *status_info)
{
    //Check parameters and state
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK(status_info != NULL, ESP_ERR_INVALID_ARG);

    CAN_ENTER_CRITICAL();
    status_info->tx_error_counter = can_hal_get_tec(&can_context);
    status_info->rx_error_counter = can_hal_get_rec(&can_context);
    status_info->msgs_to_tx = p_can_obj->tx_msg_count;
    status_info->msgs_to_rx = p_can_obj->rx_msg_count;
    status_info->tx_failed_count = p_can_obj->tx_failed_count;
    status_info->rx_missed_count = p_can_obj->rx_missed_count;
    status_info->arb_lost_count = p_can_obj->arb_lost_count;
    status_info->bus_error_count = p_can_obj->bus_error_count;
    if (p_can_obj->control_flags & CTRL_FLAG_RECOVERING) {
        status_info->state = CAN_STATE_RECOVERING;
    } else if (p_can_obj->control_flags & CTRL_FLAG_BUS_OFF) {
        status_info->state = CAN_STATE_BUS_OFF;
    } else if (p_can_obj->control_flags & CTRL_FLAG_STOPPED) {
        status_info->state = CAN_STATE_STOPPED;
    } else {
        status_info->state = CAN_STATE_RUNNING;
    }
    CAN_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t can_clear_transmit_queue(void)
{
    //Check State
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);
    CAN_CHECK(p_can_obj->tx_queue != NULL, ESP_ERR_NOT_SUPPORTED);

    CAN_ENTER_CRITICAL();
    //If a message is currently undergoing transmission, the tx interrupt handler will decrement tx_msg_count
    p_can_obj->tx_msg_count = (p_can_obj->control_flags & CTRL_FLAG_TX_BUFF_OCCUPIED) ? 1 : 0;
    xQueueReset(p_can_obj->tx_queue);
    CAN_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t can_clear_receive_queue(void)
{
    //Check State
    CAN_CHECK(p_can_obj != NULL, ESP_ERR_INVALID_STATE);

    CAN_ENTER_CRITICAL();
    p_can_obj->rx_msg_count = 0;
    xQueueReset(p_can_obj->rx_queue);
    CAN_EXIT_CRITICAL();

    return ESP_OK;
}
#endif
