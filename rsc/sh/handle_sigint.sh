#!/bin/bash

source ./rsc/sh/color.sh

# PID_LOG="pid.txt"

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
	PID_FOUND=$(ps | grep lemipc | cut -d ' ' -f 1)
	for pid in ${PID_FOUND}
	do
		send_sigint ${pid}
		sleep 0.1
	done
	PID_FOUND=$(ps | grep lemipc | cut -d ' ' -f 1)
	if [ "${PID_FOUND}" == "" ]; then
		display_color_msg ${GREEN} "No pid found, all lemipc instance clean up"
	else
		display_color_msg ${RED} "Pid found: ${PID_FOUND}"
	fi
}