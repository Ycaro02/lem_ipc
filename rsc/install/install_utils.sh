#!/bin/bash

source ${PWD}/rsc/sh/color.sh

function get_file_name {
	local url=${1}

	local file_name=$(echo -n ${url} | rev | awk -F '/' '{print $1}' | rev)
	echo -n ${file_name}
}

function remove_archive_extension {
	local archive_name="${1}"
	echo -n "${archive_name}" | awk -F .tar '{print $1}'
}

function get_archive_extention {
	local name=${1}

	echo -n ${name} | rev | cut -d . -f 1-2 | rev
}

# Load the archive file from the url and extract it to the directory, handle tar.gz and tar.xz
function archive_to_dir {
	local name=${1}
	local extension=$(get_archive_extention ${name})

	
	if [[ ${extension} == "tar.gz" ]]; then
		tar -xzf ${DEPS_DIR}/${name} -C ${DEPS_DIR} >> ${FD_OUT} 2>&1
	elif [[ ${extension} == "tar.xz" ]]; then
		tar -xf ${DEPS_DIR}/${name} -C ${DEPS_DIR} >> ${FD_OUT} 2>&1
	fi

	local dir_name=$(remove_archive_extension ${name})
	echo -n "${dir_name}"
}

# Handle archive download with backup link
function handle_lib_download {
	local url="${1}"
	local backup_url="${2}"

	local file=$(get_file_name ${url})

	cd ${BASE_DIR}



	display_color_msg ${GREEN} "Download on ${url}" 1>&2
    
	wget ${url} >> ${FD_OUT} 2>&1 || true

	if [[ ! -f ${file} ]]; then
		display_double_color_msg ${RED} "Error when download on ${url}" ${YELLOW} "\ntry to use backup url ${backup_url}" 1>&2
		local file=$(get_file_name ${backup_url})
		wget ${backup_url} >> ${FD_OUT} 2>&1
	fi

	echo -n ${file}
}


# Install a library from url, name, extension and configure flags, using configure make and make install (default configure flags is empty)
function install_library {
    local url=${1}
	local backup_url=${2}
	local name=${3}

	file=$(handle_lib_download ${url} ${backup_url})

	cd ${BASE_DIR}

	mv ${file} ${DEPS_DIR}/${name}

	archive_to_dir ${name}

	rm ${DEPS_DIR}/${name}

	local dir_name=$(archive_to_dir ${name})
	echo dir_name: ${dir_name}
	cd ${DEPS_DIR}/${dir_name}

	# run configure script
    ./configure --prefix=${INSTALL_DIR} $configure_flags
	# compile and install
    make -s -j$(nproc)
    make -s install

    cd ${DEPS_DIR}
}




# Load a library from a url and configure flags
function load_lib {
	local url="${1}"
	local backup_url="${2}"
	local file_name=${3}

	display_color_msg ${MAGENTA} "Download ${name}..."
	install_library ${url} ${backup_url} ${file_name} >> ${FD_OUT} 2>&1
	display_color_msg ${GREEN} "Done ${name}"
}

