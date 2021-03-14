/*! @file bulk_client.h
 *  @brief Заголовочный файл сервера
 */

#ifndef BULKCLIENT_H
#define BULKCLIENT_H

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>

namespace ba = boost::asio;

class AsyncClient {
public:
    AsyncClient(ba::io_context& io_context)
        : m_tcp_socket{io_context}
        , m_resolver{io_context} {

    }

    // Active object? :)
    void get(const std::string& host, const std::string& service) {

        // 1. Need to resolve the host name
        ba::ip::tcp::resolver::query query{host, service};
        m_resolver.async_resolve(
            query,
            [&](const boost::system::error_code &ec, ba::ip::tcp::resolver::iterator it) {
                this->resolveHandler(ec, it);
            }
        );
    }

private:
    void resolveHandler(const boost::system::error_code &ec, ba::ip::tcp::resolver::iterator it) {
        if (ec) {
            std::cout << "resolveHandler - failed! err = " << ec.message() << std::endl;
            return;
        }

        if (it == ba::ip::tcp::resolver::iterator{}) {
            std::cout << "resolveHandler - no address!" << std::endl;
            return;
        }

        m_tcp_socket.async_connect(
            *it,
            [&](const boost::system::error_code &ec) {
                this->connectHandler(ec);
            }
        );
    }

    void connectHandler(const boost::system::error_code &ec) {
        if (ec) {
            std::cout << "connectHandler - failed! err = " << ec.message() << std::endl;
            return;
        }

        const std::string request = "GET /notExists HTTP/1.1\r\n\r\n";
        ba::write(m_tcp_socket, ba::buffer(request));

        m_tcp_socket.async_read_some(
            ba::buffer(m_buffer),
            [&](const boost::system::error_code &ec, std::size_t bytes_transferred) {
                this->readHandler(ec, bytes_transferred);
            }
        );
    }

    void readHandler(const boost::system::error_code &ec, std::size_t bytes_transferred) {
        if (ec) {
            std::cout << "readHandler - failed! err = " << ec.message() << std::endl;
            return;
        }

        if (!bytes_transferred) {
            std::cout << "readHandler - no more data!" << std::endl;
            return;
        }

        std::cout << "[readHandler] - read " << bytes_transferred << " bytes" << std::endl;

        std::cout.write(m_buffer.data(), bytes_transferred);
        m_tcp_socket.async_read_some(
            ba::buffer(m_buffer),
            [&](const boost::system::error_code &ec, std::size_t bytes_transferred) {
                readHandler(ec, bytes_transferred);
            }
        );
    }

    ba::ip::tcp::socket m_tcp_socket;
    ba::ip::tcp::resolver m_resolver;
    std::array<char, 4096> m_buffer;

};

#endif
