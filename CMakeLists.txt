cmake_minimum_required(VERSION 3.10.0)
project(Code VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

add_executable(Code main.cpp
    src/request.cpp
    include/request.h
    src/requestManager.cpp
    include/requestManager.h
    src/application.cpp
    include/application.h)

if(APPLE)
    execute_process(
        COMMAND brew --prefix openssl@3
        OUTPUT_VARIABLE OPENSSL_ROOT_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

target_include_directories(Code PRIVATE include)
target_include_directories(Code PRIVATE external)

target_compile_definitions(Code PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
target_link_libraries(Code PRIVATE OpenSSL::SSL OpenSSL::Crypto)

if(APPLE)
    target_link_libraries(Code PRIVATE "-framework CoreFoundation" "-framework Security")
endif()
