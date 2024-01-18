#!/bin/bash

# Get the directory of the script
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Move to the parent directory of the script
cd "$script_dir"/..

# Compile the client code
gcc Client.c -o client.out

# Run the client with the specified parameters and disconnect immediatly
echo "EOF" | ./client.out 127.0.0.1 7788 client1

