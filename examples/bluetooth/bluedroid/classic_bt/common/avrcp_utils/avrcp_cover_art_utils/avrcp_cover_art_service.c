/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "jpeg_decoder.h"
#include "avrcp_common_utils.h"
#include "avrcp_cover_art_service.h"

/* tags*/
#define RC_CA_SRV_TAG  "RC_CA_SRV"

//Define the height and width of the jpeg file. Make sure this matches the actual jpeg
//dimensions.
#define IMAGE_W 200
#define IMAGE_H 200

// Using SPI2 in the example, as it also supports octal modes on some targets
#define LCD_HOST       SPI2_HOST
// To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many.
// More means more memory use, but less overhead for setting up / finishing transfers. Make sure 240
// is dividable by this.
#define PARALLEL_LINES CONFIG_EXAMPLE_LCD_FLUSH_PARALLEL_LINES

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ (20 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  0
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_DATA0          23  /*!< for 1-line SPI, this also referred as MOSI */
#define EXAMPLE_PIN_NUM_PCLK           19
#define EXAMPLE_PIN_NUM_CS             22
#define EXAMPLE_PIN_NUM_DC             21
#define EXAMPLE_PIN_NUM_RST            18
#define EXAMPLE_PIN_NUM_BK_LIGHT       5

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              200
#define EXAMPLE_LCD_V_RES              200
// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8

/**
 * @brief AVRCP cover art service control block structure
 */
typedef struct {
    bool connected;                          /* Connection status flag */
    bool getting;                            /* Flag indicating if image is being retrieved */
    /* Related to the image */
    uint8_t image_hdl_old[7];                /* Previous image handle, used to detect image changes */
    uint32_t image_size;                     /* Size of the image data in bytes */
    uint8_t *image_data;                     /* Pointer to the image data buffer */
    bool image_final;                        /* Indicate whether the image reception has been completed */
    uint16_t *pixels;                        /* Pointer to decoded pixel data */
    esp_lcd_panel_io_handle_t io_handle;     /* LCD panel IO handle */
    esp_lcd_panel_handle_t panel_handle;     /* LCD panel handle */
} avrc_cover_art_srv_cb_t;

/*******************************
 * STATIC FUNCTION DECLARATIONS
 ******************************/

/* image handle check */
static bool avrc_cover_art_srv_image_handle_check(uint8_t *image_handle, int len);
/* free image data */
static void avrc_cover_art_srv_free_image_data(void);
/* initialize display (currently supports LCD only) */
static void avrc_cover_art_srv_init_display(void);
/* deinitialize display (currently supports LCD only) */
static void avrc_cover_art_srv_deinit_display(void);
/* free pixels */
static void avrc_cover_art_srv_free_pixels(void);
/* decode image */
static esp_err_t avrc_cover_art_srv_decode_image(void);
/* display image */
static void avrc_cover_art_srv_display_image(void);

/*******************************
 * STATIC VARIABLE DEFINITIONS
 ******************************/

/* avrcp cover art service control block */
static avrc_cover_art_srv_cb_t s_avrc_cover_art_srv_cb;

/********************************
 * STATIC FUNCTION DEFINITIONS
 *******************************/

static bool avrc_cover_art_srv_image_handle_check(uint8_t *image_handle, int len)
{
    /* Image handle length must be 7 */
    if (len == 7 && memcmp(s_avrc_cover_art_srv_cb.image_hdl_old, image_handle, 7) != 0) {
        memcpy(s_avrc_cover_art_srv_cb.image_hdl_old, image_handle, 7);
        return true;
    }
    return false;
}

static void avrc_cover_art_srv_free_image_data(void)
{
    if (s_avrc_cover_art_srv_cb.image_data) {
        free(s_avrc_cover_art_srv_cb.image_data);
        s_avrc_cover_art_srv_cb.image_data = NULL;
    }
    s_avrc_cover_art_srv_cb.image_size = 0;
}

static void avrc_cover_art_srv_init_display(void)
{
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    /* Initialize the GPIO of backlight */
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    spi_bus_config_t buscfg = {
        .sclk_io_num = EXAMPLE_PIN_NUM_PCLK,
        .mosi_io_num = EXAMPLE_PIN_NUM_DATA0,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = PARALLEL_LINES * EXAMPLE_LCD_H_RES * 2 + 8
    };
    /* Initialize the SPI bus */
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_DC,
        .cs_gpio_num = EXAMPLE_PIN_NUM_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    /* Attach the LCD to the SPI bus */
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &s_avrc_cover_art_srv_cb.io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    /* Initialize the LCD configuration */
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(s_avrc_cover_art_srv_cb.io_handle, &panel_config, &s_avrc_cover_art_srv_cb.panel_handle));

    /* Turn off backlight to avoid unpredictable display on the LCD screen while initializing
     * the LCD panel driver. (Different LCD screens may need different levels) */
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL));

    /* Reset the display */
    ESP_ERROR_CHECK(esp_lcd_panel_reset(s_avrc_cover_art_srv_cb.panel_handle));

    /* Initialize LCD panel */
    ESP_ERROR_CHECK(esp_lcd_panel_init(s_avrc_cover_art_srv_cb.panel_handle));

    /* Turn on the screen */
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(s_avrc_cover_art_srv_cb.panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(s_avrc_cover_art_srv_cb.panel_handle, true));

    /* Swap x and y axis (Different LCD screens may need different options) */
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(s_avrc_cover_art_srv_cb.panel_handle, true));

    /* Turn on backlight (Different LCD screens may need different levels) */
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL));
}

static void avrc_cover_art_srv_deinit_display(void)
{
    /* Turn off backlight first */
    ESP_ERROR_CHECK(gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL));

    /* Turn off the display */
    if (s_avrc_cover_art_srv_cb.panel_handle) {
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(s_avrc_cover_art_srv_cb.panel_handle, false));
        /* Delete the LCD panel */
        ESP_ERROR_CHECK(esp_lcd_panel_del(s_avrc_cover_art_srv_cb.panel_handle));
        s_avrc_cover_art_srv_cb.panel_handle = NULL;
    }

    /* Delete the LCD panel IO */
    if (s_avrc_cover_art_srv_cb.io_handle) {
        ESP_ERROR_CHECK(esp_lcd_panel_io_del(s_avrc_cover_art_srv_cb.io_handle));
        s_avrc_cover_art_srv_cb.io_handle = NULL;
    }

    /* Free the SPI bus */
    ESP_ERROR_CHECK(spi_bus_free(LCD_HOST));

    /* Reset backlight GPIO */
    ESP_ERROR_CHECK(gpio_reset_pin(EXAMPLE_PIN_NUM_BK_LIGHT));
}

static void avrc_cover_art_srv_free_pixels(void)
{
    if (s_avrc_cover_art_srv_cb.pixels) {
        free(s_avrc_cover_art_srv_cb.pixels);
        s_avrc_cover_art_srv_cb.pixels = NULL;
    }
}

static esp_err_t avrc_cover_art_srv_decode_image(void)
{
    esp_err_t ret = ESP_OK;

    avrc_cover_art_srv_free_pixels();

    /* Allocate pixel memory. Each line is an array of IMAGE_W 16-bit pixels; the `s_avrc_cover_art_srv_cb.pixels` array itself contains pointers to these lines. */
    s_avrc_cover_art_srv_cb.pixels = calloc(IMAGE_H * IMAGE_W, sizeof(uint16_t));

    ESP_GOTO_ON_FALSE(s_avrc_cover_art_srv_cb.pixels, ESP_ERR_NO_MEM, err, RC_CA_SRV_TAG, "Error allocating memory for lines");

    /* JPEG decode config */
    esp_jpeg_image_cfg_t jpeg_cfg = {
        .indata = (uint8_t *)s_avrc_cover_art_srv_cb.image_data,
        .indata_size = s_avrc_cover_art_srv_cb.image_size,
        .outbuf = (uint8_t*)(s_avrc_cover_art_srv_cb.pixels),
        .outbuf_size = IMAGE_W * IMAGE_H * sizeof(uint16_t),
        .out_format = JPEG_IMAGE_FORMAT_RGB565,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags = {
            .swap_color_bytes = 1,
        }
    };

    /* JPEG decode */
    esp_jpeg_image_output_t outimg;
    ret = esp_jpeg_decode(&jpeg_cfg, &outimg);

    ESP_LOGI(RC_CA_SRV_TAG, "JPEG image decoded! Size of the decoded image is: %dpx x %dpx.", outimg.width, outimg.height);

    return ret;
err:
    /* Something went wrong! Exit cleanly, de-allocating everything we allocated. */
    avrc_cover_art_srv_free_pixels();
    return ret;
}

static void avrc_cover_art_srv_display_image(void)
{
    if (s_avrc_cover_art_srv_cb.panel_handle && s_avrc_cover_art_srv_cb.pixels) {
        esp_lcd_panel_draw_bitmap(s_avrc_cover_art_srv_cb.panel_handle, 0, 0, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, s_avrc_cover_art_srv_cb.pixels);
    }
}

/********************************
 * EXTERNAL FUNCTION DEFINITIONS
 *******************************/

void avrc_cover_art_srv_open(void)
{
    memset(&s_avrc_cover_art_srv_cb, 0, sizeof(avrc_cover_art_srv_cb_t));
    /* initialize the display */
    avrc_cover_art_srv_init_display();
}

void avrc_cover_art_srv_close(void)
{
    /* deinitialize the display */
    avrc_cover_art_srv_deinit_display();

    avrc_cover_art_srv_free_image_data();
    avrc_cover_art_srv_free_pixels();

    memset(&s_avrc_cover_art_srv_cb, 0, sizeof(avrc_cover_art_srv_cb_t));
}

void avrc_cover_art_srv_connect(uint16_t mtu)
{
    if (!s_avrc_cover_art_srv_cb.connected) {
        ESP_LOGW(RC_CA_SRV_TAG, "Start cover art connection...");
        /* start the cover art connection */
        esp_avrc_ct_cover_art_connect(mtu);
    }
}

void avrc_cover_art_srv_set_image_final(bool final)
{
    s_avrc_cover_art_srv_cb.image_final = final;
    if (s_avrc_cover_art_srv_cb.image_final) {
        ESP_LOGI(RC_CA_SRV_TAG, "Cover Art Client final data event, image size: %lu bytes", s_avrc_cover_art_srv_cb.image_size);

        /* decode and display the image */
        avrc_cover_art_srv_decode_image();
        /* display the image */
        avrc_cover_art_srv_display_image();
        /* set the getting state to false, we can get next image now */
        s_avrc_cover_art_srv_cb.getting = false;
    }
}

void avrc_cover_art_srv_set_connected(bool connected)
{
    s_avrc_cover_art_srv_cb.connected = connected;
}

void avrc_cover_art_srv_ca_req(void)
{
    /* request cover art */
    if (s_avrc_cover_art_srv_cb.connected) {
        uint8_t attr_mask = ESP_AVRC_MD_ATTR_COVER_ART;

        esp_avrc_ct_send_metadata_cmd(bt_avrc_common_alloc_tl(), attr_mask);
    }
}

void avrc_cover_art_srv_save_image_data(uint8_t *p_data, uint16_t data_len)
{
    s_avrc_cover_art_srv_cb.image_size += data_len;

    uint8_t *p_buf = (uint8_t *)realloc(s_avrc_cover_art_srv_cb.image_data, s_avrc_cover_art_srv_cb.image_size * sizeof(uint8_t));
    if (!p_buf) {
        ESP_LOGE(RC_CA_SRV_TAG, "%s: The memory allocation of Cover art image data failed", __func__);
        avrc_cover_art_srv_free_image_data();
        return;
    }
    s_avrc_cover_art_srv_cb.image_data = p_buf;
    memcpy(s_avrc_cover_art_srv_cb.image_data + s_avrc_cover_art_srv_cb.image_size - data_len, p_data, data_len);
}

void avrc_cover_art_srv_ct_metadata_update(uint8_t *image_handle, int len)
{
    if (s_avrc_cover_art_srv_cb.connected && !s_avrc_cover_art_srv_cb.getting) {
        /* check image handle is valid and different with last one, we don't want to get an image repeatedly */
        if (avrc_cover_art_srv_image_handle_check(image_handle, len)) {
            /* free the previous image data */
            avrc_cover_art_srv_free_image_data();
            /* get the linked thumbnail */
            esp_avrc_ct_cover_art_get_linked_thumbnail(image_handle);
            s_avrc_cover_art_srv_cb.getting = true;
        }
    }
}
