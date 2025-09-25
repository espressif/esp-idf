/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "string.h"
#include "freertos/FreeRTOS.h"
#include "SEGGER_RTT.h"
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_SYSVIEW_Conf.h"

#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_trace_port_transport.h"
#include "esp_trace_port_encoder.h"
#include "esp_trace_types.h"
#include "esp_private/startup_internal.h"
#include "adapters/adapter_encoder_sysview.h"

const static char *TAG = "segger_rtt";

#define SYSVIEW_EVENTS_BUF_SZ         255U

// size of down channel data buf
#define SYSVIEW_DOWN_BUF_SIZE   32
#if CONFIG_SEGGER_SYSVIEW_BUF_WAIT_TMO == -1
#define SEGGER_HOST_WAIT_TMO    ESP_TRACE_TMO_INFINITE
#else
#define SEGGER_HOST_WAIT_TMO    CONFIG_SEGGER_SYSVIEW_BUF_WAIT_TMO
#endif

static uint8_t s_events_buf[SYSVIEW_EVENTS_BUF_SZ];
static uint16_t s_events_buf_filled;
static uint8_t s_down_buf[SYSVIEW_DOWN_BUF_SIZE];

/**
 * @brief Encoder reference for SEGGER RTT layer
 *
 * This maintains proper architectural layering:
 * - SEGGER RTT is part of the SystemView encoder implementation
 * - It accesses transport through the encoder's transport reference (s_encoder->tp)
 * - NOT through global singleton (that would be a layer violation)
 *
 * Set by SEGGER_RTT_ESP_SetEncoder() during encoder init.
 */
static esp_trace_encoder_t *s_encoder = NULL;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       SEGGER_RTT_ESP_SetEncoder()
*
*  Function description
*    Inject encoder handle from esp_trace adapter.
*    This allows SEGGER RTT to access transport through the encoder's
*    transport reference, maintaining proper architectural layering.
*
*  Parameters
*    encoder  Pointer to encoder instance from esp_trace
*
*  Return value
*    0 if successful, -1 if encoder is not initialized or missing required functions in transport.
*/
int SEGGER_RTT_ESP_SetEncoder(esp_trace_encoder_t *encoder)
{
    /* Check if adapters have all required functions */
    if (!encoder || !encoder->ctx ||
            !encoder->vt->give_lock ||
            !encoder->vt->take_lock ||
            !encoder->tp->vt->down_buffer_config ||
            !encoder->tp->vt->write ||
            !encoder->tp->vt->flush_nolock ||
            !encoder->tp->vt->read ||
            !encoder->tp->vt->get_link_type) {
        ESP_LOGE(TAG, "Encoder not initialized or missing required functions in transport");
        return -1;
    }

    s_encoder = encoder;

    return 0;
}

/*********************************************************************
*
*       SEGGER_SYSVIEW_GetEncoder()
*
*  Function description
*    Returns the encoder handle for accessing transport functions.
*    This is used by SEGGER_SYSVIEW_Config_FreeRTOS.c to access
*    transport lock functions.
*
*  Parameters
*    None
*
*  Return value
*    Pointer to encoder instance, or NULL if not initialized.
*/
esp_trace_encoder_t* SEGGER_SYSVIEW_GetEncoder(void)
{
    return s_encoder;
}

/*********************************************************************
*
*       SEGGER_RTT_ESP_GetDestCpu()
*
*  Function description
*    Gets the destination CPU from the encoder context.
*
*  Parameters
*    None
*
*  Return value
*    CPU ID (0 or 1) to trace
*/
static int SEGGER_RTT_ESP_GetDestCpu(void)
{
    sysview_encoder_ctx_t *ctx = s_encoder->ctx;
    return ctx->dest_cpu;
}

/*********************************************************************
*
*       SEGGER_RTT_ESP_FlushNoLock()
*
*  Function description
*    Flushes buffered events.
*
*  Parameters
*    min_sz  Threshold for flushing data. If current filling level is above this value, data will be flushed. JTAG destinations only.
*    tmo     Timeout for operation (in us). Use ESP_APPTRACE_TMO_INFINITE to wait indefinitely.
*
*  Return value
*    None.
*/
void SEGGER_RTT_ESP_FlushNoLock(void)
{
    esp_err_t res;

    if (!s_encoder) {
        ESP_LOGE(TAG, "Encoder not initialized");
        return;
    }

    esp_trace_transport_t *tp = s_encoder->tp;

    if (s_events_buf_filled > 0) {
        res = tp->vt->write(tp, s_events_buf, s_events_buf_filled, SEGGER_HOST_WAIT_TMO);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write buffered events (%d)!", res);
        }
    }
    // flush even if we failed to write buffered events, because no new events will be sent after STOP
    res = tp->vt->flush_nolock(tp);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to flush buffered events (%d)!", res);
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
*
*  Return value
*    None.
*/
void SEGGER_RTT_ESP_Flush(void)
{
    SEGGER_SYSVIEW_LOCK();
    SEGGER_RTT_ESP_FlushNoLock();
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
unsigned SEGGER_RTT_ReadNoLock(unsigned BufferIndex, void* pData, unsigned BufferSize)
{
    if (!s_encoder) {
        return 0;
    }
    size_t size = BufferSize;
    esp_err_t res = s_encoder->tp->vt->read(s_encoder->tp, pData, &size, 0);
    return res != ESP_OK ? 0 : size;
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
unsigned SEGGER_RTT_WriteSkipNoLock(unsigned BufferIndex, const void* pBuffer, unsigned NumBytes)
{
    if (!s_encoder) {
        return 0;  // Encoder is not initialized
    }

    esp_trace_transport_t *tp = s_encoder->tp;
    uint8_t *pbuf = (uint8_t *)pBuffer;
    uint8_t event_id = *pbuf;

    if (tp->vt->get_link_type(tp) == ESP_TRACE_LINK_UART) {
        int dest_cpu = SEGGER_RTT_ESP_GetDestCpu();
        if (
            (dest_cpu != esp_cpu_get_core_id()) &&
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
        ) {
            return NumBytes;
        }

        // This is workaround for SystemView!
        // Without this line SystemView will hangs on when heap tracing enabled.
        if (event_id == SYSVIEW_EVTID_MODULEDESC) {
            return NumBytes;
        }
    }

    if (NumBytes > SYSVIEW_EVENTS_BUF_SZ) {
        ESP_LOGE(TAG, "Too large event %u bytes!", NumBytes);
        return 0;
    }
    if (tp->vt->get_link_type(tp) == ESP_TRACE_LINK_DEBUG_PROBE) {
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

        if (s_events_buf_filled + NumBytes > SYSVIEW_EVENTS_BUF_SZ) {

            esp_err_t res = tp->vt->write(tp, s_events_buf, s_events_buf_filled, SEGGER_HOST_WAIT_TMO);
            if (res != ESP_OK) {
                return 0; // skip current data buffer only, accumulated events are kept
            }
            s_events_buf_filled = 0;
        }
    }
    memcpy(&s_events_buf[s_events_buf_filled], pBuffer, NumBytes);
    s_events_buf_filled += NumBytes;

    if (tp->vt->get_link_type(tp) == ESP_TRACE_LINK_UART) {
        esp_err_t res = tp->vt->write(tp, pBuffer, NumBytes, SEGGER_HOST_WAIT_TMO);
        if (res != ESP_OK) {
            return 0; // skip current data buffer only, accumulated events are kept
        }
        s_events_buf_filled = 0;
    }

    if (event_id == SYSVIEW_EVTID_TRACE_STOP) {
        SEGGER_RTT_ESP_FlushNoLock();
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
int SEGGER_RTT_ConfigUpBuffer(unsigned BufferIndex, const char* sName, void* pBuffer, unsigned BufferSize, unsigned Flags)
{
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
int SEGGER_RTT_ConfigDownBuffer(unsigned BufferIndex, const char* sName, void* pBuffer, unsigned BufferSize, unsigned Flags)
{
    if (!s_encoder) {
        return -1;
    }
    return s_encoder->tp->vt->down_buffer_config(s_encoder->tp, s_down_buf, sizeof(s_down_buf));
}
