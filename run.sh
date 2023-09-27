# g++ main.cpp -o main -g -std=c++23 -O2

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build
rm compile_commands.json
compdb -p build/ list > compile_commands.json
#mv build/compile_commands.json .
make CXXFLAGS="-O2" -j 16 -C build
printf "\n\n=============================================================================\n\n"

./build/NeithECS
