cmake_minimum_required (VERSION 2.8)
cmake_policy(SET CMP0012 NEW)

project (unit_tests)

    set (TOP ../../)

    
    #We do not need gmock (for now). Overwriting with FORCE
    set (BUILD_GMOCK ON CACHE BOOL "Builds gmock" FORCE)
    #but we do need gtest in all cases
    set (BUILD_GTEST ON  CACHE BOOL "Builds gtest" FORCE)
   
    set (gtest_force_shared_crt ON CACHE BOOL
    "Use shared (DLL) run-time lib even when Google Test is built as static lib."
    FORCE)

    add_subdirectory(${TOP}/googletest
                     ${CMAKE_CURRENT_BINARY_DIR}/googletest

    )

#less constrained macros:

    #This is same as # cxx_executable(name dir lib srcs...) from googletest/cmake/internal_utils.cmake
    #but less constrained
    #
    # add_test_executable(name projectName libraries srcs...)
    #
    # Creates a named target that depends on the given libs and is built
    # from the given source files.

    function(add_test_executable projectName libraries sources )
        cxx_executable_with_flags(
            ${projectName} "${cxx_default}" "${libraries}" ${sources})
    endfunction()


    include_directories(
        ./
        ${TOP}/hk_node/src
        ${TOP}/googletest/googletest/include/
        ${TOP}/googletest/googlemock/include/
    )


   set(SERIAL_SOURCES
        ${TOP}/hk_node/src/serial.cpp
        serial_ut.cc
        Arduino.h
        ${TOP}/hk_node/src/hk_node.h
        ${TOP}/hk_node/src/serial.h
    )


    set(SERIAL_UT_LIBS
            gtest_main
            gmock_main
        )


    add_test_executable(serial_ut "${SERIAL_UT_LIBS}" "${SERIAL_SOURCES}" )
    