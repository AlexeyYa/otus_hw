/*! @file bulk_server.cpp
 *  @brief Сервер
 */

#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#include "bulk_server.h"

void session::do_read()
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
                async::disconnect(handle);
            }
        });

}

void session::do_process_recieved(size_t length)
{
    size_t pos = 0;
    char sep = '\n';
    for (size_t i = 0; i < length; ++i)
    {
        if (data_[i] == sep)
        {
            if (pos != i)
                async::receive(data_ + pos, i - pos, this->handle);
            pos = i + 1;
        }
    }
    if (length - pos)
        async::receive(data_ + pos, length - pos, this->handle);
}

void session::do_write(const std::string& content)
{
    std::cout << content;
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(content.c_str(), content.size()),
        [](boost::system::error_code /*ec*/, std::size_t /*length*/)
        { /*std::cout << ec.message() << std::endl;*/ });
}


void server::do_accept()
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

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: bulk_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server server(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
    }
    return 0;
}
