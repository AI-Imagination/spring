cmake_minimum_required(VERSION 3.2)
project(spring)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-fPIC")

option(WITH_TESTING     "Compile VisualDL with unit testing"        ON)

set(THIRD_PARTY_PATH "${CMAKE_BINARY_DIR}/third_party" CACHE STRING
        "A path setting third party libraries download & build directories.")

include(external/gflags)    # download, build, install gflags
include(external/glog)      # download, build, install glog
include(external/gtest)     # download, build, install gtest

add_executable(spring main.cc gc.cc _memory.h lexer.h test_lexer.cc lexer.cc parser.cc variable.h ${HEADER_FILES} gc.cc gc.h memory.cc memory.h tests/test_memory.cc status.h)
target_link_libraries(spring gtest glog pthread)
