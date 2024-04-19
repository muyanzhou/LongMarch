file(GLOB SOURCES "*.cpp" "*.h")

set(LONGMARCH_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(LONGMARCH_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)

add_library(LongMarch ${SOURCES})

add_subdirectory(grassland)

target_include_directories(LongMarch PUBLIC ${LONGMARCH_INCLUDE_DIR})

target_link_libraries(LongMarch PUBLIC Grassland)
