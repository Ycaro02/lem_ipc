#!/bin/bash

source ./rsc/sh/color.sh
LEMIPC="./lemipc"
OUT_VALGRIND_DISPLAY="valgrind_display.log"
VALGRIND_DISPLAY="valgrind --leak-check=full --track-fds=yes --log-file=${OUT_VALGRIND_DISPLAY}"
LEMIPC_DISPLAY="./lemipc_display"
SLEEP_VAL=0.001

VALGRIND="valgrind --leak-check=full --track-fds=yes"



handle_opt() {
	if [ -z "${1}" ]; then
		display_color_msg ${RED} "Usage: ${0} <VALGRIND_BOOLEAN_OPT>"
		exit 1
	fi

	if [ "${1}" == "1" ]; then
		LEMIPC="${VALGRIND} ${LEMIPC}"
		LEMIPC_DISPLAY="${VALGRIND_DISPLAY} ${LEMIPC_DISPLAY}"
		SLEEP_VAL=0.1
	fi


	if [ ! -f "${LEMIPC}" ]; then
		make -s
	fi
}

display_valgrind_log() {
	if [ -f "${OUT_VALGRIND_DISPLAY}" ]; then
		display_color_msg ${GREEN} "Valgrind log found."
		cat ${OUT_VALGRIND_DISPLAY}
		rm ${OUT_VALGRIND_DISPLAY}
	fi
}


run_test() {
	local nb_player=${1}
	local nb_team=${2}
	./rsc/mk/ascii.sh "tester"
	# rm_pid_log ${PID_LOG}

	for ((i=0; i<${nb_player}; i++));
	do
		local team_id=$(((i % ${nb_team}) + 1))
		display_color_msg ${GREEN} "Lauching number ${i} team ${team_id} ..."
		if [ ${i} -eq 3 ]; then
			display_color_msg ${YELLOW} "Lauch display handler ${LEMIPC_DISPLAY} ..."
			${LEMIPC_DISPLAY} &
			DISPLAY_PID=$!
		fi
		${LEMIPC} ${team_id} &
		#sleep ${SLEEP_VAL}
	done

	# wait all children pid
	display_color_msg ${GREEN} "Waiting all children to finish ..."
	wait $(jobs -p)
	display_valgrind_log
	# display_color_msg ${RED} "Killing all processes ..."
	# ./rsc/sh/check_ipcs_free.sh
}

handle_opt "$@"
# run_test 10 4
# run_test 200 4
# run_test 500 4
# run_test 1000 4
run_test 2000 4
# run_test 5000 26
