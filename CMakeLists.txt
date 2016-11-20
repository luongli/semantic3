cmake_minimum_required(VERSION 3.6)
project(semantic3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    charcode.c
    charcode.h
    debug.c
    debug.h
    error.c
    error.h
    main.c
    parser.c
    parser.h
    reader.c
    reader.h
    scanner.c
    scanner.h
    semantics.c
    semantics.h
    symtab.c
    symtab.h
    token.c
    token.h)

add_executable(semantic3 ${SOURCE_FILES})