#!/bin/bash

source $PWD/rsc/sh/color.sh

check_ipcs_free() {
	local ipcs_free=$(ipcs -m | grep -e "0x" | wc -l)
	if [ $ipcs_free -ne 0 ]; then
		display_color_msg ${RED} "Shared memory segment leak found."
	else
		display_color_msg ${GREEN} "No Shared memory segment leak."
	fi
}

check_ipcs_free
