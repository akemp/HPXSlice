# Require a recent version of cmake
cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

# This project is C++ based.
project(HPXSlice CXX)

# This adds the HPX cmake configuration directory to the search path.
set(CMAKE_MODULE_PATH
    $ENV{HPX_DIR}/share/cmake-2.8/Modules)

# Instruct cmake to find the HPX settings
find_package(HPX)

include_directories(${HPX_INCLUDE_DIR})
link_directories(${HPX_LIBRARY_DIR})

include (HPX_INCLUDE)

	
set (sources 
	slice/contour.cpp
	slice/datastructures.hpp
	slice/gzip.cpp
	slice/io.cpp
	slice/retLine.cpp
	slice/treeHeaders.cpp
	slice/treeHeaders.hpp
	slice/zlib.cpp
	)
# add example executable
add_hpx_executable(slice
             SOURCES ${sources}
             ${PLAYGROUND_FLAGS} 
             FOLDER 
             COMPONENT_DEPENDENCIES ${hpx_RUNTIME_LIBRARIES})
