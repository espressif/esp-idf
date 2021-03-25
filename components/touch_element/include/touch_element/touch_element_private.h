// Copyright 2016-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "touch_element/touch_element.h"
#include "touch_element/touch_button.h"
#include "touch_element/touch_slider.h"
#include "touch_element/touch_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TE_TAG "Touch Element"
#define TE_DEBUG_TAG "Touch Element Debug"
#define TE_UNUSED(arg)            (void)arg

#define TE_CHECK(cond, ret_val) ({                                            \
    if (!(cond)) {                                                            \
        ESP_LOGE(TE_TAG, "%s(%d)", __FUNCTION__, __LINE__);     \
        return (ret_val);                                                     \
    }                                                                         \
})

#define TE_CHECK_GOTO(cond, label) ({                                         \
    if (!(cond)) {                                                            \
        goto label;                                                           \
    }                                                                         \
})

#define TE_FREE_AND_NULL(ptr) ({                                              \
    free(ptr);                                                                \
    (ptr) = NULL;                                                             \
})

#define TE_DEFAULT_THRESHOLD_DIVIDER(obj)       ((obj)->global_config->threshold_divider)
#define TE_DEFAULT_LONGPRESS_TIME(obj)          ((obj)->global_config->default_lp_time)

typedef enum {
    TE_STATE_IDLE = 0,
    TE_STATE_PRESS,
    TE_STATE_RELEASE,
} te_state_t;

typedef te_state_t te_dev_state_t;
typedef touch_elem_type_t te_dev_type_t;

typedef struct {
    float sens;                   //!< Touch channel sensitivity
    touch_pad_t channel;          //!< Touch channel number(index)
    te_dev_type_t type;           //!< Touch channel type  TODO: need to refactor as te_class_type_t
    te_dev_state_t state;         //!< Touch channel current state
} te_dev_t;

typedef enum {
    TE_CLS_TYPE_BUTTON = 0,
    TE_CLS_TYPE_SLIDER,
    TE_CLS_TYPE_MATRIX,
    TE_CLS_TYPE_MAX  //TODO: add waterproof class
} te_class_type_t;

typedef struct  {
    touch_elem_handle_t handle;
    bool (*check_channel) (touch_pad_t);
    esp_err_t (*set_threshold) (void);
    void (*process_state) (void);
    void (*update_state) (touch_pad_t, te_state_t);
} te_object_methods_t;

/* -------------------------------------------- Waterproof basic type --------------------------------------------- */
struct te_waterproof_s {
    te_dev_t *guard_device;                     //Waterproof guard channel device
    touch_elem_handle_t *mask_handle;           //Waterproof masked handle array
    touch_pad_t shield_channel;                 //Waterproof shield channel
    bool is_shield_level_set;                   //Waterproof shield level setting bit
};
typedef struct te_waterproof_s* te_waterproof_handle_t;
/* -------------------------------------------- Button basic type --------------------------------------------- */
typedef struct {
    touch_elem_dispatch_t dispatch_method;      //Button dispatch method
    touch_button_callback_t callback;           //Button callback routine
    uint32_t event_mask;                        //Button subscribed event mask
    void *arg;                                  //User input argument
} te_button_handle_config_t;

typedef te_state_t te_button_state_t;           //TODO: add Long Press state

struct te_button_s {
    te_button_handle_config_t *config;          //Button configuration
    te_dev_t *device;                           //Base device information
    te_button_state_t current_state;            //Button current state
    te_button_state_t last_state;               //Button last state
    touch_button_event_t event;                 //Button outside state(for application layer)
    uint32_t trigger_cnt;                       //Button long time trigger counter
    uint32_t trigger_thr;                       //Button long time trigger counter threshold
};

typedef struct te_button_s* te_button_handle_t;
/* -------------------------------------------- Slider basic type --------------------------------------------- */
typedef struct {
    touch_elem_dispatch_t dispatch_method;      //Slider dispatch method
    touch_slider_callback_t callback;           //Slider callback routine
    uint32_t event_mask;                        //Slider subscribed event mask
    void *arg;                                  //User input argument
} te_slider_handle_config_t;

typedef te_state_t te_slider_state_t;

struct te_slider_s {
    te_slider_handle_config_t *config;          //Slider configuration
    te_dev_t **device;                          //Base device information set
    te_slider_state_t current_state;            //Slider current state
    te_slider_state_t last_state;               //Slider last state
    touch_slider_event_t event;                 //Slider outside state(for application layer)
    float position_scale;                       //Slider position scale(step size)
    float *quantify_signal_array;               //Slider re-quantization array
    uint32_t *channel_bcm;                      //Channel benchmark array
    uint32_t channel_bcm_update_cnt;            //Channel benchmark update counter
    uint32_t filter_reset_cnt;                  //Slider reset counter
    uint32_t last_position;                     //Slider last position
    touch_slider_position_t position;           //Slider position
    uint8_t position_range;                     //Slider position range([0, position_range])
    uint8_t channel_sum;                        //Slider channel sum
    uint8_t *pos_filter_window;                 //Slider position moving average filter window
    uint8_t pos_window_idx;                     //Slider position moving average filter window index
    bool is_first_sample;                       //Slider first time sample record bit
};

typedef struct te_slider_s* te_slider_handle_t;
/* -------------------------------------------- Matrix basic type --------------------------------------------- */
typedef struct {
    touch_elem_dispatch_t dispatch_method;      //Matrix button dispatch method
    touch_matrix_callback_t callback;           //Matrix button callback routine
    uint32_t event_mask;                        //Matrix button subscribed event mask
    void *arg;                                  //User input argument
} te_matrix_handle_config_t;

typedef te_state_t te_matrix_state_t;           //TODO: add Long Press state

struct te_matrix_s {
    te_matrix_handle_config_t *config;          //Matrix button configuration
    te_dev_t **device;                          //Base device information
    te_matrix_state_t current_state;            //Matrix button current state
    te_matrix_state_t last_state;               //Matrix button current state
    touch_matrix_event_t event;                 //Matrix button outside state(for application layer)
    uint32_t trigger_cnt;                       //Matrix button long time trigger counter
    uint32_t trigger_thr;                       //Matrix button long time trigger counter threshold
    touch_matrix_position_t position;           //Matrix button position
    uint8_t x_channel_num;                      //The number of touch sensor channel in x axis
    uint8_t y_channel_num;                      //The number of touch sensor channel in y axis
};

typedef struct te_matrix_s* te_matrix_handle_t;
/* ------------------------------------------------------------------------------------------------------------------ */

/* --------------------------------------------- Global system methods ---------------------------------------------- */
uint32_t te_read_smooth_signal(touch_pad_t channel_num);
bool te_system_check_state(void);
//TODO: Refactor this function with function overload
esp_err_t te_dev_init(te_dev_t **device, uint8_t device_num, te_dev_type_t type, const touch_pad_t *channel, const float *sens, float divider);
void te_dev_deinit(te_dev_t **device, uint8_t device_num);
esp_err_t te_dev_set_threshold(te_dev_t *device);
esp_err_t te_event_give(touch_elem_message_t te_message);
uint8_t te_get_timer_period(void);
void te_object_method_register(te_object_methods_t *object_methods, te_class_type_t object_type);
void te_object_method_unregister(te_class_type_t object_type);
bool te_object_check_channel(const touch_pad_t *channel_array, uint8_t channel_sum);
bool waterproof_check_mask_handle(touch_elem_handle_t te_handle);
/* ------------------------------------------------------------------------------------------------------------------ */

#ifdef __cplusplus
}
#endif
