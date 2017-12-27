#!/bin/bash
if [ "$#" -ne 4 ]; then
    echo "usage: $0 <IP src> <IP dst> <PORT dst> <secret>"
    exit 1
fi
python gen_hmac.py "$4" "$1" | socat - "udp-sendto:$2:$3,bind=$1"
