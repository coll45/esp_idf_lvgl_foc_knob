file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")
file(GLOB_RECURSE DISPLAY ${CMAKE_SOURCE_DIR} "display/*.c")
file(GLOB_RECURSE FOC ${CMAKE_SOURCE_DIR} "foc/*.cpp")
idf_component_register(SRCS "main.c" ${SRC_UI} ${FOC} ${DISPLAY}
                    INCLUDE_DIRS "." "ui" "foc" "display")
