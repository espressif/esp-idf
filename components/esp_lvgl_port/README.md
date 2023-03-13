# LVGL ESP Portation

[![Component Registry](https://components.espressif.com/components/espressif/esp_lvgl_port/badge.svg)](https://components.espressif.com/components/espressif/esp_lvgl_port)

This component helps with using LVGL with Espressif's LCD and touch drivers. It can be used with any project with LCD display. 

## Features
* Initialization of the LVGL
    * Create task and timer
    * Handle rotating
* Add/remove display (using [`esp_lcd`](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd.html))
* Add/remove touch input (using [`esp_lcd_touch`](https://github.com/espressif/esp-bsp/tree/master/components/lcd_touch))

## Usage

### Initialization
``` c
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    esp_err_t err = lvgl_port_init(&lvgl_cfg);
```

### Add screen

Add an LCD screen to the LVGL. It can be called multiple times for adding multiple LCD screens. 

This part is necessary only in IDF 5.0 and older:

``` c
    static lv_disp_t * disp;
    /* The component calls esp_lcd_panel_draw_bitmap API for send data to the screen. There must be called 
    lvgl_port_flush_ready(disp) after each transaction to display. The best way is to use on_color_trans_done 
    callback from esp_lcd IO config structure. */
    static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
    {
        lv_disp_t ** disp = (lv_disp_t **)user_ctx;
        lvgl_port_flush_ready((*disp)->driver);
        return false;
    }
    
    /* LCD IO */
    esp_lcd_panel_io_spi_config_t io_config =
	{
		...
		.on_color_trans_done = notify_lvgl_flush_ready,
		.user_ctx = &disp
	};
	esp_lcd_panel_io_handle_t io_handle = NULL;
	ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t) 1, &io_config, &io_handle));

    /* LCD driver initialization */
    ...
    esp_lcd_panel_handle_t lcd_panel_handle;
    esp_err_t err = esp_lcd_new_panel_st7789(io_handle, &panel_config, &lcd_panel_handle);
```

Main part of the code (IDF version independent):

``` c
    /* Add LCD screen */
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = lcd_panel_handle,
        .buffer_size = DISP_WIDTH*DISP_HEIGHT,
        .double_buffer = true,
        .hres = DISP_WIDTH,
        .vres = DISP_HEIGHT,
        .monochrome = false,
        /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
        }
    };
    disp = lvgl_port_add_disp(&disp_cfg);
    
    /* ... the rest of the initialization ... */

    /* If deinitializing LVGL port, remember to delete all displays: */
    lvgl_port_remove_disp(disp);
```

### Add touch input

Add touch input to the LVGL. It can be called more times for adding more touch inputs. 
``` c
    /* Touch driver initialization */
    ...
    esp_lcd_touch_handle_t tp;
    esp_err_t err = esp_lcd_touch_new_i2c_gt911(io_handle, &tp_cfg, &tp);

    /* Add touch input (for selected screen) */
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp_spi,
        .handle = tp,
    };
    lv_indev_t* touch_handle = lvgl_port_add_touch(&touch_cfg);
    
    /* ... the rest of the initialization ... */

    /* If deinitializing LVGL port, remember to delete all touches: */
    lvgl_port_remove_touch(touch_handle);
```

### Add buttons input

Add buttons input to the LVGL. It can be called more times for adding more buttons inputs for different displays. This feature is available only when the component `espressif/button` was added into the project.
``` c
    /* Buttons configuration structure */
    const button_config_t bsp_button_config[] = {
        {
            .type = BUTTON_TYPE_ADC,
            .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
            .adc_button_config.button_index = 0,
            .adc_button_config.min = 2310, // middle is 2410mV
            .adc_button_config.max = 2510
        },
        {
            .type = BUTTON_TYPE_ADC,
            .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
            .adc_button_config.button_index = 1,
            .adc_button_config.min = 1880, // middle is 1980mV
            .adc_button_config.max = 2080
        },
        {
            .type = BUTTON_TYPE_ADC,
            .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
            .adc_button_config.button_index = 2,
            .adc_button_config.min = 720, // middle is 820mV
            .adc_button_config.max = 920
        },
    };

    const lvgl_port_nav_btns_cfg_t btns = {
        .disp = disp_spi,
        .button_prev = &bsp_button_config[0],
        .button_next = &bsp_button_config[1],
        .button_enter = &bsp_button_config[2]
    };

    /* Add buttons input (for selected screen) */
    lv_indev_t* buttons_handle = lvgl_port_add_navigation_buttons(&btns);
    
    /* ... the rest of the initialization ... */

    /* If deinitializing LVGL port, remember to delete all buttons: */
    lvgl_port_remove_navigation_buttons(buttons_handle);
```

**Note:** When you use navigation buttons for control LVGL objects, these objects must be added to LVGL groups. See [LVGL documentation](https://docs.lvgl.io/master/overview/indev.html?highlight=lv_indev_get_act#keypad-and-encoder) for more info.

### LVGL API usage

Every LVGL calls must be protected with these lock/unlock commands:
``` c
	/* Wait for the other task done the screen operation */
    lvgl_port_lock(0);
    ...
    lv_obj_t * screen = lv_disp_get_scr_act(disp);
    lv_obj_t * obj = lv_label_create(screen);
    ...
    /* Screen operation done -> release for the other task */
    lvgl_port_unlock();
```

### Rotating screen
``` c
    lv_disp_set_rotation(disp, LV_DISP_ROT_90);
```

**Note:** During the rotating, the component call [`esp_lcd`](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd.html) API.
