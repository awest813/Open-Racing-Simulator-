# SOLID-2.0 collision detection library
add_library(solid STATIC
    src/BBoxTree.cpp
    src/Box.cpp
    src/C-api.cpp
    src/Complex.cpp
    src/Cone.cpp
    src/Convex.cpp
    src/Cylinder.cpp
    src/Endpoint.cpp
    src/Object.cpp
    src/Polygon.cpp
    src/Polyhedron.cpp
    src/RespTable.cpp
    src/Response.cpp
    src/Simplex.cpp
    src/Sphere.cpp
    src/Transform.cpp
)

# Public include path: resolves <SOLID/solid.h> and <3D/*.h>
target_include_directories(solid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Private include path: the src/ headers reference each other with quotes
target_include_directories(solid PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    # Suppress truncated identifier warnings from old STL usage
    target_compile_options(solid PRIVATE /wd4786 /wd4996)
endif()
