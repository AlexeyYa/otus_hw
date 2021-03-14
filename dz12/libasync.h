/*! @file main.cpp
 *  @brief Исполняемый файл
 */

#pragma once

#include <iostream>
#include <functional>

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

//extern "C" LIBASYNC_API handle_t connect(size_t block_size, std::ostream& os = std::cout);
extern "C" LIBASYNC_API handle_t connect(size_t block_size, std::function<void(const std::string&)> callback);

extern "C" LIBASYNC_API void receive(char* buffer, size_t buffer_size, handle_t context);

extern "C" LIBASYNC_API void disconnect(handle_t context);
}
