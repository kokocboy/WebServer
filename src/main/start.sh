#!/bin/sh
g++ main.cpp -o webServer
kill -9 `ps aux | grep './webServer' | grep -v S+ | awk '{printf($2)}'`
g++ main.cpp