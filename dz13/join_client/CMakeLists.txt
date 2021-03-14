cmake_minimum_required(VERSION 3.5)

if($ENV{TRAVIS_BUILD_NUMBER})
    project(join_client VERSION 0.1.$ENV{TRAVIS_BUILD_NUMBER} LANGUAGES CXX)
elseif($ENV{APPVEYOR_BUILD_NUMBER})
    project(join_client VERSION 0.1.$ENV{APPVEYOR_BUILD_NUMBER} LANGUAGES CXX)
else()
    project(join_client VERSION 0.1.1 LANGUAGES CXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

find_package(Boost COMPONENTS system asio)
include_directories(${Boost_INCLUDE_DIR})

add_executable(join_client bulk_client.cpp)
target_link_libraries(join_client Threads::Threads)


if (MSVC)
    target_compile_options(
        join_client PRIVATE
        /W4
    )
    add_compile_definitions(_WIN32_WINNT=0x0601)
else ()
    target_compile_options(
        join_client PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()
