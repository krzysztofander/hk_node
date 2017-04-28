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

    set(GOOGLET_UT_LIBS
            gtest_main
            gmock_main
        )
   set(SERIAL_SOURCES
        ${TOP}/hk_node/src/serial.cpp
        ${TOP}/hk_node/src/comm.cpp
        ${TOP}/hk_node/src/comm_statemachine.cpp
        ${TOP}/hk_node/src/out_builder.cpp
        ${TOP}/hk_node/src/comm_r.cpp
        ${TOP}/hk_node/src/blinker.cpp
        ${TOP}/hk_node/src/comm_extra_records.cpp
        ${TOP}/hk_node/src/comm_extra_rec_handlers.cpp
        ${TOP}/hk_node/src/comm_defs.cpp
        ${TOP}/hk_node/src/comm_state.cpp
        ${TOP}/hk_node/src/MiniInParser.cpp
        ${TOP}/hk_node/src/in_command_wrap.cpp
        serial_ut.cc
        serial_ut_r.cc 
        blinker_ut.cc
        preamble_and_bt_ut.cc
        mock.cc
        mock.h
        Arduino.h
        ${TOP}/hk_node/src/hk_node.h
        ${TOP}/hk_node/src/hk_node_utils.h
        ${TOP}/hk_node/src/hk_node_config.h
        ${TOP}/hk_node/src/blinker.h
        ${TOP}/hk_node/src/comm.h
        ${TOP}/hk_node/src/out_builder.h
        ${TOP}/hk_node/src/in_command_wrap.h

        ${TOP}/hk_node/src/comm_extra_records.h
        ${TOP}/hk_node/src/comm_extra_rec_handlers.h
        ${TOP}/hk_node/src/comm_defs.h
        ${TOP}/hk_node/src/comm_state.h

        ${TOP}/hk_node/src/serial.h
        ${TOP}/hk_node/src/MiniInParser.h
    )
    add_definitions(-DINUNITTEST)
    add_test_executable(serial_ut "${GOOGLET_UT_LIBS}" "${SERIAL_SOURCES}" )

    set(EXECUTUR_SOURCES
        ${TOP}/hk_node/src/executor.cpp
        ${TOP}/hk_node/src/temp_measurement.cpp
        executor_ut.cc
        executor_ut.h
        temp_measure_ut.cc
        Arduino.h
        ${TOP}/hk_node/src/hk_node.h
        ${TOP}/hk_node/src/executor.h
        ${TOP}/hk_node/src/executor_base.h
        ${TOP}/hk_node/src/temp_measurement.h
        

    )

    add_test_executable(executor_ut "${GOOGLET_UT_LIBS}" "${EXECUTUR_SOURCES}" )

    set(MAIN_SOURCES
        ${TOP}/hk_node/src/hk_node_main.cpp
        ${TOP}/hk_node/src/supp.cpp
        ${TOP}/hk_node/src/blinker.cpp
        ${TOP}/hk_node/src/sleeper.cpp
        ${TOP}/hk_node/src/temp_sensor.cpp
        ${TOP}/hk_node/src/nv.cpp
        ${TOP}/hk_node/src/adc.cpp
        Arduino.h
        ${TOP}/hk_node/src/hk_node.h
        ${TOP}/hk_node/src/supp.h
        ${TOP}/hk_node/src/blinker.h
        ${TOP}/hk_node/src/sleeper.h
        ${TOP}/hk_node/src/nv.h
        ${TOP}/hk_node/src/adc.h
        ${TOP}/hk_node/README.md

    )

	set (BUILD_MAIN_UT FALSE CACHE BOOL "Build main UT")
	if (${BUILD_MAIN_UT})
		
	    add_test_executable(main_ut "${GOOGLET_UT_LIBS}" "${MAIN_SOURCES}" )
	else()
		
		add_custom_target(main_ut
			SOURCES "${MAIN_SOURCES}"
			)

	endif()