/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_openthread.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/toolchain.h>
#include <stdint.h>
#include <utils/uart.h>

OT_TOOL_WEAK void otPlatUartReceived(const uint8_t *, uint16_t)
{
}

OT_TOOL_WEAK void otPlatUartSendDone(void)
{
}

OT_TOOL_WEAK void otPlatDiagRadioTransmitDone(otInstance *, otRadioFrame *, otError)
{
}

OT_TOOL_WEAK void otPlatDiagRadioReceiveDone(otInstance *, otRadioFrame *, otError)
{
}

OT_TOOL_WEAK void otPlatDiagAlarmFired(otInstance *)
{
}
