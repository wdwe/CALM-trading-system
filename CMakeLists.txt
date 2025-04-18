cmake_minimum_required(VERSION 3.27)
project(calm-trader)

set(CMAKE_CXX_STANDARD 20)
#enable_testing()


# trading
add_executable(${PROJECT_NAME}
        trader.cpp
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC calm
)

target_link_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_BINARY_DIR}/lib
)

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
)

# historical data
add_executable(historian
        historian.cpp
)

target_link_libraries(historian
        PUBLIC calm
)

target_link_directories(historian
        PUBLIC ${CMAKE_BINARY_DIR}/lib
)

target_include_directories(historian
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
)



include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG 0.8.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)

# Arrow
find_package(Arrow REQUIRED)


# include/link for all targets
include_directories(/usr/lib/include)
include_directories(/usr/local/include/bsoncxx/v_noabi)
include_directories(/usr/local/include/mongocxx/v_noabi)

link_libraries(/usr/local/lib/libhiredis.a)
link_libraries(/usr/local/lib/libbsoncxx.so)
link_libraries(/usr/local/lib/libmongocxx.so)
link_directories(/usr/lib/x86_64-linux-gnu)
link_libraries(spdlog fmt yaml-cpp::yaml-cpp)
link_libraries(Arrow::arrow_shared)

# subdirectory
add_subdirectory(lib)
add_subdirectory(external)
add_subdirectory(tests)