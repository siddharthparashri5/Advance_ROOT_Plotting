cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# ============================================================================
# Project
# ============================================================================
project(AdvancedPlotGUI
    VERSION 2.3
    DESCRIPTION "Advanced ROOT plotting GUI"
    LANGUAGES CXX
)

# ============================================================================
# C++ Standard
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Build Type
# ============================================================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ============================================================================
# Find ROOT
# ============================================================================
find_package(ROOT REQUIRED COMPONENTS
    Core RIO Tree Hist Graf Gpad Gui Rint RooFit RooFitCore
)

include(${ROOT_USE_FILE})
message(STATUS "Using ROOT ${ROOT_VERSION}")

# ============================================================================
# Include directories
# ============================================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
)

# ============================================================================
# Sources
# ============================================================================
set(LIB_SOURCES
    src/AdvancedPlotGUI.cpp
    src/ColumnSelectorDialog.cpp
    src/CSVPreviewDialog.cpp
    src/RootDataInspector.cpp
    src/ROOTFileBrowser.cpp
    src/FileHandler.cpp
    src/PlotManager.cpp
    src/ScriptEngine.cpp
    src/PlotTypes.cpp
    src/RootEntrySelector.cpp
    src/ROOTBranchSelectorDialog.cpp
)

set(CMAKE_ROOT_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)

# Headers that require dictionary
set(DICT_HEADERS
    AdvancedPlotGUI.h
    ColumnSelectorDialog.h
    CSVPreviewDialog.h
    RootDataInspector.h
    ROOTFileBrowser.h
    RootEntrySelector.h
    ROOTBranchSelectorDialog.h
)

# ============================================================================
# Generate ROOT dictionary
# ============================================================================
ROOT_GENERATE_DICTIONARY(
    AdvancedPlotGUIDict
    ${DICT_HEADERS}
    #LINKDEF include/LinkDef.h
)

# ============================================================================
# Shared Library (GUI + ROOT classes)
# ============================================================================
add_library(AdvancedPlotGUI SHARED
    ${LIB_SOURCES}
     ${CMAKE_CURRENT_BINARY_DIR}/AdvancedPlotGUIDict.cxx
)

target_link_libraries(AdvancedPlotGUI
    Core
    RIO
    Tree
    Hist
    Graf
    Gpad
    Net
    Gui
    Rint
    RooFit
    RooFitCore
    ROOTTPython
)

target_include_directories(AdvancedPlotGUI
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(AdvancedPlotGUI PROPERTIES
    OUTPUT_NAME AdvancedPlotGUI
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Optional compile flags to reduce ROOT macro warnings
target_compile_options(AdvancedPlotGUI PRIVATE -Wall -Wno-unused-variable)

# ============================================================================
# Executable (tiny launcher)
# ============================================================================
add_executable(AdvancedPlotGUIApp
    main.cpp
)

target_link_libraries(AdvancedPlotGUIApp
    AdvancedPlotGUI
)

set_target_properties(AdvancedPlotGUIApp PROPERTIES
    OUTPUT_NAME AdvancedPlotGUIApp
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    # NOTE: CMake's BUILD_RPATH/INSTALL_RPATH properties take a *CMake list*
    # (semicolon-separated), which CMake then translates into one -rpath
    # linker flag per entry. A colon-joined string works "by accident" on
    # Linux (ld's single -rpath flag already accepts a colon-separated list,
    # matching ELF DT_RUNPATH), but breaks on macOS: dyld doesn't split on
    # colons at all, so the whole colon-joined string gets treated as one
    # literal (nonexistent) path -- exactly the "Library not loaded" error
    # this caused. Semicolons here are the portable, correct choice.
    #
    # BUILD_RPATH: used when running ./AdvancedPlotGUIApp straight out of the
    # build directory (before installing).
    BUILD_RPATH "${CMAKE_BINARY_DIR};${ROOT_LIBRARY_DIR}"
    # INSTALL_RPATH: baked into the binary once it's installed via
    # `cmake --install`/`make install`, so it finds libAdvancedPlotGUI at its
    # installed location (CMAKE_INSTALL_PREFIX/lib) instead of the build
    # directory -- this is what makes a true system-wide install work even
    # after the build folder is deleted or moved.
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${ROOT_LIBRARY_DIR}"
)

# ============================================================================
# Installation
# ============================================================================
install(TARGETS AdvancedPlotGUI AdvancedPlotGUIApp
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# ROOT's dictionary generation also produces a *_rdict.pcm (and often a
# .rootmap) file next to the library -- these are needed at runtime and must
# ship alongside libAdvancedPlotGUI.so, but their exact name depends on the
# ROOT version/macro implementation and isn't known until after the build
# runs. install(CODE ...) below globs for them at *install time* (not
# configure time, since they don't exist yet when CMake first runs) and
# copies whatever it finds into the same lib/ directory.
install(CODE "
    file(GLOB _AdvancedPlotGUI_PCM_FILES
        \"${CMAKE_CURRENT_BINARY_DIR}/*.pcm\"
        \"${CMAKE_CURRENT_BINARY_DIR}/*.rootmap\")
    foreach(_f \${_AdvancedPlotGUI_PCM_FILES})
        message(STATUS \"Installing dictionary file: \${_f}\")
        file(INSTALL DESTINATION \"${CMAKE_INSTALL_PREFIX}/lib\" TYPE FILE FILES \"\${_f}\")
    endforeach()
")

# ============================================================================
# Summary
# ============================================================================
message(STATUS "")
message(STATUS "================================================")
message(STATUS " AdvancedPlotGUI (Canonical ROOT Layout)")
message(STATUS "================================================")
message(STATUS " ROOT version : ${ROOT_VERSION}")
message(STATUS " Build type   : ${CMAKE_BUILD_TYPE}")
message(STATUS " Library      : libAdvancedPlotGUI.so")
message(STATUS " Executable   : AdvancedPlotGUIApp")
message(STATUS "================================================")
message(STATUS "")
