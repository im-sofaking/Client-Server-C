#!/bin/bash

# Get the directory of the script
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Move to the parent directory of the script
cd "$script_dir"/..

# Compile the server code
gcc Server.c -o server.out

# Run the server with the desired port and directory path
./server.out 7788 "$script_dir"/..

