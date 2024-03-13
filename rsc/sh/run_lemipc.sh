#!/bin/bash

source ./rsc/sh/color.sh

# make -s

LEMIPC="./lemipc"

send_sigint() {
	PID=$1
	# display_color_msg ${YELLOW} "Sending SIGINT to pid: ${PID}"
	kill -2 ${PID}
}

sigint_test() {
	display_color_msg ${LIGHT_BLUE} "Running lemipc basic test ..."
	
	${LEMIPC} 1 &
	pid1=$!
	sleep 1

	${LEMIPC} 8 &
	pid2=$!
	sleep 1

	${LEMIPC} 19 &
	pid3=$!
	sleep 1

	${LEMIPC} 88 &
	pid4=$!
	sleep 1

	${LEMIPC} 42 &
	pid5=$!
	
	display_color_msg ${YELLOW} "Lauch display handler ..."
	./lemipc_display &
	pid6=$!
	
	display_color_msg ${YELLOW} "Let runing for 5 seconds ..."
	sleep 5
	

	send_sigint ${pid6}
	send_sigint ${pid1}
	send_sigint ${pid5}
	send_sigint ${pid2}
	send_sigint ${pid3}
	sleep 1
	send_sigint ${pid4}

	# send_sigint ${pid6}
	
	# display_color_msg ${YELLOW} "Waiting for process to finish ..."
	wait ${pid1} && wait ${pid5} && wait ${pid2} && wait ${pid3} && wait ${pid4} && wait ${pid6}
	# display_color_msg ${YELLOW} "Waiting protect ..."
	sleep 1
	display_color_msg ${YELLOW} "Waiting protect finish ..."
	./rsc/sh/check_ipcs_free.sh
}

sigint_test