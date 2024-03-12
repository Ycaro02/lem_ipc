#!/bin/bash

source ./rsc/sh/color.sh

make -s

LEMIPC="./lemipc"

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

run_test