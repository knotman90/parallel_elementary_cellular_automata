project(ParallelCAGeneration)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

file(GLOB SRC
    "source/*.cpp"
)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
add_executable(generateCA ${SRC})
set_property(TARGET generateCA PROPERTY CXX_STANDARD 17)
#target_link_libraries(generateCA ${OPENCAL_LIBRARIES})
