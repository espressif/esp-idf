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

void Uart_Init(int no);
void uartAttach();
void ets_set_appcpu_boot_addr(uint32_t ent);
int ets_getAppEntry();

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
	ets_printf("Running app cpu, entry point is %p\n", call_user_start_cpu1);
	ets_delay_us(60000);

	SET_PERI_REG_MASK(APPCPU_CTRL_REG_B, DPORT_APPCPU_CLKGATE_EN);
	SET_PERI_REG_MASK(APPCPU_CTRL_REG_A, DPORT_APPCPU_RESETTING);
	CLEAR_PERI_REG_MASK(APPCPU_CTRL_REG_A, DPORT_APPCPU_RESETTING);


	for (int i=0; i<20; i++) ets_delay_us(40000);
	ets_set_appcpu_boot_addr((uint32_t)call_user_start_cpu1);

	ets_delay_us(10000);
	
//	while (ets_getAppEntry()==(int)call_user_start_cpu1) ;
	//Because of Reasons (tm), the pro cpu cannot use the SPI flash while the app cpu is booting.
//	while(((READ_PERI_REG(RTC_STORE7))&BIT(31)) == 0) ; // check APP boot complete flag
	ets_delay_us(50000);
	ets_delay_us(50000);
	ets_printf("\n\nBack to pro cpu.\n");
#else
	CLEAR_PERI_REG_MASK(APPCPU_CTRL_REG_B, DPORT_APPCPU_CLKGATE_EN);
#endif
	user_start_cpu0();
}


extern int xPortGetCoreID();

extern int _init_start;

/*
We arrive here because the pro CPU pulled us from reset. IRAM is in place, cache is still disabled, we can execute C code.
*/
void IRAM_ATTR call_user_start_cpu1() {
	//We need to do this ASAP because otherwise the structure to catch the SYSCALL instruction, which
	//we abuse to do ROM calls, won't work.

	asm volatile (\
		"wsr	%0, vecbase\n" \
		::"r"(&_init_start));

	//Enable SPI flash
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA3_U, FUNC_SD_DATA3_SPIWP); // swap PIN SDDATA3 from uart1 to spi, because cache need spi

	ets_printf("App cpu up\n");
	
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
	
	user_start_cpu1();
}



extern volatile int port_xSchedulerRunning;
extern int xPortStartScheduler();

void user_start_cpu1(void) {
	ets_printf("App cpu is running!\n");
	//Wait for the freertos initialization is finished on CPU0
	while (port_xSchedulerRunning == 0) ;
	ets_printf("Core0 started initializing FreeRTOS. Jumping to scheduler.\n");
	//Okay, start the scheduler!
	xPortStartScheduler();
}

extern void (*__init_array_start)(void);
extern void (*__init_array_end)(void);

extern esp_err_t app_main();
static void do_global_ctors(void) {
    void (**p)(void);
    for(p = &__init_array_start; p != &__init_array_end; ++p)
        (*p)();
}


void user_start_cpu0(void) {
	esp_err_t ret;

	ets_setup_syscalls();
	do_global_ctors();

#if 1 //workaround
    for (uint8_t i = 5; i < 8; i++) {
        ets_printf("erase sector %d\n", i);
        spi_flash_erase_sector(i);
    }
#endif
    ets_printf("nvs_flash_init\n");
    ret = nvs_flash_init(5, 3);
    if (ESP_OK != ret) {
        ets_printf("nvs_flash_init fail, ret=%d\n", ret);
    }

    system_init();

    esp_event_init(NULL);

	// TODO: consider ethernet interface
#if CONFIG_WIFI_ENABLED
    tcpip_adapter_init();
#endif

#if CONFIG_WIFI_ENABLED && CONFIG_WIFI_AUTO_STARTUP
#include "esp_wifi.h"
	esp_wifi_startup(app_main);
#else
	app_main();
#endif

	vTaskStartScheduler();
}

