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

#ifndef EH_FRAME_PARSER_H
#define EH_FRAME_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print backtrace for the given execution frame.
 *
 * @param frame_or Snapshot of the CPU registers when the program stopped its
 *                 normal execution. This frame is usually generated on the
 *                 stack when an exception or an interrupt occurs.
 */
void esp_eh_frame_print_backtrace(const void *frame_or);

#ifdef __cplusplus
}
#endif

#endif
