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

/******************************************************************************
 * Description: A stub to make the ESP32 debuggable by GDB over the serial
 * port, at least enough to do a backtrace on panic. This gdbstub is read-only:
 * it allows inspecting the ESP32 state
 *******************************************************************************/

#include <string.h>
#include "rom/ets_sys.h"
#include "soc/uart_reg.h"
#include "soc/io_mux_reg.h"
#include "esp_gdbstub.h"
#include "esp_panic.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

//Length of buffer used to reserve GDB commands. Has to be at least able to fit the G command, which
//implies a minimum size of about 320 bytes.
#define PBUFLEN 512

static unsigned char cmd[PBUFLEN];		//GDB command input buffer
static char chsum;						//Running checksum of the output packet

#define ATTR_GDBFN

//Receive a char from the uart. Uses polling and feeds the watchdog.
static int ATTR_GDBFN gdbRecvChar() {
	int i;
	while (((READ_PERI_REG(UART_STATUS_REG(0))>>UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT)==0) ;
	i=READ_PERI_REG(UART_FIFO_REG(0));
	return i;
}

//Send a char to the uart.
static void ATTR_GDBFN gdbSendChar(char c) {
	while (((READ_PERI_REG(UART_STATUS_REG(0))>>UART_TXFIFO_CNT_S)&UART_TXFIFO_CNT)>=126) ;
	WRITE_PERI_REG(UART_FIFO_REG(0), c);
}

//Send the start of a packet; reset checksum calculation.
static void ATTR_GDBFN gdbPacketStart() {
	chsum=0;
	gdbSendChar('$');
}

//Send a char as part of a packet
static void ATTR_GDBFN gdbPacketChar(char c) {
	if (c=='#' || c=='$' || c=='}' || c=='*') {
		gdbSendChar('}');
		gdbSendChar(c^0x20);
		chsum+=(c^0x20)+'}';
	} else {
		gdbSendChar(c);
		chsum+=c;
	}
}

//Send a string as part of a packet
static void ATTR_GDBFN gdbPacketStr(const char *c) {
	while (*c!=0) {
		gdbPacketChar(*c);
		c++;
	}
}

//Send a hex val as part of a packet. 'bits'/4 dictates the number of hex chars sent.
static void ATTR_GDBFN gdbPacketHex(int val, int bits) {
	char hexChars[]="0123456789abcdef";
	int i;
	for (i=bits; i>0; i-=4) {
		gdbPacketChar(hexChars[(val>>(i-4))&0xf]);
	}
}

//Finish sending a packet.
static void ATTR_GDBFN gdbPacketEnd() {
	gdbSendChar('#');
	gdbPacketHex(chsum, 8);
}

//Error states used by the routines that grab stuff from the incoming gdb packet
#define ST_ENDPACKET -1
#define ST_ERR -2
#define ST_OK -3
#define ST_CONT -4

//Grab a hex value from the gdb packet. Ptr will get positioned on the end
//of the hex string, as far as the routine has read into it. Bits/4 indicates
//the max amount of hex chars it gobbles up. Bits can be -1 to eat up as much
//hex chars as possible.
static long ATTR_GDBFN gdbGetHexVal(unsigned char **ptr, int bits) {
	int i;
	int no;
	unsigned int v=0;
	char c;
	no=bits/4;
	if (bits==-1) no=64;
	for (i=0; i<no; i++) {
		c=**ptr;
		(*ptr)++;
		if (c>='0' && c<='9') {
			v<<=4;
			v|=(c-'0');
		} else if (c>='A' && c<='F') {
			v<<=4;
			v|=(c-'A')+10;
		} else if (c>='a' && c<='f') {
			v<<=4;
			v|=(c-'a')+10;
		} else if (c=='#') {
			if (bits==-1) {
				(*ptr)--;
				return v;
			}
			return ST_ENDPACKET;
		} else {
			if (bits==-1) {
				(*ptr)--;
				return v;
			}
			return ST_ERR;
		}
	}
	return v;
}

//Swap an int into the form gdb wants it
static int ATTR_GDBFN iswap(int i) {
	int r;
	r=((i>>24)&0xff);
	r|=((i>>16)&0xff)<<8;
	r|=((i>>8)&0xff)<<16;
	r|=((i>>0)&0xff)<<24;
	return r;
}

//Read a byte from ESP32 memory.
static unsigned char ATTR_GDBFN readbyte(unsigned int p) {
	int *i=(int*)(p&(~3));
	if (p<0x20000000 || p>=0x80000000) return -1;
	return *i>>((p&3)*8);
}


//Register file in the format exp108 gdb port expects it.
//Inspired by gdb/regformats/reg-xtensa.dat
typedef struct {
	uint32_t pc;
	uint32_t a[64];
	uint32_t lbeg;
	uint32_t lend;
	uint32_t lcount;
	uint32_t sar;
	uint32_t windowbase;
	uint32_t windowstart;
	uint32_t configid0;
	uint32_t configid1;
	uint32_t ps;
	uint32_t threadptr;
	uint32_t br;
	uint32_t scompare1;
	uint32_t acclo;
	uint32_t acchi;
	uint32_t m0;
	uint32_t m1;
	uint32_t m2;
	uint32_t m3;
	uint32_t expstate;  //I'm going to assume this is exccause...
	uint32_t f64r_lo;
	uint32_t f64r_hi;
	uint32_t f64s;
	uint32_t f[16];
	uint32_t fcr;
	uint32_t fsr;
} GdbRegFile;


GdbRegFile gdbRegFile;

/*
//Register format as the Xtensa HAL has it:
STRUCT_FIELD (long, 4, XT_STK_EXIT,     exit)
STRUCT_FIELD (long, 4, XT_STK_PC,       pc)
STRUCT_FIELD (long, 4, XT_STK_PS,       ps)
STRUCT_FIELD (long, 4, XT_STK_A0,       a0)
[..]
STRUCT_FIELD (long, 4, XT_STK_A15,      a15)
STRUCT_FIELD (long, 4, XT_STK_SAR,      sar)
STRUCT_FIELD (long, 4, XT_STK_EXCCAUSE, exccause)
STRUCT_FIELD (long, 4, XT_STK_EXCVADDR, excvaddr)
STRUCT_FIELD (long, 4, XT_STK_LBEG,   lbeg)
STRUCT_FIELD (long, 4, XT_STK_LEND,   lend)
STRUCT_FIELD (long, 4, XT_STK_LCOUNT, lcount)
// Temporary space for saving stuff during window spill
STRUCT_FIELD (long, 4, XT_STK_TMP0,   tmp0)
STRUCT_FIELD (long, 4, XT_STK_TMP1,   tmp1)
STRUCT_FIELD (long, 4, XT_STK_TMP2,   tmp2)
STRUCT_FIELD (long, 4, XT_STK_VPRI,   vpri)
STRUCT_FIELD (long, 4, XT_STK_OVLY,   ovly)
#endif
STRUCT_END(XtExcFrame)
*/

static void commonRegfile() {
	if (gdbRegFile.a[0] & 0x8000000U) gdbRegFile.a[0] = (gdbRegFile.a[0] & 0x3fffffffU) | 0x40000000U;
	if (!esp_stack_ptr_is_sane(gdbRegFile.a[1])) gdbRegFile.a[1] = 0xDEADBEEF;
	gdbRegFile.windowbase=0; //0
	gdbRegFile.windowstart=0x1; //1
	gdbRegFile.configid0=0xdeadbeef; //ToDo
	gdbRegFile.configid1=0xdeadbeef; //ToDo
	gdbRegFile.threadptr=0xdeadbeef; //ToDo
	gdbRegFile.br=0xdeadbeef; //ToDo
	gdbRegFile.scompare1=0xdeadbeef; //ToDo
	gdbRegFile.acclo=0xdeadbeef; //ToDo
	gdbRegFile.acchi=0xdeadbeef; //ToDo
	gdbRegFile.m0=0xdeadbeef; //ToDo
	gdbRegFile.m1=0xdeadbeef; //ToDo
	gdbRegFile.m2=0xdeadbeef; //ToDo
	gdbRegFile.m3=0xdeadbeef; //ToDo
}

static void dumpHwToRegfile(XtExcFrame *frame) {
	int i;
	long *frameAregs=&frame->a0;
	gdbRegFile.pc=(frame->pc & 0x3fffffffU)|0x40000000U;
	for (i=0; i<16; i++) gdbRegFile.a[i]=frameAregs[i];
	for (i=16; i<64; i++) gdbRegFile.a[i]=0xDEADBEEF;
	gdbRegFile.lbeg=frame->lbeg;
	gdbRegFile.lend=frame->lend;
	gdbRegFile.lcount=frame->lcount;
	gdbRegFile.ps=(frame->ps & PS_UM) ? (frame->ps & ~PS_EXCM) : frame->ps;
	//All windows have been spilled to the stack by the ISR routines. The following values should indicate that.
	gdbRegFile.sar=frame->sar;
	commonRegfile();
	gdbRegFile.expstate=frame->exccause; //ToDo
}

//Send the reason execution is stopped to GDB.
static void sendReason() {
	//exception-to-signal mapping
	char exceptionSignal[]={4,31,11,11,2,6,8,0,6,7,0,0,7,7,7,7};
	int i=0;
	gdbPacketStart();
	gdbPacketChar('T');
	i=gdbRegFile.expstate&0x7f;
	if (i<sizeof(exceptionSignal)) {
		gdbPacketHex(exceptionSignal[i], 8);
	} else {
		gdbPacketHex(11, 8);
	}
	gdbPacketEnd();
}

static int sendPacket(const char * text) {
	gdbPacketStart();
	if (text != NULL) gdbPacketStr(text);
	gdbPacketEnd();
	return ST_OK;
}

#if CONFIG_GDBSTUB_SUPPORT_TASKS

#define STUB_TASKS_NUM  CONFIG_GDBSTUB_MAX_TASKS

//Remember the exception frame that caused panic since it's not saved in TCB
static XtExcFrame paniced_frame;

//Allows GDBStub to disable task support after a crash
//(e.g. if GDBStub crashes while trying to get task list, e.g. due to corrupted list structures)
static enum {
	HANDLER_NOT_STARTED,
	HANDLER_STARTED,
	HANDLER_TASK_SUPPORT_DISABLED
} handlerState;

static void dumpTaskToRegfile(XtSolFrame *frame) {
	int i;
	long *frameAregs=&frame->a0;
	gdbRegFile.pc=(frame->pc & 0x3fffffffU)|0x40000000U;
	for (i=0; i<4; i++) gdbRegFile.a[i]=frameAregs[i];
	for (i=4; i<64; i++) gdbRegFile.a[i]=0xDEADBEEF;
	gdbRegFile.lbeg=0;
	gdbRegFile.lend=0;
	gdbRegFile.lcount=0;
	gdbRegFile.ps=(frame->ps & PS_UM) ? (frame->ps & ~PS_EXCM) : frame->ps;
	//All windows have been spilled to the stack by the ISR routines. The following values should indicate that.
	gdbRegFile.sar=0;
	commonRegfile();
	gdbRegFile.expstate=0; //ToDo
}

// Fetch the task status. Returns the total number of tasks.
static unsigned getTaskInfo(unsigned index, unsigned * handle, const char ** name, unsigned * coreId) {
	static unsigned taskCount = 0;
	static TaskSnapshot_t tasks[STUB_TASKS_NUM];

	if (!taskCount) {
		unsigned tcbSize = 0;
		taskCount = uxTaskGetSnapshotAll(tasks, STUB_TASKS_NUM, &tcbSize);
	}
	if (index < taskCount) {
		TaskHandle_t h = (TaskHandle_t)tasks[index].pxTCB;
		if (handle) *handle = (unsigned)h;
		if (name) *name = pcTaskGetTaskName(h);
		if (coreId) *coreId = xTaskGetAffinity(h);
	}
	return taskCount;
}

typedef struct
{
	uint8_t * topOfStack;
} DumpTCB;


static void dumpTCBToRegFile(unsigned handle) {
	// A task handle is a pointer to a TCB in FreeRTOS
	DumpTCB * tcb = (DumpTCB*)handle;
	uint8_t * pxTopOfStack = tcb->topOfStack;

	//Deduced from coredump code
	XtExcFrame * frame = (XtExcFrame*)pxTopOfStack;
	if (frame->exit) {
		// It's an exception frame
		dumpHwToRegfile(frame);
	} else {
		XtSolFrame * taskFrame = (XtSolFrame*)pxTopOfStack;
		dumpTaskToRegfile(taskFrame);
	}
}

#define CUR_TASK_INDEX_NOT_SET -2
#define CUR_TASK_INDEX_UNKNOWN -1

// Get the index of the task currently running on the current CPU, and cache the result
static int findCurrentTaskIndex() {
	static int curTaskIndex = CUR_TASK_INDEX_NOT_SET;
	if (curTaskIndex == CUR_TASK_INDEX_NOT_SET) {
		unsigned curHandle = (unsigned)xTaskGetCurrentTaskHandleForCPU(xPortGetCoreID());
		unsigned handle;
		unsigned count = getTaskInfo(0, 0, 0, 0);
		for(int k=0; k<(int)count; k++) {
			if (getTaskInfo(k, &handle, 0, 0) && curHandle == handle) {
				curTaskIndex = k;
				return curTaskIndex;
			}
		}
		curTaskIndex = CUR_TASK_INDEX_UNKNOWN;
	}
	return curTaskIndex;
}

#endif // CONFIG_GDBSTUB_SUPPORT_TASKS

//Handle a command as received from GDB.
static int gdbHandleCommand(unsigned char *cmd, int len) {
	//Handle a command
	int i, j, k;
	unsigned char *data=cmd+1;
	if (cmd[0]=='g') {		//send all registers to gdb
		int *p=(int*)&gdbRegFile;
		gdbPacketStart();
		for (i=0; i<sizeof(GdbRegFile)/4; i++) gdbPacketHex(iswap(*p++), 32);
		gdbPacketEnd();
	} else if (cmd[0]=='G') {	//receive content for all registers from gdb
		int *p=(int*)&gdbRegFile;
		for (i=0; i<sizeof(GdbRegFile)/4; i++) *p++=iswap(gdbGetHexVal(&data, 32));
		sendPacket("OK");
	} else if (cmd[0]=='m') {	//read memory to gdb
		i=gdbGetHexVal(&data, -1);
		data++;
		j=gdbGetHexVal(&data, -1);
		gdbPacketStart();
		for (k=0; k<j; k++) {
			gdbPacketHex(readbyte(i++), 8);
		}
		gdbPacketEnd();
	} else if (cmd[0]=='?') {	//Reply with stop reason
		sendReason();
#if CONFIG_GDBSTUB_SUPPORT_TASKS
	} else if (handlerState != HANDLER_TASK_SUPPORT_DISABLED) {
		if (cmd[0]=='H') { //Continue with task
			if (cmd[1]=='g' || cmd[1]=='c') {
				const char * ret = "OK";
				data++;
				i=gdbGetHexVal(&data, -1);
				handlerState = HANDLER_STARTED; //Hg0 is the first packet received after connect
				j = findCurrentTaskIndex();
				if (i == j || (j == CUR_TASK_INDEX_UNKNOWN && i == 0)) {
					//GDB has asked us for the current task on this CPU.
					//This task either was executing when we have entered the panic handler,
					//or was already switched out and we have paniced during the context switch.
					//Either way we are interested in the stack frame where panic has happened,
					//so obtain the state from the exception frame rather than the TCB.
					dumpHwToRegfile(&paniced_frame);
				} else {
					unsigned handle, count;
					//Get the handle for that task
					count = getTaskInfo(i, &handle, 0, 0);
					//Then extract TCB and gdbRegFile from it
					if (i < count) dumpTCBToRegFile(handle);
					else ret = "E00";
				}
				return sendPacket(ret);
			}
			return sendPacket(NULL);
		} else if (cmd[0]=='T') {	//Task alive check
			unsigned count;
			data++;
			i=gdbGetHexVal(&data, -1);
			count = getTaskInfo(i, 0, 0, 0);
			return sendPacket(i < count ? "OK": "E00");
		} else if (cmd[0]=='q') {	//Extended query
			// React to qThreadExtraInfo or qfThreadInfo or qsThreadInfo or qC, without using strcmp
			if (len > 16 && cmd[1] == 'T' && cmd[2] == 'h' && cmd[3] == 'r' && cmd[7] == 'E' && cmd[12] == 'I' && cmd[16] == ',') {
				data=&cmd[17];
				i=gdbGetHexVal(&data, -1);

				unsigned handle = 0, coreId = 3;
				const char * name = 0;
				// Extract the task name and CPU from freeRTOS
				unsigned tCount = getTaskInfo(i, &handle, &name, &coreId);
				if (i < tCount)	{
					gdbPacketStart();
					for(k=0; name[k]; k++)	gdbPacketHex(name[k], 8);
					gdbPacketStr("20435055"); // CPU
					gdbPacketStr(coreId == 0 ? "30": coreId == 1 ? "31" : "78"); // 0 or 1 or x
					gdbPacketEnd();
					return ST_OK;
				}
			} else if (len >= 12 && (cmd[1] == 'f' || cmd[1] == 's') && (cmd[2] == 'T' && cmd[3] == 'h' && cmd[4] == 'r' && cmd[5] == 'e' && cmd[6] == 'a' && cmd[7] == 'd' && cmd[8] == 'I')) {
				// Only react to qfThreadInfo and qsThreadInfo, not using strcmp here since it can be in ROM
				// Extract the number of task from freeRTOS
				static int taskIndex = 0;
				unsigned tCount = 0;
				if (cmd[1] == 'f') {
					taskIndex = 0;
					handlerState = HANDLER_STARTED;	//It seems it's the first request GDB is sending
				}
				tCount = getTaskInfo(0, 0, 0, 0);
				if (taskIndex < tCount)	{
					gdbPacketStart();
					gdbPacketStr("m");
					gdbPacketHex(taskIndex, 32);
					gdbPacketEnd();
					taskIndex++;
				} else return sendPacket("l");
			} else if (len >= 2 && cmd[1] == 'C') {
				// Get current task id
				gdbPacketStart();
				k = findCurrentTaskIndex();
				if (k != CUR_TASK_INDEX_UNKNOWN) {
					gdbPacketStr("QC");
					gdbPacketHex(k, 32);
				} else gdbPacketStr("bad");
				gdbPacketEnd();
				return ST_OK;
			}
			return sendPacket(NULL);
		}
#endif // CONFIG_GDBSTUB_SUPPORT_TASKS
	} else {
		//We don't recognize or support whatever GDB just sent us.
		return sendPacket(NULL);
	}
	return ST_OK;
}


//Lower layer: grab a command packet and check the checksum
//Calls gdbHandleCommand on the packet if the checksum is OK
//Returns ST_OK on success, ST_ERR when checksum fails, a
//character if it is received instead of the GDB packet
//start char.
static int gdbReadCommand() {
	unsigned char c;
	unsigned char chsum=0, rchsum;
	unsigned char sentchs[2];
	int p=0;
	unsigned char *ptr;
	c=gdbRecvChar();
	if (c!='$') return c;
	while(1) {
		c=gdbRecvChar();
		if (c=='#') {	//end of packet, checksum follows
			cmd[p]=0;
			break;
		}
		chsum+=c;
		if (c=='$') {
			//Wut, restart packet?
			chsum=0;
			p=0;
			continue;
		}
		if (c=='}') {		//escape the next char
			c=gdbRecvChar();
			chsum+=c;
			c^=0x20;
		}
		cmd[p++]=c;
		if (p>=PBUFLEN) return ST_ERR;
	}
	//A # has been received. Get and check the received chsum.
	sentchs[0]=gdbRecvChar();
	sentchs[1]=gdbRecvChar();
	ptr=&sentchs[0];
	rchsum=gdbGetHexVal(&ptr, 8);
	if (rchsum!=chsum) {
		gdbSendChar('-');
		return ST_ERR;
	} else {
		gdbSendChar('+');
		return gdbHandleCommand(cmd, p);
	}
}


void esp_gdbstub_panic_handler(XtExcFrame *frame) {
#if CONFIG_GDBSTUB_SUPPORT_TASKS
	if (handlerState == HANDLER_STARTED) {
		//We have re-entered GDB Stub. Try disabling task support.
		handlerState = HANDLER_TASK_SUPPORT_DISABLED;
		gdbPacketEnd(); // Ends up any pending GDB packet (this creates a garbage value)
	} else if (handlerState == HANDLER_NOT_STARTED) {
		//Need to remember the frame that panic'd since gdb will ask for all threads before ours
		memcpy(&paniced_frame, frame, sizeof(paniced_frame));
		dumpHwToRegfile(&paniced_frame);
	}
#else // CONFIG_GDBSTUB_SUPPORT_TASKS
	dumpHwToRegfile(frame);
#endif // CONFIG_GDBSTUB_SUPPORT_TASKS

	//Make sure txd/rxd are enabled
	gpio_pullup_dis(1);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_U0RXD);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_U0TXD);

	sendReason();
	while(gdbReadCommand()!=ST_CONT);
	while(1);
}
