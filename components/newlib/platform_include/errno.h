
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
#ifndef _ESP_PLATFORM_ERRNO_H_
#define _ESP_PLATFORM_ERRNO_H_

#include_next "errno.h"

//
// Possibly define some missing errors
//
#ifndef ESHUTDOWN
#define ESHUTDOWN      108  /* Cannot send after transport endpoint shutdown */
#endif

#ifndef EAI_SOCKTYPE
#define	EAI_SOCKTYPE	10	/* ai_socktype not supported */
#endif

#ifndef EAI_AGAIN
#define	EAI_AGAIN	     2   /* temporary failure in name resolution */
#endif

#ifndef EAI_BADFLAGS
#define	EAI_BADFLAGS	 3	 /* invalid value for ai_flags */
#endif

#endif // _ESP_PLATFORM_ERRNO_H_
