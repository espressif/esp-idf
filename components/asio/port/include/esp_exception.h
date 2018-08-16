
// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _ESP_EXCEPTION_H_
#define _ESP_EXCEPTION_H_

//
// This exception stub is enabled only if exceptions are disabled in menuconfig
//
#if !defined(CONFIG_CXX_EXCEPTIONS) && defined (ASIO_NO_EXCEPTIONS)

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
#endif // CONFIG_CXX_EXCEPTIONS==1 && defined (ASIO_NO_EXCEPTIONS)

#endif // _ESP_EXCEPTION_H_
