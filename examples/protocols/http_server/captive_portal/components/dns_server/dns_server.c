/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <sys/param.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_system.h"
#include "esp_check.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "dns_server.h"

#define DNS_PORT (53)
#define DNS_MAX_LEN (256)

#define OPCODE_MASK (0x7800)
#define QR_FLAG (1 << 7)
#define QD_TYPE_A (0x0001)
#define ANS_TTL_SEC (300)
#define RECV_TIMEOUT_SEC (1)

static const char *TAG = "example_dns_redirect_server";

// DNS Header Packet
typedef struct __attribute__((__packed__))
{
    uint16_t id;
    uint16_t flags;
    uint16_t qd_count;
    uint16_t an_count;
    uint16_t ns_count;
    uint16_t ar_count;
} dns_header_t;

// DNS Question Packet
typedef struct __attribute__((__packed__))
{
    uint16_t type;
    uint16_t class;
} dns_question_t;

// DNS Answer Packet
typedef struct __attribute__((__packed__))
{
    uint16_t ptr_offset;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t addr_len;
    uint32_t ip_addr;
} dns_answer_t;

// DNS server handle
struct dns_server_handle {
    volatile bool started;
    SemaphoreHandle_t exited;
    int num_of_entries;
    dns_entry_pair_t entry[];
};

/*
    Parse the name from the packet from the DNS name format to a regular .-seperated name
    returns the pointer to the next part of the packet
*/
static char *parse_dns_name(char *raw_name, const char *raw_end, char *parsed_name, size_t parsed_name_max_len)
{

    char *label = raw_name;
    char *name_itr = parsed_name;
    size_t name_len = 0;

    do {
        if (label >= raw_end) {
            return NULL;
        }
        uint8_t sub_name_len = *label;
        // (len + 1) since we are adding  a '.'
        name_len += (sub_name_len + 1);
        // The label and the length byte that follows it must lie within the packet
        if (name_len > parsed_name_max_len || sub_name_len >= raw_end - label - 1) {
            return NULL;
        }

        // Copy the sub name that follows the the label
        memcpy(name_itr, label + 1, sub_name_len);
        name_itr[sub_name_len] = '.';
        name_itr += (sub_name_len + 1);
        label += sub_name_len + 1;
    } while (*label != 0);

    // Terminate the final string, replacing the last '.'
    parsed_name[name_len - 1] = '\0';
    // Return pointer to first char after the name
    return label + 1;
}

// Parses the DNS request and prepares a DNS response with the IP of the softAP
static int parse_dns_request(char *req, size_t req_len, char *dns_reply, size_t dns_reply_max_len, dns_server_handle_t h)
{
    if (req_len < sizeof(dns_header_t) || req_len > dns_reply_max_len) {
        return -1;
    }

    // Prepare the reply
    memset(dns_reply, 0, dns_reply_max_len);
    memcpy(dns_reply, req, req_len);

    // Endianness of NW packet different from chip
    dns_header_t *header = (dns_header_t *)dns_reply;
    ESP_LOGD(TAG, "DNS query with header id: 0x%X, flags: 0x%X, qd_count: %d",
             ntohs(header->id), ntohs(header->flags), ntohs(header->qd_count));

    // Not a query, so that a spoofed reply can't make two servers answer each other forever
    if (header->flags & QR_FLAG) {
        return 0;
    }

    // Not a standard query
    if ((header->flags & OPCODE_MASK) != 0) {
        return 0;
    }

    // Set question response flag
    header->flags |= QR_FLAG;

    uint16_t qd_count = ntohs(header->qd_count);

    // Worst case: every question gets an answer
    if (qd_count * sizeof(dns_answer_t) + req_len > dns_reply_max_len) {
        return -1;
    }

    // Pointer to current answer and question
    const char *req_end = dns_reply + req_len;
    char *cur_ans_ptr = dns_reply + req_len;
    char *cur_qd_ptr = dns_reply + sizeof(dns_header_t);
    uint16_t an_count = 0;
    char name[128];

    // Respond to all questions based on configured rules
    for (int qd_i = 0; qd_i < qd_count; qd_i++) {
        char *name_end_ptr = parse_dns_name(cur_qd_ptr, req_end, name, sizeof(name));
        if (name_end_ptr == NULL || name_end_ptr + sizeof(dns_question_t) > req_end) {
            ESP_LOGE(TAG, "Failed to parse DNS question %d", qd_i);
            return -1;
        }

        dns_question_t *question = (dns_question_t *)(name_end_ptr);
        uint16_t qd_type = ntohs(question->type);
        uint16_t qd_class = ntohs(question->class);
        char *qd_ptr = cur_qd_ptr;
        cur_qd_ptr = name_end_ptr + sizeof(dns_question_t);

        ESP_LOGD(TAG, "Received type: %d | Class: %d | Question for: %s", qd_type, qd_class, name);

        if (qd_type == QD_TYPE_A) {
            esp_ip4_addr_t ip = { .addr = IPADDR_ANY };
            // Check the configured rules to decide whether to answer this question or not
            for (int i = 0; i < h->num_of_entries; ++i) {
                // check if the name either corresponds to the entry, or if we should answer to all queries ("*")
                if (strcmp(h->entry[i].name, "*") == 0 || strcmp(h->entry[i].name, name) == 0) {
                    if (h->entry[i].if_key) {
                        esp_netif_ip_info_t ip_info;
                        esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey(h->entry[i].if_key), &ip_info);
                        ip.addr = ip_info.ip.addr;
                        break;
                    } else if (h->entry[i].ip.addr != IPADDR_ANY) {
                        ip.addr = h->entry[i].ip.addr;
                        break;
                    }
                }
            }
            if (ip.addr == IPADDR_ANY) {    // no rule applies, continue with another question
                continue;
            }
            dns_answer_t *answer = (dns_answer_t *)cur_ans_ptr;

            answer->ptr_offset = htons(0xC000 | (qd_ptr - dns_reply));
            answer->type = htons(qd_type);
            answer->class = htons(qd_class);
            answer->ttl = htonl(ANS_TTL_SEC);

            ESP_LOGD(TAG, "Answer with PTR offset: 0x%" PRIX16 " and IP 0x%" PRIX32, ntohs(answer->ptr_offset), ip.addr);

            answer->addr_len = htons(sizeof(ip.addr));
            answer->ip_addr = ip.addr;

            cur_ans_ptr += sizeof(dns_answer_t);
            an_count++;
        }
    }
    header->an_count = htons(an_count);
    return cur_ans_ptr - dns_reply;
}

/*
    Sets up a socket and listen for DNS queries,
    replies to all type A queries with the IP of the softAP
*/
void dns_server_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family;
    int ip_protocol;
    dns_server_handle_t handle = pvParameters;

    while (handle->started) {

        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(DNS_PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            close(sock);
            break;
        }
        ESP_LOGI(TAG, "Socket bound, port %d", DNS_PORT);

        // Wake up periodically so that stop_dns_server() is noticed
        struct timeval timeout = { .tv_sec = RECV_TIMEOUT_SEC, .tv_usec = 0 };
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
            ESP_LOGE(TAG, "Unable to set socket receive timeout: errno %d", errno);
            close(sock);
            break;
        }

        while (handle->started) {
            ESP_LOGD(TAG, "Waiting for data");
            struct sockaddr_in6 source_addr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(source_addr);
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer), 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    continue;
                }
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                // Get the sender's ip address as string
                if (source_addr.sin6_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
                } else if (source_addr.sin6_family == PF_INET6) {
                    inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
                }

                char reply[DNS_MAX_LEN];
                int reply_len = parse_dns_request(rx_buffer, len, reply, DNS_MAX_LEN, handle);

                ESP_LOGI(TAG, "Received %d bytes from %s | DNS reply with len: %d", len, addr_str, reply_len);
                if (reply_len <= 0) {
                    ESP_LOGE(TAG, "Failed to prepare a DNS reply");
                } else {
                    int err = sendto(sock, reply, reply_len, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                    if (err < 0) {
                        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                        break;
                    }
                }
            }
        }

        ESP_LOGI(TAG, "Shutting down socket");
        shutdown(sock, 0);
        close(sock);
    }
    // stop_dns_server() may free the handle as soon as this is given
    xSemaphoreGive(handle->exited);
    vTaskDelete(NULL);
}

dns_server_handle_t start_dns_server(dns_server_config_t *config)
{
    dns_server_handle_t handle = calloc(1, sizeof(struct dns_server_handle) + config->num_of_entries * sizeof(dns_entry_pair_t));
    ESP_RETURN_ON_FALSE(handle, NULL, TAG, "Failed to allocate dns server handle");

    handle->exited = xSemaphoreCreateBinary();
    if (handle->exited == NULL) {
        ESP_LOGE(TAG, "Failed to create dns server semaphore");
        goto err;
    }
    handle->started = true;
    handle->num_of_entries = config->num_of_entries;
    memcpy(handle->entry, config->item, config->num_of_entries * sizeof(dns_entry_pair_t));

    if (xTaskCreate(dns_server_task, "dns_server", 4096, handle, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create dns server task");
        goto err;
    }
    return handle;

err:
    if (handle->exited) {
        vSemaphoreDelete(handle->exited);
    }
    free(handle);
    return NULL;
}

void stop_dns_server(dns_server_handle_t handle)
{
    if (handle) {
        // Let the task leave recvfrom() and close its socket instead of deleting it from here
        handle->started = false;
        xSemaphoreTake(handle->exited, portMAX_DELAY);
        vSemaphoreDelete(handle->exited);
        free(handle);
    }
}
