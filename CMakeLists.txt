cmake_minimum_required(VERSION 3.6)
project(ENLink)

set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(Boost 1.40 REQUIRED COMPONENTS chrono filesystem system program_options timer)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES
        ENLink.cpp
        ENLink.h
        ENMultiObjEvaluator.cpp
        ENMultiObjEvaluator.h
        OptParser.cpp
        OptParser.h
        InitialiseIbanezEN2.h
        InitialiseOwaEN2.h)


IF (WIN32)
	include_directories("dlfcn-win32-1.1.0")
	set (SOURCE_FILES ${SOURCE_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/dlfcn-win32-1.1.0/dlfcn.c")
ENDIF(WIN32) 

add_library(ENLink SHARED ${SOURCE_FILES})
if (WIN32)
        TARGET_LINK_LIBRARIES(ENLink ${Boost_LIBRARIES} psapi)
ELSE(WIN32)
        TARGET_LINK_LIBRARIES(ENLink ${Boost_LIBRARIES})
ENDIF(WIN32)

add_subdirectory(tests)
add_subdirectory(Ibanez_EN2)
add_subdirectory(OWA_EN2)
add_subdirectory(OWA_EN3)