cmake_minimum_required(VERSION 3.14)
project(RCanvasDemo VERSION 1.0 LANGUAGES CXX)

find_package(ROOT REQUIRED COMPONENTS RIO Gpad ROOTGpadv7 ROOTBrowserv7 ROOTNTuple ROOTNTupleUtil WebGui6 ROOTWebDisplay)

include_directories(${ROOT_INCLUDE_DIRS} inc)
link_directories(${ROOT_LIBRARY_DIRS})

set(TREEMAP_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/inc/RTreeMapPainter.hxx
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/RTreeMapBase.hxx
)
set(TREEMAP_LINKDEF  ${CMAKE_CURRENT_SOURCE_DIR}/inc/LinkDef.h)

root_generate_dictionary(
        TreeMapDict
        ${TREEMAP_HEADER}
        ${TREEMAP_LINKDEF}
        OPTIONS -I${CMAKE_CURRENT_SOURCE_DIR}/inc
)

set(SRC_FILES
        main.cpp
        src/RTreeMapPainter.cxx
        src/RTreeMapBase.cxx
        src/RTreeMapImporter.cxx
        ${CMAKE_CURRENT_BINARY_DIR}/TreeMapDict.cxx
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(RCanvasDemo PRIVATE
        ROOT::RIO ROOT::ROOTGpadv7 ROOT::Gpad ROOT::ROOTBrowserv7 ROOT::ROOTNTuple ROOT::ROOTNTupleUtil ROOT::WebGui6 ROOT::ROOTWebDisplay
)