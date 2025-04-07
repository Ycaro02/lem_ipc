#!/bin/bash




# Function to download and install SDL2_ttf
function load_SDL2_TTF {
	local url="${1}"
	local backup_url=${2}

	cd ${BASE_DIR}
	
	local file=$(handle_lib_download ${url} ${backup_url})

    tar -xzf ${file} >> ${FD_OUT} 2>&1
    rm -rf ${file}
	local dir_name=$(remove_archive_extension ${file})
	echo -n ${dir_name}

	cd ${dir_name}

	mkdir build
	cd build
	cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
		-DCMAKE_PREFIX_PATH=${INSTALL_DIR} \
		-DFREETYPE_INCLUDE_DIRS=${INSTALL_DIR}/include/freetype2 \
		-DFREETYPE_LIBRARY=${INSTALL_DIR}/lib/libfreetype.so \
		-DSDL2_INCLUDE_DIR=${INSTALL_DIR}/include/SDL2 \
		>> ${FD_OUT} 2>&1

	make -s -j$(nproc) >> ${FD_OUT} 2>&1
	make -s install >> ${FD_OUT} 2>&1

    # Clean up
    cd ../..
	display_color_msg ${GREEN} "${dir_name} installed successfully."
}

function load_SDL2 {
	local sdl_archive="${1}"
	local backup_url="${2}"
	# local sdl_dir_version="${2}"
	local sdl_dir="${BASE_DIR}/SDL2"
	
	cd ${BASE_DIR}


	if [ ! -d "${sdl_dir}" ]; then
        # display_color_msg ${CYAN} "Get SDL2 repo..."

		local file=$(handle_lib_download ${sdl_archive} ${backup_url})
    	tar -xzf ${file} >> ${FD_OUT} 2>&1
    	rm -rf ${file}
		local dir_name=$(remove_archive_extension ${file})

		echo "mv ${dir_name} ${sdl_dir}"
		mv ${dir_name} ${sdl_dir}
		
		cd ${sdl_dir}
		display_color_msg ${YELLOW} "Compile and install SDL2 in ${INSTALL_DIR}..."
		mkdir build
		cd build
		cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
			-DCMAKE_PREFIX_PATH=${INSTALL_DIR} \
            -DCMAKE_INCLUDE_PATH=${INSTALL_DIR}/include \
            -DCMAKE_LIBRARY_PATH=${INSTALL_DIR}/lib \
            -DBUILD_SHARED_LIBS=ON \
			>> ${FD_OUT} 2>&1

		make -s -j$(nproc) >> ${FD_OUT} 2>&1
		make -s install >> ${FD_OUT} 2>&1
		display_color_msg ${GREEN} "SDL2 installation done in ${INSTALL_DIR}."
	fi

}

# Get the current directory
PWD=$(pwd)

# Variables
BASE_DIR="$PWD/rsc/lib"
DEPS_DIR="$BASE_DIR/deps"
INSTALL_DIR="$BASE_DIR/install"

# Load the color script and utils functions
# Need to declare PWD and DEPS/INSTALL_DIR before loading the script
source ${PWD}/rsc/install/install_utils.sh

mkdir -p ${DEPS_DIR} ${INSTALL_DIR}/lib/pkgconfig ${INSTALL_DIR}/include

FD_OUT="${PWD}/rsc/log/load_lib.log"
display_color_msg ${CYAN} "Quiet mode enabled log in ${FD_OUT}"

# Set environment variables for dependencies
export PKG_CONFIG_PATH="${INSTALL_DIR}/lib/pkgconfig"
export CFLAGS="-I${INSTALL_DIR}/include"
export CXXFLAGS="-I${INSTALL_DIR}/include -L${INSTALL_DIR}/lib"
export LDFLAGS="-L${INSTALL_DIR}/lib"

rm -f ${FD_OUT}

# Cut script execution if any command fails
set -e 

# load_dependencies
load_SDL2 "https://www.libsdl.org/release/SDL2-2.30.5.tar.gz" "https://github.com/libsdl-org/SDL/releases/download/release-2.30.5/SDL2-2.30.5.tar.gz"
# load_SDL2 "https://testaloURL1" "https://github.com/libsdl-org/SDL/releases/download/release-2.30.5/SDL2-2.30.5.tar.gz"

# Load FreeType for SDL2_ttf for text rendering
load_lib "https://sourceforge.net/projects/freetype/files/freetype2/2.11.0/freetype-2.11.0.tar.gz/download" "https://download.savannah.gnu.org/releases/freetype/freetype-2.11.0.tar.gz" "freetype-2.11.0.tar.gz"
# load_lib "https://FREETYPE_NOP" "https://download.savannah.gnu.org/releases/freetype/freetype-2.11.0.tar.gz" "freetype-2.11.0.tar.gz"

load_SDL2_TTF "https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.22.0.tar.gz" "2.22.0" "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-2.22.0.tar.gz"
# load_SDL2_TTF "https://SDL_TFF_NOP" "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-2.22.0.tar.gz"
