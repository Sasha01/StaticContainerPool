CMAKE_MINIMUM_REQUIRED()

project("StaticContainerPool")



set(SRC_FILES ${SRC_FILES} main.c)
set(INCLUDE_DIRS)
add_subdirectory(SCP)
message(${INCLUDE_DIRS})
include_directories(${INCLUDE_DIRS})

add_compile_options(-Wall -Wextra -pedantic -Werror -fstack-usage -Xlinker -Map=output.map --coverage)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${CMAKE_PROJECT_NAME} gcov)

