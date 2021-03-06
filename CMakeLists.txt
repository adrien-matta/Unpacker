cmake_minimum_required (VERSION 2.6)
project (Unpacker++)
 include("FindROOT.cmake") 
 
# Set usefull variables
set(UNPACKER "$ENV{UNPACKER}")
set(UNPACKER_LIB_DIR "${UNPACKER}/lib")
set(NPTOOL "$ENV{NPTOOL}")
set(NPLIB "${NPTOOL}/NPLib")
set(NPTOOL_INCLUDE_DIR "${NPLIB}/include")
set(NPTOOL_LIB_DIR "${NPLIB}/lib")

# The version number.
set (UP_VEUPION_MAJOR 1)
set (UP_VEUPION_MINOR 0)

set(CMAKE_BUILD_TYPE Release)

# If the compiler is Clang, silence the unrecognised flags
if(${CMAKE_CXX_COMPILER_ID} MATCHES ".*Clang.*")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set(CMAKE_INCLUDE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/include )

#link_directories( ${LINK_DIRECTORIES})
# add the binary tree to the search path for include files
# so that we will find UPConfig.h
#include_directories(${CMAKE_INCLUDE_OUTPUT_DIRECTORY})

set(INCLUDE_DIRECTORIES ${ROOT_INCLUDE_DIR} )
include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR})
link_directories( ${LINK_DIRECTORIES})

# Finding NPLib
find_package(NPLib)
include(${NPLib_USE_FILE})

# add lib
set(UP_SUB_DIRECTORIES UpCore)

### for Darwin
set(CMAKE_MACOSX_RPATH 1)
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH ${UNPACKER_LIB_DIR})
# Set the Search path for library
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# set Unpacker/lib as one of the link directory
link_directories(${UNPACKER_LIB_DIR})
# set NPLib/lib as one of the link directory
link_directories(${NPTOOL_LIB_DIR})

# Macro that add the sub dir and the include file install path
foreach(subdir ${UP_SUB_DIRECTORIES})
  add_subdirectory (${subdir})  
  include_directories(${subdir})
file(GLOB incfiles "${subdir}/*.h")
  foreach(incfile ${incfiles})
  install (FILES ${incfile}
           DESTINATION ${CMAKE_BINARY_DIR}/include)
  endforeach(incfile)
endforeach(subdir)

add_executable(unpacker Unpacker.cxx)   
target_link_libraries(unpacker ${ROOT_LIBRARIES} UnpackerCore)
