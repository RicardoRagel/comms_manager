cmake_minimum_required(VERSION 2.8.9)
project(comms_manager)
set(CMAKE_BUILD_TYPE Release)

#########
# BUILD
#########

# Include headers directory
include_directories(include 
					include/logger 
					include/player)

# Add lib source files to variables
set(COMM_MANAGER_LIB src/CommsManager.cpp)
set(LOGGER_LIB src/logger/CommsLogger.cpp)
set(PLAYER_LIB src/player/CommsPlayer.cpp)

# Generate the shared library from the sources
add_library(CommsManager SHARED ${COMM_MANAGER_LIB})
add_library(CommsLogger  SHARED ${LOGGER_LIB})
target_link_libraries(CommsLogger CommsManager -pthread)
add_library(CommsPlayer  SHARED ${PLAYER_LIB})
target_link_libraries(CommsPlayer CommsManager -pthread)

# Generate logger executable
add_executable(comms_logger src/logger/main.cpp)
target_link_libraries(comms_logger CommsLogger)

# Generate player executable
add_executable(comms_player src/player/main.cpp)
target_link_libraries(comms_player CommsPlayer)

#########
# INSTALL
#########

# Install the library and the headers
# Use "sudo make install" to apply

# Define installation folder paths: /usr/local/lib;include;bin
include(GNUInstallDirs)
set(INSTALL_LIB_PATH 	${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(INSTALL_HEADER_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(INSTALL_BIN_PATH 	${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})

# CommsManager library
install(TARGETS CommsManager 
		DESTINATION ${INSTALL_LIB_PATH})
install(FILES include/CommsManager.hpp include/SharedMemory.hpp 
		DESTINATION ${INSTALL_HEADER_PATH})
# CommsLogger library
install(TARGETS CommsLogger 
		DESTINATION ${INSTALL_LIB_PATH}) 
install(FILES include/logger/CommsLogger.hpp 
		DESTINATION ${INSTALL_HEADER_PATH})
# CommsPlayer library
install(TARGETS CommsPlayer 
		DESTINATION ${INSTALL_LIB_PATH}) 
install(FILES include/player/CommsPlayer.hpp 
		DESTINATION ${INSTALL_HEADER_PATH})
# comms_logger executable
install(TARGETS comms_logger 
		DESTINATION ${INSTALL_BIN_PATH})
# comms_player executable
install(TARGETS comms_player 
		DESTINATION ${INSTALL_BIN_PATH})

#########
# PACK (DEBIAN)
#########
# WARNING: Previous INSTALL must be done

# Debian package generation
# Use "sudo cpack" to apply

set(LIB_VERSION_MAJOR 1)
set(LIB_VERSION_MINOR 1)
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