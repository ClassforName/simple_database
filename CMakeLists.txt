cmake_minimum_required(VERSION 3.10)
SET(CMAKE_BUILD_TYPE "Debug") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

project(SimpleDB)
include_directories(${PROJECT_SOURCE_DIR})

set(DBRESOURCEFILE
    "${CMAKE_CURRENT_SOURCE_DIR}/data.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/InputBuffer.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/LinuxAPIForWindows.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/MetaCommandCompile.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/StatementCompile.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/table.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Node.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/db.c"
)

add_executable(db ${DBRESOURCEFILE})

