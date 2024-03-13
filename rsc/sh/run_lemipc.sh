#!/bin/bash

source ./rsc/sh/color.sh

# make -s

./rsc/mk/ascii.sh "tester"

LEMIPC="./lemipc"
PID_LOG="pid.txt"

rm_pid_log() {
	if [ -f ${PID_LOG} ]; then
		rm ${PID_LOG}
	fi
}

send_sigint() {
	PID=$1
	# display_color_msg ${YELLOW} "Sending SIGINT to pid: ${PID}"
	kill -2 ${PID}
}

send_sigint_all() {
	display_color_msg ${YELLOW} "Sending SIGINT to all pid ..."
	for pid in $(cat ${PID_LOG})
	do
		send_sigint ${pid}
		sleep 0.1
	done
}

sigint_loop_test() {

	rm_pid_log

	for i in {1..10}
	do
		${LEMIPC} ${i} &
		local loc_pid=$!
		echo ${loc_pid} >> ${PID_LOG}
		sleep 0.5
	done
	
	display_color_msg ${YELLOW} "Lauch display handler ..."
	./lemipc_display &
	local display_pid=$!
	display_color_msg ${YELLOW} "Let runing for 5 seconds ..."
	sleep 5

	local all_pid=$(cat ${PID_LOG} | tr '\n' ' ') 
	send_sigint ${display_pid}
	sleep 1
	send_sigint_all
	wait ${all_pid}
	sleep 1
	display_color_msg ${YELLOW} "Waiting protect finish ..."
	./rsc/sh/check_ipcs_free.sh
	rm_pid_log
}

sigint_loop_test