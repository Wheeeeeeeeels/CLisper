cmake_minimum_required(VERSION 3.23)
project(CLisper_main C)

set(CMAKE_C_STANDARD 11)

add_executable(CLisper_main
        prompt.c
        mpc.c
        parsing.c)

SET(DEPENDENCE_LIBS ${DEPENDENCE_LIBS}
        ${THIRDPARTY_DEPENDENCE_LIBS}
        -lm
        -lz
        -ldl
        -lbz2
        )
