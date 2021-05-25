//
// async_udp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>

#include "asio.hpp"

#include "protocol_examples_common.h"
#include "esp_event.h"
#include "nvs_flash.h"


using asio::ip::udp;

class server
{
public:
  server(asio::io_context& io_context, short port)
    : socket_(io_context, udp::endpoint(udp::v4(), port))
  {
    do_receive();
  }

  void do_receive()
  {
    socket_.async_receive_from(
        asio::buffer(data_, max_length), sender_endpoint_,
        [this](std::error_code ec, std::size_t bytes_recvd)
        {
          if (!ec && bytes_recvd > 0)
          {
            data_[bytes_recvd] = 0;
            std::cout << data_ << std::endl;
            do_send(bytes_recvd);
          }
          else
          {
            do_receive();
          }
        });
  }

  void do_send(std::size_t length)
  {
    socket_.async_send_to(
        asio::buffer(data_, length), sender_endpoint_,
        [this](std::error_code /*ec*/, std::size_t  bytes /*bytes_sent*/)
        {
          do_receive();
        });
  }

private:
  udp::socket socket_;
  udp::endpoint sender_endpoint_;
  enum { max_length = 1024 };
  char data_[max_length];
};

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* This helper function configures blocking UART I/O */
    ESP_ERROR_CHECK(example_configure_stdin_stdout());

    asio::io_context io_context;

    server s(io_context, std::atoi(CONFIG_EXAMPLE_PORT));

    std::cout << "ASIO engine is up and running" << std::endl;

    io_context.run();
}
