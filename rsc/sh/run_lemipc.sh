#!/bin/bash

source ./rsc/sh/color.sh
LEMIPC="./lemipc"
OUT_VALGRIND_DISPLAY="valgrind_display.log"
VALGRIND_DISPLAY="valgrind --leak-check=full --track-fds=yes --log-file=${OUT_VALGRIND_DISPLAY}"
LEMIPC_DISPLAY="./lemipc_display"
SLEEP_VAL=0.001

VALGRIND="valgrind --leak-check=full --track-fds=yes"



handle_opt() {

	if [ "${1}" == "1" ]; then
		display_color_msg ${YELLOW} "Valgrind option enabled."
		LEMIPC="${VALGRIND} ${LEMIPC}"
		echo "" > ${OUT_VALGRIND_DISPLAY}
		LEMIPC_DISPLAY="${VALGRIND_DISPLAY} ${LEMIPC_DISPLAY}"
		SLEEP_VAL=0.1
	else
		display_color_msg ${YELLOW} "Valgrind option disabled."
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

	display_color_msg ${YELLOW} "Lauch display handler ${LEMIPC_DISPLAY} ..."
	${LEMIPC_DISPLAY} &
	DISPLAY_PID=$!
	sleep 2

	for ((i=0; i<${nb_player}; i++));
	do
		local team_id=$(((i % ${nb_team}) + 1))
		display_color_msg ${GREEN} "Lauching number ${i} team ${team_id} ..."
		# (${LEMIPC} ${team_id} &) >> logger.txt
		${LEMIPC} ${team_id} &
	done

	# wait all children pid
	display_color_msg ${GREEN} "Waiting all children to finish ..."
	wait $(jobs -p)
	display_valgrind_log
}

if [ $# -lt 2 ]; then
	NB_PLAYER=50
	NB_TEAM=4
else 
	NB_PLAYER=${1}
	NB_TEAM=${2}
fi

handle_opt ${3}

run_test ${NB_PLAYER} ${NB_TEAM}
# run_test 10 4
# run_test 200 4
# run_test 500 4
# run_test 1000 4
# run_test 2000 4
# run_test 6000 4
# run_test 6000 10
