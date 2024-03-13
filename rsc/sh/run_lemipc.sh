#!/bin/bash



source ./rsc/sh/handle_sigint.sh

# make -s



sigint_loop_test() {
	
	./rsc/mk/ascii.sh "tester"
	rm_pid_log ${PID_LOG}

	for i in {1..12}
	do
		${LEMIPC} ${i} &
		local loc_pid=$!
		echo ${loc_pid} >> ${PID_LOG}
		sleep 0.5
	done
	
	display_color_msg ${YELLOW} "Lauch display handler ..."
	./lemipc_display &
	local display_pid=$!
	display_color_msg ${YELLOW} "Let runing for 10 seconds ..."
	sleep 10

	local all_pid=$(cat ${PID_LOG} | tr '\n' ' ') 
	# send_sigint ${display_pid}
	# sleep 1
	send_sigint_all
	wait ${all_pid}
	sleep 1
	display_color_msg ${YELLOW} "Waiting protect finish ..."
	./rsc/sh/check_ipcs_free.sh
	rm_pid_log ${PID_LOG}
}



sigint_loop_test