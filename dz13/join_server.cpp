/*! @file join_server.cpp
 *  @brief Сервер
 */

#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#include "server.h"
#include <string>

/*
class MockSession : public Isession
{
    virtual void do_write(const std::string &content) override
    {
        std::cout << content << std::endl;
    }
};*/

int main(int argc, char* argv[])
{
    int cmd_thread_count = 2;
    std::vector<std::thread> threads;
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: join_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server server(io_context, std::atoi(argv[1]));

        for (int i = 0; i < cmd_thread_count; i++)
            threads.emplace_back([&]()
            {
                std::shared_ptr<Command> cmd;
                while (CollectionsData_.CommandQueue.wpop(cmd))
                {
                    cmd->Execute();
                }
            });

        io_context.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
/*
    std::string str;
    auto ses = std::make_shared<MockSession>();
    std::thread t1([&]()
    {
        while (getline(std::cin, str))
        {
            str = str + '\n';
            Parse(str, ses);
        }
    });

    std::thread t2([&]()
    {
        std::shared_ptr<Command> cmd;
        while (CollectionsData_.CommandQueue.wpop(cmd))
        {
            cmd->Execute();
        }
    });
    t1.join();
    t2.join();
*/
    return 0;
}
