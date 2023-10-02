# g++ main.cpp -o main -g -std=c++23 -O2

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build

printf "\n\n=============================================================================\n\n"

rm compile_commands.json
compdb -p build/ list > compile_commands.json
#mv build/compile_commands.json .
#
printf "\n\n=============================================================================\n\n"

# make CXXFLAGS="-O2" -j 16 -C build
cmake --build build --config Release -j 16

printf "\n\n=============================================================================\n\n"
cd build > /dev/null

ctest --output-on-failure -j 16

result=$?


printf "\n\n=============================================================================\n\n"

if [ $result -eq 0 ]; then
    ./NeithECS
fi
