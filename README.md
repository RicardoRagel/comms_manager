# comms-manager
A simple C++ Inter-Process Communication (IPC) Manager using Shared Memory for Linux.

# Overview
Although there are already several good IPC libraries integrated in different frameworks ([ROS](https://www.ros.org/), )

# Dependencies
This library is provided with a CMakeLists compiler file, so to use it you will need:
* [CMake](https://cmake.org/)

# Installation

## CommsManager Library
Compile the CommsManager library as usual:
```bash
mkdir build && cd build
cmake ..
make
```
And install it:
```bash
sudo make install
```
The current installation path is: `/usr/local/lib`.

## Examples
To test the provided examples, just go to the [example](example/) folder and compile them:
```bash
cd example
mkdir build && cd build
cmake ..
make
```

# Usage
An example about how to link to and to use this library is provided in the [example](example/) folder.

## Linking this library from your project
Simply, add this lines to your CMakeLists.txt:
```cmake
set ( PROJECT_LINK_LIBS libCommsManager.so)
link_directories( /usr/local/lib )
```

## Defining your messages

First, you need to define the message data as a struct:  [example_data.h](example/msg/example_data.h).

Then, you can create the message as a CommsManager child class that contains the previous data: [example_msg.h](example/msg/example_msg.h).

## Creating a publisher

To publish the previous messages into a comms channel:

```cpp
// Example Data Message
#include "msg/example_msg.h"

...

// Create the channel
ExampleMsg msg("/channel");

// Fill the message
msg.data.V[0] = 33;
msg.data.flag = true;

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
ExampleMsg msg("/channel");

// Read message from the channel
bool blocking = <true or false>;
bool new_msg_exist = msg.read(blocking);

if(new_msg_exist)
{
    printf("New message: V(0)= %.2f, flag=%s\n", msg.data.V[0], msg.data.flag == false?"false":"true");

    <do something>
}
else
{
    printf("No new messages...\n");
}

```

Check the complete example in [example_sub.cpp](example/example_sub.cpp).


## Running the examples:

Once you have compiled and installed the library and compiled the example, just run the publisher in one terminal:

```bash
./example_pub
```

and the subrscriber in another terminal choosing the blocking option:

```bash
./example_sub true
```