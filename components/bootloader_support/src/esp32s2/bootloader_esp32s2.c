// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
#include <stdint.h>
#include "sdkconfig.h"
#include "bootloader_common.h"
#include "soc/efuse_reg.h"
#include "soc/gpio_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "esp32s2/rom/efuse.h"
#include "esp32s2/rom/gpio.h"
#include "esp32s2/rom/spi_flash.h"

#include "bootloader_init.h"
#include "bootloader_clock.h"
#include "bootloader_flash_config.h"
#include "bootloader_mem.h"
#include "bootloader_flash_priv.h"

#include "esp32s2/rom/cache.h"
#include "esp32s2/rom/ets_sys.h"
#include "esp32s2/rom/spi_flash.h"
#include "esp32s2/rom/rtc.h"
#include "esp32s2/rom/uart.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_image_format.h"
#include "flash_qio_mode.h"
#include "soc/assist_debug_reg.h"
#include "soc/cpu.h"
#include "soc/dport_reg.h"
#include "soc/extmem_reg.h"
#include "soc/rtc.h"
#include "soc/spi_periph.h"
#include <string.h>

static const char *TAG = "boot.esp32s2";
void bootloader_configure_spi_pins(int drv)
{
    const uint32_t spiconfig = ets_efuse_get_spiconfig();
    uint8_t wp_pin = ets_efuse_get_wp_pad();
    uint8_t clk_gpio_num = SPI_CLK_GPIO_NUM;
    uint8_t q_gpio_num   = SPI_Q_GPIO_NUM;
    uint8_t d_gpio_num   = SPI_D_GPIO_NUM;
    uint8_t cs0_gpio_num = SPI_CS0_GPIO_NUM;
    uint8_t hd_gpio_num  = SPI_HD_GPIO_NUM;
    uint8_t wp_gpio_num  = SPI_WP_GPIO_NUM;
    if (spiconfig == 0) {

    } else {
        clk_gpio_num = spiconfig         & 0x3f;
        q_gpio_num = (spiconfig >> 6)    & 0x3f;
        d_gpio_num = (spiconfig >> 12)   & 0x3f;
        cs0_gpio_num = (spiconfig >> 18) & 0x3f;
        hd_gpio_num = (spiconfig >> 24)  & 0x3f;
        wp_gpio_num = wp_pin;
    }
    gpio_pad_set_drv(clk_gpio_num, drv);
    gpio_pad_set_drv(q_gpio_num,   drv);
    gpio_pad_set_drv(d_gpio_num,   drv);
    gpio_pad_set_drv(cs0_gpio_num, drv);
    if (hd_gpio_num <= MAX_PAD_GPIO_NUM) {
        gpio_pad_set_drv(hd_gpio_num, drv);
    }
    if (wp_gpio_num <= MAX_PAD_GPIO_NUM) {
        gpio_pad_set_drv(wp_gpio_num, drv);
    }
}

static void bootloader_reset_mmu(void)
{
    //ToDo: save the autoload value
    Cache_Suspend_ICache();
    Cache_Invalidate_ICache_All();
    Cache_MMU_Init();

    /* normal ROM boot exits with DROM0 cache unmasked,
    but serial bootloader exits with it masked. */
    REG_CLR_BIT(EXTMEM_PRO_ICACHE_CTRL1_REG, EXTMEM_PRO_ICACHE_MASK_DROM0);
}

static void update_flash_config(const esp_image_header_t *bootloader_hdr)
{
    uint32_t size;
    switch (bootloader_hdr->spi_size) {
    case ESP_IMAGE_FLASH_SIZE_1MB:
        size = 1;
        break;
    case ESP_IMAGE_FLASH_SIZE_2MB:
        size = 2;
        break;
    case ESP_IMAGE_FLASH_SIZE_4MB:
        size = 4;
        break;
    case ESP_IMAGE_FLASH_SIZE_8MB:
        size = 8;
        break;
    case ESP_IMAGE_FLASH_SIZE_16MB:
        size = 16;
        break;
    default:
        size = 2;
    }
    uint32_t autoload = Cache_Suspend_ICache();
    // Set flash chip size
    esp_rom_spiflash_config_param(g_rom_flashchip.device_id, size * 0x100000, 0x10000, 0x1000, 0x100, 0xffff);
    // TODO: set mode
    // TODO: set frequency
    Cache_Resume_ICache(autoload);
}

static void print_flash_info(const esp_image_header_t *bootloader_hdr)
{
    ESP_LOGD(TAG, "magic %02x", bootloader_hdr->magic);
    ESP_LOGD(TAG, "segments %02x", bootloader_hdr->segment_count);
    ESP_LOGD(TAG, "spi_mode %02x", bootloader_hdr->spi_mode);
    ESP_LOGD(TAG, "spi_speed %02x", bootloader_hdr->spi_speed);
    ESP_LOGD(TAG, "spi_size %02x", bootloader_hdr->spi_size);

    const char *str;
    switch (bootloader_hdr->spi_speed) {
    case ESP_IMAGE_SPI_SPEED_40M:
        str = "40MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_26M:
        str = "26.7MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_20M:
        str = "20MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_80M:
        str = "80MHz";
        break;
    default:
        str = "20MHz";
        break;
    }
    ESP_LOGI(TAG, "SPI Speed      : %s", str);

    /* SPI mode could have been set to QIO during boot already,
       so test the SPI registers not the flash header */
    uint32_t spi_ctrl = REG_READ(SPI_MEM_CTRL_REG(0));
    if (spi_ctrl & SPI_MEM_FREAD_QIO) {
        str = "QIO";
    } else if (spi_ctrl & SPI_MEM_FREAD_QUAD) {
        str = "QOUT";
    } else if (spi_ctrl & SPI_MEM_FREAD_DIO) {
        str = "DIO";
    } else if (spi_ctrl & SPI_MEM_FREAD_DUAL) {
        str = "DOUT";
    } else if (spi_ctrl & SPI_MEM_FASTRD_MODE) {
        str = "FAST READ";
    } else {
        str = "SLOW READ";
    }
    ESP_LOGI(TAG, "SPI Mode       : %s", str);

    switch (bootloader_hdr->spi_size) {
    case ESP_IMAGE_FLASH_SIZE_1MB:
        str = "1MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_2MB:
        str = "2MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_4MB:
        str = "4MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_8MB:
        str = "8MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_16MB:
        str = "16MB";
        break;
    default:
        str = "2MB";
        break;
    }
    ESP_LOGI(TAG, "SPI Flash Size : %s", str);
}

static void IRAM_ATTR bootloader_init_flash_configure(void)
{
    bootloader_flash_dummy_config(&bootloader_image_hdr);
    bootloader_flash_cs_timing_config();
}

static esp_err_t bootloader_init_spi_flash(void)
{
    bootloader_init_flash_configure();
#ifndef CONFIG_SPI_FLASH_ROM_DRIVER_PATCH
    const uint32_t spiconfig = ets_efuse_get_spiconfig();
    if (spiconfig != EFUSE_SPICONFIG_SPI_DEFAULTS && spiconfig != EFUSE_SPICONFIG_HSPI_DEFAULTS) {
        ESP_LOGE(TAG, "SPI flash pins are overridden. Enable CONFIG_SPI_FLASH_ROM_DRIVER_PATCH in menuconfig");
        return ESP_FAIL;
    }
#endif

    bootloader_flash_unlock();

#if CONFIG_ESPTOOLPY_FLASHMODE_QIO || CONFIG_ESPTOOLPY_FLASHMODE_QOUT
    bootloader_enable_qio_mode();
#endif

    print_flash_info(&bootloader_image_hdr);
    update_flash_config(&bootloader_image_hdr);
    //ensure the flash is write-protected
    bootloader_enable_wp();
    return ESP_OK;
}

static void bootloader_init_uart_console(void)
{
#if CONFIG_ESP_CONSOLE_UART_NONE
    ets_install_putc1(NULL);
    ets_install_putc2(NULL);
#else // CONFIG_ESP_CONSOLE_UART_NONE
    const int uart_num = CONFIG_ESP_CONSOLE_UART_NUM;

    uartAttach(NULL);
    ets_install_uart_printf();

    // Wait for UART FIFO to be empty.
    uart_tx_wait_idle(0);

#if CONFIG_ESP_CONSOLE_UART_CUSTOM
    // Some constants to make the following code less upper-case
    const int uart_tx_gpio = CONFIG_ESP_CONSOLE_UART_TX_GPIO;
    const int uart_rx_gpio = CONFIG_ESP_CONSOLE_UART_RX_GPIO;
    // Switch to the new UART (this just changes UART number used for
    // ets_printf in ROM code).
    uart_tx_switch(uart_num);
    // If console is attached to UART1 or if non-default pins are used,
    // need to reconfigure pins using GPIO matrix
    if (uart_num != 0 || uart_tx_gpio != 43 || uart_rx_gpio != 44) {
        // Change pin mode UART to GPIO
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_GPIO44);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_GPIO43);
        // Route GPIO signals to/from pins
        // (arrays should be optimized away by the compiler)
        const uint32_t tx_idx_list[2] = {U0TXD_OUT_IDX, U1TXD_OUT_IDX};
        const uint32_t rx_idx_list[2] = {U0RXD_IN_IDX, U1RXD_IN_IDX};
        const uint32_t uart_reset[2] = {DPORT_UART_RST, DPORT_UART1_RST};
        const uint32_t tx_idx = tx_idx_list[uart_num];
        const uint32_t rx_idx = rx_idx_list[uart_num];

        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[uart_rx_gpio], PIN_FUNC_GPIO);
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[uart_rx_gpio]);
        gpio_pad_pullup(uart_rx_gpio);

        gpio_matrix_out(uart_tx_gpio, tx_idx, 0, 0);
        gpio_matrix_in(uart_rx_gpio, rx_idx, 0);
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[uart_tx_gpio], PIN_FUNC_GPIO);

        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, uart_reset[uart_num]);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, uart_reset[uart_num]);
    }
#endif // CONFIG_ESP_CONSOLE_UART_CUSTOM

    // Set configured UART console baud rate
    const int uart_baud = CONFIG_ESP_CONSOLE_UART_BAUDRATE;
    uart_div_modify(uart_num, (rtc_clk_apb_freq_get() << 4) / uart_baud);

#endif // CONFIG_ESP_CONSOLE_UART_NONE
}

static void wdt_reset_cpu0_info_enable(void)
{
    DPORT_REG_SET_BIT(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_ASSIST_DEBUG);
    DPORT_REG_CLR_BIT(DPORT_PERI_RST_EN_REG, DPORT_PERI_EN_ASSIST_DEBUG);
    REG_WRITE(ASSIST_DEBUG_PRO_PDEBUGENABLE, 1);
    REG_WRITE(ASSIST_DEBUG_PRO_RCD_RECORDING, 1);
}

static void wdt_reset_info_dump(int cpu)
{
    uint32_t inst = 0, pid = 0, stat = 0, data = 0, pc = 0,
             lsstat = 0, lsaddr = 0, lsdata = 0, dstat = 0;
    const char *cpu_name = cpu ? "APP" : "PRO";

    stat = 0xdeadbeef;
    pid = 0;
    inst = REG_READ(ASSIST_DEBUG_PRO_RCD_PDEBUGINST);
    dstat = REG_READ(ASSIST_DEBUG_PRO_RCD_PDEBUGSTATUS);
    data = REG_READ(ASSIST_DEBUG_PRO_RCD_PDEBUGDATA);
    pc = REG_READ(ASSIST_DEBUG_PRO_RCD_PDEBUGPC);
    lsstat = REG_READ(ASSIST_DEBUG_PRO_RCD_PDEBUGLS0STAT);
    lsaddr = REG_READ(ASSIST_DEBUG_PRO_RCD_PDEBUGLS0ADDR);
    lsdata = REG_READ(ASSIST_DEBUG_PRO_RCD_PDEBUGLS0DATA);

    if (DPORT_RECORD_PDEBUGINST_SZ(inst) == 0 &&
            DPORT_RECORD_PDEBUGSTATUS_BBCAUSE(dstat) == DPORT_RECORD_PDEBUGSTATUS_BBCAUSE_WAITI) {
        ESP_LOGW(TAG, "WDT reset info: %s CPU PC=0x%x (waiti mode)", cpu_name, pc);
    } else {
        ESP_LOGW(TAG, "WDT reset info: %s CPU PC=0x%x", cpu_name, pc);
    }
    ESP_LOGD(TAG, "WDT reset info: %s CPU STATUS        0x%08x", cpu_name, stat);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PID           0x%08x", cpu_name, pid);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGINST    0x%08x", cpu_name, inst);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGSTATUS  0x%08x", cpu_name, dstat);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGDATA    0x%08x", cpu_name, data);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGPC      0x%08x", cpu_name, pc);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGLS0STAT 0x%08x", cpu_name, lsstat);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGLS0ADDR 0x%08x", cpu_name, lsaddr);
    ESP_LOGD(TAG, "WDT reset info: %s CPU PDEBUGLS0DATA 0x%08x", cpu_name, lsdata);
}

static void bootloader_check_wdt_reset(void)
{
    int wdt_rst = 0;
    RESET_REASON rst_reas[2];

    rst_reas[0] = rtc_get_reset_reason(0);
    if (rst_reas[0] == RTCWDT_SYS_RESET || rst_reas[0] == TG0WDT_SYS_RESET || rst_reas[0] == TG1WDT_SYS_RESET ||
        rst_reas[0] == TG0WDT_CPU_RESET || rst_reas[0] == TG1WDT_CPU_RESET || rst_reas[0] == RTCWDT_CPU_RESET) {
        ESP_LOGW(TAG, "PRO CPU has been reset by WDT.");
        wdt_rst = 1;
    }
    if (wdt_rst) {
        // if reset by WDT dump info from trace port
        wdt_reset_info_dump(0);
    }
    wdt_reset_cpu0_info_enable();
}

void abort(void)
{
#if !CONFIG_ESP_SYSTEM_PANIC_SILENT_REBOOT
    ets_printf("abort() was called at PC 0x%08x\r\n", (intptr_t)__builtin_return_address(0) - 3);
#endif
    if (esp_cpu_in_ocd_debug_mode()) {
        __asm__("break 0,0");
    }
    while (1) {
    }
}

static void bootloader_super_wdt_auto_feed(void)
{
    REG_SET_BIT(RTC_CNTL_SWD_CONF_REG, RTC_CNTL_SWD_AUTO_FEED_EN);
}

esp_err_t bootloader_init(void)
{
    esp_err_t ret = ESP_OK;
    bootloader_super_wdt_auto_feed();
    // protect memory region

    bootloader_init_mem();

    /* check that static RAM is after the stack */
#ifndef NDEBUG
    {
        assert(&_bss_start <= &_bss_end);
        assert(&_data_start <= &_data_end);
    }
#endif
    // clear bss section
    bootloader_clear_bss_section();
    // reset MMU
    bootloader_reset_mmu();
    // config clock
    bootloader_clock_configure();
    // initialize uart console, from now on, we can use esp_log
    bootloader_init_uart_console();
    /* print 2nd bootloader banner */
    bootloader_print_banner();
    // update flash ID
    bootloader_flash_update_id();
    // Check and run XMC startup flow
    if ((ret = bootloader_flash_xmc_startup()) != ESP_OK) {
        ESP_LOGE(TAG, "failed when running XMC startup flow, reboot!");
        goto err;
    }
    // read bootloader header
    if ((ret = bootloader_read_bootloader_header()) != ESP_OK) {
        goto err;
    }
    // read chip revision and check if it's compatible to bootloader
    if ((ret = bootloader_check_bootloader_validity()) != ESP_OK) {
        goto err;
    }
    // initialize spi flash
    if ((ret = bootloader_init_spi_flash()) != ESP_OK) {
        goto err;
    }
    // check whether a WDT reset happend
    bootloader_check_wdt_reset();
    // config WDT
    bootloader_config_wdt();
    // enable RNG early entropy source
    bootloader_enable_random();
err:
    return ret;
}
