# esp_lcd Driver Design

## Class Diagram

`esp_lcd` driver focuses on two parts: panel driver and IO driver. The panel driver is a bunch of operations on the **frame-buffer**, no matter where the frame-buffer is located. The IO driver is mainly consumed by the controller-based LCD panel drivers (e.g. ST7789). Usually such LCD controller can support various IO interfaces (e.g. I80, SPI, I2C, etc). So we define an abstract interface for the IO driver.

```mermaid
classDiagram
    class esp_lcd_panel_t {
        <<interface>>
        +reset() esp_err_t
        +init() esp_err_t
        +draw_bitmap(int x_start, int y_start, int x_end, int y_end, const void *color_data) esp_err_t
        +mirror(bool x_axis, bool y_axis) esp_err_t
        +swap_xy(bool swap_axes) esp_err_t
        +set_gap(int x_gap, int y_gap) esp_err_t
        +invert_color(bool invert_color_data) esp_err_t
        +disp_on_off(bool on_off) esp_err_t
    }

    esp_lcd_rgb_panel_t --|> esp_lcd_panel_t : Inheritance
    class esp_lcd_rgb_panel_t {
        -int panel_id
        -size_t data_width
        -int disp_gpio
        -intr_handle_t intr
        -uint8_t* frame_buffer
        -gdma_channel_handle_t gdma_channel
        -dma_descriptor_t* dma_nodes
        -on_vsync(void* user_data) bool
    }

    esp_lcd_panel_model_t --|> esp_lcd_panel_t : Inheritance
    esp_lcd_panel_model_t "1" --> "1" esp_lcd_panel_io_t : Use
    class esp_lcd_panel_model_t {
        -esp_lcd_panel_io_t* io
        -int reset_gpio_num
    }

    class esp_lcd_panel_io_t {
        <<interface>>
        +rx_param(int lcd_cmd, void *param, size_t param_size)
        +tx_param(int lcd_cmd, const void *param, size_t param_size)
        +tx_color(int lcd_cmd, const void *color, size_t color_size)
    }

    esp_lcd_panel_io_i2c_t --|> esp_lcd_panel_io_t : Inheritance
    class esp_lcd_panel_io_i2c_t {
        -int i2c_bus_id
        -int ctrl_phase_cmd
        -int ctrl_phase_data
        -on_color_trans_done(void* user_data) bool
    }

    esp_lcd_panel_io_spi_t --|> esp_lcd_panel_io_t : Inheritance
    class esp_lcd_panel_io_spi_t {
        -spi_device_handle_t spi_dev
        -int dc_gpio_num
        -spi_transaction_t trans_worker
        -on_color_trans_done(void* user_data) bool
    }

    esp_lcd_panel_io_i80_t --|> esp_lcd_panel_io_t : Inheritance
    class esp_lcd_panel_io_i80_t {
        -esp_lcd_i80_bus_t* bus
        -int cs_gpio_num
        -int dc_level
        -size_t pclk_hz
        -QueueHandle_t trans_queue
        -QueueHandle_t done_queue
        -on_color_trans_done(void* user_data) bool
    }

    esp_lcd_i80_bus_t "1" --> "1..*" esp_lcd_panel_io_i80_t : Has
    class esp_lcd_i80_bus_t {
        -int bus_id
        -size_t data_width
        -intr_handle_t intr
        -gdma_cannel_handle_t dma_chan
        -dma_descriptor_t* dma_nodes
        -list_t i80_devices
    }
```
