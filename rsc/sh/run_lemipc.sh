#!/bin/bash

source ./rsc/sh/color.sh

make -s

display_color_msg ${LIGHT_BLUE} "Running lemipc test ..."

lemipc="./lemipc"

for i in {1..40} ; do
    if [ $((i % 2)) -eq 0 ]; then
        ${lemipc} ${i} &
    else
        DOUBLE=$((i * 2))
        ${lemipc} ${DOUBLE} &
    fi
done

${lemipc} 99 &
wait $! && ./rsc/sh/check_ipcs_free.sh