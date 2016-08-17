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
#include <stdlib.h>

#include <xtensa/config/core.h>

#include "rom/rtc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"

#include "soc/uart_register.h"
#include "soc/io_mux_reg.h"
#include "soc/dport_reg.h"
#include "soc/rtc_cntl_reg.h"

#include "gdbstub.h"

/*
Panic handlers; these get called when an unhandled exception occurs or the assembly-level
task switching / interrupt code runs into an unrecoverable error. The default task stack
overflow handler also is in here.
*/

#if !CONFIG_FREERTOS_PANIC_SILENT_REBOOT
//os_printf may be broken, so we fix our own printing fns...
inline static void panicPutchar(char c) {
	while (((READ_PERI_REG(UART_STATUS_REG(0))>>UART_TXFIFO_CNT_S)&UART_TXFIFO_CNT)>=126) ;
	WRITE_PERI_REG(UART_FIFO_REG(0), c);
}

inline static void panicPutStr(const char *c) {
	int x=0;
	while (c[x]!=0) {
		panicPutchar(c[x]);
		x++;
	}
}

inline static void panicPutHex(int a) {
	int x;
	int c;
	panicPutchar(' ');
	for (x=0; x<8; x++) {
		c=(a>>28)&0xf;
		if (c<10) panicPutchar('0'+c); else panicPutchar('a'+c-10);
		a<<=4;
	}
}

inline static void panicPutDec(int a) {
	int n1, n2;
	n1=a%10;
	n2=a/10;
	panicPutchar(' ');
	if (n2==0) panicPutchar(' '); else panicPutchar(n2+'0');
	panicPutchar(n1+'0');
}
#else
//No printing wanted. Stub out these functions.
inline static void panicPutchar(char c) { }
inline static void panicPutStr(const char *c) { }
inline static void panicPutHex(int a) { }
inline static void panicPutDec(int a) { }
#endif

int xPortGetCoreID();

void  __attribute__((weak)) vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName ) {
	panicPutStr("***ERROR*** A stack overflow in task");
	panicPutStr((char*)pcTaskName);
	panicPutStr("has been detected.\n");
}

static const char *edesc[]={
		"IllegalInstruction", "Syscall", "InstructionFetchError", "LoadStoreError",
		"Level1Interrupt", "Alloca", "IntegerDivideByZero", "PCValue",
		"Privileged", "LoadStoreAlignment", "res", "res",
		"InstrPDAddrError", "LoadStorePIFDataError", "InstrPIFAddrError", "LoadStorePIFAddrError",
		"InstTLBMiss", "InstTLBMultiHit", "InstFetchPrivilege", "res",
		"InstrFetchProhibited", "res", "res", "res",
		"LoadStoreTLBMiss", "LoadStoreTLBMultihit", "LoadStorePrivilege", "res",
		"LoadProhibited", "StoreProhibited", "res", "res",
		"Cp0Dis", "Cp1Dis", "Cp2Dis", "Cp3Dis",
		"Cp4Dis", "Cp5Dis", "Cp6Dis", "Cp7Dis"
	};


void commonErrorHandler(XtExcFrame *frame);

//The fact that we've panic'ed probably means the other CPU is now running wild, possibly
//messing up the serial output, so we kill it here.
static void haltOtherCore() {
	if (xPortGetCoreID()==0) {
		//Kill app cpu
		CLEAR_PERI_REG_MASK(RTC_OPTIONS0, RTC_CNTL_SW_STALL_APPCPU_C1<<RTC_CNTL_SW_STALL_APPCPU_C1_S);
		SET_PERI_REG_MASK(RTC_OPTIONS0, 0x21<<RTC_CNTL_SW_STALL_APPCPU_C1_S);
		CLEAR_PERI_REG_MASK(RTC_SW_CPU_STALL, RTC_CNTL_SW_STALL_APPCPU_C0<<RTC_CNTL_SW_STALL_APPCPU_C0_S);
		SET_PERI_REG_MASK(RTC_SW_CPU_STALL, 2<<RTC_CNTL_SW_STALL_APPCPU_C0_S);
	} else {
		//Kill pro cpu
		CLEAR_PERI_REG_MASK(RTC_OPTIONS0, RTC_CNTL_SW_STALL_PROCPU_C1<<RTC_CNTL_SW_STALL_PROCPU_C1_S);
		SET_PERI_REG_MASK(RTC_OPTIONS0, 0x21<<RTC_CNTL_SW_STALL_PROCPU_C1_S);
		CLEAR_PERI_REG_MASK(RTC_SW_CPU_STALL, RTC_CNTL_SW_STALL_PROCPU_C0<<RTC_CNTL_SW_STALL_PROCPU_C0_S);
		SET_PERI_REG_MASK(RTC_SW_CPU_STALL, 2<<RTC_CNTL_SW_STALL_PROCPU_C0_S);
	}
}

//Returns true when a debugger is attached using JTAG.
static int inOCDMode() {
#if CONFIG_FREERTOS_DEBUG_OCDAWARE
	int dcr;
	int reg=0x10200C; //DSRSET register
	asm("rer %0,%1":"=r"(dcr):"r"(reg));
	return (dcr&0x1);
#else
	return 0; //Always return no debugger is attached.
#endif
}

void panicHandler(XtExcFrame *frame) {
	haltOtherCore();
	panicPutStr("Guru Meditation Error: Core ");
	panicPutDec(xPortGetCoreID());
	panicPutStr(" panic'ed.\r\n");

	if (inOCDMode()) {
		asm("break.n 1");
	}
	commonErrorHandler(frame);
}

void xt_unhandled_exception(XtExcFrame *frame) {
	int *regs=(int*)frame;
	int x;

	haltOtherCore();
	panicPutStr("Guru Meditation Error of type ");
	x=regs[20];
	if (x<40) panicPutStr(edesc[x]); else panicPutStr("Unknown");
	panicPutStr(" occured on core ");
	panicPutDec(xPortGetCoreID());
	if (inOCDMode()) {
		panicPutStr(" at pc=");
		panicPutHex(regs[1]);
		panicPutStr(". Setting bp and returning..\r\n");
		//Stick a hardware breakpoint on the address the handler returns to. This way, the OCD debugger
		//will kick in exactly at the context the error happened.
		asm(
			"wsr.ibreaka0 %0\n" \
			"rsr.ibreakenable a3\n" \
			"movi a4,1\n" \
			"or a4, a4, a3\n" \
			"wsr.ibreakenable a4\n" \
			::"r"(regs[1]):"a3","a4");
		return;
	}
	panicPutStr(". Exception was unhandled.\r\n");
	commonErrorHandler(frame);
}


/*
We arrive here after a panic or unhandled exception, when no OCD is detected. Dump the registers to the
serial port and either jump to the gdb stub, halt the CPU or reboot.
*/
void commonErrorHandler(XtExcFrame *frame) {
	int *regs=(int*)frame;
	int x, y;
	const char *sdesc[]={
		"PC      ","PS      ","A0      ","A1      ","A2      ","A3      ","A4      ","A5      ",
		"A6      ","A7      ","A8      ","A9      ","A10     ","A11     ","A12     ","A13     ",
		"A14     ","A15     ","SAR     ","EXCCAUSE","EXCVADDR","LBEG    ","LEND    ","LCOUNT  "};

	panicPutStr("Register dump:\r\n");

	for (x=0; x<24; x+=4) {
		for (y=0; y<4; y++) {
			if (sdesc[x+y][0]!=0) {
				panicPutStr(sdesc[x+y]);
				panicPutStr(": ");
				panicPutHex(regs[x+y+1]);
				panicPutStr("  ");
			}
		}
		panicPutStr("\r\n");
	}
#if CONFIG_FREERTOS_PANIC_GDBSTUB
	panicPutStr("Entering gdb stub now.\r\n");
	gdbstubPanicHandler(frame);
#elif CONFIG_FREERTOS_PANIC_PRINT_REBOOT || CONFIG_FREERTOS_PANIC_SILENT_REBOOT
	panicPutStr("Rebooting...\r\n");
	for (x=0; x<100; x++) ets_delay_us(1000);
	software_reset();
#else
	panicPutStr("CPU halted.\r\n");
	while(1);
#endif
}
