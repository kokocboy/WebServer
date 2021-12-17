#!/bin/sh
kill -9 `ps aux | grep './webServer' | grep -v S+ | awk '{printf($2)}'`
g++ main.cpp -o webServer -pthread
./webServer