/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdatomic.h>
#include "sys/queue.h"
#include "esp_private/dma2d.h"
#include "driver/jpeg_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "hal/jpeg_hal.h"
#include "esp_intr_types.h"
#include "esp_pm.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JPEG_MEM_ALLOC_CAPS               (MALLOC_CAP_DEFAULT)

#define JPEG_ALLOW_INTR_PRIORITY_MASK     (ESP_INTR_FLAG_LOWMED)

// JPEG encoder and decoder shares same interrupt ID.
#define JPEG_INTR_ALLOC_FLAG              (ESP_INTR_FLAG_SHARED)

#define JPEG_ALIGN_UP(num, align)         (((num) + ((align) - 1)) & ~((align) - 1))

typedef struct jpeg_decoder_t jpeg_decoder_t;
typedef struct jpeg_encoder_t jpeg_encoder_t;
typedef struct jpeg_codec_t jpeg_codec_t;
typedef struct jpeg_codec_t *jpeg_codec_handle_t;

typedef struct jpeg_isr_handler_ {
    uint32_t mask;
    intr_handler_t handler;
    void* handler_arg;
    uint32_t flags;
    SLIST_ENTRY(jpeg_isr_handler_) next;
} jpeg_isr_handler_t;

struct jpeg_codec_t {
    SemaphoreHandle_t codec_mutex;    // pretend that one picture is in process, no other picture can interrupt current stage.
    jpeg_hal_context_t hal;           // Hal layer for each port(bus)
    portMUX_TYPE spinlock;            // To protect pre-group register level concurrency access
    intr_handle_t intr_handle;        // jpeg codec interrupt handler
    int intr_priority;                // jpeg codec interrupt priority
    SLIST_HEAD(jpeg_isr_handler_list_, jpeg_isr_handler_) jpeg_isr_handler_list; // List for jpeg interrupt.
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock; // power manage lock
#endif
};

typedef enum {
    JPEG_DEC_DIRECT_OUTPUT_HB = 0, /*!< Direct output */
    JPEG_DEC_YUV444_HB = 1,        /*!< output YUV444 format */
    JPEG_DEC_RGB888_HB = 2,        /*!< output RGB888 format */
    JPEG_DEC_RGB565_HB = 3,        /*!< output RGB565 format */
    JPEG_DEC_GRAY_HB = 4,          /*!< output the gray picture */
    JPEG_DEC_BEST_HB_MAX,          /*!< Max value of output formats */
} jpeg_dec_format_hb_t;

typedef struct {
    uint8_t *buffer_offset;                                     // Pointer points to picture buffer.
    uint32_t buffer_left;                                       // Data left in the picture.
    uint32_t header_size;                                       // record the picture header size.
    uint32_t process_v;                                         // vertical output in processing, multiple of mcu.
    uint32_t process_h;                                         // horizontal output in processing, multiple of mcu.
    uint32_t origin_v;                                          // vertical for the origin size of picture.
    uint32_t origin_h;                                          // horizontal for the origin size of picture.
    uint8_t mcux;                                               // the best value of minimum coding unit horizontal unit
    uint8_t mcuy;                                               // minimum coding unit vertical unit
    uint8_t qt_tbl_num;                                         // quantization table number
    uint32_t qt_tbl[JPEG_COMPONENT_NUMBER_MAX][JPEG_QUANTIZATION_TABLE_LEN];            // quantization table content [id]
    uint8_t nf;                                                 // number of frames
    uint8_t ci[JPEG_COMPONENT_NUMBER_MAX];                      // Component identifier.
    uint8_t hivi[JPEG_COMPONENT_NUMBER_MAX];                    // H&V sampling factor
    uint8_t hi[JPEG_COMPONENT_NUMBER_MAX];                      // Horizontal sampling factor
    uint8_t vi[JPEG_COMPONENT_NUMBER_MAX];                      // Vertical sampling factor
    uint8_t qtid[JPEG_COMPONENT_NUMBER_MAX];                    // Quantization table destination selector
    jpeg_huffman_table_info_t huffinfo;                         // Huffman table specification information
    uint8_t huffbits[2][2][JPEG_HUFFMAN_BITS_LEN_TABLE_LEN];    // Huffman bit distribution tables [id][dcac]
    uint8_t huffcode[2][2][JPEG_HUFFMAN_AC_VALUE_TABLE_LEN];    // Huffman decoded data tables [id][dcac]
    uint32_t tmp_huff[JPEG_HUFFMAN_AC_VALUE_TABLE_LEN];         // temp buffer to store huffman code
    bool dht_marker;                                            // If we have Huffman table present in header
    uint16_t ri;                                                // Restart interval
} jpeg_dec_header_info_t;

struct jpeg_decoder_t {
    jpeg_codec_t *codec_base;                    // Pointer to jpeg codec hardware base
    jpeg_dec_header_info_t *header_info;         // Pointer to current picture information
    jpeg_down_sampling_type_t sample_method;     // method of sampling the JPEG picture.
    jpeg_dec_output_format_t output_format;      // picture output format.
    jpeg_dec_rgb_element_order_t rgb_order;      // RGB pixel order
    jpeg_yuv_rgb_conv_std_t conv_std;            // YUV RGB conversion standard
    bool no_color_conversion;                    // No color conversion, directly output based on compressed format
    uint8_t bit_per_pixel;                       // bit size per pixel
    QueueHandle_t evt_queue;                     // jpeg event from 2DDMA and JPEG engine
    uint8_t *decoded_buf;                        // pointer to the rx buffer.
    uint32_t total_size;                         // jpeg picture origin size (in bytes)
    TickType_t timeout_tick;                     // timeout value for jpeg decoder (in cpu tick).
    jpeg_isr_handler_t *intr_handle;             // jpeg decoder interrupt handler
    //dma handles
    dma2d_pool_handle_t dma2d_group_handle;      // 2D-DMA group handle
    dma2d_descriptor_t *rxlink;                  // Pointer to 2D-DMA rx descriptor
    dma2d_descriptor_t *txlink;                  // Pointer to 2D-DMA tx descriptor
    uint32_t dma_desc_size;                      // tx and rx linker alignment
    dma2d_channel_handle_t dma2d_rx_channel;     // DMA2D RX channel handle
    dma2d_channel_handle_t dma2d_tx_channel;     // DMA2D TX channel handle
    dma2d_trans_t* trans_desc;   // DMA2D transaction descriptor
};

typedef enum {
    JPEG_DMA2D_RX_EOF = BIT(0),     // DMA2D rx eof event
    JPEG_DMA2D_RX_DONE = BIT(1),    // DMA2D rx done event
    JPEG_DMA2D_TX_DONE = BIT(2),    // DMA2D tx done event
    JPEG_DMA2D_RX_DESC_EMPTY = BIT(3),    // DMA2D rx empty event
} jpeg_dma2d_evt_enum_t;

typedef struct {
    jpeg_dma2d_evt_enum_t dma_evt;   // jpeg-2ddma event, (triggered from 2ddma interrupt)
    uint32_t jpgd_status;            // jpeg decoder status, (triggered from jpeg interrupt)
} jpeg_dma2d_dec_evt_t;

typedef enum {
    JPEG_ENC_SRC_RGB888_HB = 0,      // Input RGB888 format
    JPEG_ENC_SRC_YUV422_HB = 1,       // Input YUV422 format
    JPEG_ENC_SRC_RGB565_HB = 2,      // Input RGB565 format
    JPEG_ENC_SRC_GRAY_HB = 3,        // Input GRAY format
    JPEG_ENC_BEST_HB_MAX,
} jpeg_enc_format_hb_t;

typedef struct {
    jpeg_dma2d_evt_enum_t dma_evt;    // jpeg-2ddma event, (triggered from 2ddma interrupt)
    uint32_t encoder_status;          // jpeg encoder status, (triggered from jpeg interrupt)
} jpeg_enc_dma2d_evt_t;

typedef struct {
    uint8_t *header_buf;                           // Pointer to the header of jpeg header buffer
    uint32_t header_len;                           // Record for header length
    uint32_t m_quantization_tables[2][JPEG_QUANTIZATION_TABLE_LEN];         // quantization tables
    uint8_t num_components;                        // number of components
    uint32_t origin_h;                             // horizontal of original picture
    uint32_t origin_v;                             // vertical of original picture
    uint32_t quality;                              // JPEG compressed quality.
    jpeg_down_sampling_type_t sub_sample;          // Picture sub-sampling method
} jpeg_enc_header_info_t;

struct jpeg_encoder_t {
    jpeg_codec_t *codec_base;                      // Pointer to jpeg codec hardware base
    jpeg_enc_src_type_t color_space;               // Picture source color space
    jpeg_enc_input_format_t picture_format;        // Source picture format
    jpeg_enc_header_info_t *header_info;           // Pointer to header buffer information
    uint32_t bytes_per_pixel;                      // Bytes per pixel of source image format
    uint8_t mcux;                                  // the best value of minimum coding unit horizontal unit
    uint8_t mcuy;                                  // minimum coding unit vertical unit
    jpeg_isr_handler_t *intr_handle;               // jpeg encoder interrupt handler
    TickType_t timeout_tick;                       // timeout value for jpeg decoder (in cpu tick).
    QueueHandle_t evt_queue;                       // jpeg event from 2DDMA and JPEG engine
    // dma_handles
    dma2d_pool_handle_t dma2d_group_handle;        // 2D-DMA group handle
    dma2d_descriptor_t *rxlink;                    // Pointer to 2D-DMA rx descriptor
    dma2d_descriptor_t *txlink;                    // Pointer to 2D-DMA tx descriptor
    uint32_t dma_desc_size;                        // tx and rx linker alignment
    dma2d_trans_t* trans_desc;                     // DMA2D transaction descriptor
    dma2d_channel_handle_t dma2d_rx_channel;       // DMA2D RX channel handle
    dma2d_channel_handle_t dma2d_tx_channel;       // DMA2D TX channel handle
};

#define JPEG_DMA2D_2D_ENABLE       (1)        // DMA2D two dimension enable
#define JPEG_DMA2D_2D_DISABLE      (0)        // DMA2D one dimension enable
#define JPEG_DMA2D_MAX_SIZE        (0x3fff)   // DMA2D max size (low 14 bit)
#define JPEG_DMA2D_EOF_LAST        (1)        // DMA2D EOF last
#define JPEG_DMA2D_EOF_NOT_LAST    (0)        // DMA2D EOF not last
#define JPEG_DMA2D_1D_HIGH_14BIT   (14)       // 1D high 14 bites

/**
 * @brief Acquire a JPEG codec handle.
 *
 * @param[out] jpeg_new_codec  Pointer to the variable where the acquired JPEG codec handle will be stored.
 *
 * @return
 *         - ESP_OK if the JPEG codec handle is successfully acquired.
 *         - ESP_ERR_NO_MEM if there is not enough memory to acquire the codec handle.
 *         - Other error codes indicating the cause of the failure.
 */
esp_err_t jpeg_acquire_codec_handle(jpeg_codec_handle_t *jpeg_new_codec);

/**
 * @brief Release a JPEG codec handle.
 *
 * @param[in] jpeg_codec  The JPEG codec handle to release.
 *
 * @return
 *         - ESP_OK if the JPEG codec handle is successfully released.
 *         - ESP_ERR_INVALID_ARG if the provided JPEG codec handle is invalid.
 *         - Other error codes indicating the cause of the failure.
 */
esp_err_t jpeg_release_codec_handle(jpeg_codec_handle_t jpeg_codec);

/**
 * @brief Register an ISR handler for JPEG interrupt
 *
 * This function registers an Interrupt Service Routine (ISR) handler for JPEG interrupt.
 *
 * @param jpeg_codec The JPEG codec handle
 * @param handler The ISR handler function to be registered
 * @param handler_arg An argument to be passed to the ISR handler function
 * @param jpeg_intr_mask The JPEG interrupt mask value
 * @param flags Additional flags for ISR registration
 * @param jpeg_intr_handler JPEG interrupt handler
 * @return esp_err_t Returns ESP_OK on success, or an error code on failure
 */
esp_err_t jpeg_isr_register(jpeg_codec_handle_t jpeg_codec, intr_handler_t handler, void* handler_arg, uint32_t jpeg_intr_mask, uint32_t flags, jpeg_isr_handler_t** jpeg_intr_handler);

/**
 * @brief Deregister an ISR handler for JPEG interrupt
 *
 * This function deregisters an Interrupt Service Routine (ISR) handler for JPEG interrupt.
 *
 * @param jpeg_codec The JPEG codec handle
 * @param handler The ISR handler function to be deregistered
 * @param handler_arg The argument previously passed to the ISR handler function
 * @param jpeg_intr_handler JPEG interrupt handler
 * @return esp_err_t Returns ESP_OK on success, or an error code on failure
 */
esp_err_t jpeg_isr_deregister(jpeg_codec_handle_t jpeg_codec, jpeg_isr_handler_t *jpeg_intr_handler);

/**
 * @brief Check the interrupt priority for JPEG codec
 *
 * This function checks the interrupt priority for the JPEG codec to ensure it meets the specified requirements.
 *
 * @param jpeg_codec The JPEG codec handle
 * @param intr_priority The interrupt priority value to be checked
 * @return esp_err_t Returns ESP_OK if the interrupt priority meets the requirements, or an error code on failure
 */
esp_err_t jpeg_check_intr_priority(jpeg_codec_handle_t jpeg_codec, int intr_priority);

#ifdef __cplusplus
}
#endif
