/*! @file libasync.cpp
 *  @brief
 */
 
#define LIBASYNC_EXPORTS

#include "libasync.h"
#include "bulk.h"
#include "cmd.h"
#include <string>
#include <mutex>
#include <map>
#include <memory>

namespace async {

/*!
 * @brief Внутреннее состояние
 */
struct State{
    std::mutex console_mutex;
    std::map<size_t, std::unique_ptr<Bulk<Command>>> bulks;
    size_t counter = 0;
} libasync_state;

/*handle_t connect(size_t block_size, std::ostream& os)
{
    auto ptr = std::make_unique<Bulk<Command>>(os, libasync_state.console_mutex, libasync_state.counter, block_size);
    auto pair = std::make_pair(async::libasync_state.counter,
                               std::move(ptr));
    async::libasync_state.bulks.emplace(std::move(pair));
    return async::libasync_state.counter++;
}*/

handle_t connect(size_t block_size, std::function<void(const std::string&)> callback)
{
    auto ptr = std::make_unique<Bulk<Command>>(callback, libasync_state.console_mutex, libasync_state.counter, block_size);
    auto pair = std::make_pair(async::libasync_state.counter,
                               std::move(ptr));
    async::libasync_state.bulks.emplace(std::move(pair));
    return async::libasync_state.counter++;
}

void receive(char* buffer, size_t buffer_size, handle_t context)
{
    auto it = async::libasync_state.bulks.find(context);
    if (it != async::libasync_state.bulks.end())
        it->second->Parse(std::string(buffer, buffer_size));
}

void disconnect(handle_t context)
{
    //std::cout << "Bulk " << context << '\n';
    async::libasync_state.bulks.erase(async::libasync_state.bulks.find(context));
}

}
