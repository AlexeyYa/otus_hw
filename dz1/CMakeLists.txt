cmake_minimum_required(VERSION 3.5)

if($ENV{TRAVIS_BUILD_NUMBER})
    project(helloworld VERSION 0.1.$ENV{TRAVIS_BUILD_NUMBER} LANGUAGES CXX)
elseif($ENV{APPVEYOR_BUILD_NUMBER})
    project(helloworld VERSION 0.1.$ENV{APPVEYOR_BUILD_NUMBER} LANGUAGES CXX)
else()
    project(helloworld VERSION 0.1.1 LANGUAGES CXX)
endif()

message(STATUS "Setting up project")
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

# Now simply link against gtest or gtest_main as needed. Eg
#add_executable(example example.cpp)
#target_link_libraries(example gtest_main)
#add_test(NAME example_test COMMAND example)
message(VERBOSE "Setting version and ci/cd")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_VERSION "Use version" ON)


configure_file(bintray.json.in bintray.json)
if(USE_VERSION)
    configure_file(tutorial.h.in tutorial.h)
endif()

message(STATUS "Compilation")
add_executable(helloworld main.cpp)

target_include_directories(helloworld PUBLIC
                            "${PROJECT_BINARY_DIR}"
                            )

install(TARGETS helloworld DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/tutorial.h"
    DESTINATION include
    )

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "${helloworld_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${helloworld_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${helloworld_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexey")
include(CPack)

message(NOTICE "Adding tests")
enable_testing()
add_executable(tests tests/tests.cpp)

# Add src to the include directories for the test target
target_include_directories(
    tests PRIVATE
    "${PROJECT_BINARY_DIR}"
)

# Link test target against gtest libraries
target_link_libraries(tests PRIVATE gtest gtest_main)

if (MSVC)
    target_compile_options(
        helloworld PRIVATE
        /W4
    )
    target_compile_options(
        tests PRIVATE
        /W4
    )
else ()
    target_compile_options(
        helloworld PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(
        tests PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

add_test(NAME Runs COMMAND helloworld 0)
add_test(tests tests)