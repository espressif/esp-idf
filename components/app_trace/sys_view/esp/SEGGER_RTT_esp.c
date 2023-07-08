/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "string.h"
#include "freertos/FreeRTOS.h"
#include "SEGGER_RTT.h"
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_SYSVIEW_Conf.h"

#include "esp_app_trace.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_private/startup_internal.h"

const static char *TAG = "segger_rtt";

#define SYSVIEW_EVENTS_BUF_SZ         255U

// size of down channel data buf
#define SYSVIEW_DOWN_BUF_SIZE   32
#define SEGGER_STOP_WAIT_TMO    1000000 //us
#if CONFIG_APPTRACE_SV_BUF_WAIT_TMO == -1
#define SEGGER_HOST_WAIT_TMO    ESP_APPTRACE_TMO_INFINITE
#else
#define SEGGER_HOST_WAIT_TMO    CONFIG_APPTRACE_SV_BUF_WAIT_TMO
#endif

static uint8_t s_events_buf[SYSVIEW_EVENTS_BUF_SZ];
static uint16_t s_events_buf_filled;
static uint8_t s_down_buf[SYSVIEW_DOWN_BUF_SIZE];

#if CONFIG_APPTRACE_SV_DEST_UART

#define ESP_APPTRACE_DEST_SYSVIEW ESP_APPTRACE_DEST_UART
#if CONFIG_APPTRACE_SV_DEST_CPU_0 || CONFIG_FREERTOS_UNICORE
#define APPTRACE_SV_DEST_CPU 0
#else
#define APPTRACE_SV_DEST_CPU 1
#endif // CONFIG_APPTRACE_SV_DEST_CPU_0

#elif CONFIG_APPTRACE_SV_DEST_JTAG || (CONFIG_APPTRACE_ENABLE && CONFIG_APPTRACE_DEST_UART_NONE)
#define ESP_APPTRACE_DEST_SYSVIEW ESP_APPTRACE_DEST_TRAX
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       SEGGER_RTT_ESP_FlushNoLock()
*
*  Function description
*    Flushes buffered events.
*
*  Parameters
*    min_sz  Threshold for flushing data. If current filling level is above this value, data will be flushed. TRAX destinations only.
*    tmo     Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
*
*  Return value
*    None.
*/
void SEGGER_RTT_ESP_FlushNoLock(unsigned long min_sz, unsigned long tmo)
{
    esp_err_t res;
    if (s_events_buf_filled > 0) {
      res = esp_apptrace_write(ESP_APPTRACE_DEST_SYSVIEW, s_events_buf, s_events_buf_filled, tmo);
      if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to flush buffered events (%d)!", res);
      }
    }
    // flush even if we failed to write buffered events, because no new events will be sent after STOP
    res = esp_apptrace_flush_nolock(ESP_APPTRACE_DEST_SYSVIEW, min_sz, tmo);
    if (res != ESP_OK) {
      ESP_LOGE(TAG, "Failed to flush apptrace data (%d)!", res);
    }
    s_events_buf_filled = 0;
}

/*********************************************************************
*
*       SEGGER_RTT_ESP_Flush()
*
*  Function description
*    Flushes buffered events.
*
*  Parameters
*    min_sz  Threshold for flushing data. If current filling level is above this value, data will be flushed. TRAX destinations only.
*    tmo     Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinetly.
*
*  Return value
*    None.
*/
void SEGGER_RTT_ESP_Flush(unsigned long min_sz, unsigned long tmo)
{
    SEGGER_SYSVIEW_LOCK();
    SEGGER_RTT_ESP_FlushNoLock(min_sz, tmo);
    SEGGER_SYSVIEW_UNLOCK();
}

/*********************************************************************
*
*       SEGGER_RTT_ReadNoLock()
*
*  Function description
*    Reads characters from SEGGER real-time-terminal control block
*    which have been previously stored by the host.
*    Do not lock against interrupts and multiple access.
*
*  Parameters
*    BufferIndex  Index of Down-buffer to be used (e.g. 0 for "Terminal").
*    pBuffer      Pointer to buffer provided by target application, to copy characters from RTT-down-buffer to.
*    BufferSize   Size of the target application buffer.
*
*  Return value
*    Number of bytes that have been read.
*/
unsigned SEGGER_RTT_ReadNoLock(unsigned BufferIndex, void* pData, unsigned BufferSize) {
  uint32_t size = BufferSize;
  esp_err_t res = esp_apptrace_read(ESP_APPTRACE_DEST_SYSVIEW, pData, &size, 0);
  if (res != ESP_OK) {
    return 0;
  }
  return size;
}

/*********************************************************************
*
*       SEGGER_RTT_WriteSkipNoLock
*
*  Function description
*    Stores a specified number of characters in SEGGER RTT
*    control block which is then read by the host.
*    SEGGER_RTT_WriteSkipNoLock does not lock the application and
*    skips all data, if the data does not fit into the buffer.
*
*  Parameters
*    BufferIndex  Index of "Up"-buffer to be used (e.g. 0 for "Terminal").
*    pBuffer      Pointer to character array. Does not need to point to a \0 terminated string.
*    NumBytes     Number of bytes to be stored in the SEGGER RTT control block.
*
*  Return value
*    Number of bytes which have been stored in the "Up"-buffer.
*
*  Notes
*    (1) If there is not enough space in the "Up"-buffer, all data is dropped.
*    (2) For performance reasons this function does not call Init()
*        and may only be called after RTT has been initialized.
*        Either by calling SEGGER_RTT_Init() or calling another RTT API function first.
*/
unsigned SEGGER_RTT_WriteSkipNoLock(unsigned BufferIndex, const void* pBuffer, unsigned NumBytes) {
  uint8_t *pbuf = (uint8_t *)pBuffer;
  uint8_t event_id = *pbuf;
#if CONFIG_APPTRACE_SV_DEST_UART
  if (
    (APPTRACE_SV_DEST_CPU != esp_cpu_get_core_id()) &&
    (
      (event_id == SYSVIEW_EVTID_ISR_ENTER) ||
      (event_id == SYSVIEW_EVTID_ISR_EXIT) ||
      (event_id == SYSVIEW_EVTID_TASK_START_EXEC) ||
      (event_id == SYSVIEW_EVTID_TASK_STOP_EXEC) ||
      (event_id == SYSVIEW_EVTID_TASK_START_READY) ||
      (event_id == SYSVIEW_EVTID_TASK_STOP_READY) ||
      (event_id == SYSVIEW_EVTID_MARK_START) ||
      (event_id == SYSVIEW_EVTID_MARK_STOP) ||
      (event_id == SYSVIEW_EVTID_TIMER_ENTER) ||
      (event_id == SYSVIEW_EVTID_TIMER_EXIT) ||
      (event_id == SYSVIEW_EVTID_STACK_INFO) ||
      (event_id == SYSVIEW_EVTID_MODULEDESC)
    )
  ){
    return NumBytes;
  }

// This is workaround for SystemView!
// Without this line SystemView will hangs on when heap tracing enabled.
  if(event_id == SYSVIEW_EVTID_MODULEDESC){
    return NumBytes;
  }
#endif // CONFIG_APPTRACE_SV_DEST_UART

  if (NumBytes > SYSVIEW_EVENTS_BUF_SZ) {
      ESP_LOGE(TAG, "Too large event %u bytes!", NumBytes);
      return 0;
  }
#if CONFIG_APPTRACE_SV_DEST_JTAG
  if (esp_cpu_get_core_id()) { // dual core specific code
    // use the highest - 1 bit of event ID to indicate core ID
    // the highest bit can not be used due to event ID encoding method
    // this reduces supported ID range to [0..63] (for 1 byte IDs) plus [128..16383] (for 2 bytes IDs)
    if (*pbuf & 0x80) { // 2 bytes ID
      *(pbuf + 1) |= (1 << 6);
    } else if (NumBytes != 10 || *pbuf != 0) { // ignore sync sequence
      *pbuf |= (1 << 6);
    }
  }
#endif // CONFIG_APPTRACE_SV_DEST_JTAG
#if CONFIG_APPTRACE_SV_DEST_JTAG
  if (s_events_buf_filled + NumBytes > SYSVIEW_EVENTS_BUF_SZ) {

    esp_err_t res = esp_apptrace_write(ESP_APPTRACE_DEST_SYSVIEW, s_events_buf, s_events_buf_filled, SEGGER_HOST_WAIT_TMO);
    if (res != ESP_OK) {
      return 0; // skip current data buffer only, accumulated events are kept
    }
    s_events_buf_filled = 0;
  }
#endif
  memcpy(&s_events_buf[s_events_buf_filled], pBuffer, NumBytes);
  s_events_buf_filled += NumBytes;

#if CONFIG_APPTRACE_SV_DEST_UART
  esp_err_t res = esp_apptrace_write(ESP_APPTRACE_DEST_SYSVIEW, pBuffer, NumBytes, SEGGER_HOST_WAIT_TMO);
  if (res != ESP_OK)
  {
    return 0; // skip current data buffer only, accumulated events are kept
  }
  s_events_buf_filled = 0;
#endif

  if (event_id == SYSVIEW_EVTID_TRACE_STOP)
  {
    SEGGER_RTT_ESP_FlushNoLock(0, SEGGER_STOP_WAIT_TMO);
  }
  return NumBytes;
}

/*********************************************************************
*
*       SEGGER_RTT_ConfigUpBuffer
*
*  Function description
*    Run-time configuration of a specific up-buffer (T->H).
*    Buffer to be configured is specified by index.
*    This includes: Buffer address, size, name, flags, ...
*
*  Parameters
*    BufferIndex  Index of the buffer to configure.
*    sName        Pointer to a constant name string.
*    pBuffer      Pointer to a buffer to be used.
*    BufferSize   Size of the buffer.
*    Flags        Operating modes. Define behavior if buffer is full (not enough space for entire message).
*
*  Return value
*    >= 0 - O.K.
*     < 0 - Error
*
*  Additional information
*    Buffer 0 is configured on compile-time.
*    May only be called once per buffer.
*    Buffer name and flags can be reconfigured using the appropriate functions.
*/
int SEGGER_RTT_ConfigUpBuffer(unsigned BufferIndex, const char* sName, void* pBuffer, unsigned BufferSize, unsigned Flags) {
  s_events_buf_filled = 0;
  return 0;
}

/*********************************************************************
*
*       SEGGER_RTT_ConfigDownBuffer
*
*  Function description
*    Run-time configuration of a specific down-buffer (H->T).
*    Buffer to be configured is specified by index.
*    This includes: Buffer address, size, name, flags, ...
*
*  Parameters
*    BufferIndex  Index of the buffer to configure.
*    sName        Pointer to a constant name string.
*    pBuffer      Pointer to a buffer to be used.
*    BufferSize   Size of the buffer.
*    Flags        Operating modes. Define behavior if buffer is full (not enough space for entire message).
*
*  Return value
*    >= 0  O.K.
*     < 0  Error
*
*  Additional information
*    Buffer 0 is configured on compile-time.
*    May only be called once per buffer.
*    Buffer name and flags can be reconfigured using the appropriate functions.
*/
int SEGGER_RTT_ConfigDownBuffer(unsigned BufferIndex, const char* sName, void* pBuffer, unsigned BufferSize, unsigned Flags) {
  esp_apptrace_down_buffer_config(s_down_buf, sizeof(s_down_buf));
  return 0;
}

/*************************** Init hook ****************************
 *
 * This init function is placed here because this port file will be
 * linked whenever SystemView is used.
 */

ESP_SYSTEM_INIT_FN(sysview_init, BIT(0), 120)
{
    SEGGER_SYSVIEW_Conf();
    return ESP_OK;
}


/*************************** End of file ****************************/
