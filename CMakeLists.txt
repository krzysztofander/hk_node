cmake_minimum_required (VERSION 2.8)
cmake_policy(SET CMP0012 NEW)

project (home_control_node)

    set (TOP ${CMAKE_SOURCE_DIR})

# Unit Tests
#===========

    set(BUILD_UNIT_TESTS TRUE CACHE BOOL "Master enable for unit test")
    if (${BUILD_UNIT_TESTS})
        add_subdirectory(ut)
    endif()   

# Documentation
#==============

    set(BUILD_DOCUMENTATION TRUE CACHE BOOL "Enables build of documentation")
	add_subdirectory(documentation)


	# Unit Tests
#===========

set(BUILD_MINIPARSER TRUE CACHE BOOL "MINIPARSER")
    if (${BUILD_MINIPARSER})
        add_subdirectory(miniparser)
    endif()   

