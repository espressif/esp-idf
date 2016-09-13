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
#include <stdint.h>
#include <string.h>

#include "esp_attr.h"
#include "esp_err.h"

#include "rom/ets_sys.h"
#include "rom/uart.h"

#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"

#include "tcpip_adapter.h"

#include "heap_alloc_caps.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_spi_flash.h"
#include "esp_ipc.h"

static void IRAM_ATTR user_start_cpu0(void);
static void IRAM_ATTR call_user_start_cpu1();
static void IRAM_ATTR user_start_cpu1(void);
void Cache_Read_Enable();
extern void ets_setup_syscalls(void);


extern int __cpu1_entry_point;
extern int _bss_start;
extern int _bss_end;
extern int _init_start;
extern int _init_end;
extern int _iram_romjumptable_start;
extern int _iram_romjumptable_end;
extern int _iram_text_start;
extern int _iram_text_end;

/*
We arrive here after the bootloader finished loading the program from flash. The hardware is mostly uninitialized,
flash cache is down and the app CPU is in reset. We do have a stack, so we can do the initialization in C.
*/

static bool app_cpu_started = false;

void IRAM_ATTR call_user_start_cpu0() {
	//Kill wdt
	REG_CLR_BIT(0x3ff4808c, BIT(10)); //RTCCNTL+8C RTC_WDTCONFIG0 RTC_
	REG_CLR_BIT(0x6001f048, BIT(14)); //DR_REG_BB_BASE+48

	//Move exception vectors to IRAM
	asm volatile (\
		"wsr	%0, vecbase\n" \
		::"r"(&_init_start));

	uartAttach();
	ets_install_uart_printf();

	//Make page 0 access raise an exception
	//Also some other unused pages so we can catch weirdness
	//ToDo: this but nicer.
	asm volatile (\
		"movi a4,0x00000000\n" \
		"movi a5,0xf\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x80000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xa0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xc0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xe0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x20000000\n" \
		"movi a5,0x0\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x40000000\n" \
		"movi a5,0x2\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"isync\n" \
		:::"a4","a5");

	memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));

	//Initialize heap allocator
	heap_alloc_caps_init();

	ets_printf("Pro cpu up.\n");
    
#ifndef CONFIG_FREERTOS_UNICORE
	ets_printf("Starting app cpu, entry point is %p\n", call_user_start_cpu1);

	SET_PERI_REG_MASK(APPCPU_CTRL_REG_B, DPORT_APPCPU_CLKGATE_EN);
	CLEAR_PERI_REG_MASK(APPCPU_CTRL_REG_C, DPORT_APPCPU_RUNSTALL);
	SET_PERI_REG_MASK(APPCPU_CTRL_REG_A, DPORT_APPCPU_RESETTING);
	CLEAR_PERI_REG_MASK(APPCPU_CTRL_REG_A, DPORT_APPCPU_RESETTING);
	ets_set_appcpu_boot_addr((uint32_t)call_user_start_cpu1);

	while (!app_cpu_started) {
		ets_delay_us(100);
	}
#else
	CLEAR_PERI_REG_MASK(APPCPU_CTRL_REG_B, DPORT_APPCPU_CLKGATE_EN);
#endif
	ets_printf("Pro cpu start user code\n");
	user_start_cpu0();
}


extern int _init_start;

void IRAM_ATTR call_user_start_cpu1() {
	asm volatile (\
		"wsr	%0, vecbase\n" \
		::"r"(&_init_start));

	//Make page 0 access raise an exception
	//Also some other unused pages so we can catch weirdness
	//ToDo: this but nicer.
	asm volatile (\
		"movi a4,0x00000000\n" \
		"movi a5,0xf\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x80000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xa0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xc0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0xe0000000\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x20000000\n" \
		"movi a5,0x0\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"movi a4,0x40000000\n" \
		"movi a5,0x2\n" \
		"wdtlb a5,a4\n" \
		"witlb a5,a4\n" \
		"isync\n" \
		:::"a4","a5");
	
	ets_printf("App cpu up.\n");
	app_cpu_started = 1;
	user_start_cpu1();
}

extern volatile int port_xSchedulerRunning[2];

void IRAM_ATTR user_start_cpu1(void) {
	// Wait for FreeRTOS initialization to finish on PRO CPU
	while (port_xSchedulerRunning[0] == 0) {
	    ;
	}
	ets_printf("Starting scheduler on APP CPU.\n");
	xPortStartScheduler();
}

extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);

static void do_global_ctors(void) {
    void (**p)(void);
    for(p = &__init_array_start; p != &__init_array_end; ++p)
        (*p)();
}

extern esp_err_t app_main();

void user_start_cpu0(void) {
	ets_setup_syscalls();
	do_global_ctors();
	esp_ipc_init();
	spi_flash_init();

#if CONFIG_WIFI_ENABLED
    esp_err_t ret = nvs_flash_init(5, 3);
    if (ret != ESP_OK) {
        printf("nvs_flash_init failed, ret=%d\n", ret);
    }

    system_init();

    esp_event_init(NULL);

    tcpip_adapter_init();
#endif

#if CONFIG_WIFI_ENABLED && CONFIG_WIFI_AUTO_STARTUP
#include "esp_wifi.h"
	esp_wifi_startup(app_main);
#else
	app_main();
#endif

	ets_printf("Starting scheduler on PRO CPU.\n");
	vTaskStartScheduler();
}

