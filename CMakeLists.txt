#[[
    Abstractions and Models are NOT at WAR!
                                            - Cobwoy's Talisman
    But Abstractions don't care and Models can't understand!!
                                            - Lul, Practicality
 ]]
cmake_minimum_required(VERSION 3.4)

# Set the Project name
set(APPLICATION_NAME "BiosReader")

# Set Application project
project(${APPLICATION_NAME} VERSION 1.0)

# Setting some flags

# For complete list of Compiler IDs check output_required_files
# https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html#variable:CMAKE_%3CLANG%3E_COMPILER_ID
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-enum-enum-conversion")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration")
endif()

# Remember it is SMBIOS data that is stored in LE format
# Our program however can be facing any of the BE or LE (not sure about middle-endian or mixed-endian) compilation palatform
if(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
	add_compile_definitions(BR_BIG_ENDIAN)
elseif(CMAKE_C_BYTE_ORDER STREQUAL "LITTLE_ENDIAN")
	add_compile_definitions(BR_LITTLE_ENDIAN)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_compile_definitions(BR_SIXTY_FOUR_BIT_ISA)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    add_compile_definitions(BR_THIRTY_TWO_BIT_ISA)
endif()

if(WIN32)
    add_compile_definitions(BR_WINDOWS_PLATFORM)
	if(MSVC)
	    # For faster multithreaded compilation
        #add_definitions(/MP)
	endif()
elseif(UNIX AND NOT APPLE)
    add_compile_definitions(BR_LINUX_PLATFORM)
    add_compile_definitions(__USE_GNU)
elseif(APPLE)
    add_compile_definitions(BR_MAC_PLATFORM)
    add_compile_definitions(__USE_GNU)
    find_library(IOKit IOKit)
    find_library(CoreServices CoreServices)
endif()

# BiosReader's personal code
file(GLOB_RECURSE CFILES ${CMAKE_CURRENT_SOURCE_DIR}/src/private/*.c)
# Shows the headerfile directory in project
file(GLOB_RECURSE HEADERFILES ${CMAKE_CURRENT_SOURCE_DIR}/src/public/*.h)

# Building the project (STATIC or DYNAMIC (SHARED))
if (BUILD_SHARED_LIBS)
    add_library(${APPLICATION_NAME} SHARED ${CFILES} ${HEADERFILES})
else()
    add_library(${APPLICATION_NAME} STATIC ${CFILES} ${HEADERFILES})
endif()

if(WIN32)
    target_link_libraries(${APPLICATION_NAME} PUBLIC Ws2_32.lib)
elseif(APPLE)
    target_link_libraries(${APPLICATION_NAME} PUBLIC ${CoreServices} ${IOKit})
elseif(UNIX AND NOT APPLE)
    #target_link_libraries(${APPLICATION_NAME} PUBLIC libcapng)
endif()

target_include_directories(${APPLICATION_NAME}
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src/public
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/public>
        # where external projects will look for the library's public headers
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_definitions(${APPLICATION_NAME} PUBLIC BiosReader)

# Post build command
if(UNIX AND NOT APPLE)
	if (BUILD_SHARED_LIBS)
	    set(Library_NAME "libBiosReader.so")
	else()
	    set(Library_NAME "BiosReader")
	endif()

    add_custom_command(TARGET ${APPLICATION_NAME} POST_BUILD
	    COMMAND cd ${REPOSITORYROOT}/KarmaBin
	    COMMAND mkdir temp
	    COMMAND cp ${Library_NAME} temp/${Library_NAME}
	    COMMAND cd temp
	    COMMAND sudo install -o root -g root -m u=rxs,g-rx,o=x -t ${REPOSITORYROOT}/KarmaBin/ ${Library_NAME}
	    COMMAND rm ${Library_NAME}
	    COMMAND cd ${REPOSITORYROOT}/KarmaBin
	    COMMAND sudo rmdir temp
    )
endif()

#[[
 *                                          /\
 *                                         / /
 *                                      /\| |
 *                                      | | |/\
 *                                      | | / /
 *                                      | `  /
 *                                      `\  (___
 *                                     _.->  ,-.-.
 *                                  _.'      |  \ \
 *                                 /    _____| 0 |0\
 *                                |    /`    `^-.\.-'`-._
 *                                |   |                  `-._
 *                                |   :                      `.
 *                                \    `._     `-.__         O.'
 *         _.--,                   \     `._     __.^--._O_..-'
 *        `---, `.                  `\     /` ` `
 *             `\ `,                  `\   |
 *              |   :                   ;  |
 *              /    `.              ___|__|___
 *             /       `.           (          )
 *            /    `---.:____...---' `--------`.
 *           /        (         `.      __      `.
 *          |          `---------' _   /  \       \
 *          |    .-.      _._     (_)  `--'        \
 *          |   (   )    /   \                       \
 *           \   `-'     \   /                       ;-._
 *            \           `-'           \           .'   `.
 *            /`.                  `\    `\     _.-'`-.    `.___
 *           |   `-._                `\    `\.-'       `-.   ,--`
 *            \      `--.___        ___`\    \           ||^\\
 *             `._        | ``----''     `.   `\         `'  `
 *                `--;     \  jgs          `.   `.
 *                   //^||^\\               //^||^\\
 *                   '  `'  `               '   '  `
 ]]
