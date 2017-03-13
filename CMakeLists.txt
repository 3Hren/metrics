cmake_minimum_required(VERSION 2.8)

# Description.
set(LIBRARY_NAME metrics)
project(${LIBRARY_NAME})

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Choose new behaviour for CMP0042.
# See http://www.cmake.org/cmake/help/v3.0/policy/CMP0042.html for more details.
if (POLICY CMP0042)
	cmake_policy(SET CMP0042 NEW)
endif (POLICY CMP0042)

find_package(Boost 1.54 REQUIRED COMPONENTS
    system
)

# System is required to get rid of errors in foreign libs
include_directories(BEFORE SYSTEM ${Boost_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(${LIBRARY_NAME} SHARED
    src/accumulator/sliding/window
    src/accumulator/decaying/exponentially
    src/accumulator/snapshot/uniform
    src/accumulator/snapshot/weighted
    src/ewma
    src/factory
    src/meter
    src/metric
    src/registry
    src/tags
    src/timer
    src/usts/ewma
)

target_link_libraries(${LIBRARY_NAME}
    ${Boost_LIBRARIES})

set(COMPILE_FLAGS "-std=c++11")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wall")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wextra")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Waddress")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Warray-bounds")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wbuiltin-macro-redefined")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wctor-dtor-privacy")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Winit-self")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wnon-virtual-dtor")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wold-style-cast")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Woverloaded-virtual")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wswitch")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wunreachable-code")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -pedantic -pedantic-errors")

set_target_properties(${LIBRARY_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})

# The rule is that: any breakage of the ABI must be indicated by incrementing the SOVERSION.
# So, adding e.g. functions is no problem, modifying argument lists or removing functions would
# required the SOVERSION to be incremented. Similar rules hold of course for non-opaque
# data-structures.
set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION 3.0.0)
set_target_properties(${LIBRARY_NAME} PROPERTIES SOVERSION 3)

# Install section.
install(
    TARGETS
        ${LIBRARY_NAME}
    LIBRARY DESTINATION lib
)

install(
    DIRECTORY
        ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
    COMPONENT development
)

# Testing Framework.
enable_testing()

add_subdirectory(${PROJECT_SOURCE_DIR}/tests/.googletest EXCLUDE_FROM_ALL)

include_directories(BEFORE SYSTEM
    ${Boost_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/tests/.googletest/googletest/include
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wno-gnu-zero-variadic-macro-arguments" SUPPORTS_NO_GNU_ZERO_VARIADIC_MACRO_ARGUMENTS_FLAG)

# Some parts of gtest rely on this GNU extension, don't warn on it.
if(SUPPORTS_NO_GNU_ZERO_VARIADIC_MACRO_ARGUMENTS_FLAG)
	add_definitions("-Wno-gnu-zero-variadic-macro-arguments")
endif()

add_executable(libmetrics-tests
    tests/accumulator/sliding/window
    tests/accumulator/decaying/exponentially
    tests/accumulator/snapshot/uniform
    tests/accumulator/snapshot/weighted
    tests/counter
    tests/detail/cpp14/tuple
    tests/detail/ewma
    tests/detail/histogram
    tests/detail/meter
    tests/detail/timer
    tests/gauge
    tests/meter
    tests/registry
    tests/tagged
)

set_target_properties(libmetrics-tests PROPERTIES
	COMPILE_FLAGS "${COMPILE_FLAGS}"
)

target_link_libraries(libmetrics-tests
	metrics
	gmock
	gtest
	gtest_main
)

add_test(metrics libmetrics-tests)
