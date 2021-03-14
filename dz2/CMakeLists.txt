cmake_minimum_required(VERSION 3.5)

if($ENV{TRAVIS_BUILD_NUMBER})
    project(ip_filter VERSION 0.1.$ENV{TRAVIS_BUILD_NUMBER} LANGUAGES CXX)
elseif($ENV{APPVEYOR_BUILD_NUMBER})
    project(ip_filter VERSION 0.1.$ENV{APPVEYOR_BUILD_NUMBER} LANGUAGES CXX)
else()
    project(ip_filter VERSION 0.1.1 LANGUAGES CXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(USE_BOOST "Use boost" ON)

configure_file(bintray.json.in bintray.json)
configure_file(ip.h.in ip.h)

add_executable(ip_filter ip_filter.cpp)
target_include_directories(ip_filter PUBLIC
                            "${PROJECT_BINARY_DIR}"
                            )


add_executable(tests tests.cpp)
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_definitions("-std=c++17")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

target_include_directories(tests PUBLIC
                            "${PROJECT_BINARY_DIR}"
                            )

if(USE_BOOST)
    target_link_libraries(ip_filter ${Boost_LIBRARIES})
elseif(WIN32)
    target_link_libraries(ip_filter wsock32 ws2_32)
    target_link_libraries(tests wsock32 ws2_32)
endif()

install(TARGETS ip_filter DESTINATION bin)

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "${ip_filter_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${ip_filter_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${ip_filter_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexey")
include(CPack)

enable_testing()
add_test(unit_tests tests)
include(FindUnixCommands)

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ip_filter.tsv" _in)
if (UNIX)
    add_test(
        NAME md5sum
        COMMAND ${BASH} -c "./ip_filter < ${_in} | md5sum"
    )
    set_tests_properties(md5sum
        PROPERTIES PASS_REGULAR_EXPRESSION "24e7a7b2270daee89c64d3ca5fb3da1a  -"
    )
else()
    if (WIN32)
        add_test(
            NAME md5sum
            COMMAND powershell "Get-Content ip_filter.tsv | ./Release/ip_filter | Out-File -FilePath ./output.txt; (Get-FileHash ./output.txt -Algorithm MD5).Hash"
        )
        set_tests_properties(md5sum
            PROPERTIES PASS_REGULAR_EXPRESSION "6BA70A55EE52AB9E8C313FDFADAAAB00"
        )
    else()
        message(FATAL_ERROR "Unknown shell command for ${CMAKE_HOST_SYSTEM_NAME}")
    endif()
endif()

if (MSVC)
    target_compile_options(
        ip_filter PRIVATE
        /W4
    )
    add_compile_definitions(_WIN32_WINNT=0x0601)
else ()
    target_compile_options(
        ip_filter PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()
