
#!/bin/sh

set -e # Exit early if any commands fail

# Navigate to the script's directory
cd "$(dirname "$0")"

# Build the project (CMake)
# Release build optimized for speed
cmake -DCMAKE_BUILD_TYPE=Release -B build -S .
cmake --build ./build -j

# Run the server
exec $(dirname $0)/build/http-server "$@"
