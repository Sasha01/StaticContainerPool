#!/bin/bash

# Absolute path to this script, e.g. /home/user/bin/foo.sh
SCRIPT=$(readlink -f "$0")
# Absolute path this script is in, thus /home/user/bin
SCP_ROOT_DIR=$(dirname "$SCRIPT")/..

export PATH=$PATH:/home/sasha/.local/bin
cd $SCP_ROOT_DIR/build
gcovr -r ./.. --html-details -o cov.html # --html-self-contained
