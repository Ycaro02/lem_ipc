#!/bin/bash

source $PWD/rsc/sh/color.sh

check_ipcs_free() {
	IPCS_OPT="$1"
	MSG="$2"
	local ipcs_free=$(ipcs ${IPCS_OPT} | grep -e "0x" | wc -l)
	if [ $ipcs_free -ne 0 ]; then
		display_color_msg ${RED} "${MSG} leak found."
	else
		display_color_msg ${GREEN} "No ${MSG} leak."
	fi
}

check_ipcs_free -m "Shared memory"
check_ipcs_free -s "Semaphore"
