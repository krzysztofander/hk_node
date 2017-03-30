cmake_minimum_required (VERSION 2.8)
cmake_policy(SET CMP0012 NEW)

project (miniparser)

        set(SOURCES
        main.cpp
        ../src/MiniInParser.cpp
        ../src/MiniInParser.h

        MiniInParserTests.cpp
        MiniInParserTests.h

    )
   include_directories(../src)


   add_executable(miniparser ${SOURCES})
