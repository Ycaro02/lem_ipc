#!/bin/bash

source $PWD/rsc/sh/color.sh

check_ipcs_free() {
	local ipcs_free=$(ipcs -m | grep -v "0x" | wc -l)
	if [ $ipcs_free -ne 4 ]; then
		display_color_msg ${RED} "Ipc leak found."
	else
		display_color_msg ${GREEN} "No IPC leak."
	fi
}

check_ipcs_free
