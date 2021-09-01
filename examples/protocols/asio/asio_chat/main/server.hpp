//
// server.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include <list>
#include <set>
#include <deque>
#include <utility>
#include "asio.hpp"
#include "chat_message.hpp"

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

extern std::mutex server_ready;

//----------------------------------------------------------------------


class chat_participant
{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_msgs_)
      participant->deliver(msg);
  }

  void leave(chat_participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const chat_message& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_)
      participant->deliver(msg);
  }

private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------


class chat_session
  : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
public:
  chat_session(asio::ip::tcp::socket socket, chat_room& room)
    : socket_(std::move(socket)),
      room_(room)
  {
  }

  void start()
  {
    room_.join(shared_from_this());
    do_read_header();
  }

  void deliver(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:
  void do_read_header()
  {
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            room_.leave(shared_from_this());
          }
         });
   }

   void do_read_body()
   {
     auto self(shared_from_this());
     asio::async_read(socket_,
         asio::buffer(read_msg_.body(), read_msg_.body_length()),
         [this, self](std::error_code ec, std::size_t /*length*/)
         {
           if (!ec)
           {
             ESP_LOGD("asio-chat:", "%s", read_msg_.body());
             room_.deliver(read_msg_);
             do_read_header();
           }
           else
           {
             room_.leave(shared_from_this());
           }
         });
   }

   void do_write()
   {
     auto self(shared_from_this());
     asio::async_write(socket_,
         asio::buffer(write_msgs_.front().data(),
           write_msgs_.front().length()),
         [this, self](std::error_code ec, std::size_t /*length*/)
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
             room_.leave(shared_from_this());
           }
         });
   }

   asio::ip::tcp::socket socket_;
   chat_room& room_;
   chat_message read_msg_;
   chat_message_queue write_msgs_;
 };

//----------------------------------------------------------------------

class chat_server
{
public:
  chat_server(asio::io_context& io_context,
      const asio::ip::tcp::endpoint& endpoint)
    : acceptor_(io_context, endpoint)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    std::lock_guard<std::mutex> guard(server_ready);
    acceptor_.async_accept(
       [this](std::error_code ec, asio::ip::tcp::socket socket)
       {
         if (!ec)
         {
           std::make_shared<chat_session>(std::move(socket), room_)->start();
         }

         do_accept();
       });
  }

  asio::ip::tcp::acceptor acceptor_;
  chat_room room_;
};

#endif // CHAT_SERVER_HPP
