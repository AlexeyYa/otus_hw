/*! @file libasync.cpp
 *  @brief
 */
 
#define LIBASYNC_EXPORTS

#include "libasync.h"
#include "bulk.h"
#include "cmd.h"
#include <string>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <memory>

namespace async {

/*!
 * @brief Внутреннее состояние
 */
struct State{
    std::mutex console_mutex;
    std::shared_mutex bulks_mutex;
    std::map<size_t, std::unique_ptr<Bulk<Command>>> bulks;
    size_t counter = 0;
} libasync_state;

handle_t connect(size_t block_size, std::ostream& os)
{
    auto ptr = std::make_unique<Bulk<Command>>(os, libasync_state.console_mutex, libasync_state.counter, block_size);
    auto pair = std::make_pair(async::libasync_state.counter,
                               std::move(ptr));
    std::unique_lock lock(libasync_state.bulks_mutex);
    async::libasync_state.bulks.emplace(std::move(pair));
    return async::libasync_state.counter++;
}

void receive(handle_t handle, const char *data, std::size_t size)
{
    std::shared_lock lock(libasync_state.bulks_mutex);
    auto it = async::libasync_state.bulks.find(handle);
    if (it == async::libasync_state.bulks.end())
        return;

    size_t pos = 0;
    char sep = '\n';
    for (size_t i = 0; i < size; ++i)
    {
        if (data[i] == sep)
        {
            if (pos != i)
                it->second->Parse(std::string(data + pos, i - pos));
            pos = i + 1;
        }
    }
    if (size != pos)
        it->second->Parse(std::string(data + pos, size - pos));
}

void disconnect(handle_t context)
{
    std::unique_lock lock(libasync_state.bulks_mutex);
    async::libasync_state.bulks.erase(async::libasync_state.bulks.find(context));
}

}
