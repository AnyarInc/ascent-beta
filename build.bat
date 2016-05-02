@echo off

set generator=Visual Studio 14 2015 Win64

if not exist build/NUL mkdir build
cd build

echo Building IDE...
cmake  ../ -G "%generator%" 

echo Building libraries...
cmake --build . --target ALL_BUILD --config Debug
cmake --build . --target ALL_BUILD --config Release

set /p finished = Build finished: Press ENTER to close.
exit