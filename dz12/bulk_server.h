/*! @file bulk_server.h
 *  @brief Заголовочный файл сервера
 */

#ifndef BULKSERVER_H
#define BULKSERVER_H

#include "libasync.h"

#include <boost/asio.hpp>
#include <memory>

using boost::asio::ip::tcp;

class session
  : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket) : socket_(std::move(socket))
    {}

    void start()
    {
        handle = async::connect(3,[&](const std::string& content){
            this->do_write(content); });
        do_read();
    }
private:
    void do_read();
    void do_process_recieved(size_t length);
    void do_write(const std::string& content);

    async::handle_t handle;
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
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
    void do_accept();

    tcp::acceptor acceptor_;
};

#endif
