#!/bin/bash

lemipc="./lemipc"

for i in {1..40} ; do
    if [ $((i % 2)) -eq 0 ]; then
        ${lemipc} ${i} &
    else
        DOUBLE=$((i * 2))
        ${lemipc} ${DOUBLE} &
    fi
done