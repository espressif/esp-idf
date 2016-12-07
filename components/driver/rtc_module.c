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
#include "soc/sens_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "rtc_io.h"
#include "touch_pad.h"
#include "adc.h"
#include "dac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"

static const char *RTC_MODULE_TAG = "RTC_MODULE";

#define RTC_MODULE_CHECK(a, str, ret_val) if (!(a)) {                                \
    ESP_LOGE(RTC_MODULE_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str); \
    return (ret_val);                                                              \
}

#define ADC1_CHECK_FUNCTION_RET(fun_ret) if(fun_ret!=ESP_OK){\
    ESP_LOGE(RTC_MODULE_TAG,"%s:%d\n",__FUNCTION__,__LINE__);\
    return ESP_FAIL;\
}

portMUX_TYPE rtc_spinlock = portMUX_INITIALIZER_UNLOCKED;

//Reg,Mux,Fun,IE,Up,Down,Rtc_number
const rtc_gpio_desc_t rtc_gpio_desc[GPIO_PIN_COUNT] = {
    {RTC_IO_TOUCH_PAD1_REG, RTC_IO_TOUCH_PAD1_MUX_SEL_M, RTC_IO_TOUCH_PAD1_FUN_SEL_S, RTC_IO_TOUCH_PAD1_FUN_IE_M, RTC_IO_TOUCH_PAD1_RUE_M, RTC_IO_TOUCH_PAD1_RDE_M, 11}, //0
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //1
    {RTC_IO_TOUCH_PAD2_REG, RTC_IO_TOUCH_PAD2_MUX_SEL_M, RTC_IO_TOUCH_PAD2_FUN_SEL_S, RTC_IO_TOUCH_PAD2_FUN_IE_M, RTC_IO_TOUCH_PAD2_RUE_M, RTC_IO_TOUCH_PAD2_RDE_M, 12}, //2
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //3
    {RTC_IO_TOUCH_PAD0_REG, RTC_IO_TOUCH_PAD0_MUX_SEL_M, RTC_IO_TOUCH_PAD0_FUN_SEL_S, RTC_IO_TOUCH_PAD0_FUN_IE_M, RTC_IO_TOUCH_PAD0_RUE_M, RTC_IO_TOUCH_PAD0_RDE_M, 10}, //4
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //5
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //6
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //7
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //8
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //9
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //10
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //11
    {RTC_IO_TOUCH_PAD5_REG, RTC_IO_TOUCH_PAD5_MUX_SEL_M, RTC_IO_TOUCH_PAD5_FUN_SEL_S, RTC_IO_TOUCH_PAD5_FUN_IE_M, RTC_IO_TOUCH_PAD5_RUE_M, RTC_IO_TOUCH_PAD5_RDE_M, 15}, //12
    {RTC_IO_TOUCH_PAD4_REG, RTC_IO_TOUCH_PAD4_MUX_SEL_M, RTC_IO_TOUCH_PAD4_FUN_SEL_S, RTC_IO_TOUCH_PAD4_FUN_IE_M, RTC_IO_TOUCH_PAD4_RUE_M, RTC_IO_TOUCH_PAD4_RDE_M, 14}, //13
    {RTC_IO_TOUCH_PAD6_REG, RTC_IO_TOUCH_PAD6_MUX_SEL_M, RTC_IO_TOUCH_PAD6_FUN_SEL_S, RTC_IO_TOUCH_PAD6_FUN_IE_M, RTC_IO_TOUCH_PAD6_RUE_M, RTC_IO_TOUCH_PAD6_RDE_M, 16}, //14
    {RTC_IO_TOUCH_PAD3_REG, RTC_IO_TOUCH_PAD3_MUX_SEL_M, RTC_IO_TOUCH_PAD3_FUN_SEL_S, RTC_IO_TOUCH_PAD3_FUN_IE_M, RTC_IO_TOUCH_PAD3_RUE_M, RTC_IO_TOUCH_PAD3_RDE_M, 13}, //15
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //16
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //17
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //18
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //19
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //20
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //21
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //22
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //23
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //24
    {RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_MUX_SEL_M, RTC_IO_PDAC1_FUN_SEL_S, RTC_IO_PDAC1_FUN_IE_M, RTC_IO_PDAC1_RUE_M, RTC_IO_PDAC1_RDE_M, 6},                           //25
    {RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_MUX_SEL_M, RTC_IO_PDAC2_FUN_SEL_S, RTC_IO_PDAC2_FUN_IE_M, RTC_IO_PDAC2_RUE_M, RTC_IO_PDAC2_RDE_M, 7},                           //26
    {RTC_IO_TOUCH_PAD7_REG, RTC_IO_TOUCH_PAD7_MUX_SEL_M, RTC_IO_TOUCH_PAD7_FUN_SEL_S, RTC_IO_TOUCH_PAD7_FUN_IE_M, RTC_IO_TOUCH_PAD7_RUE_M, RTC_IO_TOUCH_PAD7_RDE_M, 17}, //27
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //28
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //29
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //30
    {0, 0, 0, 0, 0, 0, -1},                                                                                                                                            //31
    {RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32P_MUX_SEL_M, RTC_IO_X32P_FUN_SEL_S, RTC_IO_X32P_FUN_IE_M, RTC_IO_X32P_RUE_M, RTC_IO_X32P_RDE_M, 9},                            //32
    {RTC_IO_XTAL_32K_PAD_REG, RTC_IO_X32N_MUX_SEL_M, RTC_IO_X32N_FUN_SEL_S, RTC_IO_X32N_FUN_IE_M, RTC_IO_X32N_RUE_M, RTC_IO_X32N_RDE_M, 8},                            //33
    {RTC_IO_ADC_PAD_REG, RTC_IO_ADC1_MUX_SEL_M, RTC_IO_ADC1_FUN_SEL_S, RTC_IO_ADC1_FUN_IE_M, 0, 0, 4},                                                                //34
    {RTC_IO_ADC_PAD_REG, RTC_IO_ADC2_MUX_SEL_M, RTC_IO_ADC2_FUN_SEL_S, RTC_IO_ADC2_FUN_IE_M, 0, 0, 5},                                                                //35
    {RTC_IO_SENSOR_PADS_REG, RTC_IO_SENSE1_MUX_SEL_M, RTC_IO_SENSE1_FUN_SEL_S, RTC_IO_SENSE1_FUN_IE_M, 0, 0, 0},                                                      //36
    {RTC_IO_SENSOR_PADS_REG, RTC_IO_SENSE2_MUX_SEL_M, RTC_IO_SENSE2_FUN_SEL_S, RTC_IO_SENSE2_FUN_IE_M, 0, 0, 1},                                                      //37
    {RTC_IO_SENSOR_PADS_REG, RTC_IO_SENSE3_MUX_SEL_M, RTC_IO_SENSE3_FUN_SEL_S, RTC_IO_SENSE3_FUN_IE_M, 0, 0, 2},                                                       //38
    {RTC_IO_SENSOR_PADS_REG, RTC_IO_SENSE4_MUX_SEL_M, RTC_IO_SENSE4_FUN_SEL_S, RTC_IO_SENSE4_FUN_IE_M, 0, 0, 3},                                                      //39
};

/*---------------------------------------------------------------
                        RTC IO
---------------------------------------------------------------*/
esp_err_t rtc_gpio_init(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(RTC_GPIO_IS_VALID_GPIO(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
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
    RTC_MODULE_CHECK(RTC_GPIO_IS_VALID_GPIO(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
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
    RTC_MODULE_CHECK(RTC_GPIO_IS_VALID_GPIO(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].ie);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

static esp_err_t rtc_gpio_input_disable(gpio_num_t gpio_num)
{
    RTC_MODULE_CHECK(RTC_GPIO_IS_VALID_GPIO(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].ie);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t rtc_gpio_set_level(gpio_num_t gpio_num, uint32_t level)
{
    int rtc_gpio_num = rtc_gpio_num = rtc_gpio_desc[gpio_num].rtc_num;;
    RTC_MODULE_CHECK(RTC_GPIO_IS_VALID_GPIO(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);

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
    RTC_MODULE_CHECK(RTC_GPIO_IS_VALID_GPIO(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);

    portENTER_CRITICAL(&rtc_spinlock);
    level = READ_PERI_REG(RTC_GPIO_IN_REG);
    portEXIT_CRITICAL(&rtc_spinlock);
    return ((level >> (RTC_GPIO_IN_NEXT_S + rtc_gpio_num)) & 0x01);
}

esp_err_t rtc_gpio_set_direction(gpio_num_t gpio_num, rtc_gpio_mode_t mode)
{
    RTC_MODULE_CHECK(RTC_GPIO_IS_VALID_GPIO(gpio_num), "RTC_GPIO number error", ESP_ERR_INVALID_ARG);

    switch (mode) {
    case RTC_GPIO_MODE_INPUT_ONLY:
        rtc_gpio_output_disable(gpio_num);
        rtc_gpio_input_enable(gpio_num);
        break;
    case RTC_GPIO_MODE_OUTPUT_ONLY:
        rtc_gpio_output_enable(gpio_num);
        rtc_gpio_input_disable(gpio_num);
        break;
    case RTC_GPIO_MODE_INPUT_OUTUT:
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
        return ESP_FAIL;
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
        return ESP_FAIL;
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
        return ESP_FAIL;
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
        return ESP_FAIL;
    }

    //this is a rtc pad
    portENTER_CRITICAL(&rtc_spinlock);
    CLEAR_PERI_REG_MASK(rtc_gpio_desc[gpio_num].reg, rtc_gpio_desc[gpio_num].pulldown);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

/*---------------------------------------------------------------
                    Touch Pad
---------------------------------------------------------------*/
esp_err_t touch_pad_isr_handler_register(uint32_t touch_intr_num, void(*fn)(void *), void *arg)
{
    RTC_MODULE_CHECK(fn, "Touch_Pad ISR null", ESP_ERR_INVALID_ARG);
    ESP_INTR_DISABLE(touch_intr_num);
    intr_matrix_set(xPortGetCoreID(), ETS_RTC_CORE_INTR_SOURCE, touch_intr_num);
    xt_set_interrupt_handler(touch_intr_num, fn, arg);
    ESP_INTR_ENABLE(touch_intr_num);

    return ESP_OK;
}

static esp_err_t touch_pad_get_io_num(touch_pad_t touch_num, gpio_num_t *gpio_num)
{
    switch (touch_num) {
    case TOUCH_PAD_NUM0:
        *gpio_num = 4;
        break;
    case TOUCH_PAD_NUM1:
        *gpio_num = 0;
        break;
    case TOUCH_PAD_NUM2:
        *gpio_num = 2;
        break;
    case TOUCH_PAD_NUM3:
        *gpio_num = 15;
        break;
    case TOUCH_PAD_NUM4:
        *gpio_num = 13;
        break;
    case TOUCH_PAD_NUM5:
        *gpio_num = 12;
        break;
    case TOUCH_PAD_NUM6:
        *gpio_num = 14;
        break;
    case TOUCH_PAD_NUM7:
        *gpio_num = 27;
        break;
    case TOUCH_PAD_NUM8:
        *gpio_num = 33;
        break;
    case TOUCH_PAD_NUM9:
        *gpio_num = 32;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t touch_pad_init_config(uint16_t sleep_cycle, uint16_t sample_cycle_num)
{
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_BITS(RTC_IO_TOUCH_CFG_REG, RTC_IO_TOUCH_XPD_BIAS, 1, RTC_IO_TOUCH_XPD_BIAS_S);
    SET_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_MEAS_EN_CLR);
    //clear touch enable
    WRITE_PERI_REG(SENS_SAR_TOUCH_ENABLE_REG, 0x0);
    //enable Rtc Touch pad Timer
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_TOUCH_SLP_TIMER_EN);
    //config pad module sleep time and sample num
    //Touch pad SleepCycle Time = 150Khz
    SET_PERI_REG_BITS(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_SLEEP_CYCLES, sleep_cycle, SENS_TOUCH_SLEEP_CYCLES_S);//150kHZ
    //Touch Pad Measure Time= 8Mhz
    SET_PERI_REG_BITS(SENS_SAR_TOUCH_CTRL1_REG, SENS_TOUCH_MEAS_DELAY, sample_cycle_num, SENS_TOUCH_MEAS_DELAY_S); //8Mhz
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

void touch_pad_init()
{
    touch_pad_init_config(TOUCH_PAD_SLEEP_CYCLE_CONFIG, TOUCH_PAD_MEASURE_CYCLE_CONFIG);
}

static void touch_pad_counter_init(touch_pad_t touch_num)
{
    portENTER_CRITICAL(&rtc_spinlock);
    //Enable Tie,Init Level(Counter)
    SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + touch_num * 4, RTC_IO_TOUCH_PAD0_TIE_OPT_M);
    //Touch Set Slop(Counter)
    SET_PERI_REG_BITS(RTC_IO_TOUCH_PAD0_REG + touch_num * 4, RTC_IO_TOUCH_PAD0_DAC_V, 7, RTC_IO_TOUCH_PAD0_DAC_S);
    //Enable Touch Pad IO
    SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + touch_num * 4, RTC_IO_TOUCH_PAD0_START_M);
    portEXIT_CRITICAL(&rtc_spinlock);
}

static void touch_pad_power_on(touch_pad_t touch_num)
{
    portENTER_CRITICAL(&rtc_spinlock);
    //Enable Touch Pad Power on
    SET_PERI_REG_MASK(RTC_IO_TOUCH_PAD0_REG + touch_num * 4, RTC_IO_TOUCH_PAD0_XPD_M);
    portEXIT_CRITICAL(&rtc_spinlock);
}

static void toch_pad_io_init(touch_pad_t touch_num)
{
    gpio_num_t gpio_num = GPIO_NUM_0;
    touch_pad_get_io_num(touch_num, &gpio_num);
    rtc_gpio_init(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_pulldown_dis(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
}

static esp_err_t touch_start(touch_pad_t touch_num)
{
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);

    //Enable Digital rtc control :work mode and out mode
    SET_PERI_REG_MASK(SENS_SAR_TOUCH_ENABLE_REG, (1 << (touch_num + SENS_TOUCH_PAD_WORKEN_S)) | \
                      (1 << (touch_num + SENS_TOUCH_PAD_OUTEN2_S)) | \
                      (1 << (touch_num + SENS_TOUCH_PAD_OUTEN1_S)));
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

static esp_err_t touch_stop(touch_pad_t touch_num)
{
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);

    //Disable Digital rtc control :work mode and out mode
    CLEAR_PERI_REG_MASK(SENS_SAR_TOUCH_ENABLE_REG, (1 << (touch_num + SENS_TOUCH_PAD_WORKEN_S)) | \
                        (1 << (touch_num + SENS_TOUCH_PAD_OUTEN2_S)) | \
                        (1 << (touch_num + SENS_TOUCH_PAD_OUTEN1_S)));

    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t touch_pad_config(touch_pad_t touch_num, uint16_t threshold)
{
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    //clear touch force ,select the Touch mode is Timer
    CLEAR_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_START_EN_M);
    CLEAR_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_START_FORCE_M);
    //set threshold
    uint8_t shift;
    shift = (touch_num & 1) ? SENS_TOUCH_OUT_TH1_S : SENS_TOUCH_OUT_TH0_S;
    SET_PERI_REG_BITS((SENS_SAR_TOUCH_THRES1_REG + (touch_num / 2) * 4), SENS_TOUCH_OUT_TH0, threshold, shift);
    //When touch value < threshold ,the Intr will give
    CLEAR_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL1_REG, SENS_TOUCH_OUT_SEL);
    //Intr will give ,when SET0 < threshold
    SET_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL1_REG, SENS_TOUCH_OUT_1EN);
    //Enable Rtc Touch Module Intr,the Interrupt need Rtc out  Enable
    SET_PERI_REG_MASK(RTC_CNTL_INT_ENA_REG, RTC_CNTL_TOUCH_INT_ENA);
    portEXIT_CRITICAL(&rtc_spinlock);
    touch_pad_power_on(touch_num);
    toch_pad_io_init(touch_num);
    touch_pad_counter_init(touch_num);
    touch_start(touch_num);

    return ESP_OK;
}

esp_err_t touch_pad_read(touch_pad_t touch_num, uint16_t *touch_value)
{
    RTC_MODULE_CHECK(touch_num < TOUCH_PAD_MAX, "Touch_Pad Num Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(touch_value!=NULL, "touch_value", ESP_ERR_INVALID_ARG);
    uint32_t v0 = READ_PERI_REG(SENS_SAR_TOUCH_ENABLE_REG);
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(SENS_SAR_TOUCH_ENABLE_REG, (1 << (touch_num)));
    //Disable Intr
    CLEAR_PERI_REG_MASK(SENS_SAR_TOUCH_ENABLE_REG, (1 << (touch_num + SENS_TOUCH_PAD_OUTEN2_S)) | \
                        ((1 << (touch_num + SENS_TOUCH_PAD_OUTEN1_S))));
    toch_pad_io_init(touch_num);
    touch_pad_counter_init(touch_num);
    touch_pad_power_on(touch_num);
    //force oneTime test start
    SET_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_START_EN_M);
    SET_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_START_FORCE_M);
    SET_PERI_REG_BITS(SENS_SAR_TOUCH_CTRL1_REG, SENS_TOUCH_XPD_WAIT, 10, SENS_TOUCH_XPD_WAIT_S);
    while (GET_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_MEAS_DONE) == 0) {};
    uint8_t shift = (touch_num & 1) ? SENS_TOUCH_MEAS_OUT1_S : SENS_TOUCH_MEAS_OUT0_S;
    *touch_value = READ_PERI_REG(SENS_SAR_TOUCH_OUT1_REG + (touch_num / 2) * 4) >> shift;
    WRITE_PERI_REG(SENS_SAR_TOUCH_ENABLE_REG, v0);
    //force oneTime test end
    //clear touch force ,select the Touch mode is Timer
    CLEAR_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_START_EN_M);
    CLEAR_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL2_REG, SENS_TOUCH_START_FORCE_M);
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

/*---------------------------------------------------------------
                    ADC
---------------------------------------------------------------*/
static esp_err_t adc1_pad_get_io_num(adc1_channel_t channel, gpio_num_t *gpio_num)
{
    RTC_MODULE_CHECK(channel < ADC1_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);

    switch (channel) {
    case ADC1_CHANNEL_0:
        *gpio_num = 36;
        break;
    case ADC1_CHANNEL_1:
        *gpio_num = 37;
        break;
    case ADC1_CHANNEL_2:
        *gpio_num = 38;
        break;
    case ADC1_CHANNEL_3:
        *gpio_num = 39;
        break;
    case ADC1_CHANNEL_4:
        *gpio_num = 32;
        break;
    case ADC1_CHANNEL_5:
        *gpio_num = 33;
        break;
    case ADC1_CHANNEL_6:
        *gpio_num = 34;
        break;
    case ADC1_CHANNEL_7:
        *gpio_num = 35;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t adc1_pad_init(adc1_channel_t channel)
{
    gpio_num_t gpio_num = 0;
    ADC1_CHECK_FUNCTION_RET(adc1_pad_get_io_num(channel, &gpio_num));
    ADC1_CHECK_FUNCTION_RET(rtc_gpio_init(gpio_num));
    ADC1_CHECK_FUNCTION_RET(rtc_gpio_output_disable(gpio_num));
    ADC1_CHECK_FUNCTION_RET(rtc_gpio_input_disable(gpio_num));
    ADC1_CHECK_FUNCTION_RET(gpio_set_pull_mode(gpio_num, GPIO_FLOATING));

    return ESP_OK;
}

esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)
{
    RTC_MODULE_CHECK(channel < ADC1_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);
    RTC_MODULE_CHECK(atten <= ADC_ATTEN_11db, "ADC Atten Err", ESP_ERR_INVALID_ARG);
    adc1_pad_init(channel);
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_BITS(SENS_SAR_ATTEN1_REG, 3, atten, (channel * 2)); //SAR1_atten
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

esp_err_t adc1_config_width(adc_bits_width_t width_bit)
{
    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_BITS(SENS_SAR_START_FORCE_REG, SENS_SAR1_BIT_WIDTH_V, width_bit, SENS_SAR1_BIT_WIDTH_S);  //SAR2_BIT_WIDTH[1:0]=0x3, SAR1_BIT_WIDTH[1:0]=0x3
    //Invert the adc value,the Output value is invert
    SET_PERI_REG_MASK(SENS_SAR_READ_CTRL_REG, SENS_SAR1_DATA_INV);
    //Set The adc sample width,invert adc value,must
    SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_SAMPLE_BIT_V, width_bit, SENS_SAR1_SAMPLE_BIT_S); //digital sar1_bit_width[1:0]=3
    portEXIT_CRITICAL(&rtc_spinlock);

    return ESP_OK;
}

int adc1_get_voltage(adc1_channel_t channel)
{
    uint16_t adc_value;
    uint8_t atten = 0;

    RTC_MODULE_CHECK(channel < ADC1_CHANNEL_MAX, "ADC Channel Err", ESP_ERR_INVALID_ARG);
    portENTER_CRITICAL(&rtc_spinlock);
    //Adc Controler is Rtc module,not ulp coprocessor
    SET_PERI_REG_BITS(SENS_SAR_MEAS_START1_REG, 1, 1, SENS_MEAS1_START_FORCE_S); //force pad mux and force start
    //Bit1=0:Fsm  Bit1=1(Bit0=0:PownDown Bit10=1:Powerup)
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S); //force XPD_SAR=0, use XPD_FSM
    //Disable Amp Bit1=0:Fsm  Bit1=1(Bit0=0:PownDown Bit10=1:Powerup)
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_AMP, 0x2, SENS_FORCE_XPD_AMP_S); //force XPD_AMP=0
    //Open the ADC1 Data port Not ulp coprocessor
    SET_PERI_REG_BITS(SENS_SAR_MEAS_START1_REG, 1, 1, SENS_SAR1_EN_PAD_FORCE_S); //open the ADC1 data port
    //Select channel
    SET_PERI_REG_BITS(SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD, (1 << channel), SENS_SAR1_EN_PAD_S); //pad enable
    SET_PERI_REG_BITS(SENS_SAR_MEAS_CTRL_REG, 0xfff, 0x0, SENS_AMP_RST_FB_FSM_S);  //[11:8]:short ref ground, [7:4]:short ref, [3:0]:rst fb
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT1_REG, SENS_SAR_AMP_WAIT1, 0x1, SENS_SAR_AMP_WAIT1_S);
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT1_REG, SENS_SAR_AMP_WAIT2, 0x1, SENS_SAR_AMP_WAIT2_S);
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_SAR_AMP_WAIT3, 0x1, SENS_SAR_AMP_WAIT3_S);
    while (GET_PERI_REG_BITS2(SENS_SAR_SLAVE_ADDR1_REG, 0x7, SENS_MEAS_STATUS_S) != 0); //wait det_fsm==0
    SET_PERI_REG_BITS(SENS_SAR_MEAS_START1_REG, 1, 0, SENS_MEAS1_START_SAR_S); //start force 0
    SET_PERI_REG_BITS(SENS_SAR_MEAS_START1_REG, 1, 1, SENS_MEAS1_START_SAR_S); //start force 1
    while (GET_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DONE_SAR) == 0) {}; //read done
    adc_value = GET_PERI_REG_BITS2(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DATA_SAR, SENS_MEAS1_DATA_SAR_S);
    portEXIT_CRITICAL(&rtc_spinlock);

    return adc_value;
}

/*---------------------------------------------------------------
                    DAC
---------------------------------------------------------------*/
static esp_err_t dac_pad_get_io_num(dac_channel_t channel, gpio_num_t *gpio_num)
{
    RTC_MODULE_CHECK(channel < DAC_CHANNEL_MAX, "DAC Channel Err", ESP_ERR_INVALID_ARG);

    switch (channel) {
    case DAC_CHANNEL_1:
        *gpio_num = 25;
        break;
    case DAC_CHANNEL_2:
        *gpio_num = 26;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t dac_rtc_pad_init(dac_channel_t channel)
{
    RTC_MODULE_CHECK(channel < DAC_CHANNEL_MAX, "DAC Channel Err", ESP_ERR_INVALID_ARG);
    gpio_num_t gpio_num = 0;
    dac_pad_get_io_num(channel, &gpio_num);
    rtc_gpio_init(gpio_num);
    rtc_gpio_output_disable(gpio_num);
    rtc_gpio_input_disable(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
    rtc_gpio_pulldown_dis(gpio_num);

    return ESP_OK;
}

static esp_err_t dac_out_enable(dac_channel_t channel)
{
    if (channel == DAC_CHANNEL_1) {
        portENTER_CRITICAL(&rtc_spinlock);
        SET_PERI_REG_MASK(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_XPD_DAC | RTC_IO_PDAC1_DAC_XPD_FORCE);
        portEXIT_CRITICAL(&rtc_spinlock);
    } else if (channel == DAC_CHANNEL_2) {
        portENTER_CRITICAL(&rtc_spinlock);
        SET_PERI_REG_MASK(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_XPD_DAC | RTC_IO_PDAC2_DAC_XPD_FORCE);
        portEXIT_CRITICAL(&rtc_spinlock);
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

static esp_err_t  dac_out_disable(dac_channel_t channel)
{
    if (channel == DAC_CHANNEL_1) {
        portENTER_CRITICAL(&rtc_spinlock);
        CLEAR_PERI_REG_MASK(RTC_IO_PAD_DAC1_REG, RTC_IO_PDAC1_XPD_DAC | RTC_IO_PDAC1_DAC_XPD_FORCE);
        portEXIT_CRITICAL(&rtc_spinlock);
    } else if (channel == DAC_CHANNEL_2) {
        portENTER_CRITICAL(&rtc_spinlock);
        CLEAR_PERI_REG_MASK(RTC_IO_PAD_DAC2_REG, RTC_IO_PDAC2_XPD_DAC | RTC_IO_PDAC2_DAC_XPD_FORCE);
        portEXIT_CRITICAL(&rtc_spinlock);
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t dac_out_voltage(dac_channel_t channel, uint8_t dac_value)
{
    RTC_MODULE_CHECK(channel < DAC_CHANNEL_MAX, "DAC Channel Err", ESP_ERR_INVALID_ARG);
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
    dac_out_enable(channel);

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

    portENTER_CRITICAL(&rtc_spinlock);
    SET_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL1_REG, SENS_XPD_HALL_FORCE_M);     // hall sens force enable
    SET_PERI_REG_MASK(RTC_IO_HALL_SENS_REG, RTC_IO_XPD_HALL);      // xpd hall
    SET_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL1_REG, SENS_HALL_PHASE_FORCE_M);   // phase force
    CLEAR_PERI_REG_MASK(RTC_IO_HALL_SENS_REG, RTC_IO_HALL_PHASE);      // hall phase
    Sens_Vp0 = adc1_get_voltage(ADC1_CHANNEL_0);
    Sens_Vn0 = adc1_get_voltage(ADC1_CHANNEL_3);
    SET_PERI_REG_MASK(RTC_IO_HALL_SENS_REG, RTC_IO_HALL_PHASE);
    Sens_Vp1 = adc1_get_voltage(ADC1_CHANNEL_0);
    Sens_Vn1 = adc1_get_voltage(ADC1_CHANNEL_3);
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S);
    CLEAR_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL1_REG, SENS_XPD_HALL_FORCE);
    CLEAR_PERI_REG_MASK(SENS_SAR_TOUCH_CTRL1_REG, SENS_HALL_PHASE_FORCE);
    portEXIT_CRITICAL(&rtc_spinlock);
    hall_value = (Sens_Vp1 - Sens_Vp0) - (Sens_Vn1 - Sens_Vn0);

    return hall_value;
}

int hall_sensor_read()
{
    adc1_pad_init(ADC1_CHANNEL_0);
    adc1_pad_init(ADC1_CHANNEL_3);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_0db);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_0db);
    return hall_sensor_get_value();
}
