cmake_minimum_required(VERSION 3.14)
project(RCanvasDemo VERSION 1.0 LANGUAGES CXX)

find_package(ROOT REQUIRED COMPONENTS RIO ROOTGpadv7 ROOTBrowserv7 ROOTNTuple ROOTNTupleUtil)

include_directories(${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(RCanvasDemo PRIVATE
        ROOT::RIO ROOT::ROOTGpadv7 ROOT::ROOTBrowserv7 ROOT::ROOTNTuple ROOT::ROOTNTupleUtil
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)