add_library(track MODULE
    track.cpp
    track3.cpp
    track4.cpp
    trackitf.cpp
    trackutil.cpp
)

target_include_directories(track PRIVATE ${CMAKE_SOURCE_DIR}/src/interfaces)
target_include_directories(track PRIVATE ${CMAKE_SOURCE_DIR}/src/modules)
target_include_directories(track PRIVATE ${CMAKE_SOURCE_DIR}/src/libs)

if(WIN32)
    target_compile_definitions(track PRIVATE WIN32 _WINDOWS)
    set_target_properties(track PROPERTIES
        LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/track.def\""
        LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/track.def"
    )
endif()
torcs_add_plugin_module(track "modules/track")

if(MSVC)
    target_compile_options(track PRIVATE /wd4996)
    target_link_options(track PRIVATE /NODEFAULTLIB:LIBCMT /NODEFAULTLIB:LIBCMTD)
endif()

if(WIN32)
    target_link_libraries(track tgf txml robottools sg ul)
else()
    target_link_libraries(track tgf txml robottools)
endif()
