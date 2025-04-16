#!/bin/bash

source rsc/sh/color.sh

ascii_lemipc() {

	echo -e "\n"
	echo -e "  ${L_CYAN}██          ███████     ███    ███     ██     ██████       ██████ ${RESET}"
	echo -e "  ${L_CYAN}██          ██          ████  ████     ██     ██   ██     ██      ${RESET}"
	echo -e "  ${L_CYAN}██          █████       ██ ████ ██     ██     ██████      ██      ${RESET}"
	echo -e "  ${L_CYAN}██          ██          ██  ██  ██     ██     ██          ██      ${RESET}"
	echo -e "  ${L_CYAN}███████     ███████     ██      ██     ██     ██           ██████ ${RESET}\n\n"
}


ascii_tester() {
	echo -e ""
	echo -e "  ${L_MAGENTA}██          ███████     ███    ███     ██     ██████       ██████  ${RESET}"
	echo -e "  ${L_MAGENTA}██          ██          ████  ████     ██     ██   ██     ██       ${RESET}"
	echo -e "  ${L_MAGENTA}██          █████       ██ ████ ██     ██     ██████      ██       ${RESET}"
	echo -e "  ${L_MAGENTA}██          ██          ██  ██  ██     ██     ██          ██       ${RESET}"
	echo -e "  ${L_MAGENTA}███████     ███████     ██      ██     ██     ██           ██████  ${RESET}\n\n"

	echo -e "  ${L_RED}████████    ███████    ███████    ████████    ███████    ██████  ${RESET}"
	echo -e "  ${L_RED}   ██       ██         ██            ██       ██         ██   ██ ${RESET}"
	echo -e "  ${L_RED}   ██       █████      ███████       ██       █████      ██████  ${RESET}"
	echo -e "  ${L_RED}   ██       ██              ██       ██       ██         ██   ██ ${RESET}"
	echo -e "  ${L_RED}   ██       ███████    ███████       ██       ███████    ██   ██ ${RESET}\n"
}

if [ "$1" == "lemipc" ]; then
	ascii_lemipc
fi

if [ "$1" == "tester" ]; then
	ascii_tester
fi