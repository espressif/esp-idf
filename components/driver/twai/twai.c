/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/idf_additions.h"
#include "esp_types.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_pm.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_clk_tree.h"
#include "clk_ctrl_os.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "esp_private/gpio.h"
#include "driver/twai.h"
#include "soc/soc_caps.h"
#include "soc/soc.h"
#include "soc/io_mux_reg.h"
#include "soc/twai_periph.h"
#include "hal/twai_hal.h"
#include "esp_rom_gpio.h"
#if SOC_TWAI_SUPPORT_SLEEP_RETENTION
#include "esp_private/sleep_retention.h"
#endif

/* ---------------------------- Definitions --------------------------------- */
//Internal Macros
#define TWAI_TAG "TWAI"
#define TWAI_CHECK(cond, ret_val) ({                                        \
            if (!(cond)) {                                                  \
                return (ret_val);                                           \
            }                                                               \
})

#define TWAI_SET_FLAG(var, mask)    ((var) |= (mask))
#define TWAI_RESET_FLAG(var, mask)  ((var) &= ~(mask))

#ifdef CONFIG_TWAI_ISR_IN_IRAM
#define TWAI_MALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define TWAI_MALLOC_CAPS    MALLOC_CAP_DEFAULT
#endif  //CONFIG_TWAI_ISR_IN_IRAM

#define DRIVER_DEFAULT_INTERRUPTS   0xE7        //Exclude data overrun (bit[3]) and brp_div (bit[4])

#define ALERT_LOG_LEVEL_WARNING     TWAI_ALERT_ARB_LOST  //Alerts above and including this level use ESP_LOGW
#define ALERT_LOG_LEVEL_ERROR       TWAI_ALERT_TX_FAILED //Alerts above and including this level use ESP_LOGE

#if !SOC_RCC_IS_INDEPENDENT
#define TWAI_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define TWAI_RCC_ATOMIC()
#endif

#if SOC_PERIPH_CLK_CTRL_SHARED
#define TWAI_PERI_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define TWAI_PERI_ATOMIC()
#endif

#define TWAI_USE_RETENTION_LINK  (SOC_TWAI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)

/* ------------------ Typedefs, structures, and variables ------------------- */

//Control structure for TWAI driver
typedef struct twai_obj_t {
    int controller_id;
    twai_hal_context_t hal;  // hal context
    //Control and status members
    twai_state_t state;
    twai_mode_t mode;
    uint32_t rx_missed_count;
    uint32_t rx_overrun_count;
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
    //Power Management Lock
    esp_pm_lock_handle_t pm_lock;
    portMUX_TYPE spinlock;
} twai_obj_t;

static twai_handle_t g_twai_objs[SOC_TWAI_CONTROLLER_NUM];
static portMUX_TYPE g_spinlock = portMUX_INITIALIZER_UNLOCKED;

/* -------------------- Sleep Retention ------------------------ */
#if TWAI_USE_RETENTION_LINK
static esp_err_t s_twai_create_sleep_retention_link_cb(void *obj);
#endif

/* -------------------- Interrupt and Alert Handlers ------------------------ */

static void twai_alert_handler(twai_obj_t *p_twai_obj, uint32_t alert_code, int *alert_req)
{
    if (p_twai_obj->alerts_enabled & alert_code) {
        //Signify alert has occurred
        TWAI_SET_FLAG(p_twai_obj->alerts_triggered, alert_code);
        *alert_req = 1;
#ifndef CONFIG_TWAI_ISR_IN_IRAM     //Only log if ISR is not in IRAM
        if (p_twai_obj->alerts_enabled & TWAI_ALERT_AND_LOG) {
            if (alert_code >= ALERT_LOG_LEVEL_ERROR) {
                ESP_EARLY_LOGE(TWAI_TAG, "Alert %" PRIu32, alert_code);
            } else if (alert_code >= ALERT_LOG_LEVEL_WARNING) {
                ESP_EARLY_LOGW(TWAI_TAG, "Alert %" PRIu32, alert_code);
            } else {
                ESP_EARLY_LOGI(TWAI_TAG, "Alert %" PRIu32, alert_code);
            }
        }
#endif  //CONFIG_TWAI_ISR_IN_IRAM
    }
}

static inline void twai_handle_rx_buffer_frames(twai_obj_t *p_twai_obj, BaseType_t *task_woken, int *alert_req)
{
#ifdef SOC_TWAI_SUPPORTS_RX_STATUS
    uint32_t msg_count = twai_hal_get_rx_msg_count(&p_twai_obj->hal);

    for (uint32_t i = 0; i < msg_count; i++) {
        twai_hal_frame_t frame;
        if (twai_hal_read_rx_buffer_and_clear(&p_twai_obj->hal, &frame)) {
            //Valid frame copied from RX buffer
            if (xQueueSendFromISR(p_twai_obj->rx_queue, &frame, task_woken) == pdTRUE) {
                p_twai_obj->rx_msg_count++;
                twai_alert_handler(p_twai_obj, TWAI_ALERT_RX_DATA, alert_req);
            } else {    //Failed to send to queue
                p_twai_obj->rx_missed_count++;
                twai_alert_handler(p_twai_obj, TWAI_ALERT_RX_QUEUE_FULL, alert_req);
            }
        } else {    //Failed to read from RX buffer because message is overrun
            p_twai_obj->rx_overrun_count++;
            twai_alert_handler(p_twai_obj, TWAI_ALERT_RX_FIFO_OVERRUN, alert_req);
        }
    }
#else   //SOC_TWAI_SUPPORTS_RX_STATUS
    uint32_t msg_count = twai_hal_get_rx_msg_count(&p_twai_obj->hal);
    bool overrun = false;
    //Clear all valid RX frames
    for (int i = 0; i < msg_count; i++) {
        twai_hal_frame_t frame;
        if (twai_hal_read_rx_buffer_and_clear(&p_twai_obj->hal, &frame)) {
            //Valid frame copied from RX buffer
            if (xQueueSendFromISR(p_twai_obj->rx_queue, &frame, task_woken) == pdTRUE) {
                p_twai_obj->rx_msg_count++;
                twai_alert_handler(p_twai_obj, TWAI_ALERT_RX_DATA, alert_req);
            } else {
                p_twai_obj->rx_missed_count++;
                twai_alert_handler(p_twai_obj, TWAI_ALERT_RX_QUEUE_FULL, alert_req);
            }
        } else {
            overrun = true;
            break;
        }
    }
    //All remaining frames are treated as overrun. Clear them all
    if (overrun) {
        p_twai_obj->rx_overrun_count += twai_hal_clear_rx_fifo_overrun(&p_twai_obj->hal);
        twai_alert_handler(p_twai_obj, TWAI_ALERT_RX_FIFO_OVERRUN, alert_req);
    }
#endif  //SOC_TWAI_SUPPORTS_RX_STATUS
}

static inline void twai_handle_tx_buffer_frame(twai_obj_t *p_twai_obj, BaseType_t *task_woken, int *alert_req)
{
    //Handle previously transmitted frame
    if (twai_hal_check_last_tx_successful(&p_twai_obj->hal)) {
        twai_alert_handler(p_twai_obj, TWAI_ALERT_TX_SUCCESS, alert_req);
    } else {
        p_twai_obj->tx_failed_count++;
        twai_alert_handler(p_twai_obj, TWAI_ALERT_TX_FAILED, alert_req);
    }

    //Update TX message count
    p_twai_obj->tx_msg_count--;
    assert(p_twai_obj->tx_msg_count >= 0);      //Sanity check

    //Check if there are more frames to transmit
    if (p_twai_obj->tx_msg_count > 0 && p_twai_obj->tx_queue != NULL) {
        twai_hal_frame_t frame;
        int res = xQueueReceiveFromISR(p_twai_obj->tx_queue, &frame, task_woken);
        if (res == pdTRUE) {
            twai_hal_set_tx_buffer_and_transmit(&p_twai_obj->hal, &frame);
        } else {
            assert(false && "failed to get a frame from TX queue");
        }
    } else {
        //No more frames to transmit
        twai_alert_handler(p_twai_obj, TWAI_ALERT_TX_IDLE, alert_req);
    }
}

static void twai_intr_handler_main(void *arg)
{
    twai_obj_t *p_twai_obj = (twai_obj_t *)arg;
    BaseType_t task_woken = pdFALSE;
    int alert_req = 0;
    uint32_t events;
    portENTER_CRITICAL_ISR(&p_twai_obj->spinlock);
    events = twai_hal_get_events(&p_twai_obj->hal);    //Get the events that triggered the interrupt

#if defined(CONFIG_TWAI_ERRATA_FIX_RX_FRAME_INVALID) || defined(CONFIG_TWAI_ERRATA_FIX_RX_FIFO_CORRUPT)
    if (events & TWAI_HAL_EVENT_NEED_PERIPH_RESET) {
        twai_hal_prepare_for_reset(&p_twai_obj->hal);
        TWAI_RCC_ATOMIC() {
            twai_ll_reset_register(p_twai_obj->controller_id);
        }
        twai_hal_recover_from_reset(&p_twai_obj->hal);
        p_twai_obj->rx_missed_count += twai_hal_get_reset_lost_rx_cnt(&p_twai_obj->hal);
        twai_alert_handler(p_twai_obj, TWAI_ALERT_PERIPH_RESET, &alert_req);
    }
#endif
    if (events & TWAI_HAL_EVENT_RX_BUFF_FRAME) {
        //Note: This event will never occur if there is a periph reset event
        twai_handle_rx_buffer_frames(p_twai_obj, &task_woken, &alert_req);
    }
    if (events & TWAI_HAL_EVENT_TX_BUFF_FREE) {
        twai_handle_tx_buffer_frame(p_twai_obj, &task_woken, &alert_req);
    }

    //Handle events that only require alerting (i.e. no handler)
    if (events & TWAI_HAL_EVENT_BUS_OFF) {
        p_twai_obj->state = TWAI_STATE_BUS_OFF;
        twai_alert_handler(p_twai_obj, TWAI_ALERT_BUS_OFF, &alert_req);
    }
    if (events & TWAI_HAL_EVENT_BUS_RECOV_CPLT) {
        p_twai_obj->state = TWAI_STATE_STOPPED;
        twai_alert_handler(p_twai_obj, TWAI_ALERT_BUS_RECOVERED, &alert_req);
    }
    if (events & TWAI_HAL_EVENT_BUS_ERR) {
        p_twai_obj->bus_error_count++;
        twai_alert_handler(p_twai_obj, TWAI_ALERT_BUS_ERROR, &alert_req);
    }
    if (events & TWAI_HAL_EVENT_ARB_LOST) {
        p_twai_obj->arb_lost_count++;
        twai_alert_handler(p_twai_obj, TWAI_ALERT_ARB_LOST, &alert_req);
    }
    if (events & TWAI_HAL_EVENT_BUS_RECOV_PROGRESS) {
        //Bus-recovery in progress. TEC has dropped below error warning limit
        twai_alert_handler(p_twai_obj, TWAI_ALERT_RECOVERY_IN_PROGRESS, &alert_req);
    }
    if (events & TWAI_HAL_EVENT_ERROR_PASSIVE) {
        //Entered error passive
        twai_alert_handler(p_twai_obj, TWAI_ALERT_ERR_PASS, &alert_req);
    }
    if (events & TWAI_HAL_EVENT_ERROR_ACTIVE) {
        //Returned to error active
        twai_alert_handler(p_twai_obj, TWAI_ALERT_ERR_ACTIVE, &alert_req);
    }
    if (events & TWAI_HAL_EVENT_ABOVE_EWL) {
        //TEC or REC surpassed error warning limit
        twai_alert_handler(p_twai_obj, TWAI_ALERT_ABOVE_ERR_WARN, &alert_req);
    }
    if (events & TWAI_HAL_EVENT_BELOW_EWL) {
        //TEC and REC are both below error warning
        twai_alert_handler(p_twai_obj, TWAI_ALERT_BELOW_ERR_WARN, &alert_req);
    }

    portEXIT_CRITICAL_ISR(&p_twai_obj->spinlock);

    if (p_twai_obj->alert_semphr != NULL && alert_req) {
        //Give semaphore if alerts were triggered
        xSemaphoreGiveFromISR(p_twai_obj->alert_semphr, &task_woken);
    }
    if (task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/* -------------------------- Helper functions  ----------------------------- */

static void twai_configure_gpio(int controller_id, gpio_num_t tx, gpio_num_t rx, gpio_num_t clkout, gpio_num_t bus_status)
{
    // assert the GPIO number is not a negative number (shift operation on a negative number is undefined)
    assert(tx >= 0 && rx >= 0);

    //Set RX pin
    gpio_func_sel(rx, PIN_FUNC_GPIO);
    gpio_input_enable(rx);
    esp_rom_gpio_connect_in_signal(rx, twai_controller_periph_signals.controllers[controller_id].rx_sig, false);

    //Set TX pin
    gpio_func_sel(tx, PIN_FUNC_GPIO);
    esp_rom_gpio_connect_out_signal(tx, twai_controller_periph_signals.controllers[controller_id].tx_sig, false, false);

    //Configure output clock pin (Optional)
    if (clkout >= 0 && clkout < GPIO_NUM_MAX) {
        gpio_func_sel(clkout, PIN_FUNC_GPIO);
        esp_rom_gpio_connect_out_signal(clkout, twai_controller_periph_signals.controllers[controller_id].clk_out_sig, false, false);
    }

    //Configure bus status pin (Optional)
    if (bus_status >= 0 && bus_status < GPIO_NUM_MAX) {
        gpio_func_sel(bus_status, PIN_FUNC_GPIO);
        esp_rom_gpio_connect_out_signal(bus_status, twai_controller_periph_signals.controllers[controller_id].bus_off_sig, false, false);
    }
}

static void twai_free_driver_obj(twai_obj_t *p_obj)
{
    //Free driver object and any dependent SW resources it uses (queues, semaphores, interrupts, PM locks etc)
#if CONFIG_PM_ENABLE
    if (p_obj->pm_lock != NULL) {
        ESP_ERROR_CHECK(esp_pm_lock_delete(p_obj->pm_lock));
    }
#endif //CONFIG_PM_ENABLE
    if (p_obj->isr_handle) {
        ESP_ERROR_CHECK(esp_intr_free(p_obj->isr_handle));
    }
    //Delete queues and semaphores
    if (p_obj->tx_queue != NULL) {
        vQueueDeleteWithCaps(p_obj->tx_queue);
    }
    if (p_obj->rx_queue != NULL) {
        vQueueDeleteWithCaps(p_obj->rx_queue);
    }
    if (p_obj->alert_semphr != NULL) {
        vSemaphoreDeleteWithCaps(p_obj->alert_semphr);
    }

#if TWAI_USE_RETENTION_LINK
    const periph_retention_module_t retention_id = twai_reg_retention_info[p_obj->controller_id].module_id;
    if (sleep_retention_is_module_created(retention_id)) {
        assert(sleep_retention_is_module_inited(retention_id));
        sleep_retention_module_free(retention_id);
    }
    if (sleep_retention_is_module_inited(retention_id)) {
        sleep_retention_module_deinit(retention_id);
    }

#endif

    heap_caps_free(p_obj);
}

static esp_err_t twai_alloc_driver_obj(const twai_general_config_t *g_config, twai_clock_source_t clk_src, int controller_id,  twai_obj_t **p_twai_obj_ret)
{
    //Allocate driver object and any dependent SW resources it uses (queues, semaphores, interrupts, PM locks etc)
    esp_err_t ret;

    //Create a TWAI driver object
    twai_obj_t *p_obj = heap_caps_calloc(1, sizeof(twai_obj_t), TWAI_MALLOC_CAPS);
    if (p_obj == NULL) {
        return ESP_ERR_NO_MEM;
    }
    if (g_config->tx_queue_len > 0) {
        p_obj->tx_queue = xQueueCreateWithCaps(g_config->tx_queue_len, sizeof(twai_hal_frame_t), TWAI_MALLOC_CAPS);
    }
    p_obj->rx_queue = xQueueCreateWithCaps(g_config->rx_queue_len, sizeof(twai_hal_frame_t), TWAI_MALLOC_CAPS);
    p_obj->alert_semphr = xSemaphoreCreateBinaryWithCaps(TWAI_MALLOC_CAPS);
    if ((g_config->tx_queue_len > 0 && p_obj->tx_queue == NULL) || p_obj->rx_queue == NULL || p_obj->alert_semphr == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }
    //Allocate interrupt
    ret = esp_intr_alloc(twai_controller_periph_signals.controllers[controller_id].irq_id,
                         g_config->intr_flags | ESP_INTR_FLAG_INTRDISABLED,
                         twai_intr_handler_main,
                         p_obj,
                         &p_obj->isr_handle);
    if (ret != ESP_OK) {
        goto err;
    }

    p_obj->controller_id = controller_id;

#if CONFIG_PM_ENABLE
#if SOC_TWAI_CLK_SUPPORT_APB
    // DFS can change APB frequency. So add lock to prevent sleep and APB freq from changing
    if (clk_src == TWAI_CLK_SRC_APB) {
        // TODO: pm_lock name should also reflect the controller ID
        ret = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "twai", &(p_obj->pm_lock));
        if (ret != ESP_OK) {
            goto err;
        }
    }
#else // XTAL
    // XTAL freq can be closed in light sleep, so we need to create a lock to prevent light sleep
    ret = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "twai", &(p_obj->pm_lock));
    if (ret != ESP_OK) {
        goto err;
    }
#endif //SOC_TWAI_CLK_SUPPORT_APB
#endif //CONFIG_PM_ENABLE

#if TWAI_USE_RETENTION_LINK
    sleep_retention_module_t module = twai_reg_retention_info[controller_id].module_id;
    sleep_retention_module_init_param_t init_param = {
        .cbs = {
            .create = {
                .handle = s_twai_create_sleep_retention_link_cb,
                .arg = p_obj,
            },
        },
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
    };
    if (sleep_retention_module_init(module, &init_param) != ESP_OK) {
        ESP_LOGW(TWAI_TAG, "init sleep retention failed for TWAI%d, power domain may be turned off during sleep", controller_id);
    }

    if (g_config->general_flags.sleep_allow_pd) {
        if (sleep_retention_module_allocate(module) != ESP_OK) {
            ESP_LOGW(TWAI_TAG, "create retention module failed, power domain can't turn off");
        }
    }
#endif

    *p_twai_obj_ret = p_obj;
    return ESP_OK;

err:
    twai_free_driver_obj(p_obj);
    return ret;
}

/* ---------------------------- Public Functions ---------------------------- */
esp_err_t twai_driver_install_v2(const twai_general_config_t *g_config, const twai_timing_config_t *t_config, const twai_filter_config_t *f_config, twai_handle_t *ret_twai)
{
    //Check arguments
    TWAI_CHECK(g_config != NULL, ESP_ERR_INVALID_ARG);
    TWAI_CHECK(t_config != NULL, ESP_ERR_INVALID_ARG);
    TWAI_CHECK(f_config != NULL, ESP_ERR_INVALID_ARG);
    TWAI_CHECK(g_config->controller_id < SOC_TWAI_CONTROLLER_NUM, ESP_ERR_INVALID_ARG);
    TWAI_CHECK(g_config->rx_queue_len > 0, ESP_ERR_INVALID_ARG);
    TWAI_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(g_config->tx_io), ESP_ERR_INVALID_ARG);
    TWAI_CHECK(GPIO_IS_VALID_GPIO(g_config->rx_io), ESP_ERR_INVALID_ARG);
#ifndef CONFIG_TWAI_ISR_IN_IRAM
    TWAI_CHECK(!(g_config->intr_flags & ESP_INTR_FLAG_IRAM), ESP_ERR_INVALID_ARG);
#endif
    int controller_id = g_config->controller_id;
    TWAI_CHECK(g_twai_objs[controller_id] == NULL, ESP_ERR_INVALID_STATE);
#if !SOC_TWAI_SUPPORT_SLEEP_RETENTION
    TWAI_CHECK(!g_config->general_flags.sleep_allow_pd, ESP_ERR_INVALID_ARG);
#endif

    //Get clock source resolution
    uint32_t clock_source_hz = 0;
    twai_clock_source_t clk_src = t_config->clk_src;
    //Fall back to default clock source
    if (clk_src == 0) {
        clk_src = TWAI_CLK_SRC_DEFAULT;
    }
    esp_clk_tree_src_get_freq_hz(clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clock_source_hz);

    //Check brp validation
    uint32_t brp = t_config->brp;
    if (t_config->quanta_resolution_hz) {
        TWAI_CHECK(clock_source_hz % t_config->quanta_resolution_hz == 0, ESP_ERR_INVALID_ARG);
        brp = clock_source_hz / t_config->quanta_resolution_hz;
    }
    TWAI_CHECK(twai_ll_check_brp_validation(brp), ESP_ERR_INVALID_ARG);

    esp_err_t ret;
    twai_obj_t *p_twai_obj;

    //Create a TWAI object (including queues, semaphores, interrupts, and PM locks)
    ret = twai_alloc_driver_obj(g_config, clk_src, controller_id, &p_twai_obj);
    if (ret != ESP_OK) {
        return ret;
    }

    //Initialize flags and variables. All other members are already set to zero by twai_alloc_driver_obj()
    portMUX_INITIALIZE(&p_twai_obj->spinlock);
    p_twai_obj->state = TWAI_STATE_STOPPED;
    p_twai_obj->mode = g_config->mode;
    p_twai_obj->alerts_enabled = g_config->alerts_enabled;

    //Assign the TWAI object
    portENTER_CRITICAL(&g_spinlock);
    if (g_twai_objs[controller_id] == NULL) {
        g_twai_objs[controller_id] = p_twai_obj;
    } else {
        //Check if driver is already installed
        portEXIT_CRITICAL(&g_spinlock);
        ret = ESP_ERR_INVALID_STATE;
        goto err;
    }
    portEXIT_CRITICAL(&g_spinlock);

    //Enable TWAI peripheral register clock
    TWAI_RCC_ATOMIC() {
        twai_ll_enable_bus_clock(controller_id, true);
        twai_ll_reset_register(controller_id);
    }
    TWAI_PERI_ATOMIC() {
        //Enable functional clock
        twai_ll_set_clock_source(p_twai_obj->controller_id, clk_src);
        twai_ll_enable_clock(p_twai_obj->controller_id, true);
    }

    //Initialize TWAI HAL layer
    twai_hal_config_t hal_config = {
        .clock_source_hz = clock_source_hz,
        .controller_id = controller_id,
    };
    bool res = twai_hal_init(&p_twai_obj->hal, &hal_config);
    assert(res);
    twai_hal_configure(&p_twai_obj->hal, t_config, f_config, DRIVER_DEFAULT_INTERRUPTS, g_config->clkout_divider);

    //Assign GPIO and Interrupts
    twai_configure_gpio(controller_id, g_config->tx_io, g_config->rx_io, g_config->clkout_io, g_config->bus_off_io);

#if CONFIG_PM_ENABLE
    //Acquire PM lock
    if (p_twai_obj->pm_lock) {
        ESP_ERROR_CHECK(esp_pm_lock_acquire(p_twai_obj->pm_lock));     //Acquire pm_lock during the whole driver lifetime
    }
#endif //CONFIG_PM_ENABLE

    //Enable interrupt
    ESP_ERROR_CHECK(esp_intr_enable(p_twai_obj->isr_handle));

    *ret_twai = p_twai_obj;

    return ESP_OK;      //TWAI module is still in reset mode, users need to call twai_start() afterwards

err:
    twai_free_driver_obj(p_twai_obj);
    return ret;
}

esp_err_t twai_driver_install(const twai_general_config_t *g_config, const twai_timing_config_t *t_config, const twai_filter_config_t *f_config)
{
    // the handle-less driver API only supports one TWAI controller, i.e. the g_twai_objs[0]
    TWAI_CHECK(g_config->controller_id == 0, ESP_ERR_INVALID_ARG);
    return twai_driver_install_v2(g_config, t_config, f_config, &g_twai_objs[0]);
}

esp_err_t twai_driver_uninstall_v2(twai_handle_t handle)
{
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);
    int controller_id = handle->controller_id;
    twai_obj_t *p_twai_obj;

    portENTER_CRITICAL(&g_spinlock);
    //Check state
    if (g_twai_objs[controller_id] == NULL) {
        portEXIT_CRITICAL(&g_spinlock);
        return ESP_ERR_INVALID_STATE;
    }
    p_twai_obj = g_twai_objs[controller_id];
    if (!(p_twai_obj->state == TWAI_STATE_STOPPED || p_twai_obj->state == TWAI_STATE_BUS_OFF)) {
        portEXIT_CRITICAL(&g_spinlock);
        return ESP_ERR_INVALID_STATE;
    }
    g_twai_objs[controller_id] = NULL;
    portEXIT_CRITICAL(&g_spinlock);

    //Clear registers by reading
    twai_hal_deinit(&p_twai_obj->hal);
    TWAI_PERI_ATOMIC() {
        twai_ll_enable_clock(controller_id, false);
    }
    TWAI_RCC_ATOMIC() {
        twai_ll_enable_bus_clock(controller_id, false);
    }

#if CONFIG_PM_ENABLE
    if (p_twai_obj->pm_lock) {
        //Release and delete power management lock
        ESP_ERROR_CHECK(esp_pm_lock_release(p_twai_obj->pm_lock));
    }
#endif //CONFIG_PM_ENABLE

    //Disable interrupt
    ESP_ERROR_CHECK(esp_intr_disable(p_twai_obj->isr_handle));
    //Free twai driver object
    twai_free_driver_obj(p_twai_obj);
    return ESP_OK;
}

esp_err_t twai_driver_uninstall(void)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_driver_uninstall_v2(g_twai_objs[0]);
}

esp_err_t twai_start_v2(twai_handle_t handle)
{
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);
    twai_obj_t *p_twai_obj = handle;

    portENTER_CRITICAL(&handle->spinlock);
    if (p_twai_obj->state != TWAI_STATE_STOPPED) {
        portEXIT_CRITICAL(&handle->spinlock);
        return ESP_ERR_INVALID_STATE;
    }

    //Reset RX queue, RX message count, amd TX queue
    xQueueReset(p_twai_obj->rx_queue);
    if (p_twai_obj->tx_queue != NULL) {
        xQueueReset(p_twai_obj->tx_queue);
    }
    p_twai_obj->rx_msg_count = 0;
    p_twai_obj->tx_msg_count = 0;
    twai_hal_start(&p_twai_obj->hal, p_twai_obj->mode);

    p_twai_obj->state = TWAI_STATE_RUNNING;
    portEXIT_CRITICAL(&handle->spinlock);
    return ESP_OK;
}

esp_err_t twai_start(void)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_start_v2(g_twai_objs[0]);
}

esp_err_t twai_stop_v2(twai_handle_t handle)
{
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);
    twai_obj_t *p_twai_obj = handle;

    portENTER_CRITICAL(&handle->spinlock);
    //Check state
    if (p_twai_obj->state != TWAI_STATE_RUNNING) {
        portEXIT_CRITICAL(&handle->spinlock);
        return ESP_ERR_INVALID_STATE;
    }

    twai_hal_stop(&p_twai_obj->hal);

    //Reset TX Queue and message count
    if (p_twai_obj->tx_queue != NULL) {
        xQueueReset(p_twai_obj->tx_queue);
    }
    p_twai_obj->tx_msg_count = 0;
    p_twai_obj->state = TWAI_STATE_STOPPED;

    portEXIT_CRITICAL(&handle->spinlock);
    return ESP_OK;
}

esp_err_t twai_stop(void)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_stop_v2(g_twai_objs[0]);
}

esp_err_t twai_transmit_v2(twai_handle_t handle, const twai_message_t *message, TickType_t ticks_to_wait)
{
    //Check arguments
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);
    TWAI_CHECK(message != NULL, ESP_ERR_INVALID_ARG);
    TWAI_CHECK((message->data_length_code <= TWAI_FRAME_MAX_DLC) || message->dlc_non_comp, ESP_ERR_INVALID_ARG);

    twai_obj_t *p_twai_obj = handle;
    portENTER_CRITICAL(&handle->spinlock);
    //Check State
    if (p_twai_obj->mode == TWAI_MODE_LISTEN_ONLY) {
        portEXIT_CRITICAL(&handle->spinlock);
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (p_twai_obj->state != TWAI_STATE_RUNNING) {
        portEXIT_CRITICAL(&handle->spinlock);
        return ESP_ERR_INVALID_STATE;
    }
    //Format frame
    esp_err_t ret = ESP_FAIL;
    twai_hal_frame_t tx_frame;
    twai_hal_format_frame(message, &tx_frame);

    //Check if frame can be sent immediately
    if (p_twai_obj->tx_msg_count == 0) {
        //No other frames waiting to transmit. Bypass queue and transmit immediately
        twai_hal_set_tx_buffer_and_transmit(&p_twai_obj->hal, &tx_frame);
        p_twai_obj->tx_msg_count++;
        ret = ESP_OK;
    }
    portEXIT_CRITICAL(&handle->spinlock);

    if (ret != ESP_OK) {
        if (p_twai_obj->tx_queue == NULL) {
            //TX Queue is disabled and TX buffer is occupied, message was not sent
            ret = ESP_FAIL;
        } else if (xQueueSend(p_twai_obj->tx_queue, &tx_frame, ticks_to_wait) == pdTRUE) {
            //Copied to TX Queue
            portENTER_CRITICAL(&handle->spinlock);
            if ((!twai_hal_check_state_flags(&p_twai_obj->hal, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED)) && uxQueueMessagesWaiting(p_twai_obj->tx_queue) > 0) {
                //If the TX buffer is free but the TX queue is not empty. Check if we need to manually start a transmission
                if (twai_hal_check_state_flags(&p_twai_obj->hal, TWAI_HAL_STATE_FLAG_BUS_OFF) || !twai_hal_check_state_flags(&p_twai_obj->hal, TWAI_HAL_STATE_FLAG_RUNNING)) {
                    //TX buffer became free due to bus-off or is no longer running. No need to start a transmission
                    ret = ESP_ERR_INVALID_STATE;
                } else {
                    //Manually start a transmission
                    int res = xQueueReceive(p_twai_obj->tx_queue, &tx_frame, 0);
                    assert(res == pdTRUE);
                    (void)res;
                    twai_hal_set_tx_buffer_and_transmit(&p_twai_obj->hal, &tx_frame);
                    p_twai_obj->tx_msg_count++;
                    ret = ESP_OK;
                }
            } else {
                //Frame was copied to queue, waiting to be transmitted
                p_twai_obj->tx_msg_count++;
                ret = ESP_OK;
            }
            portEXIT_CRITICAL(&handle->spinlock);
        } else {
            //Timed out waiting for free space on TX queue
            ret = ESP_ERR_TIMEOUT;
        }
    }
    return ret;
}

esp_err_t twai_transmit(const twai_message_t *message, TickType_t ticks_to_wait)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_transmit_v2(g_twai_objs[0], message, ticks_to_wait);
}

esp_err_t twai_receive_v2(twai_handle_t handle, twai_message_t *message, TickType_t ticks_to_wait)
{
    //Check arguments and state
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);
    TWAI_CHECK(message != NULL, ESP_ERR_INVALID_ARG);

    twai_obj_t *p_twai_obj = handle;
    //Get frame from RX Queue or RX Buffer
    twai_hal_frame_t rx_frame;
    if (xQueueReceive(p_twai_obj->rx_queue, &rx_frame, ticks_to_wait) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    portENTER_CRITICAL(&handle->spinlock);
    p_twai_obj->rx_msg_count--;
    portEXIT_CRITICAL(&handle->spinlock);

    //Decode frame
    twai_hal_parse_frame(&rx_frame, message);
    return ESP_OK;
}

esp_err_t twai_receive(twai_message_t *message, TickType_t ticks_to_wait)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_receive_v2(g_twai_objs[0], message, ticks_to_wait);
}

esp_err_t twai_read_alerts_v2(twai_handle_t handle, uint32_t *alerts, TickType_t ticks_to_wait)
{
    //Check arguments and state
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);
    TWAI_CHECK(alerts != NULL, ESP_ERR_INVALID_ARG);

    twai_obj_t *p_twai_obj = handle;

    //Wait for an alert to occur
    if (xSemaphoreTake(p_twai_obj->alert_semphr, ticks_to_wait) == pdTRUE) {
        portENTER_CRITICAL(&handle->spinlock);
        *alerts = p_twai_obj->alerts_triggered;
        p_twai_obj->alerts_triggered = 0;    //Clear triggered alerts
        portEXIT_CRITICAL(&handle->spinlock);
        return ESP_OK;
    } else {
        *alerts = 0;
        return ESP_ERR_TIMEOUT;
    }
}

esp_err_t twai_read_alerts(uint32_t *alerts, TickType_t ticks_to_wait)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_read_alerts_v2(g_twai_objs[0], alerts, ticks_to_wait);
}

esp_err_t twai_reconfigure_alerts_v2(twai_handle_t handle, uint32_t alerts_enabled, uint32_t *current_alerts)
{
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);

    twai_obj_t *p_twai_obj = handle;

    portENTER_CRITICAL(&handle->spinlock);
    //Clear any unhandled alerts
    if (current_alerts != NULL) {
        *current_alerts = p_twai_obj->alerts_triggered;
    }
    p_twai_obj->alerts_triggered = 0;
    p_twai_obj->alerts_enabled = alerts_enabled;         //Update enabled alerts
    portEXIT_CRITICAL(&handle->spinlock);

    return ESP_OK;
}

esp_err_t twai_reconfigure_alerts(uint32_t alerts_enabled, uint32_t *current_alerts)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_reconfigure_alerts_v2(g_twai_objs[0], alerts_enabled, current_alerts);
}

esp_err_t twai_initiate_recovery_v2(twai_handle_t handle)
{
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);
    twai_obj_t *p_twai_obj = handle;

    portENTER_CRITICAL(&handle->spinlock);
    //Check state
    if (p_twai_obj->state != TWAI_STATE_BUS_OFF) {
        portEXIT_CRITICAL(&handle->spinlock);
        return ESP_ERR_INVALID_STATE;
    }

    //Reset TX Queue/Counters
    if (p_twai_obj->tx_queue != NULL) {
        xQueueReset(p_twai_obj->tx_queue);
    }
    p_twai_obj->tx_msg_count = 0;

    //Trigger start of recovery process
    twai_hal_start_bus_recovery(&p_twai_obj->hal);
    p_twai_obj->state = TWAI_STATE_RECOVERING;
    portEXIT_CRITICAL(&handle->spinlock);

    return ESP_OK;
}

esp_err_t twai_initiate_recovery(void)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_initiate_recovery_v2(g_twai_objs[0]);
}

esp_err_t twai_get_status_info_v2(twai_handle_t handle, twai_status_info_t *status_info)
{
    //Check parameters
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);
    TWAI_CHECK(status_info != NULL, ESP_ERR_INVALID_ARG);
    twai_obj_t *p_twai_obj = handle;

    portENTER_CRITICAL(&handle->spinlock);
    if (p_twai_obj->mode == TWAI_MODE_LISTEN_ONLY) {
        //Error counters are frozen under listen only mode thus are meaningless. Simply return 0 in this case.
        status_info->tx_error_counter = 0;
        status_info->rx_error_counter = 0;
    } else {
        status_info->tx_error_counter = twai_hal_get_tec(&p_twai_obj->hal);
        status_info->rx_error_counter = twai_hal_get_rec(&p_twai_obj->hal);
    }
    status_info->msgs_to_tx = p_twai_obj->tx_msg_count;
    status_info->msgs_to_rx = p_twai_obj->rx_msg_count;
    status_info->tx_failed_count = p_twai_obj->tx_failed_count;
    status_info->rx_missed_count = p_twai_obj->rx_missed_count;
    status_info->rx_overrun_count = p_twai_obj->rx_overrun_count;
    status_info->arb_lost_count = p_twai_obj->arb_lost_count;
    status_info->bus_error_count = p_twai_obj->bus_error_count;
    status_info->state = p_twai_obj->state;
    portEXIT_CRITICAL(&handle->spinlock);

    return ESP_OK;
}

esp_err_t twai_get_status_info(twai_status_info_t *status_info)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_get_status_info_v2(g_twai_objs[0], status_info);
}

esp_err_t twai_clear_transmit_queue_v2(twai_handle_t handle)
{
    //Check parameters
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);
    TWAI_CHECK(handle->tx_queue != NULL, ESP_ERR_NOT_SUPPORTED);
    twai_obj_t *p_twai_obj = handle;

    portENTER_CRITICAL(&handle->spinlock);
    //If a message is currently undergoing transmission, the tx interrupt handler will decrement tx_msg_count
    p_twai_obj->tx_msg_count = twai_hal_check_state_flags(&p_twai_obj->hal, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED) ? 1 : 0;
    xQueueReset(p_twai_obj->tx_queue);
    portEXIT_CRITICAL(&handle->spinlock);

    return ESP_OK;
}

esp_err_t twai_clear_transmit_queue(void)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_clear_transmit_queue_v2(g_twai_objs[0]);
}

esp_err_t twai_clear_receive_queue_v2(twai_handle_t handle)
{
    //Check parameters
    TWAI_CHECK(handle != NULL, ESP_ERR_INVALID_ARG);
    twai_obj_t *p_twai_obj = handle;

    portENTER_CRITICAL(&handle->spinlock);
    p_twai_obj->rx_msg_count = 0;
    xQueueReset(p_twai_obj->rx_queue);
    portEXIT_CRITICAL(&handle->spinlock);

    return ESP_OK;
}

esp_err_t twai_clear_receive_queue(void)
{
    // the handle-less driver API only support one TWAI controller, i.e. the g_twai_objs[0]
    return twai_clear_receive_queue_v2(g_twai_objs[0]);
}

#if TWAI_USE_RETENTION_LINK
static esp_err_t s_twai_create_sleep_retention_link_cb(void *obj)
{
    twai_obj_t *host = (twai_obj_t *)obj;
    return sleep_retention_entries_create(twai_reg_retention_info[host->controller_id].entry_array,
                                          twai_reg_retention_info[host->controller_id].array_size,
                                          REGDMA_LINK_PRI_TWAI,
                                          twai_reg_retention_info[host->controller_id].module_id);
}
#endif
