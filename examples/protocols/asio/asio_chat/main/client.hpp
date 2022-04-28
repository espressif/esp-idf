//
// client.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#include <deque>
#include "asio.hpp"
#include "chat_message.hpp"

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{
public:
  chat_client(asio::io_context& io_context,
      const asio::ip::tcp::resolver::results_type& endpoints)
    : io_context_(io_context),
      socket_(io_context)
  {
    do_connect(endpoints);
  }

  void write(const chat_message& msg)
  {
    asio::post(io_context_,
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }

  void close()
  {
    asio::post(io_context_, [this]() { socket_.close(); });
  }

private:
void do_connect(const asio::ip::tcp::resolver::results_type& endpoints)
    {
      asio::async_connect(socket_, endpoints,
          [this](std::error_code ec, asio::ip::tcp::endpoint)
          {
            if (!ec)
            {
              do_read_header();
            }
          });
    }

    void do_read_header()
    {
      asio::async_read(socket_,
          asio::buffer(read_msg_.data(), chat_message::header_length),
          [this](std::error_code ec, std::size_t /*length*/)
          {
            if (!ec && read_msg_.decode_header())
            {
              do_read_body();
            }
            else
            {
              socket_.close();
            }
          });
    }

    void do_read_body()
    {
      asio::async_read(socket_,
          asio::buffer(read_msg_.body(), read_msg_.body_length()),
          [this](std::error_code ec, std::size_t /*length*/)
          {
            if (!ec)
            {
              do_read_header();
            }
            else
            {
              socket_.close();
            }
          });
    }

     void do_write()
     {
      asio::async_write(socket_,
          asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
          [this](std::error_code ec, std::size_t /*length*/)
          {
            if (!ec)
            {
              write_msgs_.pop_front();
              if (!write_msgs_.empty())
              {
                do_write();
              }
            }
            else
            {
              socket_.close();
            }
          });
      }

private:
    asio::io_context& io_context_;
    asio::ip::tcp::socket socket_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
};

#endif // CHAT_CLIENT_HPP
