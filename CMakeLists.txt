cmake_minimum_required(VERSION 3.19)
project(SpreadSheet LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

#set(sources "main.cpp" "mainwindow.cpp" "mainwindow.h" "mainwindow.ui" "spreadsheet/FormulaAST.h"
#    "spreadsheet/formula.h" "spreadsheet/common.h" "spreadsheet/sheet.h" "spreadsheet/test_runner_p.h" "spreadsheet/cell.h"
#    "spreadsheet/FormulaAST.cpp" "spreadsheet/formula.cpp" "spreadsheet/sheet.cpp" "spreadsheet/structures.cpp" "spreadsheet/cell.cpp"
#)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

#Comment it, I have env troubles with java
#set(JAVA_HOME "C:\\Program Files\\Microsoft\\jdk-25.0.2.10-hotspot\\bin\\java.exe")
#set(Java_JAVA_EXECUTABLE "C:\\Program Files\\Microsoft\\jdk-25.0.2.10-hotspot\\bin\\java.exe")

set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/antlr/antlr-4.13.2-complete.jar)
include(${CMAKE_CURRENT_SOURCE_DIR}/antlr/FindANTLR.cmake)
add_definitions(
    -DANTLR4CPP_STATIC
    -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
)
add_subdirectory(antlr4_runtime)
set(WITH_STATIC_CRT OFF CACHE BOOL "Visual C++ static CRT for ANTLR" FORCE)
antlr_target(FormulaParser antlr/Formula.g4 LEXER PARSER LISTENER)
include_directories(
    ${ANTLR4_INCLUDE_DIRS}
    ${ANTLR_FormulaParser_OUTPUT_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/antlr4_runtime/runtime/src
)

file(GLOB sources
    *.cpp
    *.h
    spreadsheet/*.cpp
    spreadsheet/*.h
    windows/*.cpp
    windows/*.h
    windows/*.ui
    features/*.h
    features/*.cpp
)

qt_add_executable(SpreadSheet
    WIN32 MACOSX_BUNDLE
    ${ANTLR_FormulaParser_CXX_OUTPUTS}
    ${sources}
)

target_link_libraries(SpreadSheet
    PRIVATE
        Qt::Core
        Qt::Widgets
        antlr4_static
)

include(GNUInstallDirs)

install(TARGETS SpreadSheet
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SpreadSheet
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
