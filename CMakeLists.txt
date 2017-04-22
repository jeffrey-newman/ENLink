cmake_minimum_required(VERSION 3.6)
project(ENLink)
set(CMAKE_CXX_STANDARD 11)

IF(MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
ENDIF(MSVC)

IF(WIN32)
	SET(USE_BOOST_DLL ON CACHE BOOL "Whether using shared (dynamic) target for boost libraries")
ENDIF(WIN32)

IF(WIN32)
IF(${USE_BOOST_DLL})
	set(Boost_USE_STATIC_LIBS        OFF)
	set(Boost_USE_MULTITHREADED      ON)
	set(Boost_USE_STATIC_RUNTIME     OFF)
	set(BOOST_ALL_DYN_LINK           ON )   # force dynamic linking for all libraries
	ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
ELSE(${USE_BOOST_DLL})
	set(Boost_USE_STATIC_LIBS        ON)
	set(Boost_USE_MULTITHREADED      ON)
	set(Boost_USE_STATIC_RUNTIME     ON)
	set(BOOST_ALL_DYN_LINK           OFF )
ENDIF(${USE_BOOST_DLL})
ENDIF(WIN32)

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
	SET(Redefine_snprintf OFF CACHE BOOL "Whether to define snprintf to use _snprintf, seems required for vs2013")
	if (Redefine_snprintf)
		ADD_DEFINITIONS(-DRedefine_snprintf)
	ENDIF(Redefine_snprintf)	

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

install(TARGETS ENLink
		DESTINATION ${CMAKE_BINARY_DIR}/bin
		CONFIGURATIONS Release RelWithDebInfo Debug)
		
function(install_boost_components_debug BOOST_VAR)
	get_filename_component(path_release_base 
          ${BOOST_VAR} NAME_WE)
#		  message(STATUS "path_release_base: " ${path_release_base})
	get_filename_component(path_release_dir 
          ${BOOST_VAR} DIRECTORY)

	install(FILES "${path_release_dir}/${path_release_base}.dll"
          DESTINATION ${CMAKE_BINARY_DIR}/bin
          CONFIGURATIONS Debug 
        )
endfunction(install_boost_components_debug)

IF (WIN32)
IF(${USE_BOOST_DLL})
	install_boost_components_debug(${Boost_FILESYSTEM_LIBRARY_DEBUG})
	install_boost_components_debug(${Boost_CHRONO_LIBRARY_DEBUG})
	install_boost_components_debug(${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG})
	install_boost_components_debug(${Boost_SYSTEM_LIBRARY_DEBUG})
	install_boost_components_debug(${Boost_TIMER_LIBRARY_DEBUG})
ENDIF(${USE_BOOST_DLL})
ENDIF(WIN32)

function(install_boost_components_release BOOST_VAR)
	get_filename_component(path_release_base 
          ${BOOST_VAR} NAME_WE)
#		  message(STATUS "path_release_base: " ${path_release_base})
	get_filename_component(path_release_dir 
          ${BOOST_VAR} DIRECTORY)

	install(FILES "${path_release_dir}/${path_release_base}.dll"
          DESTINATION ${CMAKE_BINARY_DIR}/bin
          CONFIGURATIONS Release RelWithDebInfo
        )
endfunction(install_boost_components_release)

IF (WIN32)
IF(${USE_BOOST_DLL})
	install_boost_components_release(${Boost_FILESYSTEM_LIBRARY_RELEASE})
	install_boost_components_release(${Boost_CHRONO_LIBRARY_RELEASE})
	install_boost_components_release(${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE})
	install_boost_components_release(${Boost_SYSTEM_LIBRARY_RELEASE})
	install_boost_components_release(${Boost_TIMER_LIBRARY_RELEASE})
ENDIF(${USE_BOOST_DLL})
ENDIF(WIN32)

