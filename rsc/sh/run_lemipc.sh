#!/bin/bash

source ./rsc/sh/color.sh
LEMIPC="./lemipc"

if [ ! -f "${LEMIPC}" ]; then
	make -s
fi

sigint_loop_test() {
	
	./rsc/mk/ascii.sh "tester"
	# rm_pid_log ${PID_LOG}

	for i in {1..200}
	do
		local team_id=$(((i % 8) + 1))

		if [ $i -eq 10 ]
		then
			display_color_msg ${YELLOW} "Lauch display handler ..."
			./lemipc_display &
			sleep 1
		fi
		display_color_msg ${GREEN} "Lauching number ${i} team ${team_id} ..."
		${LEMIPC} ${team_id} &
		sleep 0.01
	done
	
	display_color_msg ${YELLOW} "Let runing make clear for clean ..."
}

sigint_loop_test