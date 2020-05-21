

#include "slip_modem.h"


#include "esp_netif.h"
#include "esp_netif_slip.h"
#include "esp_event.h"
#include "esp_log.h"


#include "lwip/opt.h"
#include "lwip/sio.h"
#include "lwip/ip.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"

#include "esp_netif_slip.h"


#define SLIP_RX_TASK_PRIORITY   10
#define SLIP_RX_TASK_STACK_SIZE (4 * 1024)


static const char *TAG = "esp-slip_modem";


// UART container object
typedef struct {
    // UART device number for SIO use
    uart_port_t uart_dev;

    // UART baud rate for configuration
    uint32_t uart_baud;

    // UART TX pin for configuration
    int uart_tx_pin;

    // UART RX pin for configuration
    int uart_rx_pin;

    // QueueHandle for uart driver
    QueueHandle_t uart_queue;

    // TaskHandle for receive task
    TaskHandle_t uart_rx_task;
} esp_slip_uart_t;


// Modem object, implements glue logic for slip_driver and esp_netif
struct esp_slip_modem {
    // ESP base netif driver
    esp_netif_driver_base_t base;

    // LWIP slip context
    lwip_slip_ctx_t         *slip_driver;

    // Uart for use with slip
    esp_slip_uart_t uart;

    // Buffer for incoming messages
    uint8_t *buffer;
    uint32_t buffer_len;

    // Filter callbacks for application-specific slip message handling
    slip_rx_filter_cb_t *rx_filter;
    void *rx_filter_ctx;

    // Running flag
    bool running;
};


// Forward function definitions
static void esp_slip_modem_uart_rx_task(void *arg);
static esp_err_t esp_slip_modem_post_attach(esp_netif_t *esp_netif, void *args);


// TODO: netif internal functions required for driver operation
esp_err_t esp_netif_start_slip(lwip_slip_ctx_t *slip_ctx);
esp_err_t esp_netif_stop_slip(lwip_slip_ctx_t *slip_ctx);

void esp_netif_lwip_slip_output(lwip_slip_ctx_t *slip_ctx, void *buffer, size_t len);
void esp_netif_lwip_slip_input(void *ctx, void *buffer, size_t len, void *eb);


// Create a new slip netif
void *esp_slip_modem_create(esp_netif_t *slip_netif, esp_slip_modem_config_t *modem_config)
{
    ESP_LOGI(TAG, "%s: Creating slip modem (netif: %p)", __func__, slip_netif);

    // Fetch lwip slip ctx object
    // TODO: is the the best / a reasonable approach?
    lwip_slip_ctx_t *slip_ctx = esp_netif_lwip_slip_get_ctx(slip_netif);

    ESP_LOGD(TAG, "%s (netif: %p)", __func__, slip_netif);

    esp_slip_modem_t *slip_modem = calloc(1, sizeof(esp_slip_modem_t));
    if (!slip_modem) {
        ESP_LOGE(TAG, "create netif glue failed");
        return NULL;
    }

    // Attach driver and post_attach callbacks
    slip_modem->slip_driver = slip_ctx;
    slip_modem->base.post_attach = esp_slip_modem_post_attach;

    // Attach config
    slip_modem->buffer_len = modem_config->rx_buffer_len;

    slip_modem->rx_filter = modem_config->rx_filter;
    slip_modem->rx_filter_ctx = modem_config->rx_filter_ctx;

    slip_modem->uart.uart_dev = modem_config->uart_dev;
    slip_modem->uart.uart_baud = modem_config->uart_baud;
    slip_modem->uart.uart_rx_pin = modem_config->uart_rx_pin;
    slip_modem->uart.uart_tx_pin = modem_config->uart_tx_pin;

    // Return new modem, with a cast to the first item
    return &slip_modem->base;
}

// Internal handler called on driver start
static esp_err_t esp_slip_driver_start(esp_slip_modem_t *slip_modem)
{
    ESP_LOGD(TAG, "%s: Starting SLIP modem (modem %p)", __func__, slip_modem);

    // Allocate RX buffer if one does not exist
    if (slip_modem->buffer == NULL) {
        slip_modem->buffer = malloc(slip_modem->buffer_len);
    }
    if (slip_modem->buffer == NULL) {
        ESP_LOGE(TAG, "error allocating rx buffer");
        return ESP_ERR_NO_MEM;
    }

    // Then, initialise UART

    // Build configuration
    uart_config_t uart_config = {
        .baud_rate = slip_modem->uart.uart_baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    // Initialise uart
    ESP_ERROR_CHECK(uart_param_config(slip_modem->uart.uart_dev, &uart_config));

    // Set UART pins
    ESP_ERROR_CHECK(uart_set_pin(slip_modem->uart.uart_dev, slip_modem->uart.uart_tx_pin, slip_modem->uart.uart_rx_pin, 0, 0));

    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(slip_modem->uart.uart_dev, slip_modem->buffer_len, slip_modem->buffer_len, 10, &slip_modem->uart.uart_queue, 0));

    // Start slip RX task
    slip_modem->running = true;
    xTaskCreate(esp_slip_modem_uart_rx_task, "slip_modem_uart_rx_task", SLIP_RX_TASK_STACK_SIZE, slip_modem, SLIP_RX_TASK_PRIORITY, &slip_modem->uart.uart_rx_task);

    // Finally, initialise slip network interface
    esp_netif_start_slip(slip_modem->slip_driver);

    return ESP_OK;
}


esp_err_t esp_slip_modem_destroy(esp_slip_modem_t *slip_modem)
{
    // Stop slip driver
    esp_netif_stop_slip(slip_modem->slip_driver);

    // Stop uart rx task
    vTaskDelete(slip_modem->uart.uart_rx_task);

    // Delete driver
    uart_driver_delete(slip_modem->uart.uart_dev);

    // Free slip interface
    free(slip_modem);

    return ESP_OK;
}

// Modem transmit for glue logic
esp_err_t esp_slip_modem_transmit(void *slip_driver, void *buffer, size_t len)
{
    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_DEBUG);

    lwip_slip_ctx_t *slip_ctx = (lwip_slip_ctx_t *) slip_driver;

    esp_netif_lwip_slip_output(slip_ctx, buffer, len);

    return ESP_OK;
}

// Modem receive for glue logic
void esp_slip_modem_receive(esp_netif_t *esp_netif, void *buffer, size_t len)
{
    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_DEBUG);

    esp_netif_receive(esp_netif, buffer, len, NULL);
}

// Post-attach handler for netif
static esp_err_t esp_slip_modem_post_attach(esp_netif_t *esp_netif, void *args)
{
    esp_slip_modem_t *slip_modem = (esp_slip_modem_t *) args;

    ESP_LOGD(TAG, "%s (netif: %p args: %p)", __func__, esp_netif, args);

    const esp_netif_driver_ifconfig_t driver_ifconfig = {
        .driver_free_rx_buffer = NULL,
        .transmit = esp_slip_modem_transmit,
        .handle = slip_modem->slip_driver,
    };

    slip_modem->base.netif = esp_netif;
    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));

    esp_slip_driver_start(slip_modem);

    return ESP_OK;
}

esp_err_t esp_slip_modem_set_default_handlers(esp_netif_t *esp_netif)
{
    esp_err_t ret;

    if (esp_netif == NULL) {
        ESP_LOGE(TAG, "esp-netif handle can't be null");
        return ESP_ERR_INVALID_ARG;
    }

    ret = esp_event_handler_register(SLIP_EVENT, SLIP_EVENT_START, esp_netif_action_start, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_register(SLIP_EVENT, SLIP_EVENT_STOP, esp_netif_action_stop, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

fail:
    esp_eth_clear_default_handlers(esp_netif);
    return ret;

    return ESP_OK;
}

esp_err_t esp_slip_modem_clear_default_handlers(void *esp_netif)
{
    if (!esp_netif) {
        ESP_LOGE(TAG, "esp-netif handle can't be null");
        return ESP_ERR_INVALID_ARG;
    }
    esp_event_handler_unregister(SLIP_EVENT, SLIP_EVENT_START, esp_netif_action_start);
    esp_event_handler_unregister(SLIP_EVENT, SLIP_EVENT_STOP, esp_netif_action_stop);

    return ESP_OK;
}



static void esp_slip_modem_uart_rx_task(void *arg)
{
    esp_slip_modem_t *slip_modem = (esp_slip_modem_t *) arg;

    ESP_LOGD(TAG, "Start SLIP modem RX task (slip_modem %p slip_ctx %p filter: %p)", slip_modem, slip_modem->slip_driver, slip_modem->rx_filter);
    ESP_LOGD(TAG, "Uart: %d, buffer: %p (%d bytes)", slip_modem->uart.uart_dev, slip_modem->buffer, slip_modem->buffer_len);


    while (slip_modem->running == true) {
        // Read data from the UART
        int len = uart_read_bytes(slip_modem->uart.uart_dev, slip_modem->buffer, slip_modem->buffer_len, 1 / portTICK_RATE_MS);

        if (len > 0) {

            // Log slip RX data
            ESP_LOGD(TAG, "rx %d bytes", len);
            ESP_LOG_BUFFER_HEX_LEVEL(TAG, slip_modem->buffer, len, ESP_LOG_DEBUG);

            // Ensure null termination
            slip_modem->buffer[len] = '\0';

            // Filter if provided
            if ((slip_modem->rx_filter != NULL) && slip_modem->rx_filter(slip_modem->rx_filter_ctx, slip_modem->buffer, len)) {
                continue;
            }

            // Pass received bytes in to slip interface
            esp_netif_lwip_slip_input(slip_modem->slip_driver, slip_modem->buffer, len, NULL);
        }

        // Yeild to allow other tasks to progress
        vTaskDelay(1 * portTICK_PERIOD_MS);
    }
}

