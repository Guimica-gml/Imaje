#!/usr/bin/env sh
set -e

CFLAGS="-Wall -Wextra -pedantic -std=c11"
CLIBS=""

gcc $CFLAGS -o imaje main.c $CLIBS

if [ "$1" = "run" ]
then
    shift
    ./imaje "$@"
fi
