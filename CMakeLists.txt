cmake_minimum_required (VERSION 2.8)
cmake_policy(SET CMP0012 NEW)

project (home_control_node)

    set (TOP ../)

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
