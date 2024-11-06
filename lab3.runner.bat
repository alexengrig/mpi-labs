@echo off
set EXE_PATH=cmake-build-debug/lab3.exe

set PROCESS_COUNTS=3 5 9 13 16

for %%P in (%PROCESS_COUNTS%) do (
    echo mpiexec -n %%P %EXE_PATH% benchmark
    mpiexec -n %%P %EXE_PATH% benchmark
    echo.
)
