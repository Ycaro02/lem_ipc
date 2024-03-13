#!/bin/bash

SOLO_LOG="solo_pid.txt"

source ./rsc/sh/handle_sigint.sh


display_test() {
	${LEMIPC} 1 &
	local loc_pid=$!
	echo ${loc_pid} > ${SOLO_LOG}
	sleep 0.5
	display_color_msg ${YELLOW} "Lauch display handler ..."
	./lemipc_display &
}

rm_solo_log() {
	if [ -f "${SOLO_LOG}" ]; then
		local pid=$(cat ${SOLO_LOG})
		if [ "${pid}" != "" ]; then
			send_sigint ${pid}
			rm  ${SOLO_LOG}
		fi
	fi
}

if [ "$1" == "rm" ]; then
	rm_solo_log
else
	source ./rsc/sh/color.sh
	rm_solo_log
	sleep 1
	display_test
fi