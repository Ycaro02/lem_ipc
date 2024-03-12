#!/bin/bash

source $PWD/rsc/sh/color.sh

all_check() {
	ipcs | grep -e "0x" | grep "666" | cut -d ' ' -f 2 > /dev/null
	if [ $? -ne 0 ]; then
		display_color_msg ${RED} "Leak found after clean."
		free_ipcs ${IPCS_OPT} ${MSG}
	else
		display_color_msg ${GREEN} "No leak after clear."
	fi

}

free_ipcs() {
	IPCS_OPT="$1"
	MSG="$2"
	IPCS_ID=$(ipcs ${IPCS_OPT} | grep -e "0x" | grep "666" | cut -d ' ' -f 2)
	if [ $? -ne 0 ]; then
		display_color_msg ${GREEN} "Nothing to clean no ${MSG} leak found."
	else
		display_color_msg ${YELLOW} "Clear ${IPCS_ID} ${MSG} leak found."
		ipcrm ${IPCS_OPT} ${IPCS_ID}
		ipcs -s | grep -e "0x" | grep "666" | cut -d ' ' -f 2
	fi
}

check_ipcs_free() {
	IPCS_OPT="$1"
	MSG="$2"
	local ipcs_free=$(ipcs ${IPCS_OPT} | grep -e "0x" | grep -v "600" | wc -l)
	if [ $ipcs_free -ne 0 ]; then
		display_color_msg ${RED} "${MSG} leak found."
		free_ipcs ${IPCS_OPT} ${MSG}
	else
		display_color_msg ${GREEN} "No ${MSG} leak."
	fi
}

check_ipcs_free -m "Shared memory"
check_ipcs_free -s "Semaphore"
all_check
