#!/bin/bash
for i in $(seq start_line end_line)
do
    echo "break $i"
done > breakpoints.gdb