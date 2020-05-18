// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SIO_LWIP_H_
#define _SIO_LWIP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set SIO port baud rate
 *
 * @param[in] fd       SIO file descriptor.
 * @param[in] baudrate The baud rate.
 */
void sio_set_baudrate(sio_fd_t fd, uint32_t baudrate);

/**
 * @brief   Set the SIO port pins
 *
 * @note    For @p cts and @p rts you can pass GPIO_PIN_NO_CHANGE value to leave
 * them as is if you don't want HW flow control.
 *
 * @param[in] fd  SIO file descriptor.
 * @param[in] tx  The UART TX pin.
 * @param[in] rx  The UART RX pin.
 * @param[in] rts The UART RTS pin.
 * @param[in] cts The UART CTS pin.
 */
void sio_set_pin(sio_fd_t fd, int tx, int rx, int rts, int cts);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _SIO_LWIP_H_ */
