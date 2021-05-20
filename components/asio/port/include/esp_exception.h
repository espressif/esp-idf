
/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ESP_EXCEPTION_H_
#define _ESP_EXCEPTION_H_

//
// This exception stub is enabled only if exceptions are disabled in menuconfig
//
#if !defined(CONFIG_COMPILER_CXX_EXCEPTIONS) && defined (ASIO_NO_EXCEPTIONS)

#include "esp_log.h"

//
// asio exception stub
//
namespace asio {
namespace detail {
template <typename Exception>
void throw_exception(const Exception& e)
{
  ESP_LOGE("esp32_asio_exception", "Caught exception: %s!", e.what());
  abort();
}
}}
#endif // CONFIG_COMPILER_CXX_EXCEPTIONS==1 && defined (ASIO_NO_EXCEPTIONS)

#endif // _ESP_EXCEPTION_H_
