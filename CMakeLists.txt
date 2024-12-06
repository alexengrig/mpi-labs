cmake_minimum_required(VERSION 3.10)
project(mpi-labs C)

set(CMAKE_C_STANDARD 11)

# Paths of MS-MPI
set(MPI_C_INCLUDE_DIR "C:/Program Files (x86)/Microsoft SDKs/MPI/Include")
set(MPI_C_LIBRARIES "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64/msmpi.lib")

set(CMAKE_INCLUDE_PATH ${MPI_C_INCLUDE_DIR} ${CMAKE_INCLUDE_PATH})
set(CMAKE_LIBRARY_PATH ${MPI_C_LIBRARIES} ${CMAKE_LIBRARY_PATH})
set(MPI_INCLUDE_DIRS ${MPI_C_INCLUDE_DIR})

include_directories(${MPI_C_INCLUDE_DIR})
link_directories("C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64")

message(STATUS "MPI_INCLUDE_DIRS: ${MPI_INCLUDE_DIRS}")
message(STATUS "MPI_LIBRARIES: ${MPI_LIBRARIES}")

find_package(MPI REQUIRED)

add_executable(lab1 lab1.c)
target_include_directories(lab1 PRIVATE ${MPI_C_INCLUDE_DIR})
target_link_libraries(lab1 PRIVATE MPI::MPI_C)

add_executable(lab1sync lab1sync.c)
target_include_directories(lab1sync PRIVATE ${MPI_C_INCLUDE_DIR})
target_link_libraries(lab1sync PRIVATE MPI::MPI_C)

add_executable(lab2 lab2.c)
target_include_directories(lab2 PRIVATE ${MPI_C_INCLUDE_DIR})
target_link_libraries(lab2 PRIVATE MPI::MPI_C)

add_executable(lab3 lab3.c)
target_include_directories(lab3 PRIVATE ${MPI_C_INCLUDE_DIR})
target_link_libraries(lab3 PRIVATE MPI::MPI_C)

add_executable(lab4 lab4.c)
target_include_directories(lab4 PRIVATE ${MPI_C_INCLUDE_DIR})
target_link_libraries(lab4 PRIVATE MPI::MPI_C)

add_executable(lab5 lab5.c)
target_include_directories(lab5 PRIVATE ${MPI_C_INCLUDE_DIR})
target_link_libraries(lab5 PRIVATE MPI::MPI_C)

add_executable(lab6 lab6.c)
target_include_directories(lab6 PRIVATE ${MPI_C_INCLUDE_DIR})
target_link_libraries(lab6 PRIVATE MPI::MPI_C)

add_executable(lab7 lab7.c)
target_include_directories(lab7 PRIVATE ${MPI_C_INCLUDE_DIR})
target_link_libraries(lab7 PRIVATE MPI::MPI_C)
