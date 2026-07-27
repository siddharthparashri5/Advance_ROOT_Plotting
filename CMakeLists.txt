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
    MODULE AdvancedPlotGUI
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
    # NOTE: modern Linux toolchains emit DT_RUNPATH by default (not the older
    # DT_RPATH), and RUNPATH is NOT transitively inherited -- it only helps
    # resolve *this library's own* direct dependencies (e.g. libGraf.so,
    # libCore.so, ...), not dependencies of things that depend on it. So
    # libAdvancedPlotGUI.so needs its own rpath pointing at ROOT's lib dir;
    # setting this only on the AdvancedPlotGUIApp executable (as before)
    # left this library relying entirely on $LD_LIBRARY_PATH being set in
    # the environment (e.g. via thisroot.sh), which isn't guaranteed when
    # launched outside an interactive shell (app drawer, login-shell, etc.).
    BUILD_RPATH "${ROOT_LIBRARY_DIR}"
    INSTALL_RPATH "${ROOT_LIBRARY_DIR}"
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
# ship alongside libAdvancedPlotGUI.so, but their exact generated name has
# proven to vary across ROOT versions/macro implementations (some honor the
# MODULE argument above and name it libAdvancedPlotGUI_rdict.pcm; others
# still name it after the dictionary target, libAdvancedPlotGUIDict_rdict.pcm).
# So: install whatever gets generated (whatever it's named), AND additionally
# force a copy under the exact canonical name ROOT's runtime looks for when
# it loads libAdvancedPlotGUI.so -- this makes the install correct either way.
install(CODE "
    file(GLOB _AdvancedPlotGUI_PCM_FILES
        \"${CMAKE_CURRENT_BINARY_DIR}/*.pcm\"
        \"${CMAKE_CURRENT_BINARY_DIR}/*.rootmap\")
    foreach(_f \${_AdvancedPlotGUI_PCM_FILES})
        message(STATUS \"Installing dictionary file: \${_f}\")
        file(INSTALL DESTINATION \"${CMAKE_INSTALL_PREFIX}/lib\" TYPE FILE FILES \"\${_f}\")
    endforeach()

    # Defensive fallback -- guarantee the canonical filenames exist too,
    # regardless of what rootcling actually called them.
    file(GLOB _AdvancedPlotGUI_ANY_PCM \"${CMAKE_CURRENT_BINARY_DIR}/*_rdict.pcm\")
    list(LENGTH _AdvancedPlotGUI_ANY_PCM _n_pcm)
    if(_n_pcm GREATER 0)
        list(GET _AdvancedPlotGUI_ANY_PCM 0 _src_pcm)
        message(STATUS \"Ensuring canonical pcm name from: \${_src_pcm}\")
        file(INSTALL DESTINATION \"${CMAKE_INSTALL_PREFIX}/lib\" TYPE FILE
             RENAME libAdvancedPlotGUI_rdict.pcm FILES \"\${_src_pcm}\")
    endif()

    file(GLOB _AdvancedPlotGUI_ANY_ROOTMAP \"${CMAKE_CURRENT_BINARY_DIR}/*.rootmap\")
    list(LENGTH _AdvancedPlotGUI_ANY_ROOTMAP _n_rootmap)
    if(_n_rootmap GREATER 0)
        list(GET _AdvancedPlotGUI_ANY_ROOTMAP 0 _src_rootmap)
        message(STATUS \"Ensuring canonical rootmap name from: \${_src_rootmap}\")
        file(INSTALL DESTINATION \"${CMAKE_INSTALL_PREFIX}/lib\" TYPE FILE
             RENAME libAdvancedPlotGUI.rootmap FILES \"\${_src_rootmap}\")
    endif()
")

# ============================================================================
# Desktop integration -- app drawer / applications menu entry + icon
# ============================================================================
# Exec= needs the real installed path, which depends on CMAKE_INSTALL_PREFIX,
# so the .desktop file is generated from a template (@CMAKE_INSTALL_PREFIX@
# substituted) rather than shipped as a static file.
configure_file(
    ${CMAKE_SOURCE_DIR}/resources/AdvancedPlotGUIApp.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/AdvancedPlotGUIApp.desktop
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/AdvancedPlotGUIApp.desktop
    DESTINATION share/applications
)

# Icon: installed both under the standard hicolor icon theme path (used by
# GNOME/KDE/XFCE icon theming + caching) and the older share/pixmaps fallback
# (still checked by essentially every desktop environment), so it shows up
# regardless of which convention the user's desktop prefers.
install(FILES ${CMAKE_SOURCE_DIR}/resources/advancedplotgui.png
    DESTINATION share/icons/hicolor/256x256/apps
)
install(FILES ${CMAKE_SOURCE_DIR}/resources/advancedplotgui.png
    DESTINATION share/pixmaps
)

# Best-effort refresh so the app shows up immediately without a logout/login.
# Both tools are optional -- silently skipped if not present (e.g. minimal
# desktop-less systems, or DESTDIR-staged packaging builds).
install(CODE "
    find_program(_UPDATE_DESKTOP_DB update-desktop-database)
    if(_UPDATE_DESKTOP_DB)
        execute_process(COMMAND \${_UPDATE_DESKTOP_DB} \"${CMAKE_INSTALL_PREFIX}/share/applications\"
                         ERROR_QUIET OUTPUT_QUIET)
    endif()
    find_program(_UPDATE_ICON_CACHE gtk-update-icon-cache)
    if(_UPDATE_ICON_CACHE)
        execute_process(COMMAND \${_UPDATE_ICON_CACHE} -f -t \"${CMAKE_INSTALL_PREFIX}/share/icons/hicolor\"
                         ERROR_QUIET OUTPUT_QUIET)
    endif()
")

# ============================================================================
# Uninstall target -- CMake has no built-in one
# ============================================================================
# Usage: sudo make uninstall  (or: sudo cmake --build . --target uninstall)
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
        COMMENT "Uninstalling AdvancedPlotGUI"
    )
endif()

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
