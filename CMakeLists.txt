cmake_minimum_required(VERSION 3.0.0)
project(rocksdb VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_REQUIRED ON)

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(REQUIRES rocksdb/6.20.3@ benchmark/1.5.6@
                GENERATORS cmake cmake_paths cmake_find_package
                BUILD_TYPE Debug
                BASIC_SETUP CMAKE_TARGETS)

#find_package(rocksdb)

set(SRCFILES
#        utils.cpp
        utils.h utils.cpp)

add_executable(rocksdb main.cpp)
target_link_libraries(rocksdb CONAN_PKG::rocksdb)

add_executable(multi_process_example multi_process_example.cpp)
target_link_libraries(multi_process_example CONAN_PKG::rocksdb)

#add_executable(rocksdb-load easy-bench.cpp ${SRCFILES})
#target_link_libraries(rocksdb-load CONAN_PKG::rocksdb CONAN_PKG::benchmark)

add_executable(easy-rocks-load easy-rocks-load.cpp ${SRCFILES})
target_link_libraries(easy-rocks-load CONAN_PKG::rocksdb )

add_executable(multi-rocks-load multi-rocks-load.cpp ${SRCFILES})
target_link_libraries(multi-rocks-load CONAN_PKG::rocksdb )

include(CTest)
enable_testing()

#add_test(NAME rocksdb-load COMMAND rocksdb-load)
add_test(NAME easy-rocks-load COMMAND easy-rocks-load)
add_test(NAME multi-rocks-load COMMAND multi-rocks-load)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
