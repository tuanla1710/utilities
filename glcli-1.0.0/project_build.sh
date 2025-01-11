#!/bin/bash
# set -x
set -e

# Color definitions
RED='\E[1;31m'
GREEN='\E[1;32m'
YELLOW='\E[1;33m'
BLUE='\E[1;34m'
PINK='\E[1;35m'
RES='\E[0m'

# Variables
ROOT_PATH=$(cd $(dirname $0); pwd)
OUTPUT_PATH="${ROOT_PATH}/output"
CHECK_MODE=''
BUILD_MODE=''

# Export variables
export ROOT_PATH

# Functions
function help() {
    echo -e "${YELLOW}#============================[USAGE]===========================#${RES}"
    echo -e "${GREEN}[Usage]:${RES}"
    echo -e "1) init project"
    echo -e "    example: ./project_start.sh init"
    echo -e "2) check code"
    echo -e "    example: ./project_start.sh check --CHECK_MODE [clang-format]"
    echo -e "3) start build"
    echo -e "    example: ./project_start.sh build --BUILD_MODE [x86_64|arm64]"
    echo -e "${YELLOW}#============================[USAGE]===========================#${RES}"
}


# Function to set host compiler environment variables
set_host_compiler_env() {
    export CC=gcc
    export CXX=g++
    export AR=ar
    export AS=as
    export LD=ld
    export RANLIB=ranlib
    export STRIP=strip
}

# Function to set cross compiler environment variables
set_cross_compiler_env() {
    export CC=riscv32-poky-linux-gcc
    export CXX=riscv32-poky-linux-g++
    export AR=riscv32-poky-linux-ar
    export AS=riscv32-poky-linux-as
    export LD=riscv32-poky-linux-ld
    export RANLIB=riscv32-poky-linux-ranlib
    export STRIP=riscv32-poky-linux-strip
}

function project_init() {
    cd "${ROOT_PATH}"
    [[ -d "${OUTPUT_PATH}" ]] && rm -rf "${OUTPUT_PATH}"
    mkdir -pv "${OUTPUT_PATH}"
}

function code_check() {
    cd "${ROOT_PATH}"
    echo "Check code: start"
    # ./check_code.sh ${CHECK_MODE}
    echo "Check code: finished."
}

function package_build() {
    cd "${ROOT_PATH}"
    echo "Build package: start"
    pushd sources
    ./build_cross.sh
        cp oru-cli ${OUTPUT_PATH}
        cp oru-cli-server ${OUTPUT_PATH}
        make clean
    popd
    echo "Build package: finished."
}

function parameters_process() {
    while [[ "$#" -gt 0 ]]; do
        case "$1" in
            init | check | build | help)
                operation_mode=$1; shift;;
            --CHECK_MODE)
                CHECK_MODE=$2; shift 2;;
            --BUILD_MODE)
                BUILD_MODE=$2; shift 2;;
            *)
                echo -e "${RED}[unknown parameter]: $1 ${RES}"
                help
                exit 1
                ;;
        esac
    done
}

function main() {
    parameters_process "$@"
    case ${operation_mode} in
        init)
            echo ">init..."
            project_init
            ;;
        check)
            echo ">check..."
            code_check
            ;;
        build)
            echo ">init..."
            project_init

            echo ">Building..."
            package_build
            ;;
        help)
            help
            ;;
        *)
            echo "Unsupported operation mode!"
            help
            exit 1
            ;;
    esac
}

# Main entry point
main "$@"
