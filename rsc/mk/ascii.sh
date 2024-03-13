#!/bin/bash

source rsc/sh/color.sh

ascii_lemipc() {

	echo -e "\n"
	echo -e "  ${LIGHT_CYAN}██          ███████     ███    ███     ██     ██████       ██████ ${RESET}"
	echo -e "  ${LIGHT_CYAN}██          ██          ████  ████     ██     ██   ██     ██      ${RESET}"
	echo -e "  ${LIGHT_CYAN}██          █████       ██ ████ ██     ██     ██████      ██      ${RESET}"
	echo -e "  ${LIGHT_CYAN}██          ██          ██  ██  ██     ██     ██          ██      ${RESET}"
	echo -e "  ${LIGHT_CYAN}███████     ███████     ██      ██     ██     ██           ██████ ${RESET}\n\n"
}


ascii_tester() {
	echo -e ""
	echo -e "  ${LIGHT_RED}██          ███████     ███    ███     ██     ██████       ██████ \t ████████    ███████    ███████    ████████    ███████    ██████  ${RESET}"
	echo -e "  ${LIGHT_RED}██          ██          ████  ████     ██     ██   ██     ██      \t    ██       ██         ██            ██       ██         ██   ██ ${RESET}"
	echo -e "  ${LIGHT_RED}██          █████       ██ ████ ██     ██     ██████      ██      \t    ██       █████      ███████       ██       █████      ██████  ${RESET}"
	echo -e "  ${LIGHT_RED}██          ██          ██  ██  ██     ██     ██          ██      \t    ██       ██              ██       ██       ██         ██   ██ ${RESET}"
	echo -e "  ${LIGHT_RED}███████     ███████     ██      ██     ██     ██           ██████ \t    ██       ███████    ███████       ██       ███████    ██   ██ ${RESET}\n"
}

if [ "$1" == "lemipc" ]; then
	ascii_lemipc
fi

if [ "$1" == "tester" ]; then
	ascii_tester
fi