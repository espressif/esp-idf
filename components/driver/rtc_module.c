// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <esp_types.h>
#include <stdlib.h>
#include <ctype.h>
#include "rom/ets_sys.h"
#include "esp_log.h"
#include "soc/rtc_io_reg.h"
#include "soc/rtc_io_struct.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/syscon_reg.h"
#include "soc/syscon_struct.h"
#include "rtc_io.h"
#include "touch_pad.h"
#include "adc.h"
#include "dac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_intr_alloc.h"
#include "sys/lock.h"
#include "driver/rtc_cntl.h"
#include "driver/gpio.h"

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "rom/queue.h"


#define ADC_FSM_RSTB_WAIT_DEFAULT     (8)
#define ADC_FSM_START_WAIT_DEFAULT    (5)
#define ADC_FSM_STANDBY_WAIT_DEFAULT  (100)
#define ADC_FSM_TIME_KEEP             (-1)
#define ADC_MAX_MEAS_NUM_DEFAULT      (255)
#define ADC_MEAS_NUM_LIM_DEFAULT      (1)
#define SAR_ADC_CLK_DIV_DEFUALT       (2)
#define ADC_PATT_LEN_MAX              (16)
#define TOUCH_PAD_FILTER_FACTOR_DEFAULT   (16)
#define TOUCH_PAD_SHIFT_DEFAULT           (4)
#define DAC_ERR_STR_CHANNEL_ERROR   "DAC channel error"

static const char *RTC_MODULE_TAG = "RTC_MODULE";

#define RTC_MODULE_CHECK(a, str, ret_val) if (!(a)) {                                \
    ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str); \
    return (ret_val);                                                              \
}

#define RTC_RES_CHECK(res, ret_val) if ( (a) != ESP_OK) {                           \
        ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s)", __FILE__, __LINE__, __FUNCTION__); \
        return (ret_val);                                                              \
}

#define ADC_CHECK_UNIT(unit) RTC_MODULE_CHECK(adc_unit < ADC_UNIT_2, "ADC unit error, only support ADC1 for now", ESP_ERR_INVALID_ARG)

#define ADC1_CHECK_FUNCTION_RET(fun_ret) if(fun_ret!=ESP_OK){\
    ESP_LOGE(RTC_MODULE_TAG,"%s:%d\n",__FUNCTION__,__LINE__);\
    return ESP_FAIL;\
}

#define ADC2_CHECK_FUNCTION_RET(fun_ret) do { if(fun_ret!=ESP_OK){\
    ESP_LOGE(RTC_MODULE_TAG,"%s:%d\n",__FUNCTION__,__LINE__);\
    return ESP_FAIL;\
} }while (0)

portMUX_TYPE rtc_spinlock = portMUX_INITIALIZER_UNLOCKED;
static SemaphoreHandle_t rtc_touch_mux = NULL;
/*
In ADC2, there're two locks used for different cases:
1. lock shared with app and WIFI: 
   when wifi using the ADC2, we assume it will never stop, 
   so app checks the lock and returns immediately if failed.

2. lock shared between tasks: 
   when several tasks sharing the ADC2, we want to guarantee 
   all the requests will be handled.
   Since conversions are short (about 31us), app returns the lock very soon, 
   we use a spinlock to stand there waiting to do conversions one by one.

adc2_spinlock should be acquired first, then adc2_wifi_lock or rtc_spinlock.
*/
//prevent ADC2 being used by wifi and other tasks at the same time.
static _lock_t adc2_wifi_lock = NULL;
//prevent ADC2 being used by tasks (regardless of WIFI)
portMUX_TYPE adc2_spinlock = portMUX_INITIALIZER_UNLOCKED;

typedef struct {
    TimerHandle_t timer;
    uint32_t filtered_val[TOUCH_PAD_MAX];
    uint32_t filter_period;
    uint32_t period;
    bool enable;
} touch_pad_filter_t;
static touch_pad_filter_t *s_touch_pad_filter = NULL;

typedef enum {
    ADC_FORCE_FSM = 0x0,
    ADC_FORCE_DISABLE = 0x2,
    ADC_FORCE_ENABLE = 0x3,
} adc_force_mode_t;

//Reg,Mux,Fun,IE,Up,Down,Rtc_number
const rtc_gpio_desc_t rtc_gpio_desc[GPIO_PIN_COUNT] = {
    {RTC_IO_TOUCH_PAD1_REG, RTC_IO_TOUCH_PAD1_MUX_SEL_M, RTC_IO_TOUCH_PAD1_FUN_SEL_S, RTC_IO_TOUCH_PAD1_FUN_IE_M, RTC_IO_TOUCH_PAD1_RUE_M, RTC_IO_TOUCH_PAD1_RDE_M, RTC_IO_TOUCH_PAD1_SLP_SEL_M, RTC_IO_TOUCH_PAD1_SLP_IE_M, RTC_IO_TOUCH_PAD1_HOLD_M, RTC_CNTL_TOUCH_PAD1_HOLD_FORCE_M, RTC_IO_TOUCH_PAD1_DRV_V, RTC_IO_TOUCH_PAD1_DRV_S, RTCIO_GPIO0_CHANNEL}, //0
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //1
    {RTC_IO_TOUCH_PAD2_REG, RTC_IO_TOUCH_PAD2_MUX_SEL_M, RTC_IO_TOUCH_PAD2_FUN_SEL_S, RTC_IO_TOUCH_PAD2_FUN_IE_M, RTC_IO_TOUCH_PAD2_RUE_M, RTC_IO_TOUCH_PAD2_RDE_M, RTC_IO_TOUCH_PAD2_SLP_SEL_M, RTC_IO_TOUCH_PAD2_SLP_IE_M, RTC_IO_TOUCH_PAD2_HOLD_M, RTC_CNTL_TOUCH_PAD2_HOLD_FORCE_M, RTC_IO_TOUCH_PAD2_DRV_V, RTC_IO_TOUCH_PAD2_DRV_S, RTCIO_GPIO2_CHANNEL}, //2
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //3
    {RTC_IO_TOUCH_PAD0_REG, RTC_IO_TOUCH_PAD0_MUX_SEL_M, RTC_IO_TOUCH_PAD0_FUN_SEL_S, RTC_IO_TOUCH_PAD0_FUN_IE_M, RTC_IO_TOUCH_PAD0_RUE_M, RTC_IO_TOUCH_PAD0_RDE_M, RTC_IO_TOUCH_PAD0_SLP_SEL_M, RTC_IO_TOUCH_PAD0_SLP_IE_M, RTC_IO_TOUCH_PAD0_HOLD_M,  RTC_CNTL_TOUCH_PAD0_HOLD_FORCE_M, RTC_IO_TOUCH_PAD0_DRV_V, RTC_IO_TOUCH_PAD0_DRV_S, RTCIO_GPIO4_CHANNEL}, //4
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //6
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //7
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //8
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //9
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //11
    {RTC_IO_TOUCH_PAD5_REG, RTC_IO_TOUCH_PAD5_MUX_SEL_M, RTC_IO_TOUCH_PAD5_FUN_SEL_S, RTC_IO_TOUCH_PAD5_FUN_IE_M, RTC_IO_TOUCH_PAD5_RUE_M, RTC_IO_TOUCH_PAD5_RDE_M, RTC_IO_TOUCH_PAD5_SLP_SEL_M, RTC_IO_TOUCH_PAD5_SLP_IE_M, RTC_IO_TOUCH_PAD5_HOLD_M, RTC_CNTL_TOUCH_PAD5_HOLD_FORCE_M, RTC_IO_TOUCH_PAD5_DRV_V, RTC_IO_TOUCH_PAD5_DRV_S, RTCIO_GPIO12_CHANNEL}, //12
    {RTC_IO_TOUCH_PAD4_REG, RTC_IO_TOUCH_PAD4_MUX_SEL_M, RTC_IO_TOUCH_PAD4_FUN_SEL_S, RTC_IO_TOUCH_PAD4_FUN_IE_M, RTC_IO_TOUCH_PAD4_RUE_M, RTC_IO_TOUCH_PAD4_RDE_M, RTC_IO_TOUCH_PAD4_SLP_SEL_M, RTC_IO_TOUCH_PAD4_SLP_IE_M, RTC_IO_TOUCH_PAD4_HOLD_M, RTC_CNTL_TOUCH_PAD4_HOLD_FORCE_M, RTC_IO_TOUCH_PAD4_DRV_V, RTC_IO_TOUCH_PAD4_DRV_S, RTCIO_GPIO13_CHANNEL}, //13
    {RTC_IO_TOUCH_PAD6_REG, RTC_IO_TOUCH_PAD6_MUX_SEL_M, RTC_IO_TOUCH_PAD6_FUN_SEL_S, RTC_IO_TOUCH_PAD6_FUN_IE_M, RTC_IO_TOUCH_PAD6_RUE_M, RTC_IO_TOUCH_PAD6_RDE_M, RTC_IO_TOUCH_PAD6_SLP_SEL_M, RTC_IO_TOUCH_PAD6_SLP_IE_M, RTC_IO_TOUCH_PAD6_HOLD_M, RTC_CNTL_TOUCH_PAD6_HOLD_FORCE_M, RTC_IO_TOUCH_PAD6_DRV_V, RTC_IO_TOUCH_PAD6_DRV_S, RTCIO_GPIO14_CHANNEL}, //14
    {RTC_IO_TOUCH_PAD3_REG, RTC_IO_TOUCH_PAD3_MUX_SEL_M, RTC_IO_TOUCH_PAD3_FUN_SEL_S, RTC_IO_TOUCH_PAD3_FUN_IE_M, RTC_IO_TOUCH_PAD3_RUE_M, RTC_IO_TOUCH_PAD3_RDE_M, RTC_IO_TOUCH_PAD3_SLP_SEL_M, RTC_IO_TOUCH_PAD3_SLP_IE_M, RTC_IO_TOUCH_PAD3_HOLD_M, RTC_CNTL_TOUCH_PAD3_HOLD_FORCE_M, RTC_IO_TOUCH_PAD3_DRV_V, RTC_IO_TOUCH_PAD3_DRV_S, RTCIO_GPIO15_CHANNEL}, //15
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //16
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //17
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //18
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //19
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //20
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //21
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //22
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //23
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //24
    {RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_MUX_SEL_M, RTC_IO_PDAC1_FUN_SEL_S, RTC_IO_PDAC1_FUN_IE_M, RTC_IO_PDAC1_RUE_M, RTC_IO_PDAC1_RDE_M, RTC_IO_PDAC1_SLP_SEL_M, RTC_IO_PDAC1_SLP_IE_M, RTC_IO_PDAC1_HOLD_M, RTC_CNTL_PDAC1_HOLD_FORCE_M, RTC_IO_PDAC1_DRV_V, RTC_IO_PDAC1_DRV_S, RTCIO_GPIO25_CHANNEL},                           //25
    {RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_MUX_SEL_M, RTC_IO_PDAC2_FUN_SEL_S, RTC_IO_PDAC2_FUN_IE_M, RTC_IO_PDAC2_RUE_M, RTC_IO_PDAC2_RDE_M, RTC_IO_PDAC2_SLP_SEL_M, RTC_IO_PDAC2_SLP_IE_M, RTC_IO_PDAC2_HOLD_M, RTC_CNTL_PDAC2_HOLD_FORCE_M, RTC_IO_PDAC2_DRV_V, RTC_IO_PDAC2_DRV_S, RTCIO_GPIO26_CHANNEL},                           //26
    {RTC_IO_TOUCH_PAD7_REG, RTC_IO_TOUCH_PAD7_MUX_SEL_M, RTC_IO_TOUCH_PAD7_FUN_SEL_S, RTC_IO_TOUCH_PAD7_FUN_IE_M, RTC_IO_TOUCH_PAD7_RUE_M, RTC_IO_TOUCH_PAD7_RDE_M, RTC_IO_TOUCH_PAD7_SLP_SEL_M, RTC_IO_TOUCH_PAD7_SLP_IE_M, RTC_IO_TOUCH_PAD7_HOLD_M, RTC_CNTL_TOUCH_PAD7_HOLD_FORCE_M, RTC_IO_TOUCH_PAD7_DRV_V, RTC_IO_TOUCH_PAD7_DRV_S, RTCIO_GPIO27_CHANNEL}, //27
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //28
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //29
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //30
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //31
    {RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32P_MUX_SEL_M, RTC_IO_X32P_FUN_SEL_S, RTC_IO_X32P_FUN_IE_M, RTC_IO_X32P_RUE_M, RTC_IO_X32P_RDE_M, RTC_IO_X32P_SLP_SEL_M, RTC_IO_X32P_SLP_IE_M, RTC_IO_X32P_HOLD_M, RTC_CNTL_X32P_HOLD_FORCE_M, RTC_IO_X32P_DRV_V, RTC_IO_X32P_DRV_S, RTCIO_GPIO32_CHANNEL},                            //32
    {RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_MUX_SEL_M, RTC_IO_X32N_FUN_SEL_S, RTC_IO_X32N_FUN_IE_M, RTC_IO_X32N_RUE_M, RTC_IO_X32N_RDE_M, RTC_IO_X32N_SLP_SEL_M, RTC_IO_X32N_SLP_IE_M, RTC_IO_X32N_HOLD_M, RTC_CNTL_X32N_HOLD_FORCE_M, RTC_IO_X32N_DRV_V, RTC_IO_X32N_DRV_S, RTCIO_GPIO33_CHANNEL},                            //33
    {RTC_IO_ADC_PAD_REG, RTC_IO_ADC1_MUX_SEL_M, RTC_IO_ADC1_FUN_SEL_S, RTC_IO_ADC1_FUN_IE_M, 0, 0, RTC_IO_ADC1_SLP_SEL_M, RTC_IO_ADC1_SLP_IE_M, RTC_IO_ADC1_HOLD_M, RTC_CNTL_ADC1_HOLD_FORCE_M, 0, 0, RTCIO_GPIO34_CHANNEL},                                                                //34
    {RTC_IO_ADC_PAD_REG, RTC_IO_ADC2_MUX_SEL_M, RTC_IO_ADC2_FUN_SEL_S, RTC_IO_ADC2_FUN_IE_M, 0, 0, RTC_IO_ADC2_SLP_SEL_M, RTC_IO_ADC2_SLP_IE_M, RTC_IO_ADC1_HOLD_M, RTC_CNTL_ADC2_HOLD_FORCE_M, 0, 0, RTCIO_GPIO35_CHANNEL},                                                                //35
    {RTC_IO_SENSOR_PADS_REG, RTC_IO_SENSE1_MUX_SEL_M, RTC_IO_SENSE1_FUN_SEL_S, RTC_IO_SENSE1_FUN_IE_M, 0, 0, RTC_IO_SENSE1_SLP_SEL_M, RTC_IO_SENSE1_SLP_IE_M, RTC_IO_SENSE1_HOLD_M, RTC_CNTL_SENSE1_HOLD_FORCE_M, 0, 0, RTCIO_GPIO36_CHANNEL},                                                      //36
    {RTC_IO_SENSOR_PADS_REG, RTC_IO_SENSE2_MUX_SEL_M, RTC_IO_SENSE2_FUN_SEL_S, RTC_IO_SENSE2_FUN_IE_M, 0, 0, RTC_IO_SENSE2_SLP_SEL_M, RTC_IO_SENSE2_SLP_IE_M, RTC_IO_SENSE1_HOLD_M, RTC_CNTL_SENSE2_HOLD_FORCE_M, 0, 0, RTCIO_GPIO37_CHANNEL},                                                      //37
    {RTC_IO_SENSOR_PADS_REG, RTC_IO_SENSE3_MUX_SEL_M, RTC_IO_SENSE3_FUN_SEL_S, RTC_IO_SENSE3_FUN_IE_M, 0, 0, RTC_IO_SENSE3_SLP_SEL_M, RTC_IO_SENSE3_SLP_IE_M, RTC_IO_SENSE1_HOLD_M, RTC_CNTL_SENSE3_HOLD_FORCE_M, 0, 0, RTCIO_GPIO38_CHANNEL},                                                       //38
    {RTC_IO_SENSOR_PADS_REG, RTC_IO_SENSE4_MUX_SEL_M, RTC_IO_SENSE4_FUN_SEL_S, RTC_IO_SENSE4_FUN_IE_M, 0, 0, RTC_IO_SENSE4_SLP_SEL_M, RTC_IO_SENSE4_SLP_IE_M, RTC_IO_SENSE1_HOLD_M, RTC_CNTL_SENSE4_HOLD_FORCE_M, 0, 0, RTCIO_GPIO39_CHANNEL},                                                      //39
};

/*---------------------------------------------------------------
                        RTC IO
---------------------------------------------------------------*/
esp_err_t rtc_gpio_init(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    // 0: GPIO connected to digital GPIO module. 1: GPIO connected to analog RTC module.
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, (rtc_gpio_desc[gpio_num].mux));
    //0:RTC FUNCIOTN 1,2,3:Reserved
    SET_PERI_REG_BITS(rtc_gpio_desc[gpio_num].reg, RTC_IO_TOUCH_PAD1_FUN_SEL_V, 0x0, rtc_gpio_desc[gpio_num].func);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t rtc_gpio_deinit(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    //Select Gpio as Digital Gpio
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, (rtc_gpio_desc[gpio_num].mux));
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

static esp_err_t rtc_gpio_output_enable(gpio_num_t gpio_num)
{
    int rtc_gpio_num = rtc_gpio_desc[gpio_num].rtc_num;
    RTC_MODULE_CHECK(rtc_gpio_num != -1, "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    SET_PERI_REG_MASK(RTC_GPIO_ENABLE_W1TS_REG, (1 << (rtc_gpio_num + RTC_GPIO_ENABLE_W1TS_S)));
    CLEAR_PERI_REG_MASK(RTC_GPIO_ENABLE_W1TC_REG, (1 << (rtc_gpio_num + RTC_GPIO_ENABLE_W1TC_S)));

    return ESP_OK;
}

static esp_err_t rtc_gpio_output_disable(gpio_num_t gpio_num)
{
    int rtc_gpio_num = rtc_gpio_desc[gpio_num].rtc_num;
    RTC_MODULE_CHECK(rtc_gpio_num != -1, "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    CLEAR_PERI_REG_MASK(RTC_GPIO_ENABLE_W1TS_REG, (1 << (rtc_gpio_num + RTC_GPIO_ENABLE_W1TS_S)));
    SET_PERI_REG_MASK(RTC_GPIO_ENABLE_W1TC_REG, (1 << ( rtc_gpio_num + RTC_GPIO_ENABLE_W1TC_S)));

    return ESP_OK;
}

static esp_err_t rtc_gpio_input_enable(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].ie);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

static esp_err_t rtc_gpio_input_disable(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].ie);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t rtc_gpio_set_level(gpio_num_t gpio_num, uint32_t level)
{
    int rtc_gpio_num = rtc_gpio_num = rtc_gpio_desc[gpio_num].rtc_num;;
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);

    if (level) {
        WRITE_PERI_REG(RTC_GPIO_OUT_W1TS_REG, (1 << (rtc_gpio_num + RTC_GPIO_OUT_DATA_W1TS_S)));
    } else {
        WRITE_PERI_REG(RTC_GPIO_OUT_W1TC_REG, (1 << (rtc_gpio_num + RTC_GPIO_OUT_DATA_W1TC_S)));
    }

    return ESP_OK;
}

uint32_t rtc_gpio_get_level(gpio_num_t gpio_num)
{
    uint32_t level = 0;
    int rtc_gpio_num = rtc_gpio_desc[gpio_num].rtc_num;
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    level = READ_PERI_REG(RTC_GPIO_IN_REG);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ((level >> (RTC_GPIO_IN_NEXT_S + rtc_gpio_num)) & 0x01);
}

esp_err_t rtc_gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Output pad only", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(strength < GPIO_DRIVE_CAP_MAX, "GPIO drive capability error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_BITS(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].drv_v, strength, rtc_gpio_desc[gpio_num].drv_s);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t rtc_gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t* strength)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Output pad only", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(strength != NULL, "GPIO drive pointer error", ESP_ERR_INVALID_ARG);

    *strength = GET_PERI_REG_BITS2(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].drv_v, rtc_gpio_desc[gpio_num].drv_s);
    return ESP_OK;
}

esp_err_t rtc_gpio_set_direction(gpio_num_t gpio_num, rtc_gpio_mode_t mode)
{
    RTC_MODULE_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);

    switch (mode) {
    case RTC_GPIO_MODE_INPUT_ONLY:
        rtc_gpio_output_disable(gpio_num);
        rtc_gpio_input_enable(gpio_num);
        break;
    case RTC_GPIO_MODE_OUTPUT_ONLY:
        rtc_gpio_output_enable(gpio_num);
        rtc_gpio_input_disable(gpio_num);
        break;
    case RTC_GPIO_MODE_INPUT_OUTPUT:
        rtc_gpio_output_enable(gpio_num);
        rtc_gpio_input_enable(gpio_num);
        break;
    case RTC_GPIO_MODE_DISABLED:
        rtc_gpio_output_disable(gpio_num);
        rtc_gpio_input_disable(gpio_num);
        break;
    }

    return ESP_OK;
}

esp_err_t rtc_gpio_pullup_en(gpio_num_t gpio_num)
{
    //this is a digital pad
    if (rtc_gpio_desc[gpio_num].pullup == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    //this is a rtc pad
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].pullup);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t rtc_gpio_pulldown_en(gpio_num_t gpio_num)
{
    //this is a digital pad
    if (rtc_gpio_desc[gpio_num].pulldown == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    //this is a rtc pad
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].pulldown);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t rtc_gpio_pullup_dis(gpio_num_t gpio_num)
{
    //this is a digital pad
    if ( rtc_gpio_desc[gpio_num].pullup == 0 ) {
        return ESP_ERR_INVALID_ARG;
    }

    //this is a rtc pad
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].pullup);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t rtc_gpio_pulldown_dis(gpio_num_t gpio_num)
{
    //this is a digital pad
    if (rtc_gpio_desc[gpio_num].pulldown == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    //this is a rtc pad
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].pulldown);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t rtc_gpio_hold_en(gpio_num_t gpio_num)
{
    // check if an RTC IO
    if (rtc_gpio_desc[gpio_num].pullup == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].hold);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t rtc_gpio_hold_dis(gpio_num_t gpio_num)
{
    // check if an RTC IO
    if (rtc_gpio_desc[gpio_num].pullup == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].hold);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}


void rtc_gpio_force_hold_dis_all()
{
    for (int gpio = 0; gpio < GPIO_PIN_COUNT; ++gpio) {
        const rtc_gpio_desc_t* desc = &rtc_gpio_desc[gpio];
        if (desc->hold_force != 0) {
            REG_CLR_BIT(RTC_CNTL_HOLD_FORCE_REG, desc->hold_force);
        }
    }
}


/*---------------------------------------------------------------
                    Touch Pad
---------------------------------------------------------------*/
//Some register bits of touch sensor 8 and 9 are mismatched, we need to swap the bits.
#define BITSWAP(data, n, m)   (((data >> n) &  0x1)  == ((data >> m) & 0x1) ? (data) : ((data) ^ ((0x1 <<n) | (0x1 << m))))
#define TOUCH_BITS_SWAP(v)  BITSWAP(v, TOUCH_PAD_NUM8, TOUCH_PAD_NUM9)

//Some registers of touch sensor 8 and 9 are mismatched, we need to swap register index
inline static touch_pad_t touch_pad_num_wrap(touch_pad_t touch_num)
{
    if (touch_num == TOUCH_PAD_NUM8) {
        return TOUCH_PAD_NUM9;
    } else if (touch_num == TOUCH_PAD_NUM9) {
        return TOUCH_PAD_NUM8;
    }
    return touch_num;
}

esp_err_t touch_pad_isr_handler_register(void (*fn)(void *), void *arg, int no_use, intr_handle_t *handle_no_use)
{
    RTC_MODULE_CHECK(fn, "Touch_Pad ISR null", ESP_ERR_INVALID_ARG);
    return rtc_isr_register(fn, arg, RTC_CNTL_TOUCH_INT_ST_M);
}

esp_err_t touch_pad_isr_register(intr_handler_t fn, void* arg)
{
    RTC_MODULE_CHECK(fn, "Touch_Pad ISR null", ESP_ERR_INVALID_ARG);
    return rtc_isr_register(fn, arg, RTC_CNTL_TOUCH_INT_ST_M);
}

esp_err_t touch_pad_isr_deregister(intr_handler_t fn, void *arg)
{
    return rtc_isr_deregister(fn, arg);
}

static esp_err_t touch_pad_get_io_num(touch_pad_t touch_num, gpio_num_t *gpio_num)
{
    switch (touch_num) {
    case TOUCH_PAD_NUM0:
        *gpio_num = TOUCH_PAD_NUM0_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM1:
        *gpio_num = TOUCH_PAD_NUM1_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM2:
        *gpio_num = TOUCH_PAD_NUM2_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM3:
        *gpio_num = TOUCH_PAD_NUM3_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM4:
        *gpio_num = TOUCH_PAD_NUM4_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM5:
        *gpio_num = TOUCH_PAD_NUM5_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM6:
        *gpio_num = TOUCH_PAD_NUM6_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM7:
        *gpio_num = TOUCH_PAD_NUM7_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM8:
        *gpio_num = TOUCH_PAD_NUM8_GPIO_NUM;
        break;
    case TOUCH_PAD_NUM9:
        *gpio_num = TOUCH_PAD_NUM9_GPIO_NUM;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

static uint32_t _touch_filter_iir(uint32_t in_now, uint32_t out_last, uint32_t k)
{
    if (k == 0) {
        return in_now;
    } else {
        uint32_t out_now = (in_now + (k - 1) * out_last) / k;
        return out_now;
    }
}

static void touch_pad_filter_cb(void *arg)
{
    if (s_touch_pad_filter == NULL) {
        return;
    }
    uint16_t val;
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        touch_pad_read(i, &val);
        s_touch_pad_filter->filtered_val[i] = s_touch_pad_filter->filtered_val[i] == 0 ? (val << TOUCH_PAD_SHIFT_DEFAULT) : s_touch_pad_filter->filtered_val[i];
        s_touch_pad_filter->filtered_val[i] = _touch_filter_iir((val << TOUCH_PAD_SHIFT_DEFAULT),
                s_touch_pad_filter->filtered_val[i], TOUCH_PAD_FILTER_FACTOR_DEFAULT);
    }
}

esp_err_t touch_pad_set_meas_time(uint16_t sleep_cycle, uint16_t meas_cycle)
{
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    portENTER_CRITICAL(&rtc_spinlock);
    //touch sensor sleep cycle Time = sleep_cycle / RTC_SLOW_CLK( can be 150k or 32k depending on the options)
    SENS.sar_touch_ctrl2.touch_sleep_cycles = sleep_cycle;
    //touch sensor measure time= meas_cycle / 8Mhz
    SENS.sar_touch_ctrl1.touch_meas_delay = meas_cycle;
    portEXIT_CRITICAL(&rtc_spinlock);
    xSemaphoreGive(rtc_touch_mux);
    return ESP_OK;
}

esp_err_t touch_pad_get_meas_time(uint16_t *sleep_cycle, uint16_t *meas_cycle)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (sleep_cycle) {
        *sleep_cycle = SENS.sar_touch_ctrl2.touch_sleep_cycles;
    }
    if (meas_cycle) {
        *meas_cycle = SENS.sar_touch_ctrl1.touch_meas_delay;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_set_voltage(touch_high_volt_t refh, touch_low_volt_t refl, touch_volt_atten_t atten)
{
    RTC_MODULE_CHECK(((refh < TOUCH_HVOLT_MAX) && (refh >= (int )TOUCH_HVOLT_KEEP)), "touch refh error",
            ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(((refl < TOUCH_LVOLT_MAX) && (refh >= (int )TOUCH_LVOLT_KEEP)), "touch refl error",
            ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(((atten < TOUCH_HVOLT_ATTEN_MAX) && (refh >= (int )TOUCH_HVOLT_ATTEN_KEEP)), "touch atten error",
            ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    if (refh > TOUCH_HVOLT_KEEP) {
        RTCIO.touch_cfg.drefh = refh;
    }
    if (refl > TOUCH_LVOLT_KEEP) {
        RTCIO.touch_cfg.drefl = refl;
    }
    if (atten > TOUCH_HVOLT_ATTEN_KEEP) {
        RTCIO.touch_cfg.drange = atten;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_voltage(touch_high_volt_t *refh, touch_low_volt_t *refl, touch_volt_atten_t *atten)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (refh) {
        *refh = RTCIO.touch_cfg.drefh;
    }
    if (refl) {
        *refl = RTCIO.touch_cfg.drefl;
    }
    if (atten) {
        *atten = RTCIO.touch_cfg.drange;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_set_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t slope, touch_tie_opt_t opt)
{
    RTC_MODULE_CHECK((slope < TOUCH_PAD_SLOPE_MAX), "touch slope error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((opt < TOUCH_PAD_TIE_OPT_MAX), "touch opt error", ESP_ERR_INVALID_ARG);
    
    touch_pad_t touch_pad_wrap = touch_pad_num_wrap(touch_num);
    portENTER_CRITICAL(&rtc_spinlock);
    RTCIO.touch_pad[touch_pad_wrap].tie_opt = opt;
    RTCIO.touch_pad[touch_num].dac = slope;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t *slope, touch_tie_opt_t *opt)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    
    touch_pad_t touch_pad_wrap = touch_pad_num_wrap(touch_num);
    portENTER_CRITICAL(&rtc_spinlock);
    if(opt) {
        *opt = RTCIO.touch_pad[touch_pad_wrap].tie_opt;
    }
    if(slope) {
        *slope = RTCIO.touch_pad[touch_num].dac;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_io_init(touch_pad_t touch_num)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    gpio_num_t gpio_num = GPIO_NUM_0;
    touch_pad_get_io_num(touch_num, &gpio_num);
    rtc_gpio_init(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_pulldown_dis(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
    return ESP_OK;
}

esp_err_t touch_pad_set_fsm_mode(touch_fsm_mode_t mode)
{
    RTC_MODULE_CHECK((mode < TOUCH_FSM_MODE_MAX), "touch fsm mode error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl2.touch_start_en = 0;
    SENS.sar_touch_ctrl2.touch_start_force = mode;
    RTCCNTL.state0.touch_slp_timer_en = (mode == TOUCH_FSM_MODE_TIMER ? 1 : 0);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_fsm_mode(touch_fsm_mode_t *mode)
{
    if (mode) {
        *mode = SENS.sar_touch_ctrl2.touch_start_force;
    }
    return ESP_OK;
}

esp_err_t touch_pad_sw_start()
{
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl2.touch_start_en = 0;
    SENS.sar_touch_ctrl2.touch_start_en = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint16_t threshold)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    touch_pad_t tp_wrap = touch_pad_num_wrap(touch_num);
    portENTER_CRITICAL(&rtc_spinlock);
    if (tp_wrap & 0x1) {
        SENS.touch_thresh[tp_wrap / 2].l_thresh = threshold;
    } else {
        SENS.touch_thresh[tp_wrap / 2].h_thresh = threshold;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint16_t *threshold)
{
    RTC_MODULE_CHECK((touch_num < TOUCH_PAD_MAX), "touch IO error", ESP_ERR_INVALID_ARG);
    touch_pad_t tp_wrap = touch_pad_num_wrap(touch_num);
    if (threshold) {
        *threshold = (tp_wrap & 0x1 )? \
                SENS.touch_thresh[tp_wrap / 2].l_thresh : \
                SENS.touch_thresh[tp_wrap / 2].h_thresh;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_trigger_mode(touch_trigger_mode_t mode)
{
    RTC_MODULE_CHECK((mode < TOUCH_TRIGGER_MAX), "touch trigger mode error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl1.touch_out_sel = mode;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_trigger_mode(touch_trigger_mode_t *mode)
{
    if (mode) {
        *mode = SENS.sar_touch_ctrl1.touch_out_sel;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_trigger_source(touch_trigger_src_t src)
{
    RTC_MODULE_CHECK((src < TOUCH_TRIGGER_SOURCE_MAX), "touch trigger source error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl1.touch_out_1en = src;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_get_trigger_source(touch_trigger_src_t *src)
{
    if (src) {
        *src = SENS.sar_touch_ctrl1.touch_out_1en;
    }
    return ESP_OK;
}

esp_err_t touch_pad_set_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask)
{
    RTC_MODULE_CHECK((set1_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set1 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((set2_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set2 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((en_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch work_en bitmask error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_enable.touch_pad_outen1 |= TOUCH_BITS_SWAP(set1_mask);
    SENS.sar_touch_enable.touch_pad_outen2 |= TOUCH_BITS_SWAP(set2_mask);
    SENS.sar_touch_enable.touch_pad_worken |= TOUCH_BITS_SWAP(en_mask);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t touch_pad_get_group_mask(uint16_t *set1_mask, uint16_t *set2_mask, uint16_t *en_mask)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (set1_mask) {
        *set1_mask = TOUCH_BITS_SWAP(SENS.sar_touch_enable.touch_pad_outen1);
    }
    if (set2_mask) {
        *set2_mask = TOUCH_BITS_SWAP(SENS.sar_touch_enable.touch_pad_outen2);
    }
    if (en_mask) {
        *en_mask = TOUCH_BITS_SWAP(SENS.sar_touch_enable.touch_pad_worken);
    }
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t touch_pad_clear_group_mask(uint16_t set1_mask, uint16_t set2_mask, uint16_t en_mask)
{
    RTC_MODULE_CHECK((set1_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set1 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((set2_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch set2 bitmask error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK((en_mask <= TOUCH_PAD_BIT_MASK_MAX), "touch work_en bitmask error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_enable.touch_pad_outen1 &= TOUCH_BITS_SWAP(~set1_mask);
    SENS.sar_touch_enable.touch_pad_outen2 &= TOUCH_BITS_SWAP(~set2_mask);
    SENS.sar_touch_enable.touch_pad_worken &= TOUCH_BITS_SWAP(~en_mask);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

uint32_t IRAM_ATTR touch_pad_get_status()
{
    uint32_t status = SENS.sar_touch_ctrl2.touch_meas_en;
    return TOUCH_BITS_SWAP(status);
}

esp_err_t IRAM_ATTR touch_pad_clear_status()
{
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl2.touch_meas_en_clr = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_intr_enable()
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.int_ena.rtc_touch = 1;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_intr_disable()
{
    portENTER_CRITICAL(&rtc_spinlock);
    RTCCNTL.int_ena.rtc_touch = 0;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t touch_pad_config(touch_pad_t touch_num, uint16_t threshold)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    touch_pad_set_thresh(touch_num, threshold);
    touch_pad_io_init(touch_num);
    touch_pad_set_cnt_mode(touch_num, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_HIGH);
    touch_pad_set_group_mask((1 << touch_num), (1 << touch_num), (1 << touch_num));
    return ESP_OK;
}

esp_err_t touch_pad_init()
{
    if (rtc_touch_mux == NULL) {
        rtc_touch_mux = xSemaphoreCreateMutex();
    }
    if (rtc_touch_mux == NULL) {
        return ESP_FAIL;
    }
    touch_pad_intr_disable();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_DEFAULT);
    touch_pad_set_trigger_mode(TOUCH_TRIGGER_MODE_DEFAULT);
    touch_pad_set_trigger_source(TOUCH_TRIGGER_SOURCE_DEFAULT);
    touch_pad_clear_status();
    touch_pad_set_meas_time(TOUCH_PAD_SLEEP_CYCLE_DEFAULT, TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    return ESP_OK;
}

esp_err_t touch_pad_deinit()
{
    if (rtc_touch_mux == NULL) {
        return ESP_FAIL;
    }
    touch_pad_filter_delete();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW);
    touch_pad_clear_status();
    touch_pad_intr_disable();
    vSemaphoreDelete(rtc_touch_mux);
    rtc_touch_mux = NULL;
    return ESP_OK;
}

esp_err_t touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value)
{
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);

    touch_pad_t tp_wrap = touch_pad_num_wrap(touch_num);
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    while (SENS.sar_touch_ctrl2.touch_meas_done == 0) {};
    *touch_value = (tp_wrap & 0x1) ? \
                        SENS.touch_meas[tp_wrap / 2].l_val: \
                        SENS.touch_meas[tp_wrap / 2].h_val;
    xSemaphoreGive(rtc_touch_mux);
    return ESP_OK;
}

IRAM_ATTR esp_err_t touch_pad_read_filtered(touch_pad_t touch_num, uint16_t *touch_value)
{
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_FAIL);
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(touch_value != NULL, "touch_value", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    *touch_value = (s_touch_pad_filter->filtered_val[touch_num] >> TOUCH_PAD_SHIFT_DEFAULT);
    return ESP_OK;
}

esp_err_t touch_pad_set_filter_period(uint32_t new_period_ms)
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    RTC_MODULE_CHECK(new_period_ms > 0, "Touch pad filter period error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        xTimerChangePeriod(s_touch_pad_filter->timer, new_period_ms / portTICK_PERIOD_MS, portMAX_DELAY);
        s_touch_pad_filter->period = new_period_ms;
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_get_filter_period(uint32_t* p_period_ms)
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    RTC_MODULE_CHECK(p_period_ms != NULL, "Touch pad period pointer error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        *p_period_ms = s_touch_pad_filter->period;
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_start(uint32_t filter_period_ms)
{
    RTC_MODULE_CHECK(filter_period_ms >= portTICK_PERIOD_MS, "Touch pad filter period error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(rtc_touch_mux != NULL, "Touch pad not initialized", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter == NULL) {
        s_touch_pad_filter = (touch_pad_filter_t *) calloc(1, sizeof(touch_pad_filter_t));
        if (s_touch_pad_filter == NULL) {
            ret = ESP_ERR_NO_MEM;
        }
    }
    if (s_touch_pad_filter->timer == NULL) {
        s_touch_pad_filter->timer = xTimerCreate("filter_tmr", filter_period_ms / portTICK_PERIOD_MS, pdTRUE,
        NULL, touch_pad_filter_cb);
        if (s_touch_pad_filter->timer == NULL) {
            ret = ESP_ERR_NO_MEM;
        }
        xTimerStart(s_touch_pad_filter->timer, portMAX_DELAY);
        s_touch_pad_filter->enable = true;
    } else {
        xTimerChangePeriod(s_touch_pad_filter->timer, filter_period_ms / portTICK_PERIOD_MS, portMAX_DELAY);
        s_touch_pad_filter->period = filter_period_ms;
        xTimerStart(s_touch_pad_filter->timer, portMAX_DELAY);
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_stop()
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        xTimerStop(s_touch_pad_filter->timer, portMAX_DELAY);
        s_touch_pad_filter->enable = false;
    } else {
        ESP_LOGE(RTC_MODULE_TAG, "Touch pad filter deleted");
        ret = ESP_ERR_INVALID_STATE;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ret;
}

esp_err_t touch_pad_filter_delete()
{
    RTC_MODULE_CHECK(s_touch_pad_filter != NULL, "Touch pad filter not initialized", ESP_ERR_INVALID_STATE);
    xSemaphoreTake(rtc_touch_mux, portMAX_DELAY);
    if (s_touch_pad_filter != NULL) {
        if (s_touch_pad_filter->timer != NULL) {
            xTimerStop(s_touch_pad_filter->timer, portMAX_DELAY);
            xTimerDelete(s_touch_pad_filter->timer, portMAX_DELAY);
            s_touch_pad_filter->timer = NULL;
        }
        free(s_touch_pad_filter);
        s_touch_pad_filter = NULL;
    }
    xSemaphoreGive(rtc_touch_mux);
    return ESP_OK;
}

/*---------------------------------------------------------------
                    ADC Common
---------------------------------------------------------------*/
static esp_err_t adc_set_fsm_time(int rst_wait, int start_wait, int standby_wait, int sample_cycle)
{
    portENTER_CRITICAL(&rtc_spinlock);
    // Internal FSM reset wait time
    if (rst_wait >= 0) {
        SYSCON.saradc_fsm.rstb_wait = rst_wait;
    }
    // Internal FSM start wait time
    if (start_wait >= 0) {
        SYSCON.saradc_fsm.start_wait = start_wait;
    }
    // Internal FSM standby wait time
    if (standby_wait >= 0) {
        SYSCON.saradc_fsm.standby_wait = standby_wait;
    }
    // Internal FSM standby sample cycle
    if (sample_cycle >= 0) {
        SYSCON.saradc_fsm.sample_cycle = sample_cycle;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

static esp_err_t adc_set_data_format(adc_i2s_encode_t mode)
{
    portENTER_CRITICAL(&rtc_spinlock);
    //data format:
    //0: ADC_ENCODE_12BIT  [15:12]-channel [11:0]-12 bits ADC data
    //1: ADC_ENCODE_11BIT  [15]-1 [14:11]-channel [10:0]-11 bits ADC data, the resolution should not be larger than 11 bits in this case.
    SYSCON.saradc_ctrl.data_sar_sel = mode;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

static esp_err_t adc_set_measure_limit(uint8_t meas_num, bool lim_en)
{
    portENTER_CRITICAL(&rtc_spinlock);
    // Set max measure number
    SYSCON.saradc_ctrl2.max_meas_num = meas_num;
    // Enable max measure number limit
    SYSCON.saradc_ctrl2.meas_num_limit = lim_en;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

static esp_err_t adc_set_work_mode(adc_unit_t adc_unit)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (adc_unit == ADC_UNIT_1) {
        // saradc mode sel : 0--single saradc;  1--double saradc;  2--alternative saradc
        SYSCON.saradc_ctrl.work_mode = 0;
        //ENABLE ADC  0: ADC1  1: ADC2, only work for single SAR mode
        SYSCON.saradc_ctrl.sar_sel = 0;
    } else if (adc_unit == ADC_UNIT_2) {
        // saradc mode sel : 0--single saradc;  1--double saradc;  2--alternative saradc
        SYSCON.saradc_ctrl.work_mode = 0;
        //ENABLE ADC1  0: SAR1  1: SAR2  only work for single SAR mode
        SYSCON.saradc_ctrl.sar_sel = 1;
    } else if (adc_unit == ADC_UNIT_BOTH) {
        // saradc mode sel : 0--single saradc;  1--double saradc;  2--alternative saradc
        SYSCON.saradc_ctrl.work_mode = 1;
    } else if (adc_unit == ADC_UNIT_ALTER) {
        // saradc mode sel : 0--single saradc;  1--double saradc;  2--alternative saradc
        SYSCON.saradc_ctrl.work_mode = 2;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

static esp_err_t adc_set_atten(adc_unit_t adc_unit, adc_channel_t channel, adc_atten_t atten)
{
    ADC_CHECK_UNIT(adc_unit);
    if (adc_unit & ADC_UNIT_1) {
        RTC_MODULE_CHECK((adc1_channel_t)channel < ADC1_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);
    }
    RTC_MODULE_CHECK(atten < ADC_ATTEN_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    if (adc_unit & ADC_UNIT_1) {
        //SAR1_atten
        SET_PERI_REG_BITS(SENS_SAR_ATTEN1_REG, SENS_SAR1_ATTEN_VAL_MASK, atten, (channel * 2));
    }
    if (adc_unit & ADC_UNIT_2) {
        //SAR2_atten
        SET_PERI_REG_BITS(SENS_SAR_ATTEN2_REG, SENS_SAR2_ATTEN_VAL_MASK, atten, (channel * 2));
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

void adc_power_on()
{
    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_meas_wait2.force_xpd_sar = ADC_FORCE_FSM;
    portEXIT_CRITICAL(&rtc_spinlock);
}

void adc_power_off()
{
    portENTER_CRITICAL(&rtc_spinlock);
    //Bit1  0:Fsm  1: SW mode
    //Bit0  0:SW mode power down  1: SW mode power on
    SENS.sar_meas_wait2.force_xpd_sar = ADC_FORCE_DISABLE;
    portEXIT_CRITICAL(&rtc_spinlock);
}

esp_err_t adc_set_clk_div(uint8_t clk_div)
{
    portENTER_CRITICAL(&rtc_spinlock);
    // ADC clock devided from APB clk, 80 / 2 = 40Mhz,
    SYSCON.saradc_ctrl.sar_clk_div = clk_div;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t adc_set_i2s_data_source(adc_i2s_source_t src)
{
    RTC_MODULE_CHECK(src < ADC_I2S_DATA_SRC_MAX, "ADC i2s data source error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    // 1: I2S input data is from SAR ADC (for DMA)  0: I2S input data is from GPIO matrix
    SYSCON.saradc_ctrl.data_to_i2s = src;
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t adc_gpio_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    ADC_CHECK_UNIT(adc_unit);
    gpio_num_t gpio_num = 0;
    if (adc_unit & ADC_UNIT_1) {
        RTC_MODULE_CHECK((adc1_channel_t) channel < ADC1_CHANNEL_MAX, "ADC1 channel error", ESP_ERR_INVALID_ARG);
        ADC1_CHECK_FUNCTION_RET(adc1_pad_get_io_num((adc1_channel_t) channel, &gpio_num));
        ADC1_CHECK_FUNCTION_RET(rtc_gpio_init(gpio_num));
        ADC1_CHECK_FUNCTION_RET(rtc_gpio_output_disable(gpio_num));
        ADC1_CHECK_FUNCTION_RET(rtc_gpio_input_disable(gpio_num));
        ADC1_CHECK_FUNCTION_RET(gpio_set_pull_mode(gpio_num, GPIO_FLOATING));
    }
    return ESP_OK;
}

esp_err_t adc_set_data_inv(adc_unit_t adc_unit, bool inv_en)
{
    portENTER_CRITICAL(&rtc_spinlock);
    if (adc_unit & ADC_UNIT_1) {
        // Enable ADC data invert
        SENS.sar_read_ctrl.sar1_data_inv = inv_en;
    }
    if (adc_unit & ADC_UNIT_2) {
        // Enable ADC data invert
        SENS.sar_read_ctrl2.sar2_data_inv = inv_en;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t adc_set_data_width(adc_unit_t adc_unit, adc_bits_width_t bits)
{
    ADC_CHECK_UNIT(adc_unit);
    RTC_MODULE_CHECK(bits < ADC_WIDTH_MAX, "ADC bit width error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    if (adc_unit & ADC_UNIT_1) {
        SENS.sar_start_force.sar1_bit_width = bits;
        SENS.sar_read_ctrl.sar1_sample_bit = bits;
    }
    if (adc_unit & ADC_UNIT_2) {
        SENS.sar_start_force.sar2_bit_width = bits;
        SENS.sar_read_ctrl2.sar2_sample_bit = bits;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

/*-------------------------------------------------------------------------------------
 *                      ADC I2S
 *------------------------------------------------------------------------------------*/
static esp_err_t adc_set_i2s_data_len(adc_unit_t adc_unit, int patt_len)
{
    ADC_CHECK_UNIT(adc_unit);
    RTC_MODULE_CHECK((patt_len < ADC_PATT_LEN_MAX) && (patt_len > 0), "ADC pattern length error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    if(adc_unit & ADC_UNIT_1) {
        SYSCON.saradc_ctrl.sar1_patt_len = patt_len - 1;
    }
    if(adc_unit & ADC_UNIT_2) {
        SYSCON.saradc_ctrl.sar2_patt_len = patt_len - 1;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

static esp_err_t adc_set_i2s_data_pattern(adc_unit_t adc_unit, int seq_num, adc_channel_t channel, adc_bits_width_t bits, adc_atten_t atten)
{
    ADC_CHECK_UNIT(adc_unit);
    if (adc_unit & ADC_UNIT_1) {
        RTC_MODULE_CHECK((adc1_channel_t) channel < ADC1_CHANNEL_MAX, "ADC1 channel error", ESP_ERR_INVALID_ARG);
    }
    RTC_MODULE_CHECK(bits < ADC_WIDTH_MAX, "ADC bit width error", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(atten < ADC_ATTEN_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    //Configure pattern table, each 8 bit defines one channel
    //[7:4]-channel [3:2]-bit width [1:0]- attenuation
    //BIT WIDTH: 3: 12BIT  2: 11BIT  1: 10BIT  0: 9BIT
    //ATTEN: 3: ATTEN = 11dB 2: 6dB 1: 2.5dB 0: 0dB
    uint8_t val = (channel << 4) | (bits << 2) | (atten << 0);
    if (adc_unit & ADC_UNIT_1) {
        SYSCON.saradc_sar1_patt_tab[seq_num / 4] &= (~(0xff << ((3 - (seq_num % 4)) * 8)));
        SYSCON.saradc_sar1_patt_tab[seq_num / 4] |= (val << ((3 - (seq_num % 4)) * 8));
    }
    if (adc_unit & ADC_UNIT_2) {
        SYSCON.saradc_sar2_patt_tab[seq_num / 4] &= (~(0xff << ((3 - (seq_num % 4)) * 8)));
        SYSCON.saradc_sar2_patt_tab[seq_num / 4] |= (val << ((3 - (seq_num % 4)) * 8));
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t adc_i2s_mode_init(adc_unit_t adc_unit, adc_channel_t channel)
{
    ADC_CHECK_UNIT(adc_unit);
    if (adc_unit & ADC_UNIT_1) {
        RTC_MODULE_CHECK((adc1_channel_t) channel < ADC1_CHANNEL_MAX, "ADC1 channel error", ESP_ERR_INVALID_ARG);
    }

    uint8_t table_len = 1;
    //POWER ON SAR
    adc_power_on();
    adc_gpio_init(adc_unit, channel);
    adc_set_i2s_data_len(adc_unit, table_len);
    adc_set_i2s_data_pattern(adc_unit, 0, channel, ADC_WIDTH_BIT_12, ADC_ATTEN_DB_11);
    portENTER_CRITICAL(&rtc_spinlock);
    if (adc_unit & ADC_UNIT_1) {
        //switch SARADC into DIG channel
        SENS.sar_read_ctrl.sar1_dig_force = 1;
    }
    if (adc_unit & ADC_UNIT_2) {
        //switch SARADC into DIG channel
        SENS.sar_read_ctrl2.sar2_dig_force = 1;
        //1: SAR ADC2 is controlled by DIG ADC2 CTRL 0: SAR ADC2 is controlled by PWDET CTRL
        SYSCON.saradc_ctrl.sar2_mux = 1;
    }
    portEXIT_CRITICAL(&rtc_spinlock);
    adc_set_i2s_data_source(ADC_I2S_DATA_SRC_ADC);
    adc_set_clk_div(SAR_ADC_CLK_DIV_DEFUALT);
    // Set internal FSM wait time.
    adc_set_fsm_time(ADC_FSM_RSTB_WAIT_DEFAULT, ADC_FSM_START_WAIT_DEFAULT, ADC_FSM_STANDBY_WAIT_DEFAULT,
            ADC_FSM_TIME_KEEP);
    adc_set_work_mode(adc_unit);
    adc_set_data_format(ADC_ENCODE_12BIT);
    adc_set_measure_limit(ADC_MAX_MEAS_NUM_DEFAULT, ADC_MEAS_NUM_LIM_DEFAULT);
    //Invert The Level, Invert SAR ADC1 data
    adc_set_data_inv(adc_unit, true);
    return ESP_OK;
 }

/*-------------------------------------------------------------------------------------
 *                      ADC1
 *------------------------------------------------------------------------------------*/
esp_err_t adc1_pad_get_io_num(adc1_channel_t channel, gpio_num_t *gpio_num)
{
    RTC_MODULE_CHECK(channel < ADC1_CHANNEL_MAX, "ADC1 Channel Err", ESP_ERR_INVALID_ARG);

    switch (channel) {
    case ADC1_CHANNEL_0:
        *gpio_num = ADC1_CHANNEL_0_GPIO_NUM;
        break;
    case ADC1_CHANNEL_1:
        *gpio_num = ADC1_CHANNEL_1_GPIO_NUM;
        break;
    case ADC1_CHANNEL_2:
        *gpio_num = ADC1_CHANNEL_2_GPIO_NUM;
        break;
    case ADC1_CHANNEL_3:
        *gpio_num = ADC1_CHANNEL_3_GPIO_NUM;
        break;
    case ADC1_CHANNEL_4:
        *gpio_num = ADC1_CHANNEL_4_GPIO_NUM;
        break;
    case ADC1_CHANNEL_5:
        *gpio_num = ADC1_CHANNEL_5_GPIO_NUM;
        break;
    case ADC1_CHANNEL_6:
        *gpio_num = ADC1_CHANNEL_6_GPIO_NUM;
        break;
    case ADC1_CHANNEL_7:
        *gpio_num = ADC1_CHANNEL_7_GPIO_NUM;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)
{
    RTC_MODULE_CHECK(channel < ADC1_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(atten < ADC_ATTEN_MAX, "ADC Atten Err", ESP_ERR_INVALID_ARG);
    adc_gpio_init(ADC_UNIT_1, channel);
    adc_set_atten(ADC_UNIT_1, channel, atten);
    return ESP_OK;
}

esp_err_t adc1_config_width(adc_bits_width_t width_bit)
{
    RTC_MODULE_CHECK(width_bit < ADC_WIDTH_MAX, "ADC bit width error", ESP_ERR_INVALID_ARG);
    adc_set_data_width(ADC_UNIT_1, width_bit);
    adc_set_data_inv(ADC_UNIT_1, true);
    return ESP_OK;
}

int adc1_get_raw(adc1_channel_t channel)
{
    uint16_t adc_value;
    RTC_MODULE_CHECK(channel < ADC1_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);

    adc_power_on(); 

    portENTER_CRITICAL(&rtc_spinlock);
    //Adc Controler is Rtc module,not ulp coprocessor
    SENS.sar_meas_start1.meas1_start_force = 1;
    //Disable Amp Bit1=0:Fsm  Bit1=1(Bit0=0:PownDown Bit10=1:Powerup)
    SENS.sar_meas_wait2.force_xpd_amp = 0x2;
    //Open the ADC1 Data port Not ulp coprocessor
    SENS.sar_meas_start1.sar1_en_pad_force = 1;
    //Select channel
    SENS.sar_meas_start1.sar1_en_pad = (1 << channel);
    SENS.sar_meas_ctrl.amp_rst_fb_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_gnd_fsm = 0;
    SENS.sar_meas_wait1.sar_amp_wait1 = 1;
    SENS.sar_meas_wait1.sar_amp_wait2 = 1;
    SENS.sar_meas_wait2.sar_amp_wait3 = 1;
    while (SENS.sar_slave_addr1.meas_status != 0);
    SENS.sar_meas_start1.meas1_start_sar = 0;
    SENS.sar_meas_start1.meas1_start_sar = 1;
    while (SENS.sar_meas_start1.meas1_done_sar == 0);
    adc_value = SENS.sar_meas_start1.meas1_data_sar;
    portEXIT_CRITICAL(&rtc_spinlock);
    return adc_value;
}

int adc1_get_voltage(adc1_channel_t channel)    //Deprecated. Use adc1_get_raw() instead
{
    return adc1_get_raw(channel);
}

void adc1_ulp_enable(void)
{
    adc_power_on();

    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_meas_start1.meas1_start_force = 0;
    SENS.sar_meas_start1.sar1_en_pad_force = 0;
    SENS.sar_meas_wait2.force_xpd_amp = 0x2;
    SENS.sar_meas_ctrl.amp_rst_fb_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_gnd_fsm = 0;
    SENS.sar_meas_wait1.sar_amp_wait1 = 0x1;
    SENS.sar_meas_wait1.sar_amp_wait2 = 0x1;
    SENS.sar_meas_wait2.sar_amp_wait3 = 0x1;
    portEXIT_CRITICAL(&rtc_spinlock);
}

/*---------------------------------------------------------------
                    ADC2
---------------------------------------------------------------*/
esp_err_t adc2_pad_get_io_num(adc2_channel_t channel, gpio_num_t *gpio_num)
{
    RTC_MODULE_CHECK(channel < ADC2_CHANNEL_MAX, "ADC2 Channel Err", ESP_ERR_INVALID_ARG);

    switch (channel) {
    case ADC2_CHANNEL_0:
        *gpio_num = ADC2_CHANNEL_0_GPIO_NUM;
        break;
    case ADC2_CHANNEL_1:
        *gpio_num = ADC2_CHANNEL_1_GPIO_NUM;
        break;
    case ADC2_CHANNEL_2:
        *gpio_num = ADC2_CHANNEL_2_GPIO_NUM;
        break;
    case ADC2_CHANNEL_3:
        *gpio_num = ADC2_CHANNEL_3_GPIO_NUM;
        break;
    case ADC2_CHANNEL_4:
        *gpio_num = ADC2_CHANNEL_4_GPIO_NUM;
        break;
    case ADC2_CHANNEL_5:
        *gpio_num = ADC2_CHANNEL_5_GPIO_NUM;
        break;
    case ADC2_CHANNEL_6:
        *gpio_num = ADC2_CHANNEL_6_GPIO_NUM;
        break;
    case ADC2_CHANNEL_7:
        *gpio_num = ADC2_CHANNEL_7_GPIO_NUM;
        break;
    case ADC2_CHANNEL_8:
        *gpio_num = ADC2_CHANNEL_8_GPIO_NUM;
        break;
    case ADC2_CHANNEL_9:
        *gpio_num = ADC2_CHANNEL_9_GPIO_NUM;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t adc2_wifi_acquire()
{
    //lazy initialization
    //for wifi, block until acquire the lock
    _lock_acquire( &adc2_wifi_lock );
    ESP_LOGD( RTC_MODULE_TAG, "Wi-Fi takes adc2 lock." );
    return ESP_OK;
}

esp_err_t adc2_wifi_release()
{
    RTC_MODULE_CHECK((uint32_t*)adc2_wifi_lock != NULL, "wifi release called before acquire", ESP_ERR_INVALID_STATE );

    _lock_release( &adc2_wifi_lock );
    ESP_LOGD( RTC_MODULE_TAG, "Wi-Fi returns adc2 lock." );
    return ESP_OK;
}

static esp_err_t adc2_pad_init(adc2_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    ADC2_CHECK_FUNCTION_RET(adc2_pad_get_io_num(channel, &gpio_num));
    ADC2_CHECK_FUNCTION_RET(rtc_gpio_init(gpio_num));
    ADC2_CHECK_FUNCTION_RET(rtc_gpio_output_disable(gpio_num));
    ADC2_CHECK_FUNCTION_RET(rtc_gpio_input_disable(gpio_num));
    ADC2_CHECK_FUNCTION_RET(gpio_set_pull_mode(gpio_num, GPIO_FLOATING));
    return ESP_OK;
}

esp_err_t adc2_config_channel_atten(adc2_channel_t channel, adc_atten_t atten)
{
    RTC_MODULE_CHECK(channel < ADC2_CHANNEL_MAX, "ADC2 Channel Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(atten <= ADC_ATTEN_11db, "ADC2 Atten Err", ESP_ERR_INVALID_ARG);

    adc2_pad_init(channel);
    portENTER_CRITICAL( &adc2_spinlock );

    //lazy initialization
    //avoid collision with other tasks
    if ( _lock_try_acquire( &adc2_wifi_lock ) == -1 ) {
        //try the lock, return if failed (wifi using).
        portEXIT_CRITICAL( &adc2_spinlock );
        return ESP_ERR_TIMEOUT;
    }
    SENS.sar_atten2 = ( SENS.sar_atten2 & ~(3<<(channel*2)) ) | ((atten&3) << (channel*2));
    _lock_release( &adc2_wifi_lock );
    
    portEXIT_CRITICAL( &adc2_spinlock );
    return ESP_OK;
}

static inline void adc2_config_width(adc_bits_width_t width_bit)
{
    portENTER_CRITICAL(&rtc_spinlock);
    //sar_start_force shared with ADC1
    SENS.sar_start_force.sar2_bit_width = width_bit;
    portEXIT_CRITICAL(&rtc_spinlock);

    //Invert the adc value,the Output value is invert
    SENS.sar_read_ctrl2.sar2_data_inv = 1;
    //Set The adc sample width,invert adc value,must digital sar2_bit_width[1:0]=3
    SENS.sar_read_ctrl2.sar2_sample_bit = width_bit;
    //Take the control from WIFI    
    SENS.sar_read_ctrl2.sar2_pwdet_force = 0;
}

//registers in critical section with adc1:
//SENS_SAR_START_FORCE_REG, 
esp_err_t adc2_get_raw(adc2_channel_t channel, adc_bits_width_t width_bit, int* raw_out)
{
    uint16_t adc_value = 0;
    RTC_MODULE_CHECK(channel < ADC2_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);

    //in critical section with whole rtc module
    adc_power_on();

    //avoid collision with other tasks
    portENTER_CRITICAL(&adc2_spinlock); 
    //lazy initialization
    //try the lock, return if failed (wifi using).
    if ( _lock_try_acquire( &adc2_wifi_lock ) == -1 ) {
        portEXIT_CRITICAL( &adc2_spinlock );
        return ESP_ERR_TIMEOUT;
    }
    //in critical section with whole rtc module
    adc2_config_width( width_bit );
    
    //Adc Controler is Rtc module,not ulp coprocessor
    SENS.sar_meas_start2.meas2_start_force = 1; //force pad mux and force start
    //Open the ADC2 Data port Not ulp coprocessor
    SENS.sar_meas_start2.sar2_en_pad_force = 1; //open the ADC2 data port
    //Select channel
    SENS.sar_meas_start2.sar2_en_pad = 1 << channel; //pad enable
    SENS.sar_meas_start2.meas2_start_sar = 0; //start force 0
    SENS.sar_meas_start2.meas2_start_sar = 1; //start force 1
    while (SENS.sar_meas_start2.meas2_done_sar == 0) {}; //read done
    adc_value = SENS.sar_meas_start2.meas2_data_sar;
    _lock_release( &adc2_wifi_lock );
    portEXIT_CRITICAL(&adc2_spinlock);

    *raw_out = (int)adc_value;
    return ESP_OK;
}

esp_err_t adc2_vref_to_gpio(gpio_num_t gpio)
{
    int channel;
    if(gpio == GPIO_NUM_25){
        channel = 8;    //Channel 8 bit
    }else if (gpio == GPIO_NUM_26){
        channel = 9;    //Channel 9 bit
    }else if (gpio == GPIO_NUM_27){
        channel = 7;    //Channel 7 bit
    }else{
        return ESP_ERR_INVALID_ARG;
    }

    //Configure RTC gpio
    rtc_gpio_init(gpio);
    rtc_gpio_output_disable(gpio);
    rtc_gpio_input_disable(gpio);
    rtc_gpio_pullup_dis(gpio);
    rtc_gpio_pulldown_dis(gpio);

    RTCCNTL.bias_conf.dbg_atten = 0;     //Check DBG effect outside sleep mode
    //set dtest (MUX_SEL : 0 -> RTC; 1-> vdd_sar2)
    RTCCNTL.test_mux.dtest_rtc = 1;      //Config test mux to route v_ref to ADC2 Channels
    //set ent
    RTCCNTL.test_mux.ent_rtc = 1;
    //set sar2_en_test
    SENS.sar_start_force.sar2_en_test = 1;
    //force fsm
    SENS.sar_meas_wait2.force_xpd_sar = ADC_FORCE_ENABLE;    //Select power source of ADC
    //set sar2 en force
    SENS.sar_meas_start2.sar2_en_pad_force = 1;      //Pad bitmap controlled by SW
    //set en_pad for channels 7,8,9 (bits 0x380)
    SENS.sar_meas_start2.sar2_en_pad = 1<<channel;

    return ESP_OK;
}

/*---------------------------------------------------------------
                    DAC
---------------------------------------------------------------*/
esp_err_t dac_pad_get_io_num(dac_channel_t channel, gpio_num_t *gpio_num)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(gpio_num, "Param null", ESP_ERR_INVALID_ARG);

    switch (channel) {
    case DAC_CHANNEL_1:
        *gpio_num = DAC_CHANNEL_1_GPIO_NUM;
        break;
    case DAC_CHANNEL_2:
        *gpio_num = DAC_CHANNEL_2_GPIO_NUM;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t dac_rtc_pad_init(dac_channel_t channel)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    gpio_num_t gpio_num = 0;
    dac_pad_get_io_num(channel, &gpio_num);
    rtc_gpio_init(gpio_num);
    rtc_gpio_output_disable(gpio_num);
    rtc_gpio_input_disable(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
    rtc_gpio_pulldown_dis(gpio_num);

    return ESP_OK;
}

esp_err_t dac_output_enable(dac_channel_t channel)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    dac_rtc_pad_init(channel);
    portENTER_CRITICAL(&rtc_spinlock);
    if (channel == DAC_CHANNEL_1) {
        SET_PERI_REG_MASK(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_XPD_DAC | RTC_IO_PDAC1_DAC_XPD_FORCE);
    } else if (channel == DAC_CHANNEL_2) {
        SET_PERI_REG_MASK(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_XPD_DAC | RTC_IO_PDAC2_DAC_XPD_FORCE);
    }
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_output_disable(dac_channel_t channel)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    if (channel == DAC_CHANNEL_1) {
        CLEAR_PERI_REG_MASK(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_XPD_DAC | RTC_IO_PDAC1_DAC_XPD_FORCE);
    } else if (channel == DAC_CHANNEL_2) {
        CLEAR_PERI_REG_MASK(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_XPD_DAC | RTC_IO_PDAC2_DAC_XPD_FORCE);
    }
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_output_voltage(dac_channel_t channel, uint8_t dac_value)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    //Disable Tone
    CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_SW_TONE_EN);

    //Disable Channel Tone
    if (channel == DAC_CHANNEL_1) {
        CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN1_M);
    } else if (channel == DAC_CHANNEL_2) {
        CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN2_M);
    }

    //Set the Dac value
    if (channel == DAC_CHANNEL_1) {
        SET_PERI_REG_BITS(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_DAC, dac_value, RTC_IO_PDAC1_DAC_S);   //dac_output
    } else if (channel == DAC_CHANNEL_2) {
        SET_PERI_REG_BITS(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_DAC, dac_value, RTC_IO_PDAC2_DAC_S);   //dac_output
    }

    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t dac_out_voltage(dac_channel_t channel, uint8_t dac_value)
{
    RTC_MODULE_CHECK((channel >= DAC_CHANNEL_1) && (channel < DAC_CHANNEL_MAX), DAC_ERR_STR_CHANNEL_ERROR, ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    //Disable Tone
    CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_SW_TONE_EN);

    //Disable Channel Tone
    if (channel == DAC_CHANNEL_1) {
        CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN1_M);
    } else if (channel == DAC_CHANNEL_2) {
        CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN2_M);
    }

    //Set the Dac value
    if (channel == DAC_CHANNEL_1) {
        SET_PERI_REG_BITS(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_DAC, dac_value, RTC_IO_PDAC1_DAC_S);   //dac_output
    } else if (channel == DAC_CHANNEL_2) {
        SET_PERI_REG_BITS(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_DAC, dac_value, RTC_IO_PDAC2_DAC_S);   //dac_output
    }

    portEXIT_CRITICAL(&rtc_spinlock);
    //dac pad init
    dac_rtc_pad_init(channel);
    dac_output_enable(channel);

    return ESP_OK;
}

esp_err_t dac_i2s_enable()
{
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_DAC_DIG_FORCE_M | SENS_DAC_CLK_INV_M);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

esp_err_t dac_i2s_disable()
{
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_DAC_DIG_FORCE_M | SENS_DAC_CLK_INV_M);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ESP_OK;
}

/*---------------------------------------------------------------
                        HALL SENSOR
---------------------------------------------------------------*/
static int hall_sensor_get_value()    //hall sensor without LNA
{
    int Sens_Vp0;
    int Sens_Vn0;
    int Sens_Vp1;
    int Sens_Vn1;
    int hall_value;
    
    adc_power_on();

    portENTER_CRITICAL(&rtc_spinlock);
    SENS.sar_touch_ctrl1.xpd_hall_force = 1;     // hall sens force enable
    RTCIO.hall_sens.xpd_hall = 1;      // xpd hall
    SENS.sar_touch_ctrl1.hall_phase_force = 1;   // phase force

    RTCIO.hall_sens.hall_phase = 0;      // hall phase
    Sens_Vp0 = adc1_get_raw(ADC1_CHANNEL_0);
    Sens_Vn0 = adc1_get_raw(ADC1_CHANNEL_3);
    RTCIO.hall_sens.hall_phase = 1;
    Sens_Vp1 = adc1_get_raw(ADC1_CHANNEL_0);
    Sens_Vn1 = adc1_get_raw(ADC1_CHANNEL_3);

    SENS.sar_touch_ctrl1.xpd_hall_force = 0;
    SENS.sar_touch_ctrl1.hall_phase_force = 0;
    portEXIT_CRITICAL(&rtc_spinlock);
    hall_value = (Sens_Vp1 - Sens_Vp0) - (Sens_Vn1 - Sens_Vn0);

    return hall_value;
}

int hall_sensor_read()
{
    adc_gpio_init(ADC_UNIT_1, ADC1_CHANNEL_0);
    adc_gpio_init(ADC_UNIT_1, ADC1_CHANNEL_3);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);
    return hall_sensor_get_value();
}

/*---------------------------------------------------------------
                        INTERRUPT HANDLER
---------------------------------------------------------------*/


typedef struct rtc_isr_handler_ {
    uint32_t mask;
    intr_handler_t handler;
    void* handler_arg;
    SLIST_ENTRY(rtc_isr_handler_) next;
} rtc_isr_handler_t;

static SLIST_HEAD(rtc_isr_handler_list_, rtc_isr_handler_) s_rtc_isr_handler_list =
        SLIST_HEAD_INITIALIZER(s_rtc_isr_handler_list);
portMUX_TYPE s_rtc_isr_handler_list_lock = portMUX_INITIALIZER_UNLOCKED;
static intr_handle_t s_rtc_isr_handle;

static void rtc_isr(void* arg)
{
    uint32_t status = REG_READ(RTC_CNTL_INT_ST_REG);
    rtc_isr_handler_t* it;
    portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
    SLIST_FOREACH(it, &s_rtc_isr_handler_list, next) {
        if (it->mask & status) {
            portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
            (*it->handler)(it->handler_arg);
            portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
        }
    }
    portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, status);
}

static esp_err_t rtc_isr_ensure_installed()
{
    esp_err_t err = ESP_OK;
    portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
    if (s_rtc_isr_handle) {
        goto out;
    }

    REG_WRITE(RTC_CNTL_INT_ENA_REG, 0);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, UINT32_MAX);
    err = esp_intr_alloc(ETS_RTC_CORE_INTR_SOURCE, 0, &rtc_isr, NULL, &s_rtc_isr_handle);
    if (err != ESP_OK) {
        goto out;
    }

out:
    portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
    return err;
}


esp_err_t rtc_isr_register(intr_handler_t handler, void* handler_arg, uint32_t rtc_intr_mask)
{
    esp_err_t err = rtc_isr_ensure_installed();
    if (err != ESP_OK) {
        return err;
    }

    rtc_isr_handler_t* item = malloc(sizeof(*item));
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    item->handler = handler;
    item->handler_arg = handler_arg;
    item->mask = rtc_intr_mask;
    portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
    SLIST_INSERT_HEAD(&s_rtc_isr_handler_list, item, next);
    portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
    return ESP_OK;
}

esp_err_t rtc_isr_deregister(intr_handler_t handler, void* handler_arg)
{
    rtc_isr_handler_t* it;
    rtc_isr_handler_t* prev = NULL;
    bool found = false;
    portENTER_CRITICAL(&s_rtc_isr_handler_list_lock);
    SLIST_FOREACH(it, &s_rtc_isr_handler_list, next) {
        if (it->handler == handler && it->handler_arg == handler_arg) {
            if (it == SLIST_FIRST(&s_rtc_isr_handler_list)) {
                SLIST_REMOVE_HEAD(&s_rtc_isr_handler_list, next);
            } else {
                SLIST_REMOVE_AFTER(prev, next);
            }
            found = true;
            break;
        }
        prev = it;
    }
    portEXIT_CRITICAL(&s_rtc_isr_handler_list_lock);
    return found ? ESP_OK : ESP_ERR_INVALID_STATE;
}
