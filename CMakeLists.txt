cmake_minimum_required(VERSION 3.28)
project(Othello)

set(CMAKE_CXX_STANDARD 17)

add_executable(untitled main.cpp
        AI.h
        DumAI.h
        SmartAI.h
        GameState.h)
