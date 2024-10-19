git submodule update --init --force --remote  --recursive

mkdir build_debug
mkdir build_release

cmake -DCMAKE_BUILD_TYPE=Debug   -S . -B build_debug
cmake -DCMAKE_BUILD_TYPE=Release -S . -B build_release

cmake --build build_debug   --config Debug --parallel
cmake --build build_release --config Release --parallel

