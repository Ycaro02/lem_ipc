#!/bin/bash

source ./rsc/sh/color.sh

make -s

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
	
	send_sigint ${pid1}
	send_sigint ${pid5}
	send_sigint ${pid2}
	send_sigint ${pid3}
	send_sigint ${pid4}

	send_sigint ${pid6}
	
	# display_color_msg ${YELLOW} "Waiting for process to finish ..."
	wait ${pid1} && wait ${pid5} && wait ${pid2} && wait ${pid3} && wait ${pid4} && wait ${pid6}
	# display_color_msg ${YELLOW} "Waiting protect ..."
	sleep 5
	display_color_msg ${YELLOW} "Waiting protect finish ..."
	./rsc/sh/check_ipcs_free.sh
}

helgrind_test() {
	display_color_msg ${LIGHT_BLUE} "Running lemipc hellgrind test ..."

	HELGRIND="valgrind --tool=helgrind --history-level=full"
	VAL_OUT="valgrind_out"

	for i in {1..40} ; do
		${HELGRIND} --log-file=${VAL_OUT}${i} ${LEMIPC} ${i} &
		sleep 1
	done


	LAST_PID=$!
	display_color_msg ${YELLOW} "Helgrind test last pid: ${LAST_PID}"
	wait ${LAST_PID} && sleep 1 && ./rsc/sh/check_ipcs_free.sh
}

run_test() {

	display_color_msg ${LIGHT_BLUE} "Running lemipc test ..."

	for i in {1..40} ; do
		if [ $((i % 2)) -eq 0 ]; then
			${LEMIPC} ${i} &
		else
			DOUBLE=$((i * 2))
			${LEMIPC} ${DOUBLE} &
		fi
		if [ $i -eq 1 ]; then
			display_color_msg ${YELLOW} "First pid wait data creation: $!"
		fi
	done

	${LEMIPC} 99 &
	LAST_PID=$!
	display_color_msg ${YELLOW} "Last pid: ${LAST_PID}"
	wait ${LAST_PID} && sleep 5 && ./rsc/sh/check_ipcs_free.sh
}

sigint_test