/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_ldo_regulator.h"
#include "esp_cache.h"
#include "driver/i2c_master.h"
#include "driver/isp.h"
#include "isp_af_scheme_sa.h"
#include "esp_cam_ctlr_csi.h"
#include "esp_cam_ctlr.h"
#include "esp_sccb_intf.h"
#include "esp_sccb_i2c.h"
#include "esp_cam_sensor.h"
#include "example_dsi_init.h"
#include "example_dsi_init_config.h"
#include "example_sensor_init.h"
#include "example_config.h"

static const char *TAG = "isp_dsi";

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);
static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);

/*---------------------------------------------------------------
                      Ping-Pong Buffer Management
---------------------------------------------------------------*/
typedef struct {
    void *fb0;                   // Frame buffer 0
    void *fb1;                   // Frame buffer 1
    void *csi_buffer;            // Current buffer for CSI to write
    void *dsi_buffer;            // Current buffer for DSI to display
    esp_lcd_panel_handle_t panel;// DPI panel handle
    int h_res;                   // Horizontal resolution
    int v_res;                   // Vertical resolution (full screen)
#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
    int crop_h_res;              // Cropped horizontal resolution
    int crop_v_res;              // Cropped vertical resolution
    void *pending_buffer;        // Buffer pending to be displayed
    SemaphoreHandle_t frame_ready_sem; // Semaphore to signal frame ready
#endif
} pingpong_buffer_ctx_t;

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
/**
 * @brief Process frame: Add blank areas to fill full screen resolution
 *
 * Algorithm: Fill from bottom to top to avoid overwriting crop data
 * - Cropped image is placed at original position (relative to full frame)
 * - Other areas are filled with white (0xFFFF for RGB565)
 *
 * Example: Original 100x100, crop (50,50) to (100,100) → shows at bottom-right
 *
 * @param buffer Frame buffer to process (contains cropped image at start)
 * @param ctx Ping-pong buffer context
 */
static void process_frame_with_blanks(void *buffer, pingpong_buffer_ctx_t *ctx)
{
    if (ctx->crop_v_res == ctx->v_res) {
        // No cropping, no need to process
        return;
    }

    uint16_t *fb = (uint16_t *)buffer;

    const int crop_left   = CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_H;
    const int crop_top    = CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_V;
    const int crop_right  = CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_H;
    const int crop_bottom = CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_V;
    const int crop_width  = crop_right - crop_left + 1;

    const int full_width  = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES;
    const int full_height = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES;

    // Helper macros for pixel indexing
#define SRC_PIXEL(x, y)  fb[(y) * crop_width + (x)]
#define DST_PIXEL(x, y)  fb[(y) * full_width + (x)]

    // ========== Step 1: Fill bottom blank region [crop_bottom+1, full_height) ==========
    if (crop_bottom + 1 < full_height) {
        memset(&DST_PIXEL(0, crop_bottom + 1),
               0xFF,
               full_width * (full_height - crop_bottom - 1) * EXAMPLE_RGB565_BYTES_PER_PIXEL);
    }

    // ========== Step 2: Process crop region [crop_top, crop_bottom] ==========
    for (int y = crop_bottom; y >= crop_top; y--) {
        int src_y = y - crop_top;  // Corresponding row in cropped data (0-based)

        // Fill right blank region first (crop_right+1, full_width)
        if (crop_right + 1 < full_width) {
            memset(&DST_PIXEL(crop_right + 1, y),
                   0xFF,
                   (full_width - crop_right - 1) * EXAMPLE_RGB565_BYTES_PER_PIXEL);
        }

        // Copy crop data from source to destination
        memcpy(&DST_PIXEL(crop_left, y),
               &SRC_PIXEL(0, src_y),
               crop_width * EXAMPLE_RGB565_BYTES_PER_PIXEL);

        // Fill left blank region [0, crop_left)
        if (crop_left > 0) {
            memset(&DST_PIXEL(0, y),
                   0xFF,
                   crop_left * EXAMPLE_RGB565_BYTES_PER_PIXEL);
        }
    }

    // ========== Step 3: Fill top blank region [0, crop_top) ==========
    if (crop_top > 0) {
        memset(&DST_PIXEL(0, 0),
               0xFF,
               full_width * crop_top * EXAMPLE_RGB565_BYTES_PER_PIXEL);
    }

#undef SRC_PIXEL
#undef DST_PIXEL
}
#endif

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
typedef union {
    struct {
        uint16_t s    : 4;
        uint16_t d    : 10;
        uint16_t flag : 1;
        uint16_t pd   : 1;
    };
    struct {
        uint16_t byte2 : 8;
        uint16_t byte1 : 8;
    };
    uint16_t val;
} dw9714_reg_t;

static bool IRAM_ATTR s_af_env_change_cb(isp_af_ctlr_t af_ctrlr, const esp_isp_af_env_detector_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    vTaskNotifyGiveFromISR(task_handle, &mustYield);

    return (mustYield == pdTRUE);
}

static esp_err_t s_sensor_set_focus_val(int focus_val, void *arg)
{
    esp_sccb_io_handle_t dw9714_io_handle = arg;

    dw9714_reg_t reg = {0};
    reg.d = (uint16_t)((focus_val / 120.0) * 1023.0);

    uint8_t data[2] = {0};
    data[0] = reg.byte1;
    data[1] = reg.byte2;

    uint16_t reg_addr = (data[0] << 8) + (data[1]);
    uint8_t reg_val = 0;

    esp_err_t ret = esp_sccb_transmit_reg_a16v8(dw9714_io_handle, reg_addr, reg_val);
    if (ret != ESP_OK) {
        printf("dw9714 esp_sccb_transmit_reg_a16v8 failed\n");
        return ret;
    }

    return ESP_OK;
}

static void af_task(void *arg)
{
    TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();

    typedef struct af_task_param_t {
        isp_proc_handle_t isp_proc;
        esp_sccb_io_handle_t dw9714_io_handle;
    } af_task_param_t;

    af_task_param_t af_task_param  = *(af_task_param_t *)arg;

    /**
     * AF window, windows for ISP hardware to record the
     * - luminance
     * - definition
     * of the current windows
     */
    esp_isp_af_config_t af_config = {
        .window = {
            [0] = {
                .top_left = {
                    .x = (CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES / 2) - 100,
                    .y = (CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES / 2) - 100,
                },
                .btm_right = {
                    .x = (CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES / 2) + 99,
                    .y = (CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES / 2) + 99,
                },
            },
            [1] = {
                .top_left = {
                    .x = (CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES / 2) - 100,
                    .y = (CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES / 2) - 100,
                },
                .btm_right = {
                    .x = (CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES / 2) + 99,
                    .y = (CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES / 2) + 99,
                },
            },
            [2] = {
                .top_left = {
                    .x = (CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES / 2) - 100,
                    .y = (CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES / 2) - 100,
                },
                .btm_right = {
                    .x = (CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES / 2) + 99,
                    .y = (CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES / 2) + 99,
                },
            },
        },
        .edge_thresh = 128,
    };

    isp_af_ctlr_t af_ctrlr = NULL;
    ESP_ERROR_CHECK(esp_isp_new_af_controller(af_task_param.isp_proc, &af_config, &af_ctrlr));

    esp_isp_af_env_config_t env_config = {
        .interval = 10,
    };
    ESP_ERROR_CHECK(esp_isp_af_controller_set_env_detector(af_ctrlr, &env_config));

    esp_isp_af_env_detector_evt_cbs_t cbs = {
        .on_env_change = s_af_env_change_cb,
    };
    ESP_ERROR_CHECK(esp_isp_af_env_detector_register_event_callbacks(af_ctrlr, &cbs, task_handle));

    isp_af_sa_scheme_config_t af_scheme_config = {
        .first_step_val = 12,
        .first_approx_cycles = 10,
        .second_step_val = 2,
        .second_approx_cycles = 10,
    };
    isp_af_scheme_handle_t af_scheme = NULL;
    ESP_ERROR_CHECK(isp_af_create_sa_scheme(af_ctrlr, &af_scheme_config, &af_scheme));

    isp_af_sa_scheme_sensor_drv_t sensor_driver = {
        .af_sensor_set_focus = s_sensor_set_focus_val,
    };
    isp_af_sa_scheme_sensor_info_t sensor_info = {
        .focus_val_max = 120,
    };
    ESP_ERROR_CHECK(isp_af_sa_scheme_register_sensor_driver(af_scheme, &sensor_driver, &sensor_info, af_task_param.dw9714_io_handle));

    int definition_thresh = 0;
    int luminance_thresh = 0;
    ESP_ERROR_CHECK(esp_isp_af_controller_enable(af_ctrlr));

    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_ERROR_CHECK(isp_af_process(af_scheme, &definition_thresh, &luminance_thresh));
        ESP_ERROR_CHECK(esp_isp_af_controller_set_env_detector_threshold(af_ctrlr, definition_thresh, luminance_thresh));
    }
}

/*---------------------------------------------------------------
                      Gamma Correction
---------------------------------------------------------------*/

static uint32_t s_gamma_correction_curve(uint32_t x)
{
    return pow((double)x / 256, 0.7) * 256;
}

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
/*---------------------------------------------------------------
                      Frame Processing Task
---------------------------------------------------------------*/
static void frame_processing_task(void *arg)
{
    pingpong_buffer_ctx_t *ctx = (pingpong_buffer_ctx_t *)arg;

    while (1) {
        // Wait for frame ready signal from ISR
        if (xSemaphoreTake(ctx->frame_ready_sem, portMAX_DELAY) == pdTRUE) {
            // Process the frame: add blank areas if needed
            process_frame_with_blanks(ctx->pending_buffer, ctx);

            // Ping-Pong switch: swap CSI write buffer and DSI display buffer
            void *temp = ctx->csi_buffer;
            ctx->csi_buffer = ctx->dsi_buffer;
            ctx->dsi_buffer = temp;

            // Trigger buffer switch by calling draw_bitmap
            // DPI driver will detect which buffer we're using and switch to it
            ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(ctx->panel,
                                                      0, 0,
                                                      ctx->h_res,
                                                      ctx->crop_v_res,
                                                      ctx->pending_buffer));

            ESP_LOGD(TAG, "Frame displayed: %p", ctx->pending_buffer);
        }
    }
}
#endif // CONFIG_EXAMPLE_ISP_CROP_ENABLE

void app_main(void)
{
    esp_err_t ret = ESP_FAIL;
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
    esp_lcd_panel_io_handle_t mipi_dbi_io = NULL;
    esp_lcd_panel_handle_t mipi_dpi_panel = NULL;
    void *fb0 = NULL;
    void *fb1 = NULL;
    size_t frame_buffer_size = 0;

    //mipi ldo
    esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
    esp_ldo_channel_config_t ldo_mipi_phy_config = {
        .chan_id = CONFIG_EXAMPLE_USED_LDO_CHAN_ID,
        .voltage_mv = CONFIG_EXAMPLE_USED_LDO_VOLTAGE_MV,
    };
    ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_mipi_phy_config, &ldo_mipi_phy));

    /**
     * @background
     * Sensor use RAW8
     * ISP convert to RGB565
     */
    //---------------DSI Init with Dual Frame Buffers------------------//
    example_dsi_alloc_config_t dsi_alloc_config = {
        .num_fbs = 2,  // Enable dual frame buffers
    };
    example_dsi_resource_alloc(&dsi_alloc_config, &mipi_dsi_bus, &mipi_dbi_io, &mipi_dpi_panel, &fb0, &fb1);

    //---------------Necessary variable config------------------//
    int display_h_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES;
    int display_v_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES;

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
    // Use cropped resolution for frame buffer
    display_h_res = CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_H - CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_H + 1;
    display_v_res = CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_V - CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_V + 1;
#endif

    frame_buffer_size = CONFIG_EXAMPLE_MIPI_DSI_DISP_HRES * CONFIG_EXAMPLE_MIPI_DSI_DISP_VRES * EXAMPLE_RGB565_BYTES_PER_PIXEL;

    ESP_LOGI(TAG, "Original CSI resolution: %dx%d", CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES, CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES);
    ESP_LOGI(TAG, "Display resolution: %dx%d, bits per pixel: %d", display_h_res, display_v_res, EXAMPLE_RGB565_BITS_PER_PIXEL);
    ESP_LOGI(TAG, "Frame buffers: fb0=%p, fb1=%p", fb0, fb1);

    //---------------Ping-Pong Buffer Context------------------//
    pingpong_buffer_ctx_t pp_ctx = {
        .fb0 = fb0,
        .fb1 = fb1,
        .csi_buffer = fb0,  // CSI starts writing to fb0
        .dsi_buffer = fb1,  // DSI starts displaying fb1
        .panel = mipi_dpi_panel,
        .h_res = CONFIG_EXAMPLE_MIPI_DSI_DISP_HRES,
        .v_res = CONFIG_EXAMPLE_MIPI_DSI_DISP_VRES,
#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
        .crop_h_res = display_h_res,
        .crop_v_res = display_v_res,
        .pending_buffer = NULL,
        .frame_ready_sem = xSemaphoreCreateBinary(),
#endif
    };

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
    if (pp_ctx.frame_ready_sem == NULL) {
        ESP_LOGE(TAG, "Failed to create frame ready semaphore");
        return;
    }
#endif

    esp_cam_ctlr_trans_t new_trans = {
        .buffer = pp_ctx.csi_buffer,
        .buflen = frame_buffer_size,
    };

    //--------Camera Sensor and SCCB Init-----------//
    example_sensor_handle_t sensor_handle = {
        .sccb_handle = NULL,
        .i2c_bus_handle = NULL,
    };
    example_sensor_config_t cam_sensor_config = {
        .i2c_port_num = I2C_NUM_0,
        .i2c_sda_io_num = EXAMPLE_MIPI_CSI_CAM_SCCB_SDA_IO,
        .i2c_scl_io_num = EXAMPLE_MIPI_CSI_CAM_SCCB_SCL_IO,
        .port = ESP_CAM_SENSOR_MIPI_CSI,
        .format_name = EXAMPLE_CAM_FORMAT,
    };
    example_sensor_init(&cam_sensor_config, &sensor_handle);

    //---------------VCM SCCB Init------------------//
    esp_sccb_io_handle_t dw9714_io_handle = NULL;
    sccb_i2c_config_t i2c_config = {
        .scl_speed_hz = EXAMPLE_MIPI_SCCB_FREQ,
        .device_address = EXAMPLE_DW9714_DEV_ADDR,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    };
    ESP_ERROR_CHECK(sccb_new_i2c_io(sensor_handle.i2c_bus_handle, &i2c_config, &dw9714_io_handle));

    //---------------CSI Init------------------//
    esp_cam_ctlr_csi_config_t csi_config = {
        .ctlr_id = 0,
        .h_res = display_h_res,
        .v_res = display_v_res,
        .lane_bit_rate_mbps = EXAMPLE_MIPI_CSI_LANE_BITRATE_MBPS,
        .input_data_color_type = CAM_CTLR_COLOR_RAW8,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .data_lane_num = 2,
        .byte_swap_en = false,
        .queue_items = 1,
    };
    esp_cam_ctlr_handle_t handle = NULL;
    ret = esp_cam_new_csi_ctlr(&csi_config, &handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "csi init fail[%d]", ret);
        return;
    }

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans = s_camera_get_new_vb,
        .on_trans_finished = s_camera_get_finished_trans,
    };
    if (esp_cam_ctlr_register_event_callbacks(handle, &cbs, &pp_ctx) != ESP_OK) {
        ESP_LOGE(TAG, "ops register fail");
        return;
    }

    ESP_ERROR_CHECK(esp_cam_ctlr_enable(handle));
    /*---------------------------------------------------------------
                          ISP Init
    ---------------------------------------------------------------*/
    isp_proc_handle_t isp_proc = NULL;
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
        .has_line_start_packet = false,
        .has_line_end_packet = false,
        .h_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES,
        .v_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES,
    };
    ESP_ERROR_CHECK(esp_isp_new_processor(&isp_config, &isp_proc));
    ESP_ERROR_CHECK(esp_isp_enable(isp_proc));

    /*---------------------------------------------------------------
                          BF
    ---------------------------------------------------------------*/
    esp_isp_bf_config_t bf_config = {
        .denoising_level = 5,
        .padding_mode = ISP_BF_EDGE_PADDING_MODE_SRND_DATA,
        .bf_template = {
            {1, 2, 1},
            {2, 4, 2},
            {1, 2, 1},
        },
        .padding_line_tail_valid_start_pixel = 0,
        .padding_line_tail_valid_end_pixel = 0,
    };
    ESP_ERROR_CHECK(esp_isp_bf_configure(isp_proc, &bf_config));
    ESP_ERROR_CHECK(esp_isp_bf_enable(isp_proc));

    /*---------------------------------------------------------------
                          BLC
    ---------------------------------------------------------------*/
#if CONFIG_ESP32P4_REV_MIN_FULL >= 300
    /**
     * This piece of BLC code is to show how to use the BLC related APIs.
     * Suggested way to calibrate the BLC is by covering the lens and record the raw data.
     * Then, use the recorded data to calibrate the BLC.
     */
    esp_isp_blc_config_t blc_config = {
        .window = {
            .top_left = {
                .x = 0,
                .y = 0,
            },
            .btm_right = {
                .x = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES,
                .y = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES,
            },
        },
        .filter_enable = true,
        .filter_threshold = {
            .top_left_chan_thresh = 128,
            .top_right_chan_thresh = 128,
            .bottom_left_chan_thresh = 128,
            .bottom_right_chan_thresh = 128,
        },
        .stretch = {
            .top_left_chan_stretch_en = true,
            .top_right_chan_stretch_en = true,
            .bottom_left_chan_stretch_en = true,
            .bottom_right_chan_stretch_en = true,
        },
    };
    ESP_ERROR_CHECK(esp_isp_blc_configure(isp_proc, &blc_config));
    ESP_ERROR_CHECK(esp_isp_blc_enable(isp_proc));

    esp_isp_blc_offset_t blc_offset = {
        .top_left_chan_offset = 20,
        .top_right_chan_offset = 20,
        .bottom_left_chan_offset = 20,
        .bottom_right_chan_offset = 20,
    };
    ESP_ERROR_CHECK(esp_isp_blc_set_correction_offset(isp_proc, &blc_offset));
#endif

    /*---------------------------------------------------------------
                          DEMOSAIC
    ---------------------------------------------------------------*/
    esp_isp_demosaic_config_t demosaic_config = {
        .grad_ratio = {
            .integer = 2,
            .decimal = 5,
        },
    };
    ESP_ERROR_CHECK(esp_isp_demosaic_configure(isp_proc, &demosaic_config));
    ESP_ERROR_CHECK(esp_isp_demosaic_enable(isp_proc));

    /*---------------------------------------------------------------
                          CCM
    ---------------------------------------------------------------*/
    /**
     * CCM is used for color correction and white balance adjustment.
     * It should be configured after demosaic and before gamma correction.
     *
     * The matrix format is:
     * [R_out]   [RR RG RB] [R_in]
     * [G_out] = [GR GG GB] [G_in]
     * [B_out]   [BR BG BB] [B_in]
     *
     * For ESP32P4 ECO5:
     * - Matrix coefficients range: ±15.996 (4-bit integer + 8-bit fraction)
     * - For earlier versions: ±3.999 (2-bit integer + 10-bit fraction)
     */
    esp_isp_ccm_config_t ccm_config = {
        .matrix = {
            // Default identity matrix (no color correction)
            {1.0, 0.0, 0.0},   // R channel: R = 1.0*R + 0.0*G + 0.0*B
            {0.0, 1.0, 0.0},   // G channel: G = 0.0*R + 1.0*G + 0.0*B
            {0.0, 0.0, 1.0}    // B channel: B = 0.0*R + 0.0*G + 1.0*B
        },
        .saturation = false        // Don't use saturation for out-of-range values
    };

    ESP_ERROR_CHECK(esp_isp_ccm_configure(isp_proc, &ccm_config));
    ESP_ERROR_CHECK(esp_isp_ccm_enable(isp_proc));

    /*---------------------------------------------------------------
                          GAMMA
    ---------------------------------------------------------------*/
    isp_gamma_curve_points_t pts = {};
    ESP_ERROR_CHECK(esp_isp_gamma_fill_curve_points(s_gamma_correction_curve, &pts));
    ESP_ERROR_CHECK(esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_R, &pts));
    ESP_ERROR_CHECK(esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_G, &pts));
    ESP_ERROR_CHECK(esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_B, &pts));
    ESP_ERROR_CHECK(esp_isp_gamma_enable(isp_proc));

    /*---------------------------------------------------------------
                          SHARPEN
    ---------------------------------------------------------------*/
    esp_isp_sharpen_config_t sharpen_config = {
        .h_freq_coeff = {
            .integer = 2,
            .decimal = 0,
        },
        .m_freq_coeff = {
            .integer = 2,
            .decimal = 0,
        },
        .h_thresh = 255,
        .l_thresh = 0,
        .padding_mode = ISP_SHARPEN_EDGE_PADDING_MODE_SRND_DATA,
        .sharpen_template = {
            {1, 2, 1},
            {2, 4, 2},
            {1, 2, 1},
        },
        .padding_line_tail_valid_start_pixel = 0,
        .padding_line_tail_valid_end_pixel = 0,
    };
    ESP_ERROR_CHECK(esp_isp_sharpen_configure(isp_proc, &sharpen_config));
    ESP_ERROR_CHECK(esp_isp_sharpen_enable(isp_proc));

    /*---------------------------------------------------------------
                          COLOR
    ---------------------------------------------------------------*/
    esp_isp_color_config_t color_config = {
        .color_contrast = {
            .integer = 1,
            .decimal = 0,
        },
        .color_saturation = {
            .integer = 1,
            .decimal = 0,
        },
        .color_hue = 0,
        .color_brightness = 0,
    };
    ESP_ERROR_CHECK(esp_isp_color_configure(isp_proc, &color_config));
    ESP_ERROR_CHECK(esp_isp_color_enable(isp_proc));

#if CONFIG_ESP32P4_REV_MIN_FULL >= 100
    /*---------------------------------------------------------------
                          LSC
    ---------------------------------------------------------------*/
    esp_isp_lsc_gain_array_t gain_array = {};
    esp_isp_lsc_config_t lsc_config = {
        .gain_array = &gain_array,
    };
    size_t gain_size = 0;
    ESP_ERROR_CHECK(esp_isp_lsc_allocate_gain_array(isp_proc, &gain_array, &gain_size));

    isp_lsc_gain_t gain_val = {
        .decimal = 204,
        .integer = 0,
    };
    for (int i = 0; i < gain_size; i++) {
        gain_array.gain_r[i].val = gain_val.val;
        gain_array.gain_gr[i].val = gain_val.val;
        gain_array.gain_gb[i].val = gain_val.val;
        gain_array.gain_b[i].val = gain_val.val;
    }
    ESP_ERROR_CHECK(esp_isp_lsc_configure(isp_proc, &lsc_config));
    ESP_ERROR_CHECK(esp_isp_lsc_enable(isp_proc));
#endif

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
    /*---------------------------------------------------------------
                          CROP
    ---------------------------------------------------------------*/
    esp_isp_crop_config_t crop_config = {
        .window = {
            .top_left = {
                .x = CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_H,
                .y = CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_V
            },
            .btm_right = {
                .x = CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_H,
                .y = CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_V
            }
        }
    };
    ESP_ERROR_CHECK(esp_isp_crop_configure(isp_proc, &crop_config));
    ESP_ERROR_CHECK(esp_isp_crop_enable(isp_proc));
    ESP_LOGI(TAG, "ISP Crop enabled: (%d,%d) to (%d,%d)",
             CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_H, CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_V,
             CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_H, CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_V);
#endif

    typedef struct af_task_param_t {
        isp_proc_handle_t isp_proc;
        esp_sccb_io_handle_t dw9714_io_handle;
    } af_task_param_t;

    af_task_param_t af_task_param = {
        .isp_proc = isp_proc,
        .dw9714_io_handle = dw9714_io_handle,
    };
    xTaskCreatePinnedToCore(af_task, "af_task", 8192, &af_task_param, 5, NULL, 0);

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
    //---------------Frame Processing Task------------------//
    xTaskCreatePinnedToCore(frame_processing_task, "frame_proc", 4096, &pp_ctx, 6, NULL, 0);
    ESP_LOGI(TAG, "Frame processing task created");
#endif

    //---------------DPI Reset------------------//
    example_dpi_panel_reset(mipi_dpi_panel);

    //init both frame buffers to white
    memset(fb0, 0xFF, frame_buffer_size);
    memset(fb1, 0xFF, frame_buffer_size);
    esp_cache_msync((void *)fb0, frame_buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)fb1, frame_buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    if (esp_cam_ctlr_start(handle) != ESP_OK) {
        ESP_LOGE(TAG, "Driver start fail");
        return;
    }

    example_dpi_panel_init(mipi_dpi_panel);

    while (1) {
        ESP_ERROR_CHECK(esp_cam_ctlr_receive(handle, &new_trans, ESP_CAM_CTLR_MAX_DELAY));
    }
}

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    pingpong_buffer_ctx_t *ctx = (pingpong_buffer_ctx_t *)user_data;

    // Provide the current CSI buffer for the next frame
    trans->buffer = ctx->csi_buffer;
    trans->buflen = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES * CONFIG_EXAMPLE_MIPI_DSI_DISP_VRES * EXAMPLE_RGB565_BYTES_PER_PIXEL;

    return false;
}

bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    pingpong_buffer_ctx_t *ctx = (pingpong_buffer_ctx_t *)user_data;

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
    BaseType_t high_task_wakeup = pdFALSE;
    ctx->pending_buffer = trans->buffer;
    xSemaphoreGiveFromISR(ctx->frame_ready_sem, &high_task_wakeup);
    return (high_task_wakeup == pdTRUE);
#else
    void *temp = ctx->csi_buffer;
    ctx->csi_buffer = ctx->dsi_buffer;
    ctx->dsi_buffer = temp;

    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(ctx->panel, 0, 0, ctx->h_res, ctx->v_res, trans->buffer));
    return false;
#endif
}
