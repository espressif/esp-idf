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

#ifndef _ESP_DPORT_ACCESS_H_
#define _ESP_DPORT_ACCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

void esp_dport_access_stall_other_cpu_start(void);
void esp_dport_access_stall_other_cpu_end(void);
void esp_dport_access_int_init(void);
void esp_dport_access_int_deinit(void);

#if defined(BOOTLOADER_BUILD) || defined(CONFIG_FREERTOS_UNICORE) || !defined(ESP_PLATFORM)
#define DPORT_STALL_OTHER_CPU_START()
#define DPORT_STALL_OTHER_CPU_END()
#else
#define DPORT_STALL_OTHER_CPU_START()   esp_dport_access_stall_other_cpu_start()
#define DPORT_STALL_OTHER_CPU_END()     esp_dport_access_stall_other_cpu_end()
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ESP_DPORT_ACCESS_H_ */
