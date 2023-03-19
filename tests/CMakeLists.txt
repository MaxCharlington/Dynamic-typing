link_libraries(${PROJECT_NAME})
add_compile_options(-ggdb -Wall -Wextra -Wconversion -Wno-unused-variable)

add_executable(test_blob         test_blob.cpp)
add_executable(test_compiletime  test_compiletime.cpp)
add_executable(test_js           test_js.cpp)
add_executable(test_runtime      test_runtime.cpp)
add_executable(test_type_interop test_type_interop.cpp)
