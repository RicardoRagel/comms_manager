cmake_minimum_required(VERSION 2.8.9)
project(comms_manager)
set(CMAKE_BUILD_TYPE Release)

#########
# BUILD
#########

# Include headers directory
include_directories(include)

# Add source files to a variable
set(SOURCES src/CommsManager.cpp)

# Generate the shared library from the sources
add_library(CommsManager SHARED ${SOURCES})

#########
# INSTALL
#########

# Install the library and the headers
# Use "sudo make install" to apply

include(GNUInstallDirs)
install(TARGETS CommsManager DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}) # That is /usr/local/lib
install(FILES include/CommsManager.hpp include/SharedMemory.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}) # That is /usr/local/include

#########
# PACK
#########
# WARNING: Previous INSTALL must be done

# Debian package generation
# Use "sudo cpack" to apply

set(LIB_VERSION_MAJOR 1)
set(LIB_VERSION_MINOR 0)
set(LIB_VERSION_PATCH 0)
set(LIB_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${LIB_VERSION})
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "RicardoRagel")
set(CPACK_PACKAGE_CONTACT "Ricardo Ragel de la Torre <ricardoragel4@gmail.com>")
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/deb) 		# Directory where place the debian.
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}) # Directory where this debians have to install the packaged targets,
															# Setting to CMAKE_INSTALL_PREFIX, then it will be the same used in the
															# install( ... DESTINATION <>), that in this case is: "/usr/local".
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "IPC manager library written in C++")
set(CPACK_GENERATOR "DEB")

include (CPack)