cmake_minimum_required(VERSION 3.10)

project(shamir_secret_sharing C)

set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(shamir_secret_sharing
    src/shamir_secret_sharing.c
    src/galois.c
    src/shamir.c
)

set_target_properties(shamir_secret_sharing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)