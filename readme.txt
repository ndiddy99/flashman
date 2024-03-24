Flashman only supports Unix-like systems (Linux, OS X, maybe MinGW) right now, and requires hidapi-hidraw to function (libhidapi-dev package on Debian).
You can make it work on Windows by adding mingw stuff to CMakeLists.txt and building hidapi yourself (I may do this in the future, but if you want it added sooner than "when I get around to it", do it yourself and submit a PR).

To build:
1. cmake -B build
2. cmake --build build

Note: You may either need to run flashman as root or set up a udev rule to get the connection to the cartridge to work correctly.

