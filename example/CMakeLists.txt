cmake_minimum_required(VERSION 2.8.9)
project(comms_manager_example)

#########
# BUILD
#########

# Link for the shared library:
set(PROJECT_LINK_LIBS libCommsManager.so)
link_directories( ${CMAKE_INSTALL_PREFIX} ) # That is /usr/local

# Include header of the shared library
#include_directories(/usr/local/include)

# Create executables
add_executable(example_pub example_pub.cpp)
target_link_libraries(example_pub ${PROJECT_LINK_LIBS} )
add_executable(example_sub example_sub.cpp)
target_link_libraries(example_sub ${PROJECT_LINK_LIBS} )

#########
# INSTALL
#########

# Install the executable to be executable from everywhere
# Use "sudo make install" to apply
install(TARGETS example_pub DESTINATION /usr/local/bin)

# NOTE: To execute this app from everywhere, apart of this last 
# instructions you have to add in the .bashrc: 
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib