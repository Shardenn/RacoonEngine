mkdir build
pushd .
cd build
cmake -G "Visual Studio 17 2022" ..
start RacoonEngine.sln
popd