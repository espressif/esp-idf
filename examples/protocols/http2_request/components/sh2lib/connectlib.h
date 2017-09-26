// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
#ifndef __ESP_EXAMPLE_CONNECT_LIB_H_
#define __ESP_EXAMPLE_CONNECT_LIB_H_

struct uri {
    const char *host;
    /* In this program, path contains query component as well. */
    const char *path;
    size_t pathlen;
    const char *hostport;
    size_t hostlen;
    size_t hostportlen;
    uint16_t port;
};

/* connect() to a TCP host, return socket descriptor */
int connect_to_host(const char *host, size_t hostlen, uint16_t port);

/* Parse a URI into its components */
int parse_uri(struct uri *res, const char *uri);

#endif /* ! __ESP_EXAMPLE_CONNECT_LIB_H_ */
