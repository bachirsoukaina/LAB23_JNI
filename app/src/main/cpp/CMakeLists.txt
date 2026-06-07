cmake_minimum_required(VERSION 3.22.1)

project("shieldcore")

add_library(
        shieldcore
        SHARED
        shield.cpp)

find_library(
        android-log
        log)

target_link_libraries(
        shieldcore
        ${android-log})