cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(rntuplebrowser)

find_package(ROOT CONFIG REQUIRED)

add_library(RNTupleBrowser SHARED RNTupleBrowser.cxx)
target_include_directories(RNTupleBrowser
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(RNTupleBrowser
        PUBLIC
        ROOT::Core
        ROOT::RIO
)

add_executable(Main Main.cxx)
target_link_libraries(Main
        RNTupleBrowser
        ROOT::Core
        ROOT::RIO
)
