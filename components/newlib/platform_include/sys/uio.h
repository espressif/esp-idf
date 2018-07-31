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
#ifndef _ESP_PLATFORM_SYS_UIO_H_
#define _ESP_PLATFORM_SYS_UIO_H_

int writev(int s, const struct iovec *iov, int iovcnt);

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);

#endif // _ESP_PLATFORM_SYS_UIO_H_
