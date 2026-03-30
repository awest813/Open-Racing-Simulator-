set(LEARNING_SOURCES
    ANN.cpp
    ann_policy.cpp
    Distribution.cpp
    List.cpp
    MathFunctions.cpp
    policy.cpp
    string_utils.cpp
)

add_library(learning STATIC ${LEARNING_SOURCES})

target_include_directories(learning PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)
