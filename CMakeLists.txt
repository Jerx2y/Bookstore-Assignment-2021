cmake_minimum_required(VERSION 3.16)
project(Bookstore)

set(CMAKE_CXX_STANDARD 20)
# add_compile_options(-DLOCAL)

include_directories(
        ${PROJECT_SOURCE_DIR}/lib/
        ${PROJECT_SOURCE_DIR}/src/
        ${PROJECT_SOURCE_DIR}/src/models/
)
set(src_dir
        ${PROJECT_SOURCE_DIR}/lib/varchar.cpp
        ${PROJECT_SOURCE_DIR}/src/ull.cpp
        ${PROJECT_SOURCE_DIR}/src/utils.cpp
        ${PROJECT_SOURCE_DIR}/src/models/account.cpp
        ${PROJECT_SOURCE_DIR}/src/models/book.cpp
        ${PROJECT_SOURCE_DIR}/src/models/transaction.cpp
        ${PROJECT_SOURCE_DIR}/src/commands.cpp
        ${PROJECT_SOURCE_DIR}/src/main.cpp
        )
add_executable(code ${src_dir})

# 添加链接库
# target_link_libraries(code ${PROJECT_SOURCE_DIR}/bin/libUll.a)

# 后缀名为 .a 和 .so 的文件分别为静态链接库和动态链接库