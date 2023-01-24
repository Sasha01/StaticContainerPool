CMAKE_MINIMUM_REQUIRED()

project("StaticContainerPool")

set(SRC_FILES ${SRC_FILES} main.c)
set(INCLUDE_DIRS)
add_subdirectory(SCP)

include_directories(${INCLUDE_DIRS})
add_executable("StaticContainerPool" ${SRC_FILES})

