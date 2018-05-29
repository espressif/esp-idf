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
#ifndef _ESP_PLATFORM_SYS_POLL_H_
#define _ESP_PLATFORM_SYS_POLL_H_

#define	POLLIN		0x0001		/* any readable data available */
#define	POLLOUT		0x0004		/* file descriptor is writeable */
#define	POLLPRI		0x0002		/* OOB/Urgent readable data */
#define	POLLERR		0x0008		/* some poll error occurred */
#define	POLLHUP		0x0010		/* file descriptor was "hung up" */

struct pollfd {
   int fd;        /* The descriptor. */
   short events;  /* The event(s) is/are specified here. */
   short revents; /* Events found are returned here. */
};

typedef unsigned int nfds_t;
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

#endif // _ESP_PLATFORM_SYS_POLL_H_
