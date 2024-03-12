#!/bin/bash

source ./rsc/sh/color.sh

make -s

LEMIPC="./lemipc"

send_SIGINT() {
	PID=$1
	display_color_msg ${YELLOW} "Sending SIGINT to pid: ${PID}"
	kill -2 ${PID}
}

basic_test() {
	display_color_msg ${LIGHT_BLUE} "Running lemipc basic test ..."
	${LEMIPC} 1 &
	first_pid=$!
	${LEMIPC} 19 &
	second_pid=$!
	display_color_msg ${YELLOW} "Let runing for 10 seconds ..."
	sleep 10
	send_SIGINT ${first_pid}
	send_SIGINT ${second_pid}
	display_color_msg ${YELLOW} "Waiting for process to finish ..."
	wait ${first_pid} && wait ${second_pid}
	display_color_msg ${YELLOW} "Waiting protect ..."
	sleep 10
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

basic_test