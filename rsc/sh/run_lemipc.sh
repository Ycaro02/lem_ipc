#!/bin/bash

source ./rsc/sh/color.sh
LEMIPC="./lemipc"

if [ ! -f "${LEMIPC}" ]; then
	make -s
fi

sigint_loop_test() {
	
	./rsc/mk/ascii.sh "tester"
	# rm_pid_log ${PID_LOG}

	for i in {1..10}
	do
		local team_id=$(((i % 2) + 1))
		display_color_msg ${GREEN} "Lauching number ${i} team ${team_id} ..."
		${LEMIPC} ${team_id} &
		sleep 0.01
	done

	display_color_msg ${YELLOW} "Lauch display handler ..."
	sleep 5
	./lemipc_display &
	
	display_color_msg ${YELLOW} "Let runing make clear for clean ..."
}

sigint_loop_test