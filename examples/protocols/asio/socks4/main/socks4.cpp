/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 *
 *  ASIO Socks4 example
*/

#include <string>
#include <array>
#include <asio.hpp>
#include <memory>
#include <system_error>
#include <utility>
#include "esp_log.h"
#include "socks4.hpp"
#include "nvs_flash.h"
#include "esp_event.h"
#include "protocol_examples_common.h"

constexpr auto TAG = "asio_socks4";
using asio::ip::tcp;

namespace {

void esp_init()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_log_level_set("async_request", ESP_LOG_DEBUG);

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
}

/**
 * @brief Simple class to add the resolver to a chain of actions
 *
 */
class AddressResolution : public std::enable_shared_from_this<AddressResolution> {
public:
    explicit AddressResolution(asio::io_context &context) : ctx(context), resolver(ctx) {}

    /**
     * @brief Initiator function for the address resolution
     *
     * @tparam CompletionToken callable responsible to use the results.
     *
     * @param host Host address
     * @param port Port for the target, must be number due to a limitation on lwip.
     */
    template<class CompletionToken>
    void resolve(const std::string &host, const std::string &port, CompletionToken &&completion_handler)
    {
        auto self(shared_from_this());
        resolver.async_resolve(host, port, [self, completion_handler](const asio::error_code & error, tcp::resolver::results_type results) {
            if (error) {
                ESP_LOGE(TAG, "Failed to resolve: %s", error.message().c_str());
                return;
            }
            completion_handler(self, results);
        });
    }

private:
    asio::io_context &ctx;
    tcp::resolver resolver;

};

/**
 * @brief Connection class
 *
 * The lowest level dependency on our asynchronous task, Connection provide an interface to TCP sockets.
 * A similar class could be provided for a TLS connection.
 *
 * @note: All read and write operations are written on an explicit strand, even though an implicit strand
 * occurs in this example since we run the io context in a single task.
 *
 */
class Connection : public std::enable_shared_from_this<Connection> {
public:
    explicit Connection(asio::io_context &context) : ctx(context), strand(context), socket(ctx) {}

    /**
     * @brief Start the connection
     *
     * Async operation to start a connection. As the final act of the process the Connection class pass a
     * std::shared_ptr of itself to the completion_handler.
     * Since it uses std::shared_ptr as an automatic control of its lifetime this class must be created
     * through a std::make_shared call.
     *
     * @tparam completion_handler A callable to act as the final handler for the process.
     * @param host host address
     * @param port port number - due to a limitation on lwip implementation this should be the number not the
     *                           service name typically seen in ASIO examples.
     *
     * @note The class could be modified to store the completion handler, as a member variable, instead of
     * pass it along asynchronous calls to allow the process to run again completely.
     *
     */
    template<class CompletionToken>
    void start(tcp::resolver::results_type results, CompletionToken &&completion_handler)
    {
        connect(results, completion_handler);
    }

    /**
     * @brief Start an async write on the socket
     *
     * @tparam data
     * @tparam completion_handler A callable to act as the final handler for the process.
     *
     */
    template<class DataType, class CompletionToken>
    void write_async(const DataType &data, CompletionToken &&completion_handler)
    {
        asio::async_write(socket, data, asio::bind_executor(strand, completion_handler));
    }

    /**
     * @brief Start an async read on the socket
     *
     * @tparam data
     * @tparam completion_handler A callable to act as the final handler for the process.
     *
     */
    template<class DataBuffer, class CompletionToken>
    void read_async(DataBuffer &&in_data, CompletionToken &&completion_handler)
    {
        asio::async_read(socket, in_data, asio::bind_executor(strand, completion_handler));
    }

private:

    template<class CompletionToken>
    void connect(tcp::resolver::results_type results, CompletionToken &&completion_handler)
    {
        auto self(shared_from_this());
        asio::async_connect(socket, results, [self, completion_handler](const asio::error_code & error, [[maybe_unused]] const tcp::endpoint & endpoint) {
            if (error) {
                ESP_LOGE(TAG, "Failed to connect: %s", error.message().c_str());
                return;
            }
            completion_handler(self);
        });
    }
    asio::io_context &ctx;
    asio::io_context::strand strand;
    tcp::socket socket;
};

}

namespace Socks {

struct ConnectionData {
    ConnectionData(socks4::request::command_type cmd, const asio::ip::tcp::endpoint &endpoint,
                   const std::string &user_id) : request(cmd, endpoint, user_id) {};
    socks4::request request;
    socks4::reply reply;
};

template<class CompletionToken>
void async_connect(asio::io_context &context, std::string proxy, std::string proxy_port, std::string host, std::string port, CompletionToken &&completion_handler)
{
    /*
     * The first step is to resolve the address of the proxy we want to connect to.
     * The AddressResolution itself is injected to the completion handler.
     */
    // Resolve proxy
    std::make_shared<AddressResolution>(context)->resolve(proxy, proxy_port,
    [&context, host, port, completion_handler](std::shared_ptr<AddressResolution> resolver, tcp::resolver::results_type proxy_resolution) {
        // We also need to resolve the target host address
        resolver->resolve(host, port, [&context, proxy_resolution, completion_handler](std::shared_ptr<AddressResolution> resolver, tcp::resolver::results_type host_resolution) {
            // Make connection with the proxy
            ESP_LOGI(TAG, "Startig Proxy Connection");
            std::make_shared<Connection>(context)->start(proxy_resolution,
            [resolver, host_resolution, completion_handler](std::shared_ptr<Connection> connection) {
                auto connect_data = std::make_shared<ConnectionData>(socks4::request::connect, *host_resolution, "");
            ESP_LOGI(TAG, "Sending Request to proxy for host connection.");
                connection->write_async(connect_data->request.buffers(), [connection, connect_data, completion_handler](std::error_code error, std::size_t bytes_received) {
                    if (error) {
                        ESP_LOGE(TAG, "Proxy request write error: %s", error.message().c_str());
                        return;
                    }
                    connection->read_async(connect_data->reply.buffers(), [connection, connect_data, completion_handler](std::error_code error, std::size_t bytes_received) {
                        if (error) {

                            ESP_LOGE(TAG, "Proxy response read error: %s", error.message().c_str());
                            return;
                        }
                        if (!connect_data->reply.success()) {
                            ESP_LOGE(TAG, "Proxy error: %#x", connect_data->reply.status());
                        }
                        completion_handler(connection);

                    });

                });

            });

        });
    });

}
} // namespace Socks

namespace Http {
enum class Method { GET };

/**
 * @brief Simple HTTP request class
 *
 * The user needs to write the request information direct to header and body fields.
 *
 * Only GET verb is provided.
 *
 */
class Request {
public:
    Request(Method method, std::string host, std::string port, const std::string &target) : host_data(std::move(host)), port_data(std::move(port))
    {
        header_data.append("GET ");
        header_data.append(target);
        header_data.append(" HTTP/1.1");
        header_data.append("\r\n");
        header_data.append("Host: ");
        header_data.append(host_data);
        header_data.append("\r\n");
        header_data.append("\r\n");
    };

    void set_header_field(std::string const &field)
    {
        header_data.append(field);
    }

    void append_to_body(std::string const &data)
    {
        body_data.append(data);
    };

    const std::string &host() const
    {
        return host_data;
    }

    const std::string &service_port() const
    {
        return port_data;
    }

    const std::string &header() const
    {
        return header_data;
    }

    const std::string &body() const
    {
        return body_data;
    }

private:
    std::string host_data;
    std::string port_data;
    std::string header_data;
    std::string body_data;
};

/**
 * @brief Simple HTTP response class
 *
 * The response is built from received data and only parsed to split header and body.
 *
 * A copy of the received data is kept.
 *
 */
struct Response {
    /**
     * @brief Construct a response from a contiguous buffer.
     *
     * Simple http parsing.
     *
     */
    template<class DataIt>
    explicit Response(DataIt data, size_t size)
    {
        raw_response = std::string(data, size);

        auto header_last = raw_response.find("\r\n\r\n");
        if (header_last != std::string::npos) {
            header = raw_response.substr(0, header_last);
        }
        body = raw_response.substr(header_last + 3);
    }
    /**
     * @brief Print response content.
     */
    void print()
    {
        ESP_LOGI(TAG, "Header :\n %s", header.c_str());
        ESP_LOGI(TAG, "Body : \n %s", body.c_str());
    }

    std::string raw_response;
    std::string header;
    std::string body;
};

/** @brief HTTP Session
 *
 * Session class to handle HTTP protocol implementation.
 *
 */
class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(std::shared_ptr<Connection> connection_in) : connection(std::move(connection_in))
    {
    }

    template<class CompletionToken>
    void send_request(const Request &request, CompletionToken &&completion_handler)
    {
        auto self = shared_from_this();
        send_data = { asio::buffer(request.header()), asio::buffer(request.body()) };
        connection->write_async(send_data, [self, &completion_handler](std::error_code error, std::size_t bytes_transfered) {
            if (error) {
                ESP_LOGE(TAG, "Request write error: %s", error.message().c_str());
                return;
            }
            ESP_LOGD(TAG, "Bytes Transfered: %d", bytes_transfered);
            self->get_response(completion_handler);
        });
    }

private:
    template<class CompletionToken>
    void get_response(CompletionToken &&completion_handler)
    {
        auto self = shared_from_this();
        connection->read_async(asio::buffer(receive_buffer), [self, &completion_handler](std::error_code error, std::size_t bytes_received) {
            if (error and error.value() != asio::error::eof) {
                return;
            }
            ESP_LOGD(TAG, "Bytes Received: %d", bytes_received);
            if (bytes_received == 0) {
                return;
            }
            Response response(std::begin(self->receive_buffer), bytes_received);

            completion_handler(self, response);
        });
    }
    /*
     * For this example we assumed 2048 to be enough for the receive_buffer
     */
    std::array<char, 2048> receive_buffer;
    /*
     * The hardcoded 2 below is related to the type we receive the data to send. We gather the parts from Request, header
     * and body, to send avoiding the copy.
     */
    std::array<asio::const_buffer, 2> send_data;
    std::shared_ptr<Connection> connection;
};
}// namespace Http

extern "C" void app_main(void)
{
    // Basic initialization of ESP system
    esp_init();

    asio::io_context io_context;
    Http::Request request(Http::Method::GET, "www.httpbin.org", "80", "/get");
    Socks::async_connect(io_context, CONFIG_EXAMPLE_PROXY_ADDRESS, CONFIG_EXAMPLE_PROXY_PORT, request.host(), request.service_port(),
    [&request](std::shared_ptr<Connection> connection) {
        // Now we create a HTTP::Session and inject the necessary connection.
        std::make_shared<Http::Session>(connection)->send_request(request, [](std::shared_ptr<Http::Session> session, Http::Response response) {
            response.print();
        });
    });
    // io_context.run will block until all the tasks on the context are done.
    io_context.run();
    ESP_LOGI(TAG, "Context run done");

    ESP_ERROR_CHECK(example_disconnect());
}
