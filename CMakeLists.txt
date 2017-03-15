cmake_minimum_required(VERSION 3.6)
project(EN2Link)

set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(Boost 1.40 REQUIRED COMPONENTS filesystem system program_options timer)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES
        EN2Link.cpp
        EN2Link.h
        ENMultiObjEvaluator.cpp
        ENMultiObjEvaluator.h
        OptParser.cpp
        OptParser.h)

add_library(EN2Link SHARED ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(EN2Link ${Boost_LIBRARIES})

add_subdirectory(tests)