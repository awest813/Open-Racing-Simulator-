# CMakeLists.txt for oglgraph — Modern OpenGL 3.3 Core Profile renderer
# This module is a drop-in replacement for ssggraph.  It implements the
# same tGraphicItf interface (src/interfaces/graphic.h) using shaders,
# shadow maps, HDR + bloom post-processing, and GPU-side VBO/VAO geometry.

set(OGLGRAPH_SOURCES
    AC3DLoader.cpp
    Camera.cpp
    gl_loader.cpp
    Mesh.cpp
    OGLRenderer.cpp
    oglgraph.cpp
    Particles.cpp
    PostProcess.cpp
    Shader.cpp
    ShadowMap.cpp
    SkidMarks.cpp
    Texture.cpp
)

# ----- Library target -------------------------------------------------------
if(WIN32)
    add_library(oglgraph STATIC ${OGLGRAPH_SOURCES})
else()
    add_library(oglgraph MODULE ${OGLGRAPH_SOURCES})
endif()

# ----- Include paths --------------------------------------------------------
target_include_directories(oglgraph PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
    ${PROJECT_SOURCE_DIR}/src/libs/tgf
    ${PROJECT_SOURCE_DIR}/src/libs/tgfclient
)

if(WIN32)
    target_include_directories(oglgraph PRIVATE
        ${PROJECT_SOURCE_DIR}/src/windows/include
    )
endif()

# ----- Compiler flags -------------------------------------------------------
if(MSVC)
    # Suppress safe-string deprecation warnings (legacy sprintf/strncpy in AC3D loader)
    target_compile_options(oglgraph PRIVATE /wd4996)
    # Enable C++14 for range-for and lambdas used in OGLRenderer
    target_compile_options(oglgraph PRIVATE /std:c++14)
    # Prevent min/max macros from clashing with std::min/max
    target_compile_definitions(oglgraph PRIVATE NOMINMAX)
endif()


# ----- Link dependencies ----------------------------------------------------
if(WIN32)
    target_link_libraries(oglgraph PRIVATE
        tgf txml
        opengl32
        # Note: plib (ssg/ssgaux/sg/ul) is NOT required — oglgraph uses its
        # own AC3DLoader and does not depend on the plib scene graph.
    )

    # Windows .def file for DLL export of the entry-point symbol
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/oglgraph.def")
        set_target_properties(oglgraph PROPERTIES
            LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/oglgraph.def\""
            LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/oglgraph.def"
        )
    endif()
else()
    target_link_libraries(oglgraph PRIVATE
        tgf txml
        GL
        dl
    )
    torcs_add_plugin_module(oglgraph "modules/graphic")
endif()

# ----- Shader deployment ----------------------------------------------------
# Copy GLSL shaders to the runtime directory so the renderer can find them
# at the path returned by:  GetDataDir() + "modules/graphic/oglgraph/shaders/"

set(SHADER_SRC_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_DST_DIR  "${PROJECT_SOURCE_DIR}/runtime/modules/graphic/oglgraph/shaders")

file(GLOB SHADER_FILES "${SHADER_SRC_DIR}/*.vert" "${SHADER_SRC_DIR}/*.frag")

foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME "${SHADER}" NAME)
    configure_file("${SHADER}" "${SHADER_DST_DIR}/${SHADER_NAME}" COPYONLY)
endforeach()

# Also copy to build_x86 runtime tree (Windows dev build)
if(WIN32)
    set(SHADER_BUILD_DST "${PROJECT_SOURCE_DIR}/build_x86/runtime/modules/graphic/oglgraph/shaders")
    foreach(SHADER ${SHADER_FILES})
        get_filename_component(SHADER_NAME "${SHADER}" NAME)
        configure_file("${SHADER}" "${SHADER_BUILD_DST}/${SHADER_NAME}" COPYONLY)
    endforeach()
endif()
