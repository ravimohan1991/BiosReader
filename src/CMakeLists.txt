#[[
    Abstractions and Models are NOT at WAR!
                                            - Cobwoy's Talisman
    But Abstractions don't care and Models can't understand!!
                                            - Lul, Practicality
 ]]

# Some defines
# add_compile_definitions(IMGUI_IMPL_VULKAN_NO_PROTOTYPES)

# Platform specific defines and context setup
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
    find_library(IOKit IOKit)
    find_library(CoreServices CoreServices)
endif()

# Configuration specific defines/settings
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	add_compile_definitions(BR_ENABLE_ASSERTS)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(BR_DEBUG)
endif()

# BiosReader's personal code
file(GLOB_RECURSE CFILES ${CMAKE_CURRENT_SOURCE_DIR}/private/*.c)
# Shows the headerfile directory in project
file(GLOB_RECURSE HEADERFILES ${CMAKE_CURRENT_SOURCE_DIR}/public/*.h)

# Building the project (STATIC or DYNAMIC (SHARED))
# if (BUILD_SHARED_LIBS)
#    add_library(${APPLICATION_NAME} SHARED ${CFILES} ${HEADERFILES})
#else()
#    add_library(${APPLICATION_NAME} STATIC ${CFILES} ${HEADERFILES})
#endif()

add_executable(${APPLICATION_NAME} ${CFILES} ${HEADERFILES})

if(WIN32)
    target_link_libraries(${APPLICATION_NAME} PUBLIC Ws2_32.lib)
elseif(APPLE)
    target_link_libraries(${APPLICATION_NAME} PUBLIC ${CoreServices} ${IOKit})
endif()

target_include_directories(${APPLICATION_NAME}
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/public
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

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
