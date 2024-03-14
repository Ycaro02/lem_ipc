#!/bin/bash

source ./rsc/sh/color.sh

PID_LOG="pid.txt"
LEMIPC="./lemipc"

rm_pid_log() {
	if [ -f "${1}" ]; then
		rm "${1}"
	fi
}

send_sigint() {
	PID=$1
	# display_color_msg ${YELLOW} "Sending SIGINT to pid: ${PID}"
	kill -2 ${PID} 2> /dev/null
}

send_sigint_all() {
	display_color_msg ${YELLOW} "Sending SIGINT to all pid ..."
	for pid in $(cat ${PID_LOG})
	do
		send_sigint ${pid}
		sleep 0.1
	done
}

