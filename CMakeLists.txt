# ============================================================================
# CMakeLists.txt for Advanced ROOT Plotting GUI
# Author: Siddharth Parashri
# Version: 2.3 - Cleaned up and fixed
# ============================================================================

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# ============================================================================
# Project Definition
# ============================================================================
project(AdvancedPlotGUI
    VERSION 2.0
    DESCRIPTION "Advanced ROOT plotting GUI with scripting capabilities"
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
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# ============================================================================
# Find ROOT
# ============================================================================
find_package(ROOT 6.0 REQUIRED COMPONENTS
    Core RIO Tree Hist Graf Gpad Gui RooFit RooFitCore Rint
)

if(NOT ROOT_FOUND)
    message(FATAL_ERROR "ROOT not found! Please source thisroot.sh")
endif()

message(STATUS "Found ROOT ${ROOT_VERSION} at ${ROOT_DIR}")

# ============================================================================
# Find Python3
# ============================================================================
find_package(Python3 COMPONENTS Development)
if(Python3_FOUND)
    message(STATUS "Found Python ${Python3_VERSION}")
else()
    message(WARNING "Python3 not found - Python scripting will be limited")
endif()

# ============================================================================
# Include Directories
# ============================================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
)
if(Python3_FOUND)
    include_directories(${Python3_INCLUDE_DIRS})
endif()

# ============================================================================
# Compiler Flags
# FIX 1: Removed -fno-PIC (conflicts with -no-pie and causes warnings).
#         -Wall is good; add -Wno-unused-variable to silence ROOT macro noise.
# ============================================================================
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS} -Wall -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Disable PIE for the executable (required for ROOT GUI apps on some distros)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")

# ============================================================================
# Source Files
# ============================================================================
set(SOURCES
    src/AdvancedPlotGUI.cpp
    src/ColumnSelector.cpp
    src/RootDataInspector.cpp
    src/CSVPreviewDialog.cpp
    src/DropTextEntry.cpp
    src/FileHandler.cpp
    src/PlotManager.cpp
    src/ScriptEngine.cpp
    src/PlotTypes.cpp
)

# ============================================================================
# ROOT Dictionary
#         the "Unused class rule" warning disappears.
# FIX 3: Removed PlotManager.h, ScriptEngine.h, FileHandler.h from the
#         rootcling header list — those managers don't inherit from TObject and
#         have no ClassDef, so rootcling cannot (and does not need to) generate
#         a dictionary for them. Listing them only causes "Unused class rule"
#         warnings.
# ============================================================================
set(DICT_OUTPUT  ${CMAKE_SOURCE_DIR}/build/ProjectDict.cpp)
set(DICT_PCM     ${CMAKE_BINARY_DIR}/ProjectDict_rdict.pcm)
set(DICT_ROOTMAP ${CMAKE_BINARY_DIR}/ProjectDict.rootmap)

add_custom_command(
    OUTPUT ${DICT_OUTPUT} ${DICT_PCM}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
    COMMAND rootcling -f ${DICT_OUTPUT}
                     -rmf ${DICT_ROOTMAP}
                     -rml libAdvancedPlotGUI.so
                     -I${CMAKE_SOURCE_DIR}/include
                     # Headers that contain ClassDef — rootcling ONLY needs these
                     ${CMAKE_SOURCE_DIR}/include/AdvancedPlotGUI.h
                     ${CMAKE_SOURCE_DIR}/include/ColumnSelector.h
                     ${CMAKE_SOURCE_DIR}/include/CSVPreviewDialog.h
                     ${CMAKE_SOURCE_DIR}/include/RootDataInspector.h
                     ${CMAKE_SOURCE_DIR}/include/DropTextEntry.h
                     # LinkDef MUST be last
                     ${CMAKE_SOURCE_DIR}/include/LinkDef.h
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS
        ${CMAKE_SOURCE_DIR}/include/AdvancedPlotGUI.h
        ${CMAKE_SOURCE_DIR}/include/ColumnSelector.h
        ${CMAKE_SOURCE_DIR}/include/CSVPreviewDialog.h
        ${CMAKE_SOURCE_DIR}/include/RootDataInspector.h
        ${CMAKE_SOURCE_DIR}/include/DropTextEntry.h
        ${CMAKE_SOURCE_DIR}/include/LinkDef.h
    COMMENT "Generating ROOT dictionary ProjectDict.cpp..."
    VERBATIM
)

list(APPEND SOURCES ${DICT_OUTPUT})

# ============================================================================
# Executable
# ============================================================================
add_executable(AdvancedPlotGUI ${SOURCES})

# Copy PCM to binary dir after build so ROOT can find it at runtime
add_custom_command(TARGET AdvancedPlotGUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/ProjectDict_rdict.pcm
            ${CMAKE_BINARY_DIR}/libAdvancedPlotGUI_rdict.pcm
    COMMENT "Copying PCM file for runtime..."
)

# ============================================================================
# Link Libraries
# FIX 4: Use ROOT_LIBRARIES (set by find_package) instead of manually calling
#         root-config. This is cleaner and avoids string-splitting bugs.
#         Explicitly name the components we need so the linker order is clear.
# FIX 5: Removed the duplicate target_link_libraries block that called
#         root-config a second time — it was linking everything twice.
# ============================================================================
target_link_libraries(AdvancedPlotGUI
    # ROOT components (resolved cleanly by find_package)
    ROOT::Core
    ROOT::RIO
    ROOT::Tree
    ROOT::Hist
    ROOT::Graf
    ROOT::Gpad
    ROOT::Gui
    ROOT::Rint
    ROOT::RooFit
    ROOT::RooFitCore
)

# ROOTTPython (for TPython::Exec) — optional, graceful fallback
find_library(ROOTPYTHON_LIB
    NAMES ROOTTPython PyROOT
    HINTS ${ROOT_LIBRARY_DIR}
)
if(ROOTPYTHON_LIB)
    target_link_libraries(AdvancedPlotGUI ${ROOTPYTHON_LIB})
    message(STATUS "Linking TPython library: ${ROOTPYTHON_LIB}")
else()
    message(WARNING "ROOTTPython/PyROOT not found - TPython::Exec will be unavailable")
endif()

# Python itself
if(Python3_FOUND)
    target_link_libraries(AdvancedPlotGUI ${Python3_LIBRARIES})
endif()

# ============================================================================
# RPATH
# ============================================================================
set_target_properties(AdvancedPlotGUI PROPERTIES
    INSTALL_RPATH             "${ROOT_LIBRARY_DIR}"
    BUILD_WITH_INSTALL_RPATH  FALSE
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

# ============================================================================
# Run script — generated at configure time
# ============================================================================
file(WRITE ${CMAKE_BINARY_DIR}/run.sh
"#!/bin/sh
export LD_LIBRARY_PATH=${ROOT_LIBRARY_DIR}:\${LD_LIBRARY_PATH}
exec \${0%/*}/AdvancedPlotGUI \$@
")
execute_process(COMMAND chmod +x ${CMAKE_BINARY_DIR}/run.sh)

# ============================================================================
# Installation
# ============================================================================
install(TARGETS AdvancedPlotGUI
    RUNTIME DESTINATION bin
)
install(FILES ${DICT_PCM}
    DESTINATION lib
    OPTIONAL
)

# ============================================================================
# Custom Targets (helpers)
# ============================================================================
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E env
            "LD_LIBRARY_PATH=${ROOT_LIBRARY_DIR}:$ENV{LD_LIBRARY_PATH}"
            $<TARGET_FILE:AdvancedPlotGUI>
    DEPENDS AdvancedPlotGUI
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running AdvancedPlotGUI..."
)

add_custom_target(clean-dict
    COMMAND ${CMAKE_COMMAND} -E remove ${DICT_OUTPUT} ${DICT_PCM}
            ${CMAKE_BINARY_DIR}/ProjectDict.rootmap
    COMMENT "Cleaning dictionary files..."
)

add_custom_target(stats
    COMMAND wc -l
            ${CMAKE_SOURCE_DIR}/src/*.cpp
            ${CMAKE_SOURCE_DIR}/include/*.h
    COMMENT "Project line counts"
)

add_custom_target(check-root
    COMMAND root-config --version
    COMMAND root-config --libs
    COMMAND ls -la ${ROOT_LIBRARY_DIR}/libGui*       || echo "Gui not found"
    COMMAND ls -la ${ROOT_LIBRARY_DIR}/libRooFit*    || echo "RooFit not found"
    COMMAND ls -la ${ROOT_LIBRARY_DIR}/libROOTTPython* 2>/dev/null || echo "TPython not found"
    COMMENT "Checking ROOT installation..."
)

# ============================================================================
# Configuration Summary
# ============================================================================
message(STATUS "")
message(STATUS "================================================")
message(STATUS "  Advanced Plot GUI - Configuration Summary")
message(STATUS "================================================")
message(STATUS "  Project:      ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  Build type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  ROOT version: ${ROOT_VERSION}")
message(STATUS "  ROOT lib dir: ${ROOT_LIBRARY_DIR}")
if(Python3_FOUND)
    message(STATUS "  Python:       ${Python3_VERSION}")
else()
    message(STATUS "  Python:       Not found")
endif()
message(STATUS "  Dict output:  ${DICT_OUTPUT}")
message(STATUS "================================================")
message(STATUS "")
