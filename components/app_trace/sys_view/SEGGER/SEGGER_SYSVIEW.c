/*
 * SPDX-FileCopyrightText: 1995-2021 SEGGER Microcontroller GmbH
 *
 * SPDX-License-Identifier: BSD-1-Clause
 *
 * SPDX-FileContributor: 2023 Espressif Systems (Shanghai) CO LTD
 */
/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2021 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the SystemView and RTT protocol, and J-Link.       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: 3.42                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW.c
Purpose : System visualization API implementation.
Revision: $Rev: 28341 $

Additional information:
  Packet format:
    Packets with IDs 0..23 are standard packets with known structure.
    For efficiency, they do *NOT* contain a length field.
    <ID><Data><TimeStampDelta>

    Packets with IDs 24..31 are standard packets with extendible
    structure and contain a length field.
    <ID><Lenght><Data><TimeStampDelta>

    Packet ID 31 is used for SystemView extended events.
    <ID><Lenght><ID_EX><Data><TimeStampDelta>

    Packets with IDs >= 32 always contain a length field.
    <ID><Length><Data><TimeStampDelta>

  Packet IDs:
       0..  31 : Standard packets, known by SystemView.
      32..1023 : OS-definable packets, described in a SystemView description file.
    1024..2047 : User-definable packets, described in a SystemView description file.
    2048..32767: Undefined.

  Data encoding:
    Basic types (int, short, char, ...):
      Basic types are encoded little endian with most-significant bit variant
      encoding.
      Each encoded byte contains 7 data bits [6:0] and the MSB continuation bit.
      The continuation bit indicates whether the next byte belongs to the data
      (bit set) or this is the last byte (bit clear).
      The most significant bits of data are encoded first, proceeding to the
      least significant bits in the final byte (little endian).

      Example encoding:
        Data: 0x1F4 (500)
        Encoded: 0xF4 (First 7 data bits 74 | Continuation bit)
                 0x03 (Second 7 data bits 03, no continuation)

        Data: 0xFFFFFFFF
        Encoded: 0xFF 0xFF 0xFF 0xFF 0x0F

        Data: 0xA2 (162),   0x03 (3), 0x7000
        Encoded: 0xA2 0x01  0x03      0x80 0xE0 0x01

    Byte arrays and strings:
      Byte arrays and strings are encoded as <NumBytes> followed by the raw data.
      NumBytes is encoded as a basic type with a theoretical maximum of 4G.

      Example encoding:
        Data: "Hello World\0" (0x48 0x65 0x6C 0x6C 0x6F 0x20 0x57 0x6F 0x72 0x6C 0x64 0x00)
        Encoded: 0x0B 0x48 0x65 0x6C 0x6C 0x6F 0x20 0x57 0x6F 0x72 0x6C 0x64

  Examples packets:
  01 F4 03 80 80 10 // Overflow packet. Data is a single U32.
                       This packet means: 500 packets lost, Timestamp is 0x40000

  02 0F 50          // ISR(15) Enter. Timestamp 80 (0x50)

  03 20             // ISR Exit. Timestamp 32 (0x20) (Shortest possible packet.)

  Sample code for user defined Packets:
    #define MY_ID   0x400                // Any value between 0x400 and 0x7FF
    void SendMyPacket(unsigned Para0, unsigned Para1, const char* s) {
      U8  aPacket[SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32 + MAX_STR_LEN + 1];
      U8* pPayload;
      //
      pPayload = SEGGER_SYSVIEW_PPREPARE_PACKET(aPacket);               // Prepare the packet for SystemView
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para0);             // Add the first parameter to the packet
      pPayload = SEGGER_SYSVIEW_EncodeU32(pPayload, Para1);             // Add the second parameter to the packet
      pPayload = SEGGER_SYSVIEW_EncodeString(pPayload, s, MAX_STR_LEN); // Add the string to the packet
      //
      SEGGER_SYSVIEW_SendPacket(&aPacket[0], pPayload, MY_ID);          // Send the packet with EventId = MY_ID
    }

    #define MY_ID_1 0x401
    void SendOnePara(unsigned Para0) {
      SEGGER_SYSVIEW_RecordU32(MY_ID_1, Para0);
    }

*/

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/

#define SEGGER_SYSVIEW_C  // For EXTERN statements in SEGGER_SYSVIEW.h

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "SEGGER_SYSVIEW_Int.h"
#include "SEGGER_RTT.h"

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
#if SEGGER_SYSVIEW_ID_SHIFT
  #define SHRINK_ID(Id)   (((Id) - _SYSVIEW_Globals.RAMBaseAddress) >> SEGGER_SYSVIEW_ID_SHIFT)
#else
  #define SHRINK_ID(Id)   ((Id) - _SYSVIEW_Globals.RAMBaseAddress)
#endif

#if SEGGER_SYSVIEW_RTT_CHANNEL > 0
  #define CHANNEL_ID_UP   SEGGER_SYSVIEW_RTT_CHANNEL
  #define CHANNEL_ID_DOWN SEGGER_SYSVIEW_RTT_CHANNEL
#else
  #define CHANNEL_ID_UP   _SYSVIEW_Globals.UpChannel
  #define CHANNEL_ID_DOWN _SYSVIEW_Globals.DownChannel
#endif

#if SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE
  #if (SEGGER_SYSVIEW_RTT_BUFFER_SIZE % SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE)
    #error "SEGGER_SYSVIEW_RTT_BUFFER_SIZE must be a multiple of SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE"
  #endif
#endif

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
// Timestamps may be less than full 32-bits, in which case we need to zero
// the unused bits to properly handle overflows.
// Note that this is a quite common scenario, as a 32-bit time such as
// SysTick might be scaled down to reduce bandwith
// or a 16-bit hardware time might be used.
#if SEGGER_SYSVIEW_TIMESTAMP_BITS < 32  // Eliminate unused bits in case hardware timestamps are less than 32 bits
  #define MAKE_DELTA_32BIT(Delta) Delta <<= 32 - SEGGER_SYSVIEW_TIMESTAMP_BITS; \
                                  Delta >>= 32 - SEGGER_SYSVIEW_TIMESTAMP_BITS;
#else
  #define MAKE_DELTA_32BIT(Delta)
#endif

#if SEGGER_SYSVIEW_SUPPORT_LONG_ID
  #define _MAX_ID_BYTES       5u
#else
  #define _MAX_ID_BYTES       2u
#endif

#if SEGGER_SYSVIEW_SUPPORT_LONG_DATA
  #define _MAX_DATA_BYTES     5u
#else
  #define _MAX_DATA_BYTES     2u
#endif

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
#define ENABLE_STATE_OFF        0
#define ENABLE_STATE_ON         1
#define ENABLE_STATE_DROPPING   2

#define FORMAT_FLAG_LEFT_JUSTIFY   (1u << 0)
#define FORMAT_FLAG_PAD_ZERO       (1u << 1)
#define FORMAT_FLAG_PRINT_SIGN     (1u << 2)
#define FORMAT_FLAG_ALTERNATE      (1u << 3)

#define MODULE_EVENT_OFFSET        (512)

/*********************************************************************
*
*       Types, local
*
**********************************************************************
*/
typedef struct {
  U8*       pBuffer;
  U8*       pPayload;
  U8*       pPayloadStart;
  U32       Options;
  unsigned  Cnt;
} SEGGER_SYSVIEW_PRINTF_DESC;

typedef struct {
        U8                      EnableState;   // 0: Disabled, 1: Enabled, (2: Dropping)
        U8                      UpChannel;
        U8                      RecursionCnt;
        U32                     SysFreq;
        U32                     CPUFreq;
        U32                     LastTxTimeStamp;
        U32                     RAMBaseAddress;
#if (SEGGER_SYSVIEW_POST_MORTEM_MODE == 1)
        U32                     PacketCount;
#else
        U32                     DropCount;
        U8                      DownChannel;
#endif
        U32                     DisabledEvents;
  const SEGGER_SYSVIEW_OS_API*  pOSAPI;
        SEGGER_SYSVIEW_SEND_SYS_DESC_FUNC*   pfSendSysDesc;
} SEGGER_SYSVIEW_GLOBALS;

/*********************************************************************
*
*       Function prototypes, required
*
**********************************************************************
*/
static void _SendPacket(U8* pStartPacket, U8* pEndPacket, unsigned int EventId);

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static const U8 _abSync[10] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#if SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE
  #ifdef SEGGER_SYSVIEW_SECTION
    //
    // Alignment + special section required
    //
    #if (defined __GNUC__)
      __attribute__ ((section (SEGGER_SYSVIEW_SECTION), aligned (SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE))) static char _UpBuffer  [SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
      #if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
        __attribute__ ((section (SEGGER_SYSVIEW_SECTION), aligned (SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE))) static char _DownBuffer[8];  // Small, fixed-size buffer, for back-channel comms
      #endif
    #elif (defined __ICCARM__) || (defined __ICCRX__)
      #pragma location=SEGGER_SYSVIEW_SECTION
      #pragma data_alignment=SEGGER_RTT_CPU_CACHE_LINE_SIZE
      static char _UpBuffer  [SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
      #if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
        #pragma location=SEGGER_SYSVIEW_SECTION
        #pragma data_alignment=SEGGER_RTT_CPU_CACHE_LINE_SIZE
        static char _DownBuffer[8];  // Small, fixed-size buffer, for back-channel comms
      #endif
    #elif (defined __CC_ARM)
      __attribute__ ((section (SEGGER_SYSVIEW_SECTION), aligned (SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE), zero_init)) static char _UpBuffer  [SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
      #if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
        __attribute__ ((section (SEGGER_SYSVIEW_SECTION), aligned (SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE), zero_init)) static char _DownBuffer[8];  // Small, fixed-size buffer, for back-channel comms
      #endif
    #else
      #error "Do not know how to place SystemView buffers in specific section"
    #endif
  #else
    //
    // Only alignment required
    //
    #if (defined __GNUC__)
      __attribute__ ((aligned (SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE))) static char _UpBuffer  [SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
      #if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
        __attribute__ ((aligned (SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE))) static char _DownBuffer[8];  // Small, fixed-size buffer, for back-channel comms
      #endif
    #elif (defined __ICCARM__) || (defined __ICCRX__)
      #pragma data_alignment=SEGGER_RTT_CPU_CACHE_LINE_SIZE
      static char _UpBuffer  [SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
      #if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
        #pragma data_alignment=SEGGER_RTT_CPU_CACHE_LINE_SIZE
        static char _DownBuffer[8];  // Small, fixed-size buffer, for back-channel comms
      #endif
    #elif (defined __CC_ARM)
      __attribute__ ((aligned (SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE), zero_init)) static char _UpBuffer  [SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
      #if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
        __attribute__ ((aligned (SEGGER_SYSVIEW_CPU_CACHE_LINE_SIZE), zero_init)) static char _DownBuffer[8];  // Small, fixed-size buffer, for back-channel comms
      #endif
    #else
      #error "Do not know how to align SystemView buffers to cache line size"
    #endif
  #endif
#else
  #ifdef SEGGER_SYSVIEW_SECTION
    //
    // Only special section required
    //
    #if (defined __GNUC__)
      __attribute__ ((section (SEGGER_SYSVIEW_SECTION))) static char _UpBuffer  [SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
      #if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
        __attribute__ ((section (SEGGER_SYSVIEW_SECTION))) static char _DownBuffer[8];  // Small, fixed-size buffer, for back-channel comms
      #endif
    #elif (defined __ICCARM__) || (defined __ICCRX__)
      #pragma location=SEGGER_SYSVIEW_SECTION
      static char _UpBuffer  [SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
      #if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
        #pragma location=SEGGER_SYSVIEW_SECTION
        static char _DownBuffer[8];  // Small, fixed-size buffer, for back-channel comms
      #endif
    #elif (defined __CC_ARM)
      __attribute__ ((section (SEGGER_SYSVIEW_SECTION), zero_init)) static char _UpBuffer  [SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
      #if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
        __attribute__ ((section (SEGGER_SYSVIEW_SECTION), zero_init)) static char _DownBuffer[8];  // Small, fixed-size buffer, for back-channel comms
      #endif
    #else
      #error "Do not know how to place SystemView buffers in specific section"
    #endif
  #else
    //
    // Neither special section nor alignment required
    //
    static char _UpBuffer  [SEGGER_SYSVIEW_RTT_BUFFER_SIZE];
    #if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
      static char _DownBuffer[8];  // Small, fixed-size buffer, for back-channel comms
    #endif
  #endif
#endif

static SEGGER_SYSVIEW_GLOBALS _SYSVIEW_Globals;

static SEGGER_SYSVIEW_MODULE* _pFirstModule;
static U8                     _NumModules;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

#define ENCODE_U32(pDest, Value) {                                                  \
                                   U8* pSysviewPointer;                             \
                                   U32 SysViewData;                                 \
                                   pSysviewPointer = pDest;                         \
                                   SysViewData = Value;                             \
                                   while(SysViewData > 0x7F) {                      \
                                     *pSysviewPointer++ = (U8)(SysViewData | 0x80); \
                                     SysViewData >>= 7;                             \
                                   };                                               \
                                   *pSysviewPointer++ = (U8)SysViewData;            \
                                   pDest = pSysviewPointer;                         \
                                 };



#if (SEGGER_SYSVIEW_USE_STATIC_BUFFER == 1)
static U8 _aPacket[SEGGER_SYSVIEW_MAX_PACKET_SIZE];

#define RECORD_START(PacketSize)  SEGGER_SYSVIEW_LOCK();                            \
                                  pPayloadStart = _PreparePacket(_aPacket);

#define RECORD_END()              SEGGER_SYSVIEW_UNLOCK()

#else

#define RECORD_START(PacketSize)  U8 aPacket[(PacketSize)];                         \
                                  pPayloadStart = _PreparePacket(aPacket);          \

#define RECORD_END()

#endif

/*********************************************************************
*
*       _EncodeData()
*
*  Function description
*    Encode a byte buffer in variable-length format.
*
*  Parameters
*    pPayload - Pointer to where string will be encoded.
*    pSrc     - Pointer to data buffer to be encoded.
*    NumBytes - Number of bytes in the buffer to be encoded.
*
*  Return value
*    Pointer to the byte following the value, i.e. the first free
*    byte in the payload and the next position to store payload
*    content.
*
*  Additional information
*    The data is encoded as a count byte followed by the contents
*    of the data buffer.
*    Make sure NumBytes + 1 bytes are free for the payload.
*/
static U8* _EncodeData(U8* pPayload, const char* pSrc, unsigned int NumBytes) {
  unsigned int  n;
  const U8*     p;
  //
  n = 0;
  p = (const U8*)pSrc;
  //
  // Write Len
  //
  if (NumBytes < 255)  {
    *pPayload++ = (U8)NumBytes;
  } else {
    *pPayload++ = 255;
    *pPayload++ = (NumBytes & 255);
    *pPayload++ = ((NumBytes >> 8) & 255);
  }
  while (n < NumBytes) {
    *pPayload++ = *p++;
    n++;
  }
  return pPayload;
}

/*********************************************************************
*
*       _EncodeStr()
*
*  Function description
*    Encode a string in variable-length format.
*
*  Parameters
*    pPayload - Pointer to where string will be encoded.
*    pText    - String to encode.
*    Limit    - Maximum number of characters to encode from string.
*
*  Return value
*    Pointer to the byte following the value, i.e. the first free
*    byte in the payload and the next position to store payload
*    content.
*
*  Additional information
*    The string is encoded as a count byte followed by the contents
*    of the string.
*    No more than 1 + Limit bytes will be encoded to the payload.
*/
static U8 *_EncodeStr(U8 *pPayload, const char *pText, unsigned int Limit) {
  unsigned int n;
  unsigned int Len;
  //
  // Compute string len
  //
  Len = 0;
  if (pText != NULL) {
    while(*(pText + Len) != 0) {
      Len++;
    }
    if (Len > Limit) {
      Len = Limit;
    }
  }
  //
  // Write Len
  //
  if (Len < 255)  {
    *pPayload++ = (U8)Len;
  } else {
    *pPayload++ = 255;
    *pPayload++ = (Len & 255);
    *pPayload++ = ((Len >> 8) & 255);
  }
  //
  // copy string
  //
  n = 0;
  while (n < Len) {
    *pPayload++ = *pText++;
    n++;
  }
  return pPayload;
}

/*********************************************************************
*
*       _PreparePacket()
*
*  Function description
*    Prepare a SystemView event packet header.
*
*  Parameters
*    pPacket - Pointer to start of packet to initialize.
*
*  Return value
*    Pointer to first byte of packet payload.
*
*  Additional information
*    The payload length and evnetId are not initialized.
*    PreparePacket only reserves space for them and they are
*    computed and filled in by the sending function.
*/
static U8* _PreparePacket(U8* pPacket) {
  return pPacket + _MAX_ID_BYTES + _MAX_DATA_BYTES;
}

/*********************************************************************
*
*       _HandleIncomingPacket()
*
*  Function description
*    Read an incoming command from the down channel and process it.
*
*  Additional information
*    This function is called each time after sending a packet.
*    Processing incoming packets is done asynchronous. SystemView might
*    already have sent event packets after the host has sent a command.
*/
#if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
static void _HandleIncomingPacket(void) {
  U8  Cmd;
  unsigned int Status;
  //
  Status = SEGGER_RTT_ReadNoLock(CHANNEL_ID_DOWN, &Cmd, 1);
  if (Status > 0) {
    switch (Cmd) {
    case SEGGER_SYSVIEW_COMMAND_ID_START:
      SEGGER_SYSVIEW_Start();
      break;
    case SEGGER_SYSVIEW_COMMAND_ID_STOP:
      SEGGER_SYSVIEW_Stop();
      break;
    case SEGGER_SYSVIEW_COMMAND_ID_GET_SYSTIME:
      SEGGER_SYSVIEW_RecordSystime();
      break;
    case SEGGER_SYSVIEW_COMMAND_ID_GET_TASKLIST:
      SEGGER_SYSVIEW_SendTaskList();
      break;
    case SEGGER_SYSVIEW_COMMAND_ID_GET_SYSDESC:
      SEGGER_SYSVIEW_GetSysDesc();
      break;
    case SEGGER_SYSVIEW_COMMAND_ID_GET_NUMMODULES:
      SEGGER_SYSVIEW_SendNumModules();
      break;
    case SEGGER_SYSVIEW_COMMAND_ID_GET_MODULEDESC:
      SEGGER_SYSVIEW_SendModuleDescription();
      break;
    case SEGGER_SYSVIEW_COMMAND_ID_GET_MODULE:
      Status = SEGGER_RTT_ReadNoLock(CHANNEL_ID_DOWN, &Cmd, 1);
      if (Status > 0) {
        SEGGER_SYSVIEW_SendModule(Cmd);
      }
      break;
    case SEGGER_SYSVIEW_COMMAND_ID_HEARTBEAT:
      break;
    default:
      if (Cmd >= 128) { // Unknown extended command. Dummy read its parameter.
        SEGGER_RTT_ReadNoLock(CHANNEL_ID_DOWN, &Cmd, 1);
      }
      break;
    }
  }
}
#endif // (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)

/*********************************************************************
*
*       _TrySendOverflowPacket()
*
*  Function description
*    Try to transmit an SystemView Overflow packet containing the
*    number of dropped packets.
*
*  Additional information
*    Format as follows:
*      01 <DropCnt><TimeStamp>  Max. packet len is 1 + 5 + 5 = 11
*
*    Example packets sent
*      01 20 40
*
*  Return value
*    !=0:  Success, Message sent (stored in RTT-Buffer)
*    ==0:  Buffer full, Message *NOT* stored
*
*/
#if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
static int _TrySendOverflowPacket(void) {
  U32 TimeStamp;
  I32 Delta;
  int Status;
  U8  aPacket[11];
  U8* pPayload;

  aPacket[0] = SYSVIEW_EVTID_OVERFLOW;      // 1
  pPayload   = &aPacket[1];
  ENCODE_U32(pPayload, _SYSVIEW_Globals.DropCount);
  //
  // Compute time stamp delta and append it to packet.
  //
  TimeStamp  = SEGGER_SYSVIEW_GET_TIMESTAMP();
  Delta = TimeStamp - _SYSVIEW_Globals.LastTxTimeStamp;
  MAKE_DELTA_32BIT(Delta);
  ENCODE_U32(pPayload, Delta);
  //
  // Try to store packet in RTT buffer and update time stamp when this was successful
  //
  Status = (int)SEGGER_RTT_WriteSkipNoLock(CHANNEL_ID_UP, aPacket, (unsigned int)(pPayload - aPacket));
  SEGGER_SYSVIEW_ON_EVENT_RECORDED(pPayload - aPacket);
  if (Status) {
    _SYSVIEW_Globals.LastTxTimeStamp = TimeStamp;
    _SYSVIEW_Globals.EnableState--; // EnableState has been 2, will be 1. Always.
  } else {
    _SYSVIEW_Globals.DropCount++;
  }
  //
  return Status;
}
#endif  // (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)

/*********************************************************************
*
*       _SendSyncInfo()
*
*  Function description
*    Send SystemView sync packet and system information in
*    post mortem mode.
*
*  Additional information
*    Sync is 10 * 0x00 without timestamp
*/
#if (SEGGER_SYSVIEW_POST_MORTEM_MODE == 1)
static void _SendSyncInfo(void) {
  //
  // Add sync packet ( 10 * 0x00)
  // Send system description
  // Send system time
  // Send task list
  // Send module description
  // Send module information
  //
  SEGGER_RTT_WriteWithOverwriteNoLock(CHANNEL_ID_UP, _abSync, 10);
  SEGGER_SYSVIEW_ON_EVENT_RECORDED(10);
  SEGGER_SYSVIEW_RecordVoid(SYSVIEW_EVTID_TRACE_START);
  {
    U8* pPayload;
    U8* pPayloadStart;
    RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 4 * SEGGER_SYSVIEW_QUANTA_U32);
    //
    pPayload = pPayloadStart;
    ENCODE_U32(pPayload, _SYSVIEW_Globals.SysFreq);
    ENCODE_U32(pPayload, _SYSVIEW_Globals.CPUFreq);
    ENCODE_U32(pPayload, _SYSVIEW_Globals.RAMBaseAddress);
    ENCODE_U32(pPayload, SEGGER_SYSVIEW_ID_SHIFT);
    _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_INIT);
    RECORD_END();
  }
  if (_SYSVIEW_Globals.pfSendSysDesc) {
    _SYSVIEW_Globals.pfSendSysDesc();
  }
  SEGGER_SYSVIEW_RecordSystime();
  SEGGER_SYSVIEW_SendTaskList();
  if (_NumModules > 0) {
    int n;
    SEGGER_SYSVIEW_SendNumModules();
    for (n = 0; n < _NumModules; n++) {
      SEGGER_SYSVIEW_SendModule(n);
    }
    SEGGER_SYSVIEW_SendModuleDescription();
  }
}
#endif  // (SEGGER_SYSVIEW_POST_MORTEM_MODE == 1)

/*********************************************************************
*
*       _SendPacket()
*
*  Function description
*    Send a SystemView packet over RTT. RTT channel and mode are
*    configured by macros when the SystemView component is initialized.
*    This function takes care of maintaining the packet drop count
*    and sending overflow packets when necessary.
*    The packet must be passed without Id and Length because this
*    function prepends it to the packet before transmission.
*
*  Parameters
*    pStartPacket - Pointer to start of packet payload.
*                   There must be at least 4 bytes free to prepend Id and Length.
*    pEndPacket   - Pointer to end of packet payload.
*    EventId      - Id of the event to send.
*
*/
static void _SendPacket(U8* pStartPacket, U8* pEndPacket, unsigned int EventId) {
  unsigned int  NumBytes;
  U32           TimeStamp;
  U32           Delta;
#if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
  unsigned int  Status;
#endif

#if (SEGGER_SYSVIEW_USE_STATIC_BUFFER == 0)
  SEGGER_SYSVIEW_LOCK();
#endif

#if (SEGGER_SYSVIEW_POST_MORTEM_MODE == 1)
  if (_SYSVIEW_Globals.EnableState == 0) {
    goto SendDone;
  }
#else
  if (_SYSVIEW_Globals.EnableState == 1) {  // Enabled, no dropped packets remaining
    goto Send;
  }
  if (_SYSVIEW_Globals.EnableState == 0) {
    goto SendDone;
  }
  //
  // Handle buffer full situations:
  // Have packets been dropped before because buffer was full?
  // In this case try to send and overflow packet.
  //
  if (_SYSVIEW_Globals.EnableState == 2) {
    _TrySendOverflowPacket();
    if (_SYSVIEW_Globals.EnableState != 1) {
      goto SendDone;
    }
  }
Send:
#endif
  //
  // Check if event is disabled from being recorded.
  //
  if (EventId < 32) {
    if (_SYSVIEW_Globals.DisabledEvents & ((U32)1u << EventId)) {
      goto SendDone;
    }
  }
  //
  // Prepare actual packet.
  // If it is a known packet, prepend eventId only,
  // otherwise prepend packet length and eventId.
  //
  if (EventId < 24) {
    *--pStartPacket = (U8)EventId;
  } else {
    //
    // Get data length and prepend it.
    //
    NumBytes = (unsigned int)(pEndPacket - pStartPacket);
#if SEGGER_SYSVIEW_SUPPORT_LONG_DATA
    if (NumBytes < 127) {
      #error "Seems EventId should be changed to NumBytes in the next line. Please check."
      *--pStartPacket = EventId;
    } else {
      //
      // Backwards U32 encode EventId.
      //
      if (NumBytes < (1ul << 14)) { // Encodes in 2 bytes
        *--pStartPacket = (U8)(NumBytes >>  7);
        *--pStartPacket = (U8)(NumBytes | 0x80);
      } else if (NumBytes < (1ul << 21)) {    // Encodes in 3 bytes
        *--pStartPacket = (U8)(NumBytes >> 14);
        *--pStartPacket = (U8)((NumBytes >>  7) | 0x80);
        *--pStartPacket = (U8)(NumBytes | 0x80);
      } else if (NumBytes < (1ul << 28)) {    // Encodes in 4 bytes
        *--pStartPacket = (U8)(NumBytes >> 21);
        *--pStartPacket = (U8)((NumBytes >> 14) | 0x80);
        *--pStartPacket = (U8)((NumBytes >>  7) | 0x80);
        *--pStartPacket = (U8)(NumBytes | 0x80);
      } else {                              // Encodes in 5 bytes
        *--pStartPacket = (U8)(NumBytes >> 28);
        *--pStartPacket = (U8)((NumBytes >> 21) | 0x80);
        *--pStartPacket = (U8)((NumBytes >> 14) | 0x80);
        *--pStartPacket = (U8)((NumBytes >>  7) | 0x80);
        *--pStartPacket = (U8)(NumBytes | 0x80);
      }
    }
#else
    if (NumBytes > 127) {
      *--pStartPacket = (U8)(NumBytes >> 7);
      *--pStartPacket = (U8)(NumBytes | 0x80);
    } else {
      *--pStartPacket = (U8)NumBytes;
    }
#endif
    //
    // Prepend EventId.
    //
#if SEGGER_SYSVIEW_SUPPORT_LONG_ID
    if (EventId < 127) {
      *--pStartPacket = (U8)EventId;
    } else {
      //
      // Backwards U32 encode EventId.
      //
      if (EventId < (1u << 14)) { // Encodes in 2 bytes
        *--pStartPacket = (U8)(EventId >>  7);
        *--pStartPacket = (U8)(EventId | 0x80);
      } else if (EventId < (1ul << 21)) {    // Encodes in 3 bytes
        *--pStartPacket = (U8)(EventId >> 14);
        *--pStartPacket = (U8)((EventId >>  7) | 0x80);
        *--pStartPacket = (U8)(EventId | 0x80);
      } else if (EventId < (1ul << 28)) {    // Encodes in 4 bytes
        *--pStartPacket = (U8)(EventId >> 21);
        *--pStartPacket = (U8)((EventId >> 14) | 0x80);
        *--pStartPacket = (U8)((EventId >>  7) | 0x80);
        *--pStartPacket = (U8)(EventId | 0x80);
      } else {                              // Encodes in 5 bytes
        *--pStartPacket = (U8)(EventId >> 28);
        *--pStartPacket = (U8)((EventId >> 21) | 0x80);
        *--pStartPacket = (U8)((EventId >> 14) | 0x80);
        *--pStartPacket = (U8)((EventId >>  7) | 0x80);
        *--pStartPacket = (U8)(EventId | 0x80);
      }
    }
#else
    if (EventId > 127) {
      *--pStartPacket = (U8)(EventId >> 7);
      *--pStartPacket = (U8)(EventId | 0x80);
    } else {
      *--pStartPacket = (U8)EventId;
    }
#endif
  }
  //
  // Compute time stamp delta and append it to packet.
  //
  TimeStamp  = SEGGER_SYSVIEW_GET_TIMESTAMP();
  Delta = TimeStamp - _SYSVIEW_Globals.LastTxTimeStamp;
  MAKE_DELTA_32BIT(Delta);
  ENCODE_U32(pEndPacket, Delta);
#if (SEGGER_SYSVIEW_POST_MORTEM_MODE == 1)
  //
  // Store packet in RTT buffer by overwriting old data and update time stamp
  //
  SEGGER_RTT_WriteWithOverwriteNoLock(CHANNEL_ID_UP, pStartPacket, pEndPacket - pStartPacket);
  SEGGER_SYSVIEW_ON_EVENT_RECORDED(pEndPacket - pStartPacket);
  _SYSVIEW_Globals.LastTxTimeStamp = TimeStamp;
#else
  //
  // Try to store packet in RTT buffer and update time stamp when this was successful
  //
  Status = SEGGER_RTT_WriteSkipNoLock(CHANNEL_ID_UP, pStartPacket, (unsigned int)(pEndPacket - pStartPacket));
  SEGGER_SYSVIEW_ON_EVENT_RECORDED(pEndPacket - pStartPacket);
  if (Status) {
    _SYSVIEW_Globals.LastTxTimeStamp = TimeStamp;
  } else {
    _SYSVIEW_Globals.EnableState++; // EnableState has been 1, will be 2. Always.
  }
#endif

#if (SEGGER_SYSVIEW_POST_MORTEM_MODE == 1)
  //
  // Add sync and system information periodically if we are in post mortem mode
  //
  if (_SYSVIEW_Globals.RecursionCnt == 0) {   // Avoid uncontrolled nesting. This way, this routine can call itself once, but no more often than that.
    _SYSVIEW_Globals.RecursionCnt = 1;
    if (_SYSVIEW_Globals.PacketCount++ & (1 << SEGGER_SYSVIEW_SYNC_PERIOD_SHIFT)) {
      _SendSyncInfo();
      _SYSVIEW_Globals.PacketCount = 0;
    }
    _SYSVIEW_Globals.RecursionCnt = 0;
  }
SendDone:
  ; // Avoid "label at end of compound statement" error when using static buffer
#else
SendDone:
  //
  // Check if host is sending data which needs to be processed.
  // Note that since this code is called for every packet, it is very time critical, so we do
  // only what is really needed here, which is checking if there is any data
  //
  if (SEGGER_RTT_HASDATA(CHANNEL_ID_DOWN)) {
    if (_SYSVIEW_Globals.RecursionCnt == 0) {   // Avoid uncontrolled nesting. This way, this routine can call itself once, but no more often than that.
      _SYSVIEW_Globals.RecursionCnt = 1;
      _HandleIncomingPacket();
      _SYSVIEW_Globals.RecursionCnt = 0;
    }
  }
#endif
  //
#if (SEGGER_SYSVIEW_USE_STATIC_BUFFER == 0)
  SEGGER_SYSVIEW_UNLOCK();  // We are done. Unlock and return
#endif
}

#ifndef SEGGER_SYSVIEW_EXCLUDE_PRINTF // Define in project to avoid warnings about variable parameter list
/*********************************************************************
*
*       _VPrintHost()
*
*  Function description
*    Send a format string and its parameters to the host.
*
*  Parameters
*    s            Pointer to format string.
*    Options      Options to be sent to the host.
*    pParamList   Pointer to the list of arguments for the format string.
*/
static int _VPrintHost(const char* s, U32 Options, va_list* pParamList) {
  U32         aParas[SEGGER_SYSVIEW_MAX_ARGUMENTS];
  U32*        pParas;
  U32         NumArguments;
  const char* p;
  char        c;
  U8*         pPayload;
  U8*         pPayloadStart;
#if SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT
  U8 HasNonScalar;

  HasNonScalar = 0;
#endif
  //
  // Count number of arguments by counting '%' characters in string.
  // If enabled, check for non-scalar modifier flags to format string on the target.
  //
  p = s;
  NumArguments = 0;
  for (;;) {
    c = *p++;
    if (c == 0) {
      break;
    }
    if (c == '%') {
      c = *p;
#if SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT == 0
      aParas[NumArguments++] = (U32)(va_arg(*pParamList, int));
      if (NumArguments == SEGGER_SYSVIEW_MAX_ARGUMENTS) {
        break;
      }
#else
      if (c == 's') {
        HasNonScalar = 1;
        break;
      } else {
        aParas[NumArguments++] = (U32)(va_arg(*pParamList, int));
        if (NumArguments == SEGGER_SYSVIEW_MAX_ARGUMENTS) {
          break;
        }
      }
#endif
    }
  }

#if SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT
  if (HasNonScalar) {
    return -1;
  }
#endif
  //
  // Send string and parameters to host
  //
  {
    RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_MAX_STRING_LEN + 2 * SEGGER_SYSVIEW_QUANTA_U32 + SEGGER_SYSVIEW_MAX_ARGUMENTS * SEGGER_SYSVIEW_QUANTA_U32);
    pPayload = _EncodeStr(pPayloadStart, s, SEGGER_SYSVIEW_MAX_STRING_LEN);
    ENCODE_U32(pPayload, Options);
    ENCODE_U32(pPayload, NumArguments);
    pParas = aParas;
    while (NumArguments--) {
      ENCODE_U32(pPayload, (*pParas));
      pParas++;
    }
    _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_PRINT_FORMATTED);
    RECORD_END();
  }
  return 0;
}

/*********************************************************************
*
*       _StoreChar()
*
*  Function description
*    Stores a character in the printf-buffer and sends the buffer when
*     it is filled.
*
*  Parameters
*    p            Pointer to the buffer description.
*    c            Character to be printed.
*/
static void _StoreChar(SEGGER_SYSVIEW_PRINTF_DESC * p, char c) {
  unsigned int  Cnt;
  U8*           pPayload;
  U32           Options;

  Cnt = p->Cnt;
  if ((Cnt + 1u) <= SEGGER_SYSVIEW_MAX_STRING_LEN) {
    *(p->pPayload++) = (U8)c;
    p->Cnt = Cnt + 1u;
  }
  //
  // Write part of string, when the buffer is full
  //
  if (p->Cnt == SEGGER_SYSVIEW_MAX_STRING_LEN) {
    *(p->pPayloadStart) = (U8)p->Cnt;
    pPayload = p->pPayload;
    Options = p->Options;
    ENCODE_U32(pPayload, Options);
    ENCODE_U32(pPayload, 0);
    _SendPacket(p->pPayloadStart, pPayload, SYSVIEW_EVTID_PRINT_FORMATTED);
    p->pPayloadStart = _PreparePacket(p->pBuffer);
    p->pPayload = p->pPayloadStart + 1u;
    p->Cnt = 0u;
  }
}

/*********************************************************************
*
*       _PrintUnsigned()
*
*  Function description
*    Print an unsigned integer with the given formatting into the
*     formatted string.
*
*  Parameters
*    pBufferDesc  Pointer to the buffer description.
*    v            Value to be printed.
*    Base         Base of the value.
*    NumDigits    Number of digits to be printed.
*    FieldWidth   Width of the printed field.
*    FormatFlags  Flags for formatting the value.
*/
static void _PrintUnsigned(SEGGER_SYSVIEW_PRINTF_DESC * pBufferDesc, unsigned int v, unsigned int Base, unsigned int NumDigits, unsigned int FieldWidth, unsigned int FormatFlags) {
  static const char _aV2C[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  unsigned int      Div;
  unsigned int      Digit;
  unsigned int      Number;
  unsigned int      Width;
  char              c;

  Number = v;
  Digit = 1u;
  //
  // Get actual field width
  //
  Width = 1u;
  while (Number >= Base) {
    Number = (Number / Base);
    Width++;
  }
  if (NumDigits > Width) {
    Width = NumDigits;
  }
  //
  // Print leading chars if necessary
  //
  if ((FormatFlags & FORMAT_FLAG_LEFT_JUSTIFY) == 0u) {
    if (FieldWidth != 0u) {
      if (((FormatFlags & FORMAT_FLAG_PAD_ZERO) == FORMAT_FLAG_PAD_ZERO) && (NumDigits == 0u)) {
        c = '0';
      } else {
        c = ' ';
      }
      while ((FieldWidth != 0u) && (Width < FieldWidth)) {
        FieldWidth--;
        _StoreChar(pBufferDesc, c);
      }
    }
  }
  //
  // Compute Digit.
  // Loop until Digit has the value of the highest digit required.
  // Example: If the output is 345 (Base 10), loop 2 times until Digit is 100.
  //
  while (1) {
    if (NumDigits > 1u) {       // User specified a min number of digits to print? => Make sure we loop at least that often, before checking anything else (> 1 check avoids problems with NumDigits being signed / unsigned)
      NumDigits--;
    } else {
      Div = v / Digit;
      if (Div < Base) {        // Is our divider big enough to extract the highest digit from value? => Done
        break;
      }
    }
    Digit *= Base;
  }
  //
  // Output digits
  //
  do {
    Div = v / Digit;
    v -= Div * Digit;
    _StoreChar(pBufferDesc, _aV2C[Div]);
    Digit /= Base;
  } while (Digit);
  //
  // Print trailing spaces if necessary
  //
  if ((FormatFlags & FORMAT_FLAG_LEFT_JUSTIFY) == FORMAT_FLAG_LEFT_JUSTIFY) {
    if (FieldWidth != 0u) {
      while ((FieldWidth != 0u) && (Width < FieldWidth)) {
        FieldWidth--;
        _StoreChar(pBufferDesc, ' ');
      }
    }
  }
}

/*********************************************************************
*
*       _PrintInt()
*
*  Function description
*    Print a signed integer with the given formatting into the
*     formatted string.
*
*  Parameters
*    pBufferDesc  Pointer to the buffer description.
*    v            Value to be printed.
*    Base         Base of the value.
*    NumDigits    Number of digits to be printed.
*    FieldWidth   Width of the printed field.
*    FormatFlags  Flags for formatting the value.
*/
static void _PrintInt(SEGGER_SYSVIEW_PRINTF_DESC * pBufferDesc, int v, unsigned int Base, unsigned int NumDigits, unsigned int FieldWidth, unsigned int FormatFlags) {
  unsigned int  Width;
  int           Number;

  Number = (v < 0) ? -v : v;

  //
  // Get actual field width
  //
  Width = 1u;
  while (Number >= (int)Base) {
    Number = (Number / (int)Base);
    Width++;
  }
  if (NumDigits > Width) {
    Width = NumDigits;
  }
  if ((FieldWidth > 0u) && ((v < 0) || ((FormatFlags & FORMAT_FLAG_PRINT_SIGN) == FORMAT_FLAG_PRINT_SIGN))) {
    FieldWidth--;
  }

  //
  // Print leading spaces if necessary
  //
  if ((((FormatFlags & FORMAT_FLAG_PAD_ZERO) == 0u) || (NumDigits != 0u)) && ((FormatFlags & FORMAT_FLAG_LEFT_JUSTIFY) == 0u)) {
    if (FieldWidth != 0u) {
      while ((FieldWidth != 0u) && (Width < FieldWidth)) {
        FieldWidth--;
        _StoreChar(pBufferDesc, ' ');
      }
    }
  }
  //
  // Print sign if necessary
  //
  if (v < 0) {
    v = -v;
    _StoreChar(pBufferDesc, '-');
  } else if ((FormatFlags & FORMAT_FLAG_PRINT_SIGN) == FORMAT_FLAG_PRINT_SIGN) {
    _StoreChar(pBufferDesc, '+');
  } else {

  }
  //
  // Print leading zeros if necessary
  //
  if (((FormatFlags & FORMAT_FLAG_PAD_ZERO) == FORMAT_FLAG_PAD_ZERO) && ((FormatFlags & FORMAT_FLAG_LEFT_JUSTIFY) == 0u) && (NumDigits == 0u)) {
    if (FieldWidth != 0u) {
      while ((FieldWidth != 0u) && (Width < FieldWidth)) {
        FieldWidth--;
        _StoreChar(pBufferDesc, '0');
      }
    }
  }
  //
  // Print number without sign
  //
  _PrintUnsigned(pBufferDesc, (unsigned int)v, Base, NumDigits, FieldWidth, FormatFlags);
}

/*********************************************************************
*
*       _VPrintTarget()
*
*  Function description
*    Stores a formatted string.
*    This data is read by the host.
*
*  Parameters
*    sFormat      Pointer to format string.
*    Options      Options to be sent to the host.
*    pParamList   Pointer to the list of arguments for the format string.
*/
static void _VPrintTarget(const char* sFormat, U32 Options, va_list* pParamList) {
  SEGGER_SYSVIEW_PRINTF_DESC BufferDesc;
  char          c;
  int           v;
  unsigned int  NumDigits;
  unsigned int  FormatFlags;
  unsigned int  FieldWidth;
  U8*           pPayloadStart;
#if SEGGER_SYSVIEW_USE_STATIC_BUFFER == 0
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_MAX_STRING_LEN + 1 + 2 * SEGGER_SYSVIEW_QUANTA_U32);
  SEGGER_SYSVIEW_LOCK();
#else
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_MAX_STRING_LEN + 1 + 2 * SEGGER_SYSVIEW_QUANTA_U32);
#endif

#if SEGGER_SYSVIEW_USE_STATIC_BUFFER == 0
  BufferDesc.pBuffer        = aPacket;
#else
  BufferDesc.pBuffer        = _aPacket;
#endif
  BufferDesc.Cnt            = 0u;
  BufferDesc.pPayloadStart  = pPayloadStart;
  BufferDesc.pPayload       = BufferDesc.pPayloadStart + 1u;
  BufferDesc.Options        =  Options;

  do {
    c = *sFormat;
    sFormat++;
    if (c == 0u) {
      break;
    }
    if (c == '%') {
      //
      // Filter out flags
      //
      FormatFlags = 0u;
      v = 1;
      do {
        c = *sFormat;
        switch (c) {
        case '-': FormatFlags |= FORMAT_FLAG_LEFT_JUSTIFY; sFormat++; break;
        case '0': FormatFlags |= FORMAT_FLAG_PAD_ZERO;     sFormat++; break;
        case '+': FormatFlags |= FORMAT_FLAG_PRINT_SIGN;   sFormat++; break;
        case '#': FormatFlags |= FORMAT_FLAG_ALTERNATE;    sFormat++; break;
        default:  v = 0; break;
        }
      } while (v);
      //
      // filter out field with
      //
      FieldWidth = 0u;
      do {
        c = *sFormat;
        if ((c < '0') || (c > '9')) {
          break;
        }
        sFormat++;
        FieldWidth = (FieldWidth * 10u) + ((unsigned int)c - '0');
      } while (1);

      //
      // Filter out precision (number of digits to display)
      //
      NumDigits = 0u;
      c = *sFormat;
      if (c == '.') {
        sFormat++;
        do {
          c = *sFormat;
          if ((c < '0') || (c > '9')) {
            break;
          }
          sFormat++;
          NumDigits = NumDigits * 10u + ((unsigned int)c - '0');
        } while (1);
      }
      //
      // Filter out length modifier
      //
      c = *sFormat;
      do {
        if ((c == 'l') || (c == 'h')) {
          c = *sFormat;
          sFormat++;
        } else {
          break;
        }
      } while (1);
      //
      // Handle specifiers
      //
      switch (c) {
      case 'c': {
        char c0;
        v = va_arg(*pParamList, int);
        c0 = (char)v;
        _StoreChar(&BufferDesc, c0);
        break;
      }
      case 'd':
        v = va_arg(*pParamList, int);
        _PrintInt(&BufferDesc, v, 10u, NumDigits, FieldWidth, FormatFlags);
        break;
      case 'u':
        v = va_arg(*pParamList, int);
        _PrintUnsigned(&BufferDesc, (unsigned int)v, 10u, NumDigits, FieldWidth, FormatFlags);
        break;
      case 'x':
      case 'X':
        v = va_arg(*pParamList, int);
        _PrintUnsigned(&BufferDesc, (unsigned int)v, 16u, NumDigits, FieldWidth, FormatFlags);
        break;
      case 'p':
        v = va_arg(*pParamList, int);
        _PrintUnsigned(&BufferDesc, (unsigned int)v, 16u, 8u, 8u, 0u);
        break;
      case '%':
        _StoreChar(&BufferDesc, '%');
        break;
      default:
        break;
      }
      sFormat++;
    } else {
      _StoreChar(&BufferDesc, c);
    }
  } while (*sFormat);

  //
  // Write remaining data, if any
  //
  if (BufferDesc.Cnt != 0u) {
    *(BufferDesc.pPayloadStart) = (U8)BufferDesc.Cnt;
    ENCODE_U32(BufferDesc.pPayload, BufferDesc.Options);
    ENCODE_U32(BufferDesc.pPayload, 0);
    _SendPacket(BufferDesc.pPayloadStart, BufferDesc.pPayload, SYSVIEW_EVTID_PRINT_FORMATTED);
  }
#if SEGGER_SYSVIEW_USE_STATIC_BUFFER == 0
  SEGGER_SYSVIEW_UNLOCK();
  RECORD_END();
#else
  RECORD_END();
#endif
}
#endif // SEGGER_SYSVIEW_EXCLUDE_PRINTF

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       SEGGER_SYSVIEW_Init()
*
*  Function description
*    Initializes the SYSVIEW module.
*    Must be called before the SystemView Application connects to
*    the system.
*
*  Parameters
*    SysFreq        - Frequency of timestamp, usually CPU core clock frequency.
*    CPUFreq        - CPU core clock frequency.
*    pOSAPI         - Pointer to the API structure for OS-specific functions.
*    pfSendSysDesc  - Pointer to record system description callback function.
*
*  Additional information
*    This function initializes the RTT channel used to transport
*    SEGGER SystemView packets.
*    The channel is assigned the label "SysView" for client software
*    to identify the SystemView channel.
*
*    The channel is configured with the macro SEGGER_SYSVIEW_RTT_CHANNEL.
*/
void SEGGER_SYSVIEW_Init(U32 SysFreq, U32 CPUFreq, const SEGGER_SYSVIEW_OS_API *pOSAPI, SEGGER_SYSVIEW_SEND_SYS_DESC_FUNC pfSendSysDesc) {
#ifdef SEGGER_RTT_SECTION
  //
  // Explicitly initialize the RTT Control Block if it is in its dedicated section.
  //
  SEGGER_RTT_Init();
#endif
#if (SEGGER_SYSVIEW_POST_MORTEM_MODE == 1)
#if SEGGER_SYSVIEW_RTT_CHANNEL > 0
  SEGGER_RTT_ConfigUpBuffer(SEGGER_SYSVIEW_RTT_CHANNEL, "SysView", &_UpBuffer[0],   sizeof(_UpBuffer),   SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#else
  _SYSVIEW_Globals.UpChannel = (U8)SEGGER_RTT_AllocUpBuffer  ("SysView", &_UpBuffer[0],   sizeof(_UpBuffer),   SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#endif
  _SYSVIEW_Globals.RAMBaseAddress   = SEGGER_SYSVIEW_ID_BASE;
  _SYSVIEW_Globals.LastTxTimeStamp  = SEGGER_SYSVIEW_GET_TIMESTAMP();
  _SYSVIEW_Globals.pOSAPI           = pOSAPI;
  _SYSVIEW_Globals.SysFreq          = SysFreq;
  _SYSVIEW_Globals.CPUFreq          = CPUFreq;
  _SYSVIEW_Globals.pfSendSysDesc    = pfSendSysDesc;
  _SYSVIEW_Globals.EnableState      = 0;
  _SYSVIEW_Globals.PacketCount      = 0;
#else // (SEGGER_SYSVIEW_POST_MORTEM_MODE == 1)
#if SEGGER_SYSVIEW_RTT_CHANNEL > 0
  SEGGER_RTT_ConfigUpBuffer   (SEGGER_SYSVIEW_RTT_CHANNEL, "SysView", &_UpBuffer[0],   sizeof(_UpBuffer),   SEGGER_RTT_MODE_NO_BLOCK_SKIP);
  SEGGER_RTT_ConfigDownBuffer (SEGGER_SYSVIEW_RTT_CHANNEL, "SysView", &_DownBuffer[0], sizeof(_DownBuffer), SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#else
  _SYSVIEW_Globals.UpChannel = (U8)SEGGER_RTT_AllocUpBuffer  ("SysView", &_UpBuffer[0],   sizeof(_UpBuffer),   SEGGER_RTT_MODE_NO_BLOCK_SKIP);
  _SYSVIEW_Globals.DownChannel = _SYSVIEW_Globals.UpChannel;
  SEGGER_RTT_ConfigDownBuffer (_SYSVIEW_Globals.DownChannel, "SysView", &_DownBuffer[0], sizeof(_DownBuffer), SEGGER_RTT_MODE_NO_BLOCK_SKIP);
#endif
  _SYSVIEW_Globals.RAMBaseAddress   = SEGGER_SYSVIEW_ID_BASE;
  _SYSVIEW_Globals.LastTxTimeStamp  = SEGGER_SYSVIEW_GET_TIMESTAMP();
  _SYSVIEW_Globals.pOSAPI           = pOSAPI;
  _SYSVIEW_Globals.SysFreq          = SysFreq;
  _SYSVIEW_Globals.CPUFreq          = CPUFreq;
  _SYSVIEW_Globals.pfSendSysDesc    = pfSendSysDesc;
  _SYSVIEW_Globals.EnableState      = 0;
#endif  // (SEGGER_SYSVIEW_POST_MORTEM_MODE == 1)
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_SetRAMBase()
*
*  Function description
*    Sets the RAM base address, which is subtracted from IDs in order
*     to save bandwidth.
*
*  Parameters
*    RAMBaseAddress - Lowest RAM Address. (i.e. 0x20000000 on most Cortex-M)
*/
void SEGGER_SYSVIEW_SetRAMBase(U32 RAMBaseAddress) {
  _SYSVIEW_Globals.RAMBaseAddress = RAMBaseAddress;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordVoid()
*
*  Function description
*    Formats and sends a SystemView packet with an empty payload.
*
*  Parameters
*    EventID - SystemView event ID.
*/
void SEGGER_SYSVIEW_RecordVoid(unsigned int EventID) {
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE);
  //
  _SendPacket(pPayloadStart, pPayloadStart, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordU32()
*
*  Function description
*    Formats and sends a SystemView packet containing a single U32
*    parameter payload.
*
*  Parameters
*    EventID - SystemView event ID.
*    Value   - The 32-bit parameter encoded to SystemView packet payload.
*/
void SEGGER_SYSVIEW_RecordU32(unsigned int EventID, U32 Value) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, Value);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordU32x2()
*
*  Function description
*    Formats and sends a SystemView packet containing 2 U32 parameter payload.
*
*  Parameters
*    EventID - SystemView event ID.
*    Para0   - The 32-bit parameter encoded to SystemView packet payload.
*    Para1   - The 32-bit parameter encoded to SystemView packet payload.
*/
void SEGGER_SYSVIEW_RecordU32x2(unsigned int EventID, U32 Para0, U32 Para1) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, Para0);
  ENCODE_U32(pPayload, Para1);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordU32x3()
*
*  Function description
*    Formats and sends a SystemView packet containing 3 U32 parameter payload.
*
*  Parameters
*    EventID - SystemView event ID.
*    Para0   - The 32-bit parameter encoded to SystemView packet payload.
*    Para1   - The 32-bit parameter encoded to SystemView packet payload.
*    Para2   - The 32-bit parameter encoded to SystemView packet payload.
*/
void SEGGER_SYSVIEW_RecordU32x3(unsigned int EventID, U32 Para0, U32 Para1, U32 Para2) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 3 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, Para0);
  ENCODE_U32(pPayload, Para1);
  ENCODE_U32(pPayload, Para2);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordU32x4()
*
*  Function description
*    Formats and sends a SystemView packet containing 4 U32 parameter payload.
*
*  Parameters
*    EventID - SystemView event ID.
*    Para0   - The 32-bit parameter encoded to SystemView packet payload.
*    Para1   - The 32-bit parameter encoded to SystemView packet payload.
*    Para2   - The 32-bit parameter encoded to SystemView packet payload.
*    Para3   - The 32-bit parameter encoded to SystemView packet payload.
*/
void SEGGER_SYSVIEW_RecordU32x4(unsigned int EventID, U32 Para0, U32 Para1, U32 Para2, U32 Para3) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 4 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, Para0);
  ENCODE_U32(pPayload, Para1);
  ENCODE_U32(pPayload, Para2);
  ENCODE_U32(pPayload, Para3);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordU32x5()
*
*  Function description
*    Formats and sends a SystemView packet containing 5 U32 parameter payload.
*
*  Parameters
*    EventID - SystemView event ID.
*    Para0   - The 32-bit parameter encoded to SystemView packet payload.
*    Para1   - The 32-bit parameter encoded to SystemView packet payload.
*    Para2   - The 32-bit parameter encoded to SystemView packet payload.
*    Para3   - The 32-bit parameter encoded to SystemView packet payload.
*    Para4   - The 32-bit parameter encoded to SystemView packet payload.
*/
void SEGGER_SYSVIEW_RecordU32x5(unsigned int EventID, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 5 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, Para0);
  ENCODE_U32(pPayload, Para1);
  ENCODE_U32(pPayload, Para2);
  ENCODE_U32(pPayload, Para3);
  ENCODE_U32(pPayload, Para4);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordU32x6()
*
*  Function description
*    Formats and sends a SystemView packet containing 6 U32 parameter payload.
*
*  Parameters
*    EventID - SystemView event ID.
*    Para0   - The 32-bit parameter encoded to SystemView packet payload.
*    Para1   - The 32-bit parameter encoded to SystemView packet payload.
*    Para2   - The 32-bit parameter encoded to SystemView packet payload.
*    Para3   - The 32-bit parameter encoded to SystemView packet payload.
*    Para4   - The 32-bit parameter encoded to SystemView packet payload.
*    Para5   - The 32-bit parameter encoded to SystemView packet payload.
*/
void SEGGER_SYSVIEW_RecordU32x6(unsigned int EventID, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 6 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, Para0);
  ENCODE_U32(pPayload, Para1);
  ENCODE_U32(pPayload, Para2);
  ENCODE_U32(pPayload, Para3);
  ENCODE_U32(pPayload, Para4);
  ENCODE_U32(pPayload, Para5);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordU32x7()
*
*  Function description
*    Formats and sends a SystemView packet containing 7 U32 parameter payload.
*
*  Parameters
*    EventID - SystemView event ID.
*    Para0   - The 32-bit parameter encoded to SystemView packet payload.
*    Para1   - The 32-bit parameter encoded to SystemView packet payload.
*    Para2   - The 32-bit parameter encoded to SystemView packet payload.
*    Para3   - The 32-bit parameter encoded to SystemView packet payload.
*    Para4   - The 32-bit parameter encoded to SystemView packet payload.
*    Para5   - The 32-bit parameter encoded to SystemView packet payload.
*    Para6   - The 32-bit parameter encoded to SystemView packet payload.
*/
void SEGGER_SYSVIEW_RecordU32x7(unsigned int EventID, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5, U32 Para6) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 7 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, Para0);
  ENCODE_U32(pPayload, Para1);
  ENCODE_U32(pPayload, Para2);
  ENCODE_U32(pPayload, Para3);
  ENCODE_U32(pPayload, Para4);
  ENCODE_U32(pPayload, Para5);
  ENCODE_U32(pPayload, Para6);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordU32x8()
*
*  Function description
*    Formats and sends a SystemView packet containing 8 U32 parameter payload.
*
*  Parameters
*    EventID - SystemView event ID.
*    Para0   - The 32-bit parameter encoded to SystemView packet payload.
*    Para1   - The 32-bit parameter encoded to SystemView packet payload.
*    Para2   - The 32-bit parameter encoded to SystemView packet payload.
*    Para3   - The 32-bit parameter encoded to SystemView packet payload.
*    Para4   - The 32-bit parameter encoded to SystemView packet payload.
*    Para5   - The 32-bit parameter encoded to SystemView packet payload.
*    Para6   - The 32-bit parameter encoded to SystemView packet payload.
*    Para7   - The 32-bit parameter encoded to SystemView packet payload.
*/
void SEGGER_SYSVIEW_RecordU32x8(unsigned int EventID, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5, U32 Para6, U32 Para7) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 8 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, Para0);
  ENCODE_U32(pPayload, Para1);
  ENCODE_U32(pPayload, Para2);
  ENCODE_U32(pPayload, Para3);
  ENCODE_U32(pPayload, Para4);
  ENCODE_U32(pPayload, Para5);
  ENCODE_U32(pPayload, Para6);
  ENCODE_U32(pPayload, Para7);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordU32x9()
*
*  Function description
*    Formats and sends a SystemView packet containing 9 U32 parameter payload.
*
*  Parameters
*    EventID - SystemView event ID.
*    Para0   - The 32-bit parameter encoded to SystemView packet payload.
*    Para1   - The 32-bit parameter encoded to SystemView packet payload.
*    Para2   - The 32-bit parameter encoded to SystemView packet payload.
*    Para3   - The 32-bit parameter encoded to SystemView packet payload.
*    Para4   - The 32-bit parameter encoded to SystemView packet payload.
*    Para5   - The 32-bit parameter encoded to SystemView packet payload.
*    Para6   - The 32-bit parameter encoded to SystemView packet payload.
*    Para7   - The 32-bit parameter encoded to SystemView packet payload.
*    Para8   - The 32-bit parameter encoded to SystemView packet payload.
*/
void SEGGER_SYSVIEW_RecordU32x9(unsigned int EventID, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5, U32 Para6, U32 Para7, U32 Para8) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 9 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, Para0);
  ENCODE_U32(pPayload, Para1);
  ENCODE_U32(pPayload, Para2);
  ENCODE_U32(pPayload, Para3);
  ENCODE_U32(pPayload, Para4);
  ENCODE_U32(pPayload, Para5);
  ENCODE_U32(pPayload, Para6);
  ENCODE_U32(pPayload, Para7);
  ENCODE_U32(pPayload, Para8);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordU32x10()
*
*  Function description
*    Formats and sends a SystemView packet containing 10 U32 parameter payload.
*
*  Parameters
*    EventID - SystemView event ID.
*    Para0   - The 32-bit parameter encoded to SystemView packet payload.
*    Para1   - The 32-bit parameter encoded to SystemView packet payload.
*    Para2   - The 32-bit parameter encoded to SystemView packet payload.
*    Para3   - The 32-bit parameter encoded to SystemView packet payload.
*    Para4   - The 32-bit parameter encoded to SystemView packet payload.
*    Para5   - The 32-bit parameter encoded to SystemView packet payload.
*    Para6   - The 32-bit parameter encoded to SystemView packet payload.
*    Para7   - The 32-bit parameter encoded to SystemView packet payload.
*    Para8   - The 32-bit parameter encoded to SystemView packet payload.
*    Para9   - The 32-bit parameter encoded to SystemView packet payload.
*/
void SEGGER_SYSVIEW_RecordU32x10(unsigned int EventID, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5, U32 Para6, U32 Para7, U32 Para8, U32 Para9) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 10 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, Para0);
  ENCODE_U32(pPayload, Para1);
  ENCODE_U32(pPayload, Para2);
  ENCODE_U32(pPayload, Para3);
  ENCODE_U32(pPayload, Para4);
  ENCODE_U32(pPayload, Para5);
  ENCODE_U32(pPayload, Para6);
  ENCODE_U32(pPayload, Para7);
  ENCODE_U32(pPayload, Para8);
  ENCODE_U32(pPayload, Para9);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}
/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordString()
*
*  Function description
*    Formats and sends a SystemView packet containing a string.
*
*  Parameters
*    EventID - SystemView event ID.
*    pString - The string to be sent in the SystemView packet payload.
*
*  Additional information
*    The string is encoded as a count byte followed by the contents
*    of the string.
*    No more than SEGGER_SYSVIEW_MAX_STRING_LEN bytes will be encoded to the payload.
*/
void SEGGER_SYSVIEW_RecordString(unsigned int EventID, const char* pString) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 1 + SEGGER_SYSVIEW_MAX_STRING_LEN);
  //
  pPayload = _EncodeStr(pPayloadStart, pString, SEGGER_SYSVIEW_MAX_STRING_LEN);
  _SendPacket(pPayloadStart, pPayload, EventID);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_Start()
*
*  Function description
*    Start recording SystemView events.
*
*    This function is triggered by the SystemView Application on connect.
*    For single-shot or post-mortem mode recording, it needs to be called
*    by the application.
*
*  Additional information
*    This function enables transmission of SystemView packets recorded
*    by subsequent trace calls and records a SystemView Start event.
*
*    As part of start, a SystemView Init packet is sent, containing the system
*    frequency. The list of current tasks, the current system time and the
*    system description string is sent, too.
*
*  Notes
*    SEGGER_SYSVIEW_Start and SEGGER_SYSVIEW_Stop do not nest.
*    When SEGGER_SYSVIEW_CAN_RESTART is 1, each received start command
*    records the system information. This is required to enable restart
*    of recordings when SystemView unexpectedly disconnects without sending
*    a stop command before.
*/
void SEGGER_SYSVIEW_Start(void) {
#if (SEGGER_SYSVIEW_CAN_RESTART == 0)
  if (_SYSVIEW_Globals.EnableState == 0) {
#endif
    _SYSVIEW_Globals.EnableState = 1;
#if (SEGGER_SYSVIEW_POST_MORTEM_MODE == 1)
    _SendSyncInfo();
#else
    SEGGER_SYSVIEW_LOCK();
    SEGGER_RTT_WriteSkipNoLock(CHANNEL_ID_UP, _abSync, 10);
    SEGGER_SYSVIEW_UNLOCK();
    SEGGER_SYSVIEW_ON_EVENT_RECORDED(10);
    SEGGER_SYSVIEW_RecordVoid(SYSVIEW_EVTID_TRACE_START);
    {
      U8* pPayload;
      U8* pPayloadStart;
      RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 4 * SEGGER_SYSVIEW_QUANTA_U32);
      //
      pPayload = pPayloadStart;
      ENCODE_U32(pPayload, _SYSVIEW_Globals.SysFreq);
      ENCODE_U32(pPayload, _SYSVIEW_Globals.CPUFreq);
      ENCODE_U32(pPayload, _SYSVIEW_Globals.RAMBaseAddress);
      ENCODE_U32(pPayload, SEGGER_SYSVIEW_ID_SHIFT);
      _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_INIT);
      RECORD_END();
    }
    if (_SYSVIEW_Globals.pfSendSysDesc) {
      _SYSVIEW_Globals.pfSendSysDesc();
    }
    SEGGER_SYSVIEW_RecordSystime();
    SEGGER_SYSVIEW_SendTaskList();
    SEGGER_SYSVIEW_SendNumModules();
#endif
#if (SEGGER_SYSVIEW_CAN_RESTART == 0)
  }
#endif
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_Stop()
*
*  Function description
*    Stop recording SystemView events.
*
*    This function is triggered by the SystemView Application on disconnect.
*    For single-shot or postmortem mode recording, it can be called
*    by the application.
*
*  Additional information
*    This function disables transmission of SystemView packets recorded
*    by subsequent trace calls.  If transmission is enabled when
*    this function is called, a single SystemView Stop event is recorded
*    to the trace, send, and then trace transmission is halted.
*/
void SEGGER_SYSVIEW_Stop(void) {
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE);
  // We should send answer for the Stop command in any case.
  _SYSVIEW_Globals.EnableState = 1;
  if (_SYSVIEW_Globals.EnableState) {
    _SendPacket(pPayloadStart, pPayloadStart, SYSVIEW_EVTID_TRACE_STOP);
    _SYSVIEW_Globals.EnableState = 0;
  }
  RECORD_END();
}

U8 SEGGER_SYSVIEW_Started(void) {
    return _SYSVIEW_Globals.EnableState;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_GetChannelID()
*
*  Function description
*    Returns the RTT <Up> / <Down> channel ID used by SystemView.
*/
int SEGGER_SYSVIEW_GetChannelID(void) {
  return CHANNEL_ID_UP;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_GetSysDesc()
*
*  Function description
*    Triggers a send of the system information and description.
*
*/
void SEGGER_SYSVIEW_GetSysDesc(void) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32 + SEGGER_SYSVIEW_MAX_STRING_LEN + 3/*1 or 3 bytes for string length*/);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, _SYSVIEW_Globals.SysFreq);
  ENCODE_U32(pPayload, _SYSVIEW_Globals.CPUFreq);
  ENCODE_U32(pPayload, _SYSVIEW_Globals.RAMBaseAddress);
  ENCODE_U32(pPayload, SEGGER_SYSVIEW_ID_SHIFT);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_INIT);
  RECORD_END();
  if (_SYSVIEW_Globals.pfSendSysDesc) {
    _SYSVIEW_Globals.pfSendSysDesc();
  }
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_SendTaskInfo()
*
*  Function description
*    Send a Task Info Packet, containing TaskId for identification,
*    task priority and task name.
*
*  Parameters
*    pInfo - Pointer to task information to send.
*/
void SEGGER_SYSVIEW_SendTaskInfo(const SEGGER_SYSVIEW_TASKINFO *pInfo) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32 + 1 + 32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, SHRINK_ID(pInfo->TaskID));
  ENCODE_U32(pPayload, pInfo->Prio);
  pPayload = _EncodeStr(pPayload, pInfo->sName, 32);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_TASK_INFO);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, SHRINK_ID(pInfo->TaskID));
  ENCODE_U32(pPayload, pInfo->StackBase);
  ENCODE_U32(pPayload, pInfo->StackSize);
  ENCODE_U32(pPayload, 0); // Stack End, future use
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_STACK_INFO);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_SendTaskList()
*
*  Function description
*    Send all tasks descriptors to the host.
*/
void SEGGER_SYSVIEW_SendTaskList(void) {
  if (_SYSVIEW_Globals.pOSAPI && _SYSVIEW_Globals.pOSAPI->pfSendTaskList) {
    _SYSVIEW_Globals.pOSAPI->pfSendTaskList();
  }
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_SendSysDesc()
*
*  Function description
*    Send the system description string to the host.
*    The system description is used by the SystemView Application
*    to identify the current application and handle events accordingly.
*
*    The system description is usually called by the system description
*    callback, to ensure it is only sent when the SystemView Application
*    is connected.
*
*  Parameters
*    sSysDesc - Pointer to the 0-terminated system description string.
*
*  Additional information
*    One system description string may not exceed SEGGER_SYSVIEW_MAX_STRING_LEN characters.
*    Multiple description strings can be recorded.
*
*    The Following items can be described in a system description string.
*    Each item is identified by its identifier, followed by '=' and the value.
*    Items are separated by ','.
*/
void SEGGER_SYSVIEW_SendSysDesc(const char *sSysDesc) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 1 + SEGGER_SYSVIEW_MAX_STRING_LEN);
  //
  pPayload = _EncodeStr(pPayloadStart, sSysDesc, SEGGER_SYSVIEW_MAX_STRING_LEN);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_SYSDESC);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordSystime()
*
*  Function description
*    Formats and sends a SystemView Systime containing a single U64 or U32
*    parameter payload.
*/
void SEGGER_SYSVIEW_RecordSystime(void) {
  U64 Systime;
  // This code requeued because SystemView expect the answer from the device.
  // If there is no answer, then communication will be broken.
  U8 old_en = _SYSVIEW_Globals.EnableState;
  _SYSVIEW_Globals.EnableState = 1;

  if (_SYSVIEW_Globals.pOSAPI && _SYSVIEW_Globals.pOSAPI->pfGetTime) {
    Systime = _SYSVIEW_Globals.pOSAPI->pfGetTime();
    SEGGER_SYSVIEW_RecordU32x2(SYSVIEW_EVTID_SYSTIME_US,
                               (U32)(Systime),
                               (U32)(Systime >> 32));
  } else {
    SEGGER_SYSVIEW_RecordU32(SYSVIEW_EVTID_SYSTIME_CYCLES, SEGGER_SYSVIEW_GET_TIMESTAMP());
  }
  _SYSVIEW_Globals.EnableState = old_en;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordEnterISR()
*
*  Function description
*    Format and send an ISR entry event.
*
*  Additional information
*    Example packets sent
*      02 0F 50              // ISR(15) Enter. Timestamp is 80 (0x50)
*/
void SEGGER_SYSVIEW_RecordEnterISR(U32 IrqId) {
  unsigned v;
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  v = IrqId; // SEGGER_SYSVIEW_GET_INTERRUPT_ID();
  ENCODE_U32(pPayload, v);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_ISR_ENTER);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordExitISR()
*
*  Function description
*    Format and send an ISR exit event.
*
*  Additional information
*    Format as follows:
*      03 <TimeStamp>        // Max. packet len is 6
*
*    Example packets sent
*      03 20                // ISR Exit. Timestamp is 32 (0x20)
*/
void SEGGER_SYSVIEW_RecordExitISR(void) {
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE);
  //
  _SendPacket(pPayloadStart, pPayloadStart, SYSVIEW_EVTID_ISR_EXIT);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordExitISRToScheduler()
*
*  Function description
*    Format and send an ISR exit into scheduler event.
*
*  Additional information
*    Format as follows:
*      18 <TimeStamp>        // Max. packet len is 6
*
*    Example packets sent
*      18 20                // ISR Exit to Scheduler. Timestamp is 32 (0x20)
*/
void SEGGER_SYSVIEW_RecordExitISRToScheduler(void) {
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE);
  //
  _SendPacket(pPayloadStart, pPayloadStart, SYSVIEW_EVTID_ISR_TO_SCHEDULER);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordEnterTimer()
*
*  Function description
*    Format and send a Timer entry event.
*
*  Parameters
*    TimerId - Id of the timer which starts.
*/
void SEGGER_SYSVIEW_RecordEnterTimer(U32 TimerId) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, SHRINK_ID(TimerId));
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_TIMER_ENTER);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordExitTimer()
*
*  Function description
*    Format and send a Timer exit event.
*/
void SEGGER_SYSVIEW_RecordExitTimer(void) {
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE);
  //
  _SendPacket(pPayloadStart, pPayloadStart, SYSVIEW_EVTID_TIMER_EXIT);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordEndCall()
*
*  Function description
*    Format and send an End API Call event without return value.
*
*  Parameters
*    EventID - Id of API function which ends.
*/
void SEGGER_SYSVIEW_RecordEndCall(unsigned int EventID) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, EventID);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_END_CALL);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordEndCallU32()
*
*  Function description
*    Format and send an End API Call event with return value.
*
*  Parameters
*    EventID      - Id of API function which ends.
*    Para0        - Return value which will be returned by the API function.
*/
void SEGGER_SYSVIEW_RecordEndCallU32(unsigned int EventID, U32 Para0) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, EventID);
  ENCODE_U32(pPayload, Para0);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_END_CALL);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_OnIdle()
*
*  Function description
*    Record an Idle event.
*/
void SEGGER_SYSVIEW_OnIdle(void) {
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE);
  //
  _SendPacket(pPayloadStart, pPayloadStart, SYSVIEW_EVTID_IDLE);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_OnTaskCreate()
*
*  Function description
*    Record a Task Create event.  The Task Create event corresponds
*    to creating a task in the OS.
*
*  Parameters
*    TaskId        - Task ID of created task.
*/
void SEGGER_SYSVIEW_OnTaskCreate(U32 TaskId) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  TaskId = SHRINK_ID(TaskId);
  ENCODE_U32(pPayload, TaskId);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_TASK_CREATE);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_OnTaskTerminate()
*
*  Function description
*    Record a Task termination event.
*    The Task termination event corresponds to terminating a task in
*    the OS. If the TaskId is the currently active task,
*    SEGGER_SYSVIEW_OnTaskStopExec may be used, either.
*
*  Parameters
*    TaskId        - Task ID of terminated task.
*/
void SEGGER_SYSVIEW_OnTaskTerminate(U32 TaskId) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  TaskId = SHRINK_ID(TaskId);
  ENCODE_U32(pPayload, TaskId);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_TASK_TERMINATE);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_OnTaskStartExec()
*
*  Function description
*    Record a Task Start Execution event.  The Task Start event
*    corresponds to when a task has started to execute rather than
*    when it is ready to execute.
*
*  Parameters
*    TaskId - Task ID of task that started to execute.
*/
void SEGGER_SYSVIEW_OnTaskStartExec(U32 TaskId) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  TaskId = SHRINK_ID(TaskId);
  ENCODE_U32(pPayload, TaskId);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_TASK_START_EXEC);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_OnTaskStopExec()
*
*  Function description
*    Record a Task Stop Execution event.  The Task Stop event
*    corresponds to when a task stops executing and terminates.
*/
void SEGGER_SYSVIEW_OnTaskStopExec(void) {
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE);
  //
  _SendPacket(pPayloadStart, pPayloadStart, SYSVIEW_EVTID_TASK_STOP_EXEC);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_OnTaskStartReady()
*
*  Function description
*    Record a Task Start Ready event.
*
*  Parameters
*    TaskId - Task ID of task that started to execute.
*/
void SEGGER_SYSVIEW_OnTaskStartReady(U32 TaskId) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  TaskId = SHRINK_ID(TaskId);
  ENCODE_U32(pPayload, TaskId);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_TASK_START_READY);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_OnTaskStopReady()
*
*  Function description
*    Record a Task Stop Ready event.
*
*  Parameters
*    TaskId - Task ID of task that completed execution.
*    Cause  - Reason for task to stop (i.e. Idle/Sleep)
*/
void SEGGER_SYSVIEW_OnTaskStopReady(U32 TaskId, unsigned int Cause) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  TaskId = SHRINK_ID(TaskId);
  ENCODE_U32(pPayload, TaskId);
  ENCODE_U32(pPayload, Cause);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_TASK_STOP_READY);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_MarkStart()
*
*  Function description
*    Record a Performance Marker Start event to start measuring runtime.
*
*  Parameters
*    MarkerId  - User defined ID for the marker.
*/
void SEGGER_SYSVIEW_MarkStart(unsigned MarkerId) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, MarkerId);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_MARK_START);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_MarkStop()
*
*  Function description
*    Record a Performance Marker Stop event to stop measuring runtime.
*
*  Parameters
*    MarkerId  - User defined ID for the marker.
*/
void SEGGER_SYSVIEW_MarkStop(unsigned MarkerId) {
  U8 * pPayload;
  U8 * pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, MarkerId);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_MARK_STOP);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_Mark()
*
*  Function description
*    Record a Performance Marker intermediate event.
*
*  Parameters
*    MarkerId  - User defined ID for the marker.
*/
void SEGGER_SYSVIEW_Mark(unsigned int MarkerId) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, SYSVIEW_EVTID_EX_MARK);
  ENCODE_U32(pPayload, MarkerId);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_EX);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_NameMarker()
*
*  Function description
*    Send the name of a Performance Marker to be displayed in SystemView.
*
*    Marker names are usually set in the system description
*    callback, to ensure it is only sent when the SystemView Application
*    is connected.
*
*  Parameters
*    MarkerId   - User defined ID for the marker.
*    sName      - Pointer to the marker name. (Max. SEGGER_SYSVIEW_MAX_STRING_LEN Bytes)
*/
void SEGGER_SYSVIEW_NameMarker(unsigned int MarkerId, const char* sName) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32 + 1 + SEGGER_SYSVIEW_MAX_STRING_LEN);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, SYSVIEW_EVTID_EX_NAME_MARKER);
  ENCODE_U32(pPayload, MarkerId);
  pPayload = _EncodeStr(pPayload, sName, SEGGER_SYSVIEW_MAX_STRING_LEN);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_EX);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_NameResource()
*
*  Function description
*    Send the name of a resource to be displayed in SystemView.
*
*    Marker names are usually set in the system description
*    callback, to ensure it is only sent when the SystemView Application
*    is connected.
*
*  Parameters
*    ResourceId - Id of the resource to be named. i.e. its address.
*    sName      - Pointer to the resource name. (Max. SEGGER_SYSVIEW_MAX_STRING_LEN Bytes)
*/
void SEGGER_SYSVIEW_NameResource(U32 ResourceId, const char* sName) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + SEGGER_SYSVIEW_QUANTA_U32 + 1 + SEGGER_SYSVIEW_MAX_STRING_LEN);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, SHRINK_ID(ResourceId));
  pPayload = _EncodeStr(pPayload, sName, SEGGER_SYSVIEW_MAX_STRING_LEN);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_NAME_RESOURCE);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_HeapDefine()
*
*  Function description
*    Define heap.
*
*  Parameters
*    pHeap        - Pointer to heap control structure.
*    pBase        - Pointer to managed heap memory.
*    HeapSize     - Size of managed heap memory in bytes.
*    MetadataSize - Size of metadata associated with each heap allocation.
*
*  Additional information
*    SystemView can track allocations across multiple heaps.
*
*    HeapSize must be a multiple of the natural alignment unit of the
*    target.  This size is subject to compression, controlled by the
*    specific setting of SEGGER_SYSVIEW_ID_SHIFT.
*
*    MetadataSize defines the size of the per-allocation metadata.
*    For many heap implementations, the metadata size is a multiple of
*    the word size of the machine and typically contains the size
*    of the allocated block (used upon deallocation), optional
*    pointers to the preceding and/or following blocks, and optionally
*    a tag identifying the owner of the block.  Note that MetadataSize
*    is not compressed within the SystemView packet and is not
*    required to be a multiple of 1<<SEGGER_SYSVIEW_ID_SHIFT.
*/
void SEGGER_SYSVIEW_HeapDefine(void* pHeap, void *pBase, unsigned int HeapSize, unsigned int MetadataSize) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 4 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, SYSVIEW_EVTID_EX_HEAP_DEFINE);
  ENCODE_U32(pPayload, SHRINK_ID((U32)pHeap));
  ENCODE_U32(pPayload, SHRINK_ID((U32)pBase));
  ENCODE_U32(pPayload, HeapSize >> SEGGER_SYSVIEW_ID_SHIFT);
  ENCODE_U32(pPayload, MetadataSize);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_EX);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_HeapAlloc()
*
*  Function description
*    Record a system-heap allocation event.
*
*  Parameters
*    pHeap       - Pointer to heap where allocation was made.
*    pUserData   - Pointer to allocated user data.
*    UserDataLen - Size of block allocated to hold user data, excluding any metadata.
*
*  Additional information
*    The user data must be correctly aligned for the architecture, which
*    typically requires that the alignment is at least the alignment
*    of a double or a long long.  pUserData is, therefore, compressed by
*    shrinking as IDs are compressed, controlled by the specific setting
*    of SEGGER_SYSVIEW_ID_SHIFT.
*
*    In the same way, UserDataLen must reflect the size of the allocated
*    block, not the allocation size requested by the application.  This
*    size is also subject to compression, controlled by the specific setting
*    of SEGGER_SYSVIEW_ID_SHIFT.
*
*    As an example, assume the allocator is running on a Cortex-M device
*    with SEGGER_SYSVIEW_ID_SHIFT set to 2 (the word alignment of the device).
*    If a user requests an allocation of 5 bytes, a hypothetical heap
*    allocator could allocate a block with size 32 bytes for this.  The value
*    of UserDataLen sent to SystemView for recording should be 32, not 5,
*    and the 32 is compressed by shifting by two bits, the configured value
*    of SEGGER_SYSVIEW_ID_SHIFT, and describes the number of bytes that are
*    consumed from managed memory from which SystemView can calculate
*    accurate heap metrics.
*/
void SEGGER_SYSVIEW_HeapAlloc(void *pHeap, void* pUserData, unsigned int UserDataLen) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 3 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, SYSVIEW_EVTID_EX_HEAP_ALLOC);
  ENCODE_U32(pPayload, SHRINK_ID((U32)pHeap));
  ENCODE_U32(pPayload, SHRINK_ID((U32)pUserData));
  ENCODE_U32(pPayload, UserDataLen >> SEGGER_SYSVIEW_ID_SHIFT);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_EX);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_HeapAllocEx()
*
*  Function description
*    Record a per-heap allocation event.
*
*  Parameters
*    pHeap       - Pointer to heap where allocation was made.
*    pUserData   - Pointer to allocated user data.
*    UserDataLen - Size of block allocated to hold user data, excluding any metadata.
*    Tag         - Block tag, typically used to identify the owner of the block.
*
*  Additional information
*    The user data must be correctly aligned for the architecture, which
*    typically requires that the alignment is at least the alignment
*    of a double or a long long.  pUserData is, therefore, compressed by
*    shrinking as IDs are compressed, controlled by the specific setting
*    of SEGGER_SYSVIEW_ID_SHIFT.
*
*    In the same way, UserDataLen must reflect the size of the allocated
*    block, not the allocation size requested by the application.  This
*    size is also subject to compression, controlled by the specific setting
*    of SEGGER_SYSVIEW_ID_SHIFT.
*
*    As an example, assume the allocator is running on a Cortex-M device
*    with SEGGER_SYSVIEW_ID_SHIFT set to 2 (the word alignment of the device).
*    If a user requests an allocation of 5 bytes, a hypothetical heap
*    allocator could allocate a block with size 32 bytes for this.  The value
*    of UserDataLen sent to SystemView for recording should be 32, not 5,
*    and the 32 is compressed by shifting by two bits, the configured value
*    of SEGGER_SYSVIEW_ID_SHIFT, and describes the number of bytes that are
*    consumed from managed memory from which SystemView can calculate
*    accurate heap metrics.
*
*  See also
*    SEGGER_SYSVIEW_HeapAlloc().
*/
void SEGGER_SYSVIEW_HeapAllocEx(void *pHeap, void* pUserData, unsigned int UserDataLen, unsigned int Tag) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 5 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, SYSVIEW_EVTID_EX_HEAP_ALLOC_EX);
  ENCODE_U32(pPayload, SHRINK_ID((U32)pHeap));
  ENCODE_U32(pPayload, SHRINK_ID((U32)pUserData));
  ENCODE_U32(pPayload, UserDataLen >> SEGGER_SYSVIEW_ID_SHIFT);
  ENCODE_U32(pPayload, Tag);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_EX);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_HeapFree()
*
*  Function description
*    Record a heap deallocation event.
*
*  Parameters
*    pHeap     - Pointer to heap where allocation was made.
*    pUserData - Pointer to allocated user data.
*
*  Additional information
*    SystemViews track allocations and knows the size of the
*    allocated data.
*/
void SEGGER_SYSVIEW_HeapFree(void* pHeap, void* pUserData) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32);
  //
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, SYSVIEW_EVTID_EX_HEAP_FREE);
  ENCODE_U32(pPayload, SHRINK_ID((U32)pHeap));
  ENCODE_U32(pPayload, SHRINK_ID((U32)pUserData));
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_EX);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_SendPacket()
*
*  Function description
*    Send an event packet.
*
*  Parameters
*    pPacket      - Pointer to the start of the packet.
*    pPayloadEnd  - Pointer to the end of the payload.
*                   Make sure there are at least 5 bytes free after the payload.
*    EventId      - Id of the event packet.
*
*  Return value
*    !=0:  Success, Message sent.
*    ==0:  Buffer full, Message *NOT* sent.
*/
int SEGGER_SYSVIEW_SendPacket(U8* pPacket, U8* pPayloadEnd, unsigned int EventId) {
#if (SEGGER_SYSVIEW_USE_STATIC_BUFFER == 1)
  SEGGER_SYSVIEW_LOCK();
#endif
  _SendPacket(pPacket + 4, pPayloadEnd, EventId);
#if (SEGGER_SYSVIEW_USE_STATIC_BUFFER == 1)
  SEGGER_SYSVIEW_UNLOCK();
#endif
  return 0;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_EncodeU32()
*
*  Function description
*    Encode a U32 in variable-length format.
*
*  Parameters
*    pPayload - Pointer to where U32 will be encoded.
*    Value    - The 32-bit value to be encoded.
*
*  Return value
*    Pointer to the byte following the value, i.e. the first free
*    byte in the payload and the next position to store payload
*    content.
*/
U8* SEGGER_SYSVIEW_EncodeU32(U8* pPayload, U32 Value) {
  ENCODE_U32(pPayload, Value);
  return pPayload;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_EncodeString()
*
*  Function description
*    Encode a string in variable-length format.
*
*  Parameters
*    pPayload - Pointer to where string will be encoded.
*    s        - String to encode.
*    MaxLen   - Maximum number of characters to encode from string.
*
*  Return value
*    Pointer to the byte following the value, i.e. the first free
*    byte in the payload and the next position to store payload
*    content.
*
*  Additional information
*    The string is encoded as a count byte followed by the contents
*    of the string.
*    No more than 1 + MaxLen bytes will be encoded to the payload.
*/
U8* SEGGER_SYSVIEW_EncodeString(U8* pPayload, const char* s, unsigned int MaxLen) {
  return _EncodeStr(pPayload, s, MaxLen);
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_EncodeData()
*
*  Function description
*    Encode a byte buffer in variable-length format.
*
*  Parameters
*    pPayload - Pointer to where string will be encoded.
*    pSrc     - Pointer to data buffer to be encoded.
*    NumBytes - Number of bytes in the buffer to be encoded.
*
*  Return value
*    Pointer to the byte following the value, i.e. the first free
*    byte in the payload and the next position to store payload
*    content.
*
*  Additional information
*    The data is encoded as a count byte followed by the contents
*    of the data buffer.
*    Make sure NumBytes + 1 bytes are free for the payload.
*/
U8* SEGGER_SYSVIEW_EncodeData(U8 *pPayload, const char* pSrc, unsigned int NumBytes) {
  return _EncodeData(pPayload, pSrc, NumBytes);
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_EncodeId()
*
*  Function description
*    Encode a 32-bit Id in shrunken variable-length format.
*
*  Parameters
*    pPayload - Pointer to where the Id will be encoded.
*    Id       - The 32-bit value to be encoded.
*
*  Return value
*    Pointer to the byte following the value, i.e. the first free
*    byte in the payload and the next position to store payload
*    content.
*
*  Additional information
*    The parameters to shrink an Id can be configured in
*    SEGGER_SYSVIEW_Conf.h and via SEGGER_SYSVIEW_SetRAMBase().
*     SEGGER_SYSVIEW_ID_BASE: Lowest Id reported by the application.
*       (i.e. 0x20000000 when all Ids are an address in this RAM)
*     SEGGER_SYSVIEW_ID_SHIFT: Number of bits to shift the Id to
*       save bandwidth. (i.e. 2 when Ids are 4 byte aligned)
*/
U8* SEGGER_SYSVIEW_EncodeId(U8* pPayload, U32 Id) {
  Id = SHRINK_ID(Id);
  ENCODE_U32(pPayload, Id);
  return pPayload;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_ShrinkId()
*
*  Function description
*    Get the shrunken value of an Id for further processing like in
*    SEGGER_SYSVIEW_NameResource().
*
*  Parameters
*    Id       - The 32-bit value to be shrunken.
*
*  Return value
*    Shrunken Id.
*
*  Additional information
*    The parameters to shrink an Id can be configured in
*    SEGGER_SYSVIEW_Conf.h and via SEGGER_SYSVIEW_SetRAMBase().
*     SEGGER_SYSVIEW_ID_BASE: Lowest Id reported by the application.
*       (i.e. 0x20000000 when all Ids are an address in this RAM)
*     SEGGER_SYSVIEW_ID_SHIFT: Number of bits to shift the Id to
*       save bandwidth. (i.e. 2 when Ids are 4 byte aligned)
*/
U32 SEGGER_SYSVIEW_ShrinkId(U32 Id) {
  return SHRINK_ID(Id);
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RegisterModule()
*
*  Function description
*    Register a middleware module for recording its events.
*
*  Parameters
*    pModule  - The middleware module information.
*
*  Additional information
*    SEGGER_SYSVIEW_MODULE elements:
*      sDescription      - Pointer to a string containing the module name and optionally the module event description.
*      NumEvents         - Number of events the module wants to register.
*      EventOffset       - Offset to be added to the event Ids. Out parameter, set by this function. Do not modify after calling this function.
*      pfSendModuleDesc  - Callback function pointer to send more detailed module description to SystemView Application.
*      pNext             - Pointer to next registered module. Out parameter, set by this function. Do not modify after calling this function.
*/
void SEGGER_SYSVIEW_RegisterModule(SEGGER_SYSVIEW_MODULE* pModule) {
  SEGGER_SYSVIEW_LOCK();
  if (_pFirstModule == 0) {
    //
    // No module registered, yet.
    // Start list with new module.
    // EventOffset is the base offset for modules
    //
    pModule->EventOffset = MODULE_EVENT_OFFSET;
    pModule->pNext = 0;
    _pFirstModule = pModule;
    _NumModules = 1;
  } else {
    //
    // Registreded module(s) present.
    // Prepend new module in list.
    // EventOffset set from number of events and offset of previous module.
    //
    pModule->EventOffset = _pFirstModule->EventOffset + _pFirstModule->NumEvents;
    pModule->pNext = _pFirstModule;
    _pFirstModule = pModule;
    _NumModules++;
  }
  SEGGER_SYSVIEW_SendModule(0);
  if (pModule->pfSendModuleDesc) {
    pModule->pfSendModuleDesc();
  }
  SEGGER_SYSVIEW_UNLOCK();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_RecordModuleDescription()
*
*  Function description
*    Sends detailed information of a registered module to the host.
*
*  Parameters
*    pModule      - Pointer to the described module.
*    sDescription - Pointer to a description string.
*/
void SEGGER_SYSVIEW_RecordModuleDescription(const SEGGER_SYSVIEW_MODULE* pModule, const char* sDescription) {
  U8  ModuleId;
  SEGGER_SYSVIEW_MODULE* p;

  p = _pFirstModule;
  ModuleId = 0;
  do {
    if (p == pModule) {
      break;
    }
    ModuleId++;
    p = p->pNext;
  } while (p);
  {
    U8* pPayload;
    U8* pPayloadStart;
    RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32 + 1 + SEGGER_SYSVIEW_MAX_STRING_LEN);
    //
    pPayload = pPayloadStart;
    //
    // Send module description
    // Send event offset and number of events
    //
    ENCODE_U32(pPayload, ModuleId);
    ENCODE_U32(pPayload, (pModule->EventOffset));
    pPayload = _EncodeStr(pPayload, sDescription, SEGGER_SYSVIEW_MAX_STRING_LEN);
    _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_MODULEDESC);
    RECORD_END();
  }
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_SendModule()
*
*  Function description
*    Sends the information of a registered module to the host.
*
*  Parameters
*    ModuleId   - Id of the requested module.
*/
void SEGGER_SYSVIEW_SendModule(U8 ModuleId) {
  SEGGER_SYSVIEW_MODULE* pModule;
  U32 n;

  if (_pFirstModule != 0) {
    pModule = _pFirstModule;
    for (n = 0; n < ModuleId; n++) {
      pModule = pModule->pNext;
      if (pModule == 0) {
        break;
      }
    }
    if (pModule != 0) {
      U8* pPayload;
      U8* pPayloadStart;
      RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32 + 1 + SEGGER_SYSVIEW_MAX_STRING_LEN);
      //
      pPayload = pPayloadStart;
      //
      // Send module description
      // Send event offset and number of events
      //
      ENCODE_U32(pPayload, ModuleId);
      ENCODE_U32(pPayload, (pModule->EventOffset));
      pPayload = _EncodeStr(pPayload, pModule->sModule, SEGGER_SYSVIEW_MAX_STRING_LEN);
      _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_MODULEDESC);
      RECORD_END();
    }
  }
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_SendModuleDescription()
*
*  Function description
*    Triggers a send of the registered module descriptions.
*
*/
void SEGGER_SYSVIEW_SendModuleDescription(void) {
  SEGGER_SYSVIEW_MODULE* pModule;

  if (_pFirstModule != 0) {
    pModule = _pFirstModule;
    do {
      if (pModule->pfSendModuleDesc) {
        pModule->pfSendModuleDesc();
      }
      pModule = pModule->pNext;
    } while (pModule);
  }
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_SendNumModules()
*
*  Function description
*    Send the number of registered modules to the host.
*/
void SEGGER_SYSVIEW_SendNumModules(void) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2*SEGGER_SYSVIEW_QUANTA_U32);
  pPayload = pPayloadStart;
  ENCODE_U32(pPayload, _NumModules);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_NUMMODULES);
  RECORD_END();
}

#ifndef SEGGER_SYSVIEW_EXCLUDE_PRINTF // Define in project to avoid warnings about variable parameter list

/*********************************************************************
*
*       SEGGER_SYSVIEW_PrintfHostEx()
*
*  Function description
*    Print a string which is formatted on the host by the SystemView Application
*    with Additional information.
*
*  Parameters
*    s        - String to be formatted.
*    Options  - Options for the string. i.e. Log level.
*
*  Additional information
*    All format arguments are treated as 32-bit scalar values.
*/
void SEGGER_SYSVIEW_PrintfHostEx(const char* s, U32 Options, ...) {
  va_list ParamList;
#if SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT
  int r;

  va_start(ParamList, Options);
  r = _VPrintHost(s, Options, &ParamList);
  va_end(ParamList);

  if (r == -1) {
    va_start(ParamList, Options);
    _VPrintTarget(s, Options, &ParamList);
    va_end(ParamList);
  }
#else
  va_start(ParamList, Options);
  _VPrintHost(s, Options, &ParamList);
  va_end(ParamList);
#endif
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_PrintfHost()
*
*  Function description
*    Print a string which is formatted on the host by the SystemView Application.
*
*  Parameters
*    s        - String to be formatted.
*
*  Additional information
*    All format arguments are treated as 32-bit scalar values.
*/
void SEGGER_SYSVIEW_PrintfHost(const char* s, ...) {
  va_list ParamList;
#if SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT
  int r;

  va_start(ParamList, s);
  r = _VPrintHost(s, SEGGER_SYSVIEW_LOG, &ParamList);
  va_end(ParamList);

  if (r == -1) {
    va_start(ParamList, s);
    _VPrintTarget(s, SEGGER_SYSVIEW_LOG, &ParamList);
    va_end(ParamList);
  }
#else
  va_start(ParamList, s);
  _VPrintHost(s, SEGGER_SYSVIEW_LOG, &ParamList);
  va_end(ParamList);
#endif
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_WarnfHost()
*
*  Function description
*    Print a warning string which is formatted on the host by
*    the SystemView Application.
*
*  Parameters
*    s        - String to be formatted.
*
*  Additional information
*    All format arguments are treated as 32-bit scalar values.
*/
void SEGGER_SYSVIEW_WarnfHost(const char* s, ...) {
  va_list ParamList;
#if SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT
  int r;

  va_start(ParamList, s);
  r = _VPrintHost(s, SEGGER_SYSVIEW_WARNING, &ParamList);
  va_end(ParamList);

  if (r == -1) {
    va_start(ParamList, s);
    _VPrintTarget(s, SEGGER_SYSVIEW_WARNING, &ParamList);
    va_end(ParamList);
  }
#else
  va_start(ParamList, s);
  _VPrintHost(s, SEGGER_SYSVIEW_WARNING, &ParamList);
  va_end(ParamList);
#endif
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_ErrorfHost()
*
*  Function description
*    Print an error string which is formatted on the host by
*    the SystemView Application.
*
*  Parameters
*    s        - String to be formatted.
*
*  Additional information
*    All format arguments are treated as 32-bit scalar values.
*/
void SEGGER_SYSVIEW_ErrorfHost(const char* s, ...) {
  va_list ParamList;
#if SEGGER_SYSVIEW_PRINTF_IMPLICIT_FORMAT
  int r;

  va_start(ParamList, s);
  r = _VPrintHost(s, SEGGER_SYSVIEW_ERROR, &ParamList);
  va_end(ParamList);

  if (r == -1) {
    va_start(ParamList, s);
    _VPrintTarget(s, SEGGER_SYSVIEW_ERROR, &ParamList);
    va_end(ParamList);
  }
#else
  va_start(ParamList, s);
  _VPrintHost(s, SEGGER_SYSVIEW_ERROR, &ParamList);
  va_end(ParamList);
#endif
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_PrintfTargetEx()
*
*  Function description
*    Print a string which is formatted on the target before sent to
*    the host with Additional information.
*
*  Parameters
*    s        - String to be formatted.
*    Options  - Options for the string. i.e. Log level.
*/
void SEGGER_SYSVIEW_PrintfTargetEx(const char* s, U32 Options, ...) {
  va_list ParamList;

  va_start(ParamList, Options);
  _VPrintTarget(s, Options, &ParamList);
  va_end(ParamList);
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_PrintfTarget()
*
*  Function description
*    Print a string which is formatted on the target before sent to
*    the host.
*
*  Parameters
*    s        - String to be formatted.
*/
void SEGGER_SYSVIEW_PrintfTarget(const char* s, ...) {
  va_list ParamList;

  va_start(ParamList, s);
  _VPrintTarget(s, SEGGER_SYSVIEW_LOG, &ParamList);
  va_end(ParamList);
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_WarnfTarget()
*
*  Function description
*    Print a warning string which is formatted on the target before
*    sent to the host.
*
*  Parameters
*    s        - String to be formatted.
*/
void SEGGER_SYSVIEW_WarnfTarget(const char* s, ...) {
  va_list ParamList;

  va_start(ParamList, s);
  _VPrintTarget(s, SEGGER_SYSVIEW_WARNING, &ParamList);
  va_end(ParamList);
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_ErrorfTarget()
*
*  Function description
*    Print an error string which is formatted on the target before
*    sent to the host.
*
*  Parameters
*    s        - String to be formatted.
*/
void SEGGER_SYSVIEW_ErrorfTarget(const char* s, ...) {
  va_list ParamList;

  va_start(ParamList, s);
  _VPrintTarget(s, SEGGER_SYSVIEW_ERROR, &ParamList);
  va_end(ParamList);
}
#endif // SEGGER_SYSVIEW_EXCLUDE_PRINTF

/*********************************************************************
*
*       SEGGER_SYSVIEW_Print()
*
*  Function description
*    Print a string to the host.
*
*  Parameters
*    s        - String to sent.
*/
void SEGGER_SYSVIEW_Print(const char* s) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32 + SEGGER_SYSVIEW_MAX_STRING_LEN);
  //
  pPayload = _EncodeStr(pPayloadStart, s, SEGGER_SYSVIEW_MAX_STRING_LEN);
  ENCODE_U32(pPayload, SEGGER_SYSVIEW_LOG);
  ENCODE_U32(pPayload, 0);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_PRINT_FORMATTED);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_Warn()
*
*  Function description
*    Print a warning string to the host.
*
*  Parameters
*    s        - String to sent.
*/
void SEGGER_SYSVIEW_Warn(const char* s) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32 + SEGGER_SYSVIEW_MAX_STRING_LEN);
  //
  pPayload = _EncodeStr(pPayloadStart, s, SEGGER_SYSVIEW_MAX_STRING_LEN);
  ENCODE_U32(pPayload, SEGGER_SYSVIEW_WARNING);
  ENCODE_U32(pPayload, 0);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_PRINT_FORMATTED);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_Error()
*
*  Function description
*    Print an error string to the host.
*
*  Parameters
*    s        - String to sent.
*/
void SEGGER_SYSVIEW_Error(const char* s) {
  U8* pPayload;
  U8* pPayloadStart;
  RECORD_START(SEGGER_SYSVIEW_INFO_SIZE + 2 * SEGGER_SYSVIEW_QUANTA_U32 + SEGGER_SYSVIEW_MAX_STRING_LEN);
  //
  pPayload = _EncodeStr(pPayloadStart, s, SEGGER_SYSVIEW_MAX_STRING_LEN);
  ENCODE_U32(pPayload, SEGGER_SYSVIEW_ERROR);
  ENCODE_U32(pPayload, 0);
  _SendPacket(pPayloadStart, pPayload, SYSVIEW_EVTID_PRINT_FORMATTED);
  RECORD_END();
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_EnableEvents()
*
*  Function description
*    Enable standard SystemView events to be generated.
*
*  Parameters
*    EnableMask   - Events to be enabled.
*/
void SEGGER_SYSVIEW_EnableEvents(U32 EnableMask) {
  _SYSVIEW_Globals.DisabledEvents &= ~EnableMask;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_DisableEvents()
*
*  Function description
*    Disable standard SystemView events to not be generated.
*
*  Parameters
*    DisableMask  - Events to be disabled.
*/
void SEGGER_SYSVIEW_DisableEvents(U32 DisableMask) {
  _SYSVIEW_Globals.DisabledEvents |= DisableMask;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_IsStarted()
*
*  Function description
*    Handle incoming packets if any and check if recording is started.
*
*  Return value
*      0: Recording not started.
*    > 0: Recording started.
*/
int SEGGER_SYSVIEW_IsStarted(void) {
#if (SEGGER_SYSVIEW_POST_MORTEM_MODE != 1)
  //
  // Check if host is sending data which needs to be processed.
  //
  if (SEGGER_RTT_HASDATA(CHANNEL_ID_DOWN)) {
    if (_SYSVIEW_Globals.RecursionCnt == 0) {   // Avoid uncontrolled nesting. This way, this routine can call itself once, but no more often than that.
      _SYSVIEW_Globals.RecursionCnt = 1;
      _HandleIncomingPacket();
      _SYSVIEW_Globals.RecursionCnt = 0;
    }
  }
#endif
  return _SYSVIEW_Globals.EnableState;
}


/*************************** End of file ****************************/
