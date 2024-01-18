#!/bin/bash

# Function to wait for a service to start
run_tests() {
    echo "Running tests..."

    # Test server startup
    echo "Testing server startup..."
    cd singleTests
    chmod +x start_server.sh
    gnome-terminal -- bash -c "./start_server.sh & exec bash"
    
    # Delay for 4 seconds
    sleep 4
    
    # Test client startup
    echo "Testing client startup..."
    chmod +x connect_client.sh
    gnome-terminal -- bash -c "./connect_client.sh & exec bash"
    
    
}

# Run the tests
run_tests
