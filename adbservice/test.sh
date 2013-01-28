#!/bin/bash
sudo rm /usr/lib/libadbservice.so 
make clean
make
#sudo cp libadbservice.so /usr/lib/
./test_so
