#!/bin/bash
 printf '\001'"Z""00"'\002'"E"" "$1'\004' | tee -a /dev/ttyUSB0
