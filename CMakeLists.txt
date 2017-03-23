cmake_minimum_required(VERSION 3.6)
project(EN2Link)

set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(Boost 1.40 REQUIRED COMPONENTS chrono filesystem system program_options timer)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES
        EN2Link.cpp
	    EN2Link.h
        ENMultiObjEvaluator.cpp
        ENMultiObjEvaluator.h
        OptParser.cpp
        OptParser.h)

IF (WIN32)
	include_directories("dlfcn-win32-1.1.0")
	set (SOURCE_FILES ${SOURCE_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/dlfcn-win32-1.1.0/dlfcn.c")
ENDIF(WIN32) 

add_library(EN2Link SHARED ${SOURCE_FILES})
if (WIN32)
        TARGET_LINK_LIBRARIES(EN2Link ${Boost_LIBRARIES} psapi)
ELSE(WIN32)
        TARGET_LINK_LIBRARIES(EN2Link ${Boost_LIBRARIES})
ENDIF(WIN32)

add_subdirectory(tests)