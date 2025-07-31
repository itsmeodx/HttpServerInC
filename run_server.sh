
#!/bin/sh

set -e # Exit early if any commands fail

# Build the project (CMake)
cd "$(dirname "$0")"
cmake -B build -S .
cmake --build ./build

# Run the server
exec $(dirname $0)/build/http-server "$@"
