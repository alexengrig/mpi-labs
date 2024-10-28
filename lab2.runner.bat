@echo off
set EXE_PATH=cmake-build-debug/lab2.exe

set ARRAY_SIZES=16 256 1024 16384 65536 131072
set PROCESS_COUNTS=2 5 9 13 16

for %%A in (%ARRAY_SIZES%) do (
    for %%P in (%PROCESS_COUNTS%) do (
        echo mpiexec -n %%P %EXE_PATH% %%A benchmark
        mpiexec -n %%P %EXE_PATH% %%A benchmark
        echo.
    )
)
