cmake_minimum_required(VERSION 3.5)

if($ENV{TRAVIS_BUILD_NUMBER})
    project(yamr VERSION 0.1.$ENV{TRAVIS_BUILD_NUMBER} LANGUAGES CXX)
elseif($ENV{APPVEYOR_BUILD_NUMBER})
    project(yamr VERSION 0.1.$ENV{APPVEYOR_BUILD_NUMBER} LANGUAGES CXX)
else()
    project(yamr VERSION 0.1.1 LANGUAGES CXX)
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

configure_file(bintray.json.in bintray.json)

add_executable(yamr main.cpp)
target_link_libraries(yamr Threads::Threads)

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE gtest gtest_main Threads::Threads)


install(TARGETS yamr DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "${yamr_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${yamr_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${yamr_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexey")
include(CPack)

enable_testing()
add_test(unit_tests tests)

if (MSVC)
    target_compile_options(
        yamr PRIVATE
        /W4
    )
    add_compile_definitions(_WIN32_WINNT=0x0601)
else ()
    target_compile_options(
        yamr PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()
