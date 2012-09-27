#!/bin/sh
g++ -Wall \
	-I/Users/jelmer/workspace/rug/compling/dbxml-2.5.16/install/include \
	-I/usr/include/libxml2 \
	-L/Users/jelmer/workspace/rug/compling/dbxml-2.5.16/install/lib \
	-lxqilla \
	-lxerces-c \
	-lxml2 \
	-o ./queryvalidator \
	./main.cpp