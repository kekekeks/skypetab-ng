SkypeTab is a program that adds tabs to Skype™ for Linux.

[Project page](http://keks-n.net/skypetab) (description, demo, repositories, etc).

Compilation & Installation
==========================

First you need environment that's able to compile 32-bit code. You also need libX11 and Qt and headers for them.

Run qmake -spec linux-g++-32 && make

Then you will be able to start SkypeTab using LD_PRELOAD=\`pwd\`/libskypetab.so skype

Note, LD_PRELOAD requires full path to the library


Coding "guidelines"
==========================
If you want to participate, please, use tabs for indention and spaces for formatting. Thank you.