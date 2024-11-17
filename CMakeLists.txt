cmake_minimum_required(VERSION 3.5)
project(Urutau_QA)

set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Usar FetchContent para baixar a biblioteca yaml-cpp
include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG        yaml-cpp-0.6.3  # Substitua pelo tag ou branch desejado
)
FetchContent_MakeAvailable(yaml-cpp)

find_package(LLVM REQUIRED CONFIG)
#find_package(Clang REQUIRED CONFIG)

message("Found LLVM ${LLVM_PACKAGE_VERSION}")
message("Using LLVMConfig.cmake in: ${LLVM_INSTALL_PREFIX}")
#message("Found Clang in ${CLANG_INSTALL_PREFIX}")

include_directories(${LLVM_INCLUDE_DIRS})
#include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
#add_definitions(${CLANG_DEFINITIONS})
link_directories("C:/Program Files/LLVM/lib")

set(CMAKE_PREFIX_PATH "C:/Qt/6.8.0/mingw_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Urutau_QA main.cpp
        DSMatrizModule/DSMatrizModule.h
        DSMatrizModule/DSMatrizModule.cpp
        DSMatrizModule/DSMatrizModuleSample.cpp
        UI/MainWindow.cpp
        UI/MainWindow.h
        UI/HomeTab.h
        UI/HomeTab.cpp
        UI/Resources/Resources.h
        ComplianceModule/ComplianceHandle.h
        ComplianceModule/ComplianceHandle.cpp
        ComplianceModule/ComplianceChecker.cpp
        ComplianceModule/ComplianceChecker.h
        UI/ComplianceGrid.cpp
        UI/ComplianceGrid.h
        SafetyAnalyzer/SafetyAnalyzer.cpp
        SafetyAnalyzer/SafetyAnalyzer.h
)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(Urutau_QA PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        ${LLVM_LIBRARIES}
        clang
        clangBasic
        clangAST
        clangFrontend
        clangTooling
        clangEdit
        clangRewrite
        clangDriver
        clangASTMatchers
        clangLex
        clangStaticAnalyzerCore
        clangStaticAnalyzerFrontend
        yaml-cpp
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
