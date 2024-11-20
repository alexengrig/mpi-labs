@echo off
set EXE_PATH=cmake-build-debug/lab5.exe

set PROCESS_COUNTS=1 6 8 12 16

for %%P in (%PROCESS_COUNTS%) do (
    echo mpiexec -n %%P %EXE_PATH%
    mpiexec -n %%P %EXE_PATH%
    echo.
)
