@echo off
set EXE_PATH=cmake-build-debug/lab4.exe

set PROCESS_COUNTS=2 5 9 13 16

for %%P in (%PROCESS_COUNTS%) do (
    echo mpiexec -n %%P %EXE_PATH%
    mpiexec -n %%P %EXE_PATH%
    echo.
)
