/*
 * HttpSSEServer.h
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

#ifndef MAIN_HTTPD_SSE_SERVER_HPP_
#define MAIN_HTTPD_SSE_SERVER_HPP_

#include <esp_http_server.h>
#include "events.hpp"

httpd_handle_t start_webserver(EventQueueHandle_t outEvtQueue);
void stop_webserver(httpd_handle_t server);
void HardwareUpdated(C_HWStatus *);

#endif /* MAIN_HTTPD_SSE_SERVER_HPP_ */
