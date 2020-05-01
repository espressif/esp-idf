/*
 * HttpSSEServer.cpp
 *
 * Simple HTTP Server Example demonstrating Server Sent Event for
 *  server->client communication and HTTP PUT on the other direction.
 *
 * This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   V 1.0 2020.04.28 Nicolas Korber (nicolas.korber_at_bluewin.ch)
 *
 *   */

#include <esp_wifi.h>
#include <vector>
#include <sstream>
#include <regex>
#include <algorithm>
#include <esp_log.h>
#include <esp_http_server.h>
#include "httpd_sse_server.hpp"

// ?? missing in original header
extern "C" int httpd_default_send(httpd_handle_t hd, int sockfd, const char *buf, unsigned buf_len, int flags);

static const char *TAG = "http_server";
static const int max_open_sockets = 7;

// The active sessions
static std::vector<int> ActiveHWAwareSessionsFd = {};

static httpd_handle_t s_HttpServer = NULL;

// The queue that controls the output events
static EventQueueHandle_t s_OutEvtQueue = NULL;

// Handler to redirect incoming GET request for /index.html to /
static esp_err_t index_html_get_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "307 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);  // Response body can be empty
    return ESP_OK;
}

static const httpd_uri_t get_index = {
    .uri       = "/index.html",
    .method    = HTTP_GET,
    .handler   = index_html_get_handler,
    .user_ctx  = NULL
};

// Handler to respond with an icon file embedded in flash.
// Browsers expect to GET website icon at URI /favicon.ico.
static esp_err_t favicon_get_handler(httpd_req_t *req)
{
    extern const unsigned char favicon_ico_start[] asm("_binary_favicon_ico_start");
    extern const unsigned char favicon_ico_end[]   asm("_binary_favicon_ico_end");
    const size_t favicon_ico_size = (favicon_ico_end - favicon_ico_start);
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_size);
    return ESP_OK;
}

const httpd_uri_t get_favicon = {
    .uri       = "/favicon.ico",
    .method    = HTTP_GET,
    .handler   = favicon_get_handler,
    .user_ctx  = NULL
};

esp_err_t led_blue_handler(httpd_req_t *req)
{
    extern const unsigned char LedBlue_start[] asm("_binary_iconfinder_Circle_Blue_34210_png_start");
    extern const unsigned char LedBlue_end[]   asm("_binary_iconfinder_Circle_Blue_34210_png_end");
    const size_t LedBlue_size = (LedBlue_end - LedBlue_start);
    httpd_resp_set_type(req, "image/png");
    httpd_resp_send(req, reinterpret_cast<const char *>(LedBlue_start), LedBlue_size);
    return ESP_OK;
}

const httpd_uri_t get_ledblue = {
    .uri       = "/LedBlue.png",
    .method    = HTTP_GET,
    .handler   = led_blue_handler,
    .user_ctx  = NULL
};

esp_err_t led_grey_handler(httpd_req_t *req)
{
    extern const unsigned char LedGrey_start[] asm("_binary_iconfinder_Circle_Grey_34212_png_start");
    extern const unsigned char LedGrey_end[]   asm("_binary_iconfinder_Circle_Grey_34212_png_end");
    const size_t LedGrey_size = (LedGrey_end - LedGrey_start);
    httpd_resp_set_type(req, "image/png");
    httpd_resp_send(req, reinterpret_cast<const char *>(LedGrey_start), LedGrey_size);
    return ESP_OK;
}

const httpd_uri_t get_ledgrey = {
    .uri       = "/LedGrey.png",
    .method    = HTTP_GET,
    .handler   = led_grey_handler,
    .user_ctx  = NULL
};

static std::string  header(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    std::ostringstream ss;
    ss << "<!DOCTYPE html><html><head>"
       << "<title>Server Sent Event Test</title>"
       << "</head><body>" << std::endl;
    return ss.str();
}

static std::string  footer()
{
    std::ostringstream ss;
    ss << "<footer>icons by <a href='http://www.icons-land.com/'>IconsLand</a></footer>"
       << "</body></html>" << std::endl;
    return ss.str();
}

// HTTP GET root handler
static esp_err_t root_get_handler(httpd_req_t *req)
{
    std::ostringstream ss;
    ss << header(req)
       << "<script type='text/javascript'>" << std::endl
       <<    "var source = new EventSource('demo_sse');" << std::endl
       <<    "source.onmessage = function(event) {" << std::endl
       <<        "var hw = JSON.parse(event.data);" << std::endl
       <<        "var icon = (1 == hw.led? 'LedBlue.png' : 'LedGrey.png');" << std::endl
       <<        "document.getElementById('led').src = icon; };" << std::endl
       <<    "source.onopen = function() {"  << std::endl
       <<        "document.getElementById('connection').innerHTML = 'opened'; };" << std::endl
       <<    "source.onerror = function() {"  << std::endl
       <<        "document.getElementById('connection').innerHTML = 'failed'; };" << std::endl
       <<  "</script>" << std::endl
       << "<table>"
       << "<tr><td>Led :</td><td>"
       << "<input type=image id='led' src='/LedGrey.png' alt='Status Led' name = 'btnled'>"
       << "</td></tr>"
       << "<tr><td>Connection :</td><td><div id='connection'>?"
       << "</div></td></tr></table>"
       << "<script type='text/javascript'>" << std::endl
       <<    "document.getElementById('led').onclick = function(){" << std::endl
       <<       " var xmlHttp = new XMLHttpRequest();" << std::endl
       <<       " xmlHttp.open('PUT','/ctrl',true);" << std::endl
       <<       " xmlHttp.setRequestHeader('Content-Type', 'text/plain');"   << std::endl
       <<       " xmlHttp.send('btnLed');}"  << std::endl
       <<  "</script>" << std::endl
       << footer();
    std::string s = ss.str();
    const char *resp_str = s.c_str();
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}
static const httpd_uri_t root_get = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = NULL
};

// Informs the client of the new hardware status.
// the connection is already established
static void HardwareStatusSend(C_HWStatus *HWStatus)
{
    char buf[250];
    for (int fd : ActiveHWAwareSessionsFd) {
        ESP_LOGD(TAG, "Executing queued work fd : %d", fd);
        snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\r\n" \
                 "Content-Type: text/event-stream\r\n" \
                 "Cache-Control: no-cache\n") ;
        // Warning do NOT specify a content length or the connection will be closed !
        httpd_default_send(s_HttpServer, fd, buf, strlen(buf), 0);

        snprintf(buf, sizeof(buf), "data:  {\"led\":%s}\r\n\r\n", (HWStatus->getLedStatus() ? "1" : "0")) ;
        httpd_default_send(s_HttpServer, fd, "\r\n", strlen("\r\n"), 0);
        httpd_default_send(s_HttpServer, fd, buf, strlen(buf), 0);
    }
    free(HWStatus);
}

// Callback for the hardware manager to queue an update
void HardwareUpdated(C_HWStatus *HWStatus)
{
    if (!ActiveHWAwareSessionsFd.empty()) {
        ESP_LOGD(TAG, "Queuing work");
        httpd_queue_work(s_HttpServer,
                         reinterpret_cast<httpd_work_fn_t>(HardwareStatusSend),
                         HWStatus);
    }
}

// Callback for the http server. called while releasing the connection
static void FreeHardwareStatusSendContext(void *ctx)
{
    int *ctx_fd = reinterpret_cast<int *>(ctx);
    auto it = std::remove(ActiveHWAwareSessionsFd.begin(), ActiveHWAwareSessionsFd.end(), *ctx_fd);
    ActiveHWAwareSessionsFd.erase(it, ActiveHWAwareSessionsFd.end());
    // C++20 std::erase(ActiveHWAwareSessionsFd, *ctx_fd);
}

/* HTTP GET demo_sse handler */
static esp_err_t demo_sse_get_handler(httpd_req_t *req)
{
    // Keep track of the session and send the first status
    int *fd = reinterpret_cast<int *>(malloc(sizeof(int)));
    *fd = httpd_req_to_sockfd(req);
    if (*fd < 0) {
        return ESP_FAIL;
    }
    // Store the context for future use
    ActiveHWAwareSessionsFd.push_back(*fd);
    httpd_sess_set_ctx(s_HttpServer, *fd, fd, &FreeHardwareStatusSendContext);

    // Ask for the first update
    C_Event Event = C_Event(refresh_hw_status);
    xQueueSendToBack(s_OutEvtQueue, &Event, ( TickType_t ) 0 );
    return ESP_OK;
}

static const httpd_uri_t get_demo_sse = {
    .uri       = "/demo_sse",
    .method    = HTTP_GET,
    .handler   = demo_sse_get_handler,
    .user_ctx  = NULL
};

//  ctrl PUT handler.
static esp_err_t ctrl_put_handler(httpd_req_t *req)
{
    const size_t buf_len = 50;
    char buf[buf_len];
    size_t remaining = req->content_len;

    while (remaining > 0) {
        size_t rec = 0;
        /* Read the data for the request */
        if ((rec = httpd_req_recv(req, buf, buf_len)) <= 0) {
            if (rec == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= rec;
        /* Log data received */
        ESP_LOGD(TAG, "Received %.*s", rec, buf);
        std::smatch match;
        std::string answer(buf, rec);
        std::regex reg("btnLed");
        if (std::regex_search (answer, match, reg)) {
            // Send this event
            C_Event Event = C_Event(button_active);
            xQueueSendToBack(s_OutEvtQueue, &Event, ( TickType_t ) 0 );
        }
    }
    // Respond with empty body
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t ctrl = {
    .uri       = "/ctrl",
    .method    = HTTP_PUT,
    .handler   = ctrl_put_handler,
    .user_ctx  = NULL
};

httpd_handle_t start_webserver(EventQueueHandle_t outEvtQueue)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // from esp_http_server/include/esp_http_server.h
    config.stack_size = 5120;
    config.lru_purge_enable = false;
    config.max_open_sockets = max_open_sockets;
    s_OutEvtQueue = outEvtQueue;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server 192.168.4.1 on port: '%d'", config.server_port);
    if (httpd_start(&s_HttpServer, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGD(TAG, "Registering URI handlers"); // default : max 8
        httpd_register_uri_handler(s_HttpServer, &root_get);
        httpd_register_uri_handler(s_HttpServer, &get_index);
        httpd_register_uri_handler(s_HttpServer, &get_favicon);
        httpd_register_uri_handler(s_HttpServer, &get_ledblue);
        httpd_register_uri_handler(s_HttpServer, &get_ledgrey);
        httpd_register_uri_handler(s_HttpServer, &get_demo_sse);
        httpd_register_uri_handler(s_HttpServer, &ctrl);
        return s_HttpServer;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
    ActiveHWAwareSessionsFd.clear();
}
