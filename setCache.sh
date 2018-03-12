#!/bin/bash
sudo hdparm -W 0 /dev/sdd1
./bench
sudo hdparm -W 1 /dev/sdd1

