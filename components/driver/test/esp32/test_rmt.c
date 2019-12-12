/**
 * please connect GPIO18 to GPIO19
 */
#include "stdio.h"
#include <string.h>
#include "driver/rmt.h"
#include "unity.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/periph_ctrl.h"
#include "soc/rmt_periph.h"

static const char* TAG = "RMT";


#define RMT_RX_ACTIVE_LEVEL  1   /*!< Data bit is active high for self test mode */
#define RMT_TX_CARRIER_EN    0   /*!< Disable carrier for self test mode  */

#define RMT_TX_CHANNEL    1     /*!< RMT channel for transmitter */
#define RMT_TX_GPIO_NUM  18     /*!< GPIO number for transmitter signal */
#define RMT_RX_CHANNEL    0     /*!< RMT channel for receiver */
#define RMT_RX_GPIO_NUM  19     /*!< GPIO number for receiver */
#define RMT_CLK_DIV      100    /*!< RMT counter clock divider */
#define RMT_TICK_10_US    (80000000/RMT_CLK_DIV/100000)   /*!< RMT counter value for 10 us.(Source clock is APB clock) */

#define HEADER_HIGH_US    9000                         /*!< NEC protocol header: positive 9ms */
#define HEADER_LOW_US     4500                         /*!< NEC protocol header: negative 4.5ms*/
#define BIT_ONE_HIGH_US    560                         /*!< NEC protocol data bit 1: positive 0.56ms */
#define BIT_ONE_LOW_US    (2250-BIT_ONE_HIGH_US)   /*!< NEC protocol data bit 1: negative 1.69ms */
#define BIT_ZERO_HIGH_US   560                         /*!< NEC protocol data bit 0: positive 0.56ms */
#define BIT_ZERO_LOW_US   (1120-BIT_ZERO_HIGH_US)  /*!< NEC protocol data bit 0: negative 0.56ms */
#define BIT_END            560                         /*!< NEC protocol end: positive 0.56ms */
#define BIT_MARGIN         20                          /*!< NEC parse margin time */

#define ITEM_DURATION(d)  ((d & 0x7fff)*10/RMT_TICK_10_US)  /*!< Parse duration time from memory register value */
#define DATA_ITEM_NUM   34  /*!< NEC code item number: header + 32bit data + end */
#define RMT_TX_DATA_NUM  100    /*!< NEC tx test data number */
#define RMT_ITEM32_TIMEOUT_US  9500   /*!< RMT receiver timeout value(us) */

/**
 * @brief Build register value of waveform for NEC one data bit
 */
static inline void fill_item_level(rmt_item32_t* item, int high_us, int low_us)
{
    item->level0 = 1;
    item->duration0 = (high_us) / 10 * RMT_TICK_10_US;
    item->level1 = 0;
    item->duration1 = (low_us) / 10 * RMT_TICK_10_US;
}

/**
 * @brief Generate NEC header value: active 9ms + negative 4.5ms
 */
static void fill_item_header(rmt_item32_t* item)
{
    fill_item_level(item, HEADER_HIGH_US, HEADER_LOW_US);
}

/*
 * @brief Generate NEC data bit 1: positive 0.56ms + negative 1.69ms
 */
static void fill_item_bit_one(rmt_item32_t* item)
{
    fill_item_level(item, BIT_ONE_HIGH_US, BIT_ONE_LOW_US);
}

/**
 * @brief Generate NEC data bit 0: positive 0.56ms + negative 0.56ms
 */
static void fill_item_bit_zero(rmt_item32_t* item)
{
    fill_item_level(item, BIT_ZERO_HIGH_US, BIT_ZERO_LOW_US);
}

/**
 * @brief Generate NEC end signal: positive 0.56ms
 */
static void fill_item_end(rmt_item32_t* item)
{
    fill_item_level(item, BIT_END, 0x7fff);
}

/**
 * @brief Check whether duration is around target_us
 */
static inline bool check_in_range(int duration_ticks, int target_us, int margin_us)
{
    if(( ITEM_DURATION(duration_ticks) < (target_us + margin_us))
        && ( ITEM_DURATION(duration_ticks) > (target_us - margin_us))) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Check whether this value represents an NEC header
 */
static bool header_if(rmt_item32_t* item)
{
    if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
        && check_in_range(item->duration0, HEADER_HIGH_US, BIT_MARGIN)
        && check_in_range(item->duration1, HEADER_LOW_US, BIT_MARGIN)) {
        return true;
    }
    return false;
}

/**
 * @brief Check whether this value represents an NEC data bit 1
 */
static bool bit_one_if(rmt_item32_t* item)
{
    if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
        && check_in_range(item->duration0, BIT_ONE_HIGH_US, BIT_MARGIN)
        && check_in_range(item->duration1, BIT_ONE_LOW_US, BIT_MARGIN)) {
        return true;
    }
    return false;
}

/**
 * @brief Check whether this value represents an NEC data bit 0
 */
static bool bit_zero_if(rmt_item32_t* item)
{
    if((item->level0 == RMT_RX_ACTIVE_LEVEL && item->level1 != RMT_RX_ACTIVE_LEVEL)
        && check_in_range(item->duration0, BIT_ZERO_HIGH_US, BIT_MARGIN)
        && check_in_range(item->duration1, BIT_ZERO_LOW_US, BIT_MARGIN)) {
        return true;
    }
    return false;
}


/**
 * @brief Parse NEC 32 bit waveform to address and command.
 */
static int parse_items(rmt_item32_t* item, int item_num, uint16_t* addr, uint16_t* data)
{
    int w_len = item_num;
    if(w_len < DATA_ITEM_NUM) {
        return -1;
    }
    int i = 0, j = 0;
    if(!header_if(item++)) {
        return -1;
    }
    uint16_t addr_t = 0;
    for(j = 0; j < 16; j++) {
        if(bit_one_if(item)) {
            addr_t |= (1 << j);
        } else if(bit_zero_if(item)) {
            addr_t |= (0 << j);
        } else {
            return -1;
        }
        item++;
        i++;
    }
    uint16_t data_t = 0;
    for(j = 0; j < 16; j++) {
        if(bit_one_if(item)) {
            data_t |= (1 << j);
        } else if(bit_zero_if(item)) {
            data_t |= (0 << j);
        } else {
            return -1;
        }
        item++;
        i++;
    }
    *addr = addr_t;
    *data = data_t;
    return i;
}

/**
 * @brief Build NEC 32bit waveform.
 */
static int build_items(int channel, rmt_item32_t* item, int item_num, uint16_t addr, uint16_t cmd_data)
{
    int i = 0, j = 0;
    if(item_num < DATA_ITEM_NUM) {
        return -1;
    }
    fill_item_header(item++);
    i++;
    for(j = 0; j < 16; j++) {
        if(addr & 0x1) {
            fill_item_bit_one(item);
        } else {
            fill_item_bit_zero(item);
        }
        item++;
        i++;
        addr >>= 1;
    }
    for(j = 0; j < 16; j++) {
        if(cmd_data & 0x1) {
            fill_item_bit_one(item);
        } else {
            fill_item_bit_zero(item);
        }
        item++;
        i++;
        cmd_data >>= 1;
    }
    fill_item_end(item);
    i++;
    return i;
}

static void set_tx_data(int tx_channel, uint16_t cmd, uint16_t addr, int item_num, rmt_item32_t* item, int offset)
{
    while(1) {
        int i = build_items(tx_channel, item + offset, item_num - offset, ((~addr) << 8) | addr, cmd);
        printf("cmd :%d\n", cmd);
        if(i < 0) {
            break;
        }
        cmd++;
        addr++;
        offset += i;
    }
}

static int get_rx_data(RingbufHandle_t rb)
{
    uint16_t tmp = 0;
    while(rb) {
        size_t rx_size = 0;
        rmt_item32_t* rx_item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 1000);
        if(rx_item) {
            uint16_t rmt_addr;
            uint16_t rmt_cmd;
            int rx_offset = 0;
            while(1) {
                int res = parse_items(rx_item + rx_offset, rx_size / 4 - rx_offset, &rmt_addr, &rmt_cmd);
                if(res > 0) {
                    rx_offset += res + 1;
                    ESP_LOGI(TAG, "RMT RCV --- addr: 0x%04x cmd: 0x%04x", rmt_addr, rmt_cmd);
                    TEST_ASSERT(rmt_cmd == tmp);
                    tmp++;
                } else {
                    break;
                }
            }
            vRingbufferReturnItem(rb, (void*) rx_item);
        } else {
            break;
        }
    }
    return tmp;
}

/**
 * @brief RMT transmitter initialization
 */
static void tx_init(void)
{
    // the sender once it send something, its frq is 38kHz, and the duty cycle is 50%
    rmt_tx_config_t tx_cfg = {
        .loop_en = false,
        .carrier_duty_percent = 50,
        .carrier_freq_hz = 38000,
        .carrier_level = 1,
        .carrier_en = RMT_TX_CARRIER_EN,
        .idle_level = 0,
        .idle_output_en = true,
    };
    rmt_config_t rmt_tx = {
        .channel = RMT_TX_CHANNEL,
        .gpio_num = RMT_TX_GPIO_NUM,
        .mem_block_num = 1,
        .clk_div = RMT_CLK_DIV,
        .tx_config = tx_cfg,
        .rmt_mode = 0,
    };
    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, 0);
}

/**
 * @brief RMT receiver initialization
 */
static void rx_init(void)
{
    rmt_rx_config_t rx_cfg = {
        .filter_en = true,
        .filter_ticks_thresh = 100,
        .idle_threshold = RMT_ITEM32_TIMEOUT_US / 10 * (RMT_TICK_10_US),
    };
    rmt_config_t rmt_rx = {
        .channel = RMT_RX_CHANNEL,
        .gpio_num = RMT_RX_GPIO_NUM,
        .clk_div = RMT_CLK_DIV,
        .mem_block_num = 1,
        .rmt_mode = RMT_MODE_RX,
        .rx_config = rx_cfg,
    };
    rmt_config(&rmt_rx);
    rmt_driver_install(rmt_rx.channel, (sizeof(rmt_item32_t) * DATA_ITEM_NUM * (RMT_TX_DATA_NUM+6)), 0);
}

TEST_CASE("RMT init config", "[rmt][test_env=UT_T1_RMT]")
{
    // tx settings
    rmt_tx_config_t tx_cfg = {
        .loop_en = false,
        .carrier_duty_percent = 50,
        .carrier_freq_hz = 38000,
        .carrier_level = 1,
        .carrier_en = RMT_TX_CARRIER_EN,
        .idle_level = 0,
        .idle_output_en = true,
    };
    rmt_config_t rmt_tx = {
        .channel = RMT_TX_CHANNEL,
        .gpio_num = RMT_TX_GPIO_NUM,
        .mem_block_num = 1,
        .clk_div = RMT_CLK_DIV,
        .tx_config = tx_cfg,
    };
    TEST_ESP_OK(rmt_config(&rmt_tx));
    TEST_ESP_OK(rmt_driver_install(rmt_tx.channel, 0, 0));
    TEST_ESP_OK(rmt_driver_uninstall(rmt_tx.channel));

    //rx settings
    rmt_rx_config_t rx_cfg = {
        .filter_en = true,
        .filter_ticks_thresh = 100,
        .idle_threshold = RMT_ITEM32_TIMEOUT_US / 10 * (RMT_TICK_10_US),
    };
    rmt_config_t rmt_rx = {
        .channel = RMT_RX_CHANNEL,
        .gpio_num = RMT_RX_GPIO_NUM,
        .clk_div = RMT_CLK_DIV,
        .mem_block_num = 1,
        .rmt_mode = RMT_MODE_RX,
        .rx_config = rx_cfg,
    };
    TEST_ESP_OK(rmt_config(&rmt_rx));
    TEST_ESP_OK(rmt_driver_install(rmt_rx.channel, 1000, 0));
    TEST_ESP_OK(rmt_driver_uninstall(rmt_rx.channel));

    //error param setting
    rmt_config_t temp_rmt_rx1 = {
        .channel = 2,
        .gpio_num = 15,
        .clk_div = RMT_CLK_DIV,
        .mem_block_num = 1,
        .rmt_mode = RMT_MODE_RX,
        .rx_config = rx_cfg,
    };
    rmt_config_t temp_rmt_rx2 = temp_rmt_rx1;

    temp_rmt_rx2.clk_div = 0; // only invalid parameter to test
    TEST_ASSERT(rmt_config(&temp_rmt_rx2) == ESP_ERR_INVALID_ARG);

    temp_rmt_rx2 = temp_rmt_rx1;
    temp_rmt_rx2.channel = RMT_CHANNEL_MAX;
    TEST_ASSERT(rmt_config(&temp_rmt_rx2) == ESP_ERR_INVALID_ARG);

    temp_rmt_rx2 = temp_rmt_rx1;
    temp_rmt_rx2.channel = 2;
    temp_rmt_rx2.mem_block_num = 8;
    TEST_ASSERT(rmt_config(&temp_rmt_rx2) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("RMT init set function", "[rmt][test_env=UT_T1_RMT]")
{
    rmt_channel_t channel = 7;
    TEST_ESP_OK(rmt_set_pin(channel, RMT_MODE_RX, RMT_RX_GPIO_NUM));
    TEST_ESP_OK(rmt_set_clk_div(channel, RMT_CLK_DIV*2));
    TEST_ESP_OK(rmt_set_mem_block_num(channel, 1));
    TEST_ESP_OK(rmt_set_rx_filter(channel, 1, 100));
    TEST_ESP_OK(rmt_set_rx_idle_thresh(channel, RMT_ITEM32_TIMEOUT_US / 10 * (RMT_TICK_10_US)*2));
    TEST_ESP_OK(rmt_driver_install(channel, 0, 0));
    TEST_ESP_OK(rmt_driver_uninstall(channel));
}

// need to make sure its phenomenon by logic analyzer, can't run in CI
TEST_CASE("RMT clock devider, clock source set(logic analyzer)", "[rmt][ignore]")
{
    uint8_t div_cnt;
    rmt_source_clk_t src_clk;
    rmt_config_t rmt_tx;
    rmt_tx.channel = RMT_TX_CHANNEL;
    rmt_tx.mem_block_num = 1;
    rmt_tx.gpio_num = RMT_TX_GPIO_NUM;
    rmt_tx.clk_div = RMT_CLK_DIV;
    rmt_tx.tx_config.loop_en = true;
    rmt_tx.tx_config.carrier_duty_percent = 50;
    rmt_tx.tx_config.carrier_freq_hz = 38000;
    rmt_tx.tx_config.carrier_level = 1;
    rmt_tx.tx_config.carrier_en = RMT_TX_CARRIER_EN;
    rmt_tx.tx_config.idle_level = 0;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.rmt_mode = RMT_MODE_TX;

    TEST_ESP_OK(rmt_config(&rmt_tx));
    TEST_ESP_OK(rmt_get_clk_div(RMT_TX_CHANNEL, &div_cnt));
    TEST_ESP_OK(rmt_driver_install(rmt_tx.channel, 0, 0));
    TEST_ASSERT_EQUAL_UINT8(div_cnt, RMT_CLK_DIV);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // reset it and check it
    TEST_ESP_OK(rmt_set_clk_div(RMT_TX_CHANNEL, 160));
    TEST_ESP_OK(rmt_get_clk_div(RMT_TX_CHANNEL, &div_cnt));
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    TEST_ESP_OK(rmt_set_source_clk(RMT_TX_CHANNEL, RMT_BASECLK_APB));
    TEST_ESP_OK(rmt_get_source_clk(RMT_TX_CHANNEL, &src_clk));
    TEST_ASSERT_EQUAL_UINT8(div_cnt, 160);
    TEST_ASSERT_EQUAL_INT(src_clk, RMT_BASECLK_APB);
    TEST_ESP_OK(rmt_driver_uninstall(rmt_tx.channel));
}

TEST_CASE("RMT rx set and get properties", "[rmt][test_env=UT_T1_RMT]")
{
    rmt_channel_t channel = RMT_RX_CHANNEL;
    uint8_t memNum;
    uint8_t div_cnt;
    uint16_t idleThreshold;
    rmt_mem_owner_t owner;

    rx_init();

    TEST_ESP_OK(rmt_get_clk_div(channel, &div_cnt));
    TEST_ESP_OK(rmt_get_mem_block_num(channel, &memNum));
    TEST_ESP_OK(rmt_get_rx_idle_thresh(channel, &idleThreshold));
    TEST_ESP_OK(rmt_get_memory_owner(channel, &owner));

    TEST_ASSERT_EQUAL_UINT8(div_cnt, RMT_CLK_DIV);
    TEST_ASSERT_EQUAL_UINT8(memNum, 1);
    TEST_ASSERT_EQUAL_UINT16(idleThreshold, RMT_ITEM32_TIMEOUT_US / 10 * (RMT_TICK_10_US));
    TEST_ASSERT_EQUAL_INT(owner, RMT_MEM_OWNER_RX);

    //eRR
    TEST_ESP_OK(rmt_set_pin(channel, RMT_MODE_RX, 22));
    TEST_ESP_OK(rmt_set_clk_div(channel, RMT_CLK_DIV*2));
    TEST_ESP_OK(rmt_set_mem_block_num(channel, 2));
    TEST_ESP_OK(rmt_set_rx_filter(channel, 1, 100));
    TEST_ESP_OK(rmt_set_rx_idle_thresh(channel, RMT_ITEM32_TIMEOUT_US / 10 * (RMT_TICK_10_US)*2));
    TEST_ESP_OK(rmt_set_memory_owner(channel, RMT_MEM_OWNER_RX));

    TEST_ESP_OK(rmt_get_clk_div(channel, &div_cnt));
    TEST_ESP_OK(rmt_get_mem_block_num(channel, &memNum));
    TEST_ESP_OK(rmt_get_rx_idle_thresh(channel, &idleThreshold));
    TEST_ESP_OK(rmt_get_memory_owner(channel, &owner));

    TEST_ASSERT_EQUAL_UINT8(div_cnt, RMT_CLK_DIV*2);
    TEST_ASSERT_EQUAL_UINT8(memNum, 2);
    TEST_ASSERT_EQUAL_UINT16(idleThreshold, RMT_ITEM32_TIMEOUT_US / 10 * (RMT_TICK_10_US)*2);
    TEST_ASSERT_EQUAL_INT(owner, RMT_MEM_OWNER_RX);

    TEST_ESP_OK(rmt_driver_uninstall(channel));
}

TEST_CASE("RMT tx set and get properties", "[rmt][test_env=UT_T1_RMT]")
{
    rmt_channel_t channel = RMT_TX_CHANNEL;
    uint8_t memNum;
    uint8_t div_cnt;
    bool loop_en;
    rmt_mem_owner_t owner;

    tx_init();
    TEST_ESP_OK(rmt_get_clk_div(channel, &div_cnt));
    TEST_ESP_OK(rmt_get_mem_block_num(channel, &memNum));
    TEST_ESP_OK(rmt_get_tx_loop_mode(channel, &loop_en));
    TEST_ESP_OK(rmt_get_memory_owner(channel, &owner));

    TEST_ASSERT_EQUAL_INT8(loop_en, 0);
    TEST_ASSERT_EQUAL_UINT8(div_cnt, RMT_CLK_DIV);
    TEST_ASSERT_EQUAL_UINT8(memNum, 1);
    TEST_ASSERT_EQUAL_INT(owner, RMT_MEM_OWNER_TX);

    //reset by "set"
    TEST_ESP_OK(rmt_set_pin(channel, RMT_MODE_TX, RMT_TX_GPIO_NUM));
    TEST_ESP_OK(rmt_set_clk_div(channel, RMT_CLK_DIV*2));
    TEST_ESP_OK(rmt_set_mem_block_num(channel, 2));
    TEST_ESP_OK(rmt_set_tx_loop_mode(channel, 1));
    TEST_ESP_OK(rmt_set_tx_carrier(channel, 0, 1, 0, 1));
    TEST_ESP_OK(rmt_set_idle_level(channel, 1, 0));
    TEST_ESP_OK(rmt_set_memory_owner(channel, RMT_MEM_OWNER_TX));

    TEST_ESP_OK(rmt_get_clk_div(channel, &div_cnt));
    TEST_ESP_OK(rmt_get_mem_block_num(channel, &memNum));
    TEST_ESP_OK(rmt_get_tx_loop_mode(channel, &loop_en));
    TEST_ESP_OK(rmt_get_memory_owner(channel, &owner));

    TEST_ASSERT_EQUAL_INT8(loop_en, 1);
    TEST_ASSERT_EQUAL_UINT8(div_cnt, RMT_CLK_DIV*2);
    TEST_ASSERT_EQUAL_UINT8(memNum, 2);
    TEST_ASSERT_EQUAL_INT(owner, RMT_MEM_OWNER_TX);

    rmt_item32_t items[1];
    items[0].duration0 = 300 / 10 * RMT_TICK_10_US;  //300us
    items[0].level0 = 1;
    items[0].duration1 = 0;
    items[0].level1 = 0;
    for(int i=0; i<100; i++) {
        TEST_ESP_OK(rmt_write_items(RMT_TX_CHANNEL, items,
                1, /* Number of items */
                1 /* wait till done */));
        vTaskDelay(10/portTICK_PERIOD_MS);  //every 10ms to write the item
    }
    TEST_ESP_OK(rmt_driver_uninstall(channel));
}

TEST_CASE("RMT memory test", "[rmt][test_env=UT_T1_RMT]")
{
    rmt_config_t rmt_rx;
    rmt_rx.channel = RMT_RX_CHANNEL;
    rmt_rx.gpio_num = RMT_RX_GPIO_NUM;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = RMT_ITEM32_TIMEOUT_US / 10 * (RMT_TICK_10_US);
    TEST_ESP_OK(rmt_config(&rmt_rx));

    for(int i = 0; i<100; i++) {
        TEST_ESP_OK(rmt_driver_install(rmt_rx.channel, 1000, 0));
        TEST_ESP_OK(rmt_driver_uninstall(rmt_rx.channel));
    }
}

// RMT channel num and memory block relationship
TEST_CASE("RMT memory block test", "[rmt][test_env=UT_T1_RMT]")
{
    rmt_channel_t channel = 0;
    rmt_config_t rmt_rx;
    rmt_rx.channel = channel;
    rmt_rx.gpio_num = RMT_RX_GPIO_NUM;
    rmt_rx.clk_div = RMT_CLK_DIV;
    rmt_rx.mem_block_num = 1;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = 100;
    rmt_rx.rx_config.idle_threshold = RMT_ITEM32_TIMEOUT_US / 10 * (RMT_TICK_10_US);
    TEST_ESP_OK(rmt_config(&rmt_rx));
    TEST_ESP_OK(rmt_driver_install(rmt_rx.channel, 1000, 0));

    TEST_ESP_OK(rmt_set_mem_block_num(channel, 8));
    TEST_ASSERT(rmt_set_mem_block_num(channel, 9)==ESP_ERR_INVALID_ARG);
    TEST_ASSERT(rmt_set_mem_block_num(channel, -1)==ESP_ERR_INVALID_ARG);
    TEST_ESP_OK(rmt_driver_uninstall(rmt_rx.channel));

    rmt_rx.channel = 7;
    TEST_ESP_OK(rmt_config(&rmt_rx));
    TEST_ESP_OK(rmt_driver_install(rmt_rx.channel, 1000, 0));
    TEST_ASSERT(rmt_set_mem_block_num(rmt_rx.channel, 2)==ESP_ERR_INVALID_ARG);
    TEST_ASSERT(rmt_set_mem_block_num(rmt_rx.channel, -1)==ESP_ERR_INVALID_ARG);
    TEST_ESP_OK(rmt_driver_uninstall(rmt_rx.channel));
}

TEST_CASE("RMT send waveform(logic analyzer)", "[rmt][test_env=UT_T1_RMT][ignore]")
{
    tx_init();
    rmt_item32_t items[1];
    items[0].duration0 = 300 / 10 * RMT_TICK_10_US;  //300us
    items[0].level0 = 1;
    for(int i=0; i<500; i++) {
        TEST_ESP_OK(rmt_write_items(RMT_TX_CHANNEL, items,
                1, /* Number of items */
                1 /* wait till done */));
        vTaskDelay(10/portTICK_PERIOD_MS);  //every 10ms to write the item
    }
    TEST_ESP_OK(rmt_driver_uninstall(RMT_TX_CHANNEL));
}

TEST_CASE("RMT basic TX and RX", "[rmt][test_env=UT_T1_RMT]")
{
    tx_init();
    int tx_channel = RMT_TX_CHANNEL;
    uint16_t cmd = 0x0;
    uint16_t addr = 0x11;
    int tx_num = RMT_TX_DATA_NUM;
    ESP_LOGI(TAG, "RMT TX DATA");
    size_t size = (sizeof(rmt_item32_t) * DATA_ITEM_NUM * tx_num);
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
    int item_num = DATA_ITEM_NUM * tx_num;
    memset((void*) item, 0, size);
    int offset = 0;

    int rx_channel = RMT_RX_CHANNEL;
    rx_init();
    RingbufHandle_t rb = NULL;
    rmt_get_ringbuf_handle(rx_channel, &rb);
    rmt_rx_start(rx_channel, 1);
    // send data
    set_tx_data(tx_channel, cmd, addr, item_num, item, offset);
    rmt_write_items(tx_channel, item, item_num, 1);
    free(item);
    // receive data
    uint16_t tmp = get_rx_data(rb);
    TEST_ASSERT(tmp == 100);
    TEST_ESP_OK(rmt_driver_uninstall(RMT_TX_CHANNEL));
    TEST_ESP_OK(rmt_driver_uninstall(RMT_RX_CHANNEL));
}

TEST_CASE("RMT TX write item not wait", "[rmt][test_env=UT_T1_RMT]")
{
    tx_init();
    int tx_channel = RMT_TX_CHANNEL;
    uint16_t cmd = 0x0;
    uint16_t addr = 0x11;
    int tx_num = RMT_TX_DATA_NUM;
    ESP_LOGI(TAG, "RMT TX DATA");
    size_t size = (sizeof(rmt_item32_t) * DATA_ITEM_NUM * tx_num);
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
    int item_num = DATA_ITEM_NUM * tx_num;
    memset((void*) item, 0, size);
    int offset = 0;

    int rx_channel = RMT_RX_CHANNEL;
    rx_init();
    RingbufHandle_t rb = NULL;
    rmt_get_ringbuf_handle(rx_channel, &rb);
    rmt_rx_start(rx_channel, 1);

    // send data
    set_tx_data(tx_channel, cmd, addr, item_num, item, offset);
    rmt_write_items(tx_channel, item, item_num, 0);
    free(item);

    // receive data
    uint16_t tmp = get_rx_data(rb);
    TEST_ASSERT(tmp < 100);
    TEST_ESP_OK(rmt_driver_uninstall(RMT_TX_CHANNEL));
    TEST_ESP_OK(rmt_driver_uninstall(RMT_RX_CHANNEL));
}

TEST_CASE("RMT TX write item wait some ticks", "[rmt][test_env=UT_T1_RMT]")
{
    tx_init();
    int tx_channel = RMT_TX_CHANNEL;
    uint16_t cmd = 0x0;
    uint16_t addr = 0x11;
    int tx_num = RMT_TX_DATA_NUM;
    ESP_LOGI(TAG, "RMT TX DATA");
    size_t size = (sizeof(rmt_item32_t) * DATA_ITEM_NUM * tx_num);
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
    int item_num = DATA_ITEM_NUM * tx_num;
    memset((void*) item, 0, size);
    int offset = 0;

    int rx_channel = RMT_RX_CHANNEL;
    rx_init();
    RingbufHandle_t rb = NULL;
    rmt_get_ringbuf_handle(rx_channel, &rb);
    rmt_rx_start(rx_channel, 1);

    // send data
    set_tx_data(tx_channel, cmd, addr, item_num, item, offset);
    rmt_write_items(tx_channel, item, item_num, 0);
    rmt_wait_tx_done(tx_channel, portMAX_DELAY);
    free(item);

    // receive data
    uint16_t tmp = get_rx_data(rb);
    TEST_ASSERT(tmp == 100);
    TEST_ESP_OK(rmt_driver_uninstall(RMT_TX_CHANNEL));
    TEST_ESP_OK(rmt_driver_uninstall(RMT_RX_CHANNEL));
}

TEST_CASE("RMT TX stop test", "[rmt][test_env=UT_T1_RMT]")
{
    int rx_channel = RMT_RX_CHANNEL;
    rx_init();
    RingbufHandle_t rb = NULL;
    rmt_get_ringbuf_handle(rx_channel, &rb);
    rmt_rx_start(rx_channel, 1);

    vTaskDelay(10);
    tx_init();
    int tx_channel = RMT_TX_CHANNEL;
    int tx_num = RMT_TX_DATA_NUM;

    ESP_LOGI(TAG, "RMT TX DATA");
    size_t size = (sizeof(rmt_item32_t) * DATA_ITEM_NUM * tx_num);
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
    int item_num = DATA_ITEM_NUM * tx_num;
    memset((void*) item, 0, size);
    int offset = 0;
    uint16_t cmd = 0x0;
    uint16_t addr = 0x11;

    // send data
    set_tx_data(tx_channel, cmd, addr, item_num, item, offset);
    rmt_write_items(tx_channel, item, item_num, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    rmt_tx_stop(tx_channel);
    free(item);

    // receive data
    uint16_t tmp = get_rx_data(rb);
    TEST_ASSERT(tmp < 100);

    TEST_ESP_OK(rmt_driver_uninstall(RMT_TX_CHANNEL));
    TEST_ESP_OK(rmt_driver_uninstall(RMT_RX_CHANNEL));
}

TEST_CASE("RMT loop_en test", "[rmt][test_env=UT_T1_RMT][ignore]")
{
    rmt_tx_config_t tx_cfg = {
        .loop_en = true,  // set it as true
        .carrier_duty_percent = 50,
        .carrier_freq_hz = 38000,
        .carrier_level = 1,
        .carrier_en = RMT_TX_CARRIER_EN,
        .idle_level = 0,
        .idle_output_en = true,
    };
    rmt_config_t rmt_tx = {
        .channel = RMT_TX_CHANNEL,
        .gpio_num = RMT_TX_GPIO_NUM,
        .mem_block_num = 1,
        .clk_div = RMT_CLK_DIV,
        .tx_config = tx_cfg,
        .rmt_mode = 0,
    };
    rmt_config(&rmt_tx);
    rmt_driver_install(rmt_tx.channel, 0, 0);
    TEST_ESP_OK(rmt_driver_uninstall(RMT_TX_CHANNEL));

    int rx_channel = RMT_RX_CHANNEL;
    rx_init();
    RingbufHandle_t rb = NULL;
    rmt_get_ringbuf_handle(rx_channel, &rb);
    rmt_rx_start(rx_channel, 1);
    vTaskDelay(10);
    tx_init();
    int tx_channel = RMT_TX_CHANNEL;
    int tx_num = RMT_TX_DATA_NUM;

    ESP_LOGI(TAG, "RMT TX DATA");
    size_t size = (sizeof(rmt_item32_t) * DATA_ITEM_NUM * tx_num);
    rmt_item32_t* item = (rmt_item32_t*) malloc(size);
    int item_num = DATA_ITEM_NUM * tx_num;
    memset((void*) item, 0, size);
    int offset = 0;
    uint16_t cmd = 0x0;
    uint16_t addr = 0x11;

    // send data
    set_tx_data(tx_channel, cmd, addr, item_num, item, offset);
    rmt_write_items(tx_channel, item, item_num, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    rmt_tx_stop(tx_channel);
    free(item);

    // receive data
    uint16_t tmp = get_rx_data(rb);
    TEST_ASSERT(tmp < 100);

    TEST_ESP_OK(rmt_driver_uninstall(RMT_TX_CHANNEL));
    TEST_ESP_OK(rmt_driver_uninstall(RMT_RX_CHANNEL));
}

TEST_CASE("RMT use multi channel", "[rmt][test_env=UT_T1_RMT]")
{
    rmt_tx_config_t tx_cfg = {
        .loop_en = true,  // set it as true
        .carrier_duty_percent = 50,
        .carrier_freq_hz = 38000,
        .carrier_level = 1,
        .carrier_en = RMT_TX_CARRIER_EN,
        .idle_level = 0,
        .idle_output_en = true,
    };
    rmt_config_t rmt_tx1 = {
        .channel = RMT_TX_CHANNEL,
        .gpio_num = RMT_TX_GPIO_NUM,
        .mem_block_num = 4,
        .clk_div = RMT_CLK_DIV,
        .tx_config = tx_cfg,
        .rmt_mode = 0,
    };
    rmt_config(&rmt_tx1);
    rmt_driver_install(rmt_tx1.channel, 0, 0);

    rmt_config_t rmt_tx2 = rmt_tx1;
    rmt_tx2.channel = 2;
    rmt_config(&rmt_tx2);
    rmt_driver_install(rmt_tx2.channel, 0, 0);

    rmt_config_t rmt_tx3 = rmt_tx1;
    rmt_tx3.channel = 7;
    rmt_tx3.mem_block_num = 1;
    rmt_config(&rmt_tx3);
    rmt_driver_install(rmt_tx3.channel, 0, 0);

    TEST_ESP_OK(rmt_driver_uninstall(RMT_TX_CHANNEL));
    TEST_ESP_OK(rmt_driver_uninstall(2));
    TEST_ESP_OK(rmt_driver_uninstall(7));
}

