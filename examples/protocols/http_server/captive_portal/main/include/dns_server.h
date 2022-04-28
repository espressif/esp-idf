/* Captive Portal Example

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set ups and starts a simple DNS server that will respond to all queries
 * with the soft AP's IP address
 *
 */
void start_dns_server(void);


#ifdef __cplusplus
}
#endif
