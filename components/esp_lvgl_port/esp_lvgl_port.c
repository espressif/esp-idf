/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lvgl_port.h"

#include "lvgl.h"


#if __has_include ("esp_lcd_touch.h")
#include "esp_lcd_touch.h"
#endif

#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 4, 4)) || (ESP_IDF_VERSION == ESP_IDF_VERSION_VAL(5, 0, 0))
#define LVGL_PORT_HANDLE_FLUSH_READY 0
#else
#define LVGL_PORT_HANDLE_FLUSH_READY 1
#endif

static const char *TAG = "LVGL";

/*******************************************************************************
* Types definitions
*******************************************************************************/

typedef struct lvgl_port_ctx_s {
    SemaphoreHandle_t   lvgl_mux;
    esp_timer_handle_t  tick_timer;
    bool                running;
    int                 task_max_sleep_ms;
} lvgl_port_ctx_t;

typedef struct {
    esp_lcd_panel_io_handle_t io_handle;    /* LCD panel IO handle */
    esp_lcd_panel_handle_t    panel_handle; /* LCD panel handle */
    lvgl_port_rotation_cfg_t  rotation;     /* Default values of the screen rotation */
    lv_disp_drv_t             disp_drv;     /* LVGL display driver */
} lvgl_port_display_ctx_t;

#if __has_include ("esp_lcd_touch.h")
typedef struct {
    esp_lcd_touch_handle_t   handle;     /* LCD touch IO handle */
    lv_indev_drv_t           indev_drv;  /* LVGL input device driver */
} lvgl_port_touch_ctx_t;
#endif

#if __has_include ("iot_button.h")

typedef enum {
    LVGL_PORT_NAV_BTN_PREV,
    LVGL_PORT_NAV_BTN_NEXT,
    LVGL_PORT_NAV_BTN_ENTER,
    LVGL_PORT_NAV_BTN_CNT,
} lvgl_port_nav_btns_t;

typedef struct {
    button_handle_t btn[LVGL_PORT_NAV_BTN_CNT];     /* Button handlers */
    lv_indev_drv_t  indev_drv;  /* LVGL input device driver */
    bool btn_prev; /* Button prev state */
    bool btn_next; /* Button next state */
    bool btn_enter; /* Button enter state */
} lvgl_port_nav_btns_ctx_t;
#endif

/*******************************************************************************
* Local variables
*******************************************************************************/
static lvgl_port_ctx_t lvgl_port_ctx;
static int lvgl_port_timer_period_ms = 5;

/*******************************************************************************
* Function definitions
*******************************************************************************/
static void lvgl_port_task(void *arg);
static esp_err_t lvgl_port_tick_init(void);
static void lvgl_port_task_deinit(void);

// LVGL callbacks
#if LVGL_PORT_HANDLE_FLUSH_READY
static bool lvgl_port_flush_ready_callback(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx);
#endif
static void lvgl_port_flush_callback(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void lvgl_port_update_callback(lv_disp_drv_t *drv);
#if __has_include ("esp_lcd_touch.h")
static void lvgl_port_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
#endif
#if __has_include ("iot_button.h")
static void lvgl_port_navigation_buttons_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static void lvgl_port_btn_down_handler(void *arg, void *arg2);
static void lvgl_port_btn_up_handler(void *arg, void *arg2);
#endif
static void lvgl_port_pix_monochrome_callback(lv_disp_drv_t *drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);
/*******************************************************************************
* Public API functions
*******************************************************************************/

esp_err_t lvgl_port_init(const lvgl_port_cfg_t *cfg)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(cfg->task_affinity < (configNUM_CORES), ESP_ERR_INVALID_ARG, err, TAG, "Bad core number for task! Maximum core number is %d", (configNUM_CORES - 1));

    memset(&lvgl_port_ctx, 0, sizeof(lvgl_port_ctx));


    /* LVGL init */
    lv_init();
    /* Tick init */
    lvgl_port_timer_period_ms = cfg->timer_period_ms;
    ESP_RETURN_ON_ERROR(lvgl_port_tick_init(), TAG, "");
    /* Create task */
    lvgl_port_ctx.task_max_sleep_ms = cfg->task_max_sleep_ms;
    if (lvgl_port_ctx.task_max_sleep_ms == 0) {
        lvgl_port_ctx.task_max_sleep_ms = 500;
    }
    lvgl_port_ctx.lvgl_mux = xSemaphoreCreateRecursiveMutex();
    ESP_GOTO_ON_FALSE(lvgl_port_ctx.lvgl_mux, ESP_ERR_NO_MEM, err, TAG, "Create LVGL mutex fail!");

    BaseType_t res;
    if (cfg->task_affinity < 0) {
        res = xTaskCreate(lvgl_port_task, "LVGL task", cfg->task_stack, NULL, cfg->task_priority, NULL);
    } else {
        res = xTaskCreatePinnedToCore(lvgl_port_task, "LVGL task", cfg->task_stack, NULL, cfg->task_priority, NULL, cfg->task_affinity);
    }
    ESP_GOTO_ON_FALSE(res == pdPASS, ESP_FAIL, err, TAG, "Create LVGL task fail!");

err:
    if (ret != ESP_OK) {
        lvgl_port_deinit();
    }

    return ret;
}

esp_err_t lvgl_port_deinit(void)
{
    /* Stop and delete timer */
    if (lvgl_port_ctx.tick_timer != NULL) {
        esp_timer_stop(lvgl_port_ctx.tick_timer);
        esp_timer_delete(lvgl_port_ctx.tick_timer);
        lvgl_port_ctx.tick_timer = NULL;
    }

    /* Stop running task */
    if (!lvgl_port_ctx.running) {
        lvgl_port_ctx.running = false;
    } else {
        lvgl_port_task_deinit();
    }

    return ESP_OK;
}

lv_disp_t *lvgl_port_add_disp(const lvgl_port_display_cfg_t *disp_cfg)
{
    esp_err_t ret = ESP_OK;
    lv_disp_t *disp = NULL;
    lv_color_t *buf1 = NULL;
    lv_color_t *buf2 = NULL;
    assert(disp_cfg != NULL);
    assert(disp_cfg->io_handle != NULL);
    assert(disp_cfg->panel_handle != NULL);
    assert(disp_cfg->buffer_size > 0);
    assert(disp_cfg->hres > 0);
    assert(disp_cfg->vres > 0);

    /* Display context */
    lvgl_port_display_ctx_t *disp_ctx = malloc(sizeof(lvgl_port_display_ctx_t));
    ESP_GOTO_ON_FALSE(disp_ctx, ESP_ERR_NO_MEM, err, TAG, "Not enough memory for display context allocation!");
    disp_ctx->io_handle = disp_cfg->io_handle;
    disp_ctx->panel_handle = disp_cfg->panel_handle;
    disp_ctx->rotation.swap_xy = disp_cfg->rotation.swap_xy;
    disp_ctx->rotation.mirror_x = disp_cfg->rotation.mirror_x;
    disp_ctx->rotation.mirror_y = disp_cfg->rotation.mirror_y;

    uint32_t buff_caps = MALLOC_CAP_DEFAULT;
    if (disp_cfg->flags.buff_dma && disp_cfg->flags.buff_spiram) {
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG, "Alloc DMA capable buffer in SPIRAM is not supported!");
    } else if (disp_cfg->flags.buff_dma) {
        buff_caps = MALLOC_CAP_DMA;
    } else if (disp_cfg->flags.buff_spiram) {
        buff_caps = MALLOC_CAP_SPIRAM;
    }

    /* alloc draw buffers used by LVGL */
    /* it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized */
    buf1 = heap_caps_malloc(disp_cfg->buffer_size * sizeof(lv_color_t), buff_caps);
    ESP_GOTO_ON_FALSE(buf1, ESP_ERR_NO_MEM, err, TAG, "Not enough memory for LVGL buffer (buf1) allocation!");
    if (disp_cfg->double_buffer) {
        buf2 = heap_caps_malloc(disp_cfg->buffer_size * sizeof(lv_color_t), buff_caps);
        ESP_GOTO_ON_FALSE(buf2, ESP_ERR_NO_MEM, err, TAG, "Not enough memory for LVGL buffer (buf2) allocation!");
    }
    lv_disp_draw_buf_t *disp_buf = malloc(sizeof(lv_disp_draw_buf_t));
    ESP_GOTO_ON_FALSE(disp_buf, ESP_ERR_NO_MEM, err, TAG, "Not enough memory for LVGL display buffer allocation!");

    /* initialize LVGL draw buffers */
    lv_disp_draw_buf_init(disp_buf, buf1, buf2, disp_cfg->buffer_size);

    ESP_LOGD(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_ctx->disp_drv);
    disp_ctx->disp_drv.hor_res = disp_cfg->hres;
    disp_ctx->disp_drv.ver_res = disp_cfg->vres;
    disp_ctx->disp_drv.flush_cb = lvgl_port_flush_callback;
    disp_ctx->disp_drv.drv_update_cb = lvgl_port_update_callback;
    disp_ctx->disp_drv.draw_buf = disp_buf;
    disp_ctx->disp_drv.user_data = disp_ctx;

#if LVGL_PORT_HANDLE_FLUSH_READY
    /* Register done callback */
    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = lvgl_port_flush_ready_callback,
    };
    esp_lcd_panel_io_register_event_callbacks(disp_ctx->io_handle, &cbs, &disp_ctx->disp_drv);
#endif

    /* Monochrome display settings */
    if (disp_cfg->monochrome) {
        /* When using monochromatic display, there must be used full bufer! */
        ESP_GOTO_ON_FALSE((disp_cfg->hres * disp_cfg->vres == disp_cfg->buffer_size), ESP_ERR_INVALID_ARG, err, TAG, "Monochromatic display must using full buffer!");

        disp_ctx->disp_drv.full_refresh = 1;
        disp_ctx->disp_drv.set_px_cb = lvgl_port_pix_monochrome_callback;
    }

    disp = lv_disp_drv_register(&disp_ctx->disp_drv);

err:
    if (ret != ESP_OK) {
        if (buf1) {
            free(buf1);
        }
        if (buf2) {
            free(buf2);
        }
        if (disp_ctx) {
            free(disp_ctx);
        }
    }

    return disp;
}

esp_err_t lvgl_port_remove_disp(lv_disp_t *disp)
{
    assert(disp);
    lv_disp_drv_t *disp_drv = disp->driver;
    assert(disp_drv);
    lvgl_port_display_ctx_t *disp_ctx = (lvgl_port_display_ctx_t *)disp_drv->user_data;

    lv_disp_remove(disp);

    if (disp_drv) {
        if (disp_drv->draw_buf && disp_drv->draw_buf->buf1) {
            free(disp_drv->draw_buf->buf1);
            disp_drv->draw_buf->buf1 = NULL;
        }
        if (disp_drv->draw_buf && disp_drv->draw_buf->buf2) {
            free(disp_drv->draw_buf->buf2);
            disp_drv->draw_buf->buf2 = NULL;
        }
        if (disp_drv->draw_buf) {
            free(disp_drv->draw_buf);
            disp_drv->draw_buf = NULL;
        }
    }

    free(disp_ctx);

    return ESP_OK;
}

#if __has_include ("esp_lcd_touch.h")
lv_indev_t *lvgl_port_add_touch(const lvgl_port_touch_cfg_t *touch_cfg)
{
    assert(touch_cfg != NULL);
    assert(touch_cfg->disp != NULL);
    assert(touch_cfg->handle != NULL);

    /* Touch context */
    lvgl_port_touch_ctx_t *touch_ctx = malloc(sizeof(lvgl_port_touch_ctx_t));
    if (touch_ctx == NULL) {
        ESP_LOGE(TAG, "Not enough memory for touch context allocation!");
        return NULL;
    }
    touch_ctx->handle = touch_cfg->handle;

    /* Register a touchpad input device */
    lv_indev_drv_init(&touch_ctx->indev_drv);
    touch_ctx->indev_drv.type = LV_INDEV_TYPE_POINTER;
    touch_ctx->indev_drv.disp = touch_cfg->disp;
    touch_ctx->indev_drv.read_cb = lvgl_port_touchpad_read;
    touch_ctx->indev_drv.user_data = touch_ctx;
    return lv_indev_drv_register(&touch_ctx->indev_drv);
}

esp_err_t lvgl_port_remove_touch(lv_indev_t *touch)
{
    assert(touch);
    lv_indev_drv_t *indev_drv = touch->driver;
    assert(indev_drv);
    lvgl_port_touch_ctx_t *touch_ctx = (lvgl_port_touch_ctx_t *)indev_drv->user_data;

    if (touch_ctx) {
        free(touch_ctx);
    }

    return ESP_OK;
}
#endif

#if __has_include ("iot_button.h")
lv_indev_t *lvgl_port_add_navigation_buttons(const lvgl_port_nav_btns_cfg_t *buttons_cfg)
{
    esp_err_t ret = ESP_OK;
    lv_indev_t *indev = NULL;
    assert(buttons_cfg != NULL);
    assert(buttons_cfg->disp != NULL);

    /* Touch context */
    lvgl_port_nav_btns_ctx_t *buttons_ctx = malloc(sizeof(lvgl_port_nav_btns_ctx_t));
    if (buttons_ctx == NULL) {
        ESP_LOGE(TAG, "Not enough memory for buttons context allocation!");
        return NULL;
    }

    /* Previous button */
    if (buttons_cfg->button_prev != NULL) {
        buttons_ctx->btn[LVGL_PORT_NAV_BTN_PREV] = iot_button_create(buttons_cfg->button_prev);
        ESP_GOTO_ON_FALSE(buttons_ctx->btn[LVGL_PORT_NAV_BTN_PREV], ESP_ERR_NO_MEM, err, TAG, "Not enough memory for button create!");
    }

    /* Next button */
    if (buttons_cfg->button_next != NULL) {
        buttons_ctx->btn[LVGL_PORT_NAV_BTN_NEXT] = iot_button_create(buttons_cfg->button_next);
        ESP_GOTO_ON_FALSE(buttons_ctx->btn[LVGL_PORT_NAV_BTN_NEXT], ESP_ERR_NO_MEM, err, TAG, "Not enough memory for button create!");
    }

    /* Enter button */
    if (buttons_cfg->button_enter != NULL) {
        buttons_ctx->btn[LVGL_PORT_NAV_BTN_ENTER] = iot_button_create(buttons_cfg->button_enter);
        ESP_GOTO_ON_FALSE(buttons_ctx->btn[LVGL_PORT_NAV_BTN_ENTER], ESP_ERR_NO_MEM, err, TAG, "Not enough memory for button create!");
    }

    /* Button handlers */
    for (int i = 0; i < LVGL_PORT_NAV_BTN_CNT; i++) {
        ESP_ERROR_CHECK(iot_button_register_cb(buttons_ctx->btn[i], BUTTON_PRESS_DOWN, lvgl_port_btn_down_handler, buttons_ctx));
        ESP_ERROR_CHECK(iot_button_register_cb(buttons_ctx->btn[i], BUTTON_PRESS_UP, lvgl_port_btn_up_handler, buttons_ctx));
    }

    buttons_ctx->btn_prev = false;
    buttons_ctx->btn_next = false;
    buttons_ctx->btn_enter = false;

    /* Register a touchpad input device */
    lv_indev_drv_init(&buttons_ctx->indev_drv);
    buttons_ctx->indev_drv.type = LV_INDEV_TYPE_ENCODER;
    buttons_ctx->indev_drv.disp = buttons_cfg->disp;
    buttons_ctx->indev_drv.read_cb = lvgl_port_navigation_buttons_read;
    buttons_ctx->indev_drv.user_data = buttons_ctx;
    buttons_ctx->indev_drv.long_press_repeat_time = 300;
    indev = lv_indev_drv_register(&buttons_ctx->indev_drv);

err:
    if (ret != ESP_OK) {
        for (int i = 0; i < LVGL_PORT_NAV_BTN_CNT; i++) {
            if (buttons_ctx->btn[i] != NULL) {
                iot_button_delete(buttons_ctx->btn[i]);
            }
        }

        if (buttons_ctx != NULL) {
            free(buttons_ctx);
        }
    }

    return indev;
}

esp_err_t lvgl_port_remove_navigation_buttons(lv_indev_t *buttons)
{
    assert(buttons);
    lv_indev_drv_t *indev_drv = buttons->driver;
    assert(indev_drv);
    lvgl_port_nav_btns_ctx_t *buttons_ctx = (lvgl_port_nav_btns_ctx_t *)indev_drv->user_data;

    if (buttons_ctx) {
        free(buttons_ctx);
    }

    return ESP_OK;
}
#endif

bool lvgl_port_lock(uint32_t timeout_ms)
{
    assert(lvgl_port_ctx.lvgl_mux && "lvgl_port_init must be called first");

    const TickType_t timeout_ticks = (timeout_ms == 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTakeRecursive(lvgl_port_ctx.lvgl_mux, timeout_ticks) == pdTRUE;
}

void lvgl_port_unlock(void)
{
    assert(lvgl_port_ctx.lvgl_mux && "lvgl_port_init must be called first");
    xSemaphoreGiveRecursive(lvgl_port_ctx.lvgl_mux);
}

void lvgl_port_flush_ready(lv_disp_t *disp)
{
    assert(disp);
    assert(disp->driver);
    lv_disp_flush_ready(disp->driver);
}



/*******************************************************************************
* Private functions
*******************************************************************************/

static void lvgl_port_task(void *arg)
{
    uint32_t task_delay_ms = lvgl_port_ctx.task_max_sleep_ms;

    ESP_LOGI(TAG, "Starting LVGL task");
    lvgl_port_ctx.running = true;
    while (lvgl_port_ctx.running) {
        if (lvgl_port_lock(0)) {
            task_delay_ms = lv_timer_handler();
            lvgl_port_unlock();
        }
        if (task_delay_ms > lvgl_port_ctx.task_max_sleep_ms) {
            task_delay_ms = lvgl_port_ctx.task_max_sleep_ms;
        } else if (task_delay_ms < 1) {
            task_delay_ms = 1;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }

    lvgl_port_task_deinit();

    /* Close task */
    vTaskDelete( NULL );
}

static void lvgl_port_task_deinit(void)
{
    if (lvgl_port_ctx.lvgl_mux) {
        vSemaphoreDelete(lvgl_port_ctx.lvgl_mux);
    }
    memset(&lvgl_port_ctx, 0, sizeof(lvgl_port_ctx));
#if LV_ENABLE_GC || !LV_MEM_CUSTOM
    /* Deinitialize LVGL */
    lv_deinit();
#endif
}

#if LVGL_PORT_HANDLE_FLUSH_READY
static bool lvgl_port_flush_ready_callback(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    lv_disp_drv_t *disp_drv = (lv_disp_drv_t *)user_ctx;
    assert(disp_drv != NULL);
    lv_disp_flush_ready(disp_drv);
    return false;
}
#endif

static void lvgl_port_flush_callback(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    assert(drv != NULL);
    lvgl_port_display_ctx_t *disp_ctx = (lvgl_port_display_ctx_t *)drv->user_data;
    assert(disp_ctx != NULL);

    const int offsetx1 = area->x1;
    const int offsetx2 = area->x2;
    const int offsety1 = area->y1;
    const int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(disp_ctx->panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

static void lvgl_port_update_callback(lv_disp_drv_t *drv)
{
    assert(drv);
    lvgl_port_display_ctx_t *disp_ctx = (lvgl_port_display_ctx_t *)drv->user_data;
    assert(disp_ctx != NULL);
    esp_lcd_panel_handle_t panel_handle = disp_ctx->panel_handle;

    /* Solve rotation screen and touch */
    switch (drv->rotated) {
    case LV_DISP_ROT_NONE:
        /* Rotate LCD display */
        esp_lcd_panel_swap_xy(panel_handle, disp_ctx->rotation.swap_xy);
        esp_lcd_panel_mirror(panel_handle, disp_ctx->rotation.mirror_x, disp_ctx->rotation.mirror_y);
        break;
    case LV_DISP_ROT_90:
        /* Rotate LCD display */
        esp_lcd_panel_swap_xy(panel_handle, !disp_ctx->rotation.swap_xy);
        if (disp_ctx->rotation.swap_xy) {
            esp_lcd_panel_mirror(panel_handle, !disp_ctx->rotation.mirror_x, disp_ctx->rotation.mirror_y);
        } else {
            esp_lcd_panel_mirror(panel_handle, disp_ctx->rotation.mirror_x, !disp_ctx->rotation.mirror_y);
        }
        break;
    case LV_DISP_ROT_180:
        /* Rotate LCD display */
        esp_lcd_panel_swap_xy(panel_handle, disp_ctx->rotation.swap_xy);
        esp_lcd_panel_mirror(panel_handle, !disp_ctx->rotation.mirror_x, !disp_ctx->rotation.mirror_y);
        break;
    case LV_DISP_ROT_270:
        /* Rotate LCD display */
        esp_lcd_panel_swap_xy(panel_handle, !disp_ctx->rotation.swap_xy);
        if (disp_ctx->rotation.swap_xy) {
            esp_lcd_panel_mirror(panel_handle, disp_ctx->rotation.mirror_x, !disp_ctx->rotation.mirror_y);
        } else {
            esp_lcd_panel_mirror(panel_handle, !disp_ctx->rotation.mirror_x, disp_ctx->rotation.mirror_y);
        }
        break;
    }
}

static void lvgl_port_pix_monochrome_callback(lv_disp_drv_t *drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa)
{
    if (drv->rotated == LV_DISP_ROT_90 || drv->rotated == LV_DISP_ROT_270) {
        lv_coord_t tmp_x = x;
        x = y;
        y = tmp_x;
    }

    /* Write to the buffer as required for the display.
    * It writes only 1-bit for monochrome displays mapped vertically.*/
    buf += drv->hor_res * (y >> 3) + x;
    if (lv_color_to1(color)) {
        (*buf) &= ~(1 << (y % 8));
    } else {
        (*buf) |= (1 << (y % 8));
    }
}

#if __has_include ("esp_lcd_touch.h")
static void lvgl_port_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    assert(indev_drv);
    lvgl_port_touch_ctx_t *touch_ctx = (lvgl_port_touch_ctx_t *)indev_drv->user_data;
    assert(touch_ctx->handle);

    uint16_t touchpad_x[1] = {0};
    uint16_t touchpad_y[1] = {0};
    uint8_t touchpad_cnt = 0;

    /* Read data from touch controller into memory */
    esp_lcd_touch_read_data(touch_ctx->handle);

    /* Read data from touch controller */
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(touch_ctx->handle, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);

    if (touchpad_pressed && touchpad_cnt > 0) {
        data->point.x = touchpad_x[0];
        data->point.y = touchpad_y[0];
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
#endif

#if __has_include ("iot_button.h")
static uint32_t last_key = 0;
static void lvgl_port_navigation_buttons_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    assert(indev_drv);
    lvgl_port_nav_btns_ctx_t *ctx = (lvgl_port_nav_btns_ctx_t *)indev_drv->user_data;
    assert(ctx);

    /* Buttons */
    if (ctx->btn_prev) {
        data->key = LV_KEY_LEFT;
        last_key = LV_KEY_LEFT;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (ctx->btn_next) {
        data->key = LV_KEY_RIGHT;
        last_key = LV_KEY_RIGHT;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (ctx->btn_enter) {
        data->key = LV_KEY_ENTER;
        last_key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->key = last_key;
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void lvgl_port_btn_down_handler(void *arg, void *arg2)
{
    lvgl_port_nav_btns_ctx_t *ctx = (lvgl_port_nav_btns_ctx_t *) arg2;
    button_handle_t button = (button_handle_t)arg;
    if (ctx && button) {
        /* PREV */
        if (button == ctx->btn[LVGL_PORT_NAV_BTN_PREV]) {
            ctx->btn_prev = true;
        }
        /* NEXT */
        if (button == ctx->btn[LVGL_PORT_NAV_BTN_NEXT]) {
            ctx->btn_next = true;
        }
        /* ENTER */
        if (button == ctx->btn[LVGL_PORT_NAV_BTN_ENTER]) {
            ctx->btn_enter = true;
        }
    }
}

static void lvgl_port_btn_up_handler(void *arg, void *arg2)
{
    lvgl_port_nav_btns_ctx_t *ctx = (lvgl_port_nav_btns_ctx_t *) arg2;
    button_handle_t button = (button_handle_t)arg;
    if (ctx && button) {
        /* PREV */
        if (button == ctx->btn[LVGL_PORT_NAV_BTN_PREV]) {
            ctx->btn_prev = false;
        }
        /* NEXT */
        if (button == ctx->btn[LVGL_PORT_NAV_BTN_NEXT]) {
            ctx->btn_next = false;
        }
        /* ENTER */
        if (button == ctx->btn[LVGL_PORT_NAV_BTN_ENTER]) {
            ctx->btn_enter = false;
        }
    }
}
#endif

static void lvgl_port_tick_increment(void *arg)
{
    /* Tell LVGL how many milliseconds have elapsed */
    lv_tick_inc(lvgl_port_timer_period_ms);
}

static esp_err_t lvgl_port_tick_init(void)
{
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &lvgl_port_tick_increment,
        .name = "LVGL tick",
    };
    ESP_RETURN_ON_ERROR(esp_timer_create(&lvgl_tick_timer_args, &lvgl_port_ctx.tick_timer), TAG, "Creating LVGL timer filed!");
    return esp_timer_start_periodic(lvgl_port_ctx.tick_timer, lvgl_port_timer_period_ms * 1000);
}
