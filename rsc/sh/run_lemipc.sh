#!/bin/bash

source ./rsc/sh/color.sh

make -s

display_color_msg ${LIGHT_BLUE} "Running lemipc test ..."

LEMIPC="./lemipc"

for i in {1..40} ; do
    if [ $((i % 2)) -eq 0 ]; then
        ${LEMIPC} ${i} &
    else
        DOUBLE=$((i * 2))
        ${LEMIPC} ${DOUBLE} &
    fi
done

${LEMIPC} 99 &
wait $! && sleep 1 && ./rsc/sh/check_ipcs_free.sh