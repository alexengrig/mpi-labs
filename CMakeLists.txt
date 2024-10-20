cmake_minimum_required(VERSION 3.10)
project(mpi-labs C)

set(CMAKE_C_STANDARD 11)

# Paths of MS-MPI
set(MPI_C_INCLUDE_DIR "C:/Program Files (x86)/Microsoft SDKs/MPI/Include")
set(MPI_C_LIBRARIES "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64/msmpi.lib")

set(CMAKE_INCLUDE_PATH ${MPI_C_INCLUDE_DIR} ${CMAKE_INCLUDE_PATH})
set(CMAKE_LIBRARY_PATH ${MPI_C_LIBRARIES} ${CMAKE_LIBRARY_PATH})

find_package(MPI REQUIRED)

add_executable(lab1 lab1.c)
target_include_directories(lab1 PRIVATE ${MPI_C_INCLUDE_DIR})
target_link_libraries(lab1 PRIVATE MPI::MPI_C)

add_executable(lab1sync lab1sync.c)
target_include_directories(lab1sync PRIVATE ${MPI_C_INCLUDE_DIR})
target_link_libraries(lab1sync PRIVATE MPI::MPI_C)
