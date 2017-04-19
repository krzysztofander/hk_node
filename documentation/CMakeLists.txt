cmake_minimum_required (VERSION 2.8)
cmake_policy(SET CMP0012 NEW)
find_package(PythonInterp)
find_package(Doxygen)

set (PROJNAME DocumentationGeneration)

project (${PROJNAME})

#check if called on a right place
    if (NOT DEFINED TOP)
        message(FATAL_ERROR "This CMakeLists.txt can only used from a master CMakeLists.txt\n")
    endif()
    #COMMON and TOP are set correctly here

#variables

    SET(DOCGEN_CONF_INPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR})
    set(DocumentationOut "${CMAKE_CURRENT_BINARY_DIR}/out" CACHE STRING "Where to generate documentation to")

#sources

    set(DOC_CONF
        ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-config.dxc.in
        commands.rst
        parts.rst
        board.rst
        conf.py.in
        index.rst.in
        ${TOP}/README.md
        )


#build
    if (${BUILD_DOCUMENTATION})
        #if we do want documentation

#doxygen
        if (${DOXYGEN_FOUND})
            set (DOXYGEN_OUTPUT_DIRECTORY_APX doxygen_output)
            set (DOXYGEN_OUTPUT_DIRECTORY ${DocumentationOut}/${DOXYGEN_OUTPUT_DIRECTORY_APX})
            message (STATUS "Using doxygen to generate into ${DOXYGEN_OUTPUT_DIRECTORY}")

            configure_file(
                    "${CMAKE_CURRENT_SOURCE_DIR}/doxygen-config.dxc.in"
                    "${DOCGEN_CONF_INPUT_FOLDER}/doxygen-config.dxc")


            add_custom_target ( Doxygen ALL
                COMMAND
                    pwd && echo "${DOXYGEN_EXECUTABLE} ${DOCGEN_CONF_INPUT_FOLDER}/doxygen-config.dxc"
                COMMAND
                    mkdir -p ${DOXYGEN_OUTPUT_DIRECTORY}
                COMMAND
                    ${DOXYGEN_EXECUTABLE} ${DOCGEN_CONF_INPUT_FOLDER}/doxygen-config.dxc
                COMMENT "Generating with Doxygen"
                           )

        else()
            message ("No doxygen found")
        endif()


#sphinx


            configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/index.rst.in"
            "${DOCGEN_CONF_INPUT_FOLDER}/index.rst")

            configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/commands.rst"
            "${DOCGEN_CONF_INPUT_FOLDER}/commands.rst")

            configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/parts.rst"
            "${DOCGEN_CONF_INPUT_FOLDER}/parts.rst")

			configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/board.rst"
            "${DOCGEN_CONF_INPUT_FOLDER}/board.rst")


            configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in"
            "${DOCGEN_CONF_INPUT_FOLDER}/conf.py")


        set (SPHINX_WARNINGS_FILE ./temp_sphinxWarnings)

        add_custom_target(${PROJNAME} ALL
                          COMMAND
                              echo "Executing py.test so it generates Sphinx files"
                          COMMAND
                              echo "Generating documentation with Sphinx"
                          COMMAND
                              #call sphinx
                              sphinx-build -w ${SPHINX_WARNINGS_FILE} ${DOCGEN_CONF_INPUT_FOLDER} ${DocumentationOut}
                          COMMAND
                              #sphinx somehow do not return any sort of error code when encountering errors in source files
                              # Therefore parsing warnings file (see -w option) for keyword "ERROR" and, if found,
                              # returning error code other than 0 from whole custom target build so
                              # make and other tools like Jenkings report build failure
                              touch ${SPHINX_WARNINGS_FILE} && cat ${SPHINX_WARNINGS_FILE} | sed  "/ERROR/{q100}"
                                  #error code would be 100 when a keyword "ERROR" got found
                                  #error code would be 0 when there was no SPHINX_WARNINGS_FILE  (touch will create that)
                          VERBATIM
                          SOURCES ${DOC_CONF}
                          )



  else()
    #just to see a project in IDE
    add_custom_target(
                      ${PROJNAME}
                      SOURCES ${DOC_CONF}
                      )

   endif()

