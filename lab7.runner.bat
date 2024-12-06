@echo off
set EXE_PATH=cmake-build-debug/lab7.exe

set MATRIX_SIZES=10 50 100 200 500 1000 10000
set PROCESS_COUNTS=1 2 8 16 32 64

for %%M in (%MATRIX_SIZES%) do (
    for %%P in (%PROCESS_COUNTS%) do (
        echo mpiexec -n %%P %EXE_PATH% %%M benchmark
        mpiexec -n %%P %EXE_PATH% %%M benchmark
        echo.
    )
)
