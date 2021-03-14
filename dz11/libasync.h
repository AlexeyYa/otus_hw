/*! @file main.cpp
 *  @brief Исполняемый файл
 */

#pragma once

#include <iostream>

#ifdef _WIN32
    #ifdef LIBASYNC_EXPORTS
        #define LIBASYNC_API __declspec(dllexport)
    #else
        #define LIBASYNC_API __declspec(dllimport)
    #endif
#else
        #define LIBASYNC_API
#endif

namespace async{

using handle_t = size_t;

extern "C" LIBASYNC_API handle_t connect(size_t block_size, std::ostream& os = std::cout);

extern "C" LIBASYNC_API void receive(handle_t handle, const char *data, std::size_t size);

extern "C" LIBASYNC_API void disconnect(handle_t context);
}
