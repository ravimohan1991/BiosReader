#[[
    Abstractions and Models are NOT at WAR!
                                            - Cobwoy's Talisman
    But Abstractions don't care and Models can't understand!!
                                            - Lul, Practicality
 ]]

# Macros
# Recursive directory list generator
# Snippet gracefully taken from https://stackoverflow.com/a/53017508
MACRO(COLLECTIVEINCLUDES CurrentDir)
    FILE(GLOB_RECURSE Children LIST_DIRECTORIES true RELATIVE ${CurrentDir} ${CurrentDir}/*)
    FOREACH(Child ${Children})
        IF(IS_DIRECTORY ${CurrentDir}/${Child})
        target_include_directories(KarmaEngine
            PRIVATE
            ${CurrentDir}/${Child}
            PUBLIC
            $<BUILD_INTERFACE:${CurrentDir}/${Child}>
        )
        ENDIF()
    ENDFOREACH()
ENDMACRO()

# Introducing the same directory hierarchy in the solution projects (for MSVC and Xcode)
# Snippet gracefully taken from https://blog.audio-tk.com/2015/09/01/sorting-source-files-and-projects-in-folders-with-cmake-and-visual-studioxcode/
MACRO(SOURCE_GROUP_BY_FOLDER Target)
    SET(SOURCE_GROUP_DELIMITER "/")
    SET(Last_Dir "")
    SET(Files "")
    message(STATUS "Organizing KarmaEngine's files by same, project directory hierarchy.")
    FOREACH(File ${CPPFILES} ${HEADERFILES})
        FILE(RELATIVE_PATH Relative_File "${PROJECT_SOURCE_DIR}/${Target}" ${File})
        GET_FILENAME_COMPONENT(Dir "${Relative_File}" PATH)
        IF (NOT "${Dir}" STREQUAL "${Last_Dir}")
            IF (Files)
                SOURCE_GROUP("${Last_Dir}" FILES ${Files})
            ENDIF (Files)
            SET(Files "")
        ENDIF (NOT "${Dir}" STREQUAL "${Last_Dir}")
        SET(Files ${Files} ${File})
        SET(Last_Dir "${Dir}")
    ENDFOREACH(File)
    IF (Files)
        SOURCE_GROUP("${Last_Dir}" FILES ${Files})
    ENDIF (Files)
ENDMACRO(SOURCE_GROUP_BY_FOLDER)

# Print a list with style
MACRO(PRINT_LIST PList)
	SET(Counter 0)
	FOREACH(Elem IN LISTS ${PList})
		MATH(EXPR Counter "${Counter}+1")
		message(STATUS "#${Counter}: ${Elem}")
	ENDFOREACH(Elem)
ENDMACRO(PRINT_LIST)

# Then the checks
# find_package(Vulkan REQUIRED)
# find_package(OpenGL REQUIRED)
# find_package(Threads REQUIRED)
# message(STATUS "Checks are satisfied, we are good to proceed!")

# Some defines
# add_compile_definitions(IMGUI_IMPL_VULKAN_NO_PROTOTYPES)

# Platform specific defines
# NOTE: These defines transcend libraries
if(WIN32)
    add_compile_definitions(BR_WINDOWS_PLATFORM)
	if(MSVC)
	    # For faster multithreaded compilation
        #add_definitions(/MP)
	endif()
elseif(UNIX AND NOT APPLE)
    add_compile_definitions(BR_LINUX_PLATFORM)
elseif(APPLE)
    add_compile_definitions(BR_MAC_PLATFORM)
endif()

# Configuration specific defines/settings
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	add_compile_definitions(BR_ENABLE_ASSERTS)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(BR_DEBUG)
endif()

# Set the project name and version
# project(KarmaEngine VERSION 1.0) # Versioning shall change to something more appropriate

# Handling MSVC static class members for dynamic linkage. I know!
if(MSVC AND BUILD_SHARED_LIBS)
    if(WIN32)
        # Let Karma handle them
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
        add_compile_definitions(BR_DYNAMIC_LINK BR_BUILD_DLL)
        # Disable stupid C4251 (dll-interface related) warnings due to STL's negligence
        add_definitions(/wd4251)
    elseif(APPLE)
        # May need to find appropriate define name because Apple doesn't generate with lable SO
        add_compile_definitions(BR_DYNAMIC_LINK BR_BUILD_SO)
    endif()
endif()

# BiosReader's personal code
file(GLOB_RECURSE CFILES ${CMAKE_CURRENT_SOURCE_DIR}/private/*.c)
# Shows the headerfile directory in project
file(GLOB_RECURSE HEADERFILES ${CMAKE_CURRENT_SOURCE_DIR}/public/*.h)

# Organize the files in relevant groups
# SOURCE_GROUP_BY_FOLDER(src)

# Building the project (STATIC or DYNAMIC (SHARED))
if (BUILD_SHARED_LIBS)
    add_library(${LIBRARY_NAME} SHARED ${CFILES} ${HEADERFILES})
else()
    add_library(${LIBRARY_NAME} STATIC ${CFILES} ${HEADERFILES})
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
