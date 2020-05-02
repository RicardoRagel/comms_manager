cmake_minimum_required(VERSION 2.8.9)
project(comms_manager)
set(CMAKE_BUILD_TYPE Release)

# Include headers directory
include_directories(include)

# Add source files to a variable
set(SOURCES src/CommsManager.cpp)

# Generate the shared library from the sources
add_library(CommsManager SHARED ${SOURCES})

# Install the library and the headers
# Use "sudo make install" to apply
install(TARGETS CommsManager DESTINATION /usr/local/lib)
install(FILES include/CommsManager.hpp include/SharedMemory.hpp DESTINATION /usr/local/include)