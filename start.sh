#!/bin/sh
g++ main.cpp -o webServer
port=`netstat -ntulp | grep 80 | awk '{printf($7)}'`
kill -9 ${port%/./webServer}
g++ main.cpp
./webServer
