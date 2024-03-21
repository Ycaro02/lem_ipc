#!/bin/bash

source ./rsc/sh/color.sh
LEMIPC="./lemipc"

if [ ! -f "${LEMIPC}" ]; then
	make -s
fi

sigint_loop_test() {
	
	./rsc/mk/ascii.sh "tester"
	# rm_pid_log ${PID_LOG}

	for i in {0..200}
	do
		local team_id=$(((i % 200) + 1))
		display_color_msg ${GREEN} "Lauching number ${i} team ${team_id} ..."
		${LEMIPC} ${team_id} &
		sleep 0.02
	done

	display_color_msg ${YELLOW} "Lauch display handler ..."
	sleep 0.05
	./lemipc_display &

	# display_color_msg ${RED} "Killing all processes ..."
	# ./rsc/sh/check_ipcs_free.sh
}

sigint_loop_test