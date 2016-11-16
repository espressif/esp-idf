// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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


#ifndef _DRIVER_I2S_H_
#define _DRIVER_I2S_H_
#include "soc/dport_reg.h"
#include "soc/i2s_reg.h"
#include <stdint.h>
#include "dma.h"
#include "soc/soc.h"
#include "esp_err.h"

#define ETS_I2S0_INUM     17
#define ETS_I2S1_INUM     17

#define ESP_I2S1_INTR_ATTACH(func, arg) \
    xt_set_interrupt_handler(ETS_I2S1_INUM, (func), (void *)(arg))


//*****************************************************************************
//
// Make sure all of the definitions in this header have a C binding.
//
//*****************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/** @addtogroup Driver_APIs
 * @{
 */

/** \defgroup I2S_Driver_APIs I2S Driver APIs
 * @brief I2S driver APIs
 */

/** @addtogroup I2S_Driver_APIs
 * @{
 */

// ena:1 enable audio pll;
//     0 disable audio pll;
// freq = (xtal * (sdm2 + 4))/((o_div+2)*2)
// xtal is 40M/26M
// (xtal * (sdm2 + 4)):240 ~ 600MHz
// o_div              :  0 ~ 31
void rtc_plla_ena(uint8_t ena, uint32_t sdm2, uint32_t o_div);


/**
 * @brief Two I2S module.I2S0 has support pdm.
 *
 */
typedef enum {
    I2S_NUM_I2S0  = 0x0,
    I2S_NUM_I2S1  = 0x1,
    I2S_NUM_MAX   = I2S_NUM_I2S1,
} i2s_num_t;

/**
 * @brief I2S module working mode.
 *
 */
typedef enum {
    I2S_MODE_MASTER_TX   = 1,
    I2S_MODE_MASTER_RX   = 2,
    I2S_MODE_SLAVE_TX    = 3,
    I2S_MODE_SLAVE_RX    = 4,
} i2s_mode_t;

/**
 * @brief I2S bit width per sample.
 *
 */
typedef enum {
    I2S_BITS_PER_SAMPLE_8BIT   = 8,
    I2S_BITS_PER_SAMPLE_16BIT   = 16,
    I2S_BITS_PER_SAMPLE_24BIT   = 24,
    I2S_BITS_PER_SAMPLE_32BIT   = 32,
} i2s_sample_bits_t;

/**
 * @brief I2S audio data formats.
 *
 */
typedef enum {
    I2S_STD_FORMAT_I2S,
    I2S_STD_FORMAT_MSB,
    I2S_STD_FORMAT_LSB,
    I2S_STD_FORMAT_PCM_SHORT,
    I2S_STD_FORMAT_PCM_LONG,
} i2s_std_format_t;

/**
 * @brief I2S data port interface type
 */
typedef enum {
    I2S_IFACE_TYPE_DMA,
    I2S_IFACE_TYPE_CPU,
} i2s_interface_t;


/**
 * @brief I2S channel format type
 */
typedef enum {
    I2S_CHANNEL_FMT_RIGHT_LEFT,
    I2S_CHANNEL_FMT_ALL_RIGHT,
    I2S_CHANNEL_FMT_ALL_LEFT,
    I2S_CHANNEL_FMT_ONLY_RIGHT,
    I2S_CHANNEL_FMT_ONLY_LEFT,
} i2s_channel_fmt_t;

/**
 * @brief I2S audio sample rate per second, measured in Hz.
 *
 */
typedef enum {
    I2S_SAMPLE_RATE_96K  = 96000,
    I2S_SAMPLE_RATE_48K  = 48000,
    I2S_SAMPLE_RATE_44K  = 44100,
    I2S_SAMPLE_RATE_32K  = 32000,
    I2S_SAMPLE_RATE_22K  = 22050,
    I2S_SAMPLE_RATE_16K  = 16000,
    I2S_SAMPLE_RATE_11K  = 11025,
    I2S_SAMPLE_RATE_8K   = 8000,
} i2s_sample_rate_t;

/**
 * @brief PDM sample rate ratio, measured in Hz.
 *
 */
typedef enum {
    PDM_SAMPLE_RATE_RATIO_64,
    PDM_SAMPLE_RATE_RATIO_128,
} pdm_sample_rate_ratio_t;

/**
 * @brief PDM PCM convter enable/disable.
 *
 */
typedef enum {
    PDM_PCM_CONV_ENABLE,
    PDM_PCM_CONV_DISABLE,
} pdm_pcm_conv_t;

/**
 * @brief I2s module work type.
 *
 */
typedef enum {
    I2S_MODULE_WORK_TYPE_I2S,
    I2S_MODULE_WORK_TYPE_PDM,
} i2s_module_work_type_t;


// @brief I2S interrupt source defined.
typedef enum {
    I2S_INT_SRC_ONE_BUF_SEND_DONE = I2S_OUT_EOF_INT_ENA,          //BIT(12) One package buffer has been sent.
    I2S_INT_SRC_ONE_BUF_RECV_DONE = I2S_IN_SUC_EOF_INT_ENA,       //BIT(9) Received one package buffer 

    I2S_INT_SRC_TX_EMPTY          = I2S_TX_REMPTY_INT_ENA,        //BIT(5) Tx buffer is empty
    I2S_INT_SRC_RX_FULL           = I2S_RX_WFULL_INT_ENA,         //BIT(2) Rx buffer is full
    I2S_INT_SRC_TX_HALF_EMPTY     = I2S_TX_PUT_DATA_INT_ENA,      //BIT(1) Tx buffer is half empty
    I2S_INT_SRC_RX_HALF_FULL      = I2S_RX_TAKE_DATA_INT_ENA,     //BIT(0) Rx buffer is half full
} i2s_int_src_t;

/**
 * @brief I2S attribute
 */
typedef struct {
    i2s_module_work_type_t       type;
    i2s_mode_t                   mode;
    i2s_sample_rate_t            rate;
    i2s_sample_bits_t            bits;
    i2s_std_format_t             std;
    i2s_interface_t              iface;
    i2s_channel_fmt_t            channel;
    pdm_sample_rate_ratio_t      ratio;
    pdm_pcm_conv_t               conv;
} i2s_module_info_t;

/**
 * @brief I2S attribute
 */
typedef struct {
    i2s_module_info_t  tx_mode;
    i2s_module_info_t  rx_mode;
} i2s_attr_t;

// I2S DMA attribite
typedef struct {
    uint32_t              mode;
    uint32_t              buf_size;
    ping_pong_buf_t      *buf;
} i2s_dma_attr_t;


/**
 * @brief      Initialize I2s with a given module.
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @param[in]  pAttr    Pointer to a struct i2s_attr_t that indicates I2S working attribution.
 *
 * @return     None
 *
 */
void i2s_init(i2s_num_t i2sNum, i2s_attr_t *pAttr);

/**
 * @brief      Reset I2s with a given module.
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @return     None
 *
 */
void i2s_reset(i2s_num_t i2sNum);

/**
 * @brief      Start I2s module
 *
 * @param[in]  type   Indicates I2s module which mode will be working.
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @param[in]  mode    Indicates I2S working mode.
 *
 * @return     None
 *
 */
void i2s_start(i2s_module_work_type_t type, i2s_num_t i2sNum, i2s_mode_t mode);

/**
 * @brief      Stop I2s module.
 *
 * @param[in]  type    Indicates I2s module which mode will be stop.
 *
 * @param[in]  i2sNum   Number of i2s module.
 *
 * @param[in]  mode    Indicates I2S working mode.
 *
 * @return     None
 *
 */
void i2s_stop(i2s_module_work_type_t type, i2s_num_t i2sNum, i2s_mode_t mode);

/**
 * @brief      Set i2s module clock.
 *
 * @param[in]  rate   bit clock.
 *
 * @param[in]  bits    bit length.
 *
 * @return     None
 *
 */
void i2s_clk_set(int rate, int bits);

/**
 * @brief      Enable I2s DMA mode.
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @param[in]  mode    Pointer to a struct i2s_attr_t that indicates I2S working attribution.
 *
 * @return     None
 *
 */
void i2s_dma_enable(i2s_num_t i2sNum, i2s_mode_t mode);

/**
 * @brief      Disable I2s DMA mode.
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @param[in]  mode    Pointer to a struct i2s_attr_t that indicates I2S working attribution.
 *
 * @return     None
 *
 */
void i2s_dma_disable(i2s_num_t i2sNum, i2s_mode_t mode);

/**
 * @brief      Get ping buffer address.
 *
 * @param[in]  obj   Number of i2s module
 *
 * @return     None
 *
 */
uint32_t* i2s_dma_ping_buf_get(i2s_dma_attr_t *obj);

/**
 * @brief      Get pong buffer address.
 *
 * @param[in]  obj   Number of i2s module
 *
 * @param[in]  pAttr    Pointer to a struct i2s_attr_t that indicates I2S working attribution.
 *
 * @return     None
 *
 */
uint32_t* i2s_dma_pong_buf_get(i2s_dma_attr_t *obj);

/**
 * @brief      Get without working ping-pong buffer address.
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @param[in]  obj    Pointer to a struct i2s_attr_t that indicates I2S working attribution.
 *
 * @return     None
 *
 */
uint32_t* i2s_dma_status_get(i2s_num_t i2sNum, i2s_dma_attr_t *obj);

/**
 * @brief      Set the Ping-Pong buffer to the destination
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @param[in]  obj    Pointer to a struct i2s_attr_t that indicates I2S working attribution.
 *
 * @return     None
 *
 */
void i2s_dma_dest_add_set(i2s_num_t i2sNum, i2s_dma_attr_t *obj);

/**
 * @brief      Initialize DMA and create a I2S DMA instance.
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @param[in]  obj    Pointer to a struct i2s_attr_t that indicates I2S working attribution.
 *
 * @param[in]  isr    Pointer to a function that will be called
 *
 * @return     -1:indicates parameter error, -2: DMA buffer init failure, others indicates success.
 *
 */
int i2s_dma_init(i2s_num_t i2sNum, i2s_dma_attr_t* obj, void *isr);

/**
 * @brief      Destroy the I2S DMA instance.
 *
 * @param[in]  obj   Pointer to a struct i2s_attr_t that indicates I2S working attribution.
 *
 * @return     ESP_FAIL:indicates parameter error, ESP_OK :indicates success.
 *
 */
esp_err_t i2s_dma_uninit(i2s_dma_attr_t *obj);

/**
 * @brief      Enable the I2s module interrupt source.
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @param[in]  intSrc   Indicates which interrupt source to be enable.
 *
 * @return     None
 *
 */
void i2s_int_enable(i2s_num_t i2sNum, i2s_int_src_t intSrc);

/**
 * @brief      Disable the I2s module interrupt source.
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @param[in]  intSrc   Indicates which interrupt source to be disable.
 *
 * @return     None
 *
 */
void i2s_int_disable(i2s_num_t i2sNum, i2s_int_src_t intSrc);

/**
 * @brief      Clear the I2s module interrupt source.
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @param[in]  intSrc   Indicates which interrupt source to be clear.
 *
 * @return     None
 *
 */
void i2s_int_clear(i2s_num_t i2sNum, i2s_int_src_t intSrc);

/**
 * @brief      Get the I2s module interrupt source
 *
 * @param[in]  i2sNum   Number of i2s module
 *
 * @return     None
 *
 */
uint32_t i2s_int_status_get(i2s_num_t i2sNum);


/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // __I2S_H__
