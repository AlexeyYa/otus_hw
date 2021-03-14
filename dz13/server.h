/*! @file join_server.h
 *  @brief Заголовочный файл сервера
 */

#ifndef SERVER_H
#define SERVER_H

#include "isession.h"
#include "parse_input.h"

#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

class session
  : public Isession
{
public:
    session(tcp::socket socket) : socket_(std::move(socket))
    {}

    void start()
    {
        do_read();
    }

    void do_write(const std::string& content)
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(content.c_str(), content.size()),
            [](boost::system::error_code /*ec*/, std::size_t /*length*/)
            { /*std::cout << ec.message() << std::endl;*/ });
    }
private:
    void do_read()
    {
        auto self(shared_from_this());


        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                [this, self](boost::system::error_code ec, std::size_t length)
            {

                if (!ec && std::strncmp(data_, "q", 1))
                {
                    do_process_recieved(length);
                    do_read();
                }
                else
                {
                    std::cerr << "Disconnected\n";
                }
            });

    }
    void do_process_recieved(size_t length)
    {
        size_t pos = 0;
        char sep = '\n';
        for (size_t i = 0; i < length; ++i)
        {
            if (data_[i] == sep)
            {
                std::string str(data_ + pos, i - pos + 1);
                Parse(str, shared_from_this());

                pos = i;
            }
        }
        if (pos != length - 1)
            std::cerr << "Not implemented\n";
            //incomlete_ = data_ + pos;
    }

    tcp::socket socket_;
    enum { max_incomplete = 128, max_length = 1024 };
    char data_[max_length];
    char incomlete_[max_incomplete];
};

class server
{
public:
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<session>(std::move(socket))->start();
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

#endif
