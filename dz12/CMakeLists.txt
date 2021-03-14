cmake_minimum_required(VERSION 3.5)

if($ENV{TRAVIS_BUILD_NUMBER})
    project(bulk_server VERSION 0.1.$ENV{TRAVIS_BUILD_NUMBER} LANGUAGES CXX)
elseif($ENV{APPVEYOR_BUILD_NUMBER})
    project(bulk_server VERSION 0.1.$ENV{APPVEYOR_BUILD_NUMBER} LANGUAGES CXX)
else()
    project(bulk_server VERSION 0.1.1 LANGUAGES CXX)
endif()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)
# End of gtest import

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

if($ENV{APPVEYOR_BUILD_NUMBER})
    set (Boost_USE_STATIC_LIBS ON)
    set (Boost_USE_MULTITHREADED ON)
    set (Boost_USE_STATIC_RUNTIME OFF)
endif()

find_package(Boost COMPONENTS system asio)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIR})

configure_file(bintray.json.in bintray.json)

add_library(async SHARED libasync.cpp)
target_link_libraries(async Threads::Threads)

add_executable(bulk_server bulk_server.cpp)
target_link_libraries(bulk_server async Threads::Threads)

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE gtest gtest_main async Threads::Threads)

# Install library
install(TARGETS bulk_server DESTINATION bin)
install(TARGETS async DESTINATION lib)

# Install headers
install(FILES libasync.h DESTINATION include)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "${bulk_server_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${bulk_server_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${bulk_server_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexey")
include(CPack)

enable_testing()
add_test(unit_tests tests)

if (MSVC)
    target_compile_options(
        async PRIVATE
        /W4
    )
    target_compile_options(
        bulk_server PRIVATE
        /W4
    )
    add_compile_definitions(_WIN32_WINNT=0x0601)
else ()
    target_compile_options(
        async PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(
        bulk_server PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()
