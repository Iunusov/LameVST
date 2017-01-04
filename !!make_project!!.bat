@echo off

if not exist deps/vst_sdk_2.4 (
cd deps
call sync
cd ..
) 


mkdir build
echo.
cd build
cmake ..
echo.
echo Done!
echo.
echo You can find VC solution project in the "build" subfolder.
cd ..
echo.
build\LameVST_Solution.sln
PAUSE
