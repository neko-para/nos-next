#!/bin/bash

DEBUG_FLAG=""

if [[ "$1" == "d" ]]; then
    DEBUG_FLAG="-s -S"
fi

qemu-system-i386 -kernel build/kernel/kernel -debugcon stdio $DEBUG_FLAG
