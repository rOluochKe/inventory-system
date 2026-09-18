cmake_minimum_required(VERSION 3.15)
project(InventorySystem VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(inventory_core
    src/Inventory.cpp
    src/Persistence.cpp
    src/Report.cpp
)
target_include_directories(inventory_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(inventory_app src/main.cpp)
target_link_libraries(inventory_app PRIVATE inventory_core)

enable_testing()
add_executable(inventory_tests tests/test_inventory.cpp)
target_link_libraries(inventory_tests PRIVATE inventory_core)
add_test(NAME inventory_tests COMMAND inventory_tests)