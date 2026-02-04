# Advanced ROOT Plotting - User Manual

**Version:** 1.0  
**Date:** February 4, 2026  
**Author:** Siddharth Parashri

---

## Table of Contents

1. [Introduction](#introduction)
2. [Quick Start Guide](#quick-start-guide)
3. [Installation](#installation)
4. [Feature Descriptions](#feature-descriptions)
5. [Example Workflows](#example-workflows)
6. [Troubleshooting](#troubleshooting)
7. [FAQ](#faq)

---

## Introduction

Advanced ROOT Plotting is a graphical user interface application built on the ROOT framework, designed to provide intuitive and powerful data visualization capabilities for scientific data analysis. Whether you're analyzing particle physics data, statistical distributions, or general scientific measurements, this tool offers comprehensive plotting and fitting features in an easy-to-use interface.

### Key Features

- **Multi-format Support**: Load data from ROOT files, CSV, and text formats
- **Versatile Plotting**: Create histograms, scatter plots, line graphs, and more
- **Advanced Fitting**: Fit data with built-in or custom functions
- **Interactive Canvas**: Zoom, pan, and interact with your plots
- **Export Capabilities**: Save plots in multiple formats (PNG, PDF, EPS, ROOT)
- **Customization**: Full control over colors, markers, styles, and layouts

---

## Quick Start Guide

### Getting Started in 5 Minutes

1. **Launch the Application**
   ```bash
   root -l AdvancedPlotGUI.C
   ```

2. **Load Your Data**
   - Click the **"Load"** button or use `File → Open`
   - Select your data file (supported: .root, .csv, .txt, .dat)
   - The file path will appear in the text entry box

3. **Create a Plot**
   - Select your desired plot type from the dropdown menu
   - Click the **"Plot"** button
   - Your data will appear on the canvas

4. **Fit Your Data** (Optional)
   - Select a fit function from the fit dropdown menu
   - Click the **"Fit"** button
   - Fit results will be displayed on the plot

5. **Export Your Plot**
   - Click the **"Export"** button
   - Choose your desired format and location
   - Click "Save"

---

## Installation

### Prerequisites

- ROOT 6.x or later ([https://root.cern.ch](https://root.cern.ch))
- C++ compiler (GCC 7+ or Clang 5+)
- CMake 3.10 or later (for building)

### Building from Source

```bash
# Clone the repository
git clone https://github.com/siddharthparashri5/Advance_ROOT_Plotting.git
cd Advance_ROOT_Plotting

# Generate ROOT dictionary
rootcling -f ColumnSelectorDict.cpp -c ColumnSelector.h ColumnSelectorLinkDef.h

# Configure and make
make

# Run the application
./AdvancedPlotGUI
```
