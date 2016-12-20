cmake_minimum_required(VERSION 2.8)

# Options.
OPTION(ENABLE_TESTING "Build the library with tests" OFF)

# Workaround for OS X.
if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()

# Description.
set(LIBRARY_NAME metrics)

project(${LIBRARY_NAME})

find_package(Boost 1.46 REQUIRED COMPONENTS system)

include_directories(${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/foreign/rapidjson/include)

set(SOURCES
    src/accumulator/sliding/window
    src/accumulator/snapshot/uniform
    src/accumulator/snapshot/weighted
    src/ewma
    src/factory
    src/format/json.cpp
    src/meter
    src/metric
    src/registry
    src/tags
    src/timer
    src/usts/ewma)

add_library(${LIBRARY_NAME} SHARED ${SOURCES})

target_link_libraries(${LIBRARY_NAME}
    ${Boost_LIBRARIES})

set_target_properties(${LIBRARY_NAME} PROPERTIES
    COMPILE_FLAGS "-std=c++0x -fPIC -Wall -Wextra -Winit-self -Wno-sign-conversion -Wold-style-cast -Woverloaded-virtual -Wctor-dtor-privacy -Wnon-virtual-dtor -Wunreachable-code -pedantic -pedantic-errors"
)

# The rule is that: any breakage of the ABI must be indicated by incrementing the SOVERSION.
# So, adding e.g. functions is no problem, modifying argument lists or removing functions would
# required the SOVERSION to be incremented. Similar rules hold of course for non-opaque
# data-structures.
set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION 2.0.0)
set_target_properties(${LIBRARY_NAME} PROPERTIES SOVERSION 2)

# Install section.
install(
    TARGETS
        ${LIBRARY_NAME}
    LIBRARY DESTINATION lib)

install(
    DIRECTORY
        ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
    COMPONENT development)

if (ENABLE_TESTING)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/foreign/modules")

    enable_testing()

    find_package(Threads)

    # Prepare testing infrastructure.
    include(DownloadGoogleTesting)

    download_google_testing()

    include_directories(
        SYSTEM ${GTEST_INCLUDE_DIR}
        SYSTEM ${GMOCK_INCLUDE_DIR})

    link_directories(${GTEST_BINARY_DIR} ${GMOCK_BINARY_DIR})

    include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include)

    set(SOURCES
        tests/accumulator/sliding/window
        tests/accumulator/snapshot/uniform
        tests/accumulator/snapshot/weighted
        tests/counter
        tests/detail/cpp14/tuple
        tests/detail/ewma
        tests/detail/histogram
        tests/detail/meter
        tests/detail/timer
        tests/format.cpp
        tests/gauge
        tests/meter
        tests/registry
        tests/tagged
        tests/usts
        # tests/timer
    )

    add_executable(${LIBRARY_NAME}-tests ${SOURCES})

    add_dependencies(${LIBRARY_NAME}-tests googlemock)

    target_link_libraries(${LIBRARY_NAME}-tests
        ${LIBRARY_NAME}
        ${CMAKE_THREAD_LIBS_INIT}
        gmock
        gtest
        gtest_main)

    set_target_properties(${LIBRARY_NAME}-tests PROPERTIES
        COMPILE_FLAGS "-std=c++0x -Wall -Wextra -Winit-self -Wno-sign-conversion -Wold-style-cast -Woverloaded-virtual -Wctor-dtor-privacy -Wnon-virtual-dtor -Wunreachable-code -pedantic -pedantic-errors"
    )

    add_test(metrics ${LIBRARY_NAME}-tests)
endif()
