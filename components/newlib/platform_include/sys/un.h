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
#ifndef _ESP_PLATFORM_SYS_UN_H_
#define _ESP_PLATFORM_SYS_UN_H_

#define	AF_UNIX		1		/* local to host (pipes) */

struct sockaddr_un {
            short                    sun_family;                /*AF_UNIX*/
            char                     sun_path[108];             /*path name */
};

#endif // _ESP_PLATFORM_SYS_UN_H_
