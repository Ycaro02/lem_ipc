#!/bin/bash

source ./rsc/sh/color.sh

# make -s

./rsc/mk/ascii.sh "tester"

LEMIPC="./lemipc"

send_sigint() {
	PID=$1
	# display_color_msg ${YELLOW} "Sending SIGINT to pid: ${PID}"
	kill -2 ${PID}
}

send_sigint_all() {
	display_color_msg ${YELLOW} "Sending SIGINT to all pid ..."
	for pid in $(cat pid.txt)
	do
		send_sigint ${pid}
		sleep 0.1
	done
	
	if [ -f "pid.txt" ]; then
		rm pid.txt
	fi
}

sigint_loop_test() {
	for i in {1..10}
	do
		${LEMIPC} ${i} &
		local loc_pid=$!
		echo ${loc_pid} >> pid.txt
		sleep 0.5
	done

	
	display_color_msg ${YELLOW} "Lauch display handler ..."
	./lemipc_display &
	local display_pid=$!
	display_color_msg ${YELLOW} "Let runing for 5 seconds ..."
	sleep 5

	local all_pid=$(cat pid.txt | tr '\n' ' ') 

	send_sigint ${display_pid}
	sleep 1
	send_sigint_all

	wait ${all_pid}
	sleep 1
	display_color_msg ${YELLOW} "Waiting protect finish ..."
	./rsc/sh/check_ipcs_free.sh
}

sigint_loop_test