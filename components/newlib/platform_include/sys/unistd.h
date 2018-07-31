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


#ifndef _ESP_SYS_UNISTD_H
#define _ESP_SYS_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include_next <sys/unistd.h>

int     _EXFUN(truncate, (const char *, off_t __length));
int     _EXFUN(gethostname, (char *__name, size_t __len));

#ifdef __cplusplus
}
#endif
#endif /* _SYS_UNISTD_H */
