// The lines below allow to skip test in configuration other then 
// single_core which is required for this test to check placement in RTC_FAST memory
#include "sdkconfig.h"

#ifdef CONFIG_ESP32_RTCDATA_IN_FAST_MEM 

#include "unity.h"
#include "esp_system.h"
#include "rom/rtc.h"                // for rtc defines
#include "rom/uart.h"               // for uart_tx_wait_idle()
#include "esp_log.h"                // for log write functionality
#include "driver/rtc_io.h"          // for gpio configuration 
#include "esp_sleep.h"              // include sleep related functionality
#include "soc/soc.h"                // include access to soc macros
#include "soc/timer_group_reg.h"    // for watchdog register defines
#include "soc/rtc_cntl_reg.h"       // for rtc cntl register defines
#include "freertos/FreeRTOS.h"      // for xPortGetCoreID

// Test notes:
// This test case sequence checks behavior of placement .rtc_data and .rtc_rodata sections
// into RTC_FAST memory. The Kconfig option CONFIG_ESP32_RTCDATA_IN_FAST_MEM 
// is used to configure this behavior. The RTC_DATA_ATTR, RTC_RODATA_ATTR attributes
// can be used to place data into this area. If option is not set the .rtc_data 
// and .rtc_rodata are placed in slow memory segment for compatibility.
// The only PRO_CPU can access the RTC_FAST memory, so the option CONFIG_FREERTOS_UNICORE 
// should be enabled to place data into RTC_FAST memory segment.

#define ESP_EXT0_WAKEUP_LEVEL_LOW 0
#define ESP_EXT0_WAKEUP_LEVEL_HIGH 1

#define RTC_DATA_PATTERN 0xAAAAAAAA
#define RTC_RODATA_PATTERN 0x55555555
#define RTC_FAST_PATTERN 0xDDDDDDDD
#define RTC_SLOW_PATTERN 0x99999999
#define WAKE_STUB_PATTERN 0x77777777

#ifdef CONFIG_ESP32_RTCDATA_IN_FAST_MEM
#define CHECK_RTC_FAST_OPTION_ENABLED 1
#else
#define CHECK_RTC_FAST_OPTION_ENABLED 0
#endif

static RTC_DATA_ATTR uint32_t rtc_data = RTC_DATA_PATTERN;
static RTC_RODATA_ATTR uint32_t rtc_rodata = RTC_RODATA_PATTERN;
static RTC_FAST_ATTR uint32_t rtc_force_fast = RTC_FAST_PATTERN;
static RTC_SLOW_ATTR uint32_t rtc_force_slow = RTC_SLOW_PATTERN;

extern int _rtc_data_start;
extern int _rtc_data_end;
extern int _rtc_force_fast_start;
extern int _rtc_force_fast_end;
extern int _rtc_force_slow_start;
extern int _rtc_force_slow_end;

// This points to the values in RTC memory
static uint32_t *rtc_data_val_addr = (uint32_t*)&rtc_data;
static uint32_t *rtc_rodata_val_addr = (uint32_t*)&rtc_rodata;
static uint32_t *rtc_fast_val_addr = (uint32_t*)&rtc_force_fast;
static uint32_t *rtc_slow_val_addr = (uint32_t*)&rtc_force_slow;

static const char* tag = "rtc_data_fast_UnitTestMain";

static void RTC_IRAM_ATTR wake_stub(void);

static void RTC_IRAM_ATTR wake_stub_dummy(void)
{
    rtc_data = WAKE_STUB_PATTERN;
    set_rtc_memory_crc(); // update rtc memory CRC
}

static esp_err_t check_data_placement(uint32_t *value_address,
                                        uint32_t *seg_start, uint32_t *seg_end)
{
    esp_err_t result = ESP_FAIL;
    if (((uint32_t)value_address <= (uint32_t)seg_end)
        && ((uint32_t)value_address >= (uint32_t)seg_start)){
        result = ESP_OK;
    }
    return result;
}

static esp_err_t check_wake_stub_status(void)
{
    esp_err_t result = ESP_FAIL;

    uint32_t entry_addr = REG_READ(RTC_ENTRY_ADDR_REG);

    if (entry_addr == (uint32_t)&wake_stub_dummy) {
        result = ESP_OK;
    } else if (entry_addr == (uint32_t)&wake_stub) {
        result = ESP_ERR_INVALID_STATE;
    } else {
        result = ESP_FAIL;
    }

    return result;
}

static void RTC_IRAM_ATTR wake_stub(void)
{
    esp_default_wake_deep_sleep();
    
    // Set the pointer of the new wake stub function. 
    // It will be checked in test to make sure the wake stub entered
    REG_WRITE(RTC_ENTRY_ADDR_REG, (uint32_t)&wake_stub_dummy);

    // Set this value to check it in test later
    rtc_data = WAKE_STUB_PATTERN;
    set_rtc_memory_crc(); // update rtc memory CRC
}

static void setup_deep_sleep(void)
{
    // Set wake stub function to check its behavior
    // This function sets checksum of RTC fast memory appropriately
    esp_set_deep_sleep_wake_stub(&wake_stub);

    // Setup ext0 configuration to wake up immediately
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, ESP_EXT0_WAKEUP_LEVEL_HIGH));
    uart_tx_wait_idle(0);
    esp_deep_sleep_start();
}

// The lines below are required to suppress GCC warnings about casting of function pointers
// in unity macro expansion. These warnings may be treated as errors during automated test.
#pragma GCC diagnostic push  // required for GCC
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
static void reset_reason_power_on(void)
{
    printf("This test case verifies behavior of RTC_DATA variables after reset. \n");
    RESET_REASON reason = rtc_get_reset_reason(0);
    TEST_ASSERT(reason == POWERON_RESET || reason == RTCWDT_RTC_RESET);
    
    ESP_LOGI(tag, "Reset reason=(%d)", (uint16_t)reason);
                    
    printf("rtc_data_val = (0x%X), rtc_rodata_val = (0x%X), rtc_fast_val = (0x%X), rtc_slow_val = (0x%X)\r\n", 
                (uint32_t)*rtc_data_val_addr, (uint32_t)*rtc_rodata_val_addr, 
                (uint32_t)*rtc_fast_val_addr, (uint32_t)*rtc_slow_val_addr);
    
    printf("This test case called by CPU%u\r\n", (uint16_t)xPortGetCoreID());

    TEST_ASSERT(CHECK_RTC_FAST_OPTION_ENABLED == 1);
    printf("Check that values are placed in correct sections and then compare its values with patterns.\r\n");
    
    TEST_ASSERT(check_data_placement(rtc_fast_val_addr,
                                    (uint32_t*)&_rtc_force_fast_start,
                                    (uint32_t*)&_rtc_force_fast_end) == ESP_OK);
   
    TEST_ASSERT(check_data_placement(rtc_data_val_addr,
                                    (uint32_t*)&_rtc_data_start,
                                    (uint32_t*)&_rtc_data_end) == ESP_OK);
    
    TEST_ASSERT(check_data_placement(rtc_rodata_val_addr,
                                    (uint32_t*)&_rtc_data_start,
                                    (uint32_t*)&_rtc_data_end) == ESP_OK);

    printf("Values RTC_DATA_ATTR, RTC_FAST_ATTR are placed in section (0x%x - 0x%x).\r\n",
                                    (uint32_t)&_rtc_data_start, (uint32_t)&_rtc_data_end);    
                
    TEST_ASSERT(check_data_placement(rtc_slow_val_addr,
                                    (uint32_t*)&_rtc_force_slow_start,
                                    (uint32_t*)&_rtc_force_slow_end) == ESP_OK);
                                    
    printf("The RTC_SLOW values are placed in slow memory (0x%X - 0x%X).\r\n", 
                    (uint32_t)&_rtc_force_slow_start, (uint32_t)&_rtc_force_slow_end);

    TEST_ASSERT(RTC_RODATA_PATTERN == *rtc_rodata_val_addr);
    TEST_ASSERT(RTC_DATA_PATTERN == *rtc_data_val_addr);
    TEST_ASSERT(RTC_FAST_PATTERN == *rtc_fast_val_addr);
    
    TEST_ASSERT(RTC_SLOW_PATTERN == *rtc_slow_val_addr);

    printf("The values correspond to its patterns.\r\n");     
    printf("Go to deep sleep to check DEEP_SLEEP_RESET behavior. \r\n");    
    setup_deep_sleep();            
}

static void reset_reason_deep_sleep(void)
{
    printf("This test case checks behavior of RTC_DATA variables after deep sleep reset. \r\n");
    RESET_REASON reason = rtc_get_reset_reason(0);
    
    ESP_LOGI(tag, "Reset reason=(%d)", (uint16_t)reason);
                    
    printf("rtc_data_val = (0x%X), rtc_rodata_val = (0x%X), rtc_fast_val = (0x%X), rtc_slow_val = (0x%X)\r\n", 
                (uint32_t)*rtc_data_val_addr, (uint32_t)*rtc_rodata_val_addr, 
                (uint32_t)*rtc_fast_val_addr, (uint32_t)*rtc_slow_val_addr);
    
    TEST_ASSERT(reason == DEEPSLEEP_RESET);    

    if (CHECK_RTC_FAST_OPTION_ENABLED == 1) {
        printf("The CONFIG_ESP32_RTCDATA_IN_FAST_MEM is active means placement of RTC_DATA in fast segment.\r\n");
    } else {
        printf("The CONFIG_ESP32_RTCDATA_IN_FAST_MEM is not set. Please set this option\r\n");
        printf("in menuconfig to check .rtc.data placement in RTC fast memory.\r\n");
    }

    TEST_ASSERT(CHECK_RTC_FAST_OPTION_ENABLED == 1);

    // Check if deep sleep wake stub has been entered
    // this means CRC was correct
    if (check_wake_stub_status() == ESP_OK) {
        printf("The wake stub has been executed means CRC was correct.\r\n");
    } else {
        printf("The wake stub is not executed.\r\n");
    }

    TEST_ASSERT(check_wake_stub_status() == ESP_OK);
            
    // If the RTC CRC is incorrect the function return NULL
    if (esp_get_deep_sleep_wake_stub() == NULL){
        printf("The wake stub has changed the value of rtc_data as expected.\r\n");
    } else {
        printf("The rtc_data value is not changed that is incorrect. \r\n");
    }

    TEST_ASSERT(esp_get_deep_sleep_wake_stub() == NULL);            

    TEST_ASSERT(check_data_placement(rtc_data_val_addr,
                                    (uint32_t*)&_rtc_data_start,
                                    (uint32_t*)&_rtc_data_end) == ESP_OK);
    
    TEST_ASSERT(check_data_placement(rtc_rodata_val_addr,
                                    (uint32_t*)&_rtc_data_start,
                                    (uint32_t*)&_rtc_data_end) == ESP_OK);

    TEST_ASSERT(check_data_placement(rtc_fast_val_addr,
                                    (uint32_t*)&_rtc_force_fast_start,
                                    (uint32_t*)&_rtc_force_fast_end) == ESP_OK);

    printf("Values RTC_DATA_ATTR, RTC_FAST_ATTR are placed in RTC fast segment (0x%x - 0x%x).\r\n",
                        (uint32_t)&_rtc_data_start, (uint32_t)&_rtc_data_end);
    
    TEST_ASSERT(check_data_placement(rtc_slow_val_addr,
                                    (uint32_t*)&_rtc_force_slow_start,
                                    (uint32_t*)&_rtc_force_slow_end) == ESP_OK);
    
    printf("The RTC_SLOW values are placed in slow memory (0x%X - 0x%X).\r\n", 
                    (uint32_t)&_rtc_force_slow_start, (uint32_t)&_rtc_force_slow_end);   

    TEST_ASSERT(RTC_DATA_PATTERN == *rtc_data_val_addr);
    TEST_ASSERT(RTC_RODATA_PATTERN == *rtc_rodata_val_addr);
    TEST_ASSERT(RTC_FAST_PATTERN == *rtc_fast_val_addr);
    
    TEST_ASSERT(RTC_SLOW_PATTERN == *rtc_slow_val_addr);

    printf("Values correspond to its patterns.\r\n");
    printf("The test cases are done.. \r\n");
}

// The multiple stages test case to check values after certain reset reason
TEST_CASE_MULTIPLE_STAGES("RTC_DATA attributes behavior", \
                            "[restart][reset=DEEPSLEEP_RESET]",
                            reset_reason_power_on, reset_reason_deep_sleep);
#pragma GCC diagnostic pop   // require GCC

#endif // CONFIG_ESP32_RTCDATA_IN_FAST_MEM
