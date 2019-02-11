#!/usr/bin/env bash
# Compile the files.
g++ ./fs_directio.cpp -o fs_directio
g++ ./fs_default.cpp -o fs_default
g++ -pthread ./cc_mutex.cpp -o cc_mutex
g++ -pthread ./cc_spinlock.cpp -o cc_spinlock
