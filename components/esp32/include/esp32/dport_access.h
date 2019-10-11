// Copyright 2010-2017 Espressif Systems (Shanghai) PTE LTD
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

#include <sdkconfig.h>

#ifndef _ESP_DPORT_ACCESS_H_
#define _ESP_DPORT_ACCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

void esp_dport_access_stall_other_cpu_start(void);
void esp_dport_access_stall_other_cpu_end(void);
void esp_dport_access_int_init(void);
void esp_dport_access_int_pause(void);
void esp_dport_access_int_resume(void);
void esp_dport_access_read_buffer(uint32_t *buff_out, uint32_t address, uint32_t num_words);
uint32_t esp_dport_access_reg_read(uint32_t reg);
uint32_t esp_dport_access_sequence_reg_read(uint32_t reg);
//This routine does not stop the dport routines in any way that is recoverable. Please
//only call in case of panic().
void esp_dport_access_int_abort(void);

#if defined(BOOTLOADER_BUILD) || !defined(CONFIG_ESP32_DPORT_WORKAROUND) || !defined(ESP_PLATFORM)
#define DPORT_STALL_OTHER_CPU_START()
#define DPORT_STALL_OTHER_CPU_END()
#define DPORT_INTERRUPT_DISABLE()
#define DPORT_INTERRUPT_RESTORE()
#else
#define DPORT_STALL_OTHER_CPU_START()   esp_dport_access_stall_other_cpu_start()
#define DPORT_STALL_OTHER_CPU_END()     esp_dport_access_stall_other_cpu_end()
#define DPORT_INTERRUPT_DISABLE()       unsigned int intLvl = XTOS_SET_INTLEVEL(CONFIG_ESP32_DPORT_DIS_INTERRUPT_LVL)
#define DPORT_INTERRUPT_RESTORE()       XTOS_RESTORE_JUST_INTLEVEL(intLvl)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ESP_DPORT_ACCESS_H_ */
