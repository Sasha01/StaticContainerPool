CMAKE_MINIMUM_REQUIRED()

project("StaticContainerPool")



set(SRC_FILES ${SRC_FILES} main.c)
set(INCLUDE_DIRS)
add_subdirectory(SCP)
message(${INCLUDE_DIRS})
include_directories(${INCLUDE_DIRS})

add_compile_options(-Wall -Wextra -pedantic -Werror -fstack-usage -Xlinker -Map=output.map)

add_executable("StaticContainerPool" ${SRC_FILES})

