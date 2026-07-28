# Version 2.4 — Publication-Ready Plots, Smarter CSV Handling & System-Wide Install

**Tag:** `v2.4`

---

## What's New

### 📊 Configurable Histogram Binning
No more being stuck with a fixed default bin count:

- ✅ X/Y/Z bin counts editable directly in the Column Selector dialog
- ✅ Applies to TH1, TH2, and TH3
- ✅ Enabled/disabled automatically based on histogram dimensionality

### 🔤 Publication-Style Panel Labels
One click adds journal-style labels to multi-panel figures:

- ✅ Choose a numbering style: `(a),(b),(c)…`, `(i),(ii),(iii)…`, `(1),(2),(3)…`, `A,B,C…`, or `a,b,c…`
- ✅ Choose a corner: top-left, top-right, bottom-left, bottom-right
- ✅ Applied automatically across divided canvases, separate canvases, and overlay canvases
- ✅ Drawn via `TLatex` in NDC coordinates — doesn't interfere with axis titles or legends

### 📁 Smarter CSV String-Column Handling
Columns with text (categories, sample names, labels) are now first-class citizens instead of a source of silent data corruption:

- ✅ Auto-detected and classified separately from numeric columns
- ✅ **Skip** — leave them out of plotting entirely (default, unaffected numeric plotting)
- ✅ **Use as point labels** — annotate individual `TGraph`/`TGraphErrors` points
- ✅ **Use as a categorical X-axis** — turn a TH1 into a proper bar chart of a category column, counting occurrences or summing a chosen numeric column per category

### 🔕 Popup Message Control
```
Data File Panel:
  [Browse...] [Load ROOT to GUI...] [x] Show popup messages ← NEW
```
Uncheck it to route informational dialogs (success/warning) to the console instead of a modal window — useful when iterating through a batch of plots or file loads. Yes/No confirmation dialogs always still show, since those need a real decision.

### 🖥️ System-Wide Installation (Linux + macOS)
```bash
cd build
sudo make install      # installs to /usr/local by default
AdvancedPlotGUIApp      # now runs from anywhere
```
- ✅ Correct `RPATH` on **both** the library and executable targets (previously only the executable had it, which meant `libAdvancedPlotGUI.so` could fail to find ROOT's own libraries like `libGraf.so` when launched outside an interactive shell)
- ✅ Cross-platform-correct `RPATH` separators (semicolon-joined CMake lists, so macOS's `dyld` gets one `-rpath` entry per directory instead of one broken colon-joined string)
- ✅ ROOT dictionary generation given an explicit `MODULE` name, with an install-time fallback that force-copies the `.pcm`/`.rootmap` under the canonical name regardless of what a given ROOT version's macros call it internally
- ✅ New `sudo make uninstall` target (CMake has no built-in one) that reads back the install manifest and cleans up everything, including the dictionary files

### 🗂️ Desktop / App-Drawer Integration
Installing now adds a proper application menu entry:

- ✅ `.desktop` entry generated from a template (correct install path baked in via CMake, regardless of prefix)
- ✅ App icon shipped in both the standard hicolor icon theme location and the legacy `pixmaps` fallback
- ✅ Launches through a login shell (`bash -lc`) so `~/.bashrc`/`~/.profile` are sourced first — needed for manually-built ROOT installs that rely on `thisroot.sh` for `$ROOTSYS` and friends

---

## 🐛 Bug Fixes

- **CSV loader silently corrupted data with string columns.** The old `FileHandler::LoadCSVWithSettings` tried `std::stod()` on every token and silently *skipped* any value that failed to parse — for a file with a text column, this desynced every numeric column after it, row by row, with no error. It also had a leftover debug check that printed a spurious `"No data rows found in file"` warning on the very first row processed, regardless of whether data was actually found. Now delegates to the shared `DataReader::ReadCSVFile`, which correctly classifies numeric vs. string columns and keeps everything aligned.
- **`"error while loading shared libraries: libGraf.so..."` at runtime.** Modern Linux toolchains default to `DT_RUNPATH` rather than the older `DT_RPATH`, and `RUNPATH` is *not* inherited transitively — it only helps an object resolve its own direct dependencies, not the dependencies of things that depend on it. `RPATH` was only set on the executable, so `libAdvancedPlotGUI.so` had nothing but `$LD_LIBRARY_PATH` to fall back on for finding ROOT's libraries. Fixed by setting `RPATH` on the library target too.
- **`"Library not loaded: @rpath/libAdvancedPlotGUI.dylib"` on macOS.** Caused by joining multiple rpath directories with a colon in a single CMake property value — this happens to work on Linux (where `ld`'s single `-rpath` argument already accepts a colon-separated list) but breaks on macOS, where `dyld` doesn't split on colons at all. Fixed by using a semicolon-separated CMake list instead, which CMake correctly translates into one `-rpath` linker flag per directory on each platform.
- **`"ROOT PCM ... file does not exist"` after a fresh install.** Some ROOT versions' CMake dictionary macros name the generated `.pcm`/`.rootmap` after the *dictionary generation target* rather than the library it ends up compiled into, and ROOT's runtime looks the file up by the library name. Fixed by passing an explicit `MODULE` name to `ROOT_GENERATE_DICTIONARY`, plus a defensive install-time step that force-copies the generated file under the canonical name regardless.

---

## Technical Details

### New Components
- **`PopupControl.h`** — global toggle + `ShowMsgBox(...)`, a drop-in replacement for `new TGMsgBox(...)` used at all 52 existing call sites across the codebase
- **`PlotCreator::CreateTH1Categorical(...)`** — builds a TH1 with one bin per unique string-column value, axis-labeled via `TAxis::SetBinLabel`, filled by count or by summing a chosen numeric column
- **`PlotManager::DrawPanelLabel(...)`** — draws the chosen `(a)/(i)/(1)/A/a`-style label on a pad via `TLatex`, with roman-numeral and base-26 (`a..z, aa, ab, …`) generators for large panel counts
- **`resources/AdvancedPlotGUIApp.desktop.in`** + **`resources/advancedplotgui.png`** — app-drawer integration
- **`cmake/cmake_uninstall.cmake.in`** — powers the new `uninstall` target

### `PlotConfig` additions (`PlotTypes.h`)
```cpp
int bins  = 100;   // now user-editable (previously fixed)
int binsY = 100;
int binsZ = 100;
int labelColumn         = -1;  // string column for TGraph/TGraphErrors point labels
int categoryColumn      = -1;  // string column for TH1 categorical X-axis
int categoryValueColumn = -1;  // optional numeric column to sum per category
```

### `ColumnSelectorDialog` additions
- "Histogram Binning" group: X/Y/Z bin count entries, enabled based on plot type
- "Category Axis (X)" / "Category Value (Y)" combos, TH1-only
- Fixed: the "Labels" combo was previously (incorrectly) offered for TH2/TH3 too, even though nothing in the backend consumed it there — restricted to TGraph/TGraphErrors, where it's actually used

---

## 📋 Example Use Case

```
1. Load sales.csv (columns: region [text], revenue [numeric])
2. "Add Plot" → Plot type: TH1
3. Category Axis (X) = region
4. Category Value (Y) = revenue
5. "Create Plots" → bar chart, one bin per region, summed revenue per bin

--- Meanwhile, for a multi-panel figure ---
1. Add several plots via "Add Plot"
2. Canvas Options → Divided Canvas, 2x2
3. Enable "Add panel labels" → style "(a),(b),(c)…" → Top Left
4. "Create Plots" → each panel labeled (a), (b), (c), (d) automatically
```

---

## 🔧 Installation

```bash
git clone https://github.com/siddharthparashri5/Advanced_ROOT_Plotting.git
cd Advanced_ROOT_Plotting

# Build
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Run directly from the build directory
./AdvancedPlotGUIApp

# Or install system-wide, so it's runnable from anywhere
# and shows up in your application menu:
sudo make install
AdvancedPlotGUIApp

# Uninstall later:
sudo make uninstall
```

### New Files
```
include/PopupControl.h
resources/AdvancedPlotGUIApp.desktop.in
resources/advancedplotgui.png
cmake/cmake_uninstall.cmake.in
```

### Changed Files
```
include/PlotTypes.h, src/PlotTypes.cpp                       # binning, categorical TH1, labelColumn fixes
include/ColumnSelectorDialog.h, src/ColumnSelectorDialog.cpp  # binning / category axis / label UI
include/AdvancedPlotGUI.h, src/AdvancedPlotGUI.cpp            # panel label UI, popup toggle
include/PlotManager.h, src/PlotManager.cpp                    # panel label drawing
src/FileHandler.cpp                                           # CSV loader rewrite
include/ErrorHandling.h + 7 other src/*.cpp files              # routed through PopupControl
CMakeLists.txt                                                 # RPATH fixes, dictionary MODULE,
                                                                # install/uninstall, desktop integration
```

---

## ✅ Testing Checklist

- [x] X/Y/Z bin count entries enable/disable correctly per plot type
- [x] Panel labels render correctly across divided, overlay, and separate canvas modes
- [x] Panel label numbering rolls over past 26 panels (`z` → `aa`)
- [x] CSV with a string column loads without corrupting numeric columns
- [x] String column can be skipped, used as point labels, or used as categorical X-axis
- [x] Categorical TH1 correctly sums the value column per category
- [x] Popup toggle suppresses info dialogs but never suppresses Yes/No dialogs
- [x] `sudo make install` + fresh shell → `AdvancedPlotGUIApp` runs from any directory
- [x] Installed binary still runs after deleting the build directory
- [x] App appears in the application menu/app drawer with its icon
- [x] `sudo make uninstall` removes binary, library, dictionary files, desktop entry, and icon
- [x] Verified on Ubuntu (ROOT 6.26.06) and macOS/Homebrew (ROOT 6.38.04)

---

## Related Issues & PRs
- Fixes: Histogram bin count not configurable from the GUI
- Fixes: No way to add publication-style panel labels to multi-panel figures
- Fixes: CSV files with text columns silently corrupted numeric data
- Fixes: Excessive popup dialogs when running batches of plots
- Fixes: No system-wide install / app-drawer integration
- Fixes: `libGraf.so` / `@rpath` / PCM-not-found runtime errors after installing

---

## Documentation
- Full release notes: this file (`RELEASE_NOTES_v2.4.md`)
- Updated: `README.md` — new Features, Installation, Troubleshooting, and Example 5 sections
- Usage: see `README.md` → "Creating Plots" and "Customizing Plots" sections for binning/panel-label/categorical-axis usage

---

## Special Thanks
- ROOT framework team for the underlying GUI and plotting toolkit
- Everyone who reported the CSV data-corruption and app-drawer launch issues during testing

---

**Download & Try:** [Releases](https://github.com/siddharthparashri5/Advanced_ROOT_Plotting/releases)

Happy plotting! 📊
