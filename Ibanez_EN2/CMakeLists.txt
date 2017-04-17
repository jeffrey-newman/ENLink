## cmake .
## make

# cmake_minimum_required (VERSION 2.6)

# SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
# SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# project (IBANEZ_EPANET)

IF(APPLE)
  SET(CMAKE_INSTALL_NAME_DIR @executable_path)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
ENDIF(APPLE)

IF(WIN32)
  add_definitions(-DWIN32)
ELSE(WIN32)
  add_definitions(-DLINUX)
ENDIF(WIN32)


# the library
# include_directories(../../include)
file(GLOB IBANEZ_EPANET2_SOURCES *.c)
add_library(Ibanez_epanet2 SHARED  ${IBANEZ_EPANET2_SOURCES})


