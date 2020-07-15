//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <string>
#include "protocol_examples_common.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include "asio/buffer.hpp"
#include "esp_pthread.h"

extern const unsigned char server_pem_start[] asm("_binary_srv_crt_start");
extern const unsigned char server_pem_end[]   asm("_binary_srv_crt_end");

extern const unsigned char cacert_pem_start[] asm("_binary_ca_crt_start");
extern const unsigned char cacert_pem_end[]   asm("_binary_ca_crt_end");

extern const unsigned char prvtkey_pem_start[] asm("_binary_server_key_start");
extern const unsigned char prvtkey_pem_end[]   asm("_binary_server_key_end");

static const asio::const_buffer cert_chain(cacert_pem_start, cacert_pem_end - cacert_pem_start);
static const asio::const_buffer privkey(prvtkey_pem_start, prvtkey_pem_end - prvtkey_pem_start);
static const asio::const_buffer server_cert(server_pem_start, server_pem_end - server_pem_start);

using asio::ip::tcp;

static const std::size_t max_length = 1024;

class Client {
public:
    Client(asio::io_context &io_context,
           asio::ssl::context &context,
           const tcp::resolver::results_type &endpoints)
        : socket_(io_context, context)
    {

#if CONFIG_EXAMPLE_CLIENT_VERIFY_PEER
        socket_.set_verify_mode(asio::ssl::verify_peer);
#else
        socket_.set_verify_mode(asio::ssl::verify_none);
#endif // CONFIG_EXAMPLE_CLIENT_VERIFY_PEER

        connect(endpoints);
    }

private:
    void connect(const tcp::resolver::results_type &endpoints)
    {
        asio::async_connect(socket_.lowest_layer(), endpoints,
                            [this](const std::error_code & error,
        const tcp::endpoint & /*endpoint*/) {
            if (!error) {
                handshake();
            } else {
                std::cout << "Connect failed: " << error.message() << "\n";
            }
        });
    }

    void handshake()
    {
        socket_.async_handshake(asio::ssl::stream_base::client,
        [this](const std::error_code & error) {
            if (!error) {
                send_request();
            } else {
                std::cout << "Handshake failed: " << error.message() << "\n";
            }
        });
    }

    void send_request()
    {
        size_t request_length = std::strlen(request_);

        asio::async_write(socket_,
                          asio::buffer(request_, request_length),
        [this](const std::error_code & error, std::size_t length) {
            if (!error) {
                receive_response(length);
            } else {
                std::cout << "Write failed: " << error.message() << "\n";
            }
        });
    }

    void receive_response(std::size_t length)
    {
        asio::async_read(socket_,
                         asio::buffer(reply_, length),
        [this](const std::error_code & error, std::size_t length) {
            if (!error) {
                std::cout << "Reply: ";
                std::cout.write(reply_, length);
                std::cout << "\n";
            } else {
                std::cout << "Read failed: " << error.message() << "\n";
            }
        });
    }

    asio::ssl::stream<tcp::socket> socket_;
    char request_[max_length] = "GET / HTTP/1.1\r\n\r\n";
    char reply_[max_length];
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, asio::ssl::context &context)
        : socket_(std::move(socket), context)
    {
    }

    void start()
    {
        do_handshake();
    }

private:
    void do_handshake()
    {
        auto self(shared_from_this());
        socket_.async_handshake(asio::ssl::stream_base::server,
        [this, self](const std::error_code & error) {
            if (!error) {
                do_read();
            }
        });
    }

    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_),
        [this, self](const std::error_code & ec, std::size_t length) {
            if (!ec) {
                std::cout << "Server received: ";
                std::cout.write(data_, length);
                std::cout << std::endl;
                do_write(length);
            }
        });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(data_, length),
                          [this, self](const std::error_code & ec,
        std::size_t /*length*/) {
            if (!ec) {
                do_read();
            }
        });
    }

    asio::ssl::stream<tcp::socket> socket_;
    char data_[max_length];
};

class Server {
public:
    Server(asio::io_context &io_context, unsigned short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
          context_(asio::ssl::context::tls_server)
    {
        context_.set_options(
            asio::ssl::context::default_workarounds
            | asio::ssl::context::no_sslv2);
        context_.use_certificate_chain(server_cert);
        context_.use_private_key(privkey, asio::ssl::context::pem);

        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
        [this](const std::error_code & error, tcp::socket socket) {
            if (!error) {
                std::make_shared<Session>(std::move(socket), context_)->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    asio::ssl::context context_;
};

void set_thread_config(const char *name, int stack, int prio)
{
    auto cfg = esp_pthread_get_default_config();
    cfg.thread_name = name;
    cfg.stack_size = stack;
    cfg.prio = prio;
    esp_pthread_set_cfg(&cfg);
}

void ssl_server_thread()
{
    asio::io_context io_context;

    Server s(io_context, 443);

    io_context.run();
}

void ssl_client_thread()
{
    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    std::string server_ip = CONFIG_EXAMPLE_SERVER_NAME;
    std::string server_port = CONFIG_EXAMPLE_PORT;
    auto endpoints = resolver.resolve(server_ip, server_port);

    asio::ssl::context ctx(asio::ssl::context::tls_client);
#if CONFIG_EXAMPLE_CLIENT_VERIFY_PEER
    ctx.add_certificate_authority(cert_chain);
#endif // CONFIG_EXAMPLE_CLIENT_VERIFY_PEER

    Client c(io_context, ctx, endpoints);

    io_context.run();

}


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
    std::vector<std::thread> work_threads;

#if CONFIG_EXAMPLE_SERVER
    set_thread_config("Server", 16 * 1024, 5);
    work_threads.emplace_back(std::thread(ssl_server_thread));
    std::this_thread::sleep_for(std::chrono::seconds(1));
#endif // CONFIG_EXAMPLE_SERVER

#if CONFIG_EXAMPLE_CLIENT
    set_thread_config("Client", 16 * 1024, 5);
    work_threads.emplace_back(ssl_client_thread);
#endif // CONFIG_EXAMPLE_CLIENT

    for (auto &t : work_threads) {
        t.join();
    }

}
