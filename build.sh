#!/bin/sh

g++ -O3 -funroll-loops -finline-functions follow_addr.cpp -o follow_addr
g++ -O3 -funroll-loops -finline-functions follow_closest.cpp -o follow_closest
