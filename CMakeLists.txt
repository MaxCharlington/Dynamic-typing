cmake_minimum_required(VERSION 3.16)

project(dynamic_typing
        LANGUAGES CXX
        DESCRIPTION ""
        HOMEPAGE_URL "https://github.com/MaxCharlington/StackBasedFunction")

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME}
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/deps/CppHelpers)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

# set(CMAKE_CXX_CLANG_TIDY clang-tidy;-checks=*;-header-filter='${CMAKE_CURRENT_SOURCE_DIR}/include/*';-extra-arg=-Wno-unknown-warning-option)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (BUILD_TESTS EQUAL 1)
add_subdirectory(tests)
endif()
