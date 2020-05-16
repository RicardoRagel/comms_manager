# comms-manager
A simple C++ Inter-Process Communication (IPC) Manager using Shared Memory for Linux.

# Overview
Although there are already several good IPC libraries integrated in different frameworks ([ROS](https://www.ros.org/), [Google Protobuf](https://developers.google.com/protocol-buffers), [Microsoft IPC](https://github.com/microsoft/IPC), ...), The idea of this repository is to provide a simple, easy and framework-independent way to communicate two processes using shared memory.

This package provides:

1. A really simple library class **CommsManager** to create your channel messages, publisher and subscribers.
2. A **logger** program to save the current data of the channels that you select in binary data files.
3. A **player** program to reproduce the previous saved binary data files.

# Dependencies
This library is provided with a CMakeLists compiler file, so if you are going install it from source you need:
* [CMake](https://cmake.org/)

# Installation

### Debian package
This library and its tools can be installed using directly the provided debian file (check Releases page). We recommend install it using the APT command:
```bash
sudo apt install ./comms_manager-<version>-Linux.deb
```
Then if you need to remove it later, you can do it easily using the command: `sudo apt remove comms_manager`.

In case you are not able to use APT, use *dpkg* instead:
```bash
sudo dpkg -i comms_manager-<version>-Linux.deb
```

Once you have installed it, continue to the *Usage* and *Examples* sections.

### From Source
Compile the CommsManager library and tools as usual:
```bash
mkdir build && cd build
cmake ..
make
```
And install it:
```bash
sudo make install
```
The default installation path of the library is: `/usr/local/lib`.

# Usage
An example about how to link and use this library is provided in the [example](example/) folder. 

## Linking this library from your project
Simply, add this lines to your CMakeLists.txt:
```cmake
# Link for the shared library:
set(PROJECT_LINK_LIBS libCommsManager.so)
link_directories( ${CMAKE_INSTALL_PREFIX} ) # That is /usr/local

# Create executables
add_executable(your_app your_app.cpp)
target_link_libraries(your_app ${PROJECT_LINK_LIBS} )
```

## Defining your messages

First, you need to define the message data as a struct:  

```cpp
struct ExampleData
{
	struct  timespec timestamp; // Real Time
	int 	i;                  // Example integer data 
};
```

Check the exmaple: [example_data.h](example/msg/example_data.h).

Then, you can create the message as a CommsManager child class that contains the previous data: 
```cpp
#include "CommsManager.hpp"
#include "example_data.h"

class ExampleMsg : public CommsManager 
{
  public:

	ExampleMsg(const char *channel_name) : CommsManager(channel_name, &data, sizeof(ExampleData))
	{
		// Initial time
		clock_gettime(CLOCK_REALTIME, &data.timestamp);

		// Default values
		i = -1;
	}
																	
    ExampleData data;
};
```

Check example message [example_msg.h](example/msg/example_msg.h).

## Creating a publisher

To publish the previous messages into a comms channel:

```cpp
// Example Data Message
#include "msg/example_msg.h"

...

// Create the channel
ExampleMsg msg("/channel_name");

// Fill the message
clock_gettime(CLOCK_REALTIME, &msg.data.timestamp);
msg.data.i = 33;

// Pusblish it
msg.write();
```

Check the complete example in [example_pub.cpp](example/example_pub.cpp).

## Creating a subscriber

To subscribe to the previous publisher channel and read the messages, there are two options: blocking or non-blocking readings:

```cpp
// Example Data Message
#include "msg/example_msg.h"

...

// Create the channel
ExampleMsg msg("/channel_name");

// Read message from the channel
bool blocking = <true or false>;
bool new_msg_exist = msg.read(blocking);

if(new_msg_exist)
{
    printf("New message: i= %d\n", msg.data.i);

    <do something>
}
else
{
    printf("No new messages...\n");
}

```

Check the complete example in [example_sub.cpp](example/example_sub.cpp).

## Logging channels
To save the data through one or several channels, you can easily do it using the installed *comms_logger* tool:

```bash
comms_logger /channel_name_1 /channel_name_2 ...
```

This will save the data in *.log* binary file called as the channel_name (plus the timestamp) inside a hidden folder, called *.comms_log_files*, at your home folder.

## Playing back the saved channels and data
To play these previous log files, this package also provides the *comms_player* tool. Execute it together the name (without extension) of the log files you wish to reproduce:

```bash
comms_player _channel_name_1_log_file _channel_name_2_log_file ...
```

# Examples
To test the provided examples, first go to the [example](example/) folder and compile them:
```bash
cd example
mkdir build && cd build
cmake ..
make
```

### Running the examples:

Once you have compiled and installed the library and compiled the example, just run the publisher example in one terminal:

```bash
./example_pub
```

and the subrscriber example in another terminal choosing the blocking option:

```bash
./example_sub true
```