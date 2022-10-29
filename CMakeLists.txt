#[[
    Abstractions and Models are NOT at WAR!
                                            - Cobwoy's Talisman
    But Abstractions don't care and Models can't understand!!
                                            - Lul, Practicality
 ]]
cmake_minimum_required(VERSION 3.4)

# This is the top level CMakeList file having single responsibility of
# generating enough context for entire Software BiosReader.

# First we specify the C++/C standard
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

# Control where the static and shared libraries are built
# We would want a semi-packaging fashioned way where all bianries are together
# whilst accessible from IDE (local debugger for instance!).
set(REPOSITORYROOT "${CMAKE_CURRENT_SOURCE_DIR}")
# Global variable for BiosReader Libraries
set(BIOSREADERLIBRARYFOLDER "CompanionLibraries")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${REPOSITORYROOT}/BiosReaderBin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${REPOSITORYROOT}/BiosReaderBin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${REPOSITORYROOT}/BiosReaderBin")

# Set the Project name
set(APPLICATION_NAME "BiosReader")

# Finally we specify policies, if any
# This one is for inter-directory linking
# cmake_policy(SET CMP0079 NEW)

# Setting some options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Setting some properties
# For grouping Libraries in folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# Clustering ALL_BUILD and related crap projects (within the context of IDE)
# set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "Si")
# If want to disable ZERO_CHECK
# set(CMAKE_SUPPRESS_REGENERATION true)

#----------------------------------------------------------------------------#

# Set Application project
project(${APPLICATION_NAME} VERSION 1.0)
#add_compile_definitions(KR_APPLICATION_NAME="${KARMA_GAME_NAME}")

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


# Then we specify the involved libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/softvendor/libcapng)
#add_subdirectory(src)

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

add_executable(${APPLICATION_NAME} ${CFILES} ${HEADERFILES})

if(WIN32)
    target_link_libraries(${APPLICATION_NAME} PUBLIC Ws2_32.lib)
elseif(APPLE)
    target_link_libraries(${APPLICATION_NAME} PUBLIC ${CoreServices} ${IOKit})
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${APPLICATION_NAME} PUBLIC libcapng)
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

if(UNIX AND NOT APPLE)
    add_custom_command(TARGET ${APPLICATION_NAME} POST_BUILD
	    COMMAND cd ${REPOSITORYROOT}/BiosReaderBin/
	    COMMAND mkdir temp
	    COMMAND cp BiosReader temp/BiosReader
	    COMMAND cd temp
	    #COMMAND sudo install -o root -g root -m u=rxs,g=rx,o=x -t ${REPOSITORYROOT}/BiosReaderBin/lul ${REPOSITORYROOT}/BiosReaderBin/BiosReader
	    COMMAND sudo install -o root -g root -m u=rxs,g-rx,o=x -t ${REPOSITORYROOT}/BiosReaderBin/ BiosReader
	    COMMAND rm BiosReader
	    COMMAND cd ${REPOSITORYROOT}/BiosReaderBin/
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
